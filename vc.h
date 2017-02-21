#include "cache.h"
#ifndef CACHESIM_VC_H
#define CACHESIM_VC_H

struct node {
    struct block block;
    uint64_t index;
    struct node *next;
};

void init_vc(void);
void dealloc_vc(void);
void vc_insert(struct block *block, uint64_t index);
int vc_search(struct access *acc, struct block *block);

#endif //CACHESIM_VC_H
