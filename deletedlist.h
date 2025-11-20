#ifndef DELETEDLIST_H
#define DELETEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2


typedef struct 
{
    size_t offset ;/*Posicion en el fichero*/ 
    size_t size; /* Tamaño del registro eliminado*/
}deletedbook;

typedef struct 
{
    deletedbook *hueco;/*AArray dinamico de huecos*/ 
    size_t size; /*Numero de huecos*/
    size_t capacity; /*Capacidad maxima reservada*/
    int strategy;
}DeletedList;


DeletedList *deletedlist_create(int strategy);
void deletedlist_insert(DeletedList *list, deletedbook hb);

/*Buscar hueco según estrategia*/ 
int deletedlist_find(DeletedList *list, size_t needed_size, deletedbook *hb);

/*Eliminar huecos*/
void deletedlist_destroy(DeletedList *list) ;

/* 
 * Guardar la lista de huecos en disco en formato binario.
 * Se guarda la estrategia al principio y luego cada hueco con offset y size.
 */
int deletedlist_save(DeletedList *list, const char *filename);

/* 
 * Cargar la lista de huecos desde un fichero binario (*.lst)
 * El fichero tiene la siguiente estructura:
 * [int strategy][deletedbook 1][deletedbook 2]...[deletedbook n]
 * Devuelve un puntero a la lista en memoria, o NULL si hubo error.
 */
DeletedList *deletedlist_load(const char *filename);

#endif// Quitar hueco