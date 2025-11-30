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


int index_save(Index *ind,const char *filename){
   FILE *f=NULL;
   int num;
    if (ind==NULL|| filename==NULL)
    {
        return 0;
    }
    f=fopen(filename,"wb");
    if (f==NULL)
    {
        fprintf(stderr, "Error al abrir %s para escribir el índice\n", filename);
        return 0;
    }
    /*Escribimos cuantos libros hay en el indice*/
    num=ind->size;
    fwrite(&num,sizeof(int),1,f);
    /*Vamos escribiendo todo el array uno por uno introduciendo todo el array*/
    if (num>0)
    {
        fwrite(ind->array,sizeof(indexbook),num,f);
    }
    fclose(f);
    return 1;
}
Index *index_load(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL; 

    Index *idx = malloc(sizeof(Index));
    if (!idx) { fclose(fp); return NULL; }

    int count = 0;
    if (fread(&count, sizeof(int), 1, fp) != 1)
    {
        free(idx);
        fclose(fp);
        return NULL;
    }

    idx->capacity = (count > CAPACITY_INDEX) ? count : CAPACITY_INDEX;
    idx->array = calloc(idx->capacity, sizeof(indexbook));
    if (!idx->array)
    {
        free(idx);
        fclose(fp);
        return NULL;
    }

    idx->size = count;
    if (count > 0)
    {
        fread(idx->array, sizeof(indexbook), count, fp);
    }

    fclose(fp);
    return idx;
}