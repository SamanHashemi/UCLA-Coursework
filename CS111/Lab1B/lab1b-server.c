//NAME: Saman Alex Hashemipour
//EMAIL: hash.saman@gmail.com
//ID: 904903562

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

// Buffer Max size
const int BUFF_MAX = 256;

// Save state for terminal
struct termios save_state;

extern int errno;

// Declare flags
int sh_flag;
int comp_flag;

// Sockets file directories
int socket_acc, socket_inc;

// Create child and parent pipes respectively
int pipe_child[2];
int pipe_parent[2];

// PID
pid_t pid;

// Compression Streams
z_stream in_shell;
z_stream out_shell;

void sigHandler(int sig){
    if(sig == SIGINT)
        kill(pid, SIGINT);
    if(sig == SIGPIPE)
        exit(1);
}

// Print message and exit with error code 1
void errorExit(char *err)
{
    fprintf(stderr, err);
    exit(1);
}

// Setup the terminal
void termSetup(){
    
    dup2(pipe_child[0], 0);
    dup2(pipe_parent[1], 1);
    
    close(pipe_child[0]);
    close(pipe_parent[1]);
    close(pipe_child[1]);
    close(pipe_parent[0]);
    
    char *args[2] = {"/bin/bash", NULL };
    if (execvp("/bin/bash", args) == -1)
        errorExit("Error: Terminal Setup");
}

// Exit the server
void serverExit(){
    int i;
    if (waitpid(pid, &i, 0) == -1)
        errorExit("Error: waitpid");
    if (WIFEXITED(i)) {
        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(i), WEXITSTATUS(i));
        exit(0);
    }
}

// When closing the server
void closeServer() {
    
    close(pipe_child[1]);
    close(pipe_parent[0]);
    close(socket_acc);
    close(socket_inc);
    
    serverExit();
}

void compressAndCopy(int file_dir) {
    // Polling
    struct pollfd poll_list[2];
    poll_list[0].events = poll_list[1].events = POLLIN | POLLHUP | POLLERR;
    poll_list[1].fd = pipe_parent[0];
    poll_list[0].fd = file_dir;
    
    int poll_val;
    unsigned char buffer[BUFF_MAX];
    unsigned char comp_buffer[BUFF_MAX];
    char cur_char;
    
    while (1) {
        int state;
        if (waitpid(pid, &state, WNOHANG) == -1) {
            char bull = 'F';
            write(1, &bull, 1);
            errorExit("Error: waitpid");
        }
        
        poll_val = poll(poll_list, 2, 0);
        if (poll_val < 0)
            errorExit("Error: polling failure");
        
        if (poll_list[0].revents & POLLIN) {
            int rbSize = read(file_dir, buffer, sizeof(char)*256);
            if (rbSize < 0)
                errorExit("Error: keyboard read failure");
            
            if (comp_flag  && rbSize > 0){
                
                out_shell.zalloc = Z_NULL;
                out_shell.zfree = Z_NULL;
                out_shell.opaque = Z_NULL;
                
                if (inflateInit(&out_shell) != Z_OK)
                    errorExit("Error: inflateInit failure");
                
                out_shell.avail_in = rbSize;
                out_shell.next_in = buffer;
                out_shell.avail_out = BUFF_MAX;
                out_shell.next_out = comp_buffer;
                
                do{
                    if (inflate(&out_shell, Z_SYNC_FLUSH) != Z_OK)
                        errorExit("Error: inflating failure");
                } while (out_shell.avail_in > 0);
                
                inflateEnd(&out_shell);
                
                int ender = BUFF_MAX - out_shell.avail_out;
                int i = 0;
                while(i<ender){
                    cur_char = comp_buffer[i];
                    if(cur_char == '\4' || cur_char == '\3')
                        closeServer(1);
                    else if(cur_char == '\r' || cur_char == '\n'){
                        char c = '\n';
                        write(pipe_child[1], &c, sizeof(char));
                        i++;
                        continue;
                    }
                    else
                        write(pipe_child[1], &cur_char, sizeof(char));
                    i++;
                }
                memset(comp_buffer, 0, ender);
                
            }
            else{
                int i = 0;
                while(i<rbSize){
                    cur_char = buffer[i];
                    if(cur_char == '\4')
                        close(pipe_child[1]);
                    if(cur_char == '\3')
                        kill(pid, SIGINT);
                    else if(cur_char == '\r' || cur_char == '\n'){
                        char c = '\n';
                        write(pipe_child[1], &c, sizeof(char));
                        i++;
                        continue;
                    }
                    else
                        write(pipe_child[1], &buffer, sizeof(char));
                    i++;
                }
                memset(buffer, 0, rbSize);
                
                if (rbSize < 0)
                    errorExit("Error: read from keyboard failed\n");
            }
        }
        
        if ((poll_list[1].revents & POLLIN)) {
            int rbSize = read(pipe_parent[0],buffer, sizeof(char)*256);
            if (rbSize < 0)
                errorExit("Error: reading from shell using poll");
            if (comp_flag  && rbSize > 0){
                
                // Compression here
                in_shell.zalloc = Z_NULL;
                in_shell.zfree = Z_NULL;
                in_shell.opaque = Z_NULL;
                
                if (deflateInit(&in_shell, Z_DEFAULT_COMPRESSION) != Z_OK)
                    errorExit("Error: deflateInit failure");
                
                
                in_shell.avail_in = rbSize;
                in_shell.next_in = buffer;
                in_shell.avail_out = BUFF_MAX;
                in_shell.next_out = comp_buffer;
                
                
                do{
                    if (deflate(&in_shell, Z_SYNC_FLUSH) != Z_OK)
                        errorExit("Error: deflating error");
                } while (in_shell.avail_in > 0);
                
                if(write(file_dir, &comp_buffer, BUFF_MAX - in_shell.avail_out) == -1)
                    errorExit("Error: wiriting to compression buffer");
                
                deflateEnd(&in_shell);
                
            }
            else{
                int i = 0;
                while(i < rbSize){
                    cur_char = buffer[i];
                    write(file_dir, &cur_char, sizeof(char));
                    i++;
                }
                memset(buffer, 0, rbSize);
            }
            if ((poll_list[1].revents & (POLLHUP | POLLERR)))
                errorExit("Error: POLLHUP|POLLERR error");
        }
    }
}

int main(int argc, char **argv) {
    
    int port_num, index, opt;
    socklen_t  client_length;
    struct sockaddr_in server_address, client_address;
    
    atexit(closeServer);
    signal(SIGINT, sigHandler);
    signal(SIGPIPE, sigHandler);
    
    static struct option long_options[] = {
        {"port",        required_argument, &sh_flag,    1},
        {"compress",    no_argument, &comp_flag,    1},
        {0, 0, 0, 0}
    };
    
    while (1){
        opt = getopt_long(argc, argv, "", long_options, &index);
        if (opt == -1) break;
        else if (opt != 0) errorExit("Error: invalid argument");
        if (index == 0) port_num = atoi(optarg);
    }
    
    socket_acc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_acc < 0)
        errorExit("Error: socket open failure");
    memset((char*) &server_address, 0, sizeof(server_address));
    
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_num);
    server_address.sin_family = AF_INET;
    
    
    if (bind(socket_acc, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        errorExit("Error: socket binding failure");
    
    
    listen(socket_acc, 5);
    client_length = sizeof(client_address);
    
    
    socket_inc = accept(socket_acc, (struct sockaddr *) &client_address, &client_length);
    if (socket_inc < 0)
        errorExit("Error: client accepting failed");
    
    
    if (pipe(pipe_child) == -1)
        errorExit("Error: Creating child pipe");
    if (pipe(pipe_parent) == -1)
        errorExit("Error: Creating parent pipe");
    
    pid = fork();
    if (pid < 0)
        errorExit("Error: PID forking error");
    else if (pid == 0){
        termSetup();
    }
    else{
        close(pipe_child[0]);
        close(pipe_parent[1]);
        compressAndCopy(socket_inc);
    }
    
    exit(0);
}
