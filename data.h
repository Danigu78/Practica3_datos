#ifndef DATA_H
#define DATA_H

#include <stdio.h>   /* para FILE* */
#include <stddef.h>  /* para size_t */
#include "deletedlist.h"
#include "index.h"

typedef struct {                 /* El tipo Record representa un libro leído */
    int  bookID;                 /* ID del libro */
    char isbn[17];               /* ISBN del libro */
    char title[129];             /* Título del libro */
    char printedBy[129];         /* Editorial */
} Record;

/* -----------------------------------------------------------
 * Escribe un registro y devuelve el tamaño total en bytes.
 * offset_out recibe el offset donde empieza el size_t en el fichero.
 * Formato en disco:
 * [size_t total][int bookID][16 chars isbn][title]|[printedBy]
 * ----------------------------------------------------------- */
size_t writeRecord(FILE *db, int bookID, const char *isbn,
                   const char *title, const char *printedBy,
                   size_t *offset_out);

/* -----------------------------------------------------------
 * Lee un registro en 'offset' y devuelve un puntero a Record (malloc).
 * El que llama debe hacer free().
 * ----------------------------------------------------------- */
Record *readRecord(FILE *db, size_t offset);

/* -----------------------------------------------------------
 * “Borra” un registro: solo lee su tamaño y lo devuelve.
 * El contenido queda en el fichero pero se considera hueco libre.
 * ----------------------------------------------------------- */
size_t deleteRecord(FILE *db, size_t offset);

/* -----------------------------------------------------------
 * Añade un registro a la base de datos binaria (*.db)
 * - Reutiliza huecos de deleted_list si existen
 * - Actualiza índice en memoria (idx)
 * - Guarda la lista de huecos en disco (*.lst)
 * ----------------------------------------------------------- */
int db_add_record(const char *db_filename, DeletedList *deleted_list, Index *idx, Record *rec);

/* -----------------------------------------------------------
 * Borra un registro de la base de datos
 * - Inserta hueco en deleted_list
 * - Elimina entrada del índice
 * - Guarda lista de huecos en disco (*.lst)
 * ----------------------------------------------------------- */
int db_delete_record(const char *db_filename, DeletedList *deleted_list, Index *idx, int bookID);


/*--Toma un strinf con barras y todo y saca los datos para convertirlo en un registro listo para añadir a la base*/



int convert_record( const char *line, Record *reg);
#endif 