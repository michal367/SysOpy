#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <mqueue.h>
#include <sys/signal.h>
#include <poll.h>

#include "header.h"


// globals
char qname[10];
int msgqid = -1;
int server_msgqid = -1;

int id = -1; // id of user
int user_mq_on_server = -1;

mqd_t user2_mq = -1; // mq of connected on chat user
char user2_qname[10];
int user2_id;



int get_server_msgqid(){
    int server_msgqid;
    if((server_msgqid = mq_open(SERVER_NAME, O_WRONLY)) == -1){
        printf("\nCannot connect to server\n\n");
        exit(1);
    }
    return server_msgqid;
}

void connect_to_server(const char* name){
    // send key to server
    //server_msgqid = get_server_msgqid();
    struct message msg;
    msg.type = INIT;
    msg.id = 0;
    strcpy(msg.text, qname);
    send_msg(server_msgqid, &msg);

    // get id from server
    receive_msg(msgqid, &msg);
    if(msg.type == INIT){
        printf(msg.text);
        id = msg.id;
        user_mq_on_server = msg.mq;
        if(id == -1)
            exit(1);
    }
}



void list(){
    struct message msg;
    msg.type = LIST;
    msg.id = id;
    msg.mq = user_mq_on_server;
    send_msg(server_msgqid, &msg);
}

void connect(const char* str_id){
    if(user2_mq == -1){
        int id_to_connect = atoi(str_id);

        // error when not positive or not number
        // (if not number atoi returns 0)
        if(id_to_connect <= 0){ 
            printf("id has to be positive number\n");
            return;
        }

        if(id_to_connect == id){
            printf("You cannot connect to yourself\n");
            return;
        }
        
        struct message msg;
        msg.type = CONNECT;
        msg.id = id;
        msg.mq = user_mq_on_server;
        strcpy(msg.text, str_id);
        send_msg(server_msgqid, &msg);
    }
    else{
        printf("You are connected to an user already\n");
        printf("If you want to connect to another user disconnect first\n");
    }
}

void disconnect(){
    if(user2_mq != -1){
        struct message msg;
        msg.type = DISCONNECT;
        msg.id = id;
        msg.mq = user_mq_on_server;
        sprintf(msg.text, "%d", user2_id);
        send_msg(server_msgqid, &msg);
    }
    else{
        printf("You are not connected to anyone\n");
    }
}

void send_message(const char* text){
    if(user2_mq != -1){
        struct message msg;
        msg.type = MESSAGE;
        msg.id = id;
        msg.mq = user_mq_on_server;
        sprintf(msg.text, "OTHER: %s", text);

        send_msg(user2_mq, &msg);
    }
}




void exit_disconnect(){
    if(id != -1 && user2_mq != -1){
        disconnect();
        close_msgq(user2_mq);
        struct message msg;
        unsigned type = DISCONNECT;
        receive_msg_type(msgqid, &type, &msg);
        printf("%s\n", msg.text);
    }
}

void exit_handler()
{
    if(id != -1 && server_msgqid != -1){
        struct message msg;
        msg.type = STOP;
        msg.id = id;
        msg.mq = user_mq_on_server;
        send_msg(server_msgqid, &msg);
    }

    printf("Closing msg queue\n");
    if(msgqid != -1){
        close_msgq(msgqid);
        remove_msgq(qname);

        close_msgq(server_msgqid);
    }
}
void sigint_handler(int sign){
    printf("\nSIGINT received\n");
    exit_disconnect();
    exit(EXIT_SUCCESS); // before closing execute function passed to atexit
}



void help(){
    printf("\n");
    printf("COMMANDS:\n");
    printf("LIST - lists users connected to server\n");
    printf("CONNECT user_id - connects you to chat with user with id: user_id\n");
    printf("DISCONNECT - disconnects you from chat with user\n");
    printf("STOP - disconnects you from server\n");
    printf("\n");
}




int main(){
    printf("CLIENT\n\n");

    // adding exit handlers
    atexit(exit_handler);
    if(signal(SIGINT, sigint_handler) == SIG_ERR){
        printf("ERROR: signal\n");
        return -1;
    }

    printf("MSGQ DATA:\n");
    // getting qname
    int pid = (int)getpid();
    sprintf(qname, "/%d", pid);
    printf("name: %s\n", qname);
    

    // create msgqueue
    server_msgqid = get_server_msgqid();

    msgqid = create_msgq(qname);
    printf("mq: %d\n", msgqid);

    help();
    printf("\n================\n\n");


    connect_to_server(qname);


    struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI};
    int timeout = 500; // ms

    char text[MSG_SIZE];

    while(1){
        // wait timeout ms for input
        if(poll(&mypoll, 1, timeout)){
            fgets(text, sizeof(text), stdin);
            
            if(strcmp(text, "LIST\n") == 0){
                list();
            }
            else if(starts_with(text, "CONNECT") == 1){
                char* user2_str_id = text + strlen("CONNECT") + 1;
                connect(user2_str_id);
            }
            else if(strcmp(text, "DISCONNECT\n") == 0){
                disconnect();
            }
            else if(strcmp(text, "STOP\n") == 0){
                exit_disconnect();
                exit(0);
            }
            else{ // MESSAGE
                send_message(text);
            }
        }

        // check if there is a message in msgqueue
        struct message msg;
        if(receive_msg_time(msgqid, &msg) != -1){
            if(msg.type == STOP){ // server stopped
                printf("%s\n", msg.text);
                exit(0); // atexit sends STOP to server and closes msgq
            }
            else if(msg.type == DISCONNECT){
                printf("\n%s\n", msg.text);
                close_msgq(user2_mq);
                user2_mq = -1;
                printf("======= END OF CHAT =======\n\n");
            }
            else if(msg.type == CONNECT){
                if(msg.mq != -1){ // connected
                    strcpy(user2_qname, msg.text);
                    printf("other user qname: %s\n", user2_qname);

                    user2_mq = get_msgq(user2_qname);
                    user2_id = msg.mq;

                    printf("Connected to user %d\n\n", user2_id);
                    printf("======= START OF CHAT =======\n");
                }
                else{
                    printf("%s\n", msg.text);
                }
            }
            else if(msg.type == MESSAGE){
                printf(msg.text);
            }
            else{
                printf("%s\n", msg.text);
            }
        }
    }

    printf("\n");
    // msgqueue closes in function passed to atexit
    return 0;
}