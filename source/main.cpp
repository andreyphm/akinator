#include <assert.h>
#include <stdlib.h>

#include "akinator.h"
#include "font.h"

int main()
{
    program_status_data program_status = GUESS_CHARACTER;
    node_t* node = create_node("Unknown who");

    while (program_status != PROGRAM_QUIT)
    {
        program_status = action_request();
        switch (program_status)
        {
            case PROGRAM_QUIT:
            case PROGRAM_START_AGAIN:
                break;
            case GUESS_CHARACTER:
                guess_character(node);
                program_status = PROGRAM_START_AGAIN;
                break;
            case TREE_TO_TXT_FILE:
            {
                FILE* tree_txt_file = fopen("akinator_tree.txt", "w");
                print_tree_to_file(node, tree_txt_file);
                fclose(tree_txt_file);
                printf(MAKE_BOLD_GREEN("Successfully\n"));
            }
                break;
            case TREE_TO_PNG_FILE:
            {
                tree_dump(node, "akinator_tree.png");
                printf(MAKE_BOLD_GREEN("Tree visualization saved to %s\n"), "akinator_tree.png");
            }
                break;
            case FILE_TO_TREE:
            {
                free_tree(node);
                node = nullptr;

                FILE* tree_txt_file = fopen("akinator_tree.txt", "r");
                if (!tree_txt_file)
                {
                    printf(MAKE_BOLD_RED("Failed to open file\n"));
                    break;
                }
                char* buffer = read_file_to_buffer(tree_txt_file);


                if (!buffer)
                {
                    fclose(tree_txt_file);
                    break;
                }

                node_t* new_node = nullptr;
                buffer_to_tree(&new_node, buffer);

                free(buffer);
                fclose(tree_txt_file);

                if (new_node)
                {
                    free_tree(node);
                    node = new_node;
                }
                else
                    printf(MAKE_BOLD_RED("Failed to parse tree from file\n"));
                break;
            }
            default:
                break;
        }
    }
    printf(MAKE_BOLD("Program completed. COMMIT GITHUB\n"));
}
