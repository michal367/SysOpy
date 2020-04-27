#include <stdio.h>

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "header.h"

key_t key;
int semid;
int shmid;
struct shared_memory* sh_mem;



void init_shared_memory(){
    for(int i=0; i < ORDERS_AMOUNT; i++){
        sh_mem->orders[i].status = ST_FREE;
    }
}

void set_sem_value(int semid, int semnum, int value){
    union semun {
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
    } semun;
    semun.val = value;
    semctl(semid, semnum, SETVAL, semun);
}


void create_sem_shm(){
    if((key = get_shared_key()) == -1){
        printf("ERROR: get_shared_key\n");
        exit(1);
    }
    // open semaphore
    if((semid = semget(key, SEM_AMOUNT, 0700 | IPC_CREAT)) < 0){
        printf("ERROR: semget\n");
        exit(1);
    }
    set_sem_value(semid, 2, 1);
    set_sem_value(semid, 3, 1);
    set_sem_value(semid, 4, 1);

    // open shared memory
    if((shmid = shmget(key, sizeof(struct shared_memory), 0700 | IPC_CREAT)) < 0){
        printf("ERROR: shmget\n");
        exit(1);
    }
    if((sh_mem = (struct shared_memory*)shmat(shmid, NULL, 0)) == (void*)-1){
        printf("ERROR: shmat\n");
        exit(1);
    }

    init_shared_memory();
}

void close_sem_shm(){
    semctl(semid, -1, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
}


void exit_handler(){
    printf("\nClosing semaphores and shared memory\n");
    close_sem_shm();
}
void sigint_handler(int sign){
    printf("\nSIGINT received\n");
    exit(EXIT_SUCCESS); // before closing execute function passed to atexit
}


void start_process(char* const args[]){
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



int main(int argc, char** argv){

    int worker1_amount, worker2_amount, worker3_amount;
    if(argc >= 4)
        worker3_amount = atoi(argv[3]);
    else
        worker3_amount = 1;

    if(argc >= 3)
        worker2_amount = atoi(argv[2]);
    else
        worker2_amount = 1;
    
    if(argc >= 2)
        worker1_amount = atoi(argv[1]);
    else
        worker1_amount = 1;
    

    printf("worker1 amount: %d\n", worker1_amount);
    printf("worker2 amount: %d\n", worker2_amount);
    printf("worker3 amount: %d\n", worker3_amount);
    printf("if you want another workers amount use args of this program\n");
    printf("======================\n\n");


    // adding exit handlers
    atexit(exit_handler);
    if(signal(SIGINT, sigint_handler) == SIG_ERR){
        printf("ERROR: signal\n");
        return -1;
    }

    create_sem_shm();

    // create/clear file logs
    FILE* file = fopen("logs","w");
    if(file != NULL)
        fclose(file);
    

    char worker[] = "./worker";
    char* worker1_args[] = {worker, "1", NULL};
    char* worker2_args[] = {worker, "2", NULL};
    char* worker3_args[] = {worker, "3", NULL};
    

    // start workers
    for(int i=0; i<worker1_amount; i++)
        start_process(worker1_args);
    for(int i=0; i<worker2_amount; i++)
        start_process(worker2_args);
    for(int i=0; i<worker3_amount; i++)
        start_process(worker3_args);

    // wait for all workers
    int workers_amount = worker1_amount + worker2_amount + worker3_amount;
    for(int i=0; i < workers_amount; i++){
        wait(NULL);
    }

    return 0;
}