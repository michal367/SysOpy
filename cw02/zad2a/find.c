#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>

#include "functions.h"

// find
// find [path...] [expression]

// -atime
// Returns true if a file was last accessed n * 24 hours ago. 
// When find figures out how many 24-hour periods ago 
// the file was last accessed, any fractional part is ignored, 
// so to match -atime +1, a file has to have been accessed at least 
// two days ago.

// +n 	For greater than n.
// -n 	For less than n.
// n 	For exactly n.

// -mtime
// Returns true if a file's data was last modified n * 24 hours ago. 
// See the -atime option to understand how rounding affects 
// the interpretation of file modification times.

// -maxdepth
// Descend at most levels (a non-negative integer) levels 
// of directories below the command line arguments. 
// -maxdepth 0 means only apply the tests and actions 
// to the paths specified on the command line, and do not descend 
// into subdirectories at all.



int main(int argc, char** argv){

    char** paths = (char**)calloc(argc-1, sizeof(char*));
    int paths_number = 0;

    bool parameters = false;

    int atime_days = -1;
    int atime_sign = 0;

    int mtime_days = -1;
    int mtime_sign = 0;

    int maxdepth = -1;

    for(int i=1; i < argc; i++){

        if(strcmp(argv[i], "-atime") == 0){
            if(i + 1 >= argc){
                printf("ERROR: Too few arguments to atime \n");
                break;
            }
            if(argv[i+1][0] == '+')
                atime_sign = 1;
            else if(argv[i+1][0] == '-')
                atime_sign = -1;
            else
                atime_sign = 0;
            
            atime_days = abs(atoi(argv[++i]));
            
            parameters = true;
        }
        else if(strcmp(argv[i], "-mtime") == 0){
            if(i + 1 >= argc){
                printf("ERROR: Too few arguments to mtime \n");
                break;
            }
            if(argv[i+1][0] == '+')
                mtime_sign = 1;
            else if(argv[i+1][0] == '-')
                mtime_sign = -1;
            else
                mtime_sign = 0;
            
            mtime_days = abs(atoi(argv[++i]));

            parameters = true;
        }
        else if(strcmp(argv[i], "-maxdepth") == 0){
            if(i + 1 >= argc){
                printf("ERROR: Too few arguments to maxdepth \n");
                break;
            }
            maxdepth = atoi(argv[++i]);

            parameters = true;
        }
        else if(strcmp(argv[i], "help") == 0){
            help();
        }
        else{
            if(parameters == false){
                paths[paths_number] = strdup(argv[i]);
                paths_number++;
            }
            else{
                printf("ERROR: Bad argument %s \n", argv[i]);
            }
        }
    }


    if(paths_number == 0){
        paths[paths_number] = strdup(".");
        paths_number++;
    }

    for(int i=0; i < paths_number; i++){    
        if(file_exist(paths[i])){
            // actual path
            struct stat st;
            stat(paths[i], &st);
            if(file_time_correct(st.st_atime, atime_sign, atime_days) &&
                file_time_correct(st.st_mtime, mtime_sign, mtime_days)){
                
                print_file_full_info(paths[i], &st);
            }

            // subpaths
            print_files(paths[i], maxdepth, atime_sign, atime_days, mtime_sign, mtime_days);
            
        }
        else
            printf("File %s doesn't exist \n", paths[i]);

        free(paths[i]);
        printf("\n");
    }

    free(paths);

    return 0;
}

