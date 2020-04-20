#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>


#define PROJECT_ID 'C'
#define MSG_SIZE 256
#define USR_MAX_SIZE 256


// commands have importance 
// (1 - most important, 5 - least important)
#define INIT 5
#define LIST 3
#define CONNECT 4
#define DISCONNECT 2
#define STOP 1
#define MESSAGE 6
#define RCV_MOSTIMPORTANT -6 // use in msgrcv as type to get most important msg in queue (from 1 to 5)

#define AVAILABLE 1
#define UNAVAILABLE 0
#define STOPPED -1


struct message{
    long type;
    int id;
    char text[MSG_SIZE];
};

size_t message_size = sizeof(struct message) - sizeof(long);



char* gethomedir(){
    uid_t uid = getuid();
    struct passwd* pw = getpwuid(uid);
    return pw->pw_dir;
}

key_t get_server_key(){
    char* homedir = gethomedir();
    return ftok(homedir, PROJECT_ID);
}


int create_msgq(key_t key){
    int msgqid = msgget(key, IPC_CREAT | IPC_EXCL | 0660);
    if(msgqid == -1){
        printf("ERROR: msgget key: %d\n", key);
        exit(1);
    }
    return msgqid;
}

int get_msgq(key_t key){
    int msgqid = msgget(key, 0);
    if(msgqid == -1){
        printf("ERROR: msgget key: %d\n", key);
        exit(1);
    }
    return msgqid;
}


void send_msg(int msgqid, struct message* msg){
    if(msgsnd(msgqid, (void*)msg, message_size, IPC_NOWAIT) == -1){
        printf("ERROR: msgsnd to msgqid: %d\n", msgqid);
    }
}

void receive_msg(int msgqid, struct message* msg){
    if(msgrcv(msgqid, (void*)msg, message_size, 0, 0) == -1){
 		printf("ERROR: msgrcv at msgqid: %d\n", msgqid);
        exit(1);
    }
}
int receive_msg_nowait(int msgqid, struct message* msg){
    return msgrcv(msgqid, (void*)msg, message_size, 0, IPC_NOWAIT);
}
void receive_msg_mimp(int msgqid, struct message* msg){
    if(msgrcv(msgqid, (void*)msg, message_size, RCV_MOSTIMPORTANT, 0) == -1){
 		printf("ERROR: msgrcv at msgqid: %d\n", msgqid);
        exit(1);
    }
}
void receive_msg_type(int msgqid, long type, struct message* msg){
    if(msgrcv(msgqid, (void*)msg, message_size, type, 0) == -1){
 		printf("ERROR: msgrcv at msgqid: %d\n", msgqid);
        exit(1);
    }
}

void close_msgq(int msgqid){
    msgctl(msgqid, IPC_RMID, NULL);
}



void show_msq(int msgqid){
    struct msqid_ds msq;
    msgctl(msgqid, IPC_STAT, &msq);
    printf("\nMSQID_DS:\n");
    // msg_perm
    // msg_first, msg_last
    // msg_stime, msg_rtime, msg_ctime
    // wwait, rwait
    printf("msg_cbytes: %ld\n", msq.__msg_cbytes);
    printf("msg_qnum: %ld\n", msq.msg_qnum);
    printf("msg_qbytes: %ld\n", msq.msg_qbytes);
    // msg_lspid, msg_lrpid
    printf("\n");
}


int starts_with(const char *str, const char *pre){
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}




#endif // header