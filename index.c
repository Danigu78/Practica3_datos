#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

#define CAPACITY_INDEX 100



Index *index_create()
{
    Index *indice = malloc(sizeof(Index));
    if (!indice) return NULL;

    indice->array = calloc(CAPACITY_INDEX, sizeof(indexbook));
    if (!indice->array) {
        free(indice);
        return NULL;
    }

    indice->size = 0;
    indice->capacity = CAPACITY_INDEX;
    return indice;
}

void index_free(Index *idx)
{
    if (!idx) return;
    free(idx->array);
    free(idx);
}

int index_binary_search(Index *idx, int key, int *pos)
{
    int left = 0, right = (int)idx->size - 1, mid;

    while (left <= right)
    {
        mid = (left + right) / 2;
        if (idx->array[mid].key == key) 
        return mid;/*Va a devolver un numero positivo o 0*/
        else if (key < idx->array[mid].key) right = mid - 1;
        else left = mid + 1;
    }

    if (pos)
    *pos = left;/*Ese será la posicion en la que posteriormente insertaremos ese elemento si de momento no esta en el indice*/
    return -1;
}

void index_insert(Index *idx, indexbook entry)
{
    int posicion=0;
    int encontrado;
    indexbook *temp=NULL;
    if (!idx) return;

    encontrado = index_binary_search(idx, entry.key, &posicion);
    if (encontrado >= 0) {
        fprintf(stderr, "Error: BookID=%d ya existe\n", entry.key);/*Si ya existe ese book_id en el indice, ya esta incluido luego salimos de la función*/
        return;
    }

    /*Aumentamos la capacidad del indice*/
    if (idx->capacity == idx->size) {
        idx->capacity *= 2;
        temp = realloc(idx->array, idx->capacity * sizeof(indexbook));
        if (!temp) 
        return;
        idx->array = temp;
    }

    if (posicion < (int)idx->size) { 
        memmove(&idx->array[posicion + 1], &idx->array[posicion],
                (idx->size - posicion) * sizeof(indexbook)); /*movemos todo para insertar nuestra clave */
    }

    idx->array[posicion] = entry;/*Metemos nuestra clave en el hueco que toca */
    idx->size++;
}

void index_remove(Index *idx, int pos)
{
    if (!idx || pos < 0 || pos >= (int)idx->size) return;

    if (pos < (int)idx->size - 1) {
        memmove(&idx->array[pos], &idx->array[pos + 1], /* mueve todo a la posiciob del elemento que queremos sacar de la lista de indice*/
                (idx->size - pos - 1) * sizeof(indexbook));
    }

    idx->size--;
}

int index_save(Index *ind, const char *filename)
{
    FILE *f = NULL;
    if (ind == NULL || filename == NULL)
     return 0;

    f = fopen(filename, "wb"); /*wsb se usa para escribir en binario*/
    if (f == NULL) 
    return 0;
/*Escribimos el fichero binario*/
    for (int i = 0; i < (int)ind->size; i++) {
        fwrite(&(ind->array[i].key), sizeof(int), 1, f); /*Escribimos el book_id(la clave)*/
        fwrite(&(ind->array[i].offset), sizeof(long int ), 1, f); /*Escribimos el offset*/
        fwrite(&(ind->array[i].size), sizeof(size_t), 1, f);/*Escribimos el size*/
    }
    
    fclose(f);
    return 1;
}

Index *index_load(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    long filesize;
    size_t record_size;
    int count;
    Index *idx=NULL;
    if (!fp) 
    return NULL; 

    fseek(fp, 0, SEEK_END); /*Coloca el puntero a final del archivo */
    filesize = ftell(fp);/* Guarda el tamaño total del archivo en bytes*/
    rewind(fp);

    // Tamaño de registro: 4 (int) + 8 (long long) + 8 (long long) = 20 bytes
    record_size = sizeof(int) + sizeof( long int) + sizeof(size_t);
     count = filesize / record_size;

    idx = malloc(sizeof(Index));
    if (!idx) 
    { fclose(fp); 
        return NULL; }

    idx->capacity = (count > CAPACITY_INDEX) ? count : CAPACITY_INDEX;
    idx->array = calloc(idx->capacity, sizeof(indexbook));
    if (!idx->array) {
        free(idx);
        fclose(fp);
        return NULL;
    }

    idx->size = count;
    
    for (int i = 0; i < count; i++) {
        fread(&(idx->array[i].key), sizeof(int), 1, fp);/*Lee y lo mete en el array del indice*/
        fread(&(idx->array[i].offset), sizeof(unsigned long long), 1, fp);
        fread(&(idx->array[i].size), sizeof(unsigned long long), 1, fp);
    }

    fclose(fp);
    return idx;
}