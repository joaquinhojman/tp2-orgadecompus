#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_FILE 50


#define SUCCESS 0
#define MISSING_ARGUMENT 1
#define NOT_HELP_NOR_VERSION 2
#define UNKNOWN_COMMAND 3
#define FILE_ERROR 4
#define ARGV_QUANTITY 5
#define SHOULDNT_REACH_HERE 6

#define WAYS 0
#define CACHE_SIZE 1
#define BLOCK_SIZE 2
#define OUTPUT 3
#define PRINT_HELP_OR_VERSION 4
#define DONT_KNOW 5



typedef struct controller{
	u_int ways;
	u_int cache_size;
	u_int block_size;
	char input[MAX_FILE];
	char output[MAX_FILE];
	bool print_version;
	bool print_help;
}controller_t;


//Inicializo el TDA controller.
size_t controller_init(controller_t * controller, int argc, char const *argv[]);

//Destruye el TDA liberando la memoria correspondiente.
void controller_destroy(controller_t * controller);

//Informa que se han ingresado argumentos inválidos.
size_t error_handler(size_t what);

//Ejecuta el programa con los parametros inicilizados.
size_t controller_execute(controller_t * controller);

#endif
