#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>



void print_siginfo(const siginfo_t* info){
    printf("Signal number: %d\n", info->si_signo); // number of signal
    printf("Signal code: %d\n", info->si_code); // value indicating why this signal was sent
    printf("Sending process ID: %d\n", (int)info->si_pid); // pid of process that sent signal
    printf("Real UID of sending process: %d\n", (int)info->si_uid); // uid of process that sent signal
    printf("Exit value or signal: %d\n", info->si_status); // contains the exit status of the child or value of signal
    printf("User time consumed: %f\n", (double)info->si_utime / sysconf(_SC_CLK_TCK)); // contains the user time used by the child process
    printf("System time consumed: %f\n", (double)info->si_stime / sysconf(_SC_CLK_TCK)); // contains the system time used by the child process
    
    printf("\nSignal code: ");
    int code = info->si_code;
    if(info->si_signo == 2){ // SIGINT
        if(code == SI_USER)
            printf("USER\n");
        else if(code == SI_KERNEL)
            printf("KERNEL\n");
        else if(code == SI_QUEUE)
            printf("QUEUE\n");
        else if(code == SI_TKILL)
            printf("TKILL\n");
        else if(code == SI_TIMER)
            printf("SI_TIMER\n");
        else if(code == SI_MESGQ)
            printf("SI_MESGQ\n");
        else if(code == SI_ASYNCIO)
            printf("SI_ASYNCIO\n");
    }
    else if(info->si_signo == 17){ // SIGCHLD
        if(code == CLD_EXITED)
            printf("CLD_EXITED\n");
        else if(code == CLD_KILLED)
            printf("CLD_KILLED\n");
        else if(code == CLD_DUMPED)
            printf("CLD_DUMPED\n");
        else if(code == CLD_TRAPPED)
            printf("CLD_TRAPPED\n");
        else if(code == CLD_STOPPED)
            printf("CLD_STOPPED\n");
        else if(code == CLD_CONTINUED)
            printf("CLD_CONTINUED\n");
    }
}


void signal_action(int sig, siginfo_t* info, void* ucontext){
    printf("Signal number: %d\n", sig);

    printf("\nSignal info\n");
    print_siginfo(info);
    printf("==========================\n\n");

    // only if pressed CTRL+C then exit program
    if(info->si_code == SI_KERNEL)
        exit(0);
}



int main(){

    // setting sigaction of SIGINT and SIGCHLD
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = signal_action;

    if(sigaction(SIGINT, &act, NULL) != 0){
        printf("ERROR: sigaction\n");
        return -1;
    }
    if(sigaction(SIGCHLD, &act, NULL) != 0){
        printf("ERROR: sigaction\n");
        return -1;
    }


    // SENDING SIGNALS

    // QUEUE
    printf("QUEUE\n");
    union sigval sigval;
    sigval.sival_int = 12;
    int cur_pid = (int)getpid();
    sigqueue(cur_pid, SIGINT, sigval);
    

    
    // creating child process
    pid_t pid = fork();
    if(pid < 0){
        printf("ERROR: fork\n");
        return -1;
    }
    if(pid == 0){ // child
        // SIGINT USER
        printf("CHILD KILLS PARENT\n");
        kill(cur_pid, SIGINT);        
        
        // increase utime
        for(int i=0; i < 10000000; i++){
            time(NULL);
        }

        sleep(1); // for not having print of SIGINT and SIGCHLD at the same time
        printf("CHILD EXITS\n");
        exit(23);
    }
    wait(NULL);


    // TKILL - Thread KILL
    printf("RAISE\n");
    raise(SIGINT);


    
    while(1){
        sleep(5);
        // CTRL+C - KERNEL
        printf("Press CTRL+C to send SIGINT and kill process\n");
    }

    return 0;
}