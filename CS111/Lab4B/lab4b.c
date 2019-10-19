//NAME: Saman Hashemipour
//EMAIL: hash.saman@gmail.com
//ID: 904903562

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <poll.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <mraa.h>
#include <math.h>


mraa_aio_context temp_sensor;
mraa_gpio_context button;

int log_filedir;
int start_enabled = 1;
int shutdown_enabled = 0;
int period = 1;
char scale = 'F';

void errorExit(char* error) {
    fprintf(stderr, error);
    exit(1);
}

void buttonClick() {
    shutdown_enabled = 1;
}

int areEqual(char* str1, char* str2){
    return strcmp(str1, str2) == 0;
}

void commandHandler(char* command) {
    if(areEqual(command, "SCALE=")){
        if (command[6] == 'C')
            scale = 'C';
        else if (command[6] == 'F')
            scale = 'F';
        else
            errorExit("Error: Invalid command from STDIN, SCALE=[C,F]");
    }
    else if(areEqual(command, "PERIOD=")){
        period = (int)atoi(command+7);
    }

    else if (areEqual(command,"START")) {
        start_enabled = 1;
    }
    else if (areEqual(command,"STOP")) {
        start_enabled = 0;
    }
    else if (areEqual(command,"OFF")) {
        shutdown_enabled = 1;
    }
    else if(areEqual(command, "LOG")){
        errorExit("Error: Invalid command received in STDIN, LOG");
    }
    
    if (log_filedir)
        dprintf(log_filedir, "%s\n", command);
}

double getTemp(){
    int temp_val = mraa_aio_read(temp_sensor);
    int B = 4275;
    double temp = (1023.0 / ((double)temp_val) - 1.0) * 100000.0;
    double cel = 1.0 / (log(temp/100000.0)/B + 1/298.15) - 273.15;
    double far = cel * 9/5 + 32;
    if(scale == 'F'){
        return far;
    }
    else
        return cel;
}

void writeLog(){
    struct timeval clock;
    struct tm *cur_time;
    float cur_temp = getTemp();
    int temp_dec = cur_temp * 10;
    
    cur_time = localtime(&(clock.tv_sec));
    if (log_filedir)
        dprintf(log_filedir, "%02d:%02d:%02d %d.%1d\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec, temp_dec/10, temp_dec%10);
    fprintf(stdout, "%02d:%02d:%02d %d.%1d\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec, temp_dec/10, temp_dec%10);

}

int main(int argc, char **argv) {
    struct timeval clock;
    struct tm *cur_time;
    
    struct pollfd poll_stdin = {0, POLLIN, 0};
    
    char buf[1024];
    char cache[1024];
    
    int c;
    
    struct option long_options[] = {
        {"period", required_argument, NULL, 'p'},
        {"scale", required_argument, NULL, 's'},
        {"log", required_argument, NULL, 'l'},
        {0,0,0,0}
    };
    
    
    while(1){
        c = getopt_long(argc, argv, "p:s:l:", long_options, NULL);
        if(c == -1){
            break;
        }
        else if(c == 'p'){
            period = (int)atoi(optarg);
        }
        else if(c == 's'){
            if (*optarg == 'F' || *optarg == 'C') {
                scale = *optarg;
            } else {
                errorExit("usage: lab4b [--period=#] [--scale=[CF]] [--log=FILENAME]\n");
            }
        }
        else if(c == 'l'){
            log_filedir = creat(optarg, 0666);
            if (log_filedir < 0)errorExit("creat()");
        }
        else{
            errorExit("usage: lab4b [--period=#] [--scale=[CF]] [--log=FILENAME]\n");
        }
        
    }
    
    time_t next_sample = 0;
    
    temp_sensor = mraa_aio_init(1);
    button = mraa_gpio_init(60);
    mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &buttonClick, NULL);
    
    memset(buf, 0, 1024);
    memset(cache, 0, 1024);
    
    int index = 0;
    
    while (!shutdown_enabled) {
        
        if (gettimeofday(&clock, 0))
            errorExit("Error: gettimeofday()");
        if (start_enabled && clock.tv_sec >= next_sample) {
            writeLog();
            next_sample = clock.tv_sec + period;
        }
        
        int ret = poll(&poll_stdin, 1, 0);
        if (ret < 0) {
            errorExit("Error: poll()");
        }
        if (ret > 0) {
            if (poll_stdin.revents & POLLIN) {
                int num_read = read(STDIN_FILENO, buf, 1024);
                if (num_read < 0) {
                    errorExit("Error: read()");
                }
                
                int i = 0;
                while(i < num_read && index < 1024){
                    if (buf[i] == '\n') {
                        index = 0;
                        commandHandler((char*)&cache);
                        memset(cache, 0, 1024);
                    } else {
                        cache[index] = buf[i];
                        index++;
                    }
                    i++;
                }
            }
        }
    }
    
    cur_time = localtime(&(clock.tv_sec));
    if (log_filedir)
        dprintf(log_filedir, "%02d:%02d:%02d SHUTDOWN\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec);
    fprintf(stdout, "%02d:%02d:%02d SHUTDOWN\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec);
    
    mraa_aio_close(temp_sensor);
    mraa_gpio_close(button);
    exit(0);
}
