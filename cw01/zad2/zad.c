#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include "lib.h"


void print_tab(const struct Table* tab){

    printf("\n");
    printf("=========================\n");

    printf("blocks amount %I64ld:\n\n", tab->size);
    for(int i=0; i < tab->size; i++){
        printf("block nr: %d \n", i);
        printf("operations amount %I64ld:\n\n", tab->blocks[i].size);
        for(int j=0; j < tab->blocks[i].size; j++){
            printf("operation nr: %d \n", j);
            printf(tab->blocks[i].operations[j]);
            printf("\n___________________________________\n");
        }
        printf("\n==============================\n");
    }
    printf("\n\n\n");
}


void print_pairs(const struct ArrayPairs* ap){
    for(int i=0; i<ap->size; i++){
        printf(ap->pairs[i].first);
        printf(" ");
        printf(ap->pairs[i].second);
        printf("\n");
    }
}

double calc_time(clock_t end, clock_t start){
    return ((double)(end - start) / sysconf(_SC_CLK_TCK));
}
void print_time(clock_t end, clock_t start, struct tms* tms_end, struct tms* tms_start){
    printf("Real Time: %lf, User Time: %lf, System Time: %lf\n",
        calc_time(end, start),
        calc_time(tms_end->tms_utime, tms_start->tms_utime),
        calc_time(tms_end->tms_stime, tms_start->tms_stime));
}
void save_time(FILE* file, clock_t end, clock_t start, struct tms* tms_end, struct tms* tms_start){
    fprintf(file, "Real Time: %lf, User Time: %lf, System Time: %lf\n",
        calc_time(end, start),
        calc_time(tms_end->tms_utime, tms_start->tms_utime),
        calc_time(tms_end->tms_stime, tms_start->tms_stime));
}


int main(int argc, char **argv){
    /*printf("compare_pairs a:b c:d ... \t\t\t porownanie par plikow:  a z b, c z d, itd \n");
    printf("\t\t\t\t\t\t moze byc w formacie \"a:b c:d\" lub \"a b c d\" \n");
    printf("remove_block index \t\t\t\t usun z tablicy blokow o indeksie index \n");
    printf("remove_operation block_index operation_index \t usun z bloku o indeksie block_index operacje o indeksie operation_index \n\n\n");
    */

    int n = 0;
    struct Table* tab = NULL;
    struct ArrayPairs* ap = NULL;


    struct tms tms[argc];
    clock_t time[argc];
    int t_ind = 0;

    FILE* raport_file = fopen("raport_pojedynczy.txt", "w");

    for(int i=1; i<argc; i++){
        time[t_ind] = times(&tms[t_ind]);
        t_ind += 1;


        // args
        if(strcmp(argv[i], "create_table") == 0){
            printf("Create table:\n");
            fprintf(raport_file, "Create table:\n");

            n = atoi(argv[i+1]);
            //printf("%d\n", n);
            i++;
        }
        else if(strcmp(argv[i], "compare_pairs") == 0){
            printf("Compare pairs:\n");
            fprintf(raport_file, "Compare pairs:\n");

            char buffer[4000] = " ";
            for(int j=0; j<n; j++){
                strcat(buffer, argv[i+j+1]);
                strcat(buffer, " ");
            }
            i += n;
            //printf("%s\n", buffer);


            ap = create_pairs(buffer);
            if(ap == NULL)
                printf("ERROR\n");
            else{
                print_pairs(ap);

                if(tab != NULL)
                    destroy(tab);

                tab = generate(ap);
                if(tab == NULL)
                    printf("ERROR\n");
                else{
                    //printf("CORRECT\n");

                    //print_tab(tab);
                }

            }

        }
        else if(strcmp(argv[i], "remove_block") == 0){
            printf("Remove block:\n");
            fprintf(raport_file, "Remove block:\n");

            int block_index = atoi(argv[i+1]);
            i++;

            if(tab != NULL){
                erase_block(tab, block_index);
                printf("Removing block: %d\n", block_index);
            }    
            //print_tab(tab);
        }
        else if(strcmp(argv[i], "remove_operation") == 0){
            printf("Remove operation:\n");
            fprintf(raport_file, "Remove operation:\n");

            int block_index = atoi(argv[i+1]);
            int operation_index = atoi(argv[i+2]);
            i += 2;

            if(tab != NULL){
                erase_operation(tab, block_index, operation_index);
                printf("Removing operation: %d from block: %d\n", operation_index, block_index);
            }

            //print_tab(tab);
        }
        else{
            printf("Bad argument: %s \n", argv[i]);
        }
        
        time[t_ind] = times(&tms[t_ind]);
        print_time(time[t_ind], time[t_ind-1], &tms[t_ind], &tms[t_ind-1]);
        save_time(raport_file, time[t_ind], time[t_ind-1], &tms[t_ind], &tms[t_ind-1]);
        t_ind += 1;

        printf("\n");
    }

    printf("Total time:\n");
    fprintf(raport_file, "Total time:\n");

    time[t_ind] = times(&tms[t_ind]);
    print_time(time[t_ind], time[0], &tms[t_ind], &tms[0]);
    save_time(raport_file, time[t_ind], time[0], &tms[t_ind], &tms[0]);



    
    fclose(raport_file);

    if(ap)
        destroy_pairs(ap);
    if(tab)
        destroy(tab);

    return 0;
}
