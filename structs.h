// Copyright 2022 Dragomir Andrei
#ifndef STRUCTS_H_
#define STRUCTS_H_

#pragma once

// Structs file
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
    linked_list_t **buckets;
    unsigned int size;
    unsigned int hmax;
    unsigned int (*hash_function)(void *);
    int (*compare_function)(void *, void *);
};

// Details struct for book
typedef struct details details;
struct details
{
    double rating;
    int borrowed;
    int nr_of_borrows;
    int days_available;
};

// Book structure
typedef struct tome tome;
struct tome
{
    hashtable_t *book;
    details *details;
};

// Person structure
typedef struct person person;
struct person
{
    int borrowed_already;
    int score;
    char *book_borrowed;
};

#endif  // STRUCTS_H_
