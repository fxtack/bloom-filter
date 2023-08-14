#include <stdio.h>
#include <string.h>

#include "bloom_filter.h"

int main(int argc, char *argv[]) {
    int ret = 0;
    bloom_filter_t bf;
    hash_func_t hash_funcs[3] = {RSHash, JSHash, ELFHash};
    char str1[] = "hello world";
    char str2[] = "hello World";

    ret = bloom_filter_init(&bf, bf_mode_bit_mark, (1<<10), hash_funcs, 3);
    if (ret == -1) {
        fprintf(stderr, "Error: bloom filter initialization failed.\n");
        return ret;
    }

    ret = bloom_filter_add(&bf, (byte*)str1, strlen(str1));
    if (ret == -1) {
        fprintf(stderr, "Error: add entry to bloom filter failed\n");
        return ret;
    }

    ret = bloom_filter_exist(&bf, (byte*)str1, strlen(str1));
    switch (ret) {
        case -1:
            fprintf(stderr, "Error: bloom filter exists failed.\n");
            break;
        case 0:
            printf("Not Exist\n");
            break;
        case 1:
            printf("Exist\n");
            break;
        default:
            fprintf(stderr, "Error: unknown bloom filter existence result.\n");
    }

    ret = bloom_filter_exist(&bf, (byte*)str2, strlen(str2));
    switch (ret) {
        case -1:
            fprintf(stderr, "Error: bloom filter exists failed.\n");
            break;
        case 0:
            printf("Not Exist\n");
            break;
        case 1:
            printf("Exist\n");
            break;
        default:
            fprintf(stderr, "Error: unknown bloom filter existence result.\n");
    }

}