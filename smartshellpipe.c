
#include <string.h>    /* for strcpy and strtok */
#include <unistd.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


#define MAX_ARGV  10 /* maximum number of command tokens */
#define MAX_CMD   80 /* maximum length of command */

#define MAX_PIPES 1 /*maximum number of commands which can be strung together with a pipe*/


void tokenize_cmd (char * cmd, int max, char * argv[])
/* Input:  cmd is the command to be tokenized
 *         max is the maximum number of tokens allowed  
 * Output: argv is the array of tokens (command arguments) 
 */
{
    int num_args = 0;           /* number of arguments (tokens) in cmd */

    if(cmd == NULL) return;     /* nothing to do */

    /* Tokenize the command */
    argv[0] = strtok(cmd, " \n");
    while((num_args < max) && (argv[num_args] != NULL))
    {
       num_args++;
       argv[num_args] = strtok(NULL, " \n");
    }
}


void tokenize_by_pipe(char * cmd, int max, char * argv[])
/* Input:  cmd is the command to be tokenized
 *         max is the maximum number of tokens allowed  
 * Output: argv is the array of tokens (command arguments) 
 */
{
    int num_args = 0;           /* number of arguments (tokens) in cmd */

    if(cmd == NULL) return;     /* nothing to do */

    /* Tokenize the command */
    argv[0] = strtok(cmd, "|");
    while((num_args < max) && (argv[num_args] != NULL))
    {
       num_args++;
       argv[num_args] = strtok(NULL, "|");
       //printf("%s \n", argv[num_args]);
    }
}

void execute_cmd(char * cmd)
{
    char * argv[MAX_ARGV];  /* array of command arguments*/
    char cmd_copy[MAX_CMD]; /* buffer to store local copy of cmd*/
    /* Make a local copy of the command */
    if(cmd == NULL) return; /* nothing to do */
    strcpy(cmd_copy, cmd);
    /* Tokenize the command */
    tokenize_cmd(cmd_copy, MAX_ARGV, argv);
    /*fork a new process*/
    int ret; 
    ret = fork(); 
    if (ret == -1){
        printf("Fork failed \n");
        
    }
    /* child runs the command */ 
    else if (ret ==0){
        //printf("I'm the child, I'm running the command \n");
        execvp(argv[0], argv);

        printf("Oops, that command didn't work. Try something else. \n");

        exit(1);
    }
    /*Parent waits for child to finish*/

    else {
        wait(0);

    }

}

void execute_output_redirect_cmd(char * cmd, char * fname){
    printf("fname: %s \n", fname);

    char * argv[MAX_ARGV];  /* array of command arguments*/
    char cmd_copy[MAX_CMD]; /* buffer to store local copy of cmd*/
   
    /* Make a local copy of the command */
    if(cmd == NULL) return; /* nothing to do */
    strcpy(cmd_copy, cmd);

    /* Tokenize the command */
    tokenize_cmd(cmd_copy, MAX_ARGV, argv);

    /*fork a new process*/
    int ret; 
    ret = fork(); 
    if (ret == -1){
        printf("Fork failed \n");
        
    }

    /* child runs the command */ 
    else if (ret ==0){

        int fd;    
        fd = open(fname, O_RDWR | O_CREAT); 
        if (fd== -1){
            printf("file error");
        }

    /* use  dup2 to redirect ouput to fd  */ 
        dup2(fd, 1);
        close(fd);

        execvp(argv[0], argv);

        printf("Oops, that command didn't work. Try something else. \n");

        exit(1);
    }
        
    /*parent waits for child to finish*/

    else {
        wait(0);

    }


    }
    
    void execute_input_redirect_cmd(char * cmd, char * fname){


    char * argv[MAX_ARGV];  /* array of command arguments*/
    char cmd_copy[MAX_CMD]; /* buffer to store local copy of cmd*/
    /* Make a local copy of the command */
    if(cmd == NULL) return; /* nothing to do */
    strcpy(cmd_copy, cmd);

    /* Tokenize the command */
    tokenize_cmd(cmd_copy, MAX_ARGV, argv);

    /*fork a new process*/
    int ret; 
    ret = fork(); 
    if (ret == -1){
        printf("Fork failed \n");
        
    }

    /* child runs the command */ 
    else if (ret ==0){

        int fd;    
        fd = open(fname, O_RDONLY); 
        if (fd== -1){
            printf("file error");
        }

    // use  dup2 to redirect input to fd  
        dup2(fd, 0);
        close(fd);

        //printf("I'm the child, I'm running the command \n");
        execvp(argv[0], argv);

        printf("Oops, that command didn't work. Try something else. \n");

        exit(1);
    }
    /*partent waits for child to finish*/

    else {
        //printf("I'm the parent, I'm going to wait \n"); 
        wait(0);

    }


    }


void execute_piped_cmd (char * cmd1, char * cmd2){
    printf("execute piped command");

     if (cmd1 == NULL || cmd2 == NULL){
        return;
    }

    /* create argv cmd 1, argc command 2 */
    char * cmd1_argv[MAX_ARGV];
    char * cmd2_argv[MAX_ARGV];

    /*copy commands*/

    char cmd1_copy[MAX_CMD];
    char cmd2_copy[MAX_CMD];

    strcpy(cmd1_copy, cmd1);
    strcpy(cmd2_copy, cmd2);

    /*tokenize commands and save to cmd1_argv and cmd2 argv*/

    tokenize_cmd(cmd1_copy, MAX_ARGV, cmd1_argv);
    tokenize_cmd(cmd2_copy, MAX_ARGV, cmd2_argv);


    /* declare int ret to hold return value and array p to hold pipe input and ouput */
    int ret ;
    int p[2];

    /* declare pipe. if unsucessful, return -1 for failure */ 
    ret = pipe(p);
    if (ret==-1){ 
        printf(" pipeerror \n");
        return;}
    
    else{ printf("made pipe \n");}


    // fork child 1. if fork fails, print and return 
    ret = fork();
    if (ret == -1){printf("fork failed"); return;}

    // wait(0);


    /*inside child one, close the read end of the pipe, redirect output to the write end of the pipe, and execute command one*/
    if (ret == 0){
        printf("child one \n");

        // close read end (hopefully this is the read end?)
        close(p[0]);
       //redirect stdout to write end of pipe
       printf("command one: [%s] \n", cmd1_argv[0]);
   
        dup2(p[1], 1);
        //execute cmd 1, return -1 and close write end of pipe if failes

        int exec = execvp(cmd1_argv[0], cmd1_argv);
        if (exec== -1){
            printf("Failed to execute cmd 1 \n");
            close(p[1]);
             return; 
             }
    }

    // back in the parent
    else {
        printf("back in parent \n");

        // declare ret 2 to store second child 
        int ret_2;
        ret_2 = fork(); 
        if (ret_2 == -1){printf("second fork failed"); return;}
        else{
            if(ret_2 == 0){
                printf("command two: [%s] \n", cmd2_argv[0]);
                printf("in child two \n");

             /* close write end of pipe */ 
             close(p[1]);

            /* redirect stdin to read end of pipe */ 
            dup2(p[0], 0);

            /* call execvp cmd_2[argv]. if execvp fails, close read end of pipe */ 
                
            int exec = execvp(cmd2_argv[0], cmd2_argv);
            if (exec == -1){
                printf("failed to execute cmd 2 \n");
                close(p[0]);
                return;
            }

            }

            /*parent*/
            else{

                wait(0);
                wait(0);
            }

        }
    
}
}


int main()
{
   
    /*while not quitting*/
    int running= 1; 
    char * prompt_color = KMAG; 

    char buffer[MAX_CMD]; 

    execute_output_redirect_cmd("ps -f", "brandnewfile.txt");
    
    while (running !=0){

        /*read user input into buffer*/
    
        printf("%welcome to smartshell!! \n", prompt_color);
        fgets(buffer, MAX_CMD, stdin);

        char * pipe;
        pipe = strchr(buffer, '|');

        char * input; 
        input = strchr(buffer, '<');

        char * output; 
        output = strchr(buffer, '>');

        
         /*if user enters quit, end the loop*/
        if (strncmp(buffer, "quit", 4) == 0){
            printf("goodbye");
            running = 0;
        }


       
        else if (pipe != NULL) {
            printf("pipe found \n"); 
            //printf("%s \n", pipe);
            //printf("%c \n", pipe);
            //printf("%d \n", pipe);

            char * cmd_one; 
            char * cmd_two;

            *pipe = '\0' ;
            cmd_one = buffer ; 
            cmd_two = pipe+1;

            printf("%s \n", cmd_one);
            printf("%s \n", cmd_two);

            execute_piped_cmd(cmd_one, cmd_two);


        }

        else if (input != NULL){
            printf("input redirect \n"); 

            char * input_cmd; 
            char * input_file; 

            *input = '\0'; 
            input_cmd = buffer; 
            input_file = input +2; 

            printf("file: [%s] \n", input_file);

            char * newline;
            newline = strchr(input_file, '\n');
            printf("newline: %c \n", *newline);

            *newline = '\0';

            printf("cmd: [%s] \n", input_cmd);
            printf("file: [%s] \n", input_file);

            execute_input_redirect_cmd(input_cmd, input_file);


        }

        else if (output != NULL){
            printf("ouput redirect \n"); 

            char * output_cmd; 
            char * output_file; 

            *output = '\0'; 
            output_cmd = buffer; 
            output_file = output +2; 

            printf("cmd: %s \n", output_cmd);
            printf("file: %s \n", output_file);

            execute_output_redirect_cmd(output_cmd, output_file);

        }
       

        else if (strncmp(buffer, "mem", 3)==0){
            execute_cmd("free -m -t");
        }

        else if (strncmp(buffer, "disk", 4)==0){
            execute_cmd("df -h /dev/sda1");
        }

       

        else {
            execute_cmd(buffer);
          
        }

    }
    
    /* reset buffer*/
    int i; 
    for (i < MAX_CMD; i=0; i ++){
        buffer[i] = '\0';
    }

    /*reset pointers*/
    char * pipe = NULL; 
    char * input = NULL; 
    char * output = NULL;


    
}
