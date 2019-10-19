//NAME: Saman Hashemipour
//EMAIL: hash.saman@gmail.com
//ID: 904903562

#include "SortedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

// Readability lock flag numbers
const int MUTEX_FLAG = 1;
const int SPIN_FLAG = 2;

// Values for the arguments
int thread_nums = 1;
int itter_nums = 1;
int element_nums;
int cur_lock = 0;
int opt_yield = 0;

// Flags for argument
int threads_flag;
int itter_flag;
int yield_flag = 0;
int sync_flag;

// Remaining globals for program
long long counter = 0;
int spin_lock = 0;
pthread_mutex_t mutex;
SortedList_t* list;
SortedListElement_t* elements;
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

// Establish the yield
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
    srand(time(NULL));
    int i = 0;
    while(i < element_nums){
        
        char* rand_key = malloc(2*sizeof(char));
        rand_key[0] =  (rand() % 26) + 'a';
        rand_key[1] = '\0';
        
        elements[i].key = rand_key;
        i++;
    }
}

void deleteElement(SortedListElement_t* element){
    if (element == NULL)
        listExit("Error: SortedList_lookup");
    
    if (SortedList_delete(element))
        listExit("Error: SortedList_delete");
}

// Bulk of the work for the threads (per thread)
void* listThreads(void* tid){

    int i = *(int*)tid;
    
    while(i < element_nums){
        if(cur_lock == 0)
            SortedList_insert(list, &elements[i]);
        else if(cur_lock == 1){
            pthread_mutex_lock(&mutex);
            SortedList_insert(list, &elements[i]);
            pthread_mutex_unlock(&mutex);
        }
        else if(cur_lock == 2){
            while (__sync_lock_test_and_set(&spin_lock, 1));
            SortedList_insert(list, &elements[i]);
            __sync_lock_release(&spin_lock);
        }
        i += thread_nums;
    }
    
    
    int len;
    if(cur_lock == 0){
        len = SortedList_length(list);
        if (len == -1)
            listExit("Error: SortedList_length");
    }
    else if(cur_lock == 1){
        pthread_mutex_lock(&mutex);
        len = SortedList_length(list);
        if (len == -1)
            listExit("Error: SortedList_length");
        pthread_mutex_unlock(&mutex);
    }
    else if(cur_lock == 2){
        while (__sync_lock_test_and_set(&spin_lock, 1));
        len = SortedList_length(list);
        if (len == -1)
            listExit("Error: SortedList_length");
        __sync_lock_release(&spin_lock);
    }
    
    
    SortedListElement_t* element;
    i = *(int*)tid;
    while(i < element_nums){
        if(cur_lock == 0){
            element = SortedList_lookup(list, elements[i].key);
            deleteElement(element);
        }
        else if(cur_lock == 1){
            pthread_mutex_lock(&mutex);
            element = SortedList_lookup(list, elements[i].key);
            deleteElement(element);
            pthread_mutex_unlock(&mutex);
        }
        else if(cur_lock == 2){
            while (__sync_lock_test_and_set(&spin_lock, 1));
            element = SortedList_lookup(list, elements[i].key);
            deleteElement(element);
            __sync_lock_release(&spin_lock);
        }
        i += thread_nums;
    }
    return NULL;
}

int main(int argc, char **argv){
    int i = 0;
    int c = 0;
    int opt_index;
    
    static struct option long_options[] = {
        {"threads",        required_argument, &threads_flag,     1},
        {"iterations",    required_argument, &itter_flag, 1},
        {"yield",        required_argument,        &yield_flag,         1},
        {"sync",        required_argument, &sync_flag,         1}
    };
    
    
    while (c != -1){
        c = getopt_long(argc, argv, "", long_options, &opt_index);
        if (c == -1)
            break;
        if (c != 0)
            errorExit("Error: invalid argument");
        
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
            if(optarg[0] == 'm'){
                cur_lock = MUTEX_FLAG;
            }
            else if(optarg[0] == 's'){
                cur_lock = SPIN_FLAG;
            }
            else{
                errorExit("Error: sync argument not valid");
            }
        }
    }
    
    signal(SIGSEGV, segHandler);

    if (cur_lock == MUTEX_FLAG)
        pthread_mutex_init(&mutex, NULL);
    
    // Thread creationg
    pthread_t* threads = malloc(thread_nums * sizeof(pthread_t));
    int* tids = malloc(thread_nums * sizeof(int));
    
    // Elements list
    element_nums  = thread_nums * itter_nums;
    elements = malloc(element_nums*sizeof(SortedListElement_t));
    initializeKeys();
    
    list = malloc(sizeof(SortedList_t));
    list->next = list;
    list->prev = list;
    list->key = NULL;
    
    // Timer start
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    // Threads running
    while(i < thread_nums){
        tids[i] = i;
        if (pthread_create(threads + i, NULL, listThreads, &tids[i]))
            errorExit("Error: pthread_create");
        i++;
    }
    // Threads finishing (join)
    i = 0;
    while(i < thread_nums){
        int ret = pthread_join(*(threads + i), NULL);
        if (ret) errorExit("Error: pthread_join");
        i++;
    }
    // Timer stop
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    // Free mallocs
    free(elements);
    free(tids);
    
    // Get elapsed time calculation
    long long elapsed_time = ((end_time.tv_sec - start_time.tv_sec) * 1000000000) + end_time.tv_nsec - start_time.tv_nsec;

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
    
    // Print out the values in a comma sorted list
    printf("%s,%d,%d,1,%d,%lld,%d\n", tag, thread_nums, itter_nums, (int)(thread_nums * itter_nums * 3), elapsed_time, (int)(elapsed_time/(thread_nums * itter_nums * 3)));
    
    // Uncleared list
    if (SortedList_length(list) != 0)
        listExit("Error: List was not cleared");
    
    exit(0); // Success!
}
