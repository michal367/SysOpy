#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>

#include "functions.h"


double calc_time(clock_t end, clock_t start){
    return ((double)(end - start) / sysconf(_SC_CLK_TCK));
}
void print_time(clock_t end, clock_t start, struct tms* tms_end, struct tms* tms_start){
    printf("Real Time: %lf, User Time: %lf, System Time: %lf\n",
        calc_time(end, start),
        calc_time(tms_end->tms_utime, tms_start->tms_utime),
        calc_time(tms_end->tms_stime, tms_start->tms_stime));
}



int main(int argc, char** argv){
    srand(time(NULL));

    if(argc < 2)
        help();
    

    struct tms tms_start,tms_end;//, tms_first,tms_last;
    clock_t time_start,time_end;//, time_first,time_last;

    //time_first = times(&tms_first);

    for(int i=1; i<argc; i++){
        time_start = times(&tms_start);

        if(strcmp(argv[i], "generate") == 0){
            if(i + 3 >= argc){
                printf("ERROR: Too few arguments to sort \n");
                break;
            }
            char* file = strdup(argv[++i]);
            int number = atoi(argv[++i]);
            int length = atoi(argv[++i]);

            printf("Generate %s %d %d: \n", file, number, length);
            
            generate(file, number, length);

            free(file);
        }
        else if(strcmp(argv[i], "sort") == 0){
            if(i + 4 >= argc){
                printf("ERROR: Too few arguments to sort \n");
                break;
            }
            char* file = strdup(argv[++i]);
            int number = atoi(argv[++i]);
            int length = atoi(argv[++i]);
            char* mode = strdup(argv[++i]);

            printf("Sort %s %d %d %s: \n", file, number, length, mode);

            if(strcmp(mode, "sys") == 0)
                sort_sys(file, number, length);
            else if(strcmp(mode, "lib") == 0)
                sort_lib(file, number, length);
            else
                printf("Bad parameter: mode: %s \n", mode);

            free(file);
            free(mode);
        }
        else if(strcmp(argv[i], "copy") == 0){
            if(i + 5 >= argc){
                printf("ERROR: Too few arguments to copy \n");
                break;
            }
            char* source = strdup(argv[++i]);
            char* destination = strdup(argv[++i]);
            int number = atoi(argv[++i]);
            int length = atoi(argv[++i]);
            char* mode = strdup(argv[++i]);

            printf("Copy %s %s %d %d %s: \n", source, destination, number, length, mode);
            
            if(strcmp(mode, "sys") == 0)
                copy_sys(source, destination, number, length);
            else if(strcmp(mode, "lib") == 0)
                copy_lib(source, destination, number, length);
            else
                printf("Bad parameter: mode: %s \n", mode);
            

            free(source);
            free(destination);
            free(mode);
        }
        else if(strcmp(argv[i], "help") == 0){
            help();
        }
        else{
            printf("Bad argument: %s \n", argv[i]);
        }


        time_end = times(&tms_end);
        print_time(time_end, time_start, &tms_end, &tms_start);

        printf("\n");
    }



    //time_last = times(&tms_last);
    //printf("Total time:\n");
    //print_time(time_last, time_first, &tms_last, &tms_first);

    return 0;
}
