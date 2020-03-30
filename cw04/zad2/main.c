#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define OPT_IGNORE 0
#define OPT_HANDLER 1
#define OPT_MASK 2
#define OPT_PENDING 3

void sigusr1_handler(int signum){
    printf("Odebrano sygnał SIGUSR (%d)\n", signum);
}


void ignore(){
    if(signal(SIGUSR1, SIG_IGN) == SIG_ERR){
        printf("ERROR: signal ignore\n");
        exit(1);
    }
}
void handler(){
    if(signal(SIGUSR1, sigusr1_handler) == SIG_ERR){
        printf("ERROR: signal handler\n");
        exit(1);
    }
}
void mask(){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    
    if(sigprocmask(SIG_SETMASK, &mask, NULL) < 0){
        printf("ERROR: sigprocmask\n");
        exit(1);
    }
}
void pending(){
    sigset_t set;
    sigpending(&set);
    int ismember = sigismember(&set, SIGUSR1);
    if(ismember == 1)
        printf("SIGUSR1 is pending\n");
    else if(ismember == 0)
        printf("SIGUSR1 is not pending\n");
}



void start_fork(int option){
    if(option == OPT_IGNORE)
        ignore();
    else if(option == OPT_HANDLER)
        handler();
    else if(option == OPT_MASK)
        mask();
    else if(option == OPT_PENDING)
        mask();
    
    printf("PARENT:\n");
    raise(SIGUSR1);
    if(option == OPT_PENDING)
        pending();

    // creating new process
    pid_t pid = fork();
    if(pid < 0){
        printf("ERROR: fork\n");
        exit(1);
    }
    if(pid == 0){ // child
        printf("CHILD:\n");
        if(option == OPT_PENDING)
            pending();
        else{
            raise(SIGUSR1);
        }
        exit(0);
    }
    else{
        wait(NULL);
    }
}

void start_exec(int option){
    if(option == OPT_IGNORE)
        ignore();
    else if(option == OPT_MASK)
        mask();
    else if(option == OPT_PENDING)
        mask();
    
    printf("PARENT:\n");
    raise(SIGUSR1);
    if(option == OPT_PENDING)
        pending();

    // creating new process
    pid_t pid = fork();
    if(pid < 0){
        printf("ERROR: fork\n");
        exit(1);
    }
    if(pid == 0){ // child
        
        char option_str[3];
        sprintf(option_str, "%d", option);

        execl("./main_exec", "main_exec", option_str, NULL);
        exit(1);
    }
    else{
        wait(NULL);
    }
}




void help(){
    printf("main mode option\n");
    printf("mode:\n");
    printf("\tfork - ");
    printf("options: ignore/handler/mask/pending\n");
    printf("\texec - ");
    printf("options: ignore/mask/pending\n");
}

int main(int argc, char** argv){

    if(argc < 3){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    const char* mode = argv[1];
    const char* option_str = argv[2];

    int option;

    if(strcmp(option_str, "ignore") == 0)
        option = OPT_IGNORE;
    else if(strcmp(option_str, "handler") == 0)
        option = OPT_HANDLER;
    else if(strcmp(option_str, "mask") == 0)
        option = OPT_MASK;
    else if(strcmp(option_str, "pending") == 0)
        option = OPT_PENDING;
    else{
        printf("Wrong option\n");
        return -1;
    }

    if(strcmp(mode, "fork") == 0){
        start_fork(option);
    }
    else if(strcmp(mode, "exec") == 0){
        if(option == OPT_HANDLER){
            printf("Wrong mode for exec\n");
            return -1;
        }
        
        start_exec(option);
    }

    return 0;
}