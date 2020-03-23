#include "functions.h"


// globals
struct FilesList files;
struct MatrixList matrixes;

const int p_cols = 2;



void load_files(const char* list_file){
    FILE* file = fopen(list_file, "r");
    if(file){

        int lines = get_file_lines(file);

        //
        files.size = lines;
        files.files = (struct Files*)calloc(lines, sizeof(struct Files));


        size_t line_buf_size = 60;
        char* line_buf = (char*)calloc(line_buf_size, sizeof(char));

        int f_index = 0;
        while(getline(&line_buf, &line_buf_size, file) >= 0){
            
            char *ptr = strtok(line_buf, " ");
            files.files[f_index].a = strdup(ptr);
            ptr = strtok(NULL, " ");
            files.files[f_index].b = strdup(ptr);
            ptr = strtok(NULL, "\n");
            files.files[f_index].c = strdup(ptr);

            f_index++;
        }
        
        free(line_buf);
        fclose(file);
    }
    else{
        printf("Can't open file %s\n", list_file);
    }

    //for(int i=0; i<files.size; i++){
    //    printf("%s %s %s\n", files.files[i].a, files.files[i].b, files.files[i].c);
    //}
}


void load_matrixes(){

    int size = files.size;
    matrixes.size = size;
    matrixes.A = (struct Matrix*)calloc(size, sizeof(struct Matrix));
    matrixes.B = (struct Matrix*)calloc(size, sizeof(struct Matrix));

    for(int i=0; i<files.size; i++){
        FILE* file_ma = fopen(files.files[i].a, "r");
        FILE* file_mb = fopen(files.files[i].b, "r");
        FILE* file_mc = fopen(files.files[i].c, "w"); // to clear file


        if(file_ma && file_mb && file_mc){
            
            matrixes.A[i] = load_matrix_from_file(file_ma);
            matrixes.B[i] = load_matrix_from_file(file_mb);

            fclose(file_ma);
            fclose(file_mb);
            fclose(file_mc);
        }
        else{
            if(!file_ma)
                printf("Can't open file %s\n", files.files[i].a);
            else
                fclose(file_ma);

            if(!file_mb)
                printf("Can't open file %s\n", files.files[i].b);
            else
                fclose(file_mb);
            
            if(!file_mc)
                printf("Can't open file %s\n", files.files[i].c);
            else
                fclose(file_mc);
            
            matrixes.A[i].values = NULL;
            matrixes.B[i].values = NULL;
        }
    }
}


void destroy_files_list(){
    for(int i=0; i<files.size; i++){
        free(files.files[i].a);
        free(files.files[i].b);
        free(files.files[i].c);
    }
    free(files.files);
}
void destroy_matrixes(){
    for(int i=0; i<matrixes.size; i++){
        destroy_matrix(&matrixes.A[i]);
        destroy_matrix(&matrixes.B[i]);
    }
    free(matrixes.A);
    free(matrixes.B);
}




int lock(int fd){
    int l;
    while(1){
        l = flock(fd, LOCK_EX | LOCK_NB);
        if(l == 0)
            break;
        int time = (rand()%5)*1000;
        usleep(time);
    }
    return l;
}

int unlock(int fd){
    return flock(fd, LOCK_UN);
}

void save_to_file(struct Matrix m, int pos, const char* filename){
    FILE* file = fopen(filename, "a");
    if(file){
        int fd = fileno(file);
        int l = lock(fd);
        if(l == 0){
            fprintf(file, "%d %d %d :\n", m.rows, m.cols, pos);
            save_matrix_to_file(m, file);

            unlock(fd);
            fclose(file);
        }
    }
    else
        printf("Can't open file %s\n", filename);
}

void save_to_file2(struct Matrix m, int pos, char* path){
    char c_pos_filename[30] = "";
    strcat(c_pos_filename, path);
    strcat(c_pos_filename, "_");
    
    char temp[8];
    sprintf(temp, "%d", pos);
    strcat(c_pos_filename, temp);

    //printf("%s\n", c_pos_filename);

    FILE* c_pos_file = fopen(c_pos_filename, "w");
    if(c_pos_file){
        save_matrix_to_file(m, c_pos_file);

        fclose(c_pos_file);
    }
    else
        printf("Can't open file %s\n", c_pos_filename);
}


void make_file(const char* filename, int rows, int cols){
    FILE* file = fopen(filename, "r");
    if(file){
        struct Matrix result;
        create_matrix(&result, rows, cols);

        int amount = (cols+1)/2;
        for(int a=0; a < amount; a++){
            int r, c, p;
            fscanf(file, "%d %d %d ", &r, &c, &p);
            char ch;
            fscanf(file, "%c", &ch);
            if(ch != ':'){
                destroy_matrix(&result);
                fclose(file);
                return;
            }

            //printf("%d %d %d\n", r,c,p);
            for(int i=0; i < r; i++){
                for(int j=0; j < c; j++){
                    fscanf(file, "%d", &result.values[i][p+j]);
                }
            }
        }

        fclose(file);

        
        FILE* file2 = fopen(filename, "w");
        if(file2){
            save_matrix_to_file(result, file2);
            fclose(file2);
        }
        else
            printf("Can't open file %s\n", filename);
        

        destroy_matrix(&result);
    }
    else
        printf("Can't open file %s\n", filename);
}


void paste_files(int file_number, int size){
    // setting params for execvp
    int params_size = 2 + size + 1;
    char** params = (char**)calloc(params_size, sizeof(char*));
    params[0] = strdup("paste");
    params[1] = strdup("--delimiters= ");

    int pos = 0;
    int index = 2;
    for(int j=0; j<size; j++){
        char filename[30] = "";
        strcat(filename, files.files[file_number].c);
        char temp[8];
        sprintf(temp, "%d", pos);
        strcat(filename, "_");
        strcat(filename, temp);

        params[index] = strdup(filename);
        
        index++;
        pos += p_cols;
    }
    params[index] = NULL;
    
    //for(int j=0; j < params_size-1; j++)
    //    printf("%d: %s\n", j, params[j]);


    pid_t pid = fork();
    if(pid < 0){
        printf("ERROR: fork\n");
        exit(1);
    }
    else if(pid == 0){ // child
        // configure output file
        int fd = open(files.files[file_number].c, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(fd, 1);   // make stdout go to file
        dup2(fd, 2);   // make stderr go to file
        close(fd);     // fd no longer needed - the dup'ed handles

        execvp("paste", params);
        exit(1);
    }
    wait(NULL);

    // remove files i_*
    for(int j=2; j < params_size-1; j++){
        remove(params[j]);
    }

    // free params
    for(int j=0; j < params_size-1; j++)
        free(params[j]);
    free(params);
}



void start(int mode, int p_amount, int p_sec){
    pid_t* pids = (pid_t*)calloc(p_amount, sizeof(pid_t));

    for(int i=0; i < p_amount; i++){
        pids[i] = fork();
        
        if(pids[i] < 0){
            printf("ERROR: fork\n");
            exit(1);
        }
        else if(pids[i] == 0){ // child
            int m_index = 0;
            int pos = i*p_cols;
            int start_time = time(NULL);
            int multiply_amount = 0;

            while(m_index < matrixes.size){

                // adjust pos
                while(pos >= matrixes.B[m_index].cols){
                    pos -= matrixes.B[m_index].cols;
                    pos -= pos % p_cols;
                    m_index++;
                }


                if(m_index >= matrixes.size){
                    exit(multiply_amount);
                }
                else{
                    if(matrixes.A[m_index].values != NULL && matrixes.B[m_index].values != NULL){
                        //printf("PID: %d    mi: %d    pos: %d \n", (int)getpid(), m_index, pos);
                        struct Matrix C = process_multiply_matrixes(matrixes.A[m_index], matrixes.B[m_index], pos, p_cols);
                        ++multiply_amount;

                        if(mode == 1){
                            save_to_file(C, pos, files.files[m_index].c);
                        }
                        else if(mode == 2){
                            save_to_file2(C, pos, files.files[m_index].c);
                        }

                        destroy_matrix(&C);

                        // time out
                        if(time(NULL) - start_time >= p_sec)
                            exit(multiply_amount);
                    }

                    // shift pos
                    pos += p_amount*p_cols;
                }
            }
            exit(multiply_amount);
        }
    }

    
    // wait
    int status;
    for(int i=0; i < p_amount; i++){
        pid_t pid = wait(&status);
        printf("Proces PID %d wykonał %d mnożeń macierzy\n", pid, WEXITSTATUS(status));
    }
    free(pids);

    // merge processes results
    for(int i=0; i < files.size; i++){
        if(matrixes.A[i].values != NULL && matrixes.B[i].values != NULL){
            if(mode == 1){
                make_file(files.files[i].c, matrixes.A[i].rows, matrixes.B[i].cols);
            }
            else if(mode == 2){
                int size = (matrixes.B[i].cols + 1) / p_cols; // how many files
                paste_files(i, size);
            }
        }
    }    
}



void help(){
    printf("macierz list_file proc_amount sec mode\n");
    printf("list_file - file from where are got matrixes files\n");
    printf("proc_amount - amount of child processes\n");
    printf("sec - amount of seconds, that each child process can run\n");
    printf("mode:\n");
    printf("\t1 - result is written to one common file\n");
    printf("\t2 - result is written to different files and merged by paste to one file\n");
}
