#include <stdint.h>

#include "cachesim.h"

#ifndef CACHE_H
#define CACHE_H


struct access {
    char rw;
    uint64_t address;
};

void sim(struct access acc);

void get_stats(struct cache_stats_t *stats);


#endif