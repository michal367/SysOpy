#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "header.h"

key_t key;
int semid;
int shmid;
struct shared_memory* sh_mem;


void init_worker(){
    if((key = get_shared_key()) == -1){
        printf("ERROR: get_shared_key\n");
        exit(1);
    }
    // open semaphore
    if((semid = semget(key, 0, 0700)) < 0){
        printf("ERROR: semget\n");
        exit(1);
    }
    // open shared memory
    if((shmid = shmget(key, sizeof(struct shared_memory), 0700)) < 0){
        printf("ERROR: shmget\n");
        exit(1);
    }
    if((sh_mem = (struct shared_memory*)shmat(shmid, NULL, 0)) == (void*)-1){
        printf("ERROR: shmat\n");
        exit(1);
    }
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


void inc_sem(int sem_num){
    struct sembuf semb = {sem_num, 1, 0};
    semop(semid, &semb, 1);
}
void dec_sem(int sem_num){
    struct sembuf semb = {sem_num, -1, 0};
    semop(semid, &semb, 1);
}



void worker1(){
    init_worker();

    while(1){
        dec_sem(2); // block other workers1

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

        int m = semctl(semid, 0, GETVAL, NULL) + 1;
        int x = semctl(semid, 1, GETVAL, NULL);
        print_msg("WORKER1", "Dodałem liczbę", pkg_size, m, x);
        
        inc_sem(2); // unblock other workers1
        inc_sem(0); // increment worker2 job

        sleep(random_nr(1,3));
    }
}

void worker2(){
    init_worker();

    while(1){
        dec_sem(3); // block other workers2
        dec_sem(0); // wait for worker2 job
        
        int index = find_order_with_status(sh_mem->orders, ST_PREPARED);

        // worker2 job
        int new_nr = 2 * sh_mem->orders[index].package_size;
        sh_mem->orders[index].package_size = new_nr;
        sh_mem->orders[index].status = ST_PACKED;

        int m = semctl(semid, 0, GETVAL, NULL);
        int x = semctl(semid, 1, GETVAL, NULL) + 1;
        print_msg("WORKER2", "Przygotowałem zamówienie o wielkości", new_nr, m, x);

        inc_sem(3); // unblock other workers2
        inc_sem(1); // increment worker3 job

        sleep(random_nr(1,3));
    }
}

void worker3(){
    init_worker();

    while(1){
        dec_sem(4); // block other workers3
        dec_sem(1); // wait for worker3 job
        
        int index = find_order_with_status(sh_mem->orders, ST_PACKED);

        // worker3 job
        int new_nr = 3 * sh_mem->orders[index].package_size;
        sh_mem->orders[index].package_size = new_nr;
        sh_mem->orders[index].status = ST_SENT;

        int m = semctl(semid, 0, GETVAL, NULL);
        int x = semctl(semid, 1, GETVAL, NULL);
        print_msg("WORKER3", "Wysłałem zamówienie o wielkości", new_nr, m, x);

        inc_sem(4); // unblock other workers3

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