#ifndef INDEX_H
#define INDEX_H

#include <stdio.h> 
#include <stdlib.h>

typedef struct {
    int key;            // 4 bytes
    int offset;         // 4 bytes
    size_t size;        // 8 bytes
} indexbook;            // TOTAL = 16 bytes

typedef struct {
    indexbook *array;
    size_t size;
    size_t capacity;
} Index;

Index *index_create();
void index_free(Index *idx);
int index_binary_search(Index *idx, int key, int *pos);
void index_insert(Index *idx, indexbook entry);
void index_remove(Index *idx, int pos);
int index_save(Index *idx, const char *filename);
Index *index_load(const char *filename);

#endif