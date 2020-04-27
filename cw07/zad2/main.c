#include <stdio.h>

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "header.h"

sem_t* sem1, *sem2, *sem3, *sem4, *sem5;
int shmd;
struct shared_memory* sh_mem;



void init_shared_memory(){
    for(int i=0; i < ORDERS_AMOUNT; i++){
        sh_mem->orders[i].status = ST_FREE;
    }
}

sem_t* create_sem(const char* name, int init_val){
    sem_t* sem;
    if((sem = sem_open(name, O_RDWR | O_CREAT | O_EXCL, 0700, init_val)) == SEM_FAILED){
        printf("ERROR: sem_open\n");
        exit(1);
    }
    return sem;
}

void create_sem_shm(){
    // open semaphores
    sem1 = create_sem(SEM1_NAME, 0);
    sem2 = create_sem(SEM2_NAME, 0);
    sem3 = create_sem(SEM3_NAME, 1);
    sem4 = create_sem(SEM4_NAME, 1);
    sem5 = create_sem(SEM5_NAME, 1);

    // open shared memory
    if((shmd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0700)) == -1){
        printf("ERROR: shm_open\n");
        exit(1);
    }
    if(ftruncate(shmd, sizeof(struct shared_memory)) == -1){
        printf("ERROR: ftruncate\n");
        exit(1);
    }
    if((sh_mem = mmap(NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0)) == (void*)-1){
        printf("ERROR: mmap\n");
        exit(1);
    }

    init_shared_memory();
}

void close_sem_shm(){
    sem_close(sem1);
    sem_close(sem2);
    sem_close(sem3);
    sem_close(sem4);
    sem_close(sem5);
    sem_unlink(SEM1_NAME);
    sem_unlink(SEM2_NAME);
    sem_unlink(SEM3_NAME);
    sem_unlink(SEM4_NAME);
    sem_unlink(SEM5_NAME);

    munmap(sh_mem, sizeof(struct shared_memory));
    shm_unlink(SHM_NAME);
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