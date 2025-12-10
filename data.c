#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

/* -----------------------------------------------------------
 * writeRecord: Escribe registro en binario
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
 * readRecord: Lee registro del binario
 * ----------------------------------------------------------- */
Record *readRecord(FILE *db, size_t offset)
{
    fseek(db, offset, SEEK_SET);

    size_t total;
    if (fread(&total, sizeof(size_t), 1, db) != 1) return NULL;

    Record *r = malloc(sizeof(Record));
    if (!r) return NULL;

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
 * deleteRecord: Devuelve tamaño (no borra físicamente)
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
 * AÑADE REGISTRO Y GUARDA LA LISTA CON EL NOMBRE CORRECTO (.lst)
 * ----------------------------------------------------------- */
int db_add_record(const char *db_filename, DeletedList *deleted_list, Index *idx, Record *rec)
{
    indexbook entry;
    if (!db_filename || !deleted_list || !idx || !rec)
        return -1;

    FILE *f = fopen(db_filename, "r+b");
    if (!f) {
        f = fopen(db_filename, "w+b");
        if (!f) { printf("Error al abrir fichero .db\n"); return -1; }
    }

    int pos;
    int found_hole;
      size_t offset, record_size;
    deletedbook hb;
    deletedbook new_hole;
    if (index_binary_search(idx, rec->bookID, &pos) >= 0) {
        printf("Record with BookID=%d exists.\n", rec->bookID);
        fclose(f);
        return -1;
    }

  

    record_size = sizeof(int) + 16 + strlen(rec->title) + 1 + strlen(rec->printedBy);
     found_hole = deletedlist_find(deleted_list, record_size, &hb);

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

   
    entry.key = rec->bookID;
    entry.offset = offset; 
    entry.size = record_size;

    index_insert(idx, entry);

    char list_filename[256];
    strncpy(list_filename, db_filename, 250);
    list_filename[250] = '\0';
    
    char *ext = strrchr(list_filename, '.');
    if (ext) {
        strcpy(ext, ".lst");
    } else {
        strcat(list_filename, ".lst");
    }
    

    deletedlist_save(deleted_list, list_filename);

    printf("Record with BookID=%d has been added to the database\n", rec->bookID);
    fclose(f);
    return 0;
}

/* -----------------------------------------------------------
 * db_delete_record:
 * BORRA REGISTRO Y GUARDA LA LISTA CON EL NOMBRE CORRECTO (.lst)
 * ----------------------------------------------------------- */
int db_delete_record(const char *db_filename, DeletedList *deleted_list, Index *idx, int bookID)
{
    if (!db_filename || !deleted_list || !idx) return -1;

    int pos = index_binary_search(idx, bookID, NULL); 
    
    if (pos < 0) {
        printf("Item with key %d does not exist\n", bookID);
        return -1;
    }

    FILE *f = fopen(db_filename, "r+b");
    if (!f) { perror("Error al abrir fichero .db"); return -1; }

    // Casteamos a size_t por compatibilidad con fseek
    size_t offset = (size_t)idx->array[pos].offset;
    size_t size = deleteRecord(f, offset);
   
    deletedbook hb;
    hb.offset = offset;
    hb.size = size;
    
    deletedlist_insert(deleted_list, hb);

    index_remove(idx, pos);

    // --- CORRECCIÓN DEL NOMBRE DEL FICHERO ---
    char list_filename[256];
    strncpy(list_filename, db_filename, 250);
    list_filename[250] = '\0';

    char *ext = strrchr(list_filename, '.');
    if (ext) {
        strcpy(ext, ".lst");
    } else {
        strcat(list_filename, ".lst");
    }
    // -----------------------------------------

    deletedlist_save(deleted_list, list_filename);

    printf("Record with BookID=%d has been deleted from the database\n", bookID);
    fclose(f);
    return 0;
}

/* -----------------------------------------------------------
 * convert_record: Convierte texto a struct
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