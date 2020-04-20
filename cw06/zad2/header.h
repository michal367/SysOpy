#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <time.h>

#include <sys/ipc.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>


#define PROJECT_ID 'C'
#define SERVER_NAME "/server_mq"
#define QUEUE_SIZE 8
#define MSG_SIZE 256
#define USR_MAX_SIZE 256


// commands have importance 
// (5 - most important, 1 - least important)
#define INIT 1
#define LIST 3
#define CONNECT 2
#define DISCONNECT 4
#define STOP 5
#define MESSAGE 0


#define AVAILABLE 1
#define UNAVAILABLE 0
#define STOPPED -1


struct message{
    long type;
    int id;
    mqd_t mq;
    char text[MSG_SIZE];
};

size_t message_size = sizeof(struct message);



mqd_t create_msgq(const char* name){
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUE_SIZE;
    attr.mq_msgsize = message_size;

    mqd_t mq = mq_open(name, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
    if(mq == -1){
        printf("ERROR: mq_open name: %s\n", name);
        exit(1);
    }
    return mq;
}

mqd_t get_msgq(const char* name){
    mqd_t mq = mq_open(name, O_WRONLY);
    if(mq == -1){
        printf("ERROR: mq_open name: %s\n", name);
        exit(1);
    }
    return mq;
}


void send_msg(mqd_t mq, struct message* msg){
    if(mq_send(mq, (char*)msg, message_size, msg->type) == -1){
        printf("ERROR: mq_send to mq: %d\n", mq);
    }
}

void receive_msg(mqd_t mq, struct message* msg){
    if(mq_receive(mq, (char*)msg, message_size, NULL) == -1){
 		printf("ERROR: mq_receive at mq: %d\n", mq);
        exit(1);
    }
}
void receive_msg_type(mqd_t mq, unsigned* type, struct message* msg){
    if(mq_receive(mq, (char*)msg, message_size, type) == -1){
 		printf("ERROR: mq_receive at mq: %d\n", mq);
        exit(1);
    }
}
ssize_t receive_msg_time(mqd_t mq, struct message* msg){
    struct timespec tm;
    clock_gettime(0, &tm);
    tm.tv_nsec += 100000000; // 100 ms
    
    return mq_timedreceive(mq, (char*)msg, message_size, NULL, &tm);
}

void close_msgq(mqd_t mq){
    mq_close(mq);
}
void remove_msgq(const char* name){
    mq_unlink(name);
}



int starts_with(const char *str, const char *pre){
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}




#endif // header