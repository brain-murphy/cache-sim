#include <stdint.h>

#ifndef CACHESIM_H
#define CACHESIM_H

struct cache_stats_t {
    uint64_t accesses;

    uint64_t reads;
    uint64_t read_misses;
    uint64_t read_misses_combined;

    uint64_t writes;
    uint64_t write_misses;
    uint64_t write_misses_combined;

    uint64_t misses;
	uint64_t write_backs;
	uint64_t vc_misses;

    uint64_t subblock_misses;

	uint64_t bytes_transferred; 
   
	double   hit_time;
    double   miss_penalty;
    double   miss_rate;
    double   avg_access_time;
};

static const uint64_t DEFAULT_C = 15;   /* 32KB Cache */
static const uint64_t DEFAULT_B = 6;    /* 64-byte blocks */
static const uint64_t DEFAULT_S = 3;    /* 8 blocks per set */
static const uint64_t DEFAULT_V = 4;    /* 4 victim blocks */
static const uint64_t DEFAULT_K = 3;	/* 8 byte sub-blocks */

/** Argument to cache_access rw. Indicates a load */
static const char     READ = 'r';
/** Argument to cache_access rw. Indicates a store */
static const char     WRITE = 'w';

/*
Function to print the final statistics from your simulator run. Please copy and paste
wherever you are going to use this.

void print_statistics(cache_stats_t* p_stats) {
    printf("Cache Statistics\n");
    printf("Accesses: %" PRIu64 "\n", p_stats->accesses);
    printf("Reads: %" PRIu64 "\n", p_stats->reads);
    printf("Read misses: %" PRIu64 "\n", p_stats->read_misses);
    printf("Read misses combined: %" PRIu64 "\n", p_stats->read_misses_combined);
    printf("Writes: %" PRIu64 "\n", p_stats->writes);
    printf("Write misses: %" PRIu64 "\n", p_stats->write_misses);
    printf("Write misses combined: %" PRIu64 "\n", p_stats->write_misses_combined);
    printf("Misses: %" PRIu64 "\n", p_stats->misses);
    printf("Writebacks: %" PRIu64 "\n", p_stats->write_backs);
    printf("Victim cache misses: %" PRIu64 "\n", p_stats->vc_misses);
    printf("Sub-block misses: %" PRIu64 "\n", p_stats->subblock_misses);
    printf("Bytes transferred to/from memory: %" PRIu64 "\n", p_stats->bytes_transferred);
    printf("Hit Time: %f\n", p_stats->hit_time);
    printf("Miss Penalty: %f\n", p_stats->miss_penalty);
    printf("Miss rate: %f\n", p_stats->miss_rate);
    printf("Average access time (AAT): %f\n", p_stats->avg_access_time);
}
*/

#endif /* CACHESIM_H */
