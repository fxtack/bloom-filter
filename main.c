#include <stdio.h>

#include "bloom_filter.h"

int main(int argc, char *argv[]) {
    bloom_filter_t bf;
    hash_func_t hash_funcs[3] = {RSHash, JSHash, ELFHash};
    int ret = 0;

    ret = init_bloom_filter(&bf, bf_mode_bit_mark, (1<<10), hash_funcs, 3);
    if (ret == -1) {
        printf("Error: bloom filter initialization failed.\n");
    }
}