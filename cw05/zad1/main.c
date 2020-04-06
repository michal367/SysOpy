#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int get_char_amount(const char* string, char ch){
    int counter = 0;
    for(const char* c = string; *c != '\0'; c++)
        if(*c == ch)
            counter++;

    return counter;
}

char* trim(char *s){
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}


char** get_args(const char* command){
    int amount = get_char_amount(command, ' ') + 2;
    char** args = (char**)calloc(amount, sizeof(char*));
    
    char* str = strdup(command);
    char* pch = strtok(str, " ");
    int index = 0;
    while(pch != NULL){
        args[index] = strdup(pch);
        index++;
        pch = strtok(NULL, " ");
    }
    args[index] = NULL;

    free(str);

    //for(int i=0; i<amount-1; i++)
    //    printf("%s\n", args[i]);

    return args;
}


void start(const char* filename){
    FILE* file = fopen(filename, "r");
    if(file){

        size_t line_buf_size = 1024;
        char* line_buf = (char*)calloc(line_buf_size, sizeof(char));

        while(getline(&line_buf, &line_buf_size, file) >= 0){
            
            // skip empty lines
            if(line_buf[0] == '\n')
                continue;

            int commands_amount = get_char_amount(line_buf, '|') + 1;
            char** commands = (char**)calloc(commands_amount, sizeof(char*));

            // split command lines by '|' to single commands
            int index = 0;
            char* str = strdup(line_buf);
            char* pch = strtok(str,"|");
            while(pch != NULL){
                while(*pch == ' ')
                    pch++;
                commands[index] = strdup(pch);
                trim(commands[index]);
                index++;
                pch = strtok(NULL, "|");
            }

            // print commands
            if(line_buf[strlen(line_buf)-1] == '\n')
                line_buf[strlen(line_buf)-1] = '\0';
            printf("%s\n", line_buf);
            //for(int i=0; i<commands_amount; i++)
            //    printf("%s\n", commands[i]);


            // pipe
            int (*fd)[2] = calloc(commands_amount, sizeof(*fd));
            for(int i=0; i<commands_amount; i++)
                if(pipe(fd[i]) < 0){
                    printf("ERROR: pipe\n");
                    exit(1);
                }
            

            int i = 0;
            for(i=0; i<commands_amount; i++){                
                // fork
                pid_t pid = vfork(); // it has to be vfork
                                     // because fork while getline lead to infinite loop
                if(pid < 0){
                    printf("ERROR: fork\n");
                    exit(1);
                }
                else if(pid == 0){ // child
                    char** args = get_args(commands[i]);

                    // configure pipe output
                    if(i < commands_amount-1){
                        close(fd[i+1][0]);
                        if(dup2(fd[i+1][1], STDOUT_FILENO) < 0){
                            printf("ERROR: dup2 stdout\n");
                            exit(1);
                        }
                    }

                    // configure pipe input
                    if(i > 0){
                        close(fd[i][1]);
                        if(dup2(fd[i][0], STDIN_FILENO) < 0){
                            printf("ERROR: dup2 stdin\n");
                            exit(1);
                        }
                    }
                    
                    // execute command
                    execvp(args[0], args);
                    exit(1);
                }

                // close finished pipe
                close(fd[i][0]);
                close(fd[i][1]);
            }
            
            // wait for children
            for(int i=0; i<commands_amount; i++)
                wait(NULL);

            

            free(fd);
            free(str);
            for(int i=0; i<commands_amount; i++)
                free(commands[i]);
            free(commands);

            printf("\n");
        }
        
        free(line_buf);
        fclose(file);
    }
    else{
        printf("Can't open file %s\n", filename);
    }
}




void help(){
    printf("main command_file\n");
    printf("command_file - file that contains commands with pipes\n");
}


int main(int argc, char** argv){
    if(argc < 2){
        printf("Too few arguments\n");
        help();
        return -1;
    }

    char* file = argv[1];

    start(file);

    return 0;
}