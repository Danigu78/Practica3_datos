#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#define CAPACITY_INDEX 100

Index *index_create()
{
    Index *indice = NULL;
    indice = malloc(sizeof(Index));
    if (indice == NULL)
    {
        return NULL;
    }
    indice->array = (Index *)calloc(CAPACITY_INDEX, sizeof(Index));
    if (indice->array == NULL)
    {
        free(indice);
        return NULL;
    }
    indice->size = 0;
    indice->capacity = CAPACITY_INDEX;
    return indice;
}
void index_free(Index *idx)
{
    if (idx == NULL)
    {
        return;
    }
    free(idx->array);
    free(idx);
}

int index_binary_search(Index *idx, int key, int *pos)
{
    int left = 0;
    int right = idx->size - 1;
    int mid;

    while (left <= right)
    {
        mid = (right + left) / 2;
        if (idx->array[mid].key == key)
        {
            return mid;
        }
        else if (key < idx->array[mid].key)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }
    /*Clave no encontrada- Left indica la posicion de insercion*/
    if (pos)
        *pos = left; /*Posicion donde deberia insetarse*/
    return -1;
}

// Inserta entry manteniendo el índice ordenado por key.
// No permite duplicados.
void index_insert(Index *idx, indexbook entry)
{
    int posicion = 0;
    int encontrado;

    indexbook *temp = NULL;
    if (idx == NULL)
    {
        return;
    }
    encontrado = index_binary_search(idx, entry.key, &posicion);
    if (encontrado >= 0)
    {
        fprintf(stderr, "El registro con el BookaID=%d ya existe en la libreria, por lo que no se puede insertar", entry.key);
        return;
    }

    if (idx->capacity == idx->size)
    {
        idx->capacity *= 2;
        temp = realloc(idx->array, idx->capacity * sizeof(indexbook));
        if (!temp)
        {
            fprintf(stderr, "La nueva reserva de memoria, aumentando la que había ha sido erronea\n");
            return;
        }
        idx->array = temp;
    }

    if (posicion < (idx->size)) /*Si la posicion en la que debemos meter el elemento para que quede ordenado no queda al final*/
    {
        memmove(&idx->array[posicion + 1], &idx->array[posicion], (idx->size - posicion) * sizeof(indexbook)); /*Lo que hacemos es desplazar los elementos para hacer huedco en el sitio donde metemos el elemento*/
    }

    idx->array[posicion] = entry;
    idx->size++;
}
void index_remove(Index *idx, int pos)
{
    if (!idx || pos < 0 || pos > idx->size)
    {
        return;
    }
    if (pos < (int)idx->size - 1)
    {
        memmove(&idx->array[pos], &idx->array[pos + 1],
                (idx->size - pos - 1) * sizeof(indexbook));
    }
    idx->size--;
    return;
}