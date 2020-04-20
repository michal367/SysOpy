#ifndef USERS_H
#define USERS_H

#include <string.h>

#include <sys/ipc.h>
#include <mqueue.h>

#include "header.h"

struct user{
    int id;
    mqd_t mq;
    char qname[10];
    int available;
};
struct users{
    struct user users[USR_MAX_SIZE];
    int amount;
};


// global
struct users users;




void init_users(struct users* users){
    users->amount = 0;
}
int add_user(struct users* users, mqd_t mq, const char* qname){
    for(int i=0; i<users->amount; i++){
        struct user* user = &(users->users[i]);
        if(user->available == STOPPED){
            user->mq = mq;
            strcpy(user->qname, qname);
            user->available = AVAILABLE;
            return user->id;
        }
    }

    if(users->amount < USR_MAX_SIZE){
        struct user* user = &(users->users[users->amount]);
        user->id = users->amount + 1; // ids: 1,2,3,...
        user->mq = mq;
        strcpy(user->qname, qname);
        user->available = AVAILABLE;
        users->amount++;
        return user->id;
    }
    else
        return -1;
}
void remove_user(struct users* users, int id){
    users->users[id-1].available = STOPPED;
}



struct user* get_user(int id){
    return &(users.users[id-1]);
}
mqd_t get_user_mq(int id){
    return (get_user(id)->mq);
}
char* get_user_qname(int id){
    return (get_user(id)->qname);
}
int get_userid_with_mq(mqd_t mq){
    for(int i=0; i < users.amount; i++){
        if(users.users[i].mq == mq)
            return users.users[i].id;
    }
    return -1;
}




int check_if_available(int id){
    return (get_user(id)->available == AVAILABLE);
}
void set_user_unavailable(int id){
    get_user(id)->available = UNAVAILABLE;
}
void set_user_available(int id){
    get_user(id)->available = AVAILABLE;
}
void set_user_mq(int id, mqd_t mq){
    get_user(id)->mq = mq;
}





void get_users_string(struct users* users, int id, char* user_str){
    strcpy(user_str, ""); // clear string
    for(int i=0; i < users->amount; i++){
        struct user* user = &(users->users[i]);

        if(user->available != STOPPED){
            char str[10];
            strcat(user_str, "id: ");
            sprintf(str, "%d", user->id);
            strcat(user_str, str);

            strcat(user_str, "    ");
            
            if(user->available == AVAILABLE)
                strcat(user_str, "AVAILABLE");
            else if(user->available == UNAVAILABLE)
                strcat(user_str, "UNAVAILABLE");
            
            if(id == user->id){
                strcat(user_str, "    YOU");
            }
            strcat(user_str, "\n");
        }
    }
}



#endif // users