#include "matrix.h"


int min(int a, int b){
    return a < b ? a : b;
}


int get_file_lines(FILE* file){
    int lines = 0;
    char c;
    for(c = getc(file); c != EOF; c = getc(file)){
        if(c == '\n') // increment count if this character is newline 
            lines++;
    }
    rewind(file);
    return lines;
}





struct Matrix load_matrix_from_file(FILE* file){
    int rows = 0, cols = 0;
    char c;
    for(c = getc(file); c != EOF; c = getc(file)){
        if(c == '\n') // increment count if this character is newline 
            rows++;
        else if(c == ' ')
            cols++;
    }

    cols = cols/rows + 1;

    //printf("%d %d\n", rows, cols);

    struct Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;

    matrix.values = (int**)calloc(rows, sizeof(int*));
    for(int i=0; i<rows; i++)
        matrix.values[i] = (int*)calloc(cols, sizeof(int));

    rewind(file);

    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++)
            fscanf(file, "%d", &matrix.values[i][j]);
    }

    return matrix;
}


void save_matrix_to_file(struct Matrix matrix, FILE* file){
    for(int i=0; i<matrix.rows; i++){
        for(int j=0; j<matrix.cols - 1; j++)
            fprintf(file, "%d ", matrix.values[i][j]);
        fprintf(file, "%d\n", matrix.values[i][matrix.cols-1]);
    }
}


struct Matrix multiply_matrices(struct Matrix a, struct Matrix b){
    if(a.cols == b.rows){
        struct Matrix result;
        result.rows = a.rows;
        result.cols = b.cols;

        result.values = (int**)calloc(result.rows, sizeof(int*));
        for(int i=0; i<result.rows; i++)
            result.values[i] = (int*)calloc(result.cols, sizeof(int));

        for(int i=0; i<a.rows; i++){
            for(int j=0; j<b.cols; j++){
                int sum = 0;
                for(int k=0; k<a.cols; k++)
                    sum += a.values[i][k] * b.values[k][j];
                result.values[i][j] = sum;
            }
        }
        
        return result;
    }
    else{
        struct Matrix r;
        r.rows = 0;
        r.cols = 0;
        return r;
    }
}


struct Matrix process_multiply_matrixes(struct Matrix a, struct Matrix b, 
                                        int b_start_column, int b_cols_amount){
    if(a.cols == b.rows){
        struct Matrix result;
        //int rows= a.rows;
        //int cols = min(b_cols_amount, b.cols - b_start_column);
        result.rows = a.rows;
        result.cols = min(b_cols_amount, b.cols - b_start_column);

        result.values = (int**)calloc(result.rows, sizeof(int*));
        for(int i=0; i<result.rows; i++)
            result.values[i] = (int*)calloc(result.cols, sizeof(int));

        for(int i=0; i < result.rows; i++){
            for(int j=0; j < result.cols; j++){
                int sum = 0;
                for(int k=0; k < a.cols; k++)
                    sum += a.values[i][k] * b.values[k][b_start_column + j];
                result.values[i][j] = sum;
            }
        }
        
        return result;
    }
    else{
        struct Matrix r;
        r.rows = 0;
        r.cols = 0;
        return r;
    }
}


void create_matrix(struct Matrix* m, int rows, int cols){
    m->rows = rows;
    m->cols = cols;

    m->values = (int**)calloc(rows, sizeof(int*));
    for(int i=0; i<rows; i++)
        m->values[i] = (int*)calloc(cols, sizeof(int));
}

void destroy_matrix(struct Matrix* matrix){
    for(int i=0; i<matrix->rows; i++)
        free(matrix->values[i]);
    free(matrix->values);
}


bool is_matrices_equal(struct Matrix a, struct Matrix b){
    if(a.rows == b.rows && a.cols == b.cols){
        for(int i=0; i < a.rows; i++)
            for(int j=0; j < a.cols; j++)
                if(a.values[i][j] != b.values[i][j])
                    return false;
        
        return true;
    }
    return false;
}