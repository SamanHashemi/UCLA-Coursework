#include "SortedList.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
    if (list == NULL || element == NULL)
        return;
    SortedListElement_t* curr = list->next;
    
    while (curr != list)
    {
        //      fprintf(stderr, "stuck\n");
        if (strcmp(element->key, curr->key) <= 0)
            break;
        curr = curr->next;
    }
    if (opt_yield & INSERT_YIELD)
        sched_yield();
    element->prev = curr->prev;;
    element->next = curr;
    curr->prev->next = element;
    curr->prev = element;
}

int SortedList_delete( SortedListElement_t *element)
{
    if (element == NULL)
        return 1;
    
    SortedListElement_t* prev = element->prev;
    SortedListElement_t* next = element->next;
    
    if (prev->next == next->prev)
    {
        if (opt_yield & DELETE_YIELD)
            sched_yield();
        prev->next = next;
        next->prev = prev;
        return 0;
    }
    return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
    SortedListElement_t* curr;
    if (key == NULL || list == NULL)
        return NULL;
    curr = list->next;
    while (curr != list)
    {
        if (strcmp(key, curr->key) == 0)
            break;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        curr = curr->next;
    }
    return curr;
}

int SortedList_length(SortedList_t *list)
{
    if (list == NULL)
        return -1;
    SortedListElement_t* curr = list->next;
    int count = 0;
    while (curr != list)
    {
        if (curr->next->prev != curr || curr->prev->next != curr)
            return -1;
        count++;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        curr = curr->next;
    }
    return count;
}
