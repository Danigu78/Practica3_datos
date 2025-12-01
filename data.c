#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

/* -----------------------------------------------------------
 * writeRecord:
 *   Escribe un registro de libro en formato binario y devuelve
 *   el tamaño del registro. Devuelve además el offset del inicio.
 *   En el fichero .db guardamos cada libro como: [size_t total] [int bookID] [16 chars isbn] [title]|[printedBy]
 * ----------------------------------------------------------- */
size_t writeRecord(FILE *db, int bookID, const char *isbn,
                   const char *title, const char *printedBy,
                   size_t *offset_out)
{
    if (!db || !isbn || !title || !printedBy)
        return 0;

    long offset = ftell(db);
    if (offset_out)
        *offset_out = offset;

    size_t title_len = strlen(title);
    size_t printed_len = strlen(printedBy);

    size_t total = sizeof(int) + 16 + (title_len + 1) + printed_len;

    fwrite(&total, sizeof(size_t), 1, db);
    fwrite(&bookID, sizeof(int), 1, db);
    fwrite(isbn, sizeof(char), 16, db);
    fwrite(title, sizeof(char), title_len, db);
    fwrite("|", sizeof(char), 1, db);
    fwrite(printedBy, sizeof(char), printed_len, db);

    return total;
}

/* -----------------------------------------------------------
 * readRecord:
 *   Lee un registro del fichero binario en el offset dado.
 * ----------------------------------------------------------- */
Record *readRecord(FILE *db, size_t offset)
{
    fseek(db, offset, SEEK_SET);

    size_t total;
    fread(&total, sizeof(size_t), 1, db);

    Record *r = malloc(sizeof(Record));

    fread(&(r->bookID), sizeof(int), 1, db);
    fread(r->isbn, sizeof(char), 16, db);
    r->isbn[16] = '\0';

    size_t remaining = total - (4 + 16);
    char *tmp = malloc(remaining + 1);
    fread(tmp, 1, remaining, db);
    tmp[remaining] = '\0';

    char *bar = strchr(tmp, '|');
    if (bar) {
        size_t tlen = bar - tmp;
        memcpy(r->title, tmp, tlen);
        r->title[tlen] = '\0';
        strcpy(r->printedBy, bar + 1);
    } else {
        r->title[0] = '\0';
        r->printedBy[0] = '\0';
    }

    free(tmp);
    return r;
}


size_t deleteRecord(FILE *db, size_t offset)
{

}

int db_add_record(const char *db_filename, DeletedList *deleted_list, Index *idx, Record *rec)
{
    
}


int db_delete_record(const char *db_filename, DeletedList *deleted_list, Index *idx, int bookID)
{
}


int convert_record(const char *line, Record *reg)
{
   
}
