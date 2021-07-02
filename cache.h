#ifndef TP2_OC_CACHE_H
#define TP2_OC_CACHE_H

#include <stdlib.h>

#define PRINCIPAL_MEMORY_SIZE 65536

typedef struct block {
    unsigned char *data;
    int lru_value;
    int fifo_value;
    unsigned int tag: 12;
    unsigned int V: 1;
} Block;

typedef struct cache {
    Block *memory;
    size_t block_amount;
    size_t set_amount;
    size_t hit_amount;
    size_t miss_amount;
} Cache;

Cache cache_memory;
unsigned char principal_memory[PRINCIPAL_MEMORY_SIZE];
unsigned int cache_memory_size;
unsigned int block_size;
unsigned int ways;

void init();

unsigned int find_set(int address);

unsigned int find_earliest(int setnum);//

void read_block(int blocknum);

void write_byte_tomem(int address, char value);

void write_block(int way, int setnum);

unsigned char read_byte(int address);//

void write_byte(int address, unsigned char value);

int get_miss_rate();

void destroy();

#endif //TP2_OC_CACHE_H
