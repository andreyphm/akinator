#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "akinator.h"
#include "font.h"

program_status_data action_request()
{
    char input_buffer[16] = {};
    int user_input = 0;

    printf(MAKE_BOLD(
        "Program should:\n"
        "1. Guess the character\n"
        "2. Output the tree to a TXT file\n"
        "3. Output the tree to a PNG file\n"
        "4. Enter info from file to tree\n"
        "5. Shut down\n"
        "Please, answer 1, 2, 3, 4 or 5\n"));

    if (!fgets(input_buffer, sizeof(input_buffer), stdin))
        return request_re_entry();

    input_buffer[strcspn(input_buffer, "\n")] = '\0';

    user_input = atoi(input_buffer);

    switch (user_input)
    {
        case GUESS_CHARACTER:
            return GUESS_CHARACTER;
        case TREE_TO_TXT_FILE:
            return TREE_TO_TXT_FILE;
        case TREE_TO_PNG_FILE:
            return TREE_TO_PNG_FILE;
        case FILE_TO_TREE:
            return FILE_TO_TREE;
        case PROGRAM_QUIT:
            return PROGRAM_QUIT;
        default:
            clear_input_buffer();
            return request_re_entry();
    }
}

program_status_data request_re_entry()
{
    char input_buffer[16] = {};
    int user_answer = 0;

    printf(MAKE_BOLD_RED(
        "Input is incorrect.\n"
        "Do you want to continue the program?\n"
        "Answer 1 to start again, anything else to exit:\n"));

    if (!fgets(input_buffer, sizeof(input_buffer), stdin))
        return PROGRAM_QUIT;

    input_buffer[strcspn(input_buffer, "\n")] = '\0';

    user_answer = atoi(input_buffer);

    return (user_answer == 1) ? PROGRAM_START_AGAIN : PROGRAM_QUIT;
}

void clear_input_buffer()
{
    int entered_character = 0;

    do {
        entered_character = getchar();
    } while (entered_character != '\n' && entered_character != EOF);
}
