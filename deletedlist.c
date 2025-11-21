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
void deletedlist_sort(DeletedList *list)
{
    if (!list)
        return;

    /* FIRSTFIT no necesita ordenar */
    if (list->strategy == FIRSTFIT)
        return;

    deletedbook temporal;
    for (size_t i = 0; i < list->size; i++)
    {
        for (size_t j = i + 1; j < list->size; j++)
        {
            if ((list->strategy == BESTFIT && list->hueco[i].size > list->hueco[j].size) ||
                (list->strategy == WORSTFIT && list->hueco[i].size < list->hueco[j].size))
            {
                temporal = list->hueco[i];
                list->hueco[i] = list->hueco[j];
                list->hueco[j] = temporal;
            }
        }
    }
}
void deletedlist_insert(DeletedList *list, deletedbook hb)
{
    deletedbook *aux = NULL;
    if (!list)
        return;

    if (list->size == list->capacity)
    {
        list->capacity *= 2;

        aux = realloc(list->hueco, list->capacity * sizeof(deletedbook));
        if (!aux)
        {
            fprintf(stderr, "Error al hacer realloc en deletedlist_insert\n");
            return;
        }

        list->hueco = aux;
    }

    /* Insertar al final */
    list->hueco[list->size] = hb;
    list->size++;

    /* Ordenar después de insertar */
    deletedlist_sort(list);
}

/*Buscar hueco según estrategia*/
int deletedlist_find(DeletedList *list, size_t needed_size, deletedbook *hb)
{
    if (!list || !hb)
        return 0;

    int i;
    int found_index = -1;

    deletedlist_sort(list);

    if (list->strategy == FIRSTFIT)
    {
        for (i = 0; i < list->size; i++)
        {
            if (list->hueco[i].size >= needed_size)
            {
                found_index = i;
                break; // Primer hueco que cabe
            }
        }
    }
    else
    {
        /*La lista ya esta ordenada luego vamos acoger el primer elemento que quepa, es decir si el heuco es mayor que lo que necesitamos ya lo cogemos*/
        for (i = 0; i < list->size; i++)
        {
            if (list->hueco[i].size >= needed_size)
            {
                found_index = i;
                break; /*Solo buscamos uno donde quepa luyego salimos del bucle*/
            }
        }
    }

    if (found_index == -1) /*No se ha encontrado un hueco donde quepa por lo que si queremos implementar un nuevo libro a la biblioteca habra que hacerlo al final del registro*/
        return 0;

    /*Copiamos los datos del fichero que hemos encontrado*/
    *hb = list->hueco[found_index];

    if (list->hueco[found_index].size == needed_size) /*Di el hueco encontrado es exactamente igual de tamaño que lo que necesitamos lo que hacemos es eliminar de la lista ese hueco */
    {
        //
        for (i = found_index; i < list->size - 1; i++)
        {
            list->hueco[i] = list->hueco[i + 1]; /*Desplazamos */
        }
        list->size--; /*Bajamos el numero de huecos que hay en la lista*/
    }
    else
    {
        /* Hueco más grande: ajustamos offset y tamaño del hueco sobrante*/
        list->hueco[found_index].offset += needed_size;
        list->hueco[found_index].size -= needed_size;

        hb->size = needed_size;
    }

    return 1; /*Hueco encontrado y asignado*/
}

void deletedlist_destroy(DeletedList *list)
{
    if (!list)
        return;
    if (list->hueco)
        free(list->hueco);
    free(list);
}

/*
 * Guardar la lista de huecos en disco (*.lst) en formato binario
 * Estructura del fichero:
 * [int strategy][deletedbook 1][deletedbook 2]...[deletedbook n]
 * Devuelve 1 si se guardó correctamente, 0 si hubo error.
 */
int deletedlist_save(DeletedList *list, const char *filename)
{
    int i;
    if (list == NULL || !filename)
    {
        return EXIT_FAILURE;
    }
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Error al abrir el fichero %s para escritura\n", filename);
        return 0;
    }
    /*Aqui metemos la estrategia que vamos a usar en cada de meter nuevos*/
    if (fwrite(&list->strategy, sizeof(int), 1, fp) != 1)
    {
        fprintf(stderr, "Error al escribir la estrategia que utilizaremos para rellenar huecos een %s\n", filename);
        fclose(fp);
        return 0;
    }

    for (i = 0; i < list->size; i++)
    {
        if (fwrite(&list->hueco[i], sizeof(deletedbook), 1, fp) != 1) /*Esa funcion devuelve 1 si se ha escrito bien la linea*/
        {
            fprintf(stderr, "Error al escribir un hueco");
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return 1; // Guardado correcto
}
DeletedList *deletedlist_load(const char *filename)
{
    DeletedList *list = NULL;
    deletedbook temp;
    if (filename == NULL)
    {
        return NULL;
    }
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "No se ha podido leer el fichero de datos");
        return NULL;
    }
    list = (DeletedList *)malloc(sizeof(DeletedList));
    if (list == NULL)
    {
        return NULL;
    }
    if (fread(&list->strategy, sizeof(int), 1, fp) != 1)
    {
        fprintf(stderr, "Error al leer la estrategia del fichero");
        free(list);
        fclose(fp);
        return NULL;
    }
    list->size = 0;
    list->capacity = 5;
    list->hueco = (deletedbook *)malloc(list->capacity * sizeof(deletedbook));
    if (!list->hueco)
    {
        free(list);
        fclose(fp);
        return NULL;
    }

    while (fread(&temp, sizeof(deletedbook), 1, fp) == 1)
    {
        // Realloc si es necesario
        if (list->size == list->capacity)
        {
            list->capacity *= 2;
            deletedbook *aux = (deletedbook *)realloc(list->hueco, list->capacity * sizeof(deletedbook));
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

        /*Añaadimos el hueco a la lista*/
        list->hueco[list->size++] = temp;
    }

    fclose(fp);
}
