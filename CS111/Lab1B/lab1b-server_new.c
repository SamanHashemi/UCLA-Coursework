#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include "zlib.h"

pid_t child_pid = 0;
int to_child_pipe[2];
int from_child_pipe[2];
int portno;
int client_socketfd;
int cflag;
z_stream to_client;
z_stream from_client;

void fatal_usage() {
    fprintf(stderr, "Usage: lab1b-server --port=portnum [--compress]\n");
    exit(1);
}

void fatal_error(char *error_msg) {
    perror(error_msg);
    exit(1);
}

void clean_exit() {
    int status;
    if (waitpid(child_pid, &status, 0) < 0)
        fatal_error("Error with system call waitpid");
    if (WIFEXITED(status))
        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
    exit(0);
}

void close_exit() {
    if (close(to_child_pipe[1]) < 0)
        fatal_error("Failed to close pipe to shell process on exit");
    if (close(client_socketfd) < 0)
        fatal_error("Failed to close socket to client on exit");
    clean_exit();
}

void handle_sig(int sig) {
    if (sig == SIGPIPE) {
        clean_exit();
    }
}

void end_streams() {
    inflateEnd(&from_client);
    deflateEnd(&to_client);
}

void config_stream(z_stream *stream) {
    stream->zalloc = Z_NULL;
    stream->zfree = Z_NULL;
    stream->opaque = Z_NULL;
}

void read_write_shell() {
    struct pollfd pollfds[2];
    // poll keyboard/terminal
    pollfds[0].fd = from_child_pipe[0];
    pollfds[0].events = POLLIN | POLLHUP | POLLERR;
    // poll shell
    pollfds[1].fd = client_socketfd;
    pollfds[1].events = POLLIN | POLLHUP | POLLERR;
    
    // read/write buffers
    const ssize_t read_buff_size = 512;
    char read_buff[read_buff_size];
    // doubled size in case of <lf> and <cr>
    char write_client[read_buff_size * 2];
    char write_shell[read_buff_size * 2];
    // assume that compression buffer is big enough
    char read_buff_decompressed[read_buff_size * 2];
    char write_client_compressed[read_buff_size * 2];
    
    // initialize compression/decompression streams
    if (cflag) {
        config_stream(&to_client);
        if (deflateInit(&to_client, Z_DEFAULT_COMPRESSION) < 0)
            fatal_error("Failed to initalize compression stream to server");
        config_stream(&from_client);
        if (inflateInit(&from_client) < 0)
            fatal_error("Failed to initalize decompression stream from server");
        atexit(end_streams);
    }
    
    // flags
    int shutdown_flag = 0;
    int kill_child_flag = 0;    // slightly grim naming
    
    while (1) {
        int poll_ret = poll(pollfds, 2, 0);
        if (poll_ret < 0)   // poll error
            fatal_error("Error polling");
        if (poll_ret == 0)  // nothing polled
            continue;
        
        // check revents and read to read_buff
        ssize_t client_readin = 0;
        ssize_t shell_readin = 0;
        if (pollfds[0].revents)
            shell_readin = read(pollfds[0].fd,
                                read_buff,
                                read_buff_size);
        if (client_readin < 0)
            fatal_error("Error reading from client");
        if (pollfds[1].revents)
            client_readin = read(pollfds[1].fd,
                                 read_buff + client_readin,
                                 read_buff_size - client_readin);
        if (shell_readin < 0)
            fatal_error("Error reading from shell pipe");
        
        // client EOF
        if (client_readin == 0 && (pollfds[1].revents & POLLIN))
            close_exit();
        
        // shell EOF
        if (shell_readin == 0 && (pollfds[0].revents & POLLIN))
            close_exit();
        
        if (cflag) {
            from_client.avail_in = (uInt) client_readin;
            from_client.next_in = (Bytef*) read_buff;
            from_client.avail_out = (uInt) read_buff_size * 2;
            from_client.next_out = (Bytef*) read_buff_decompressed;
            // inflate to decompressed buffer
            do {
                inflate(&from_client, Z_SYNC_FLUSH);      // TODO: check for error?
            } while (from_client.avail_in > 0);
            // change write_server_size to size of compressed buffer
            client_readin = (read_buff_size * 2) - from_client.avail_out;
        }
        
        // iterate through read characters and build write buffers
        ssize_t write_client_size = 0;
        ssize_t write_shell_size = 0;
        int i;
        for (i = 0; i < shell_readin + client_readin; i++) {
            // boolean representing if character came from terminal or shell
            int fromClient = i >= shell_readin;
            // single character read from buffer
            char c = (cflag && fromClient) ? read_buff_decompressed[i - shell_readin] : read_buff[i];
            switch (c) {
                case 0x03:      // ^C
                    kill_child_flag = 1;
                    break;
                case 0x04:      // ^D
                    shutdown_flag = 1;
                    break;
                default:
                    if (fromClient)
                        write_shell[write_shell_size++] = c;
                    else
                        write_client[write_client_size++] = c;
            }
        }
        // write when buffer's size is nonzero
        if (write_client_size) {
            if (cflag) {
                // configure stream
                to_client.avail_in = (uInt) write_client_size;
                to_client.next_in = (Bytef*) write_client;
                to_client.avail_out = (uInt) read_buff_size * 2;
                to_client.next_out = (Bytef*) write_client_compressed;
                // deflate to compressed buffer
                do {
                    deflate(&to_client, Z_SYNC_FLUSH);      // TODO: check for error?
                } while (to_client.avail_in > 0);
                // change write_server_size to size of compressed buffer
                write_client_size = (read_buff_size * 2) - to_client.avail_out;
            }
            if (write(client_socketfd,
                      cflag ? write_client_compressed : write_client,
                      write_client_size) < 0)
                fatal_error("Error writing to client");
        }
        if (write_shell_size)
            if (write(to_child_pipe[1], write_shell, write_shell_size) < 0)
                fatal_error("Error writing to shell process");
        
        // respond to flags
        if (kill_child_flag)
            if (kill(child_pid, SIGINT) < 0)
                fatal_error("Failed to kill child shell process");
        if (pollfds[1].revents & POLLERR || pollfds[1].revents & POLLHUP ||
            pollfds[0].revents & POLLERR || pollfds[0].revents & POLLHUP ||
            shutdown_flag)
            close_exit();
    }
}

// parses options and returns shell flag
void parse_options(int argc, char **argv) {
    int pflag = 0;
    cflag = 0;
    
    struct option longopts[3] =
    {
        {"port",        required_argument, 0, 'p'},
        {"compress",    no_argument,       0, 'c'},
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
            case 'c':                   // compress
                cflag++;
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
}

void create_shell_process() {
    if (signal(SIGPIPE, handle_sig) == SIG_ERR)
        fatal_error("Failed to create signal handler for SIGPIPE");
    
    if (pipe(to_child_pipe)   < 0 ||
        pipe(from_child_pipe) < 0 )
        fatal_error("Failed to set up pipes between processes");
    
    child_pid = fork();
    
    if (child_pid < 0) {
        fatal_error("Failed to create new process");
    } else if (child_pid == 0) {    // child process
        // close unused pipes
        if  ((close(to_child_pipe[1])   < 0) ||
             (close(from_child_pipe[0]) < 0))
            fatal_error("Failed to close unused pipes in child process");
        
        // redirect standard fileno to pipes
        if ((dup2(to_child_pipe[0],   STDIN_FILENO)  < 0) ||
            (dup2(from_child_pipe[1], STDOUT_FILENO) < 0) ||
            (dup2(from_child_pipe[1], STDERR_FILENO) < 0))
            fatal_error("File redirection failed in child process");
        
        // close original pipe fileno
        if  ((close(to_child_pipe[0])   < 0) ||
             (close(from_child_pipe[1]) < 0))
            fatal_error("Failed to close redirected pipes in child process");
        
        // start shell process
        char *execvp_filename = "/bin/bash";
        char *execvp_argv[] = {execvp_filename, NULL};
        if (execvp(execvp_filename, execvp_argv) < 0)
            fatal_error("Failed replace child process with shell process image");
    } else if (child_pid > 0) { // parent process
        // close unused pipes
        if  ((close(to_child_pipe[0])   < 0) ||
             (close(from_child_pipe[1]) < 0))
            fatal_error("Failed to close unused pipes in parent process");
    }
}

void accept_client() {
    // create an Internet domain stream socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        fatal_error("Failed to open socket");
    
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    
    // set server_address fields
    server_address.sin_family = AF_INET;
    // set server address of the host to IP address of this machine
    server_address.sin_addr.s_addr = INADDR_ANY;
    // set port number to port number converted to network byte order
    server_address.sin_port = htons(portno);
    
    if (bind(socketfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        fatal_error("Error binding to socket address");
    listen(socketfd, 1);        // only accept 1 client
    
    struct sockaddr_in client_address;
    unsigned int client_length = sizeof(client_address);
    client_socketfd = accept(socketfd, (struct sockaddr *) &client_address, &client_length);
    if (client_socketfd < 0)
        fatal_error("Error accepting client connection");
}

int main(int argc, char **argv) {
    parse_options(argc, argv);
    accept_client();
    create_shell_process();
    read_write_shell();
    return 0;
}
