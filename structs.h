#pragma once

typedef struct info info;
struct info
{
    void *key;
    void *value;
};

typedef struct ll_node_t
{
    void *data;
    struct ll_node_t *next;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t *head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

typedef struct hashtable_t hashtable_t;
struct hashtable_t
{
    linked_list_t **buckets; /* Array de liste simplu-inlantuite. */
    /* Nr. total de noduri existente curent in toate bucket-urile. */
    unsigned int size;
    unsigned int hmax; /* Nr. de bucket-uri. */
    /* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor. */
    unsigned int (*hash_function)(void *);
    /* (Pointer la) Functie pentru a compara doua chei. */
    int (*compare_function)(void *, void *);
};

typedef struct details details;
struct details
{
    int rating;
    int pieces_sold;
};

typedef struct tome tome;
struct tome
{
    hashtable_t *book;
    details *details;
};
