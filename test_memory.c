#include <stdio.h>
#include <stdlib.h>
#include "index.h"
#include "deletedlist.h"

void test_index() {
    printf("=== TEST INDEX ===\n");
    Index *idx = index_create();
    if (!idx) { printf("Error creando índice\n"); return; }

    indexbook entry1 = {123, 0, 0};
    indexbook entry2 = {456, 0, 0};
    indexbook entry3 = {789, 0, 0};

    index_insert(idx, entry2);
    index_insert(idx, entry1);
    index_insert(idx, entry3);

    printf("Indice ordenado por key:\n");
    for (size_t i = 0; i < idx->size; i++)
        printf("Key=%d, Offset=%d, Size=%d\n",
               idx->array[i].key, idx->array[i].offset, idx->array[i].size);

    int pos;
    int found = index_binary_search(idx, 456, &pos);
    printf("Busqueda key=456 -> found=%d, pos=%d\n", found, pos);

    index_remove(idx, 1);
    printf("Despues de eliminar pos 1:\n");
    for (size_t i = 0; i < idx->size; i++)
        printf("Key=%d, Offset=%d, Size=%d\n",
               idx->array[i].key, idx->array[i].offset, idx->array[i].size);

    index_free(idx);
}

void test_deletedlist() {
    printf("\n=== TEST DELETEDLIST ===\n");
    DeletedList *list = deletedlist_create(FIRSTFIT);
    if (!list) { printf("Error creando deletedlist\n"); return; }

    deletedbook hb1 = {50, 0};
    deletedbook hb2 = {30, 100};
    deletedbook hb3 = {40, 200};

    deletedlist_insert(list, hb1);
    deletedlist_insert(list, hb2);
    deletedlist_insert(list, hb3);

    printf("Huecos en memoria:\n");
    for (size_t i=0; i<list->size; i++)
        printf("Offset=%zu, Size=%zu\n", list->hueco[i].offset, list->hueco[i].size);

    // Guardar en disco
    deletedlist_save(list, "test.lst");

    // Cargar de disco
    DeletedList *loaded = deletedlist_load("test.lst");
    printf("Huecos cargados:\n");
    for (size_t i=0; i<loaded->size; i++)
        printf("Offset=%zu, Size=%zu\n", loaded->hueco[i].offset, loaded->hueco[i].size);

    deletedlist_destroy(list);
    deletedlist_destroy(loaded);
}

int main() {
    test_index();
    test_deletedlist();
    return 0;
}
