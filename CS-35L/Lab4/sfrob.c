#include<stdio.h>
#include<stdlib.h>

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

// Wrapper to compare the value when sorting happens
int compare_wrapper(const void* a_ptr, const void* b_ptr) {
    const char* a = *(const char**)a_ptr;
    const char* b = *(const char**)b_ptr;
    return frobcmp(a,b);
}


int main(void){
    // Create the word buffer to hold the value of each word
    char* word = (char*)malloc(sizeof(char));
    int word_size = 1;
    int word_index = 0;
    
    // Hold the list of words
    char** words = (char**)malloc(sizeof(char*));
    int words_size = 1;
    int words_index = 0;
    
    int ended = 0; // if the read has ended
    
    char cur_char; // get the curent character
    
    while(ended == 0) {
        cur_char = getchar();
        
        // Check if end of file was reached
        if(cur_char == EOF) {
            ended = 1; // The file has reached the end but deal w the space
            cur_char = ' '; // Make the cur char a space
        }
        
        // We need to resize the word size if the index has caught up
        if(word_size <= word_index) {
            word_size *= 2;
            word = (char*) realloc(word, sizeof(char) * word_size);
        }
        
        // Add char to the current word
        word[word_index] = cur_char;
        word_index++;
        
        // A space was encountered
        if(cur_char == ' '){
            // Reallocate space b/c word is at max size
            if(words_size <= words_index) {
                words_size *= 2;
                words = (char**) realloc(words, sizeof(char*) * words_size);
            }
            
            // Add word to the list
            words[words_index] = word;
            words_index++;
            
            // Reset saved word (char array)
            word = (char*) malloc(sizeof(char));
            word_size = 1;
            word_index = 0;
        }
    }
    
    // Sort using the comparison wrapper
    qsort(words, words_index, sizeof(char*), &compare_wrapper);
    
    for(int i = 0; i < words_index; i++) {
        for(int j = 0;;j++) {
            putchar(words[i][j]);
            if(words[i][j] == ' ') break;
        }
    }
    //De-allocate all memory that was used
    for(int i = 0; i < words_size; i++)
    {
        free(words[i]);
    }
    free(words);
    free(word);
    exit(0);
}
