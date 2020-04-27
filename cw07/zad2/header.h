#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/time.h>

#define SEM1_NAME "/semaphore1"
#define SEM2_NAME "/semaphore2"
#define SEM3_NAME "/semaphore3"
#define SEM4_NAME "/semaphore4"
#define SEM5_NAME "/semaphore5"
#define SHM_NAME "/shared_memory"
#define SEM_AMOUNT 2


#define ORDERS_AMOUNT 30

#define ST_FREE 0
#define ST_PREPARED 1
#define ST_PACKED 2
#define ST_SENT 0

#define PKGSIZE_MAX 9


struct Order{
    int status;
    int package_size;
};

struct shared_memory{
    struct Order orders[ORDERS_AMOUNT];
};


int random_nr(int min, int max){
    return (rand() % (max - min + 1)) + min;
}

long get_ms_time(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // convert tv_sec & tv_usec to millisecond
    return (long)((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000);
}

#endif // header