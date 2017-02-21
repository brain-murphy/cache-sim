#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <float.h>

#include "cachesim.h"
#include "cache.h"

static void parse_args(int argc, char *argv[]);
static int parse_input(struct access *acc);
static void validate_args(void);
static void print_statistics(struct cache_stats_t *p_stats);


FILE *input;
char* input_file_name;

int 
main(int argc, char *argv[]) 
{
    parse_args(argc, argv);

    validate_args();

    double min_aat = DBL_MAX;
    uint64_t min_c, min_b, min_s, min_v, min_k;

    uint64_t iterations = 0;

    for (uint32_t b = 3; b <= 7; b++) {
        for (uint32_t c = b; c <= 13; c++) {
            for (uint32_t s = 0; s <= c - b; s++) {
                for (uint32_t k = 1; k < b; k++) {
                    for (uint32_t v = 0; v <= 8; v++) {
                        iterations++;
                        if (iterations % 10 == 0) {
                            printf("....%"PRIu64"\n", iterations);
                        }

                        uint64_t cache_size = ((1u << b + 3) + 2 + (64 - (b + s)) + (1u << (b - k))) * (v + (1u << (C - B)));
                        if (cache_size < 524288) {

                            B = b;
                            C = c;
                            S = s;
                            K = k;
                            V = v;

                            init_cache();

                            input = fopen(input_file_name, "r");

                            struct access acc;
                            while (parse_input(&acc)) {
                                sim(acc);
                            }

                            struct cache_stats_t stats;
                            get_stats(&stats);

                            if (stats.avg_access_time < min_aat) {
                                min_aat = stats.avg_access_time;
                                min_b = b;
                                min_c = c;
                                min_k = k;
                                min_s = s;
                                min_v = v;

                                printf("new min aat: %f, (C=%"PRIu64", B=%"PRIu64", S=%"PRIu64", V=%"PRIu64", K=%"PRIu64"\n", min_aat, min_c, min_b, min_s, min_v, min_k);
                            }


                            fclose(input);

                        }
                    }
                }
            }
        }
    }


    printf("min aat: %f, (C=%"PRIu64", B=%"PRIu64", S=%"PRIu64", V=%"PRIu64", K=%"PRIu64"\n", min_aat, min_c, min_b, min_s, min_v, min_k);


    dealloc_cache();
}

static void 
parse_args(int argc, char *argv[])
{
    input = stdin; // default

    C = DEFAULT_C;
    B = DEFAULT_B;
    S = DEFAULT_S;
    V = DEFAULT_V;
    K = DEFAULT_K;

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
                    input_file_name = arg_value;
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
    unsigned char buffer[30];
    memset(buffer,0,30);

    unsigned char c = (unsigned char) fgetc(input);

    int i = 0;
    while (c != '\n' && (char)c != EOF) {
        buffer[i] = c;
        i++;
        c = (unsigned char) fgetc(input);
    }

    if ((char)c == EOF) {
        return 0;

    } else {
        acc->rw = buffer[0];
        acc->address = strtoull(&buffer[1], NULL, 16);

        if (acc->rw != READ && acc->rw != WRITE) {
            printf("stop");
        }

        return 1;
    }

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

