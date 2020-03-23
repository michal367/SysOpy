#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "matrix.h"


void test(int amount){
    for(int i=0; i < amount; i++){
        
        char filenameA[10] = "A/";
        char filenameB[10] = "B/";
        char filenameC[10] = "C/";
        char temp[8];
        sprintf(temp, "%d", i+1);
        strcat(filenameA, temp);
        strcat(filenameB, temp);
        strcat(filenameC, temp);

        printf("%s %s %s\n", filenameA, filenameB, filenameC);
        
        FILE* fileA = fopen(filenameA, "r");
        FILE* fileB = fopen(filenameB, "r");
        FILE* fileC = fopen(filenameC, "r");

        if(fileA && fileB && fileC){
            struct Matrix A, B, C;
            A = load_matrix_from_file(fileA);
            B = load_matrix_from_file(fileB);
            C = load_matrix_from_file(fileC);

            fclose(fileA);
            fclose(fileB);
            fclose(fileC);


            struct Matrix result = multiply_matrices(A, B);

            bool correct = is_matrices_equal(C, result);
            if(correct)
                printf("Correct\n");
            else
                printf("Incorrect\n");
            
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
            if(!fileC)
                printf("Can't open file %s\n", filenameC);
            else
                fclose(fileC);
        }
    }
}


void help(){
    printf("tester amount\n");
    printf("amount - amount of pairs of matrices to test\n");
}


int main(int argc, char** argv){

    if(argc < 2){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    int amount = atoi(argv[1]);
    if(amount <= 0){
        printf("amount must be >= 1\n");
        return -1;
    }

    test(amount);

    return 0;
}