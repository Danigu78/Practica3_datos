#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "deletedlist.h"
#include "index.h"

#define LINE_MAX 512

int main(int argc, char *argv[])
{
    int strategy;
    char db_filename[256], lst_filename[256];
    char line[LINE_MAX];
    Index *indice = NULL;
    DeletedList *del = NULL;
    Record rec;
    FILE *f;
    Record *r;
    int i;
    int pos;
    int book_Id;

    if (argc < 3)
    {
        fprintf(stderr, "Error. Tienes que usar %s <strategy> <db_name>\n", argv[0]);
        fprintf(stderr, "Strategy: first_fit, best_fit, worst_fit\n");
        return 1;
    }

    if (strcmp(argv[1], "first_fit") == 0)
        strategy = FIRSTFIT;
    else if (strcmp(argv[1], "best_fit") == 0)
        strategy = BESTFIT;
    else if (strcmp(argv[1], "worst_fit") == 0)
        strategy = WORSTFIT;
    else
    {
        fprintf(stderr, "Estrategia desconocida: %s\n", argv[1]);
        return 1;
    }

    snprintf(db_filename, 256, "%s.db", argv[2]);
    snprintf(lst_filename, 256, "%s.lst", argv[2]);

    // Asegurarse de que el fichero de datos existe
    f = fopen(db_filename, "rb");
    if (!f)
    {
        f = fopen(db_filename, "wb");
        if (!f)
        {
            perror("No se pudo crear el fichero de datos");
            return 1;
        }
    }
    fclose(f);

    indice = index_create();
    if (!indice)
    {
        fprintf(stderr, "Error creando el índice\n");
        return 1;
    }

    del = deletedlist_load(lst_filename);
    if (!del)
    {
        
        del = deletedlist_create(strategy);
        if (!del)
        {
            fprintf(stderr, "Error creando la lista de borrados\n");
            index_free(indice);
            return 1;
        }
    }

    while (1)
    {
        printf("Type command and argument/s.\n");
        fflush(stdout);

        if (!fgets(line, LINE_MAX, stdin))
            break;
        line[strcspn(line, "\r\n")] = 0;
        /*Aqui ya deja de escribirse si escribimos alfo*/
        if (strncmp(line, "add ", 4) == 0)
        {
            if (convert_record(line + 4, &rec))
            {
                db_add_record(db_filename, del, indice, &rec);
            }
            else
            {
                printf("Error. El formato introducido para add es incorrecto\n");
            }
        }

        else if (strncmp(line, "del ", 4) == 0)
        {
            book_Id = atoi(line + 4);
            db_delete_record(db_filename, del, indice, book_Id);
        }
        else if (strncmp(line, "find ", 5) == 0)
        {
            book_Id = atoi(line + 5);
            if (index_binary_search(indice, book_Id, &pos) >= 0)
            {
                f = fopen(db_filename, "rb");
                if (f)
                {
                    r = readRecord(f, indice->array[pos].offset);
                    if (r)
                    {
                        printf("%d|%s|%s|%s\n", r->bookID, r->isbn, r->title, r->printedBy);
                        free(r);
                    }
                    fclose(f);
                }
            }
            else
            {
                printf("Record with bookId=%d does not exist\n", book_Id);
            }
        }
        else if (strcmp(line, "printInd") == 0)
        {
            for (i = 0; i < (int)indice->size; i++)
            {
                printf("Entry #%d\n    key: #%d\n    offset: #%ld\n",
                       i, indice->array[i].key, indice->array[i].offset);
            }
        }
        else if (strcmp(line, "printLst") == 0)
        {
            for (i = 0; i < (int)del->size; i++)
            {
                printf("Entry #%d\n    offset:#%ld    size:#%ld\n",
                       i, del->hueco[i].offset, del->hueco[i].size);
            }
        }
        else if (strncmp(line, "printRec ", 9) == 0)
        {
            for (i = 0; i < (int)indice->size; i++)
            {
                f = fopen(db_filename, "rb");
                if (!f)
                {
                    printf("Error abriendo db para printRec\n");
                    break;
                }

                r = readRecord(f, indice->array[i].offset);
                if (r)
                {
                    printf("%d|%s|%s|%s\n", r->bookID, r->isbn, r->title, r->printedBy);
                    free(r);
                }
                fclose(f);
            }
        }
        else if (strcmp(line, "exit") == 0)
        {
            break;
        }
        else
        {
            printf("comando desconocido: %s\n", line);
        }
    }

    deletedlist_save(del, lst_filename);
    deletedlist_destroy(del);
    index_free(indice);

    return 0;
}
