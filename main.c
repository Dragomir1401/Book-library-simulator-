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
    hashtable_t *library = ht_create(MAX_HASHTABLE, hash_function_string, compare_function_strings);
    char separators[3];
    separators[0] = ' ';
    separators[1] = '\'';
    separators[2] = '\n';

    while (1)
    {
        fgets(buff, BUFFER_MAX, stdin);
        char *token = strtok(buff, separators);
        if (!strncmp(token, "ADD_BOOK", 8))
            parse_add_book_input(token, &library);
        else if (!strncmp(token, "GET_BOOK", 8))
            parse_get_book_input(token, library);
        else if (!strncmp(token, "EXIT", 4))
            free_resources(library);
        else
            printf(INVALID_COMMAND);
    }
    ht_free(library);
    free(buff);
    return 0;
}