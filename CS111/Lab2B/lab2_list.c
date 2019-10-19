#include "SortedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

const int BILL = 1000000000;

// Readability lock flag numbers
const int MUTEX_FLAG = 1;
const int SPIN_FLAG = 2;

// Values for the arguments
int element_nums;
int thread_nums = 1;
int itter_nums = 1;
int list_nums = 1;
int cur_lock = 0;
int opt_yield = 0;

// Flags for argument
int threads_flag;
int itter_flag;
int sync_flag;
int lists_flag;
int yield_flag = 0;


// Remaining globals for program
long long counter = 0;
long long time_locked = 0;
int* cur_list;
int* spin_lock;
pthread_mutex_t* mutex_pt;
SortedList_t* sorted_list;
SortedListElement_t* element_list;
char yieldopts[10] = "-";
char tag[50] = "list";


// Invalid general operation
void errorExit(char *msg){
    fprintf(stderr, msg);
    exit(1);
}

// Invalid list-based operation
void listExit(char *msg){
    fprintf(stderr, msg);
    exit(2);
}

// Segmentation fault handler
void segHandler(){
    fprintf(stderr, "Error: Segmentation Fault");
    exit(2);
}

// Hash function
int hash(const char* key) {
    return (key[0] % list_nums);
}

// Set the yield
void yieldSetter(char* optarg){
    int len = strlen(optarg);
    if (len > 3)
        errorExit("Error: too many arguments for a yield");
    
    int i = 0;
    while(i < len){
        if(optarg[i] == 'i'){
            opt_yield |= INSERT_YIELD; strcat(yieldopts, "i");
        }
        else if(optarg[i] == 'd'){
            opt_yield |= DELETE_YIELD; strcat(yieldopts, "d");
        }
        else if(optarg[i] == 'l'){
            opt_yield |= LOOKUP_YIELD; strcat(yieldopts, "l");
        }
        else{
            errorExit("Error: Agument supplied not valid");
        }
        i++;
    }
}

void initializeKeys(){
    element_nums = thread_nums * itter_nums;
    element_list = malloc(element_nums*sizeof(SortedListElement_t));
    srand(time(NULL));
    int i = 0;
    while(i < element_nums){
        char* rand_key = malloc(2*sizeof(char));
        rand_key[0] = (rand() % 26) + 'a';
        rand_key[1] = '\0';
        
        element_list[i].key = rand_key;
        i++;
    }
}

void deleteElement(SortedListElement_t* element){
    if (element == NULL){
        listExit("Error: SortedList_lookup");
    }
    
    if (SortedList_delete(element)){
        listExit("Error: SortedList_delete");
    }
}

long long calc_time(struct timespec start, struct timespec end){
    return BILL * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
}

void* listThreads(void* tid){
    struct timespec lock_start;
    struct timespec lock_end;
    
    // Put the value
    int i = *(int*)tid;
    while(i < element_nums){
        if(cur_lock == 0){
            SortedList_insert(&sorted_list[cur_list[i]], &element_list[i]);
        }
        // Mutex
        else if(cur_lock == MUTEX_FLAG){
            // Timer
            if (clock_gettime(CLOCK_MONOTONIC, &lock_start) == -1){
                errorExit("Error mutex timing begin");
            }
            pthread_mutex_lock(&mutex_pt[cur_list[i]]);
            if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1){
                errorExit("Error mutex timing end");
            }
            time_locked += calc_time(lock_start, lock_end);
            SortedList_insert(&sorted_list[cur_list[i]], &element_list[i]);
            pthread_mutex_unlock(&mutex_pt[cur_list[i]]);
        }
        // Spin
        else if(cur_lock == SPIN_FLAG){
            // Timer
            if (clock_gettime(CLOCK_MONOTONIC, &lock_start) == -1){
                errorExit("Error: spin lock timer start");
            }
            while (__sync_lock_test_and_set(&spin_lock[cur_list[i]], 1));
            if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1){
                errorExit("Error: spin lock timer end");
            }
            // Work
            time_locked += calc_time(lock_start, lock_end);
            SortedList_insert(&sorted_list[cur_list[i]], &element_list[i]);
            __sync_lock_release(&spin_lock[cur_list[i]]);
        }
        i+=thread_nums;
    }
    
    // Calculate the length
    int len = 0;
    if(cur_lock == 0){ //none
        for (i = 0; i < list_nums; i++){
            int sub_len = SortedList_length(&sorted_list[i]);
            if (sub_len == -1){
                listExit("Error: SortedList_length");
            }
            len += sub_len;
        }
    }
    else if(cur_lock == MUTEX_FLAG){
        for (i = 0; i < list_nums; i++){
            if (clock_gettime(CLOCK_MONOTONIC, &lock_start) == -1){
                errorExit("Error: mutex timer start");
            }
            pthread_mutex_lock(&mutex_pt[i]);
            if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1){
                errorExit("Error: mutex timer end");
            }
            time_locked += calc_time(lock_start, lock_end);
            int sub_len = SortedList_length(&sorted_list[i]);
            if (sub_len == -1){
                listExit("Error: SortedList_length");
            }
            len += sub_len;
            pthread_mutex_unlock(&mutex_pt[i]);
        }
    }
    if(cur_lock == SPIN_FLAG){
        for (i = 0; i < list_nums; i++){
            if (clock_gettime(CLOCK_MONOTONIC, &lock_start) == -1){
                errorExit("Error: spin lock timer start");
            }
            while (__sync_lock_test_and_set(&spin_lock[i], 1));
            if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1){
                errorExit("Error: spin lock timer end");
            }
            time_locked += calc_time(lock_start, lock_end);
            int sub_len = SortedList_length(&sorted_list[i]);
            if (sub_len == -1){
                listExit("Error: SortedList_length");
            }
            len += sub_len;
            __sync_lock_release(&spin_lock[i]);
        }
    }
    
    
    SortedListElement_t* element;
    i = *(int*)tid;
    while(i < element_nums){
        
        if(cur_lock == 0){
            element = SortedList_lookup(&sorted_list[cur_list[i]], element_list[i].key);
            deleteElement(element);
        }
        if(cur_lock == MUTEX_FLAG){
            if (clock_gettime(CLOCK_MONOTONIC, &lock_start) == -1){
                errorExit("Error: mutex timer start");
            }
            pthread_mutex_lock(&mutex_pt[cur_list[i]]);
            if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1){
                errorExit("Error: mutex timer end");
            }
            
            time_locked += calc_time(lock_start, lock_end);
            element = SortedList_lookup(&sorted_list[cur_list[i]], element_list[i].key);
            deleteElement(element);
            pthread_mutex_unlock(&mutex_pt[cur_list[i]]);
        }
        if(cur_lock == SPIN_FLAG){
            if (clock_gettime(CLOCK_MONOTONIC, &lock_start) == -1){
                errorExit("Error: spin lock timer start");
            }
            while (__sync_lock_test_and_set(&spin_lock[cur_list[i]], 1));
            if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1){
                errorExit("Error: spin lock timer end");
            }
            time_locked += calc_time(lock_start, lock_end);
            element = SortedList_lookup(&sorted_list[cur_list[i]], element_list[i].key);
            deleteElement(element);
            __sync_lock_release(&spin_lock[cur_list[i]]);
        }
        i += thread_nums;
    }
    return NULL;
}


int main(int argc, char **argv){
     int opt_index;
    int i = 0;
    int c = 0;
    
    // Get the args
    static struct option long_options[] = {
        {"threads",    required_argument, &threads_flag,    1},
        {"iterations",    required_argument, &itter_flag,     1},
        {"yield",    required_argument, &yield_flag,        1},
        {"sync",    required_argument, &sync_flag,        1},
        {"lists",    required_argument, &lists_flag,        1},
        {0,          0,                 0,                0}
    };
   
    // Parse the args and perform relevant tasks
    while (1){
        c = getopt_long(argc, argv, "", long_options, &opt_index);
        if (c == -1)
            break;
        else if (c != 0){
            errorExit("Error: argument not valid");
        }
        
        if (opt_index == 0){
            thread_nums = atoi(optarg);
        }
        else if (opt_index == 1){
            itter_nums = atoi(optarg);
        }
        else if (opt_index == 2){
            yieldSetter(optarg);
        }
        else if (opt_index == 3 && strlen(optarg) == 1){
            if(optarg[0] == 'm')
                cur_lock = MUTEX_FLAG;
            else if(optarg[0] == 's')
                cur_lock = SPIN_FLAG;
            else
                errorExit("Error: sync argument input not valid");
        }
        else if (opt_index == 4){
            list_nums = atoi(optarg);
        }
    }
    
    signal(SIGSEGV, segHandler);
    
    // What locks do we set
    if (cur_lock == MUTEX_FLAG){
        mutex_pt = malloc(list_nums*sizeof(pthread_mutex_t));
        int i = 0;
        while(i < list_nums){
            pthread_mutex_init(&mutex_pt[i], NULL);
            i++;
        }
    }
    else if (cur_lock == SPIN_FLAG){
        spin_lock = malloc(list_nums * sizeof(int));
        int i = 0;
        while(i < list_nums){
            spin_lock[i] = 0;
            i++;
        }
    }
    
    // Set the lists
    sorted_list = malloc(list_nums * sizeof(SortedList_t));
    
    while(i < list_nums){
        sorted_list[i].prev = &sorted_list[i];
        sorted_list[i].next = &sorted_list[i];
        sorted_list[i].key = NULL;
        i++;
    }
    
    // Set them keys boyo
    initializeKeys();
    
    // Determine where to distribute along the list
    cur_list = malloc(element_nums*sizeof(int));
    i = 0;
    while(i < element_nums){
        cur_list[i] = hash(element_list[i].key);
        i++;
    }
   
    // Threads
    int* tids = malloc(thread_nums * sizeof(int));
    pthread_t* threads = malloc(thread_nums * sizeof(pthread_t));
    
    
    // Timer start
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    // Threads running
    i = 0;
    while(i < thread_nums){
        tids[i] = i;
        if (pthread_create(threads + i, NULL, listThreads, &tids[i])){
            errorExit("Error: pthread_create");
        }
        i++;
    }
    
    // Threads finishing (join)
    i = 0;
    while(i < thread_nums){
        int ret = pthread_join(*(threads + i), NULL);
        if (ret){
            errorExit("Error: pthread_join");
        }
        i++;
    }
    
    // Timer stop
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    // Free mallocs
    free(element_list);
    free(tids);
    free(mutex_pt);
    
    // Get elapsed time calculation
    long long elapsed_time = BILL * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
    
    if (strlen(yieldopts) == 1)
        strcat(yieldopts, "none");
    
    // Get the tags
    strcat(tag, yieldopts);
    if(cur_lock == 0){
        strcat(tag, "-none");
    }
    else if(cur_lock == 1){
        strcat(tag, "-m");
    }
    else if(cur_lock == 2){
        strcat(tag, "-s");
    }
    // Print the values in a comma separated list
    printf("%s,%d,%d,%d,%d,%lld,%d,%ld\n", tag, thread_nums, itter_nums, list_nums, (thread_nums * itter_nums * 3), elapsed_time,(int) elapsed_time/(thread_nums * itter_nums * 3), (long int)time_locked/(thread_nums * itter_nums * 3));
    
    // Uncleared List
    i=0;
    while(i < list_nums){
        if (SortedList_length(&sorted_list[i]) != 0){
            listExit("Error: list length not 0");
        }
        i++;
    }
    
    exit(0); // Suceess!
}
