#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "header.h"

sem_t* sem1, *sem2, *sem_w1, *sem_w2, *sem_w3;
//sem1 = NULL; sem2 = NULL; sem_w1 = NULL; sem_w2 = NULL; sem_w3 = NULL;
int shmd;
struct shared_memory* sh_mem = NULL;


sem_t* open_sem(const char* name){
    sem_t* sem;
    if((sem = sem_open(name, O_RDWR)) == SEM_FAILED){
        printf("ERROR: sem_open\n");
        exit(1);
    }
    return sem;
}

void init_worker(){
    // open semaphores
    sem1 = open_sem(SEM1_NAME);
    sem2 = open_sem(SEM2_NAME);
    sem_w1 = open_sem(SEM3_NAME);
    sem_w2 = open_sem(SEM4_NAME);
    sem_w3 = open_sem(SEM5_NAME);

    // open shared memory
    if((shmd = shm_open(SHM_NAME, O_RDWR, 0700)) == -1){
        printf("ERROR: shm_open\n");
        exit(1);
    }
    if((sh_mem = mmap(NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0)) == (void*)-1){
        printf("ERROR: mmap\n");
        exit(1);
    }
}

void close_sem_shm(){
    sem_close(sem1);
    sem_close(sem2);
    sem_close(sem_w1);
    sem_close(sem_w2);
    sem_close(sem_w3);

    munmap(sh_mem, sizeof(struct shared_memory));
}

void exit_handler(){
    close_sem_shm();
}
void sigint_handler(int sign){
    exit(EXIT_SUCCESS); // before closing execute function passed to atexit
}


void print_msg(const char* worker_str, const char* msg_start, int value, int value2, int value3){
    printf("%s: ", worker_str);
    printf("(%d %ldms) ", (int)getpid(), get_ms_time());
    printf("%s: %d. ", msg_start, value);
    printf("Liczba zamówień do przygotowania: %d. ", value2);
    printf("Liczba zamówień do wysłania: %d.\n", value3);

    FILE* file = fopen("logs","a");
    if(file != NULL){
        fprintf(file, "%s: ", worker_str);
        fprintf(file, "(%d %ldms) ", (int)getpid(), get_ms_time());
        fprintf(file, "%s: %d. ", msg_start, value);
        fprintf(file, "Liczba zamówień do przygotowania: %d. ", value2);
        fprintf(file, "Liczba zamówień do wysłania: %d.\n", value3);
        fclose(file);
    }
}

int find_order_with_status(struct Order* orders, int status){
    for(int i=0; i<ORDERS_AMOUNT; i++){
        if(orders[i].status == status)
            return i;
    }
    return -1;
}

void worker1(){
    init_worker();

    while(1){
        sem_wait(sem_w1); // block other workers1

        int index = -1;
        while(index == -1){
            index = find_order_with_status(sh_mem->orders, ST_FREE);
            if(index == -1)
                usleep(100 * 1000); // 100 ms
        }

        // worker1 job
        int pkg_size = random_nr(1, PKGSIZE_MAX);
        sh_mem->orders[index].package_size = pkg_size;
        sh_mem->orders[index].status = ST_PREPARED;

        int m, x;
        sem_getvalue(sem1, &m);
        sem_getvalue(sem2, &x);
        print_msg("WORKER1", "Dodałem liczbę", pkg_size, m+1, x);
        
        sem_post(sem_w1); // unblock other workers1
        sem_post(sem1); // increment worker2 job

        sleep(random_nr(1,3));
    }
}

void worker2(){
    init_worker();

    while(1){
        sem_wait(sem_w2); // block other workers2
        sem_wait(sem1); // wait for worker2 job
        
        int index = find_order_with_status(sh_mem->orders, ST_PREPARED);

        // worker2 job
        int new_nr = 2 * sh_mem->orders[index].package_size;
        sh_mem->orders[index].package_size = new_nr;
        sh_mem->orders[index].status = ST_PACKED;

        int m, x;
        sem_getvalue(sem1, &m);
        sem_getvalue(sem2, &x);
        print_msg("WORKER2", "Przygotowałem zamówienie o wielkości", new_nr, m, x+1);

        sem_post(sem_w2); // unblock other workers2
        sem_post(sem2); // increment worker3 job

        sleep(random_nr(1,3));
    }
}

void worker3(){
    init_worker();

    while(1){
        sem_wait(sem_w3); // block other workers3
        sem_wait(sem2); // wait for worker3 job
        
        int index = find_order_with_status(sh_mem->orders, ST_PACKED);

        // worker3 job
        int new_nr = 3 * sh_mem->orders[index].package_size;
        sh_mem->orders[index].package_size = new_nr;
        sh_mem->orders[index].status = ST_SENT;

        int m, x;
        sem_getvalue(sem1, &m);
        sem_getvalue(sem2, &x);
        print_msg("WORKER3", "Wysłałem zamówienie o wielkości", new_nr, m, x);

        sem_post(sem_w3); // unblock other workers3

        sleep(random_nr(1,3));
    }
}


void help(){
    printf("worker mode\n");
    printf("mode: 1, 2, 3\n");
}

int main(int argc, char** argv){
    srand(getpid());

    if(argc < 2){
        printf("Too few arguments\n");
        help();
        exit(1);
    }
    int mode = atoi(argv[1]);

    // adding exit handlers
    atexit(exit_handler);
    if(signal(SIGINT, sigint_handler) == SIG_ERR){
        printf("ERROR: signal\n");
        return -1;
    }

    if(mode == 1)
        worker1();
    else if(mode == 2)
        worker2();
    else if(mode == 3)
        worker3();
    else{
        printf("Wrong mode\n");
        help();
        exit(1);
    }

    return 0;
}