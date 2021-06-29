#include <stdio.h>
#include "controller.h"

#define MIN_ARGS 2
#define MAX_ARGS 10

int main(int argc, char const *argv[]){
	if (argc < MIN_ARGS || argc > MAX_ARGS) return (int)error_handler(ARGV_QUANTITY);

	controller_t controller;

	size_t controller_ret, execute_ret;
	controller_ret = controller_init(&controller, argc, argv);
	if(controller_ret != 0){
		error_handler(controller_ret);
		return -1;
	}

	execute_ret = controller_execute(&controller);
	if(execute_ret != 0){
		error_handler(execute_ret);
		return -1;
	}

	controller_destroy(&controller);

	return 0;
}
