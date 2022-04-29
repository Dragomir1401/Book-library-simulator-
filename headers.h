#include "structs.h"
unsigned int ht_get_size(hashtable_t *ht);

void ht_free(hashtable_t *ht);

void ht_remove_entry(hashtable_t *ht, void *key);

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
            void *value, unsigned int value_size);

void *ht_get(hashtable_t *ht, void *key);

int ht_has_key(hashtable_t *ht, void *key);

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
          int (*compare_function)(void *, void *));

unsigned int
hash_function_string(void *a);

int compare_function_strings(void *a, void *b);

void parse_add_book_input(char *token, hashtable_t **library);

void parse_get_book_input(char *token, hashtable_t *library);

void free_resources(hashtable_t *library);
