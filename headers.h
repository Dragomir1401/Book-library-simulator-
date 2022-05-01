#include "structs.h"
unsigned int ht_get_size(hashtable_t *ht);

void ht_free(hashtable_t *ht);

void *ht_remove_entry(hashtable_t *ht, void *key);

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

void parse_add_book_input(char *token, hashtable_t *library);

void parse_get_book_input(char *token, hashtable_t *library);

void free_library(hashtable_t *library);

void free_system(hashtable_t *system);

void parse_rmv_book_input(char *token, hashtable_t *library);

void parse_add_def_input(char *token, hashtable_t *library);

void parse_get_def_input(char *token, hashtable_t *library);

void parse_rmv_def_input(char *token, hashtable_t *library);

void parse_add_user_input(char *token, hashtable_t *system);

void parse_borrow_input(char *token, hashtable_t *library, hashtable_t *system);

void parse_return_input(char *token, hashtable_t *library, hashtable_t *system);

void parse_lost_input(char *token, hashtable_t *library, hashtable_t *system);

void top_books(hashtable_t *library);

void top_users(hashtable_t *system);

void ll_free(linked_list_t **pp_list);

void free_volume(tome *volume);

linked_list_t *ll_create(unsigned int data_size);

hashtable_t *ht_create_resize(hashtable_t *ht, int data_size);

void ht_resize(hashtable_t *ht, int data_size);