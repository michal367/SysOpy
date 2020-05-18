#ifndef SERVER_HEADER_H
#define SERVER_HEADER_H

#include <string.h>

#include "header.h"

struct user{
    char name[20];
    int socket;
    bool free;
    bool active;
};

struct users{
    struct user users[MAX_USERS];
    int size;
};


bool is_name_free(struct users* users,const char* name){
    for(int i=0; i < users->size; i++){
        if(strcmp(users->users[i].name, name) == 0)
            return false;
    }
    return true;
}

int add_user(struct users* users, const char* name, int socket){
    if(users->size >= MAX_USERS)
        return -1;
    struct user* user = &(users->users[users->size]);
    strcpy(user->name, name);
    user->socket = socket;
    user->free = true;
    user->active = true;
    users->size++;
    return users->size-1;
}

int get_free_user(struct users* users, int skip_id){
    for(int i=0; i < users->size; i++){
        if(i == skip_id)
            continue;
        if(users->users[i].socket != -1 && users->users[i].free == true)
            return i;
    }
    return -1;
}

int get_user_with_socket(struct users* users, int socket){
    for(int i=0; i < users->size; i++){
        if(users->users[i].socket == socket)
            return i;
    }
    return -1;
}

char* get_username_with_socket(struct users* users, int socket){
    int id = get_user_with_socket(users, socket);
    if(id != -1)
        return users->users[id].name;
    else
        return NULL;
}



struct game{
    int user1;
    int user2;
};

struct games{
    struct game games[MAX_USERS/2];
    int size;
};


int add_game(struct games* games, int user1, int user2){
    if(games->size >= MAX_USERS/2)
        return -1;
    struct game* game = &(games->games[games->size]);
    game->user1 = user1;
    game->user2 = user2;
    games->size++;
    return games->size-1;
}

int get_game_with_user_socket(struct games* games, struct users* users, int socket){
    for(int i=0; i < games->size; i++){
        if(users->users[games->games[i].user1].socket == socket ||
           users->users[games->games[i].user2].socket == socket)
            return i;
    }
    return -1;
}
int get_second_socket(struct games* games, struct users* users, int socket){
    for(int i=0; i < games->size; i++){
        if(users->users[games->games[i].user1].socket == socket)
            return users->users[games->games[i].user2].socket;
        else if(users->users[games->games[i].user2].socket == socket)
            return users->users[games->games[i].user1].socket;
    }
    return -1;
}

#endif // server_header