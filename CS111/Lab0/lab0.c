#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void readwrite(int input, int output)
{
    char* buff;
    buff = (char*) malloc(sizeof(char));
    ssize_t cur = read(input, buff, 1);
    while(cur > 0)
    {
        write(output, buff, 1);
        cur = read(input, buff, 1);
    }
    free(buff);
}

void sig_handler(int sig_num)
{
    if(sig_num == SIGSEGV)
    {
        perror("Segmentation fault found using catch. Error in sig_handler function");
        exit(4);
    }
}

int main(int argc, char* argv[])
{
    // https://stackoverflow.com/questions/17877368/getopt-passing-string-parameter-for-argument#
    char* input_file = NULL; // For the input arugment
    char* output_file = NULL; // For the output argument
    char* cause_fault = NULL; // For the segfault argument
    int seg_fault = 0;
    
    // Option realted variables
    const char* short_opts = "i:o:sc"; //
    static struct option long_opts[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"segfault", no_argument, NULL, 's'},
        {"catch", no_argument, NULL, 'c'}
    };
    
    int sel = 0;
    // Pre step in spec
    while((sel = getopt_long(argc, argv, short_opts, long_opts, NULL))!= -1) //
    {
        switch(sel)
        {
            //https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html
            case 'i':
                input_file = optarg; // get the cur option from getopt (in this case for input_file)
                break;
            case 'o':
                output_file = optarg; // get the cur option from getopt (in this case for output_file)
                break;
            case 's':
                seg_fault = 1;
                break;
            case 'c':
                // https://www.geeksforgeeks.org/signals-c-language/
                signal(SIGSEGV, sig_handler);
                break;
            default:
                printf("No correct option was selected. Correct format: ./lab0 [sc] -i filename -o filename");
                exit(1);
        }
    }
    // https://stackoverflow.com/questions/11635219/dup2-dup-why-would-i-need-to-duplicate-a-file-descriptor
    // Step 1 from spec
    if(input_file)
    {
        //http://codewiki.wikidot.com/c:system-calls:open
        int in_des = open(input_file, O_RDONLY); // get the status of the input file (w/ read only permish.)
        if(in_des >= 0) // file can open fine
        {
            close(0); // close stdin
            dup(in_des); // duplicate the input file
            close(in_des); // close the input file
        }
        else // issue opening the file
        {
            char str[100] = "Error opening file: ";
            strcat(str, input_file);
            strcat(str, ". \"open\" error Use of --input");
            perror(str);
            exit(1);
        }
    }
    // Step 2 from spec
    if(output_file)
    {
        // https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rtcre.htm
        int out_des = creat(output_file, 0666);
        if(out_des)
        {
            close(1); // close stout
            dup(out_des); // duplicate output file
            close(out_des); // close output file
        }
        else // error creating new file
        {
            char str2[100] = "Error creating file: ";
            strcat(str2, output_file);
            strcat(str2, ". \"creat\" error Use of --output");
            perror(str2);
            exit(2);
        }
    }
    // Step 3 from spec
    if(seg_fault)
    {
        char some_char = *cause_fault;
    }
    // Step 4 from spec
    readwrite(0,1); // read and write using fd0 and fd1 as parameters
    exit(0); // Success!
}
