#ifndef INDEX_H
#define INDEX_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct {
    int key;          // BookID
    long offset;      /*Es un número que indica desde el byte 0 del fichero, cuántos bytes hay hasta llegar al registro.*/
    size_t size;      // tamaño del registro

} indexbook;

typedef struct {
    indexbook *array; // array dinámico de elementos
    size_t size;      // nº de elementos en uso
    size_t capacity;  // nº de elementos reservados
} Index;

Index *index_create();
void index_free(Index *idx);

// Búsqueda binaria.
// Devuelve:
//  >=0 → posición donde se encuentra
//  <0  → no encontrado, pero *pos = posición donde debería insertarse
int index_binary_search(Index *idx, int key, int *pos);

// Inserta en posición pos
void index_insert(Index *idx, indexbook entry);

// Elimina el elemento en pos
void index_remove(Index *idx, int pos);

#endif
