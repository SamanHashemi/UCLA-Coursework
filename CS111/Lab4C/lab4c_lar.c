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

#define BUFFER_SIZE 1024

int shut_down = 0;
int enabled = 1;
int period = 1;
char scale = 'F';
int logfd;
char* id = NULL;
char* host = NULL;
int tls = 0;
char ssl_buffer[BUFFER_SIZE];

mraa_aio_context temp_sensor;
mraa_gpio_context button;

void error_handler(char* func, int err) {
    fprintf(stderr, "Error in %s: (%d) %s\n", func, err, strerror(err));
    exit(2);
}

void invalid_command_handler(char* invalid) {
    fprintf(stderr, "Error: Invalid command received, \"%s\"\n", invalid);
    exit(2);
}

void process_command(char* command) {
    if (strncmp(command, "SCALE=", 6*sizeof(char)) == 0) {
        if (command[6] == 'F')
            scale = 'F';
        else if (command[6] == 'C')
            scale = 'C';
        else
            invalid_command_handler(command);
        
    } else if (strncmp(command, "PERIOD=", 7*sizeof(char)) == 0) {
        period = (int)atoi(command+7);
        
    } else if (strcmp(command, "STOP") == 0) {
        enabled = 0;
        
    } else if (strcmp(command, "START") == 0) {
        enabled = 1;
        
    } else if (strcmp(command, "OFF") == 0) {
        shut_down = 1;
        
    } else if (!(strncmp(command, "LOG", 3*sizeof(char)) == 0)){
        invalid_command_handler(command);
    }
    
    dprintf(logfd, "%s\n", command);
}

float read_temp() {
    int reading = mraa_aio_read(temp_sensor);
    
    int B = 4275; // thermistor value
    float R0 = 100000.0; // nominal base value
    float R = 1023.0/((float) reading) - 1.0;
    R = R0*R;
    
    float C = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;
    
    return (scale == 'F') ? (C * 9)/5 + 32 : C;
}

int socket_handler(int port_no) {
    
    struct sockaddr_in serv_addr;
    struct hostent* server;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error_handler("socket()", errno);
    }
    
    server = gethostbyname(host);
    if (server == NULL) {
        error_handler("gethostbyname()", errno);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port_no);
    
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        error_handler("connect()", errno);
    }
    
    return sockfd;
}

SSL* SSL_handler(int sockfd) {
    SSL* sslClient = 0;
    if (SSL_library_init() < 0)
        error_handler("SSL_library_init()", errno);
    
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    SSL_CTX* context = SSL_CTX_new(TLSv1_client_method());
    if (context == NULL)
        error_handler("SSL_CTX_new()", errno);
    
    sslClient = SSL_new(context);
    if (sslClient == NULL)
        error_handler("SSL_new()", errno);
    if (!SSL_set_fd(sslClient, sockfd))
        error_handler("SSL_set_fd()", errno);
    if (SSL_connect(sslClient) != 1)
        error_handler("SSL_connect()", errno);
    
    return sslClient;
}

int main(int argc, char **argv) {
    
    char* exec_name = argv[0];
    if (strstr(argv[0], "tls"))
        tls = 1;
    
    int port = 0;
    
    struct option options[] = {
        {"period", 1, NULL, 'p'},
        {"scale", 1, NULL, 's'},
        {"log", 1, NULL, 'l'},
        {"id", 1, NULL, 'i'},
        {"host", 1, NULL, 'h'},
        {0,0,0,0}
    };
    
    int oc;
    while ((oc = getopt_long(argc, argv, "p:s:l:i:h:", options, NULL)) != -1) {
        switch(oc) {
            case 'p':
                period = (int)atoi(optarg);
                break;
            case 's':
                if (*optarg == 'F' || *optarg == 'C') {
                    scale = *optarg;
                } else {
                    fprintf(stderr, "usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", exec_name);
                    exit(1);
                }
                break;
            case 'l':
                logfd = creat(optarg, 0666);
                if (logfd < 0)
                    error_handler("creat()", errno);
                break;
            case 'i':
                id = optarg;
                if (strlen(id) != 9) {
                    fprintf(stderr, "Error: ID Number needs to be 9 digits. usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", exec_name);
                    exit(1);
                }
                break;
            case 'h':
                host = optarg;
                break;
            default:
                fprintf(stderr, "usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", exec_name);
                exit(1);
        }
    }
    if (!host | !id | !logfd) {
        fprintf(stderr, "usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", exec_name);
        exit(1);
    }
    if (optind < argc) {
        port = (int)atoi(argv[optind]);
        if (port <= 0) {
            fprintf(stderr, "usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", exec_name);
            exit(1);
        }
    } else {
        fprintf(stderr, "usage: %s [--period=#] [--scale=[CF]] --log=FILENAME --id=ID_NUMBER --host=HOSTNAME portnumber\n", exec_name);
        exit(1);
    }
    int sockfd = socket_handler(port);
    SSL* sslClient = 0;
    if (tls) {
        sslClient = SSL_handler(sockfd);
    }
    if (tls) {
        sprintf(ssl_buffer, "ID=%s\n", id);
        SSL_write(sslClient, ssl_buffer, strlen(ssl_buffer));
    } else
        dprintf(sockfd, "ID=%s\n", id);
    dprintf(logfd, "ID=%s\n", id);
    
    temp_sensor = mraa_aio_init(1);
    
    struct timeval clock;
    struct tm *now;
    time_t next_sample = 0;
    
    struct pollfd poll_in = {sockfd, POLLIN, 0};
    
    char buffer[BUFFER_SIZE];
    char cache[BUFFER_SIZE];
    
    memset(buffer, 0, BUFFER_SIZE);
    memset(cache, 0, BUFFER_SIZE);
    
    int index = 0;
    
    while (!shut_down) {
        
        if (gettimeofday(&clock, 0))
            error_handler("gettimeofday()", errno);
        if (enabled && clock.tv_sec >= next_sample) {
            float temp = read_temp();
            int temp_dec = temp * 10;
            
            now = localtime(&(clock.tv_sec));
            if (tls) {
                sprintf(ssl_buffer, "%02d:%02d:%02d %d.%1d\n", now->tm_hour, now->tm_min, now->tm_sec, temp_dec/10, temp_dec%10);
                SSL_write(sslClient, ssl_buffer, strlen(ssl_buffer));
            } else
                dprintf(sockfd, "%02d:%02d:%02d %d.%1d\n", now->tm_hour, now->tm_min, now->tm_sec, temp_dec/10, temp_dec%10);
            dprintf(logfd, "%02d:%02d:%02d %d.%1d\n", now->tm_hour, now->tm_min, now->tm_sec, temp_dec/10, temp_dec%10);
            
            next_sample = clock.tv_sec + period;
        }
        
        int ret = poll(&poll_in, 1, 0);
        if (ret < 0) {
            error_handler("poll()", errno);
        }
        if (ret > 0) {
            if (poll_in.revents & POLLIN) {
                int num_read;
                if (tls)
                    num_read = SSL_read(sslClient, buffer, BUFFER_SIZE);
                else
                    num_read = read(sockfd, buffer, BUFFER_SIZE);
                if (num_read < 0) {
                    error_handler("read()", errno);
                }
                
                for (int i = 0; i < num_read && index < BUFFER_SIZE; i++) {
                    if (buffer[i] == '\n') {
                        process_command((char*)&cache);
                        index = 0;
                        memset(cache, 0, BUFFER_SIZE);
                    } else {
                        cache[index] = buffer[i];
                        index++;
                    }
                }
            }
        }
    }
    
    now = localtime(&(clock.tv_sec));
    if (tls) {
        sprintf(ssl_buffer, "%02d:%02d:%02d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
        SSL_write(sslClient, ssl_buffer, strlen(ssl_buffer));
    } else
        dprintf(sockfd, "%02d:%02d:%02d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
    dprintf(logfd, "%02d:%02d:%02d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
    
    mraa_aio_close(temp_sensor);
    exit(0);
}
