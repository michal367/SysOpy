#ifndef SERVER_HEADER_H
#define SERVER_HEADER_H

#include <string.h>
#include <assert.h>

#include "header.h"

struct user{
    char name[20];
    int socket;
    bool free;
    bool active;
    struct sockaddr* addr;
    socklen_t addr_size;
};

struct users{
    struct user users[MAX_USERS];
    int size;
};


bool is_name_free(struct users* users, const char* name){
    for(int i=0; i < users->size; i++){
        if(strcmp(users->users[i].name, name) == 0)
            return false;
    }
    return true;
}

int add_user(struct users* users, const char* name, int socket, struct sockaddr* addr, socklen_t addr_size){
    if(users->size >= MAX_USERS)
        return -1;
    struct user* user = &(users->users[users->size]);
    strcpy(user->name, name);
    user->socket = socket;
    user->free = true;
    user->active = true;
    user->addr = calloc(1, sizeof(struct sockaddr));
    user->addr->sa_family = addr->sa_family;
    memcpy(user->addr->sa_data, addr->sa_data, addr_size);
    //user->addr = *addr;
    user->addr_size = addr_size;
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



int get_game_with_user(struct games* games, int user_id){
    for(int i=0; i < games->size; i++){
        if(games->games[i].user1 == user_id || games->games[i].user2 == user_id)
            return i;
    }
    return -1;
}


int get_second_user(struct games* games, int game_id, int user_id){
    if(games->games[game_id].user1 == user_id)
        return games->games[game_id].user2;
    else if(games->games[game_id].user2 == user_id)
        return games->games[game_id].user1;
    return -1;
}

#endif // server_header