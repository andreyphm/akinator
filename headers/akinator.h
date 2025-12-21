#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>

typedef struct node_t node_t;

struct node_t
{
    const char* data;
    node_t* right;
    node_t* left;
};

enum program_status_data
{
    GUESS_CHARACTER     = 1,
    TREE_TO_TXT_FILE    = 2,
    TREE_TO_PNG_FILE    = 3,
    FILE_TO_TREE        = 4,
    PROGRAM_QUIT        = 5,
    PROGRAM_START_AGAIN = 6
};

enum user_answer_data
{
    NO  = 0,
    YES = 1
};

program_status_data action_request();
program_status_data request_re_entry();
void clear_input_buffer();
void print_tree_to_file(const node_t* node, FILE* const tree_txt_file);
program_status_data guess_character(node_t* const node);
node_t* create_node(const char* data, node_t* const node);
char* read_file_to_buffer(FILE* const tree_txt_file);
char* buffer_to_tree(node_t** node_ptr, char* buffer);
void free_tree(node_t* node);
void tree_dump(node_t* const node, const char* const png_file_name);
void dump_node(node_t* node, int id);

#endif //AKINATOR_H
