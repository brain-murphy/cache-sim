#include <stdio.h>
#include <stdlib.h>

#include "cachesim.h"

static void parse_args(int argc, char *argv[]);

uint64_t C = DEFAULT_C;
uint64_t B = DEFAULT_B;
uint64_t S = DEFAULT_S;
uint64_t V = DEFAULT_V;
uint64_t K = DEFAULT_K;

FILE *input;

static void print_args() {
    printf("C: %u, B: %u, S: %u, V: %u, K: %u, input: %p\n", C, B, S, V, K, input);
}

int 
main(int argc, char *argv[]) 
{
    parse_args(argc, argv);
    print_args();
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

