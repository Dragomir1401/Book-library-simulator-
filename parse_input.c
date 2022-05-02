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

        if ((double)library->size / library->hmax >= 1)
            ht_resize(library, sizeof(tome), 0);
        free(buff);
        free(book_name);
        free(word);
        free(def);
        return;
    }
    // If we have the book already we just reset values and add new words

    tome *old_book = *(tome **)ht_get(library, book_name);
    old_book->details->rating = 0;
    old_book->details->borrowed = 0;
    old_book->details->nr_of_borrows = 0;
    old_book->details->days_available = 0;

    for (unsigned int i = 0; i < old_book->book->hmax; i++)
    {
        if (old_book->book->buckets[i]->size > 0)
        {
            ll_node_t *aux = old_book->book->buckets[i]->head;
            while (aux != NULL)
            {
                void *key = ((info *)aux->data)->key;
                aux = aux->next;
                if (ht_has_key(old_book->book, key) && !ht_has_key(volume->book, key))
                {
                    void *res = ht_remove_entry(old_book->book, key);
                    free(res);
                    old_book->book->size--;
                }
            }
        }
    }

    for (unsigned int i = 0; i < volume->book->hmax; i++)
    {
        if (volume->book->buckets[i]->size > 0)
        {
            ll_node_t *aux = volume->book->buckets[i]->head;
            while (aux)
            {
                void *value = ((info *)aux->data)->value;
                void *key = ((info *)aux->data)->key;
                ht_put(old_book->book, key, strlen(key) + 1, value, strlen(value) + 1);
                old_book->book->size++;
                aux = aux->next;
            }
        }
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
    memcpy(book_name, token, strlen(token) + 1);

    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        double rat;
        if (!volume->details->nr_of_borrows)
            rat = 0;
        else
            rat = volume->details->rating / volume->details->nr_of_borrows;
        printf("Name:%s Rating:%.3f Purchases:%d\n", book_name, rat, volume->details->nr_of_borrows);
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
    memcpy(book_name, token, strlen(token) + 1);

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
    memcpy(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memcpy(word, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *def = malloc(MAX_WORD);
    memcpy(def, token, strlen(token) + 1);

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

        if ((double)volume->book->size / volume->book->hmax >= 1)
            ht_resize(volume->book, MAX_WORD, 1);
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
    memcpy(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memcpy(word, token, strlen(token) + 1);

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
    memcpy(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memcpy(word, token, strlen(token) + 1);

    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        if (ht_has_key(volume->book, word))
        {
            void *res = ht_remove_entry(volume->book, word);
            free(res);
            volume->book->size--;
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

        if ((double)system->size / system->hmax >= 1)
            ht_resize(system, sizeof(person), 0);
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

    token = strtok(NULL, " ");
    char *user_name = malloc(MAX_USER_NAME);
    strcpy(user_name, token);

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);

    token = strtok(NULL, " \n");
    int days_available = atoi(token);

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
    {
        printf(ALREADY_BORROWED);
    }
    else if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        if (volume->details->borrowed)
        {
            printf(BORROWED);
        }
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

    person *user = *(person **)ht_get(system, user_name);
    tome *volume = *(tome **)ht_get(library, book_name);

    if (user->score < 0)
    {
        printf(BANNED);
        free(user_name);
        free(book_name);
        return;
    }

    if (strncmp(user->book_borrowed, book_name, strlen(book_name)) > 0 || strncmp(user->book_borrowed, book_name, strlen(book_name)) < 0)
    {
        printf(OTHER_BOOK);
        free(user_name);
        free(book_name);
        return;
    }

    if (!ht_has_key(library, book_name))
    {
        printf(OTHER_BOOK);
        free(user_name);
        free(book_name);
        return;
    }

    if (volume->details->days_available < days_since_borrowed)
        user->score -= 2 * (days_since_borrowed - volume->details->days_available);
    else if (volume->details->days_available > days_since_borrowed)
        user->score += volume->details->days_available - days_since_borrowed;

    if (user->score < 0)
        printf(BAN_MESSAGE, user_name);

    // Return action
    volume->details->nr_of_borrows++;
    user->borrowed_already = 0;
    strcpy(user->book_borrowed, "never_here\0");

    volume->details->borrowed = 0;
    volume->details->days_available = 0;
    volume->details->rating += rating;

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
    volume->details->rating = 0;
    volume->details->nr_of_borrows = 0;

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
