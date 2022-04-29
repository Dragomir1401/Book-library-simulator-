#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BOOK_NAME 100
#define BUFFER_MAX 300
#include "headers.h"
#include "structs.h"
#include "errors.h"
#define MAX_WORD 100

void free_resources(hashtable_t *library)
{
    ht_free(library);
}