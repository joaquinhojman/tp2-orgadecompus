#include "controller.h"
#include "file.h"
#include "cache.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define MIN_ALLOWED 2
#define MAX_OPTION 50

size_t interpret_argument(controller_t *controller, char const *arg, 
	bool * next_arg){
	if((strncmp("-V", arg, MAX_OPTION)==0) ||
	   (strncmp("--version", arg, MAX_OPTION)==0)){
		controller->print_version = true;
		return SUCCESS;
	} else if (((strncmp("-h", arg, MAX_OPTION)==0) ||
				(strncmp("--help", arg, MAX_OPTION)==0))){
		controller->print_help = true;
		return SUCCESS;
	} else if (((strncmp("-o", arg, MAX_OPTION)==0) ||
				(strncmp("--output", arg, MAX_OPTION)==0))){
		next_arg[0] = true;
		return SUCCESS;
	} else if (((strncmp("-w", arg, MAX_OPTION)==0) ||
				(strncmp("--ways", arg, MAX_OPTION)==0))){
		next_arg[1] = true;
		return SUCCESS;
	} else if (((strncmp("-cs", arg, MAX_OPTION)==0) ||
				(strncmp("--cachesize", arg, MAX_OPTION)==0))){
		next_arg[2] = true;
		return SUCCESS;
	} else if (((strncmp("-bs", arg, MAX_OPTION)==0) ||
				(strncmp("--blocksize", arg, MAX_OPTION)==0))){
		next_arg[3] = true;
		return SUCCESS;
	} else {
		if(next_arg[0]){
			strcpy(controller->output, arg);
			next_arg[0] = false;
			return SUCCESS;
		} else if(next_arg[1]){
			controller->ways = (u_int)strtoul(arg, NULL, 10);
			next_arg[1] = false;
			return SUCCESS;
		} else if(next_arg[2]){
			controller->cache_size = (u_int)strtoul(arg, NULL, 10);
			next_arg[2] = false;
			return SUCCESS;
		} else if(next_arg[3]){
			controller->block_size = (u_int)strtoul(arg, NULL, 10);
			next_arg[3] = false;
			return SUCCESS;
		} else if(next_arg[4]){
			strcpy(controller->input, arg);
			next_arg[4] = false;
			return SUCCESS;
		} else {
			return UNKNOWN_COMMAND;
		}
	}
}

size_t controller_init(controller_t * controller, int argc, char const *argv[]){
	controller->print_version = false;
	controller->print_help = false;
	controller->ways = 0;
	controller->cache_size = 0;
	controller->block_size = 0;
	strcpy(controller->output,"-"); //POR DEFAULT SE IMPRIME EN STDOUT

	if(argc == 2){
		controller->print_version = ((strncmp("-V", argv[1], MAX_OPTION)==0) ||
									 (strncmp("--version", argv[1], MAX_OPTION)==0));
		controller->print_help = ((strncmp("-h", argv[1], MAX_OPTION)==0) ||
									 (strncmp("--help", argv[1], MAX_OPTION)==0));
		if(!controller->print_version && !controller->print_help){
			return NOT_HELP_NOR_VERSION;
		} else {
			return SUCCESS; 
		}
	} else {
		bool next_arg[4];
		//output --> 0, ways-->1, cachesize-->2, blocksize-->3, input-->4 
		for (int i = 0; i < 4; i++)
			next_arg[i]=false;
		for (int i = 1; i < argc; i++) {
			if (i==argc-1) next_arg[4] = true;
			size_t possible_error = interpret_argument(controller,argv[i],next_arg);
			if(possible_error != SUCCESS)
				return possible_error;
		}
		if ((controller->ways==0) || 
			(controller->cache_size==0) || (controller->block_size==0))
			return MISSING_ARGUMENT; 
	}
	return SUCCESS;
}

void print_version(){
	fprintf(stdout, "Version 1.0\nOrganización de Computadoras - FIUBA\n");
}

void print_help(){
	fprintf(stdout, "Usage:\n\ttp2 -h\n\ttp2 -V\n\ttp2 [options] M N\n");
	fprintf(stdout, "Options:\n\t-h, --help     Imprime ayuda.\n");
	fprintf(stdout, "\t-V, --version  Versión del programa.\n");
	fprintf(stdout, "\t-o, --output   Archivo de salida.\n");
	fprintf(stdout, "\t-w, --ways  Cantidad de vías.\n");
	fprintf(stdout, "\t-cs, --cachesize   Tamaño del caché en kilobytes.\n");
	fprintf(stdout, "\t-bs, --blocksize   Tamaño del bloque en bytes.\n");
	fprintf(stdout, "Examples:\n\ttp2 -w 4 -cs 8 -bs 16 prueba1.mem\n");
}

size_t error_handler(size_t what){
	switch(what){
		case SUCCESS:
			break;
		case MISSING_ARGUMENT:
			fprintf(stderr, "There are missing arguments. Check carefully needed arguments.\n");
			print_help();
			break;
		case NOT_HELP_NOR_VERSION:
			fprintf(stderr, "Not asked for help nor version and only sent one argument.\n");	
			print_help();
			break;
		case UNKNOWN_COMMAND:
			fprintf(stderr, "A command sent is unknown.\n");	
			print_help();
			break;
		case FILE_ERROR:
			fprintf(stderr, "----------------------------\n");
			fprintf(stderr, "The file couldn't be read properly. Check it and retry.\n");	
			print_help();
			break;
		case ARGV_QUANTITY:
			fprintf(stderr, "You've entered too many arguments.\n");	
			print_help();
			break;
		case SHOULDNT_REACH_HERE:
			fprintf(stderr, "Programmer's fault.\n");	
			print_help();
			break;
		default:
			fprintf(stderr, "There was an unknown error\n");	
			print_help();
	}

	return SUCCESS;
}

size_t controller_execute(controller_t * controller){
	if(controller->print_help){
		print_help();
		return SUCCESS;
	} else if(controller->print_version) {
		print_version();
		return SUCCESS;
	}
	
	size_t file_result;
	file_t file;

	cache_memory_size = controller->cache_size;
	block_size = controller->block_size;
	ways = controller->ways;


	if(strncmp("-", controller->output, MAX_FILE)==0)	
		file_result = file_init(&file, controller->input, NULL);
	else
		file_result = file_init(&file, controller->input, controller->output);

	if (file_result != SUCCESS) return file_result;

	file_result = file_iterate(&file);

	if (file_result != SUCCESS) return file_result;

	file_result = file_uninit(&file);

	if (file_result != SUCCESS) return file_result;

	return SUCCESS;
}

void controller_destroy(controller_t * controller){

}
