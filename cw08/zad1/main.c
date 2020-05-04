#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include "header.h"


struct Matrix matrix;
struct Matrix histograms;


void* thread_sign(void* val){
    long start_time = get_us_time();

    int* values = (int*)val;
    int k = values[0];
    int a = values[1]; // start number
    int b = values[2]; // end number
    //printf("%ld: %d %d %d\n", pthread_self(), k,a,b);

    for(int i=0; i < matrix.rows; i++){
        for(int j=0; j < matrix.cols; j++){
            int value = matrix.values[i][j];
            if(value >= a && value <= b){
                histograms.values[k][value]++;
            }
        }
    }

    long* time = (long*)malloc(sizeof(long));
    *time = get_us_time() - start_time;
    //printf("THREAD self %ld: %ld us\n", pthread_self(), *time);
    pthread_exit((void*)time);
    return NULL;
}

void* thread_block(void* val){
    long start_time = get_us_time();

    int* values = (int*)val;
    int k = values[0];
    int a = values[1]; // start column
    int b = values[2]; // end column
    //printf("%ld: %d %d %d\n", pthread_self(), k,a,b);

    for(int i=0; i < matrix.rows; i++){
        for(int j=a; j <= b; j++){
            int value = matrix.values[i][j];
            histograms.values[k][value]++;
        }
    }

    long* time = (long*)malloc(sizeof(long));
    *time = get_us_time() - start_time;
    //printf("THREAD self %ld: %ld us\n", pthread_self(), *time);
    pthread_exit((void*)time);
    return NULL;
}

void* thread_interleaved(void* val){
    long start_time = get_us_time();

    int* values = (int*)val;
    int k = values[0];
    int m = values[1]; // threads amount
    //printf("%ld: %d %d\n", pthread_self(), k,m);

    for(int i=0; i < matrix.rows; i++){
        for(int j=k; j < matrix.cols; j+=m){
            int value = matrix.values[i][j];
            histograms.values[k][value]++;
        }
    }

    long* time = (long*)malloc(sizeof(long));
    *time = get_us_time() - start_time;
    //printf("THREAD self %ld: %ld us\n", pthread_self(), *time);
    pthread_exit((void*)time);
    return NULL;
}


void help(){
    printf("main threads_amount mode file_in file_out\n");
    printf("threads_amount - amount of threads\n");
    printf("mode:\n");
    printf("\tsign - each thread operates on specified range of numbers\n");
    printf("\tblock - each thread gets solid range of columns\n");
    printf("\tinterleaved - each thread gets interleaved range of columns\n");
    printf("file_in - path to input file (.ascii.pgm)\n");
    printf("file_out - path to output file (there will be saved histogram)\n");
}

int main(int argc, char** argv)
{
    if(argc < 5){
        printf("Too few arguments\n");
        help();
        exit(1);
    }

    int threads_amount = atoi(argv[1]);
    char* mode = argv[2];
    char* filename_in = argv[3];
    char* filename_out = argv[4];

    if(threads_amount <= 0){
        printf("threads_amount must be >= 1\n");
        exit(1);
    }

    if(strcmp(mode, "sign") != 0 &&
        strcmp(mode, "block") != 0 &&
        strcmp(mode, "interleaved") != 0){
            printf("Wrong mode\n");
            help();
            exit(1);
    }


    int M;
    matrix = load_matrix_from_ascii_pgm(filename_in, &M);
    int W = matrix.cols;
    //int H = matrix.rows;
    //print_matrix(&matrix);


    create_matrix(&histograms, threads_amount, M+1);
    fill_matrix(&histograms, 0);

    
    long start_time = get_us_time();

    pthread_t* tids = (pthread_t*)calloc(threads_amount, sizeof(pthread_t));
    struct Matrix args;
    create_matrix(&args, threads_amount, 3);    

    if(strcmp(mode, "sign") == 0){
        int div = div_ceil(M+1, threads_amount);
        int temp = 0;
        // create threads
        for(int i=0; i<threads_amount; i++){
            args.values[i][0] = i;
            args.values[i][1] = temp;
            args.values[i][2] = min(temp + div - 1, M);
            temp += div;
            pthread_create(&tids[i], NULL, thread_sign, (void*)args.values[i]);
        }
    }
    else if(strcmp(mode, "block") == 0){
        int div = div_ceil(W, threads_amount);
        int temp = 0;
        // create threads
        for(int i=0; i<threads_amount; i++){
            args.values[i][0] = i;
            args.values[i][1] = temp;
            args.values[i][2] = min(temp + div - 1, W-1);
            temp += div;
            pthread_create(&tids[i], NULL, thread_block, (void*)args.values[i]);
        }
    }
    else if(strcmp(mode, "interleaved") == 0){
        // create threads
        for(int i=0; i<threads_amount; i++){
            args.values[i][0] = i;
            args.values[i][1] = threads_amount;
            pthread_create(&tids[i], NULL, thread_interleaved, (void*)args.values[i]);
        }
    }


    // join threads
    long* time_ptr;
    for(int i=0; i < threads_amount; i++){
        pthread_join(tids[i], (void**)&time_ptr);
        printf("THREAD %ld: %ld us\n", tids[i], *time_ptr);
        free(time_ptr);
    }

    free(tids);
    destroy_matrix(&matrix);
    destroy_matrix(&args);

    long end_time = get_us_time();
    printf("Total time: %ld us\n", end_time - start_time);

    // join and save histogram
    int* histogram = join_histograms(histograms);
    save_histogram_to_file(filename_out, histogram, M+1);
    free(histogram);
    destroy_matrix(&histograms);


    return 0;
}
