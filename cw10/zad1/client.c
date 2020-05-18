#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/signal.h>

#include "header.h"


int server_socket;

char board[9] = {' ', ' ', ' ',
                 ' ', ' ', ' ',
                 ' ', ' ', ' '};
char sign, sign2;


void connect_to_server_unix(const char* address){
    struct sockaddr_un unix_address;
    unix_address.sun_family = AF_UNIX;
    strcpy(unix_address.sun_path, address);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1){
        printf("ERROR: socket\n");
        exit(1);
    }
    if(connect(server_socket, (struct sockaddr*)&unix_address, sizeof(unix_address)) == -1){
        printf("ERROR: Cannot connect to server\n");
        exit(1);
    }
}

void connect_to_server_inet(char* address){

    char* c = address;
    while(*c != ':')
        c++;
    *c = '\0';
    int port = atoi(++c);
    //printf("%s %d\n", address, port);

    struct sockaddr_in inet_address;
    inet_address.sin_family = AF_INET;
    inet_address.sin_port = htons(port);
    if(inet_pton(AF_INET, address, &inet_address.sin_addr) <= 0){
        printf("Bad address\n");
        exit(1);
    }
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        printf("ERROR: socket\n");
        exit(1);
    }
    if(connect(server_socket, (struct sockaddr*)&inet_address, sizeof(inet_address)) == -1){
        printf("ERROR: Cannot connect to server\n");
        exit(1);
    }
}


void disconnect(){
    send_msg(server_socket, MSG_STOP, "");
    close(server_socket);
}


void draw_board(){
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[6], board[7], board[8]);
    printf("\n");
}
void draw_numbered_board(){
    printf(" 1 | 2 | 3 \n");
    printf("---+---+---\n");
    printf(" 4 | 5 | 6 \n");
    printf("---+---+---\n");
    printf(" 7 | 8 | 9 \n");
    printf("\n");
}

int game_over(){
    if(board[0] == board[1] && board[1] == board[2]){
        if(board[0] == sign) return 1;
        else if(board[0] == sign2) return -1;
    }
    if(board[3] == board[4] && board[4] == board[5]){
        if(board[3] == sign) return 1;
        else if(board[3] == sign2) return -1;
    }
    if(board[6] == board[7] && board[7] == board[8]){
        if(board[6] == sign) return 1;
        else if(board[6] == sign2) return -1;
    }
    if(board[0] == board[3] && board[3] == board[6]){
        if(board[0] == sign) return 1;
        else if(board[0] == sign2) return -1;
    }
    if(board[1] == board[4] && board[4] == board[7]){
        if(board[1] == sign) return 1;
        else if(board[1] == sign2) return -1;
    }
    if(board[2] == board[5] && board[5] == board[8]){
        if(board[2] == sign) return 1;
        else if(board[2] == sign2) return -1;
    }
    if(board[0] == board[4] && board[4] == board[8]){
        if(board[0] == sign) return 1;
        else if(board[0] == sign2) return -1;
    }
    if(board[2] == board[4] && board[4] == board[6]){
        if(board[2] == sign) return 1;
        else if(board[2] == sign2) return -1;
    }

    for(int i=0; i < 9; i++){
        if(board[i] != 'X' && board[i] != 'O')
            return 0;
    }
    return 2;
}


void start(const char* name){
    // struct message msg_name;
    // msg_name.type = MSG_NAME;
    // strcpy(msg_name.msg, name);
    // send(server_socket, (void*)&msg_name, MSIZE, 0);
    send_msg(server_socket, MSG_NAME, name);
    
    while(1){
        struct message msg;
        int recv_size = recv(server_socket, (void*)&msg, MSIZE, 0);
        if(recv_size == 0){
            printf("Lost connection\n");
            return;
        }

        //printf("type: %d\n", msg.type);
        if(msg.type == MSG_NAME){
            printf("%s\n", msg.msg);
        }
        else if(msg.type == MSG_START){
            printf("%s\n", msg.msg);
            printf("You have 15 sec per move. If you don't move game is over.\n");

            sign = msg.msg[strlen(msg.msg)-1];
            if(sign == 'X')
                sign2 = 'O';
            else if(sign == 'O')
                sign2 = 'X';
            printf("You are %c\n", sign);

            draw_numbered_board();
        }
        else if(msg.type == MSG_MOVE){
            if(strcmp(msg.msg, "") != 0){ // recv pos
                printf("Opponent made a move at %s\n", msg.msg);
                int pos = atoi(msg.msg);
                board[pos-1] = sign2;
                draw_board();
            }
            printf("Your move\n");

            // input position
            int move_pos;
            do{
                if(scanf("%d", &move_pos) != 1){
                    printf("Bad position (it must be 1,2,...,9)\n");
                    char er[50];
                    scanf("%s", er);
                }
                else if(move_pos < 1 || move_pos > 9)
                    printf("Bad position (it must be 1,2,...,9)\n");
                else if(board[move_pos-1] == 'X' || board[move_pos-1] == 'O')
                    printf("Position is occupied\n");
            }while(move_pos < 1 || move_pos > 9 ||
                board[move_pos-1] == 'X' || board[move_pos-1] == 'O');

            board[move_pos-1] = sign;
            
            char pos[2];
            sprintf(pos, "%d", move_pos);
            printf("Position: %s\n", pos);

            draw_board();

            // check if user won or lost or tie
            int game_status = game_over();
            if(game_status == 1){ // win
                printf("YOU WON\n");
                if(send_msg(server_socket, MSG_WIN, pos) <= 0){
                    printf("Lost connection\n");
                    return;
                }
                return;
            }
            else if(game_status == -1){ // lost
                printf("YOU LOST\n");
                if(send_msg(server_socket, MSG_LOST, pos) <= 0){
                    printf("Lost connection\n");
                    return;
                }
                return;
            }
            else if(game_status == 2){ // tie
                printf("TIE\n");
                if(send_msg(server_socket, MSG_TIE, pos) <= 0){
                    printf("Lost connection\n");
                    return;
                }
                return;
            }

            // send position
            if(send_msg(server_socket, MSG_MOVE, pos) <= 0){
                printf("Lost connection\n");
                return;
            }
        }
        else if(msg.type == MSG_WAIT){
            printf("%s\n", "You wait");
        }
        else if(msg.type == MSG_WIN){
            printf("Opponent made a move at %s\n", msg.msg);
            int pos = atoi(msg.msg);
            board[pos-1] = sign2;
            draw_board();
            printf("YOU WON\n");
            return;
        }
        else if(msg.type == MSG_LOST){
            printf("Opponent made a move at %s\n", msg.msg);
            int pos = atoi(msg.msg);
            board[pos-1] = sign2;
            draw_board();
            printf("YOU LOST\n");
            return;
        }
        else if(msg.type == MSG_TIE){
            printf("Opponent made a move at %s\n", msg.msg);
            int pos = atoi(msg.msg);
            board[pos-1] = sign2;
            draw_board();
            printf("TIE\n");
            return;
        }
        else if(msg.type == MSG_PING){
            send_msg(server_socket, MSG_PING, "");
        }
        else if(msg.type == MSG_STOP){
            printf("Game stopped\n");
            return;
        }
    }
}




void exit_handler(){
    printf("Shutting down\n");
    disconnect();
}
void sigint_handler(int sign){
    printf("\nSIGINT received\n");
    exit(EXIT_SUCCESS);
}


void help(){
    printf("client name connection_type address\n");
    printf("name - name of user\n");
    printf("connection_type - type of connection to server (UNIX/INET)\n");
    printf("address - address of server\n");
    printf("\t if type is INET: IPv4 address and port\n");
    printf("\t if type is UNIX: path to unix server socket\n");
}

int main(int argc, char** argv){
    printf("CLIENT\n\n");
    
    if(argc < 4){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    // adding exit handlers
    atexit(exit_handler);
    if(signal(SIGINT, sigint_handler) == SIG_ERR){
        printf("ERROR: signal\n");
        return -1;
    }

    char* name = argv[1];
    char* connection_type = argv[2];
    char* address = argv[3];

    printf("Name: %s\n", name);
    printf("Connection type: %s\n", connection_type);
    printf("Address: %s\n\n", address);

    if(strcmp(connection_type, "UNIX") == 0){
        connect_to_server_unix(address);
    }
    else if(strcmp(connection_type, "INET") == 0){
        connect_to_server_inet(address);
    }
    else{
        printf("Bad connection type\n");
        help();
        return -1;
    }

    start(name);

    printf("END\n");

    return 0;
}