#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void checkIOError()
{
    if (ferror(stdin) != 0)
    {
        fprintf(stderr, "Error in reading stdin");
        exit(1);
    }
}


int main(int argc, char* argv[])
{

    if(argc != 3)
    {
        fprintf(stderr, "Incorrect number of arguments! Please input 2 arguments.");
        exit(1);
    }
    
    char* from = argv[1];
    char* to = argv[2];
    
    int f_len = strlen(from); // str length of from
    int t_len = strlen(to); // str length of to
    
    // Check for length of 0
    if (f_len == 0)
    {
        fprintf(stderr, "ERROR: From Empty");
        exit(1);
    }
    
    // Check for size mismatch
    if(f_len != t_len)
    {
        fprintf(stderr, "ERROR: Size Mismatch");
        exit(1);
    }
    
    // Check for duplicate characters
    for(size_t i = 0; i < f_len; i++)
    {
        for(size_t j = i+1; j < f_len; j++)
        {
            if(from[j] == from[i])
            {
                fprintf(stderr, "ERROR: Duplicate Byte");
                exit(1);
            }
        }
    }
    
    bool to_put;
    char curr_char = getchar();
    checkIOError();
    while (curr_char != EOF)
    {
        for (int i = 0; i < f_len; i++)
        {
            to_put = true;
            if (curr_char == from[i])
            {
                putchar(to[i]);
                checkIOError();
                to_put = false;
                break;
            }
        }
        if (to_put) putchar(curr_char);
        
        curr_char = getchar();
        checkIOError();
    }
}
