//NAME: Saman Hashemipour
//EMAIL: hash.saman@gmail.com
//ID: 904903562

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

// Declare flag numbers for readability
const int MUTEX_FLAG = 1;
const int SPIN_FLAG = 2;
const int COMP_AND_SWAP_FLAG = 3;

// Declare argument flags for actual use
int thread_flag;
int ittr_flag;
int sync_flag;
int yield_flag = 0;

// Counters and globals
pthread_mutex_t mutex;
long long counter = 0;
int spin_lock = 0;
int thread_num = 1;
int itter_num = 1;
int cur_lock = 0;
char str[20] = "";

// The given add function
void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (yield_flag) { sched_yield(); }
    *pointer = sum;
}

// Incorrect input error function
void errorExit(char *msg){
    fprintf(stderr, msg);
    exit(1);
}

// The function that does the bulk of the thread work (w/ locks)
void* adder(){
    long long value = 1;
    while(value > -2){
        int i = 0;
        while(i < itter_num){
            if(cur_lock == 0){
                add(&counter, value);
            }
            else if(cur_lock == 1){
                pthread_mutex_lock(&mutex);
                add(&counter, value);
                pthread_mutex_unlock(&mutex);
            }
            else if(cur_lock == 2){
                while (__sync_lock_test_and_set(&spin_lock, 1));
                add(&counter, value);
                __sync_lock_release(&spin_lock);
            }
            else if(cur_lock == 3){
                long long old;
                long long new;
                do {
                    old = counter;
                    new = old + value;
                    if (yield_flag) { sched_yield(); }
                } while (__sync_val_compare_and_swap(&counter, old, new) != old);
            }
            i++;
        }
        value -= 2;
    }
    return NULL;
}

int main(int argc, char **argv){
    int i = 0;
    int c = 0;
    int opt_index;
    struct timespec t_start;
    
    static struct option long_options[] = {
        {"threads",        required_argument, &thread_flag,     1},
        {"iterations",    required_argument, &ittr_flag, 1},
        {"yield",        no_argument,        &yield_flag,         1},
        {"sync",        required_argument, &sync_flag,         1}
    };
   
    while (c != -1){
        c = getopt_long(argc, argv, "", long_options, &opt_index);
        if (c == -1)
            break;
        else if (c != 0)
            errorExit("Error: Argument input was invalid");
        
        if (opt_index == 3 && strlen(optarg) == 1){
            if(optarg[0] == 'c'){
                cur_lock = COMP_AND_SWAP_FLAG;
            }
            else if(optarg[0] == 's'){
                cur_lock = SPIN_FLAG;
            }
            else if(optarg[0] == 'm'){
                cur_lock = MUTEX_FLAG;
            }
            else{
                 errorExit("Error: sync argument invalid");
            }
        }
        if (opt_index == 0){
            thread_num = atoi(optarg);
        }
        if (opt_index == 1){
            itter_num = atoi(optarg);
        }
    }

    if (cur_lock == MUTEX_FLAG)
        pthread_mutex_init(&mutex, NULL);
    
    
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    pthread_t *tids = malloc(thread_num * sizeof(pthread_t));
    
    
    while(i < thread_num){
        if (pthread_create(&tids[i], NULL, adder, NULL))
            errorExit("Error: Thread creation");
        i++;
    }

    i = 0;
    while(i < thread_num){
        if (pthread_join(tids[i], NULL))
            errorExit("Error: Thread joining");
        i++;
    }

    
    struct timespec t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    
    free(tids);

    
    long long elapsed_time = ((t_end.tv_sec - t_start.tv_sec) * 1000000000) + t_end.tv_nsec - t_start.tv_nsec;

    // Get tags
    if (yield_flag) strcat(str, "-yield");
    if(cur_lock == 0){
        strcat(str, "-none");
    }
    else if(cur_lock == 1){
        strcat(str, "-m");
    }
    else if(cur_lock == 2){
        strcat(str, "-s");
    }
    else if(cur_lock == 3){
        strcat(str, "-c");
    }
    printf("add%s,%d,%d,%d,%lld,%d,%lld\n", str, thread_num, itter_num,
           (int)(thread_num * itter_num  * 2), elapsed_time, (int)(elapsed_time / (thread_num * itter_num  * 2)), counter);
    
    exit(0);
}
