#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>


void start(const char* filename){
    char command[100] = "sort ";
    strcat(command, filename);

    FILE* file = popen(command, "w");
    if(file != NULL){
        pclose(file);
    }
    else{
        printf("ERROR: popen\n");
    }
}



void help(){
    printf("main filename\n");
    printf("filename - filename of file that is going to be sorted\n");
}

int main(int argc, char** argv){
    if(argc < 2){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    char* file = argv[1];

    start(file);

    return 0;
}