#include <stdio.h>
#include <string.h>

#include "bloom_filter.h"

int main(int argc, char *argv[]) {
    
    const unsigned int HASH_TIMES = 3;
    bf_result ret = 0;
    bloom_filter_t bf = { 0 };
    hash_func_t hash_funcs[HASH_TIMES] = {RSHash, JSHash, DEKHash};

    // Test bloom filter
    unsigned int i;
    char *str = NULL;
    char *add_strings[]  = { 
        "hello",
        "world",
        "earth",
        "moon"
    };
    char *test_strings[] = { 
        "hello", 
        "HELLO",
        "moon"
    };

    // Bloom filter initialize
    ret = bloom_filter_init(&bf, bf_mode_bit_mark, 128, hash_funcs, HASH_TIMES);
    if (BF_ERROR(ret)) {
        fprintf(stderr, "Error(%d): %s.\n", ret, bf_result_msg(ret));
        return ret;
    }

    // Add strings to bloom filter
    for (i = 0; i < (sizeof(add_strings) / sizeof(char*)); i++) {
        str = add_strings[i];
        ret = bloom_filter_add(&bf, (byte*)str, strlen(str));
        if (BF_ERROR(ret)) {
            fprintf(stderr, "Error(%d): %s, param: %s.\n", ret, bf_result_msg(ret), str);
            return ret;
        } else {
            printf("Add string to bloom filter: \"%s\".\n", str);
        }
    }
    
    // Check if the strings are in the bloom filter
    for (i = 0; i < (sizeof(test_strings) / sizeof(char*)); i++) {
        str = test_strings[i];
        ret = bloom_filter_exist(&bf, (byte*)str, strlen(str));
        if (BF_ERROR(ret)) {
            fprintf(stderr, "Error(%d): %s, param: %s.\n", ret, bf_result_msg(ret), str);
        } else if (ret) {
            printf("String `%s` already exists.\n", str);
        } else {
            printf("String `%s` dose not exist.\n", str);
        }
    }
    
    ret = bloom_filter_dump_buf(&bf, "my_bloom_filter_buf.bin");
    if (BF_ERROR(ret)) {
        fprintf(stderr, "Error(%d): call bloom_filter_dump_buf: %s.\n", ret, bf_result_msg(ret));
        return ret;
    }

    return 0;
}