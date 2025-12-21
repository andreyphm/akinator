#include <assert.h>
#include <ctype.h>

#include "akinator.h"
#include "font.h"

program_status_data action_request()
{
    printf(MAKE_BOLD("Program should:\n1. Guess the character\n2. Output the tree to a TXT file\n"
                                      "3. Output the tree to a PNG file\n4. Enter info from file to tree\n"
                                      "5. Shut down\nPlease, answer 1, 2, 3, 4 or 5\n"));
    int user_input = 0;
    int result_of_scanf = scanf("%d", &user_input);
    int extra_symbol = 0;
    program_status_data program_status = GUESS_CHARACTER;

    if (result_of_scanf == 1)
    {
        extra_symbol = getchar();
        if (!(extra_symbol == '\n' || extra_symbol == EOF))
        {
            clear_input_buffer();
            program_status = request_re_entry();
        }

        else
        {
            switch (user_input)
            {
            case (int) GUESS_CHARACTER:
                return GUESS_CHARACTER;
            case (int) TREE_TO_TXT_FILE:
                return TREE_TO_TXT_FILE;
            case (int) TREE_TO_PNG_FILE:
                return TREE_TO_PNG_FILE;
            case (int) FILE_TO_TREE:
                return FILE_TO_TREE;
            case (int) PROGRAM_QUIT:
                return PROGRAM_QUIT;
            default:
                program_status = request_re_entry();
            }
        }
    }

    else
    {
        clear_input_buffer();
        program_status = request_re_entry();
    }

    return program_status;
}

program_status_data request_re_entry()
{
    program_status_data program_status = GUESS_CHARACTER;
    int user_answer = 0;

    printf(MAKE_BOLD_RED("Input is incorrect.\n"
                         "Do you want to continue the program?\n"
                         "Answer 1 to start again and write something else to exit:\n"));

    scanf("%d", &user_answer);

    if (user_answer != 1)
        program_status = PROGRAM_QUIT;

    else
    {
        int extra_symbol = 0;
        program_status = PROGRAM_START_AGAIN;

        do {
            extra_symbol = getchar();

            if(!isspace(extra_symbol))
            {
                program_status = PROGRAM_QUIT;
                clear_input_buffer();
                break;
            }

        } while(extra_symbol != '\n' && extra_symbol != EOF);
    }
    return program_status;
}

void clear_input_buffer()
{
    int entered_character = 0;

    do {
        entered_character = getchar();
    } while (entered_character != '\n' && entered_character != EOF);
}
