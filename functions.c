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
    for (unsigned int i = 0; i < library->hmax; i++)
    {
        if (library->buckets[i]->size > 0)
        {
            ll_node_t *aux = library->buckets[i]->head;
            while (aux)
            {
                tome *volume = *(tome **)((info *)aux->data)->value;
                free_volume(volume);

                free(((info *)aux->data)->key);
                free(((info *)aux->data)->value);
                aux = aux->next;
            }
        }
        ll_free(&library->buckets[i]);
    }
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
                free(((info *)aux->data)->key);
                person *user = *(person **)((info *)aux->data)->value;
                free(user->book_borrowed);
                free(user);
                free(((info *)aux->data)->value);
                aux = aux->next;
            }
        }
        ll_free(&system->buckets[i]);
    }
    free(system->buckets);
    free(system);
}

int compare(char *name1, char *name2, double rating1, double rating2, int purchase1, int purchase2)
{
    double eps = 0.001;
    if (rating1 < rating2)
        return 1;
    else if (fabs(rating1 - rating2) < eps && purchase1 < purchase2)
        return 1;
    else if (fabs(rating1 - rating2) < eps && purchase1 == purchase2 && strcmp(name1, name2) < 0)
        return 1;
    return 0;
}

void swap(char **a1, char **b1, double *a2, double *b2, int *a3, int *b3)
{
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
    double eps = 0.001;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
            if (rating[i] < rating[j])
                swap(&names[i], &names[j], &rating[i], &rating[j], &purchases[i], &purchases[j]);
            else if (fabs(rating[i] - rating[j]) < eps && purchases[i] < purchases[j])
                swap(&names[i], &names[j], &rating[i], &rating[j], &purchases[i], &purchases[j]);
            else if (fabs(rating[i] - rating[j]) < eps && purchases[i] == purchases[j] && strcmp(names[j], names[i]) < 0)
                swap(&names[i], &names[j], &rating[i], &rating[j], &purchases[i], &purchases[j]);
}

void top_books(hashtable_t *library)
{
    int cnt = 0;
    char **names = malloc(library->size * sizeof(char *));
    double *rates = malloc(library->size * sizeof(double));
    int *purchases = malloc(library->size * sizeof(int));
    for (unsigned int i = 0; i < library->size; i++)
        names[i] = malloc(MAX_BOOK_NAME);

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
                if (volume->details->nr_of_borrows != 0)
                    rates[cnt] = volume->details->rating / volume->details->nr_of_borrows;
                else
                    rates[cnt] = 0;
                cnt++;
                aux = aux->next;
            }
        }
    }

    sort_by_rating(names, rates, purchases, library->size);
    printf("Books ranking:\n");
    for (unsigned int i = 0; i < library->size; i++)
    {
        printf("%d. Name:%s Rating:%.3f Purchases:%d\n", i + 1, names[i], rates[i], purchases[i]);
        free(names[i]);
    }

    free(names);
    free(rates);
    free(purchases);
}

void sort_by_scores(char **names, int *scores, int size)
{
    double dummy1 = 5;
    double dummy2 = 6;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
        {
            if (scores[i] < scores[j])
                swap(&names[i], &names[j], &dummy1, &dummy2, &scores[i], &scores[j]);
            else if (scores[i] == scores[j] && strcmp(names[i], names[j]) > 0)
                swap(&names[i], &names[j], &dummy1, &dummy2, &scores[i], &scores[j]);
        }
}

void top_users(hashtable_t *system)
{
    int cnt = 0;
    char **names = malloc(system->size * sizeof(char *));
    int *scores = malloc(system->size * sizeof(int));

    for (unsigned int i = 0; i < system->size; i++)
        names[i] = malloc(MAX_USER_NAME);

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

    sort_by_scores(names, scores, system->size);
    printf("Users ranking:\n");
    for (unsigned int i = 0; i < system->size; i++)
    {

        if (scores[i] >= 0)
            printf("%d. Name:%s Points:%d\n", i + 1, names[i], scores[i]);

        free(names[i]);
    }

    free(names);
    free(scores);
}

void free_volume(tome *volume)
{
    for (unsigned int i = 0; i < volume->book->hmax; i++)
    {
        ll_node_t *aux1 = volume->book->buckets[i]->head;

        while (aux1)
        {
            info *data = (info *)aux1->data;
            free(data->key);
            free(data->value);
            aux1 = aux1->next;
        }

        ll_free(&volume->book->buckets[i]);
    }

    free(volume->book->buckets);
    free(volume->book);
    free(volume->details);
    free(volume);
}
