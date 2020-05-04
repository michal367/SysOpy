#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/time.h>


struct Matrix{
    int** values;
    int rows;
    int cols;
};


void create_matrix(struct Matrix* m, int rows, int cols){
    m->rows = rows;
    m->cols = cols;

    m->values = (int**)calloc(rows, sizeof(int*));
    for(int i=0; i<rows; i++)
        m->values[i] = (int*)calloc(cols, sizeof(int));
}
void fill_matrix(struct Matrix* m, int value){
    for(int i=0; i<m->rows; i++)
        for(int j=0; j<m->cols; j++)
            m->values[i][j] = value;
}



struct Matrix load_matrix_from_ascii_pgm(const char* filename, int* M){
    FILE* file = fopen(filename, "r");
    if(file){
        size_t line_buf_size = 30;
        char* line_buf = (char*)calloc(line_buf_size, sizeof(char));
        
        // get first line and check if it is "P2"
        if(getline(&line_buf, &line_buf_size, file) <= 0){
            printf("ERROR: file %s is empty\n", filename);
            exit(1);
        }
        if(strcmp(line_buf, "P2\n") != 0 && 
            strcmp(line_buf, "P2\r\n") != 0){
                printf("ERROR: it's not .ascii.pgm file (it must have \"P2\" in the first line)\n");
                exit(1);
        }

        int W, H;
        int temp;

        // search for parameters W H M
        int loaded_values = 0;
        while(loaded_values < 3){
            if(getline(&line_buf, &line_buf_size, file) <= 0){
                printf("ERROR: parameters W H M not found when reading file %s\n", filename);
                exit(1);
            }
            // comment
            if(line_buf[0] == '#')
                continue;
            
            // W H M
            int bytes_consumed = 0, bytes_amount;
            while(sscanf(line_buf + bytes_consumed, "%d%n", &temp, &bytes_amount) > 0){
                bytes_consumed += bytes_amount;

                if(loaded_values == 0)
                    W = temp;
                else if(loaded_values == 1)
                    H = temp;
                else if(loaded_values == 2)
                    *M = temp;

                loaded_values++;
                if(loaded_values >= 3)
                    break;
            }
        }
        free(line_buf);
        printf("W=%d, H=%d, M=%d\n", W, H, *M);

        // create and load matrix
        struct Matrix matrix;
        create_matrix(&matrix, H, W);
        for(int i=0; i < matrix.rows; i++){
            for(int j=0; j < matrix.cols; j++)
                fscanf(file, "%d", &matrix.values[i][j]);
        }

        fclose(file);

        return matrix;
    }
    else{
        printf("Can't open file %s\n", filename);
        exit(1);
    }
}


void print_matrix(struct Matrix* matrix){
    for(int i=0; i < matrix->rows; i++){
        for(int j=0; j < matrix->cols; j++)
            printf("%d ", matrix->values[i][j]);
        printf("\n");
    }
}


void destroy_matrix(struct Matrix* matrix){
    for(int i=0; i<matrix->rows; i++)
        free(matrix->values[i]);
    free(matrix->values);
}





int* join_histograms(struct Matrix histograms){
    int size = histograms.cols;
    int* histogram = (int*)calloc(size, sizeof(int));
    for(int i=0; i < size; i++)
        histogram[i] = 0;
    for(int i=0; i < histograms.rows; i++){
        for(int j=0; j < size; j++)
            histogram[j] += histograms.values[i][j];
    }
    return histogram;
}


void save_histogram_to_file(const char* filename_out, int* histogram, int size){
    FILE* file_out = fopen(filename_out, "w");
    if(file_out){
        for(int i=0; i < size; i++){
            //printf("%d: %d\n", i, histogram[i]);
            fprintf(file_out, "%d: %d\n", i, histogram[i]);
        }
        fclose(file_out);
    }
    else{
        printf("ERROR: Can't open file %s\n", filename_out);
    }
}



int div_ceil(int a, int b){
    return (a+b-1)/b;
}
int min(int a, int b){
    return a < b ? a : b;
}

long get_us_time(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    // return microseconds
    return (long)(tv.tv_sec*1000*1000 + tv.tv_usec);
}

#endif // header