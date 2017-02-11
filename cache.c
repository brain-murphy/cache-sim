#include <stdio.h>
#include <inttypes.h>

#include "cache.h"
#include "cachesim.h"

static inline uint64_t offset(uint64_t addr);
static inline uint64_t index(uint64_t addr);
static inline uint64_t tag(uint64_t addr);

void sim(struct access acc) {

}

void get_stats(struct cache_stats_t *stats) {

}


static inline uint64_t offset(uint64_t addr) {
    return (UINT64_MAX >> (64 - B)) & addr;
}

static inline uint64_t index(uint64_t addr) {
    return (UINT64_MAX << B) & (UINT64_MAX >> (64 - (C - S))) & addr;
}

static inline uint64_t tag(uint64_t addr) {
    return (UINT64_MAX << (B + S)) & addr;
}