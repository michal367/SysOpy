#include <stdio.h>


#include "matrix.h"
#include "functions.h"




int main(int argc, char** argv){

    if(argc < 7){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    const char* matrix_list = argv[1];
    int proc_amount = atoi(argv[2]);
    if(proc_amount <= 0){
        printf("proc_amount must be >= 1\n");
        return -1;
    }

    int sec = atoi(argv[3]);
    if(sec <= 0){
        printf("sec must be >= 1\n");
        return -1;
    }
    int mode = atoi(argv[4]);
    if(mode != 1 && mode != 2){
        printf("Wrong mode\n");
        help();
        return -1;
    }

    int cpu_time_sec = atoi(argv[5]);
    if(cpu_time_sec <= 0){
        printf("cpu_time_sec must be >= 1\n");
        return -1;
    }
    int memory_MB = atoi(argv[6]);
    if(memory_MB <= 0){
        printf("memory_MB must be >= 1\n");
        return -1;
    }
    memory_MB *= 1024*1024;

    load_files(matrix_list);
    load_matrixes();

    start(mode, proc_amount, sec, cpu_time_sec, memory_MB);

    destroy_files_list();
    destroy_matrixes();

    return 0;
}

