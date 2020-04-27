#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <pwd.h>


#define PROJECT_ID 'S'
#define SEM_AMOUNT 5


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



char* gethomedir(){
    uid_t uid = getuid();
    struct passwd* pw = getpwuid(uid);
    return pw->pw_dir;
}

key_t get_shared_key(){
    char* homedir = gethomedir();
    return ftok(homedir, PROJECT_ID);
}


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