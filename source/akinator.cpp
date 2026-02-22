#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "akinator.h"
#include "font.h"

void guess_character(node_t* node)
{
    assert(node);

    char input_buffer[16]    = {};
    char object_buffer[100]  = {};
    char feature_buffer[100] = {};

    while (true)
    {
        if (node->left && node->right)
            printf(MAKE_BOLD("Your character %s?\nAnswer 1 if yes or 0 if not\n"), node->data);
        else
            printf(MAKE_BOLD("It's %s?\nAnswer 1 if yes or 0 if not\n"), node->data);

        if (!fgets(input_buffer, sizeof(input_buffer), stdin))
            return;

        input_buffer[strcspn(input_buffer, "\n")] = '\0';

        int user_answer = atoi(input_buffer);

        if (user_answer != YES && user_answer != NO)
        {
            printf(MAKE_BOLD_RED("Invalid input. Returning to menu.\n"));
            return;
        }

        if (user_answer == YES)
        {
            if (node->left)
            {
                node = node->left;
                continue;
            }
            else
            {
                printf(MAKE_BOLD_GREEN("You thought of %s!\n"), node->data);
                return;
            }
        }
        else
        {
            if (node->right)
            {
                node = node->right;
                continue;
            }
            else
            {
                printf(MAKE_BOLD("Who did you guess?\n"));
                if (!fgets(object_buffer, sizeof(object_buffer), stdin))
                    return;
                object_buffer[strcspn(object_buffer, "\n")] = '\0';

                printf(MAKE_BOLD("What is its difference from %s?\n"), node->data);
                if (!fgets(feature_buffer, sizeof(feature_buffer), stdin))
                    return;
                feature_buffer[strcspn(feature_buffer, "\n")] = '\0';

                char* old_data = node->data;
                char* new_feature = strdup(feature_buffer);
                node_t* new_left = create_node(object_buffer);
                node_t* new_right = create_node(old_data);

                if (!new_feature || !new_left || !new_right)
                {
                    free(new_feature);
                    free_tree(new_left);
                    free_tree(new_right);
                    printf(MAKE_BOLD_RED("Memory allocation failed!\n"));
                    return;
                }

                free(node->data);
                node->data = new_feature;
                node->left = new_left;
                node->right = new_right;

                printf(MAKE_BOLD_GREEN("%s has been added\n"), object_buffer);
                return;
            }
        }
    }
}

void print_tree_to_file(const node_t* node, FILE* const tree_txt_file)
{
    assert(tree_txt_file);

    if (!node)
    {
        fprintf(tree_txt_file, "nil ");
        return;
    }

    fprintf(tree_txt_file, "(");
    fprintf(tree_txt_file, "\"%s\" ", node->data);

    if (node->left)
        print_tree_to_file(node->left, tree_txt_file);
    else
        fprintf(tree_txt_file, "nil ");

    if (node->right)
        print_tree_to_file(node->right, tree_txt_file);
    else
        fprintf(tree_txt_file, "nil");

    fprintf(tree_txt_file, ")");
}

node_t* create_node(const char* data)
{
    assert(data);

    node_t* node = (node_t*) calloc(1, sizeof(node_t));

    assert(node);

    node->data = strdup(data);
    assert(node->data);

    node->left = nullptr;
    node->right = nullptr;

    return node;
}

char* buffer_to_tree(node_t** const node_ptr, char* buffer)
{
    assert(buffer);

    while (isspace((unsigned char)*buffer)) buffer++;

    if (*buffer == '(')
    {
        buffer++;

        while (isspace((unsigned char)*buffer)) buffer++;

        if (*buffer != '\"')
        {
            *node_ptr = nullptr;
            return buffer;
        }
        buffer++;

        char* start_of_buffer = buffer;
        while (*buffer && *buffer != '\"') buffer++;

        if (*buffer != '\"')
        {
            *node_ptr = nullptr;
            return buffer;
        }

        size_t new_object_len = (size_t) (buffer - start_of_buffer);

        char* data = (char*)calloc(new_object_len + 1, sizeof(char));
        if (!data)
        {
            *node_ptr = nullptr;
            return buffer;
        }
        strncpy(data, start_of_buffer, new_object_len);
        data[new_object_len] = '\0';

        *node_ptr = create_node(data);
        free(data);

        buffer++;

        if (*node_ptr)
        {
            buffer = buffer_to_tree(&((*node_ptr)->left), buffer);
            buffer = buffer_to_tree(&((*node_ptr)->right), buffer);
        }

        while (isspace((unsigned char)*buffer)) buffer++;
        if (*buffer == ')') buffer++;
    }

    else if (buffer[0] == 'n' && buffer[1] == 'i' && buffer[2] == 'l')
    {
        *node_ptr = nullptr;
        buffer += 3;
    }

    return buffer;
}

char* read_file_to_buffer(FILE* const tree_txt_file)
{
    assert(tree_txt_file);

    struct stat file_struct = {};
    fstat(fileno(tree_txt_file), &file_struct);
    size_t file_size = (size_t)file_struct.st_size;

    char* buffer = (char*) calloc(file_size + 1, sizeof(*buffer));
    file_size = fread(buffer, sizeof(*buffer), file_size, tree_txt_file);
    *(buffer + file_size) = '\0';

    return buffer;
}

void free_tree(node_t* node)
{
    if (!node) return;

    free_tree(node->left);
    free_tree(node->right);

    free((void*)node->data);
    free(node);
}

void tree_dump(node_t* const node, const char* const png_file_name)
{
    if (!node)
    {
        printf(MAKE_BOLD_RED("Tree is empty, nothing to dump\n"));
        return;
    }
    assert(png_file_name);

    FILE* txt_file = fopen("dump.txt", "w");
    assert(txt_file);

    fprintf(txt_file, "digraph G {\n");
    fprintf(txt_file, "    rankdir=TB;\n");
    fprintf(txt_file, "    node [shape=record, fontname=\"Arial\"];\n");
    fprintf(txt_file, "    edge [color=\"black\", fontname=\"Arial\", fontsize=10];\n\n");

    struct stack_element
    {
        node_t* node;
        int index;
    };

    struct stack_element* stack = (stack_element*) calloc(1000, sizeof(stack_element));
    assert(stack);

    int stack_size = 0;
    int node_index = 0;

    stack[stack_size].node = node;
    stack[stack_size].index = node_index++;
    stack_size++;

    while (stack_size > 0)
    {
        stack_size--;
        node_t* current = stack[stack_size].node;
        int current_index = stack[stack_size].index;

        fprintf(txt_file, "    n%d [label=\"{ <q> %s? | { <yes> YES | <no> NO } }\", "
                "style=filled, fillcolor=\"#ffffffff\"];\n",
                current_index, current->data ? current->data : "NULL");

        if (current->left)
        {
            int left_index = node_index++;

            fprintf(txt_file,"    n%d:yes -> n%d [color=\"black\", constraint=true];\n", current_index, left_index);
            if (stack_size < 1000)
            {
                stack[stack_size].node = current->left;
                stack[stack_size].index = left_index;
                stack_size++;
            }
        }

        if (current->right)
        {
            int right_index = node_index++;

            fprintf(txt_file, "    n%d:no -> n%d [color=\"black\", constraint=true];\n", current_index, right_index);
            if (stack_size < 1000)
            {
                stack[stack_size].node = current->right;
                stack[stack_size].index = right_index;
                stack_size++;
            }
        }
    }

    fprintf(txt_file, "}\n");
    fclose(txt_file);
    free(stack);

    char command[100];
    sprintf(command, "dot -Tpng dump.txt -o %s", png_file_name);
    system(command);
}
