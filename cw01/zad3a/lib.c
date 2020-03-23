#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

char *trim(char *s) {
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}

char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}


uint find_pos_to_split(char* str, uint start_pos){
    for(char* p=str+start_pos; p != str+strlen(str)-1; ++p)
    {
        if(*p == '\n' && isdigit(*(p+1)))
            return (p+1)-str;
    }
    return -1;
}

char* substring(char* str, uint pos1, uint pos2){
    if(pos2 >= pos1){
        uint length = pos2 - pos1;
        char* substr = malloc(length);
        strncpy(substr, str+pos1, length);
        *(substr + length - 1) = '\0';
        return substr;
    }
    return NULL;
}


char* get_file_text(char* file_name){
    // read commands results
    FILE *file  = fopen(file_name, "r"); // read only
    if(file == NULL){  // check if file exists
        printf("ERROR: Could not open file\n");
        return NULL;
    }

    fseek (file, 0, SEEK_END);
    uint length = ftell (file);
    fseek (file, 0, SEEK_SET);
    char* fbuffer = malloc(length+1);

    uint i=0;
    while(!feof(file)) {
        char ch = (char)fgetc(file);
        if(ch != -1)
            fbuffer[i++] = ch;
    }
    fbuffer[i] = '\0';
    fclose(file);

    return fbuffer;
}



void destroy(struct Table* ma){
    if(ma != NULL){
        for(int i=0; i < ma->size; i++){
            for(int j=0; j < ma->blocks[i].size; j++)
                free(ma->blocks[i].operations[j]);
            free(ma->blocks[i].operations);
        }
        free(ma->blocks);
        free(ma);
    }
}
void destroy_pairs(struct ArrayPairs* ap){
    if(ap != NULL){
        for(int i=0; i<ap->size; i++){
            free(ap->pairs[i].first);
            free(ap->pairs[i].second);
        }
        free(ap->pairs);
        free(ap);
    }
}


struct ArrayPairs* create_pairs(char* files){
    char* tmp = strdup(files);
    tmp = trim(tmp);
    tmp = replace_char(tmp, ':', ' ');

    if(strcmp(tmp, "") != 0){ // tmp != ""

        char *str, *pch;
        str = strdup(tmp);

        // splitting
        uint amount = 0;
        pch = strtok(str," ");
        while(pch != NULL){
            amount++;
            pch = strtok(NULL, " ");
        }

        if(amount%2 == 0){

            struct ArrayPairs* pairs = (struct ArrayPairs*)malloc(sizeof(struct ArrayPairs));
            pairs->pairs = (struct Pair*)calloc(amount/2, sizeof(struct Pair));
            pairs->size = amount/2;

            uint pair = 0, i=0;
            str = strdup(tmp);
            pch = strtok(str," ");
            while(pch != NULL){

                if(strlen(pch) > 100){
                    //destroy(main_array);
                    printf("ERROR: Too long name of file\n");
                    return NULL;
                }

                pair++;
                if(pair == 1){
                    pairs->pairs[i].first = strdup(pch);
                }
                else if(pair == 2){
                    pairs->pairs[i].second = strdup(pch);

                    pair = 0;
                    i++;
                }
                pch = strtok(NULL, " ");
            }
            return pairs;
        }
        else
            printf("ERROR: Amount of files are not even");
    }
    else
        printf("ERROR: There are no files");
    return NULL;
}


struct Table* generate(struct ArrayPairs* ap){
    if(ap != NULL){
        // allocating main array
        struct Table* main_array = (struct Table*)malloc(sizeof(struct Table));
        main_array->blocks = (struct Block*)calloc(ap->size, sizeof(struct Block));
        main_array->size = ap->size;

        // executing diff system command
        char command[] = "diff ";
        char file_name[] = "temp.txt";


        char buffer[500];
        for(uint i=0; i < ap->size; i++){
            strcpy(buffer, command);
            strcat(buffer, ap->pairs[i].first);
            strcat(buffer, " ");
            strcat(buffer, ap->pairs[i].second);
            strcat(buffer, " > ");
            strcat(buffer, file_name);

            // executing system command
            printf("%s\n", buffer);
            system(buffer);

            // get text from file
            char* file_text = get_file_text(file_name);
            if(file_text == NULL)
                destroy(main_array);

            if(strcmp(file_text, "") != 0){ // file_text != ""
                // checking how many operations
                uint pos = 0;
                uint operations_amount = 0;
                while(pos != -1){
                    operations_amount++;
                    pos = find_pos_to_split(file_text, pos+1);
                }

                // allocating memory for operations
                main_array->blocks[i].operations = (char**)calloc(operations_amount, sizeof(char*));
                main_array->blocks[i].size = operations_amount;


                // setting operations to array
                pos = 0;
                uint j = 0;
                while(pos != -1){

                    uint pos2 = find_pos_to_split(file_text, pos+1);
                    if(pos2 != -1){
                        main_array->blocks[i].operations[j] = substring(file_text, pos, pos2);
                        j++;
                    }
                    else
                        main_array->blocks[i].operations[j] = substring(file_text, pos, strlen(file_text));
                    pos = pos2;
                }
            }
            else
                main_array->blocks[i].size = 0;

            free(file_text);
        }

        return main_array;
    }
    return NULL;
}


size_t blocks_amount(struct Table* tab){
    return tab->size;
}
size_t operations_amount(struct Table* tab, size_t block_index){
    return tab->blocks[block_index].size;
}


void erase_block(struct Table* tab, size_t block_index){
    if(block_index < blocks_amount(tab)){
        size_t length = blocks_amount(tab);

        struct Block* new_blocks = (struct Block*)calloc(length - 1, sizeof(struct Block));
        for(uint i=0, j=0; j < length; i++,j++){
            if(i == block_index)
                j++;
            if(j < length){
                new_blocks[i].size = tab->blocks[j].size;
                new_blocks[i].operations = (char**)calloc(tab->blocks[j].size, sizeof(char*));

                for(uint k=0; k < operations_amount(tab, j); k++)
                    new_blocks[i].operations[k] = strdup(tab->blocks[j].operations[k]);
            }
        }

        for(int i=0; i < tab->size; i++){
            for(int j=0; j < tab->blocks[i].size; j++)
                free(tab->blocks[i].operations[j]);
            free(tab->blocks[i].operations);
        }
        free(tab->blocks);
        tab->size = length - 1;
        tab->blocks = new_blocks;
    }
    else
        printf("ERROR: Out of range\n");
}
void erase_operation(struct Table* tab, size_t block_index, size_t operation_index){
    if(block_index < blocks_amount(tab) && operation_index < operations_amount(tab, block_index)){

        struct Block* block_ptr = &(tab->blocks[block_index]);
        size_t length = operations_amount(tab, block_index);

        char** new_operations = (char**)calloc(length - 1, sizeof(char*));

        for(uint i=0, j=0; j < length; i++,j++){
            if(i == operation_index)
                j++;
            if(j < length)
                new_operations[i] = strdup(block_ptr->operations[j]);
        }

        block_ptr->size = length-1;
        for(uint i=0; i<length; i++)
            free(block_ptr->operations[i]);
        free(block_ptr->operations);
        block_ptr->operations = new_operations;
    }
    else
        printf("ERROR: Out of range\n");
}
