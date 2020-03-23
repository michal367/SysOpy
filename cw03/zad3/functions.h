#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include "matrix.h"


struct Files{
    char* a;
    char* b;
    char* c;
};
struct FilesList{
    struct Files* files;
    int size;
};


// ===========================

void load_files(const char* list_file);
void load_matrixes();


void destroy_files_list();
void destroy_matrixes();


int lock(int fd);
int unlock(int fd);

void save_to_file(struct Matrix m, int pos, const char* filename);
void save_to_file2(struct Matrix m, int pos, char* path);


void make_file(const char* filename, int rows, int cols);
void paste_files(int file_number, int size);


void start(int mode, int p_amount, int p_sec, int cpu_time_sec, int memory_MB);

void help();




#endif // functions