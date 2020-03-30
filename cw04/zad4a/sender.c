#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int sig_amount;
int sig_counter = 0;


void send_kill(pid_t catcher_pid, int sig_amount){
    for(int i=0; i < sig_amount; i++)
        kill(catcher_pid, SIGUSR1);
    kill(catcher_pid, SIGUSR2);
}

void send_sigqueue(pid_t catcher_pid, int sig_amount){
    union sigval sigval;
    for(int i=0; i < sig_amount; i++)
        sigqueue(catcher_pid, SIGUSR1, sigval);
    sigqueue(catcher_pid, SIGUSR2, sigval);
}

void send_sigrt(pid_t catcher_pid, int sig_amount){
    for(int i=0; i < sig_amount; i++)
        kill(catcher_pid, SIGRTMIN);
    kill(catcher_pid, SIGRTMIN+1);
}


void sigusr1_handler(int sig, siginfo_t* info, void* ucontext){
    ++sig_counter;
    if(info->si_code == SI_QUEUE){
        int value = info->si_value.sival_int;
        printf("%d. Odebrano sygnał SIGUSR1 (%d) w trybie QUEUE z wartością: %d\n", sig_counter, sig, value);
    }
    else{
        printf("%d. Odebrano sygnał SIGUSR1 (%d)\n", sig_counter, sig);
    }
}
void sigusr2_handler(int sig, siginfo_t* info, void* ucontext){
    printf("Koniec transmisji\n");
    printf("Odebrano %d sygnałów\n", sig_counter);

    if(info->si_code == SI_QUEUE){
        int value = info->si_value.sival_int;
        printf("catcher wysłał %d sygnałów w trybie QUEUE\n", value);
    }
    printf("Powinien odebrać %d\n", sig_amount);
    exit(EXIT_SUCCESS);
}


void sigrtmin_handler(int signum){
    ++sig_counter;
    printf("%d. Odebrano sygnał SIGRTMIN (%d)\n", sig_counter, signum);
}
void sigrtmin2_handler(int signum){
    printf("Koniec transmisji\n");
    printf("Odebrano %d sygnałów\n", sig_counter);
    printf("Powinien odebrać %d\n", sig_amount);
    exit(EXIT_SUCCESS);
}


void add_usr_signals_handlers(){
    struct sigaction act;
    act.sa_sigaction = sigusr1_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR2);
    act.sa_flags = SA_SIGINFO;
    if(sigaction(SIGUSR1, &act, NULL) != 0){
        printf("ERROR: sigaction\n");
        exit(1);
    }
    
    struct sigaction act2;
    act2.sa_sigaction = sigusr2_handler;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_SIGINFO;
    if(sigaction(SIGUSR2, &act2, NULL) != 0){
        printf("ERROR: sigaction\n");
        exit(1);
    }
}

void add_rt_signals_handlers(){
    struct sigaction act3;
    act3.sa_handler = sigrtmin_handler;
    sigemptyset(&act3.sa_mask);
    sigaddset(&act3.sa_mask, SIGRTMIN+1);
    act3.sa_flags = 0;
    if(sigaction(SIGRTMIN, &act3, NULL) != 0){
        printf("ERROR: sigaction\n");
        exit(1);
    }

    struct sigaction act4;
    act4.sa_handler = sigrtmin2_handler;
    sigemptyset(&act4.sa_mask);
    act4.sa_flags = 0;
    if(sigaction(SIGRTMIN+1, &act4, NULL) != 0){
        printf("ERROR: sigaction\n");
        exit(1);
    }
}



void help(){
    printf("sender catcher_pid sig_amount mode\n");
    printf("catcher_pid - pid of catcher process\n");
    printf("sig_amount - amount of signals to send to catcher\n");
    printf("mode - kill/sigqueue/sigrt\n");
}


int main(int argc, char** argv){

    if(argc < 4){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    add_usr_signals_handlers();
    add_rt_signals_handlers();


    int catcher_pid = atoi(argv[1]);
    if(catcher_pid <= 1){
        printf("catcher_pid must be > 1\n");
        return -1;
    }
    sig_amount = atoi(argv[2]);
    if(sig_amount <= 0){
        printf("sig_amount must be >= 1\n");
    }


    char* mode = argv[3];
    if(strcmp(mode, "kill") == 0)
        send_kill(catcher_pid, sig_amount);
    else if(strcmp(mode, "sigqueue") == 0)
        send_sigqueue(catcher_pid, sig_amount);
    else if(strcmp(mode, "sigrt") == 0)
        send_sigrt(catcher_pid, sig_amount);
    else{
        printf("Wrong mode\n");
        help();
        return -1;
    }

    printf("Wysłano %d sygnałów\n", sig_amount);


    while(1){
        sleep(1);
    }

    return 0;
}