#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>

#define OPT_IGNORE 0
#define OPT_HANDLER 1
#define OPT_MASK 2
#define OPT_PENDING 3


void pending(){
    sigset_t set;
    sigpending(&set);
    int ismember = sigismember(&set, SIGUSR1);
    if(ismember == 1)
        printf("SIGUSR1 is pending\n");
    else if(ismember == 0)
        printf("SIGUSR1 is not pending\n");
}


int main(int argc, char** argv){

    printf("CHILD:\n");

    if(argc < 2){
        printf("Too few arguments\n");
        exit(1);
    }
    
    // check option
    int option = atoi(argv[1]);
    /*
    if(option == OPT_IGNORE)
        printf("option = ignore\n");
    else if(option == OPT_HANDLER)
        printf("option = handler\n");
    else if(option == OPT_MASK)
        printf("option = mask\n");
    else if(option == OPT_PENDING)
        printf("option = pending\n");
    else
        printf("Wrong option\n");
    */
    
    // signal
    if(option == OPT_PENDING)
        pending();
    else{
        raise(SIGUSR1);
    }

    return 0;
}