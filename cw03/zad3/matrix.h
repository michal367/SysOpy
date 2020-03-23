#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/file.h>


struct Matrix{
    int** values;
    int cols;
    int rows;
};

struct MatrixList{
    struct Matrix* A;
    struct Matrix* B;
    int size;
};


int min(int a, int b);


int get_file_lines(FILE* file);

struct Matrix load_matrix_from_file(FILE* file);
void save_matrix_to_file(struct Matrix matrix, FILE* file);


struct Matrix multiply_matrices(struct Matrix a, struct Matrix b);
struct Matrix process_multiply_matrixes(struct Matrix a, struct Matrix b, int b_start_column, int b_cols_amount);


void create_matrix(struct Matrix* m, int rows, int cols);
void destroy_matrix(struct Matrix* matrix);

bool is_matrices_equal(struct Matrix a, struct Matrix b);




#endif // matrix