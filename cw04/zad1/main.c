#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>


int paused = 0;

void sigtstp_func(int sign){
    if(paused == 0){
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
        paused = 1;

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGINT);
        sigdelset(&mask, SIGTSTP);
        // wait for signal that is not masked
        sigsuspend(&mask);
    }
    else{
        paused = 0;
    }
}

void sigint_func(int sign){
    printf("Odebrano sygnał SIGINT\n");
    exit(EXIT_SUCCESS);
}


int main(){

    if(signal(SIGINT, sigint_func) == SIG_ERR){
        printf("ERROR: signal\n");
        return -1;
    }

    struct sigaction act;
    act.sa_handler = sigtstp_func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGTSTP, &act, NULL) != 0){
        printf("ERROR: sigaction\n");
        return -1;
    }

    while(1){
        system("ls");
        sleep(1);
    }

    return 0;
}