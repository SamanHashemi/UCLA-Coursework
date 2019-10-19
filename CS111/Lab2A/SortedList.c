// NAME: Saman Hashemipour
// EMAIL: hash.saman@gmail.com
// ID: 904903562

#include "SortedList.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *    The specified element will be inserted in to
 *    the specified list, which will be kept sorted
 *    in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
    if (list == NULL || element == NULL) return;
    SortedListElement_t *prev = list;
    SortedListElement_t *cur = list->next;
    while (cur != list){
        if (strcmp(element->key, cur->key) <= 0)
            break;
        prev = cur;
        cur = cur->next;
    }
    if (opt_yield & INSERT_YIELD) sched_yield();
    
    
    element->next = cur;
    element->prev = prev;
    prev->next = element;
    cur->prev = element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *    The specified element will be removed from whatever
 *    list it is currently in.
 *
 *    Before doing the deletion, we check to make sure that
 *    next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element){
    if (element == NULL || element->key == NULL || element->next->prev != element || element->prev->next != element)
        return 1;
    
    SortedListElement_t* prev = element->prev;
    SortedListElement_t* next = element->next;
    
    if (opt_yield & DELETE_YIELD)
        sched_yield();
    
    prev->next = next;
    next->prev = prev;
    
    return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *    The specified list will be searched for an
 *    element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
    
    SortedListElement_t* cur = list->next;
    if (list == NULL || key == NULL) return NULL;

    while (strcmp(cur->key, key) != 0){
        if (strcmp(cur->key, key) > 0 || cur == list) //element not found
            return NULL;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        cur = cur->next;
    }
    
    return cur;
}

/**
 * SortedList_length ... count elements in a sorted list
 *    While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *       -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list){
    int count = 0;
    SortedListElement_t* cur = list->next;
    if (list == NULL) return -1;

    while (cur != list){
        
        if (cur == NULL || cur->next == NULL || cur->prev == NULL || cur->prev->next != cur || cur->next->prev != cur)
            return -1;
        
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        count++;
        cur = cur->next;
    }
    return count;
}
