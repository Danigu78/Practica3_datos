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

/* -----------------------------------------------------------
 * deleteRecord:
 *   No borra realmente, solo devuelve el tamaño del registro.
 * ----------------------------------------------------------- */
size_t deleteRecord(FILE *db, size_t offset)
{
    fseek(db, offset, SEEK_SET);
    size_t total;
    fread(&total, sizeof(size_t), 1, db);
    return total;
}

/* -----------------------------------------------------------
 * db_add_record:
 *   Añade un registro en el fichero, utiliza DeletedList y Index
 * ----------------------------------------------------------- */
int db_add_record(const char *db_filename, DeletedList *deleted_list, Index *idx, Record *rec)
{
    if (!db_filename || !deleted_list || !idx || !rec)
        return -1;

    FILE *f = fopen(db_filename, "r+b");
    if (!f) {
        f = fopen(db_filename, "w+b");
        if (!f) { printf("Error al abrir fichero .db\n"); return -1; }
    }

    int pos;
    if (index_binary_search(idx, rec->bookID, &pos) >= 0) {
        printf("Record with BookID=%d exists.\n", rec->bookID);
        fclose(f);
        return -1;
    }

    size_t offset, record_size;
    deletedbook hb;

    // Buscar hueco en DeletedList
    record_size = sizeof(int) + 16 + strlen(rec->title) + 1 + strlen(rec->printedBy);
    int found_hole = deletedlist_find(deleted_list, record_size, &hb);

    if (found_hole) {
        fseek(f, hb.offset, SEEK_SET);
        writeRecord(f, rec->bookID, rec->isbn, rec->title, rec->printedBy, &offset);

        if (hb.size > record_size) {
            deletedbook new_hole = {hb.offset + record_size, hb.size - record_size};
            deletedlist_insert(deleted_list, new_hole);
        }
    } else {
        fseek(f, 0, SEEK_END);
        writeRecord(f, rec->bookID, rec->isbn, rec->title, rec->printedBy, &offset);
    }

    indexbook entry = {rec->bookID, offset, record_size};
    index_insert(idx, entry);

    deletedlist_save(deleted_list, "deleted.lst");

    printf("Record with BookID=%d has been added to the database\n", rec->bookID);
    fclose(f);
    return 0;
}

/* -----------------------------------------------------------
 * db_delete_record:
 *   Marca registro como hueco y actualiza índice y DeletedList
 * ----------------------------------------------------------- */
int db_delete_record(const char *db_filename, DeletedList *deleted_list, Index *idx, int bookID)
{
    if (!db_filename || !deleted_list || !idx) return -1;

    int pos;
    if (index_binary_search(idx, bookID, &pos) < 0) {
        printf("Record with BookID=%d not found\n", bookID);
        return -1;
    }

    FILE *f = fopen(db_filename, "r+b");
    if (!f) { perror("Error al abrir fichero .db"); return -1; }

    size_t offset = idx->array[pos].offset;
    size_t size = deleteRecord(f, offset);

    deletedbook hb = {offset, size};
    deletedlist_insert(deleted_list, hb);

    index_remove(idx, pos);

    deletedlist_save(deleted_list, "deleted.lst");

    printf("Record with BookID=%d has been deleted from the database\n", bookID);
    fclose(f);
    return 0;
}

/* -----------------------------------------------------------
 * convert_record:
 *   Convierte línea de texto a Record
 * ----------------------------------------------------------- */
int convert_record(const char *line, Record *reg)
{
    if (!line || !reg) return 0;

    char temp[512];
    strncpy(temp, line, 511);
    temp[511] = '\0';

    char *token = strtok(temp, "|");
    if (!token) return 0;
    reg->bookID = atoi(token);

    token = strtok(NULL, "|");
    if (!token) return 0;
    strncpy(reg->isbn, token, 16);
    reg->isbn[16] = '\0';

    token = strtok(NULL, "|");
    if (!token) return 0;
    strncpy(reg->title, token, 128);
    reg->title[128] = '\0';

    token = strtok(NULL, "|");
    if (!token) return 0;
    strncpy(reg->printedBy, token, 128);
    reg->printedBy[128] = '\0';

    return 1;
}
