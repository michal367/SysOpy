#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


int sig_counter = 0;


void sigusr1_handler(int signum){
    ++sig_counter;
    printf("%d. Odebrano sygnał SIGUSR1 (%d)\n", sig_counter, signum);
}

void sigusr2_handler(int sig, siginfo_t* info, void* ucontext){
    printf("Koniec transmisji\n");
    printf("Odebrano %d sygnałów\n", sig_counter);

    pid_t sender_pid = info->si_pid;

    sleep(1);
    if(info->si_code == SI_USER){ // kill
        printf("Typ transmisji: kill\n");
        for(int i=0; i < sig_counter; i++)
            kill(sender_pid, SIGUSR1);
        kill(sender_pid, SIGUSR2);
    }
    else if(info->si_code == SI_QUEUE){ // sigqueue
        printf("Typ transmisji: sigqueue\n");
        union sigval sigval;
        for(int i=0; i < sig_counter; i++){
            sigval.sival_int = i;
            sigqueue(sender_pid, SIGUSR1, sigval);
        }

        sigval.sival_int = sig_counter;
        sigqueue(sender_pid, SIGUSR2, sigval);
    }

    exit(EXIT_SUCCESS);
}


void sigrtmin_handler(int signum){
    ++sig_counter;
    printf("%d. Odebrano sygnał SIGRTMIN (%d)\n", sig_counter, signum);
}

void sigrtmin2_handler(int sig, siginfo_t* info, void* ucontext){
    printf("Koniec transmisji\n");
    printf("Odebrano %d sygnałów\n", sig_counter);

    pid_t sender_pid = info->si_pid;

    printf("Typ transmisji: sigrt\n");
    for(int i=0; i < sig_counter; i++)
        kill(sender_pid, SIGRTMIN);
    kill(sender_pid, SIGRTMIN+1);

    exit(EXIT_SUCCESS);
}


void add_signals_handlers(int signal, void (*signal_handler)(int), 
                int signal_end, void (*signal_end_handler)(int, siginfo_t*, void*)){
    
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, signal_end);
    act.sa_flags = SA_SIGINFO;
    if(sigaction(signal, &act, NULL) != 0){
        printf("ERROR: sigaction\n");
        exit(1);
    }

    struct sigaction act2;
    act2.sa_sigaction = signal_end_handler;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_SIGINFO;
    if(sigaction(signal_end, &act2, NULL) != 0){
        printf("ERROR: sigaction\n");
        exit(1);
    }
}


int main(){

    // mask signals except for SIGUSR1 and SIGUSR2
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGRTMIN);
    sigdelset(&mask, SIGRTMIN+1);
    if(sigprocmask(SIG_SETMASK, &mask, NULL) < 0){
        printf("ERROR: sigprocmask\n");
        exit(1);
    }

    printf("PID: %d\n", (int)getpid());

    add_signals_handlers(SIGUSR1, sigusr1_handler, SIGUSR2, sigusr2_handler);
    add_signals_handlers(SIGRTMIN, sigrtmin_handler, SIGRTMIN+1, sigrtmin2_handler);

    while(1){
        sleep(1);
    }

    return 0;
}