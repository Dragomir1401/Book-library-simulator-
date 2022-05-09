// Copyright 2022 Dragomir Andrei
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"
#include "structs.h"
#include "errors.h"
// Macros for allocations
#define MAX_BOOK_NAME 100
#define BUFFER_MAX 300
#define MAX_USER_NAME 100
#define MAX_WORD 100
#define HASHTABLE_MAX 7

void parse_add_book_input(char *token, hashtable_t *library)
{
    // Function which adds a book in library

    // Alloc buffer to get each word in book
    char *buff = malloc(BUFFER_MAX);

    // Separators to get full book name
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    // Get book name and number of words in book
    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);
    token = strtok(NULL, " \n");
    int def_number = atoi(token);

    // Create a book with all the definitons from input
    tome *volume = malloc(sizeof(tome));
    volume->book = ht_create(HASHTABLE_MAX, hash_function_string,
                             compare_function_strings);
    volume->details = malloc(sizeof(details));
    // Initialise book details
    volume->details->rating = 0;
    volume->details->borrowed = 0;
    volume->details->nr_of_borrows = 0;
    volume->details->days_available = 0;

    // Alloc word and def for parsing input
    char *word = malloc(MAX_WORD);
    char *def = malloc(MAX_WORD);

    while (def_number--)
    {
        // Get line and separate in word and def
        fgets(buff, BUFFER_MAX, stdin);
        char *tmp = strtok(buff, " \n");
        strcpy(word, tmp);
        tmp = strtok(NULL, " \n");
        strcpy(def, tmp);

        // Put entry in book and increase size
        ht_put(volume->book, word, strlen(word) + 1, def,
               strlen(def) + 1);
        volume->book->size++;
    }

    if (!ht_has_key(library, book_name))
    {
        // If we dont already have the book add it to library
        ht_put(library, book_name, strlen(book_name) + 1,
               &volume, sizeof(tome));
        library->size++;

        // Free allocations and leave
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

    // Remove entries which new hashtable doesnt have but old does
    for (unsigned int i = 0; i < old_book->book->hmax; i++)
    {
        // For each bucket
        if (old_book->book->buckets[i]->size > 0)
        {
            ll_node_t *aux = old_book->book->buckets[i]->head;
            // For each bucket element
            while (aux != NULL)
            {
                // Get key
                void *key = ((info *)aux->data)->key;
                // Increment before freeing resource
                aux = aux->next;

                if (ht_has_key(old_book->book, key) &&
                    !ht_has_key(volume->book, key))
                {
                    // Remove and free removed entry
                    void *res = ht_remove_entry(old_book->book, key);
                    free(res);
                    // Decrement size
                    old_book->book->size--;
                }
            }
        }
    }

    // Add entries that old book didnt have but the new one has
    for (unsigned int i = 0; i < volume->book->hmax; i++)
    {
        // For each bucket
        if (volume->book->buckets[i]->size > 0)
        {
            // For each element in bucket
            ll_node_t *aux = volume->book->buckets[i]->head;
            while (aux)
            {
                // Get key, value from old and put in the old book
                void *value = ((info *)aux->data)->value;
                void *key = ((info *)aux->data)->key;

                ht_put(old_book->book, key, strlen(key) + 1, value,
                       strlen(value) + 1);
                old_book->book->size++;

                aux = aux->next;
            }
        }
    }

    // Free the new book we took definitions from to update the old book
    ht_free(volume->book);
    free(volume->details);
    free(volume);
    // Free other allocations
    free(buff);
    free(book_name);
    free(word);
    free(def);
}

void parse_get_book_input(char *token, hashtable_t *library)
{
    // Separators for full book name
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    // Get input
    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memcpy(book_name, token, strlen(token) + 1);

    // If entry exists we get entry and print it
    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        // Rat is rating at this step
        double rat;
        if (!volume->details->nr_of_borrows)
            rat = 0;
        else
            rat = volume->details->rating / volume->details->nr_of_borrows;
        printf("Name:%s Rating:%.3f Purchases:%d\n", book_name, rat,
               volume->details->nr_of_borrows);
    }
    else
        // Not in library case
        printf(NOT_IN_LIBRARY);

    free(book_name);
}

void parse_rmv_book_input(char *token, hashtable_t *library)
{
    // Separators for full book name delimitation
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    // Get input
    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memcpy(book_name, token, strlen(token) + 1);

    // If entry exists
    if (ht_has_key(library, book_name))
    {
        // Remove entry and free it
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
    // Separators for full book name input
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    // Get input
    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memcpy(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memcpy(word, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *def = malloc(MAX_WORD);
    memcpy(def, token, strlen(token) + 1);

    // If entry exists get book and add def to it
    if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        // If book doesnt have def already put it else just update it
        if (!ht_has_key(volume->book, word))
        {
            ht_put(volume->book, word, strlen(word) + 1, def,
                   strlen(def) + 1);
            volume->book->size++;
        }
        else
        {
            ht_put(volume->book, word, strlen(word) + 1, def,
                   strlen(def) + 1);
        }
    }
    else
        printf(NOT_IN_LIBRARY);

    // Free allocations
    free(book_name);
    free(word);
    free(def);
}

void parse_get_def_input(char *token, hashtable_t *library)
{
    // Separators for getting full book name
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    // Get actual input
    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memcpy(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memcpy(word, token, strlen(token) + 1);

    // If book has word print it else print error
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

    // Free resources
    free(book_name);
    free(word);
}

void parse_rmv_def_input(char *token, hashtable_t *library)
{
    // Separators for
    char separators[2];
    separators[0] = '\"';
    separators[1] = '\n';

    // Get input
    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    memcpy(book_name, token, strlen(token) + 1);

    token = strtok(NULL, " \n");
    char *word = malloc(MAX_WORD);
    memcpy(word, token, strlen(token) + 1);

    if (ht_has_key(library, book_name))
    {
        // Get book by book_name
        tome *volume = *(tome **)ht_get(library, book_name);
        // If book has word
        if (ht_has_key(volume->book, word))
        {
            // Remove it and free
            void *res = ht_remove_entry(volume->book, word);
            free(res);
            volume->book->size--;
        }
        else
            printf(NOT_IN_BOOK);
    }
    else
        printf(NOT_IN_LIBRARY);

    // Free resources
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
        ht_put(system, user_name, strlen(user_name) + 1, &user,
               sizeof(person));
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
    // Separators for full book name
    char separators[2];
    separators[0] = '\n';
    separators[1] = '\"';

    // Get input
    token = strtok(NULL, " ");
    char *user_name = malloc(MAX_USER_NAME);
    strcpy(user_name, token);

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);

    token = strtok(NULL, " \n");
    int days_available = atoi(token);

    // If user is not in system
    if (!ht_has_key(system, user_name))
    {
        printf(NOT_REGISTERED);
        free(user_name);
        free(book_name);
        return;
    }
    // else get a pointer to the user
    person *user = *(person **)ht_get(system, user_name);

    // If user has negative score it has already been banned
    if (user->score < 0)
    {
        printf(BANNED);
        free(user_name);
        free(book_name);
        return;
    }

    // If he borrowed other book already
    if (user->borrowed_already)
    {
        printf(ALREADY_BORROWED);
    }
    else if (ht_has_key(library, book_name))
    {
        tome *volume = *(tome **)ht_get(library, book_name);
        // If book is already borrowed by someone else
        if (volume->details->borrowed)
        {
            printf(BORROWED);
        }
        else
        {
            // Initialise stats for borrowed state
            volume->details->borrowed = 1;
            volume->details->days_available = days_available;
            user->borrowed_already = 1;
            // Remember book name in user log
            strcpy(user->book_borrowed, book_name);
        }
    }
    else
    {
        printf(NOT_IN_LIBRARY);
    }

    // Free resources
    free(user_name);
    free(book_name);
}

void parse_return_input(char *token, hashtable_t *library, hashtable_t *system)
{
    // Separators to get full book name
    char separators[2];
    separators[0] = '\n';
    separators[1] = '\"';

    // Get input
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

    // Get pointer to user and book
    person *user = *(person **)ht_get(system, user_name);
    tome *volume = *(tome **)ht_get(library, book_name);

    // If user has negative score it has already been banned
    if (user->score < 0)
    {
        printf(BANNED);
        free(user_name);
        free(book_name);
        return;
    }

    // If he tries to return a book that doesnt match what he borrowed
    if (strncmp(user->book_borrowed, book_name, strlen(book_name)))
    {
        printf(OTHER_BOOK);
        free(user_name);
        free(book_name);
        return;
    }

    // If he tries to return a book thats not in library logs
    if (!ht_has_key(library, book_name))
    {
        printf(OTHER_BOOK);
        free(user_name);
        free(book_name);
        return;
    }

    // If he is late we downgrade score else upgrade his score
    if (volume->details->days_available < days_since_borrowed)
        user->score -= 2 * (days_since_borrowed -
                            volume->details->days_available);
    else if (volume->details->days_available > days_since_borrowed)
        user->score += volume->details->days_available - days_since_borrowed;

    // If he lowered score below zero he gets banned
    if (user->score < 0)
        printf(BAN_MESSAGE, user_name);

    // Return action and reset stats for book and user
    volume->details->nr_of_borrows++;
    user->borrowed_already = 0;
    volume->details->borrowed = 0;
    volume->details->days_available = 0;
    volume->details->rating += rating;

    // Free resources allocated
    free(user_name);
    free(book_name);
}

void parse_lost_input(char *token, hashtable_t *library, hashtable_t *system)
{
    // Separators to get full input
    char separators[2];
    separators[0] = '\n';
    separators[1] = '\"';

    // Get input
    token = strtok(NULL, " \n");
    char *user_name = malloc(MAX_USER_NAME);
    strcpy(user_name, token);

    token = strtok(NULL, separators);
    char *book_name = malloc(MAX_BOOK_NAME);
    strcpy(book_name, token);

    // Get pointer to person if it exists in system
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

    // Get pointer to book
    tome *volume = *(tome **)ht_get(library, book_name);

    // If user score is below 0 it has already been banned
    if (user->score < 0)
    {
        printf(BANNED);
        free(user_name);
        free(book_name);
        return;
    }
    else
    {
        // -50 because he lost a book
        user->score -= 50;
        // Ban if score gets below 0
        if (user->score < 0)
            printf(BAN_MESSAGE, user_name);
    }

    // Reset stats for user and book to unborrowed
    user->borrowed_already = 0;
    volume->details->borrowed = 0;
    volume->details->days_available = 0;
    volume->details->rating = 0;
    volume->details->nr_of_borrows = 0;

    // If book was in system remove it
    if (ht_has_key(library, book_name))
    {
        // Remove book and free it
        void *res = ht_remove_entry(library, book_name);
        tome *volume = *(tome **)res;

        free_volume(volume);
        free(res);

        library->size--;
    }
    else
        printf(NOT_IN_LIBRARY);

    // Free resources
    free(user_name);
    free(book_name);
}
