#include "file.h"

#define FILE_ERROR 4
#define SUCCESS 0
#define BUF_SIZE 60
#define OPEN_MODE_READ "rb"
#define OPEN_MODE_WRITE "a"

size_t file_init(file_t* self, 
	const char* file_name_reader, const char* file_name_writer){
	self->file_reader = file_name_reader != NULL ? 
		fopen(file_name_reader,OPEN_MODE_READ) : stdin;

	self->file_writer = file_name_writer != NULL ? 
		fopen(file_name_writer,OPEN_MODE_WRITE) : stdout;

	if ((self->file_reader==NULL) || (self->file_writer==NULL)){
		return FILE_ERROR;
	}else{
		return SUCCESS;
	}
}

size_t file_uninit(file_t* self){
	size_t result_reader = 0;
	size_t result_writer = 0;
	if(self->file_reader != stdin)
		result_reader = (size_t)fclose(self->file_reader);
	if(self->file_writer != stdout)
		result_writer = (size_t)fclose(self->file_writer);
	if ((result_reader == FILE_ERROR) || (result_writer == FILE_ERROR)){
		return FILE_ERROR;
	}else{
		return SUCCESS;
	}
}

int parse_int(char ** buf, const char first_char, const char second_char){
	int i=0;
	char temp[20];
	char * pointer=*buf+1; 
	while(*pointer != first_char)
		pointer=*buf+i++;
	i=0;
	while(*pointer != second_char)
		temp[i++]=*(++pointer);
	temp[i]='\0';
	return atoi(temp);

}

size_t file_iterate(file_t* self){
	char * buf[BUF_SIZE];
	for (int i = 0; i < BUF_SIZE; i++) buf[i]=NULL;
	size_t status=0, read=0;
    init(); //Necesitamos inicializar el TDA siempre.
	fprintf(self->file_writer, "Inicialización\n");
	while ((read = (size_t)getline(buf, &read, self->file_reader)) != -1) {
        if (strncmp(*buf, "init\n", BUF_SIZE)==0){
        	continue;
        }else if(**buf=='W'){
        	int dir = parse_int(buf, ' ', ',');
        	int value = parse_int(buf, ',', '\n');
        	size_t temp = cache_memory.hit_amount; 
        	write_byte(dir, (unsigned char) value);
        	(temp == cache_memory.hit_amount)?
        		strcpy(*buf, "MISS\0"):strcpy(*buf, "HIT\0");
			fprintf(self->file_writer,
			 "Escritura del valor %i en la posición %i: %s\n", value, dir, *buf);
        }else if(**buf=='R'){
        	int dir = parse_int(buf, ' ', '\n');
        	size_t temp = cache_memory.hit_amount; 
        	char hit;
        	unsigned char result = read_byte(dir, &hit);//TODO computar hits con "hit"? o dejarlo como está?.
        	(temp == cache_memory.hit_amount)?
        		strcpy(*buf, "MISS\0"):strcpy(*buf, "HIT\0");
			fprintf(self->file_writer,
				"Lectura del valor %i en la posicion %i: %s\n", result, dir, *buf);
        }else if (strncmp(*buf, "MR\n", BUF_SIZE)==0){
			fprintf(self->file_writer, "Miss Rate: %i\n", get_miss_rate());
        }else{
        	destroy();
        	return FILE_ERROR;
        }
    }
    free(*buf);
    destroy();
    return status;
}
