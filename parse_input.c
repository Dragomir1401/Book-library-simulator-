#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BOOK_NAME 100
#define BUFFER_MAX 300
#include "headers.h"
#include "structs.h"
#include "errors.h"
#define MAX_WORD 100
void parse_add_book_input(char *token, hashtable_t **library)
{
    char *buff = malloc(BUFFER_MAX);
    char separators[3];
    separators[0] = ' ';
    separators[1] = '\"';
    separators[2] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);
    printf("%s--\n", book_name);
    token = strtok(NULL, separators);
    int def_number = atoi(token);

    tome *volume = malloc(sizeof(tome));
    volume->book = ht_create(def_number, hash_function_string, compare_function_strings);
    volume->details = malloc(sizeof(details));
    volume->details->pieces_sold = 0;
    volume->details->rating = 0;

    char *word = malloc(MAX_WORD);
    char *def = malloc(MAX_WORD);

    while (def_number--)
    {
        fgets(buff, BUFFER_MAX, stdin);
        char *tmp = strtok(buff, " \n");
        strcpy(word, tmp);
        tmp = strtok(NULL, " \n");
        strcpy(def, tmp);

        ht_put(volume->book, word, strlen(word) + 1, def, strlen(def) + 1);
    }

    ht_put(*library, book_name, strlen(book_name) + 1, &volume, sizeof(tome));

    free(buff);
    free(book_name);
    free(word);
    free(def);
}

void parse_get_book_input(char *token, hashtable_t *library)
{
    char separators[3];
    separators[0] = ' ';
    separators[1] = '\'';
    separators[2] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);

    printf("%d\n", ht_has_key(library, book_name));

    if (ht_has_key(library, book_name))
    {
        tome *volume = (tome *)ht_get(library, book_name);
        printf("Name:%s Rating:%d Purchase:%d\n", book_name, volume->details->rating, volume->details->pieces_sold);
    }
    else
    {
        printf(NOT_IN_LIBRARY);
    }
}