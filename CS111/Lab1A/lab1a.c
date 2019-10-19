// NAME: Saman Hashemipour
// EMAIL: hash.saman@gmail.com
// ID: 904903562

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

int BUFF_MAX = 256;

int sh_flag = 0;

int pipe_child[2];
int pipe_parent[2];

struct termios save_state;

pid_t pid;

void error_exit(char *err)
{
    fprintf(stderr, err);
    exit(1);
}

void terminalSave()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &save_state);
    if (sh_flag) {
        int i;
        if (waitpid(pid, &i, 0) == -1) { // Wait pid error
            error_exit("Error: Wait on pid error");
        }
        if (WIFEXITED(i)) { // Exit error
            fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(i), WEXITSTATUS(i));
            exit(0);
        }
    }
}

void terminalSetup()
{
    tcgetattr(0, &save_state);
    atexit(terminalSave);
    
    struct termios corrVals = save_state;
    
    // From the spec
    corrVals.c_iflag = ISTRIP;
    corrVals.c_oflag = 0;
    corrVals.c_lflag  = 0;
    
    // Create terminal window with correct values
    if(tcsetattr(0, TCSANOW, &corrVals) < 0)
        error_exit("Error: Terminal mode setup error");
}

void parentProc()
{
    // Close pipes
    close(pipe_child[0]);
    close(pipe_parent[1]);
    
    // Set up polling
    struct pollfd pollfds[2];
    pollfds[0].events = pollfds[1].events = POLLIN | POLLHUP | POLLERR;
    pollfds[1].fd = pipe_parent[0];
    pollfds[0].fd = STDIN_FILENO;
    
    // Set up the input for possible commands
    char buffer[BUFF_MAX];
    char carr_line[2] = {13, 10};
    char nl = '\n';
    
    while (1) {
        // Set up poll
        int poll_val = poll(pollfds, 2, 0);
        if (poll_val < 0)
            error_exit("Error: polling error, please check usage of poll");
        if(poll_val == 0) continue;
        
        // Read from keyboard (using poll)
        if ((pollfds[0].revents & POLLIN)) { // Check if poll in is in use
            int rbSize = read(0,buffer, sizeof(char)*BUFF_MAX); // Read from keyboard input to buffer
            if (rbSize < 0) // Check for errors in read
                error_exit("Error: unable to read keyboard input");
            
            int i = 0;
            for (; i< rbSize ;i++){ // Go through the buffer
                if(buffer[i] == 4) close(pipe_child[1]); // ^D
                else if(buffer[i] == 3) kill(pid, SIGINT); // ^C
                else if(buffer[i] == 13 || buffer[i] == 10) // Carriage Return (13) and Line Feed (10)
                {
                    write(1, &carr_line, 2*sizeof(char)); // Write carriage ret and line feed
                    write(pipe_child[1], &nl, sizeof(char)); // Write the shell with a new line
                    continue;
                }
                else
                {
                    write(1, &buffer[i], sizeof(char)); // Write the char normally to stdout
                    write(pipe_child[1], &buffer, sizeof(char)); // Write the buffer to the shell
                }
            }
            // memset(buffer, 0, rbSize); possibly uncomment
            
            if (rbSize < 0)
                error_exit("Error: unable to read keyboard input");
        }
        
        // Read from shell (using poll)
        if ((pollfds[1].revents & POLLIN)) {
            int rbSize = read(pipe_parent[0], buffer, sizeof(char)*BUFF_MAX);
            if (rbSize < 0)
                error_exit("Error: unable to read shell value");
            
            int i = 0;
            for (; i < rbSize; i++){
                if(buffer[i] == 10) write(1, &carr_line, 2*sizeof(char)); // Write carrage and line to stdout
                else write(1, &buffer[i], sizeof(char)); // Write normally
            }
            memset(buffer, 0, rbSize);
        }
        
        // Look this up
        if ((pollfds[1].revents == POLLHUP) || (pollfds[1].revents ==  POLLERR) ) {
            exit(0);
        }
    }
}

void childProc()
{
    // Duplicate
    dup2(pipe_child[0], 0);
    dup2(pipe_parent[1], 1);
    
    // Close
    close(pipe_child[1]);
    close(pipe_parent[0]);
    close(pipe_child[0]);
    close(pipe_parent[1]);
    
    // Open from /bin/bash
    char *args[2] = {"/bin/bash", NULL};
    if(execvp("/bin/bash", args))
        error_exit("Error: executing bash");
}


int main(int argc, char **argv)
{
    // Create argument option struct
    static struct option long_options[] = {
        {"shell", no_argument, 0, 's'},
        {0,       0,           0, 0}
    };
    
    // Get the arguments
    int c;
    while (1){
        c = getopt_long(argc, argv, "", long_options, NULL);
        if (c == -1) break;
        switch(c){
            case 's':
                sh_flag = 1;
                break;
            default:
                error_exit("Error: Argument is not accepted. Please try a different argument");
        }
    }
    
    terminalSetup();
    
    // Set up pipes
    if(pipe(pipe_child) == -1)
        error_exit("Error: child pipe creation error");
    if(pipe(pipe_parent) == -1)
        error_exit("Error: parent pipe creation error");
    
    if (sh_flag){
        pid = fork(); // Run fork and save value in PID
        if (pid < -1) // PID Error
            error_exit("Error: pid value was less than -1");
        else if (pid == 0) // PID running child
            childProc();
        else // PID running parent
            parentProc();
    }
    
    // Last copy
    char buffer[BUFF_MAX];
    int rbSize = read(0, buffer, sizeof(char)*BUFF_MAX);
    char carr_line[2] = {13, 10};
    while (rbSize){
        int i = 0;
        for (; i<rbSize; i++){
            if(buffer[i] == 4)
                exit(0);
            if(buffer[i] == 13 || buffer[i] == 10)
                write(1, &carr_line, 2*sizeof(char));
            else
                write(1, &buffer[i] , sizeof(char));
        }
        // Clean buffer
        memset(buffer, 0, rbSize);
        rbSize = read(0, buffer, sizeof(char)*BUFF_MAX);
    }
    
    // Success!
    exit(0);
}
