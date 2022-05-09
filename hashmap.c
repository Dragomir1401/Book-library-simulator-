// Copyright 2022 Dragomir Andrei
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include <stdint.h>
#define MAX_STRING_SIZE 256
#define HMAX 7
#define MAX_WORD 100

linked_list_t *
ll_create(unsigned int data_size)
{
    // Create list function
    linked_list_t *ll;

    ll = malloc(sizeof(*ll));

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t *new_node;

    if (!list)
    {
        return;
    }

    /* n >= list->size means adding a new node to the final of list*/
    if (n > list->size)
    {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0)
    {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL)
    {
        /* Adica n == 0. */
        list->head = new_node;
    }
    else
    {
        prev->next = new_node;
    }

    list->size++;
}

ll_node_t *
ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head)
    {
        return NULL;
    }

    /* n >= list->size - 1 means eliminating last node */
    if (n > list->size - 1)
    {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0)
    {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL)
    {
        /* Means n == 0. */
        list->head = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

unsigned int
ll_get_size(linked_list_t *list)
{
    if (!list)
    {
        return -1;
    }

    return list->size;
}

void ll_free(linked_list_t **pp_list)
{
    ll_node_t *currNode;

    if (!pp_list || !*pp_list)
    {
        return;
    }

    while (ll_get_size(*pp_list) > 0)
    {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

int compare_function_strings(void *a, void *b)
{
    // Function to compare strings
    char *str_a = (char *)a;
    char *str_b = (char *)b;

    return strcmp(str_a, str_b);
}

unsigned int
hash_function_string(void *a)
{
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char *)a;
    uint64_t hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
          int (*compare_function)(void *, void *))
{
    // Alloc hashtable
    hashtable_t *ht = malloc(sizeof(hashtable_t));
    ht->buckets = malloc(hmax * sizeof(linked_list_t));
    // Alloc buckets
    for (unsigned int i = 0; i < hmax; i++)
        ht->buckets[i] = ll_create(sizeof(info));
    // Inititalise hashtable details
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;
    ht->size = 0;
    ht->hmax = hmax;

    return ht;
}

int ht_has_key(hashtable_t *ht, void *key)
{
    // Verifies if a hashtable has a key
    int index = ht->hash_function(key) % ht->hmax;
    ll_node_t *aux = ht->buckets[index]->head;
    while (aux)
    {
        if (!ht->compare_function(key, ((info *)aux->data)->key))
            return 1;
        aux = aux->next;
    }

    return 0;
}

void *
ht_get(hashtable_t *ht, void *key)
{
    // Verifies if a ht has key and returns value
    int index = ht->hash_function(key) % ht->hmax;
    ll_node_t *aux = ht->buckets[index]->head;
    while (aux)
    {
        if (!ht->compare_function(key, ((info *)aux->data)->key))
            return ((info *)aux->data)->value;

        aux = aux->next;
    }

    return NULL;
}

void ht_rehash(hashtable_t *hashtable, info *elem)
{
    // Function to rehash element and add it to a hashtable
    if (!hashtable)
        return;

    unsigned int hash = hashtable->hash_function(elem->key) % hashtable->hmax;
    linked_list_t *list = hashtable->buckets[hash];

    ll_add_nth_node(list, 0, elem);
}

linked_list_t *ht_create_list_from_ht(hashtable_t *hashtable)
{
    // Create a list
    linked_list_t *list = ll_create(hashtable->buckets[0]->data_size);
    unsigned int cnt = 0;

    // For each bucket
    while (cnt < hashtable->hmax)
    {
        if (hashtable->buckets[cnt])
        {
            // If list is empty we put pointer head to old bucket
            if (!list->head)
            {
                list->head = hashtable->buckets[cnt]->head;
                list->size += hashtable->buckets[cnt]->size;
                free(hashtable->buckets[cnt]);
            }
            else
            {
                // We put pointer to last element in list the head of the bucket
                ll_node_t *last_curr = list->head;
                while (last_curr->next)
                    last_curr = last_curr->next;

                last_curr->next = hashtable->buckets[cnt]->head;
                list->size += hashtable->buckets[cnt]->size;
                // Free the original bucket
                free(hashtable->buckets[cnt]);
            }
        }
        cnt++;
    }
    // Return the created list
    return list;
}

void ht_resize(hashtable_t *hashtable)
{
    // Function to resize a hashtable
    // Create a list with entries from old hashtable
    linked_list_t *entries = ht_create_list_from_ht(hashtable);

    // Realloc hashtable with double its size
    hashtable->hmax = hashtable->hmax * 2;
    hashtable->buckets = realloc(hashtable->buckets,
                                 hashtable->hmax * sizeof(linked_list_t *));

    // Recreate buckets
    for (uint32_t i = 0; i < hashtable->hmax; i++)
        hashtable->buckets[i] = ll_create(sizeof(info));

    // Rehash every entry which we put int the list
    ll_node_t *curr = entries->head;
    while (curr)
    {
        ht_rehash(hashtable, curr->data);
        curr = curr->next;
    }

    // Free entries list
    ll_free(&entries);
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
            void *value, unsigned int value_size)
{
    // If the load factor exceeds 1 we resize
    double load_factor = ht->size / ht->hmax;
    if (load_factor > 1)
        ht_resize(ht);

    // Get hash
    int index = ht->hash_function(key) % ht->hmax;
    // Make a copy of the key
    void *copy_key = malloc(key_size);
    memcpy(copy_key, key, key_size);

    ll_node_t *aux = ht->buckets[index]->head;
    while (aux)
    {
        // If we have the same entry before we update it
        if (!ht->compare_function(key, ((info *)aux->data)->key) &&
            ht->compare_function(value, ((info *)aux->data)->value))
        {
            // Free initial value and put over the new one
            free(((info *)aux->data)->value);
            ((info *)aux->data)->value = malloc(value_size);
            memcpy(((info *)aux->data)->value, value, value_size);
            free(copy_key);
            return;
        }
        aux = aux->next;
    }
    // Create a new entry
    info *data = malloc(sizeof(info));
    data->key = malloc(key_size);
    data->value = malloc(value_size);

    // Put memory in the new entry
    memcpy(data->key, copy_key, key_size);
    memcpy(data->value, value, value_size);
    // Add node to the bucket in charge
    ll_add_nth_node(ht->buckets[index], ht->buckets[index]->size, data);

    // Free the copy
    free(copy_key);
    free(data);
}

void *ht_remove_entry(hashtable_t *ht, void *key)
{
    // Get hash
    int index = ht->hash_function(key) % ht->hmax;
    ll_node_t *aux = ht->buckets[index]->head;
    int n = 0;
    void *res;
    while (aux)
    {
        if (!ht->compare_function(key, ((info *)aux->data)->key))
        {
            // Remove the node from the bucket
            ll_node_t *removed = ll_remove_nth_node(ht->buckets[index], n);

            // Free the removed values
            free(((info *)removed->data)->key);
            res = ((info *)removed->data)->value;
            free(removed->data);
            free(removed);
            break;
        }
        aux = aux->next;

        n++;
    }
    return res;
}

void ht_free(hashtable_t *ht)
{
    for (unsigned int i = 0; i < ht->hmax; i++)
    {
        // Work for each bucket
        ll_node_t *aux = ht->buckets[i]->head;

        while (aux)
        {
            // Free every node and key and data
            info *data = (info *)aux->data;
            free(data->key);
            free(data->value);
            aux = aux->next;
        }
        // Free the bucket
        ll_free(&ht->buckets[i]);
    }

    // Free all buckets and hashtable
    free(ht->buckets);
    free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
    // Function to get size of ht
    if (ht == NULL)
        return 0;

    return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
    // Function to get hmax of hashtable
    if (ht == NULL)
        return 0;

    return ht->hmax;
}
