#include "deletedlist.h"

DeletedList *deletedlist_create(int strategy)
{
    if ((strategy != 0) && (strategy != 1) && (strategy != 2))
    {
        fprintf(stderr, "La estrategia elegida no es valida");
        return NULL;
    }
    DeletedList *array = NULL;
    array = (DeletedList *)malloc(sizeof(DeletedList));
    if (array == NULL)
    {
        printf("j");
        return NULL;
    }
    array->capacity = 5;
    array->size = 0;
    array->strategy = strategy;
    array->hueco = (deletedbook *)malloc(array->capacity * sizeof(deletedbook));
    if (!array->hueco)
    {
        free(array);
        return NULL;
    }
    return array;
}

void deletedlist_insert(DeletedList *list, deletedbook hb)
{
    int i;
    deletedbook *aux;
    if (!list)
    {
        fprintf(stderr, "No hay lista de huecos");
        return;
    }
    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        aux = realloc(list->hueco, list->capacity * sizeof(deletedbook));
        if (aux == NULL)
        {
            return;
        }
        list->hueco = aux;
    }
    if (list->strategy = FIRSTFIT)
    {
        list->hueco[list->size] = hb;
        list->size++;
    }
    i = list->size - 1;
    if (list->strategy == BESTFIT)
    {
        while (i>=0 && list->hueco[i].size>hb.size)
        {
            list->hueco[i+1]=list->hueco[i];
            i--;
        }
        
    }
}
/*Buscar hueco según estrategia*/
int deletedlist_find(DeletedList *list, size_t size_needed)
{
    if (!list || list->size == 0) /*Si en el array de huecos no hay hucos no podemos encontrar un hueco donde insertar nada*/
    {
        return -1;
    }
    int pos = -1;
    int best_size;
    int i;
    size_t bestsize;
    switch (list->strategy)
    {
    case FIRSTFIT:
        for (i = 0; i < list->size; i++)
        {
            if (list->hueco[i].size >= size_needed) /*Buscamos el primer hueco que nos valga*/
            {
                return i; /*Devolvemos el primero que nos valaga*/
            }
        }
        break;
    case BESTFIT:
        best_size = (size_t)-1;
        for (int i = 0; i < (int)list->size; i++)
        {
            if (list->hueco[i].size >= size_needed && list->hueco[i].size < best_size)
            {
                best_size = list->hueco[i].size;
                pos = i;
            }
        }
        return pos;

    case WORSTFIT:
        best_size = 0;
        for (int i = 0; i < (int)list->size; i++)
        {
            if (list->hueco[i].size >= size_needed && list->hueco[i].size > best_size)
            {
                best_size = list->hueco[i].size;
                pos = i;
            }
        }
        return pos;

    default:
        return -1;
    }

    return -1;
}

void deletedlist_remove(DeletedList *list, int pos)
