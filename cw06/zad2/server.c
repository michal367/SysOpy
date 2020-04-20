#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <mqueue.h>
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
            send_msg(user->mq, &msg);
        }
    }
    for(int i=0; i < users.amount; i++){
        struct user* user = &(users.users[i]);
        if(user->available != STOPPED){
            // get stop confirmation
            struct message msg;
            unsigned type = STOP;
            receive_msg_type(msgqid, &type, &msg);
            printf("STOP from user %d\n", msg.id);
            close_msgq(user->mq);
            remove_msgq(user->qname);
            remove_user(&users, msg.id);
        }
    }
}


void exit_handler(){
    printf("Server is shutting down\n");
    close_users();

    printf("Closing msg queue\n");
    if(msgqid != -1){
        close_msgq(msgqid);
        remove_msgq(SERVER_NAME);
    }
}
void sigint_handler(int sign){
    printf("\nSIGINT received\n");
    exit(EXIT_SUCCESS); // before closing execute function passed to atexit
}






void init(struct message* msg){
    int user_id = add_user(&users, -1, msg->text);
    
    if(user_id != -1)
        printf("New user with name %s and id: %d\n", msg->text, user_id);
    else
        printf("New user with name %s was trying to connect, but there is no free slot\n", msg->text);


    int user_mq = mq_open(msg->text, O_WRONLY);
    set_user_mq(user_id, user_mq);

    struct message msg2;
    msg2.type = INIT;
    msg2.id = user_id; // send client's id to him
    msg2.mq = user_mq; // send client's mq (on server) to him
    if(user_id != -1)
        sprintf(msg2.text, "Connected to server with id %d\n\n", user_id);
    else
        strcpy(msg2.text, "There is no free slot\n");
    send_msg(user_mq, &msg2);
}

void list(struct message* msg){
    printf("LIST from user %d\n", msg->id);

    char text[MSG_SIZE];
    get_users_string(&users, msg->id, text);

    struct message msg2;
    msg2.type = INIT;
    msg2.id = 0;
    strcpy(msg2.text, text);
    send_msg(msg->mq, &msg2);
}

void connect(struct message* msg){
    int id_to_connect = atoi(msg->text);

    printf("CONNECT %d from user %d\n", id_to_connect, msg->id);
    
    if(check_if_available(msg->id) == 1 &&
        check_if_available(id_to_connect) == 1){

        set_user_unavailable(msg->id);
        set_user_unavailable(id_to_connect);

        int user_mq = msg->mq;
        int user2_mq = get_user_mq(id_to_connect);

        // send message to initializer of converstaion
        struct message msg2;
        msg2.type = CONNECT;
        msg2.id = 0;
        msg2.mq = id_to_connect;
        strcpy(msg2.text, get_user_qname(id_to_connect)); // qname of connected user
        //sprintf(msg2.text, "Connected to user %d\n", id_to_connect);
        send_msg(user_mq, &msg2);

        // send message to second user
        msg2.mq = msg->id;
        strcpy(msg2.text, get_user_qname(msg->id));
        //sprintf(msg2.text, "Connected to user %d\n", msg->id);
        send_msg(user2_mq, &msg2);
    }
    else{
        struct message msg2;
        msg2.type = CONNECT;
        msg2.id = 0;
        msg2.mq = -1;
        sprintf(msg2.text, "Cannot connect to user %d\n", id_to_connect);
        send_msg(msg->mq, &msg2);
    }
}

void disconnect(struct message* msg){
    printf("DISCONNECT from user %d\n", msg->id);

    int user2_id = atoi(msg->text);

    set_user_available(msg->id);
    set_user_available(user2_id);

    int user_mq = msg->mq;
    int user2_mq = get_user_mq(user2_id);

    // send message to first user
    struct message msg2;
    msg2.type = DISCONNECT;
    msg2.id = 0;
    sprintf(msg2.text, "Disconnected from user %d\n", user2_id);
    send_msg(user_mq, &msg2);

    // send message to second user
    sprintf(msg2.text, "Disconnected from user %d\n", msg->id);
    send_msg(user2_mq, &msg2);
}

void stop(struct message* msg){
    printf("STOP from user %d\n", msg->id);
    remove_user(&users, msg->id);
    close_msgq(msg->mq);
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

    // create msgqueue
    msgqid = create_msgq(SERVER_NAME);
    printf("mq: %d\n", msgqid);
    printf("\n================\n\n");



    init_users(&users);

    // receive msg
    while(1){
        struct message msg;
        receive_msg(msgqid, &msg);

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