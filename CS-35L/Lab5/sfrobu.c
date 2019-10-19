#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int compared_num = 0;

// To compare the values (the function needed) to be written
int frobcmp(char const* a, char const* b)
{
    while( *a == *b){
        if (*a == ' ')
            return 0;
        a++;
        b++;
    }
    return (((*a)^42) < ((*b)^42) ? -1 : 1);
    
}

int frobcmpUp(unsigned char const* a,unsigned char const* b)
{
    while(*a == *b)
    {
        char upA = *a ^ 42;
        char upB = *b ^ 42;
        
        if ((toupper(upA)) < (toupper(upB)))
            return -1;
        else if((toupper(upA)) > toupper(upB))
            return 1;
        else
            return 0;
    }
}

// Wrapper to compare the value when sorting happens
int compare_wrapper(const void* a_ptr, const void* b_ptr) {
    const char* a = *(const char**)a_ptr;
    const char* b = *(const char**)b_ptr;
    return frobcmp(a,b);
}

int compare_up_wrapper(const void* a_ptr, const void* b_ptr) {
    const char* a = *(const char**)a_ptr;
    const char* b = *(const char**)b_ptr;
    return frobcmpUp(a,b);
}

int main(int argc, char **argv)
{
    int option = 0;
    
    // C99 Error requires that I don't declare things in the
    int i;
    int j;
    // List of all words
    char **words;
    words = (char**)malloc(sizeof(char*));
    int words_index = 0;
    
    // The current word buffer
    char* word;
    word = (char*)malloc(sizeof(char));
    int word_index = 0;
    
    // The current char and its cond
    char curr_char[1];
    int cond = read(0, curr_char, 1);
    
    // The next char and its cond
    char next_char[1];
    int next_cond = read(0, next_char, 1);
    
    // Get the option for comparing values that are 0
    if (argc == 2 && strcmp(argv[1], "-f") == 0)
        option = 1;
    
    while(cond != 0)
    {
        // Assign the cur char to the word array and reallocate to make more space
        word[word_index] = curr_char[0];
        word = realloc(word, (word_index+2)*sizeof(char));
        
        if(curr_char[0] == ' ')
        {
            words[words_index] = word;
            words = realloc(words, (words_index+2)*sizeof(char*));
            words_index++;
            word = NULL; // You added the word so no need for it anymore really
            word = (char*)malloc(sizeof(char));
            word_index = -1;
        }
        
        if(next_cond == 0 && curr_char[0] == ' ') break; // break if both conds are 0
        
        else if (curr_char[0] == ' ' && next_char[0] == ' ') // If both chars are spaces
        {
            while(curr_char[0] == ' ') 
            {
                cond = read(0,curr_char,1);
            }
            next_cond = read(0,next_char,1);
            word_index++;
            continue;
        }
        else if(next_cond == 0)
        {
            curr_char[0] = ' ';
            word_index++;
            continue;
        }
        
        curr_char[0] = next_char[0]; // The cur char needs the next char
        next_cond = read(0, next_char, 1); // Get the next cond
        word_index++; // Next char => another character in the word array
    }
    
    // Deal with the option choices
    if(option == 1) qsort(words, words_index, sizeof(char*), &compare_up_wrapper);
    else qsort(words, words_index, sizeof(char*), &compare_wrapper);
    
    // Print the words
    long word_size;
    for(i = 0; i < words_index; i++)
    {
        word_size = 0;
        j = 0;
        while(words[i][j] != ' ')
        {
            word_size++;
            j++;
        }
        write(1,words[i], word_size);
    }
    
    free(words);
    return 0;
}
