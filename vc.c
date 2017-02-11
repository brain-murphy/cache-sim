#include <stdlib.h>
#include "vc.h"

struct cache_stats_t *stats;

extern uint64_t K;

void
init_vc(struct cache_stats_t *pstats)
{
    stats = pstats;
}

void
dealloc_vc(void)
{

}

int
vc_contains(struct access *acc)
{
    return 0;
}

void
vc_insert(struct block *block)
{
    if (block->dirty) {
        stats->write_backs += 1;

        uint64_t num_valid_sub_blocks = (1u << (B - K)) - block->first_valid_sub_block;

        stats->bytes_transferred += num_valid_sub_blocks * (1 << K);
    }
}


