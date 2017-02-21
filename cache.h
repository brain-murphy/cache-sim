#include <stdint.h>

#include "cachesim.h"

#ifndef CACHE_H
#define CACHE_H

void init_cache(void);
void dealloc_cache(void);

struct block {
    int valid;
    int dirty;
    uint64_t tag;
    uint8_t first_valid_sub_block;
};

struct access {
    char rw;
    uint64_t address;
};

void sim(struct access acc);

void get_stats(struct cache_stats_t *stats);

uint64_t tag(uint64_t addr);
uint64_t m_index(uint64_t addr);


#endif