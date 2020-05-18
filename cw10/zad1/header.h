#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>

#define MSG_SIZE 60
#define MAX_USERS 16


struct message{
    int type;
    char msg[MSG_SIZE];
};

int MSIZE = sizeof(struct message);

#define MSG_NAME 1
#define MSG_START 2
#define MSG_MOVE 3
#define MSG_WAIT 4
#define MSG_WIN 5
#define MSG_LOST 6
#define MSG_TIE 7
#define MSG_PING 8
#define MSG_STOP 9


int send_msg(int socket, int type, const char* text){
    struct message msg;
    msg.type = type;
    strcpy(msg.msg, text);
    return send(socket, (void*)&msg, MSIZE, MSG_NOSIGNAL);
}


#endif // header