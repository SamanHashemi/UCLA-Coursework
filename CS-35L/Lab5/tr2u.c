#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[])
{
    
    if(argc != 3)
    {
        fprintf(stderr, "ERROR: # of args");
        exit(1);
    }
    
    char* from = argv[1];
    char* to = argv[2];
    char val[2];
    
    int f_len = strlen(from); // str length of from
    int t_len = strlen(to); // str length of to
    
    ssize_t dw;
    
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
    for(int i = 0; i < f_len; i++)
    {
        for(int j = i+1; j < f_len; j++)
        {
            if(from[j] == from[i])
            {
                fprintf(stderr, "ERROR: Duplicate Byte");
                exit(1);
            }
        }
    }
    
    char curr_char[1];
    bool to_write;
    while(read(0,curr_char, 1) > 0)
    {
        for(int i = 0; i < f_len; i++)
        {
            to_write = true;
            if(curr_char[0] == from[i])
            {
                curr_char[0] = to[i];
                write(1,curr_char,1);
                to_write = false;
                break;
            }
        }
        if(to_write) write(1,curr_char,1);
    }
    return 0;
}
