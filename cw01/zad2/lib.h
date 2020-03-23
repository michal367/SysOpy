#ifndef LIB_H
#define LIB_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef unsigned int uint;


struct Block{
    char** operations;
    size_t size;
};

struct Table{
    struct Block* blocks;
    size_t size;
};


// Table:
// Block:
// operations


struct Pair{
    char* first;
    char* second;
};
struct ArrayPairs{
    struct Pair* pairs;
    size_t size;
};




char *trim(char *s);
char* replace_char(char* str, char find, char replace);
uint find_pos_to_split(char* str, uint start_pos);
char* substring(char* str, uint pos1, uint pos2);


char* get_file_text(char* file_name);

void destroy(struct Table* ma);
void destroy_pairs(struct ArrayPairs* ap);

struct ArrayPairs* create_pairs(char* files);
struct Table* generate(struct ArrayPairs* ap);

size_t blocks_amount(struct Table* tab);
size_t operations_amount(struct Table* tab, size_t block_index);

void erase_block(struct Table* tab, size_t block_index);
void erase_operation(struct Table* tab, size_t block_index, size_t operation_index);


#endif // lib
