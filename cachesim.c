#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

#include "cachesim.h"
#include "cache.h"

static void parse_args(int argc, char *argv[]);
static int parse_input(struct access *acc);
static void validate_args(void);
static void print_statistics(struct cache_stats_t *p_stats);

uint64_t C = DEFAULT_C;
uint64_t B = DEFAULT_B;
uint64_t S = DEFAULT_S;
uint64_t V = DEFAULT_V;
uint64_t K = DEFAULT_K;

FILE *input;

int 
main(int argc, char *argv[]) 
{
    parse_args(argc, argv);

    validate_args();

    init_cache();

    struct access acc;
    while (parse_input(&acc)) {
        sim(acc);
    }

    dealloc_cache();
}

static void 
parse_args(int argc, char *argv[])
{
    input = stdin; // default

    int arg_index = 1;
    while (arg_index < argc - 1) {
        char *string = argv[arg_index];

        if (string[0] == '-') {
            char cache_param = string[1];
            char *arg_value = argv[arg_index + 1]; 

            switch (cache_param) {
                case 'C':
                C = atoi(arg_value);
                break;

                case 'B':
                B = atoi(arg_value);
                break;

                case 'S':
                S = atoi(arg_value);
                break;

                case 'V':
                V = atoi(arg_value);
                break;

                case 'K':
                K = atoi(arg_value);
                break;

                case 'i':
                    input = fopen(arg_value, "r");
                break;

                default:
                printf("err: could not recognize flag\n");
                break;
            }
            arg_index += 2;
        } else {
            printf("could not recognize argument format.\n Use flags -C -B -S -V -K -i\n");
            break;
        }
    }
}

/*
 * This function assumes we are at the start of a line.
 * Returns 0 on EOF
 */
static int
parse_input(struct access *acc) 
{
   char buffer[30];
   memset(buffer, 0, sizeof(buffer));
   
   int i = 0;
   char c = (char)fgetc(input);
   while (c != '\n' && c != EOF) {
       buffer[i] = c;
       c = (char)fgetc(input);
       i += 1;
   }

   acc->rw = buffer[0];
   acc->address = strtoull(&buffer[1], NULL, 16);

   return c != EOF;
}

static void
validate_args(void) {
    if (B < 3 || B > 7) {
        printf("B must be in the range [3, 7].\n");
    }

    if (C < B || C > 30) {
        printf("C must be in the range [B, 30].\n");
    }

    if (K >= B || K < 1) {
        printf("K must be in the range [1, B).\n");
    }

    if (S > C - B) {
        printf("S must be in the range [0, C - B].\n");
    }

    if (V > 8) {
        printf("V must be in the range [0, 8].\n");
    }
}

static void print_statistics(struct cache_stats_t* p_stats) {
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

