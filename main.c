#include <stdio.h>
#include <string.h>

#include "bloom_filter.h"

int main(int argc, char *argv[]) {
    const unsigned int HASH_TIMES = 3;
    bf_result ret = 0;
    bloom_filter_t bf = { 0 };
    hash_func_t hash_funcs[HASH_TIMES] = {RSHash, JSHash, ELFHash};

    // Test bloom filter
    const unsigned int test_cases = 3;
    unsigned int i = 0;
    char str_1[] = "hello star";
    char str_2[] = "hello moon";
    char str_3[] = "hello star";
    char *str = NULL;
    char *strings[test_cases] = { str_1, str_2, str_3 };

    // Bloom filter initialize
    ret = bloom_filter_init(&bf, bf_mode_bit_mark, (1<<10), hash_funcs, HASH_TIMES);
    if (BF_ERROR(ret)) {
        fprintf(stderr, "Error(%d): %s.\n", ret, bf_result_msg(ret));
        return ret;
    }

    // Add string to bloom filter
    ret = bloom_filter_add(&bf, (byte*)str_1, strlen(str_1));
    if (BF_ERROR(ret)) {
        fprintf(stderr, "Error(%d): %s, param: %s.\n", ret, bf_result_msg(ret), str_1);
        return ret;
    }

    // Check if the strings are in the bloom filter
    for (; i < test_cases; i++) {
        str = strings[i];
        ret = bloom_filter_exist(&bf, (byte*)str, strlen(str));
        if (BF_ERROR(ret)) {
            fprintf(stderr, "Error(%d): %s, param: %s.\n", ret, bf_result_msg(ret), str);
        } else if (ret) {
            printf("String `%s` already exists.\n", str);
        } else {
            printf("String `%s` dose not exist.\n", str);
        }
    }

    ret = bloom_filter_dump(&bf, "my_bloom_filter.bf");
    if (BF_ERROR(ret)) {
        fprintf(stderr, "Error(%d): call bloom_filter_dump failed.\n", ret);
        return ret;
    }

    return 0;
}