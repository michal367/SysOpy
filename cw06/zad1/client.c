#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/signal.h>
#include <poll.h>

#include "header.h"


// globals
int msgqid = -1;
int server_msgqid = -1;
int id = -1; // id of user
int user2_key = -1; // key of connected on chat user



int get_server_msgqid(){
    key_t server_key = get_server_key();

    int server_msgqid;
    if((server_msgqid = msgget(server_key, 0)) == -1) {
        printf("\nCannot connect to server\n\n");
        exit(1);
    }
    return server_msgqid;
}

void connect_to_server(key_t key){
    // send key to server
    server_msgqid = get_server_msgqid();
    struct message msg;
    msg.type = INIT;
    msg.id = key;
    send_msg(server_msgqid, &msg);

    // get id from server
    receive_msg(msgqid, &msg);
    if(msg.type == INIT){
        printf(msg.text);
        id = msg.id;
        if(id == -1)
            exit(1);
    }
}



void list(){
    struct message msg;
    msg.type = LIST;
    msg.id = id;
    send_msg(server_msgqid, &msg);
}

void connect(const char* str_id){
    if(user2_key == -1){
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
        strcpy(msg.text, str_id);
        send_msg(server_msgqid, &msg);
    }
    else{
        printf("You are connected to an user already\n");
        printf("If you want to connect to another user disconnect first\n");
    }
}

void disconnect(){
    if(user2_key != -1){
        struct message msg;
        msg.type = DISCONNECT;
        msg.id = id;
        sprintf(msg.text, "%d", user2_key);
        send_msg(server_msgqid, &msg);
    }
    else{
        printf("You are not connected to anyone\n");
    }
}

void send_message(const char* text){
    if(user2_key != -1){
        struct message msg;
        msg.type = MESSAGE;
        msg.id = id;
        sprintf(msg.text, "OTHER: %s", text);

        int user2_msgqid = get_msgq(user2_key);
        send_msg(user2_msgqid, &msg);
    }
}




void exit_disconnect(){
    if(id != -1 && user2_key != -1){
        disconnect();
        struct message msg;
        receive_msg_type(msgqid, DISCONNECT, &msg);
        printf("%s\n", msg.text);
    }
}

void exit_handler()
{
    if(id != -1 && server_msgqid != -1){
        struct message msg;
        msg.type = STOP;
        msg.id = id;
        send_msg(server_msgqid, &msg);
    }

    printf("Closing msg queue\n");
    if(msgqid != -1)
        close_msgq(msgqid);
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
    // getting key
    key_t key = (key_t)getpid();
    printf("key: %d\n", key);
    // create msgqueue
    msgqid = create_msgq(key);
    printf("msgqid: %d\n", msgqid);

    help();
    printf("\n================\n\n");


    connect_to_server(key);


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
        if(receive_msg_nowait(msgqid, &msg) != -1){
            if(msg.type == STOP){ // server stopped
                printf("%s\n", msg.text);
                exit(0); // atexit sends STOP to server and closes msgq
            }
            else if(msg.type == DISCONNECT){
                printf("\n%s\n", msg.text);
                user2_key = -1;
                printf("======= END OF CHAT =======\n\n");
            }
            else if(msg.type == CONNECT){
                printf("%s\n", msg.text);
                if(msg.id != 0){ // connected
                    user2_key = msg.id;
                    //printf("other user key: %d\n", msg.id);
                    printf("======= START OF CHAT =======\n");
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