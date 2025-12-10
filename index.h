#ifndef INDEX_H
#define INDEX_H

#include <stdio.h> 
#include <stdlib.h>

typedef struct {
    int key;            /* book isbn*/
    long int offset;  /* book guardado en esta posicion. 8 bytes */
    size_t  size;    
} indexbook;            

typedef struct {
    indexbook *array;
    size_t size;
    size_t capacity;
} Index;


/*Funcion que crea un nuevo indice*/
Index *index_create();
/*Funcion para liberar un indice*/
void index_free(Index *idx);
/*Funcion que busca una clave en un indice, a parte encuentra la posición*/
int index_binary_search(Index *idx, int key, int *pos);
/*Añadir un nuevo book_id al indice*/
void index_insert(Index *idx, indexbook entry);
/*Funcion que saca del indice el libro que ocupa la posicion que se pasa por argumento de la funcion*/
void index_remove(Index *idx, int pos);
/*Funcion que gaurda el fichero binario con los datos del indice*/
int index_save(Index *idx, const char *filename);
/*Funcion que abre un fichero y carga los datos de ese fichero*/
Index *index_load(const char *filename);

#endif