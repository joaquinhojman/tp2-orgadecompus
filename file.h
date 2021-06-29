#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cache.h"

typedef struct file {
    FILE *file_reader;
    FILE *file_writer;
} file_t;

size_t file_init(file_t *self,
                 const char *file_name_reader, const char *file_name_writer);

size_t file_uninit(file_t *self);

//Realiza las iteraciones en la lectura del archivo e
//imprime en el archivo correspondiente el resultado
size_t file_iterate(file_t *self);

#endif
