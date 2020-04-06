#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>


void start(const char* fifo, const char* file, int N){
    FILE* fifo_file = fopen(fifo, "w");
    if(fifo_file){

        FILE* text_file = fopen(file, "r");
        if(text_file){

            int pid = (int)getpid();

            char* buffer = (char*)calloc(N+1, sizeof(char));
            int size;
            while((size = fread(buffer, sizeof(char), N, text_file)) > 0){
                buffer[size] = '\0';
                
                printf("#%d#%s\n", pid, buffer);

                fprintf(fifo_file, "#%d#%s\n", pid, buffer);
                fflush(fifo_file); // force writing to file (skip buffering)

                sleep(1);
            }
            free(buffer);
            

            fclose(text_file);
        }
        else{
            printf("Can't open file %s\n", file);
        }

        fclose(fifo_file);
    }
    else{
        printf("Can't open fifo file %s\n", fifo);
    }
}



void help(){
    printf("producer fifo file N\n");
    printf("fifo - path to fifo file\n");
    printf("file - path to file from producer will read\n");
    printf("N - amount of bytes that producer will read in one step\n");
}


int main(int argc, char** argv){

    if(argc < 4){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    char* fifo = argv[1];
    char* file = argv[2];
    int N = atoi(argv[3]);
    if(N <= 0){
        printf("N must be >= 1\n");
        return -1;
    }

    start(fifo, file, N);

    return 0;
}