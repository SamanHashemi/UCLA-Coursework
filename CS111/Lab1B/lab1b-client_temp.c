//NAME: Saman Alex Hashemipour
//EMAIL: hash.saman@gmail.com
//ID: 904903562

#define _GNU_SOURCE
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zlib.h>

const int BUFF_MAX = 256;

char newline_temp = '\n';

// Port Number
int port_num;

// Standard and log file dirs
int std_file_dir;
int log_filedir;

// Flags
int sh_flag;
int log_flag;
int comp_flag;

// File for the log
char* log_file = NULL;

// Save state for terminal
struct termios save_sate;

// Create errno
extern int errno;

// Input and output shells
z_stream in_shell;
z_stream shell_out;

void restoreInput();

// When error is encountered send message and exit
void errorExit(char *err)
{
    fprintf(stderr, err);
    exit(1);
}

// Same thing but with exit at 0
void validExit(char *err)
{
    fprintf(stderr, err);
    exit(0);
}

void setTerminal(){
    // Set info
    tcgetattr(0, &save_sate);
    atexit(restoreInput);
    
    if (!isatty(STDIN_FILENO))
        errorExit("Error: stdin not a terminal.");
    
    // Create the correct values
    struct termios corrVals;
    tcgetattr(0, &corrVals);
    
    // Set the values for the terminal
    corrVals.c_lflag = 0;
    corrVals.c_oflag = 0;
    corrVals.c_iflag = ISTRIP;
    
    // Assign the values
    if (tcsetattr(0, TCSANOW, &corrVals) < 0)
        errorExit("Error: Terminal setup");
}

// At exit for the terminal
void restoreInput(){
    tcsetattr(STDIN_FILENO, TCSANOW, &save_sate);
}

void copy(int std_file_dir) {
    // Polling
    struct pollfd poll_list[2];
    int file_dir = 0;
    poll_list[0].events = poll_list[1].events = POLLIN | POLLHUP | POLLERR | POLLRDHUP;
    poll_list[1].fd = std_file_dir;
    poll_list[0].fd = file_dir;
    
    int poll_val;
    unsigned char buffer[256];
    unsigned char comp_buffer[256];
    char cur_char;
    char crlf[2] = {13, 10};
    
    while (1) {
        // Get value from polling
        poll_val = poll(poll_list, 2, 0);
        if (poll_val < 0)
            errorExit("Error: polling error");
        
        if ((poll_list[0].revents & POLLIN)) {
            int j = 0;
            int bytes_read = read(file_dir, buffer, sizeof(char)*256);
            if (bytes_read < 0)
                errorExit("Error: keyboard read failure");
            if (comp_flag  && bytes_read > 0){
                in_shell.zalloc = Z_NULL;
                in_shell.zfree = Z_NULL;
                in_shell.opaque = Z_NULL;
                
                if (deflateInit(&in_shell, Z_DEFAULT_COMPRESSION) != Z_OK)
                    errorExit("Error: deflateInit failure");
                
                in_shell.avail_in = bytes_read;
                in_shell.next_in = buffer;
                in_shell.avail_out = BUFF_MAX;
                in_shell.next_out = comp_buffer;
                
                do{
                    if (deflate(&in_shell, Z_SYNC_FLUSH) != Z_OK)
                        errorExit("Error: deflate failure");
                } while (in_shell.avail_in > 0);
                
                write(std_file_dir, &comp_buffer, BUFF_MAX - in_shell.avail_out);
                
                if (log_flag){
                    int ender1 = BUFF_MAX - in_shell.avail_out;
                    while(j<ender1){
                        cur_char = comp_buffer[j];
                        
                        write(log_filedir, "SENT 1 byte: ", 13);
                        write(log_filedir, &cur_char, sizeof(char));
                        write(log_filedir, &newline_temp, sizeof(char));
                        j++;
                    }
                }
                deflateEnd(&in_shell);
            }
            
            int i = 0;
            while(i<bytes_read){
                cur_char = buffer[i];
                if(cur_char == '\r' || cur_char == '\n'){
                    write(1, &crlf, 2*sizeof(char));
                    if (!comp_flag){
                        write(std_file_dir, &cur_char, sizeof(char));
                        if (log_flag){
                            write(log_filedir, "SENT 1 byte: ", 13);
                            write(log_filedir, &cur_char, sizeof(char));
                            write(log_filedir, &newline_temp, sizeof(char));
                        }
                    }
                    i++;
                    continue;
                }
                else{
                    write(1, &cur_char, sizeof(char));
                    if (!comp_flag){
                        write(std_file_dir, &cur_char, sizeof(char));
                        if (log_flag){
                            write(log_filedir, "SENT 1 byte: ", 13);
                            write(log_filedir, &cur_char, sizeof(char));
                            write(log_filedir, &newline_temp, sizeof(char));
                        }
                    }
                }
                i++;
            }
            memset(buffer, 0, bytes_read);
            
            if (bytes_read < 0)
                errorExit("Error: keyboard read failure");
        }
        
        if ((poll_list[1].revents & POLLIN)) {
            int bytes_read = read(std_file_dir, buffer, sizeof(char)*256);
            
            if (bytes_read < 0)
                errorExit("Error: reading from shell");
            if (bytes_read == 0)
                exit(0);
            
            if (log_flag){
                int j = 0;
                while(j<bytes_read){
                    cur_char = buffer[j];
                    write(log_filedir, "RECEIVED 1 byte: ", 17);
                    write(log_filedir, &cur_char, sizeof(char));
                    write(log_filedir, &newline_temp, sizeof(char));
                    j++;
                }
            }
            
            if (comp_flag && bytes_read > 0){
                
                shell_out.zalloc = Z_NULL;
                shell_out.zfree = Z_NULL;
                shell_out.opaque = Z_NULL;
                
                if (inflateInit(&shell_out) != Z_OK)
                    errorExit( "Error: inflateInit failure");
                
                
                shell_out.avail_in = bytes_read;
                shell_out.avail_out = BUFF_MAX;
                shell_out.next_out = comp_buffer;
                shell_out.next_in = buffer;
                
                do{
                    if (inflate(&shell_out, Z_SYNC_FLUSH) != Z_OK)
                        errorExit("Error: inflating failure");
                } while (shell_out.avail_in > 0);
                
                inflateEnd(&shell_out);
                
                int ender = BUFF_MAX - shell_out.avail_out;
                int i = 0;
                while(i < ender){
                    cur_char = comp_buffer[i];
                    if(cur_char == '\r' || cur_char == '\n'){
                        write(1, &crlf, 2*sizeof(char));
                        i++;
                        continue;
                    }
                    else
                        write(1, &cur_char, sizeof(char));
                    i++;
                }
            }
            else{
                int i = 0;
                while(i < bytes_read){
                    cur_char = buffer[i];
                    if (log_flag){
                        write(log_filedir, "RECEIVED 1 byte: ", 17);
                        write(log_filedir, &cur_char, sizeof(char));
                        write(log_filedir, &newline_temp, sizeof(char));
                    }
                    if(cur_char == '\n' || cur_char == '\r'){
                        write(1, &crlf, 2*sizeof(char));
                        i++;
                        continue;
                    }
                    else{
                        write(1, &cur_char, sizeof(char));
                    }
                    i++;
                }
            }
            memset(comp_buffer, 0, bytes_read);
        }
    }
}

int main(int argc, char **argv) {
    int index, opt;
    
    struct hostent *server;
    struct sockaddr_in server_address;
    
    // Options when called from shell
    static struct option long_options[] = {
        {"port", required_argument, &sh_flag, 1},
        {"log", required_argument, &log_flag, 1},
        {"compress", no_argument, &comp_flag, 1}
    };
    
    // Parse the options
    while (1){
        opt = getopt_long(argc, argv, "", long_options, &index);
        if (opt != 0 && opt != -1) errorExit("Error: invalid argument was inputted");
        if (opt == -1) break;
        if (index == 0) port_num = atoi(optarg);
        if (index == 1) log_file = optarg;
    }
    
    // If the port number was not specified
    if (!sh_flag)
        errorExit("Error: port number not specified");
    
    // If we want to write to a log
    if (log_flag)
        log_filedir = open(log_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    
    // Socket stuff
    std_file_dir = socket(AF_INET, SOCK_STREAM, 0);
    if (std_file_dir < 0)
        validExit("Error: socket open failure");
    server = gethostbyname("127.0.0.1");
    if (server == NULL)
        validExit("Error: no host found");
    
    memset((char*) &server_address, 0, sizeof(server_address));
    memcpy((char*) &server_address.sin_addr.s_addr, (char*) server->h_addr, server->h_length);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_num);
    
    if (connect(std_file_dir, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        validExit("Error: connection failure");
    
    // Create the terminal
    setTerminal();
    
    // Copy the stuff
    copy(std_file_dir);
    
    exit(0);
}
