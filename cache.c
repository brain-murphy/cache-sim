#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "cachesim.h"
#include "vc.h"

static inline uint8_t sub_block_index(uint64_t addr);
static inline uint64_t offset(uint64_t addr);
static void lookup(struct access acc);
static int find_invalid_entry(struct block **set);

uint64_t set_size;

struct block *blocks = NULL;
struct block **sets = NULL;

struct cache_stats_t stats;

void
init_cache(void)
{
    uint64_t num_blocks = 1u << (C - B);

    size_t blocks_size = sizeof(struct block) * num_blocks;
    size_t sets_size = sizeof(struct block*) * num_blocks;

    if (blocks == NULL) {
        blocks = calloc(1u << (16), sizeof(struct block));
    }
    memset(blocks, 0, blocks_size);

    if (sets == NULL) {
        sets = calloc(1u << (16), sizeof(struct block*));
    }
    memset(sets, 0, sets_size);

    for (int i = 0; i < num_blocks; i++) {
        sets[i] = &blocks[i];
    }

    init_vc();

    set_size = 1u << S;
    memset(&stats, 0, sizeof(struct cache_stats_t));
}

void
dealloc_cache(void)
{
    free(blocks);
    free(sets);
    dealloc_vc();
}

void
sim(struct access acc)
{
    stats.accesses += 1;

    if (acc.rw == READ) {
        stats.reads += 1;
    } else if (acc.rw == WRITE) {
        stats.writes += 1;
    }

    lookup(acc);
}

void
get_stats(struct cache_stats_t *pstats)
{
    if (V) {
        stats.hit_time = 2 + 0.1f * (1 << S);
        stats.miss_penalty = 100;
        double miss_rate_l1 = ((double) stats.misses) / (double) stats.accesses;
        double miss_rate_vc = (double) (stats.vc_misses + stats.subblock_misses) / (double) stats.vc_accesses;
        stats.miss_rate = miss_rate_l1 * miss_rate_vc;
        stats.avg_access_time = stats.hit_time + stats.miss_rate * stats.miss_penalty;
    } else {
        stats.hit_time = 2 + 0.1f * (1 << S);
        stats.miss_penalty = 100;
        stats.miss_rate = ((double)stats.misses + (double)stats.subblock_misses)/ (double)stats.accesses;
        stats.avg_access_time = stats.hit_time + stats.miss_rate * stats.miss_penalty;
    }
    *pstats = stats;
}

static void
lookup(struct access acc)
{
    uint64_t addr = acc.address;

    uint64_t set_index = m_index(addr) >> B;
    struct block **set = &sets[set_index << S];

    struct block *target = NULL;

    int target_index = 0;
    while (target_index < set_size) {
        struct block *block = set[target_index];
        if (block->valid && (tag(addr) == block->tag)) {
            // block hit
            target = block;
            break;
        }
        target_index += 1;
    }

    if (!target) {
        stats.misses += 1;
        if (acc.rw == READ) {
            stats.read_misses += 1;
        } else if (acc.rw == WRITE) {
            stats.write_misses += 1;
        }

        // check vc
        stats.vc_accesses += 1;
        struct block vc_block;
        int vc_hit = vc_search(&acc, &vc_block);
        if (vc_hit) {
            // evict LRU (no invalid blocks or else vc_block would never have been evicted
            target_index = (int) set_size - 1;
            struct block *lru = set[target_index];
            vc_insert(lru, set_index);

            target = lru;

            target->valid = 1;
            target->tag = tag(addr);
            target->dirty = vc_block.dirty;
            target->first_valid_sub_block = vc_block.first_valid_sub_block;
        }
    }

    if (!target) {
        if (acc.rw == READ) {
            stats.read_misses_combined += 1;
        } else if (acc.rw == WRITE) {
            stats.write_misses_combined += 1;
        }

        // miss repair
        int invalid_index = find_invalid_entry(set);
        if (invalid_index == -1) {
            // evict LRU
            invalid_index = (int) set_size - 1;
            struct block *lru = set[invalid_index];
            vc_insert(lru, set_index);
        }

        // for LRU reordering
        target_index = invalid_index;

        target = set[target_index];
        target->valid = 1;
        target->tag = tag(addr);
        target->dirty = 0;

        // fetch sub blocks but don't increment sub block misses
        target->first_valid_sub_block = sub_block_index(addr); // no valid sub blocks
        uint64_t num_valid_sub_blocks = (1u << (B - K)) - target->first_valid_sub_block;
        stats.bytes_transferred += num_valid_sub_blocks * (1u << K);
    }

    uint8_t sub_block = sub_block_index(addr);
    if (sub_block < target->first_valid_sub_block) {
        // fetch sub blocks
        stats.subblock_misses += 1;
        stats.bytes_transferred += (target->first_valid_sub_block - sub_block) * (1u << K);
        target->first_valid_sub_block = sub_block;
    }

    if (acc.rw == WRITE) {
        target->dirty = 1;
    }

    // update LRU ordering
    while (target_index > 0) {
        // move down the LRU ordering pointers
        set[target_index] = set[target_index - 1];
        target_index -= 1;
    }
    set[0] = target;
}

static int find_invalid_entry(struct block **set) {
    for (int i = 0; i < set_size; i++) {
        if (!set[i]->valid) {
            return i;
        }
    }

    return -1;
}

static inline uint8_t
sub_block_index(uint64_t addr)
{
    return (uint8_t)(offset(addr) >> K);
}

static inline uint64_t
offset(uint64_t addr)
{
    return (UINT64_MAX >> (64 - B)) & addr;
}

uint64_t
m_index(uint64_t addr)
{
    return (UINT64_MAX << B) & (UINT64_MAX >> (64 - (C - S))) & addr;
}

uint64_t
tag(uint64_t addr)
{
    return (UINT64_MAX << (B + S)) & addr;
}