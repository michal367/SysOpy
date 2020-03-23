#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>

int file_exist(const char* filename);

bool file_time_correct(unsigned long long file_time, int time_sign, int time_days);

void print_files(const char* path, int maxdepth, 
                int atime_sign, int atime_days, 
                int mtime_sign, int mtime_days);

void print_file_full_info(char* path, const struct stat* st);
void print_file_info(const struct stat* st);

void print_time(unsigned long long seconds);
unsigned long long sec_before_n_days(unsigned int n);

void print_stat(const struct stat* st);
void print_dirent(const struct dirent* de);

void help();

#endif // functions