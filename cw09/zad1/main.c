#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>


int K = 0, N = 0;
int clients_waiting = 0;
int clients_counter = 0;
pthread_t* clients_ids = NULL;
int actual_client = 0;
int barber_sleep = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int random_nr(int min, int max){
    return (rand() % (max - min + 1)) + min;
}

void* thread_barber(void* val){
    
    while(clients_counter < N || clients_waiting != 0){
        pthread_mutex_lock(&mutex);

        while(clients_waiting == 0){
            printf("Komunikat: Golibroda: idę spać\n");
            barber_sleep = 1;
            pthread_cond_wait(&cond, &mutex); // go to sleep (wait for client)
        }
        --clients_waiting;
        printf("Komunikat: Golibroda: czeka %d klientów, golę klienta %ld\n", clients_waiting, clients_ids[actual_client]);
        ++actual_client;
        pthread_cond_broadcast(&cond2); // free seat in waitingroom

        pthread_mutex_unlock(&mutex);

        sleep(random_nr(1,5)); // barber is shaving
    }
    printf("Komunikat: Golibroda: idę spać\n");

    return NULL;
}

void* thread_client(void* val){
    pthread_mutex_lock(&mutex);

    while(clients_waiting == K){
        printf("Komunikat: Zajęte; %ld\n", pthread_self());
        pthread_cond_wait(&cond2, &mutex); // wait for free seat in waitingroom
    }
    
    ++clients_waiting;
    printf("Komunikat: Poczekalnia, wolne miejsca: %d; %ld\n", K-clients_waiting, pthread_self());
    
    clients_ids[clients_counter] = pthread_self();
    ++clients_counter;
    if(barber_sleep == 1){
        printf("Komunikat: Budzę golibrodę; %ld\n", pthread_self());
        barber_sleep = 0;
        pthread_cond_broadcast(&cond); // awake barber
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}


void help(){
    printf("main K N\n");
    printf("K - amount of chairs (>= 1)\n");
    printf("N - amount of clients (>= 0)\n");
}

int main(int argc, char** argv){

    if(argc < 3){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    K = atoi(argv[1]);
    N = atoi(argv[2]);

    if(K <= 0){
        printf("Barber can't have clients with <= 0 chairs\n");
        return -1;
    }
    if(N < 0){
        printf("Barber can't have < 0 clients\n");
        return -1;
    }

    printf("Chairs: %d\n", K);
    printf("Clients: %d\n\n", N);


    pthread_t barber_tid;
    pthread_t* clients_tids = (pthread_t*)calloc(N, sizeof(pthread_t));
    clients_ids = (pthread_t*)calloc(N, sizeof(pthread_t));

    // create barber
    pthread_create(&barber_tid, NULL, thread_barber, NULL);
    // create clients
    for(int i=0; i < N; i++){
        sleep(random_nr(1,2));
        pthread_create(&clients_tids[i], NULL, thread_client, NULL);
    }

    // join clients
    for(int i=0; i < N; i++)
        pthread_join(clients_tids[i], NULL);
    // join barber
    pthread_join(barber_tid, NULL);


    free(clients_tids);
    free(clients_ids);
    return 0;
}