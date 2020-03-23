#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

char* get_file_line_lib(FILE* file, long number, int length);
void set_file_line_lib(FILE* file, long number, int length, const char* value);

char* get_file_line_sys(int file, long number, int length);
void set_file_line_sys(int file, long number, int length, const char* value);


int random_number(int min, int max);
char* str_tolower(char* str);



void generate(const char* filename, int number, int length);



int partition_sys(int file, int length, int left, int right);
void quicksort_sys(int file, int length, int left, int right);

int partition_lib(FILE* file, int length, int left, int right);
void quicksort_lib(FILE* file, int length, int left, int right);

void sort_sys(const char* filename, int number, int length);
void sort_lib(const char* filename, int number, int length);


void copy_sys(const char* source, const char* destination, int number, int length);
void copy_lib(const char* source, const char* destination, int number, int length);



void help();


#endif // functions