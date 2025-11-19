#ifndef DATA_H
#define DATA_H

#include <stdio.h>   /* para FILE* */
#include <stddef.h>  /* para size_t */

typedef struct {                 /* El tipo Record representa un libro leído */
    int  bookID;                 /* ID del libro */
    char isbn[17];               /* ISBN del libro */
    char title[129];             /* Título del libro */
    char printedBy[129];         /* Editorial */
} Record;

/* Escribe un registro y devuelve el tamaño total (sin contar el size_t)
 * offset_out recibe el offset donde empieza el size_t en el fichero.
 */
size_t writeRecord(FILE *db, int bookID, const char *isbn,
                   const char *title, const char *printedBy,
                   size_t *offset_out);

/* Lee un registro en 'offset' y devuelve un puntero a Record (malloc).
 * El que llama debe hacer free().
 */
Record *readRecord(FILE *db, size_t offset);

/* “Borra” un registro: solo lee su tamaño y lo devuelve.
 * El contenido queda en el fichero pero se considera hueco libre.
 */
size_t deleteRecord(FILE *db, size_t offset);

#endif /* DATA_H */
