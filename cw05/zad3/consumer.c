#include <stdio.h>
#include <stdlib.h>


void start(const char* fifo, const char* file, int N){
    FILE* fifo_file = fopen(fifo, "r");
    if(fifo_file){

        FILE* text_file = fopen(file, "w");
        if(text_file){

            char* buffer = (char*)calloc(N+1, sizeof(char));
            int size;
            while((size = fread(buffer, sizeof(char), N, fifo_file)) > 0){
                buffer[size] = '\0';

                printf("%s\n", buffer);

                fwrite(buffer, sizeof(char), size, text_file);
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
    printf("consumer fifo file N\n");
    printf("fifo - path to fifo file\n");
    printf("file - path to file where consumer will write\n");
    printf("N - amount of bytes that producer will write in one step\n");
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