#include "cache.h"
#ifndef CACHESIM_VC_H
#define CACHESIM_VC_H

struct node {
    struct block *block;
    struct node *next;
    struct node *last;
};

void init_vc(struct cache_stats_t *pstats);
void dealloc_vc(void);
void vc_insert(struct block *block);
int vc_contains(struct access *acc);
struct block vc_remove(struct access *acc);

#endif //CACHESIM_VC_H
