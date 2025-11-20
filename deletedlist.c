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

/* Ordenar lista de huecos según la estrategia */
void deletedlist_sort(DeletedList *list) {
    if (!list) return;

    /* FIRSTFIT no necesita ordenar */
    if (list->strategy == FIRSTFIT) return;

    deletedbook temporal;
    for (size_t i = 0; i < list->size; i++) {
        for (size_t j = i + 1; j < list->size; j++) {
            if ((list->strategy == BESTFIT && list->hueco[i].size > list->hueco[j].size) ||
                (list->strategy == WORSTFIT && list->hueco[i].size < list->hueco[j].size)) {
                temporal = list->hueco[i];
                list->hueco[i] = list->hueco[j];
                list->hueco[j] = temporal;
            }
        }
    }
}

void deletedlist_insert(DeletedList *list, deletedbook hb) {
    if (!list) return;
    if (list->size == list->capacity) deletedlist_expand(list);
    list->hueco[list->size++] = hb;/*Lo metemos al final pero una vez metido la ordenamos*/
    deletedlist_sort(list);/*CON ESTA FUNCION SI EL */
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
