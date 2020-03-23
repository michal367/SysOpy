#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"


const int min_value = -100;
const int max_value = 100;

int random_nr(int min, int max){
    return (rand() % (max - min + 1)) + min;
}


void generate(int amount, int min, int max){
    for(int i=0; i < amount; i++){
        int ab_cr = random_nr(min, max); // a_cols == b_rows
        int a_rows = random_nr(min, max);
        int b_cols = random_nr(min, max);

        struct Matrix A, B;
        create_matrix(&A, a_rows, ab_cr);
        create_matrix(&B, ab_cr, b_cols);

        for(int i=0; i < A.rows; i++)
            for(int j=0; j < A.cols; j++)
                A.values[i][j] = random_nr(min_value, max_value);
        
        for(int i=0; i < B.rows; i++)
            for(int j = 0; j < B.cols; j++)
                B.values[i][j] = random_nr(min_value, max_value);

        
        char filenameA[10] = "A/";
        char filenameB[10] = "B/";
        char temp[8];
        sprintf(temp, "%d", i+1);
        strcat(filenameA, temp);
        strcat(filenameB, temp);

        printf("%s %s\n", filenameA, filenameB);
        
        FILE* fileA = fopen(filenameA, "w");
        FILE* fileB = fopen(filenameB, "w");

        if(fileA && fileB){
            save_matrix_to_file(A, fileA);
            save_matrix_to_file(B, fileB);

            fclose(fileA);
            fclose(fileB);
        }
        else
        {
            if(!fileA)
                printf("Can't open file %s\n", filenameA);
            else
                fclose(fileA);
            if(!fileB)
                printf("Can't open file %s\n", filenameB);
            else
                fclose(fileB);
        }

        destroy_matrix(&A);
        destroy_matrix(&B);
    }
}





void help(){
    printf("generator amount size_min size_max\n");
    printf("amount - amount of pairs of matrices\n");
    printf("size_min - minimum size of matrix");
    printf("size_max - maximum size of matrix\n");
}


int main(int argc, char** argv){
    srand(time(NULL));

    if(argc < 4){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    int amount = atoi(argv[1]);
    if(amount <= 0){
        printf("amount must be >= 1\n");
        return -1;
    }

    int min = atoi(argv[2]);
    if(min <= 0){
        printf("min must be >= 1\n");
        return -1;
    }

    int max = atoi(argv[3]);
    if(max < min){
        printf("max must be >= min\n");
        return -1;
    }

    generate(amount, min, max);

    return 0;
}