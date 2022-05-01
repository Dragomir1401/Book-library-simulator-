#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "structs.h"
#include "headers.h"
#define BUFFER_MAX 300
#define MAX_HASHTABLE 7
int main(void)
{
    char *buff = malloc(BUFFER_MAX);
    char *buff1 = malloc(BUFFER_MAX);

    hashtable_t *library = ht_create(MAX_HASHTABLE, hash_function_string, compare_function_strings);
    hashtable_t *system = ht_create(MAX_HASHTABLE, hash_function_string, compare_function_strings);
    char separators[3];
    separators[0] = ' ';
    separators[1] = '\"';
    separators[2] = '\n';
    while (1)
    {
        fgets(buff, BUFFER_MAX, stdin);
        strcpy(buff1, buff);
        char *token = strtok(buff, separators);
        if (!strncmp(token, "ADD_BOOK", 8))
            parse_add_book_input(token, library);
        else if (!strncmp(token, "GET_BOOK", 8))
            parse_get_book_input(token, library);
        else if (!strncmp(token, "RMV_BOOK", 8))
            parse_rmv_book_input(token, library);
        else if (!strncmp(token, "ADD_DEF", 7))
            parse_add_def_input(token, library);
        else if (!strncmp(token, "GET_DEF", 7))
            parse_get_def_input(token, library);
        else if (!strncmp(token, "RMV_DEF", 7))
            parse_rmv_def_input(token, library);
        else if (!strncmp(token, "ADD_USER", 8))
            parse_add_user_input(token, system);
        else if (!strncmp(token, "BORROW", 6))
        {
            FILE *output = fopen("debug", "a+");
            if (strstr(buff1, "Hogan"))
                fprintf(output, "%s\n", buff1);
            fclose(output);
            parse_borrow_input(token, library, system);
        }
        else if (!strncmp(token, "RETURN", 6))
        {
            FILE *output = fopen("debug", "a+");
            if (strstr(buff1, "Hogan"))
                fprintf(output, "%s\n", buff1);
            fclose(output);
            parse_return_input(token, library, system);
        }
        else if (!strncmp(token, "LOST", 4))
            parse_lost_input(token, library, system);
        else if (!strncmp(token, "EXIT", 4))
        {
            top_books(library);
            top_users(system);
            free_library(library);
            free_system(system);
            break;
        }
        else
            printf(INVALID_COMMAND);
    }
    free(buff);
    free(buff1);
    return 0;
}