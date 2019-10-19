//NAME: Saman Hashemipour
//EMAIL: hash.saman@gmail.com
//ID: 904903562

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <math.h>
#include <mraa.h>

mraa_aio_context temp_sensor;
mraa_gpio_context button;

int log_filedir;
int start_enabled = 1;
int period = 1;
int shutdown_enabled = 0;
int tls = 0;
char scale = 'F';
char* id = NULL;
char* host = NULL;
char ssl_buf[1024];


void errorExit(char* msg){
    fprintf(stderr, msg);
    exit(1);
}

void invalidExit(){
    fprintf(stderr, "Error: command entered not valid");
    exit(2);
}

void usageError(char* proc){
    fprintf(stderr, "usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", proc);
    exit(1);
}

int areEqual(char* str1, char* str2){
    return strcmp(str1, str2) == 0;
}

int areEqualLen(char* str1, char* str2, int size){
    return strncmp(str1, str2, size) == 0;
}

void commandHandler(char* cmd) {
    if (areEqualLen(cmd, "SCALE=", 6*sizeof(char))) {
        if (cmd[6] == 'F')
            scale = 'F';
        else if (cmd[6] == 'C')
            scale = 'C';
        else
            invalidExit();
        
    }
    else if (areEqualLen(cmd, "PERIOD=", 7*sizeof(char))) {
        period = (int)atoi(cmd+7);
        
    }
    else if (areEqual(cmd, "STOP")) {
        start_enabled = 0;
        
    }
    else if (areEqual(cmd, "START")) {
        start_enabled = 1;
        
    }
    else if (areEqual(cmd, "OFF")) {
        shutdown_enabled = 1;
        
    }
    else if (!(areEqualLen(cmd, "LOG", 3*sizeof(char)))){
        invalidExit();
    }
    
    dprintf(log_filedir, "%s\n", cmd);
}

double getTemp() {
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

int socketHandler(int port_num) {
    
    int socket_filedir = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    struct hostent* server;

    if (socket_filedir < 0) {
        errorExit("Error: socket()");
    }
    
    server = gethostbyname(host);
    if (server == NULL) {
        errorExit("Error: gethostbyname()");
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    
    if (connect(socket_filedir, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        errorExit("Error: connect()");
    }
    
    return socket_filedir;
}

SSL* SSLHandler(int socket_filedir) {
    SSL* ssl_client = 0;
    if (SSL_library_init() < 0)
        errorExit("Error: SSL_library_init()");
    
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    SSL_CTX* context = SSL_CTX_new(TLSv1_client_method());
    if (context == NULL)
        errorExit("Error: SSL_CTX_new()");
    
    ssl_client = SSL_new(context);
    if (ssl_client == NULL)
        errorExit("Error: SSL_new()");
    if (!SSL_set_fd(ssl_client, socket_filedir))
        errorExit("Error: SSL_set_fd()");
    if (SSL_connect(ssl_client) != 1)
        errorExit("Error: SSL_connect()");
    
    return ssl_client;
}

int main(int argc, char **argv) {
    
    int c;
    int port_num = 0;
    struct timeval clk;
    struct tm *cur_time;
    time_t next_sample = 0;
    
    char buf[1024];
    char cache[1024];
    
    char* ex_name = argv[0];
    if (strstr(argv[0], "tls"))
        tls = 1;

    struct option long_options[] = {
        {"period", required_argument, NULL, 'p'},
        {"scale", required_argument, NULL, 's'},
        {"log", required_argument, NULL, 'l'},
        {"id", required_argument, NULL, 'i'},
        {"host", required_argument, NULL, 'h'},
        {0,0,0,0}
    };
    
    while ((c = getopt_long(argc, argv, "p:s:l:i:h:", long_options, NULL)) != -1) {
        if(c == 'p'){
            period = (int)atoi(optarg);
        }
        else if (c == 's'){
            if (*optarg == 'F' || *optarg == 'C') {
                scale = *optarg;
            }
            else {
                usageError(ex_name);
            }
        }
        else if (c == 'l'){
            log_filedir = creat(optarg, 0666);
            if (log_filedir < 0)
                errorExit("Error: creat()");
        }
        else if (c == 'i'){
            id = optarg;
            if (strlen(id) != 9) {
                fprintf(stderr, "Error: ID Number needs to be 9 digits. usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", ex_name);
                exit(1);
            }
        }
        else if (c == 'h'){
            host = optarg;
        }
        else{
            usageError(ex_name);
        }
    }
    if (!host | !id | !log_filedir) {
        usageError(ex_name);
    }
    if (optind < argc) {
        port_num = (int)atoi(argv[optind]);
        if (port_num <= 0) {
            usageError(ex_name);
        }
    }
    else {
        usageError(ex_name);
    }
    int socket_filedir = socketHandler(port_num);
    SSL* ssl_client = 0;
    if (tls) {
        ssl_client = SSLHandler(socket_filedir);
    }
    if (!tls) {
        dprintf(socket_filedir, "ID=%s\n", id);
    }
    else{
        SSL_write(ssl_client, ssl_buf, strlen(ssl_buf));
        sprintf(ssl_buf, "ID=%s\n", id);
    }
        
    dprintf(log_filedir, "ID=%s\n", id);
    
    temp_sensor = mraa_aio_init(1);

    struct pollfd poll_in = {socket_filedir, POLLIN, 0};
    
    memset(buf, 0, 1024);
    memset(cache, 0, 1024);
    
    int indx = 0;
    
    while (!shutdown_enabled) {
        
        if (gettimeofday(&clk, 0))
            errorExit("Error: gettimeofday()");
        if (start_enabled && clk.tv_sec >= next_sample) {
            double temp = getTemp();
            int temp_dec = temp * 10;
            
            cur_time = localtime(&(clk.tv_sec));
            if (!tls) {
                dprintf(socket_filedir, "%02d:%02d:%02d %d.%1d\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec, temp_dec/10, temp_dec%10);
            }
            else{
                int tdd = temp_dec/10;
                int tdm = temp_dec%10;
                SSL_write(ssl_client, ssl_buf, strlen(ssl_buf));
                sprintf(ssl_buf, "%02d:%02d:%02d %d.%1d\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec, tdd, tdm);
            }
            next_sample = clk.tv_sec + period;
            dprintf(log_filedir, "%02d:%02d:%02d %d.%1d\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec, temp_dec/10, temp_dec%10);
        }
        
        int ret = poll(&poll_in, 1, 0);
        if (ret < 0) {
            errorExit("Error: poll()");
        }
        if (ret > 0) {
            if (poll_in.revents & POLLIN) {
                int num_read;
                if (tls)
                    num_read = SSL_read(ssl_client, buf, 1024);
                else
                    num_read = read(socket_filedir, buf, 1024);
                if (num_read < 0) {
                    errorExit("Error: read()");
                }
                int i = 0;
                while(i < num_read && indx < 1024){
                    if (buf[i] == '\n') {
                        indx = 0;
                        commandHandler((char*)&cache);
                        memset(cache, 0, 1024);
                    }
                    else {
                        cache[indx] = buf[i];
                        indx++;
                    }
                    i++;
                }
            }
        }
    }
    
    cur_time = localtime(&(clk.tv_sec));
    if (!tls) {
        dprintf(socket_filedir, "%02d:%02d:%02d SHUTDOWN\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec);
    }
    else {
        sprintf(ssl_buf, "%02d:%02d:%02d SHUTDOWN\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec);
        SSL_write(ssl_client, ssl_buf, strlen(ssl_buf));
    }
    dprintf(log_filedir, "%02d:%02d:%02d SHUTDOWN\n", cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec);
    
    mraa_aio_close(temp_sensor);
    mraa_gpio_close(button);
    exit(0);
}
