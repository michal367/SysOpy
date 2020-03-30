#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


int sig_counter = 0;


void sigusr1_handler(int sig, siginfo_t* info, void* ucontext){
    ++sig_counter;
    printf("%d. Odebrano sygnał SIGUSR1 (%d)\n", sig_counter, sig);
    
    pid_t sender_pid = info->si_pid;

    if(info->si_code == SI_USER){ // kill
        kill(sender_pid, SIGUSR1);
    }
    else if(info->si_code == SI_QUEUE){ // sigqueue
        union sigval sigval;
        sigval.sival_int = sig_counter;
        sigqueue(sender_pid, SIGUSR1, sigval);
    }
}

void sigusr2_handler(int sig, siginfo_t* info, void* ucontext){
    printf("Koniec transmisji\n");
    printf("Odebrano %d sygnałów\n", sig_counter);

    
    pid_t sender_pid = info->si_pid;

    if(info->si_code == SI_USER){ // kill
        kill(sender_pid, SIGUSR2);
    }
    else if(info->si_code == SI_QUEUE){ // sigqueue
        union sigval sigval;
        sigval.sival_int = sig_counter;
        sigqueue(sender_pid, SIGUSR2, sigval);
    }

    exit(EXIT_SUCCESS);
}


void sigrtmin_handler(int sig, siginfo_t* info, void* ucontext){
    ++sig_counter;
    printf("%d. Odebrano sygnał SIGRTMIN (%d)\n", sig_counter, sig);
    
    pid_t sender_pid = info->si_pid;
    kill(sender_pid, SIGRTMIN);
}

void sigrtmin2_handler(int sig, siginfo_t* info, void* ucontext){
    printf("Koniec transmisji\n");
    printf("Odebrano %d sygnałów\n", sig_counter);

    pid_t sender_pid = info->si_pid;
    kill(sender_pid, SIGRTMIN+1);

    exit(EXIT_SUCCESS);
}


void add_signals_handlers(int signal, void (*signal_handler)(int, siginfo_t*, void*), 
                int signal_end, void (*signal_end_handler)(int, siginfo_t*, void*)){
    
    struct sigaction act;
    act.sa_sigaction = signal_handler;
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