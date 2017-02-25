#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "string.h"
#include "vc.h"

extern struct cache_stats_t stats;

static inline void write_back(struct block *block);
static void vc_insert_zero(struct block *block, uint64_t index);
static void vc_insert_one(struct block *block, uint64_t index);
static void vc_insert_default(struct block *block, uint64_t index);


static struct node *nodes;

static struct node *free_list;
static struct node *vc;

void (*insert_function) (struct block*, uint64_t);

uint64_t vc_hits = 0;


void
init_vc(void)
{
    nodes = calloc(V, sizeof(struct node));

    struct node *last_node = NULL;
    for (int i = (int)V - 1; i >= 0; i--) {
        nodes[i].next = last_node;
        last_node = &nodes[i];
    }

    free_list = last_node;

    switch (V) {
        case 0:
            insert_function = vc_insert_zero;
            break;
        case 1:
            insert_function = vc_insert_one;
            break;
        default:
            insert_function = vc_insert_default;
            break;
    }
}

void
dealloc_vc(void)
{
    free(nodes);
}

void
vc_insert(struct block *block, uint64_t index)
{
    insert_function(block, index);
}

static void
vc_insert_zero(struct block *block, uint64_t index)
{
    if (block->dirty) {
        write_back(block);
    }
}

static void
vc_insert_one(struct block *block, uint64_t index)
{
    if (free_list == NULL) {
        //evict block

        if (vc->block.dirty) {
            write_back(&vc->block);
        }

    } else {
        vc = free_list;
        free_list = NULL;
    }

    vc->block = *block;
    vc->index = index;
}

static void
vc_insert_default(struct block *block, uint64_t index)
{
    struct node *new_node;
    if (free_list == NULL) {
        //evict first in.
        struct node *current_node = vc;
        while (current_node->next->next) { // want second to last
            current_node = current_node->next;
        }

        new_node = current_node->next;
        current_node->next = NULL;

        if (new_node->block.dirty) {
            write_back(&new_node->block);
        }

    } else {
        new_node = free_list;
        free_list = free_list->next;
    }

    new_node->block = *block;
    new_node->index = index;

    new_node->next = vc;
    vc = new_node;
}

static inline void
write_back(struct block *block)
{
    stats.write_backs += 1;

    uint64_t num_valid_sub_blocks = (1u << (B - K)) - block->first_valid_sub_block;

    stats.bytes_transferred += num_valid_sub_blocks * (1u << K);
}

int vc_search(struct access *acc, struct block *block)
{
    if (V == 0) {
        return 0;
    }

    struct node *current_node = vc;
    struct node *previous_node = NULL;

    uint64_t index = m_index(acc->address);

    while (current_node) {
        if ((current_node->block.tag == tag(acc->address)) && (current_node->index == index)) {
            break;
        }
        previous_node = current_node;
        current_node = current_node->next;
    }

    if (current_node) {
        if (previous_node) {
            //unlink node
            previous_node->next = current_node->next;
        } else {
            vc = current_node->next;
        }

        current_node->next = NULL;

        *block = current_node->block;

        memset(&current_node->block, 0, sizeof(struct block));
        current_node->index = 0;

        current_node->next = free_list;
        free_list = current_node;

        vc_hits += 1;
        return 1;
    }

    stats.vc_misses += 1;
    return 0;
}


