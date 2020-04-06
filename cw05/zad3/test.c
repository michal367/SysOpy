#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


void start_process(char** args){
    pid_t pid = fork();
    if(pid < 0){
        printf("ERROR: fork\n");
        exit(1);
    }
    else if(pid == 0){
        execv(args[0], args);
        exit(1);
    }
}


#define producers_amount 5

int main(){

    char producer[] = "./producer";
    char consumer[] = "./consumer";

    char fifo[] = "fifo";
    //char in[] = "in/in1";
    char out[] = "out";
    char N[] = "4";
    
    char* consumer_args[] = {consumer, fifo, out, N, NULL};
    char* producer_args[producers_amount][5] = {
        {producer, fifo, "in/in1", N, NULL},
        {producer, fifo, "in/in2", N, NULL},
        {producer, fifo, "in/in3", N, NULL},
        {producer, fifo, "in/in4", N, NULL},
        {producer, fifo, "in/in5", N, NULL}
    };


    // remove existing fifo
    unlink("fifo");

    // create new fifo
    int mode = 0666; // octal number
    umask(0); // real_mode = (mode & ~umask)
    mkfifo("fifo", mode);

    // start consumer
    start_process(consumer_args);

    // start producers
    for(int i=0; i < producers_amount; i++){
        start_process(producer_args[i]);
    }

    // wait for all processes
    for(int i=0; i < producers_amount + 1; i++)
        wait(NULL);



    unlink("fifo");

    return 0;
}