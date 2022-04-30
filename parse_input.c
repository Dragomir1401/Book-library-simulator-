#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BOOK_NAME 100
#define BUFFER_MAX 300
#define MAX_USER_NAME 100
#include "headers.h"
#include "structs.h"
#include "errors.h"
#define MAX_WORD 100
#define HASHTABLE_MAX 7
void parse_add_book_input(char *token, hashtable_t *library)
{
    char *buff = malloc(BUFFER_MAX);
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);
    token = strtok(NULL, " \n");
    int def_number = atoi(token);

    tome *volume = malloc(sizeof(tome));
    volume->book = ht_create(HASHTABLE_MAX, hash_function_string, compare_function_strings);
    volume->details = malloc(sizeof(details));
    volume->details->rating = 0;
    volume->details->borrowed = 0;
    volume->details->nr_of_borrows = 0;
    volume->details->days_available = 0;

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
        volume->book->size++;
    }

    if (!ht_has_key(library, book_name))
    {
        ht_put(library, book_name, strlen(book_name) + 1, &volume, sizeof(tome));
        library->size++;
        free(buff);
        free(book_name);
        free(word);
        free(def);
        return;
    }

    ht_free(volume->book);
    free(volume->details);
    free(volume);
    free(buff);
    free(book_name);
    free(word);
    free(def);
}

void parse_get_book_input(char *token, hashtable_t *library)
{
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memmove(book_name, token, strlen(token) + 1);

    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        // printf("--%s--\n", *(char **)library->buckets[library->hash_function(book_name) % library->hmax]->head->data);
        printf("Name:%s Rating:%.3f Purchases:%d\n", book_name, volume->details->rating, volume->details->nr_of_borrows);
        // printf("%d\n", ht_has_key(volume->book, "cal"));
    }
    else
        printf(NOT_IN_LIBRARY);

    free(book_name);
}

void parse_rmv_book_input(char *token, hashtable_t *library)
{
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memmove(book_name, token, strlen(token) + 1);

    if (ht_has_key(library, book_name))
    {
        void *res = ht_remove_entry(library, book_name);
        tome *volume = *(tome **)res;
        free_volume(volume);

        free(res);
        library->size--;
    }
    else
        printf(NOT_IN_LIBRARY);
    free(book_name);
}

void parse_add_def_input(char *token, hashtable_t *library)
{
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memmove(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memmove(word, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *def = malloc(MAX_WORD);
    memmove(def, token, strlen(token) + 1);

    // printf("%s %s %s\n", book_name, word, def);

    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        if (!ht_has_key(volume->book, word))
        {
            ht_put(volume->book, word, strlen(word) + 1, def, strlen(def) + 1);
            volume->book->size++;
        }
        else
        {
            ht_put(volume->book, word, strlen(word) + 1, def, strlen(def) + 1);
        }

        // printf("%d\n", ht_has_key(volume->book, word));
    }
    else
        printf(NOT_IN_LIBRARY);
    free(book_name);
    free(word);
    free(def);
}

void parse_get_def_input(char *token, hashtable_t *library)
{
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memmove(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memmove(word, token, strlen(token) + 1);

    // printf("%s %s %s\n", book_name, word, def);

    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        if (ht_has_key(volume->book, word))
        {
            char *def = (char *)ht_get(volume->book, word);
            printf("%s\n", def);
        }
        else
            printf(NOT_IN_BOOK);
    }
    else
        printf(NOT_IN_LIBRARY);

    free(book_name);
    free(word);
}

void parse_rmv_def_input(char *token, hashtable_t *library)
{
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memmove(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memmove(word, token, strlen(token) + 1);
    // printf("%s %s %s\n", book_name, word, def);

    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        if (ht_has_key(volume->book, word))
        {
            void *res = ht_remove_entry(volume->book, word);
            free(res);
            volume->book->size--;
            // printf("%d\n", ht_has_key(volume->book, word));
        }
        else
            printf(NOT_IN_BOOK);
    }
    else
        printf(NOT_IN_LIBRARY);

    free(book_name);
    free(word);
}

void parse_add_user_input(char *token, hashtable_t *system)
{
    char separators[2];
    separators[0] = ' ';
    separators[1] = '\n';

    token = strtok(NULL, separators);
    char *user_name = malloc(MAX_USER_NAME);
    strcpy(user_name, token);

    if (!ht_has_key(system, user_name))
    {
        person *user = malloc(sizeof(person));
        user->book_borrowed = malloc(MAX_BOOK_NAME);
        strcpy(user->book_borrowed, "");
        user->borrowed_already = 0;
        user->score = 100;
        ht_put(system, user_name, strlen(user_name) + 1, &user, sizeof(person));
        system->size++;
    }
    else
    {
        printf(ALREADY_REGISTERED);
    }

    free(user_name);
}

void parse_borrow_input(char *token, hashtable_t *library, hashtable_t *system)
{

    char separators[2];
    separators[0] = '\n';
    separators[1] = '\"';

    token = strtok(NULL, " \n");
    char *user_name = malloc(MAX_USER_NAME);
    strcpy(user_name, token);

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);

    token = strtok(NULL, " \n");
    int days_available = atoi(token);

    //printf("%s %s %d\n", user_name, book_name, days_available);

    if (!ht_has_key(system, user_name))
    {
        printf(NOT_REGISTERED);
        free(user_name);
        free(book_name);
        return;
    }

    person *user = *(person **)ht_get(system, user_name);

    if (user->score < 0)
    {
        printf(BANNED);
        free(user_name);
        free(book_name);
        return;
    }

    if (user->borrowed_already)
        printf(ALREADY_BORROWED);
    else if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        if (volume->details->borrowed)
            printf(BORROWED);
        else
        {
            volume->details->borrowed = 1;
            volume->details->days_available = days_available;
            user->borrowed_already = 1;
            strcpy(user->book_borrowed, book_name);
        }
    }
    else
    {
        printf(NOT_IN_LIBRARY);
    }

    free(user_name);
    free(book_name);
}

void parse_return_input(char *token, hashtable_t *library, hashtable_t *system)
{

    char separators[2];
    separators[0] = '\n';
    separators[1] = '\"';

    token = strtok(NULL, " \n");
    char *user_name = malloc(MAX_USER_NAME);
    strcpy(user_name, token);

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);

    token = strtok(NULL, " \n");
    int days_since_borrowed = atoi(token);

    token = strtok(NULL, " \n");
    int rating = atoi(token);

    //printf("%s %s %d %d \n", user_name, book_name, days_since_borrowed, rating);

    person *user = *(person **)ht_get(system, user_name);
    tome *volume = *(tome **)ht_get(library, book_name);

    if (user->score < 0)
    {
        printf(BANNED);
        free(user_name);
        free(book_name);
        return;
    }

    if (!ht_has_key(library, book_name) || strcmp(user->book_borrowed, book_name))
    {
        printf(OTHER_BOOK);
        free(user_name);
        free(book_name);
        return;
    }

    if (volume->details->days_available < days_since_borrowed)
        user->score -= 2 * (days_since_borrowed - volume->details->days_available);
    else
        user->score += (volume->details->days_available - days_since_borrowed);

    if (user->score < 0)
        printf(BAN_MESSAGE, user_name);

    // Return action
    volume->details->nr_of_borrows++;
    user->borrowed_already = 0;
    volume->details->borrowed = 0;
    volume->details->days_available = 0;
    volume->details->rating = (volume->details->rating * (volume->details->nr_of_borrows - 1) + rating) / (float)volume->details->nr_of_borrows;
    // printf("BOOK: %s  RATING: %f\n", book_name, volume->details->rating);

    free(user_name);
    free(book_name);
}

void parse_lost_input(char *token, hashtable_t *library, hashtable_t *system)
{

    char separators[2];
    separators[0] = '\n';
    separators[1] = '\"';

    token = strtok(NULL, " \n");
    char *user_name = malloc(MAX_USER_NAME);
    strcpy(user_name, token);

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);

    //printf("%s %s %d %d \n", user_name, book_name, days_since_borrowed, rating);
    person *user;
    if (ht_has_key(system, user_name))
        user = *(person **)ht_get(system, user_name);
    else
    {
        printf(NOT_REGISTERED);
        free(user_name);
        free(book_name);
        return;
    }

    tome *volume = *(tome **)ht_get(library, book_name);

    if (user->score < 0)
    {
        printf(BANNED);
        free(user_name);
        free(book_name);
        return;
    }
    else
    {
        user->score -= 50;

        if (user->score < 0)
            printf(BAN_MESSAGE, user_name);
    }

    user->borrowed_already = 0;
    volume->details->borrowed = 0;
    volume->details->days_available = 0;

    if (ht_has_key(library, book_name))
    {
        void *res = ht_remove_entry(library, book_name);
        tome *volume = *(tome **)res;
        free_volume(volume);

        free(res);
        library->size--;
    }
    else
        printf(NOT_IN_LIBRARY);

    free(user_name);
    free(book_name);
}
