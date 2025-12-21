#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "akinator.h"
#include "font.h"

int main()
{
    program_status_data program_status = GUESS_CHARACTER;
    node_t node = {"Unknown who", nullptr, nullptr};

    while (program_status != PROGRAM_QUIT)
    {
        program_status = action_request();
        switch (program_status)
        {
            case PROGRAM_QUIT:
            case PROGRAM_START_AGAIN:
                break;
            case GUESS_CHARACTER:
                guess_character(&node);
                break;
            case TREE_TO_TXT_FILE:
            {
                FILE* tree_txt_file = fopen("akinator_tree.txt", "w");
                print_tree_to_file(&node, tree_txt_file);
                fclose(tree_txt_file);
                printf(MAKE_BOLD_GREEN("Successfully\n"));
            }
                break;
            case TREE_TO_PNG_FILE:
            {
                tree_dump(&node, "akinator_tree.png");
                printf(MAKE_BOLD_GREEN("Tree visualization saved to %s\n"), "akinator_tree.png");
            }
                break;
            case FILE_TO_TREE:
            {
                free_tree(&node);

                FILE* tree_txt_file = fopen("akinator_tree.txt", "r");
                char* buffer = read_file_to_buffer(tree_txt_file);

                node_t* new_node_ptr = nullptr;
                buffer_to_tree(&new_node_ptr, buffer);
                node = *new_node_ptr;

                free(buffer);
                fclose(tree_txt_file);
                printf(MAKE_BOLD_GREEN("Successfully\n"));
            }
            default:
                break;
        }
    }

    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}

program_status_data guess_character(node_t* const node)
{
    program_status_data program_status = GUESS_CHARACTER;
    int user_answer = 0;
    char buffer[100] = {};
    int extra_symbol = 0;

    if (node->right && node->left)
        printf(MAKE_BOLD("Your character %s?\nAnswer 1 if yes or 0 if not\n"), node->data);
    else
        printf(MAKE_BOLD("It's %s?\nAnswer 1 if yes or 0 if not\n"), node->data);

    scanf("%d", &user_answer);

    if (user_answer != (int)YES && user_answer != (int)NO)
        program_status = request_re_entry();

    else
    {
        extra_symbol = getchar();
        if (!(extra_symbol == '\n' || extra_symbol == EOF))
        {
            clear_input_buffer();
            program_status = request_re_entry();
        }

        else
        {
            switch (user_answer)
            {
            case (int)YES:
                if (node->left)
                    guess_character(node->left);
                else
                    printf(MAKE_BOLD_GREEN("You thought of %s!\n"), node->data);
                break;
            case (int)NO:
                if (node->right)
                    guess_character(node->right);
                else
                {
                    printf(MAKE_BOLD("Who did you guess?\n"));
                    char* new_object = fgets(buffer, 100, stdin);
                    new_object[strcspn(new_object, "\n")] = '\0';

                    printf(MAKE_BOLD("What is its difference from %s?\n"), node->data);
                    char* new_feature = fgets(buffer + strlen(buffer) + 1, 100, stdin);
                    new_feature[strcspn(new_feature, "\n")] = '\0';

                    const char* old_node_data = node->data;
                    node->data = strdup(new_feature);
                    node->left = create_node(new_object, node->left);
                    node->right = create_node(old_node_data, node->right);

                    printf(MAKE_BOLD_GREEN("%s has been added\n"), new_object);
                }
                break;
            default:
                program_status = request_re_entry();
            }
        }
    }
    return program_status;
}

void print_tree_to_file(const node_t* node, FILE* const tree_txt_file)
{
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

node_t* create_node(const char* data, node_t* node)
{
    assert(data);

    node = (node_t*) calloc(1, sizeof(node_t));

    assert(node);

    node->data = strdup(data);
    node->left = nullptr;
    node->right = nullptr;

    return node;
}

char* buffer_to_tree(node_t** const node_ptr, char* buffer)
{
    assert(buffer);

    while (isspace(*buffer)) buffer++;

    if (*buffer == '(')
    {
        buffer++;

        while (isspace(*buffer)) buffer++;
        buffer++;

        char* start_of_buffer = buffer;
        while (*buffer != '\"' && *buffer != '\0') buffer++;
        size_t new_object_len = buffer - start_of_buffer;

        char* data = (char*)calloc(new_object_len + 1, sizeof(char));
        strncpy(data, start_of_buffer, new_object_len);
        data[new_object_len] = '\0';

        *node_ptr = create_node(data, nullptr);
        free(data);

        buffer++;

        buffer = buffer_to_tree(&((*node_ptr)->left), buffer);
        buffer = buffer_to_tree(&((*node_ptr)->right), buffer);

        while (isspace(*buffer)) buffer++;
        if (*buffer == ')') buffer++;
    }

    else if (strncmp(buffer, "nil", 3) == 0)
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
    size_t file_size = file_struct.st_size;

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
    assert(node);
    assert(png_file_name);

    FILE* txt_file = fopen("dump.txt", "w");
    assert(txt_file);

    fprintf(txt_file, "digraph G {\n");
    fprintf(txt_file, "    rankdir=TB;\n");
    fprintf(txt_file, "    node [shape=record, fontname=\"Arial\"];\n");
    fprintf(txt_file, "    edge [color=\"black\", fontname=\"Arial\", fontsize=10];\n\n");

    struct
    {
        node_t* node;
        int index;
    } stack[1000];

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
            stack[stack_size].node = current->left;
            stack[stack_size].index = left_index;
            stack_size++;
        }

        if (current->right)
        {
            int right_index = node_index++;

            fprintf(txt_file, "    n%d:no -> n%d [color=\"black\", constraint=true];\n", current_index, right_index);
            stack[stack_size].node = current->right;
            stack[stack_size].index = right_index;
            stack_size++;
        }
    }

    fprintf(txt_file, "}\n");
    fclose(txt_file);

    char command[100];
    sprintf(command, "dot -Tpng dump.txt -o %s", png_file_name);
    system(command);
}
