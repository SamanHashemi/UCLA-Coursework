#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <poll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include "zlib.h"

struct termios old_terminal_mode;

char *host_name;
int portno;
int socketfd;
z_stream to_server;
z_stream from_server;

// flag for --compression option
int cflag;
// flag for --log option
int lflag;
int logfd;

void fatal_usage() {
    fprintf(stderr, "Usage: lab1b-client --port=portnum [--host=name] [--log=filename] [--compress]\n");
    exit(1);
}

void fatal_error(char *error_msg) {
    perror(error_msg);
    exit(1);
}

void restore_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal_mode);
}

void set_terminal_mode() {
    // save old terminal state
    tcgetattr(STDIN_FILENO, &old_terminal_mode);
    atexit(restore_terminal_mode);
    
    // create new terminal state
    struct termios new_terminal_mode = old_terminal_mode;
    new_terminal_mode.c_iflag = ISTRIP;    /* only lower 7 bits    */
    new_terminal_mode.c_oflag = 0;         /* no processing        */
    new_terminal_mode.c_lflag = 0;         /* no processing        */
    
    // set new terminal state
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal_mode) < 0)
        fatal_error("Failed to set parameters associated with the terminal");
}

void write_logfile(char *type, ssize_t numbytes, char *bytes) {
    // add null terminator
    bytes[numbytes] = 0;
    if (dprintf(logfd, "%s %li bytes: %s\n", type, numbytes, bytes) < 0)
        fatal_error("Error writing to log");
}

void end_streams() {
    inflateEnd(&from_server);
    deflateEnd(&to_server);
}

void config_stream(z_stream *stream) {
    stream->zalloc = Z_NULL;
    stream->zfree = Z_NULL;
    stream->opaque = Z_NULL;
}

void read_write_server() {
    struct pollfd pollfds[2];
    // poll keyboard/terminal
    pollfds[0].fd = STDIN_FILENO;
    pollfds[0].events = POLLIN | POLLHUP | POLLERR;
    // poll server
    pollfds[1].fd = socketfd;
    pollfds[1].events = POLLIN | POLLHUP | POLLERR;
    
    // read/write buffers
    const ssize_t read_buff_size = 512;
    char read_buff[read_buff_size];
    // doubled size in case of <lf> and <cr>
    char write_terminal[read_buff_size * 2];
    char write_server[read_buff_size * 2];
    // assume that compression buffer is big enough
    char read_buff_decompressed[read_buff_size * 2];
    char write_server_compressed[read_buff_size * 2];
    
    // initialize streams
    if (cflag) {
        config_stream(&to_server);
        if (deflateInit(&to_server, Z_DEFAULT_COMPRESSION) < 0)
            fatal_error("Failed to initalize compression stream to server");
        config_stream(&from_server);
        if (inflateInit(&from_server) < 0)
            fatal_error("Failed to initalize decompression stream from server");
        atexit(end_streams);
    }
    
    while (1) {
        int poll_ret = poll(pollfds, 2, 0);
        if (poll_ret < 0)   // poll error
            fatal_error("Error polling");
        if (poll_ret == 0)  // nothing polled
            continue;
        
        // check revents and read to read_buff
        ssize_t terminal_readin = 0;
        ssize_t server_readin = 0;
        if (pollfds[0].revents & POLLIN)
            terminal_readin = read(pollfds[0].fd,
                                   read_buff,
                                   read_buff_size);
        if (terminal_readin < 0)
            fatal_error("Error reading from terminal");
        if (pollfds[1].revents & POLLIN)
            server_readin = read(pollfds[1].fd,
                                 read_buff + terminal_readin,
                                 read_buff_size - terminal_readin - 1);
        if (server_readin < 0)
            fatal_error("Error reading from server");
        // Server EOF
        if (server_readin == 0 && (pollfds[1].revents & POLLIN))
            exit(0);
        if (server_readin && lflag)
            write_logfile("RECEIVED", server_readin, read_buff + terminal_readin);
        if (cflag) {
            from_server.avail_in = (uInt) server_readin;
            from_server.next_in = (Bytef*) read_buff;
            from_server.avail_out = (uInt) read_buff_size * 2;
            from_server.next_out = (Bytef*) read_buff_decompressed;
            // inflate to decompressed buffer
            do {
                inflate(&from_server, Z_SYNC_FLUSH);      // TODO: check for error?
            } while (from_server.avail_in > 0);
            // change write_server_size to size of compressed buffer
            server_readin = (read_buff_size * 2) - from_server.avail_out;
        }
        
        // iterate through read characters and build write buffers
        ssize_t write_terminal_size = 0;
        ssize_t write_server_size = 0;
        int i;
        for (i = 0; i < server_readin + terminal_readin; i++) {
            // boolean representing if character came from terminal or shell
            int fromTerminal = i < terminal_readin;
            // single character read from buffer
            // use read_buff unless reading from compressed server
            char c = (cflag && !fromTerminal) ? read_buff_decompressed[i - terminal_readin] : read_buff[i];
            switch (c) {
                case '\n':
                    if (!fromTerminal) {
                        write_terminal[write_terminal_size++] = '\r';
                        write_terminal[write_terminal_size++] = '\n';
                        break;
                    }
                    /* FALLTHRU */
                case '\r':
                    if (fromTerminal) {
                        write_server[write_server_size++] = '\n';
                        write_terminal[write_terminal_size++] = '\r';
                        write_terminal[write_terminal_size++] = '\n';
                    }
                    break;
                case 0x03:      // ^C
                    write_terminal[write_terminal_size++] = '^';
                    write_terminal[write_terminal_size++] = 'C';
                    write_server[write_server_size++] = c;
                    break;
                case 0x04:      // ^D
                    write_terminal[write_terminal_size++] = '^';
                    write_terminal[write_terminal_size++] = 'D';
                    write_server[write_server_size++] = c;
                    break;
                default:
                    if (fromTerminal) {
                        write_server[write_server_size++] = c;
                        write_terminal[write_terminal_size++] = c;
                    } else {
                        write_terminal[write_terminal_size++] = c;
                    }
            }
        }
        // write when buffer's size is nonzero
        if (write_terminal_size)
            if (write(STDOUT_FILENO, write_terminal, write_terminal_size) < 0)
                fatal_error("Error writing to terminal");
        if (write_server_size) {
            if (cflag) {
                // configure stream
                to_server.avail_in = (uInt) write_server_size;
                to_server.next_in = (Bytef*) write_server;
                to_server.avail_out = (uInt) read_buff_size * 2;
                to_server.next_out = (Bytef*) write_server_compressed;
                // deflate to compressed buffer
                do {
                    deflate(&to_server, Z_SYNC_FLUSH);      // TODO: check for error?
                } while (to_server.avail_in > 0);
                // change write_server_size to size of compressed buffer
                write_server_size = (read_buff_size * 2) - to_server.avail_out;
            }
            if (write(socketfd,
                      cflag ? write_server_compressed : write_server,
                      write_server_size) < 0)
                fatal_error("Error writing to server");
            if (lflag)
                write_logfile("SENT",
                              write_server_size,
                              cflag ? write_server_compressed : write_server);
        }
        // exit if connection is terminated by the server
        if (pollfds[1].revents & POLLERR || pollfds[1].revents & POLLHUP)
            exit(0);
    }
}

// parses options
void parse_options(int argc, char **argv) {
    
    int pflag = 0;
    lflag = 0;
    cflag = 0;
    int hflag = 0;
    
    char *logfile;
    
    struct option longopts[5] =
    {
        {"port",        required_argument, 0, 'p'},
        {"log",         required_argument, 0, 'l'},
        {"compress",    no_argument,       0, 'c'},
        {"host",        required_argument, 0, 'h'},
        {0,             0,                 0,  0 }
    };
    while (1) {
        int option = getopt_long(argc, argv, "", longopts, NULL);
        if (option == -1)               // end of options
            break;
        switch (option) {
            case 0:                     // getopt already set flag
                break;
            case 'p':                   // port
                pflag++;
                portno = atoi(optarg);
                break;
            case 'l':                   // log
                lflag++;
                logfile = optarg;
                break;
            case 'c':                   // compress
                cflag++;
                break;
            case 'h':                   // host
                hflag++;
                host_name = optarg;
                break;
            case '?':                   // invalid option
                /* FALLTHRU */
            default:
                fprintf(stderr, "Error: unknown option\n");
                fatal_usage();
        }
    }
    if (!pflag)
        fatal_usage();
    if (!hflag)
        host_name = "localhost";
    if (lflag) {
        logfd = creat(logfile, S_IRWXU);
        if (logfd < 0)
            fatal_error("Failed to open or create logfile");
    }
}

void connect_to_server() {
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        fatal_error("Failed to open socket");
    
    struct hostent *server = gethostbyname(host_name);
    if (server == NULL)
        fatal_error("Failed to get host by name");
    
    struct sockaddr_in server_address;
    memset((char *) &server_address, 0, sizeof(server_address));
    
    // set server_address fields
    server_address.sin_family = AF_INET;
    // copy server address string to server_address
    memmove(&server_address.sin_addr.s_addr,
            server->h_addr,
            server->h_length);
    // set port number to port number converted to network byte order
    server_address.sin_port = htons(portno);
    
    if (connect(socketfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        fatal_error("Error connecting client to server");
}

int main(int argc, char **argv) {
    parse_options(argc, argv);
    set_terminal_mode();
    connect_to_server();
    read_write_server();
    return 0;
}
