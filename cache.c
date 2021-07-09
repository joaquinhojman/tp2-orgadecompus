#include "cache.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void set_all_principal_memory_zero() {
    for (int i = 0; i < PRINCIPAL_MEMORY_SIZE; i++)
        principal_memory[i] = 0;
}

void initialize_blocks() {
    for (int i = 0; i < cache_memory.block_amount; i++) {
        cache_memory.memory[i].data = malloc(block_size);
        if (cache_memory.memory[i].data == NULL)
            return;
        cache_memory.memory[i].V = 0;
        cache_memory.memory[i].fifo_value = 0;
    }
}

void init() {
    cache_memory.block_amount = (u_int)(
            cache_memory_size * pow(2, 10) / block_size);
    cache_memory.set_amount = cache_memory.block_amount / ways;
    cache_memory.memory = malloc(cache_memory.block_amount * sizeof(Block));

    if (cache_memory.memory == NULL)
        return;

    set_all_principal_memory_zero();
    initialize_blocks();
}

unsigned int find_set(int address) {
    // Asumo que los bytes por bloques son multiplo de 2
    unsigned int blocknum = (u_int) address >> (int) log2(block_size);
    return (u_int)(blocknum % cache_memory.set_amount);
}

unsigned int cache_block_number(int way, int setnum) {
    return (u_int) setnum * ways + (u_int) way;
}

void last_in_update(int new_blocknum) {
    int prev_fifo_value = cache_memory.memory[new_blocknum].fifo_value; // 0 if new
    int blocknum;
    for (int i = 0; i < ways; i++) {
        blocknum = (int) cache_block_number(i,
                                            (new_blocknum -
                                             new_blocknum % (int) ways) /
                                            (int) ways);
        if (cache_memory.memory[blocknum].V == 1) {
            if (blocknum == new_blocknum) {
                cache_memory.memory[blocknum].fifo_value = 1;
            } else {
                if (cache_memory.memory[blocknum].fifo_value < prev_fifo_value ||
                    prev_fifo_value == 0)
                    cache_memory.memory[blocknum].fifo_value++;
            }
        }
    }
}

unsigned int find_earliest(int setnum){
    for (int i = 0; i < ways; i++) {
        if (cache_memory.memory[cache_block_number(i, setnum)].fifo_value ==
            ways)
            return cache_block_number(i, setnum);
    }
    return 0; // Should not happen
}

unsigned int find_tag(int address) {
    return (u_int)(address >> (int) log2(block_size))
            >> (int) log2((double) cache_memory.set_amount);
}

// returns -1 if is not hit, otherwise returns blocknum
int hit_blocknum(int address) {
    unsigned int set = find_set(address);
    unsigned int tag = find_tag(address);

    unsigned int blocknum;
    for (int i = 0; i < ways; i++) {
        blocknum = cache_block_number(i, (int) set);
        if (cache_memory.memory[blocknum].V == 1 &&
            cache_memory.memory[blocknum].tag == tag) {
            return (int) (blocknum);
        }
    }

    return -1;
}

int find_spot(int address) {
    unsigned int set = find_set(address);

    for (int i = 0; i < ways; i++) {
        unsigned int blocknum = cache_block_number(i, (int) set);
        if (cache_memory.memory[blocknum].V == 0) {
            cache_memory.memory[blocknum].V = 1;
            return (int) (blocknum);
        }
    }

    return (int) find_earliest((int) set);
}

void move_block_to_cache(int blocknum) {
    int address = blocknum << (int) log2(block_size);
    int spot = find_spot(address);
    memcpy(cache_memory.memory[spot].data,
           &principal_memory[address],
           block_size);

    cache_memory.memory[spot].tag = find_tag(address) & 0xFFF;

    last_in_update(spot);
}

void read_block(int blocknum) {
    int address = blocknum << (int) log2(block_size);
    int hit_position = hit_blocknum(address);

    if (hit_position == -1) { // NO HIT
        move_block_to_cache(blocknum);
        cache_memory.miss_amount++;
    } else { // HIT
        cache_memory.hit_amount++;
    }
}

int get_address(int way, int setnum) {
    int blocknum = (int) cache_block_number(way, setnum);
    int address = ((cache_memory.memory[blocknum].tag
            << (u_int) log2((double) cache_memory.set_amount)) + setnum)
            << (u_int) log2(block_size);

    return address;
}

unsigned int get_byte_offset(unsigned int address) {
    return (unsigned int) (address
            << (unsigned int) (sizeof(int) * 8 - log2(block_size)))
            >> (unsigned int) (sizeof(int) * 8 - log2(block_size));
}

unsigned char read_byte(int address, char * hit) {
    size_t temp_hit_amount = cache_memory.hit_amount;
    read_block(address >> (int) log2(block_size));
    if (cache_memory.hit_amount == temp_hit_amount+1){
        *hit = '1';
    }else{
        *hit = '0';
    }
    int slot = hit_blocknum(address); // Hay hit por el read_block anterior
    return cache_memory.memory[slot].data[get_byte_offset((u_int) address)];
}

void write_byte_tomem(int address, unsigned char value) {
    principal_memory[address] = value;
}

char write_byte(int address, unsigned char value) {
    size_t temp_hit_amount = cache_memory.hit_amount;
    read_block(address >> (int) log2(block_size));

    int slot = hit_blocknum(address); // Hay hit por el read_block anterior
    cache_memory.memory[slot].data[get_byte_offset((u_int) address)] = value;
    
    write_byte_tomem(address, value);
    
    if (cache_memory.hit_amount == temp_hit_amount+1){
        return '1';
    }else{
        return '0';
    }    
}

int get_miss_rate() {
    size_t miss_requests = cache_memory.miss_amount * 100;
    size_t total_requests = cache_memory.miss_amount + cache_memory.hit_amount;
    if (total_requests == 0) return 0;
    return (int) (miss_requests / total_requests);
}

void destroy() {
    for (int i = 0; i < cache_memory.block_amount; i++)
        free(cache_memory.memory[i].data);

    free(cache_memory.memory);
}
