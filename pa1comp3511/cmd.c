#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LEN 256

// Assume that we have at most 16 pipe segments
#define MAX_PIPE_SEGMENTS 16

// Assume that each segment has at most 256 characters (including NULL)
#define MAX_SEGMENT_LENGTH 256
 
// This function will be invoked by main()
// TODO: Implement the multi-level pipes below
void process_cmd(char *cmdline);

// tokenize function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *pipe_segments[MAX_PIPE_SEGMENTS]; // character array buffer to store the pipe segements
// int num_pipe_segments; // an output integer to store the number of pipe segment parsed by this function
// char cmdline[MAX_CMDLINE_LEN]; // The input command line
//
// Sample usage of this tokenize function:
//
//  tokenize(pipe_segments, cmdline, &num_pipe_segments, "|");
// 
void tokenize(char **argv, char *line, int *numTokens, char *token);


/* The main function implementation */
int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    fgets(cmdline, MAX_CMDLINE_LEN, stdin);
    process_cmd(cmdline);
    return 0;
}

/* 
    Implementation of process_cmd

    TODO: Clearly explain how you implement process_cmd in point form. For example:

    Step 1: check whether the input is "exit" or not. If yes, then exit the whole program 
    Step 2: separate the input into different segment; where each segments are divided by the delimiter "|". The result will be put into a array of strings.
    Step 3: Create a for loop that loop as many as the number of segments.
	Step 3.1: create pfds variable to support the pipe.
	Step 3.2: If the currentsegment is not the last one, create the pipe with input of pfds[1] and output of pfds[0].
	Step 3.3: Check whether there is an error in the fork() function.
	Step 3.4: for child process
		Step 3.4.1: If it is not the first segment, make the input of the current segment the same as fd.
		Step 3.4.2: If it is not the last segment, copy the output of the current segment to the input for pipe. 
		Step 3.4.3: For each segment, tokenize it again to seperate the flag. Such as wc    -l into "wc" and "-l. Then execute it by using execvp.
	Step 3.5: for the parent process, wait for the child, then use the pipe output as the new value of fd.
 */
void process_cmd(char *cmdline) {
    //step 1
    if (strcmp(cmdline, "exit") == 0) {
	exit(0);
    }   
 
    char* strings[MAX_PIPE_SEGMENTS];
    int fd = 0;
    int n = 0;
    int* num_seg = &n;    
    
    //step 2
    tokenize(strings ,cmdline, num_seg, "|");
    
    //step 3
    int i = 0;    
    for (i = 0; i < *num_seg; ++i) {
        //step 3.1
	int pfds[2];
        
	//step3.2
        if (i < (*num_seg-1)) { 
            pipe(pfds);
        } 
        
	//step3.3
        pid_t pid;
	if ((pid= fork()) == -1){
	    exit(EXIT_FAILURE);
	}
	//step 3.4
        else if (pid == 0) {
	    //step 3.4.1
	    if(i > 0) {
		dup2(fd, 0);
		close(fd);
	    }	    
	    //step 3.4.2           
            if(i != (*num_seg-1)) {
                dup2(pfds[1], 1);
                close(pfds[1]);
            }
            //step 3.4.3    
            char* strings2[MAX_SEGMENT_LENGTH];
            int m = 0;
	    int* useless = &m;
            tokenize(strings2, strings[i], useless, " ");
            execvp(strings2[0], strings2);
        }
	//step3.5
	else {        
            wait(NULL);
            close(pfds[1]);
            fd = pfds[0];
	}
    }
}


// Implementation of tokenize function
void tokenize(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    argv[argc++] = NULL;
    *numTokens = argc - 1;
}
