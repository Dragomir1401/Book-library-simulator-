// Copyright 2022 Dragomir Andrei
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_BOOK_NAME 100
#define MAX_USER_NAME 100
#define BUFFER_MAX 300
#include "headers.h"
#include "structs.h"
#include "errors.h"
#define MAX_WORD 100

void free_library(hashtable_t *library)
{
    // Function to free the library
    for (unsigned int i = 0; i < library->hmax; i++)
    {
        if (library->buckets[i]->size > 0)
        {
            ll_node_t *aux = library->buckets[i]->head;
            while (aux)
            {
                // Get each struct of book and free it
                tome *volume = *(tome **)((info *)aux->data)->value;
                free_volume(volume);

                // Free key and value
                free(((info *)aux->data)->key);
                free(((info *)aux->data)->value);
                aux = aux->next;
            }
        }
        // Free each bucket
        ll_free(&library->buckets[i]);
    }
    // Free all buckets and hashtable
    free(library->buckets);
    free(library);
}

void free_system(hashtable_t *system)
{
    for (unsigned int i = 0; i < system->hmax; i++)
    {
        if (system->buckets[i]->size > 0)
        {
            ll_node_t *aux = system->buckets[i]->head;

            while (aux)
            {
                // Free aach person in the hashtable
                free(((info *)aux->data)->key);
                person *user = *(person **)((info *)aux->data)->value;
                free(user->book_borrowed);
                free(user);
                // Free key and value
                free(((info *)aux->data)->value);
                aux = aux->next;
            }
        }
        // Free each system bucket
        ll_free(&system->buckets[i]);
    }
    // Free all buckets and hashmap
    free(system->buckets);
    free(system);
}

void swap(char **a1, char **b1, double *a2, double *b2, int *a3, int *b3)
{
    // Function to swap values for output
    char *aux = malloc(MAX_BOOK_NAME);
    strcpy(aux, *a1);
    strcpy(*a1, *b1);
    strcpy(*b1, aux);
    free(aux);

    double tmp = *a2;
    *a2 = *b2;
    *b2 = tmp;

    int sec = *a3;
    *a3 = *b3;
    *b3 = sec;
}

void sort_by_rating(char **names, double *rating, int *purchases, int size)
{
    // Function to sort arrays for output
    double eps = 0.001;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
            if (rating[i] < rating[j])
                swap(&names[i], &names[j], &rating[i], &rating[j],
                     &purchases[i], &purchases[j]);
            else if (fabs(rating[i] - rating[j]) < eps &&
                     purchases[i] < purchases[j])
                swap(&names[i], &names[j], &rating[i], &rating[j],
                     &purchases[i], &purchases[j]);
            else if (fabs(rating[i] - rating[j]) < eps &&
                     purchases[i] == purchases[j] &&
                     strcmp(names[j], names[i]) < 0)
                swap(&names[i], &names[j], &rating[i], &rating[j],
                     &purchases[i], &purchases[j]);
}

void top_books(hashtable_t *library)
{
    // Function to get books by rating
    int cnt = 0;
    // Alloc arrays to store info about books
    char **names = malloc(library->size * sizeof(char *));
    double *rates = malloc(library->size * sizeof(double));
    int *purchases = malloc(library->size * sizeof(int));
    for (unsigned int i = 0; i < library->size; i++)
        names[i] = malloc(MAX_BOOK_NAME);

    // Get all entries in hastable and put them in arrays
    for (unsigned int i = 0; i < library->hmax; i++)
    {
        if (library->buckets[i]->size > 0)
        {
            ll_node_t *aux = library->buckets[i]->head;
            while (aux)
            {
                tome *volume = *(tome **)((info *)aux->data)->value;

                strcpy(names[cnt], (char *)((info *)aux->data)->key);
                purchases[cnt] = volume->details->nr_of_borrows;
                // If it hasnt been borrowed then rating is zero
                if (volume->details->nr_of_borrows != 0)
                    rates[cnt] = volume->details->rating /
                                 volume->details->nr_of_borrows;
                else
                    rates[cnt] = 0;

                cnt++;
                aux = aux->next;
            }
        }
    }

    // Sort arrays
    sort_by_rating(names, rates, purchases, library->size);

    // Print using the format
    printf("Books ranking:\n");
    for (unsigned int i = 0; i < library->size; i++)
    {
        printf("%d. Name:%s Rating:%.3f Purchases:%d\n", i + 1,
               names[i], rates[i], purchases[i]);
        free(names[i]);
    }

    // Free resources
    free(names);
    free(rates);
    free(purchases);
}

void sort_by_scores(char **names, int *scores, int size)
{
    // Function to sort by score each user
    double dummy1 = 5;
    double dummy2 = 6;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
        {
            if (scores[i] < scores[j])
                swap(&names[i], &names[j], &dummy1, &dummy2,
                     &scores[i], &scores[j]);
            else if (scores[i] == scores[j] &&
                     strcmp(names[i], names[j]) > 0)
                swap(&names[i], &names[j], &dummy1, &dummy2,
                     &scores[i], &scores[j]);
        }
}

void top_users(hashtable_t *system)
{
    int cnt = 0;
    // Create arrays to store info about users
    char **names = malloc(system->size * sizeof(char *));
    int *scores = malloc(system->size * sizeof(int));

    for (unsigned int i = 0; i < system->size; i++)
        names[i] = malloc(MAX_USER_NAME);

    // Put in arrays each user and its score
    for (unsigned int i = 0; i < system->hmax; i++)
    {
        if (system->buckets[i]->size > 0)
        {
            ll_node_t *aux = system->buckets[i]->head;
            while (aux)
            {
                person *user = *(person **)((info *)aux->data)->value;
                strcpy(names[cnt], (char *)((info *)aux->data)->key);
                scores[cnt] = user->score;

                cnt++;
                aux = aux->next;
            }
        }
    }

    // Sort arrays by score
    sort_by_scores(names, scores, system->size);
    printf("Users ranking:\n");
    for (unsigned int i = 0; i < system->size; i++)
    {
        // We only print users that are not banned
        if (scores[i] >= 0)
            printf("%d. Name:%s Points:%d\n", i + 1, names[i], scores[i]);

        free(names[i]);
    }

    // Free arrays
    free(names);
    free(scores);
}

void free_volume(tome *volume)
{
    // Function to free a book
    for (unsigned int i = 0; i < volume->book->hmax; i++)
    {
        ll_node_t *aux1 = volume->book->buckets[i]->head;
        while (aux1)
        {
            info *data = (info *)aux1->data;
            // Free each entry
            free(data->key);
            free(data->value);
            aux1 = aux1->next;
        }

        // Free buckets
        ll_free(&volume->book->buckets[i]);
    }

    // Free volume and its parts
    free(volume->book->buckets);
    free(volume->book);
    free(volume->details);
    free(volume);
}
