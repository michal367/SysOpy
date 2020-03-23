#include "functions.h"


char* get_file_line_lib(FILE* file, long number, int length){
    fseek(file, number*(length+1), SEEK_SET);
    char* line = (char*)calloc(length+2, sizeof(char));
    fread(line, sizeof(char), length+1, file);
    line[length+1] = '\0';
    return line;
}
void set_file_line_lib(FILE* file, long number, int length, const char* value){
    fseek(file, number*(length+1), SEEK_SET);
    fwrite(value, sizeof(char), length, file);
}



char* get_file_line_sys(int file, long number, int length){
    lseek(file, number*(length+1), SEEK_SET);
    char* line = (char*)calloc(length+2, sizeof(char));
    read(file, line, length+1);
    line[length+1] = '\0';
    return line;
}
void set_file_line_sys(int file, long number, int length, const char* value){
    lseek(file, number*(length+1), SEEK_SET);
    write(file, value, length);
}





int random_number(int min, int max){
    return rand()%(max-min) + min;
}
char* str_tolower(char* str){
    for(int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
    return str;
}




void generate(const char* filename, int number, int length){
    FILE* file = fopen(filename, "w");
    if(file){

        // allocating length+2 bytes, because those 2 are for '\n' and '\0'
        char* line = (char*)calloc(length + 2, sizeof(char));
        
        for(int i=0; i<number; i++){
            for(int j=0; j<length; j++)
                line[j] = random_number('0', 'z'); // from 48 to 122
            line[length] = '\n';
            line[length + 1] = '\0';

            //printf("%s", line);
            fwrite(line, sizeof(char), length+1, file);
        }

        free(line);
        fclose(file);
    }
}



// partitions lexicographically lines in file at indexes [left; right)
int partition_sys(int file, int length, int left, int right)
{
    int j, i = left+1;
    char* pivot = get_file_line_sys(file, left, length);
    char* pivot_lowercase = str_tolower(strdup(pivot));

    for(j = left+1; j < right; j++)
    {
        char* a = get_file_line_sys(file, j, length);
        char* a_lowercase = str_tolower(strdup(a));
        
        if(strcmp(a_lowercase, pivot_lowercase) < 0){// a < pivot
            char* b = get_file_line_sys(file, i, length);
            // swap a and b in file
            set_file_line_sys(file, j, length, b);
            set_file_line_sys(file, i, length, a);
            i++;
            free(b);
        }

        free(a);
        free(a_lowercase);
    }

    // place pivot at its position by swapping
    char* c = get_file_line_sys(file, i-1, length);
    // swap c and pivot in file
    set_file_line_sys(file, i-1, length, pivot);
    set_file_line_sys(file, left, length, c);
    
    free(pivot);
    free(pivot_lowercase);
    free(c);

    return i-1;
}

// sorts file at indexes [left; right)
void quicksort_sys(int file, int length, int left, int right)
{
    if(left < right-1)
    {
        int q = partition_sys(file, length, left, right); //finding the pivot position in sorted array
        quicksort_sys(file, length, left, q);
        quicksort_sys(file, length, q+1, right);
    }
}





// partitions lexicographically lines in file at indexes [left; right)
int partition_lib(FILE* file, int length, int left, int right)
{
    int j, i = left+1;
    char* pivot = get_file_line_lib(file, left, length);
    char* pivot_lowercase = str_tolower(strdup(pivot));

    for(j = left+1; j < right; j++)
    {
        char* a = get_file_line_lib(file, j, length);
        char* a_lowercase = str_tolower(strdup(a));
        
        if(strcmp(a_lowercase, pivot_lowercase) < 0){// a < pivot
            char* b = get_file_line_lib(file, i, length);
            // swap a and b in file
            set_file_line_lib(file, j, length, b);
            set_file_line_lib(file, i, length, a);
            i++;
            free(b);
        }

        free(a);
        free(a_lowercase);
    }

    // place pivot at its position by swapping
    char* c = get_file_line_lib(file, i-1, length);
    // swap c and pivot in file
    set_file_line_lib(file, i-1, length, pivot);
    set_file_line_lib(file, left, length, c);
    
    free(pivot);
    free(pivot_lowercase);
    free(c);

    return i-1;
}

// sorts file at indexes [left; right)
void quicksort_lib(FILE* file, int length, int left, int right)
{
    if(left < right-1)
    {
        int q = partition_lib(file, length, left, right); //finding the pivot position in sorted array
        quicksort_lib(file, length, left, q);
        quicksort_lib(file, length, q+1, right);
    }
}



void sort_sys(const char* filename, int number, int length){
    int file = open(filename, O_RDWR);
    if(file >= 0){
        quicksort_sys(file, length, 0, number);

        close(file);
    }
    else
        printf("ERROR: Can't open file %s \n", filename);
}

void sort_lib(const char* filename, int number, int length){
    FILE* file = fopen(filename, "r+");
    if(file){
        quicksort_lib(file, length, 0, number);
        
        fclose(file);
    }
    else
        printf("ERROR: Can't open file %s \n", filename);
}




void copy_sys(const char* source, const char* destination, int number, int length){
    if(strcmp(source, destination) != 0){
        int sourcef = open(source, O_RDONLY);
        int destf = open(destination, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

        if(sourcef >= 0 && destf >= 0){
            char* line = (char*)calloc(length+1, sizeof(char));
            
            for(int i=0; i < number; i++){
                if(read(sourcef, line, length+1) == length+1)
                    write(destf, line, length+1);
                else
                    break;
            }

            free(line);
            close(sourcef);
            close(destf);
        }
        else{
            if(sourcef < 0)
                printf("ERROR: %s can't be opened \n", source);
            else
                close(sourcef);

            if(destf < 0)
                printf("ERROR: %s can't be opened \n", destination);
            else
                close(destf);
        }
    }
    else
        printf("ERROR: Source file and destination file are the same \n");
}

void copy_lib(const char* source, const char* destination, int number, int length){
    if(strcmp(source, destination) != 0){
        FILE* sourcef = fopen(source, "r");
        FILE* destf = fopen(destination, "w");

        if(sourcef && destf){
            char* line = (char*)calloc(length+1, sizeof(char));
            
            for(int i=0; i < number; i++){
                if(fread(line, sizeof(char), length+1, sourcef) == length+1)
                    fwrite(line, sizeof(char), length+1, destf);
                else
                    break;
            }

            free(line);
            fclose(sourcef);
            fclose(destf);
        }
        else{
            if(!sourcef)
                printf("ERROR: %s can't be opened \n", source);
            else
                fclose(sourcef);

            if(!destf)
                printf("ERROR: %s can't be opened \n", destination);
            else
                fclose(destf);
        }
    }
    else
        printf("ERROR: Source file and destination file are the same \n");
}





void help(){
    printf("generate file number length - generates number of lines having length size to file \n");
    printf("sort file number length mode - sorts file by lines with mode (sys/lib) \n");
    printf("copy source destination number length mode - copies number of lines from file1 to file2 with mode (sys/lib) \n");
}
