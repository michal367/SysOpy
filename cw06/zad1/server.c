#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/signal.h>


#include "header.h"
#include "users.h"


// global
int msgqid = -1;



void close_users(){
    for(int i=0; i < users.amount; i++){
        struct user* user = &(users.users[i]);
        if(user->available != STOPPED){
            struct message msg;
            msg.type = STOP;
            msg.id = 0;
            strcpy(msg.text, "Server is shutting down\n");
            int user_msgqid = get_msgq(user->key);
            send_msg(user_msgqid, &msg);
        }
    }
    for(int i=0; i < users.amount; i++){
        struct user* user = &(users.users[i]);
        if(user->available != STOPPED){
            // get stop confirmation
            struct message msg;
            receive_msg_type(msgqid, STOP, &msg);
            printf("STOP from user %d\n", msg.id);
            remove_user(&users, msg.id);
        }
    }
}


void exit_handler()
{
    printf("Server is shutting down\n");
    close_users();

    printf("Closing msg queue\n");
    if(msgqid != -1)
        close_msgq(msgqid);
}
void sigint_handler(int sign){
    printf("\nSIGINT received\n");
    exit(EXIT_SUCCESS); // before closing execute function passed to atexit
}






void init(struct message* msg){
    int user_key = msg->id;
    int user_id = add_user(&users, user_key);
    
    if(user_id != -1)
        printf("New user with key %d and id: %d\n", user_key, user_id);
    else
        printf("New user with key %d was trying to connect, but there is no free slot\n", user_key);

    int user_msgqid = get_msgq(user_key);
    struct message msg2;
    msg2.type = INIT;
    msg2.id = user_id; // send client's id to him
    if(user_id != -1)
        sprintf(msg2.text, "Connected to server with id %d\n\n", user_id);
    else
        strcpy(msg2.text, "There is no free slot\n");
    send_msg(user_msgqid, &msg2);
}

void list(struct message* msg){
    printf("LIST from user %d\n", msg->id);

    char text[MSG_SIZE];
    get_users_string(&users, msg->id, text);

    key_t user_key = getkey(&users, msg->id);
    int user_msgqid = get_msgq(user_key);

    struct message msg2;
    msg2.type = INIT;
    msg2.id = 0;
    strcpy(msg2.text, text);
    send_msg(user_msgqid, &msg2);
}

void connect(struct message* msg){
    int id_to_connect = atoi(msg->text);

    printf("CONNECT %d from user %d\n", id_to_connect, msg->id);
    
    if(check_if_available(msg->id) == 1 &&
        check_if_available(id_to_connect) == 1){

        set_user_unavailable(msg->id);
        set_user_unavailable(id_to_connect);

        key_t user_key = getkey(&users, msg->id);
        key_t user2_key = getkey(&users, id_to_connect);

        // send message to initializer of converstaion
        int user_msgqid = get_msgq(user_key);
        struct message msg2;
        msg2.type = CONNECT;
        msg2.id = user2_key; // key of connected user
        sprintf(msg2.text, "Connected to user %d\n", id_to_connect);
        send_msg(user_msgqid, &msg2);

        // send message to second user
        user_msgqid = get_msgq(user2_key);
        msg2.id = user_key; // key of connected user
        sprintf(msg2.text, "Connected to user %d\n", msg->id);
        send_msg(user_msgqid, &msg2);
    }
    else{
        key_t user_key = getkey(&users, msg->id);
        int user_msgqid = get_msgq(user_key);
        struct message msg2;
        msg2.type = CONNECT;
        msg2.id = 0;
        sprintf(msg2.text, "Cannot connect to user %d\n", id_to_connect);
        send_msg(user_msgqid, &msg2);
    }
}

void disconnect(struct message* msg){
    printf("DISCONNECT from user %d\n", msg->id);

    int user2_id = get_userid_with_key(atoi(msg->text));

    set_user_available(msg->id);
    set_user_available(user2_id);

    // send message to first user
    key_t user_key = getkey(&users, msg->id);
    int user_msgqid = get_msgq(user_key);
    struct message msg2;
    msg2.type = DISCONNECT;
    msg2.id = 0;
    sprintf(msg2.text, "Disconnected from user %d\n", user2_id);
    send_msg(user_msgqid, &msg2);

    // send message to second user
    user_key = getkey(&users, user2_id);
    user_msgqid = get_msgq(user_key);
    sprintf(msg2.text, "Disconnected from user %d\n", msg->id);
    send_msg(user_msgqid, &msg2);
}

void stop(struct message* msg){
    printf("STOP from user %d\n", msg->id);
    remove_user(&users, msg->id);
}



int main(){
    printf("SERVER\n\n");

    // adding exit handlers
    atexit(exit_handler);
    if(signal(SIGINT, sigint_handler) == SIG_ERR){
        printf("ERROR: signal\n");
        return -1;
    }


    printf("MSGQ DATA:\n");
    // getting key
    key_t key = get_server_key();
    printf("key: %d\n", key);

    // create msgqueue
    msgqid = create_msgq(key);
    printf("msgqid: %d\n", msgqid);
    printf("\n================\n\n");



    init_users(&users);

    // receive msg
    while(1){
        struct message msg;
        receive_msg_mimp(msgqid, &msg);

        if(msg.type == INIT){
            init(&msg);
        }
        else if(msg.type == LIST){
            list(&msg);
        }
        else if(msg.type == CONNECT){
            connect(&msg);
        }
        else if(msg.type == DISCONNECT){
            disconnect(&msg);
        }
        else if(msg.type == STOP){
            stop(&msg);
        }
    }

    
    printf("\n");
    // msgqueue closes in function passed to atexit
    return 0;
}