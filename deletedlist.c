#include <stdio.h>
#include <stdlib.h>
#include "deletedlist.h"

/* -----------------------------------------------------------
 * Crea una lista de huecos vacía con estrategia dada
 * ----------------------------------------------------------- */
DeletedList *deletedlist_create(int strategy)
{
    if ((strategy != FIRSTFIT) && (strategy != BESTFIT) && (strategy != WORSTFIT))
    {
        fprintf(stderr, "La estrategia elegida no es válida\n");
        return NULL;
    }

    DeletedList *array = malloc(sizeof(DeletedList));
    if (!array) return NULL;

    array->capacity = 5;
    array->size = 0;
    array->strategy = strategy;
    array->hueco = malloc(array->capacity * sizeof(deletedbook));
    if (!array->hueco)
    {
        free(array);
        return NULL;
    }

    return array;
}

/* -----------------------------------------------------------
 * Ordena la lista de huecos según la estrategia
 * ----------------------------------------------------------- */
void deletedlist_sort(DeletedList *list)
{
    if (!list) return;
    if (list->strategy == FIRSTFIT) return;

    deletedbook temp;
    for (size_t i = 0; i < list->size; i++)
        for (size_t j = i + 1; j < list->size; j++)
        {
            if ((list->strategy == BESTFIT && list->hueco[i].size > list->hueco[j].size) ||
                (list->strategy == WORSTFIT && list->hueco[i].size < list->hueco[j].size))
            {
                temp = list->hueco[i];
                list->hueco[i] = list->hueco[j];
                list->hueco[j] = temp;
            }
        }
}

/* -----------------------------------------------------------
 * Inserta un hueco en la lista y reordena según estrategia
 * ----------------------------------------------------------- */
void deletedlist_insert(DeletedList *list, deletedbook hb)
{
    if (!list) return;

    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        deletedbook *aux = realloc(list->hueco, list->capacity * sizeof(deletedbook));
        if (!aux) { fprintf(stderr, "Error al realloc en deletedlist_insert\n"); return; }
        list->hueco = aux;
    }

    list->hueco[list->size++] = hb;
    deletedlist_sort(list);
}

/* -----------------------------------------------------------
 * Busca un hueco donde quepa needed_size según estrategia
 * Devuelve 1 si se encuentra, 0 si no
 * ----------------------------------------------------------- */
int deletedlist_find(DeletedList *list, size_t needed_size, deletedbook *hb)
{
    if (!list || !hb) return 0;

    deletedlist_sort(list);

    int found_index = -1;
    for (int i = 0; i < (int)list->size; i++)
    {
        if (list->hueco[i].size >= needed_size)
        {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) return 0;

    *hb = list->hueco[found_index];

    if (list->hueco[found_index].size == needed_size)
    {
        for (size_t i = found_index; i < list->size - 1; i++)
            list->hueco[i] = list->hueco[i + 1];
        list->size--;
    }
    else /*Si el hueco es mas grande que lo que necesitamos, lo tenemos que meter en ese hueco y además hacer más pequeño el hueco*/
    {
        list->hueco[found_index].offset += needed_size;/*Calculamos ek nuevo offset del hueco, es decir habrá que sumarle su offset actual antes de insertar mas el tamaño del registro que le vamos a insertar para actualizar el offset*/
        list->hueco[found_index].size -= needed_size;/*Y restamos el tamaño*/
        /*Reordenamos la lista ya que como hemos reducido el tamaño se puede romper el orden en el que estaba la lista de huecos*/
        deletedlist_sort(list);
        hb->size = needed_size;

    }

    return 1;
}

/* -----------------------------------------------------------
 * Libera la lista de huecos
 * ----------------------------------------------------------- */
void deletedlist_destroy(DeletedList *list)
{
    if (!list) return;
    if (list->hueco) free(list->hueco);
    free(list);
}

/* -----------------------------------------------------------
 * Guarda la lista de huecos en un fichero binario
 * ----------------------------------------------------------- */
int deletedlist_save(DeletedList *list, const char *filename)
{
    if (!list || !filename) return 0;

    FILE *fp = fopen(filename, "wb");
    if (!fp) { fprintf(stderr, "Error al abrir %s\n", filename); return 0; }


    /*Primero en binario empezamos a escribir la estrategia*/
    if (fwrite(&list->strategy, sizeof(int), 1, fp) != 1) { fclose(fp); return 0; }
/*LUEGO UYA */
    for (int i = 0; i < (int)list->size; i++)
        if (fwrite(&list->hueco[i], sizeof(deletedbook), 1, fp) != 1)
        {
            fclose(fp);
            return 0;
        }

    fclose(fp);
    return 1;
}

/* -----------------------------------------------------------
 * Carga la lista de huecos desde un fichero binario
 * ----------------------------------------------------------- */
DeletedList *deletedlist_load(const char *filename)
{
    if (!filename) return NULL;

    FILE *fp = fopen(filename, "rb");
   if (fp == NULL)
{
   
    return NULL;
}


    DeletedList *list = malloc(sizeof(DeletedList));
    if (!list) { fclose(fp); return NULL; }

    if (fread(&list->strategy, sizeof(int), 1, fp) != 1)
    {
        fprintf(stderr, "Error al leer la estrategia del fichero\n");
        free(list);
        fclose(fp);
        return NULL;
    }

    list->size = 0;
    list->capacity = 5;
    list->hueco = malloc(list->capacity * sizeof(deletedbook));
    if (!list->hueco) { free(list); fclose(fp); return NULL; }

    deletedbook temp;
    while (fread(&temp, sizeof(deletedbook), 1, fp) == 1)
    {
        if (list->size == list->capacity)
        {
            list->capacity *= 2;
            deletedbook *aux = realloc(list->hueco, list->capacity * sizeof(deletedbook));
            if (!aux)
            {
                fprintf(stderr, "Error al hacer realloc en deletedlist_load\n");
                free(list->hueco);
                free(list);
                fclose(fp);
                return NULL;
            }
            list->hueco = aux;
        }
        list->hueco[list->size++] = temp;
    }

    fclose(fp);
    return list; 
}
