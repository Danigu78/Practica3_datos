#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

#define CAPACITY_INDEX 100

/* -----------------------------------------------------------
 * Crea un índice vacío
 * ----------------------------------------------------------- */
Index *index_create()
{
    Index *indice = malloc(sizeof(Index));
    if (!indice) return NULL;

    indice->array = calloc(CAPACITY_INDEX, sizeof(indexbook));
    if (!indice->array)
    {
        free(indice);
        return NULL;
    }

    indice->size = 0;
    indice->capacity = CAPACITY_INDEX;
    return indice;
}

/* -----------------------------------------------------------
 * Libera memoria del índice
 * ----------------------------------------------------------- */
void index_free(Index *idx)
{
    if (!idx) return;
    free(idx->array);
    free(idx);
}

/* -----------------------------------------------------------
 * Búsqueda binaria en el índice
 * Devuelve la posición si se encuentra, -1 si no
 * Si no se encuentra, *pos contiene posición de inserción
 * ----------------------------------------------------------- */
int index_binary_search(Index *idx, int key, int *pos)
{
    int left = 0, right = (int)idx->size - 1, mid;

    while (left <= right)
    {
        mid = (left + right) / 2;
        if (idx->array[mid].key == key) return mid;
        else if (key < idx->array[mid].key) right = mid - 1;
        else left = mid + 1;
    }

    if (pos) *pos = left;
    return -1;
}

/* -----------------------------------------------------------
 * Inserta entry manteniendo el índice ordenado por key
 * No permite duplicados
 * ----------------------------------------------------------- */
void index_insert(Index *idx, indexbook entry)
{
    if (!idx) return;

    int posicion = 0;
    int encontrado = index_binary_search(idx, entry.key, &posicion);
    if (encontrado >= 0)
    {
        fprintf(stderr, "El registro con el BookID=%d ya existe en la libreria, por lo que no se puede insertar\n", entry.key);
        return;
    }

    if (idx->capacity == idx->size)
    {
        idx->capacity *= 2;
        indexbook *temp = realloc(idx->array, idx->capacity * sizeof(indexbook));
        if (!temp)
        {
            fprintf(stderr, "Error al aumentar la memoria del índice\n");
            return;
        }
        idx->array = temp;
    }

    if (posicion < (int)idx->size)
    {
        memmove(&idx->array[posicion + 1], &idx->array[posicion],
                (idx->size - posicion) * sizeof(indexbook));
    }

    idx->array[posicion] = entry;
    idx->size++;
}

/* -----------------------------------------------------------
 * Elimina un registro del índice en la posición pos
 * ----------------------------------------------------------- */
void index_remove(Index *idx, int pos)
{
    if (!idx || pos < 0 || pos >= (int)idx->size) return;

    if (pos < (int)idx->size - 1)
    {
        memmove(&idx->array[pos], &idx->array[pos + 1],
                (idx->size - pos - 1) * sizeof(indexbook));
    }

    idx->size--;
}
