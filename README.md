# bloom-filter
The simple bloom filter implemented by C.

In the following example, the buffer of the Bloom filter will be saved to a file for the purpose of observing the changes in the bit positions.
```c
#include <stdio.h>
#include <string.h>

#include "bloom_filter.h"

int main(int argc, char *argv[]) {
    
    const unsigned int HASH_TIMES = 3;
    const unsigned int BUF_BS = 128;
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

    // Bloom filter initialize
    ret = bloom_filter_init(&bf, BF_MODE_BIT_MARK, BUF_BS, hash_funcs, HASH_TIMES);
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
            return -1;
        } else {
            printf("Add string to bloom filter: \"%s\".\n", str);
        }
    }

    // Dump bloom filter buffer to file
    ret = bloom_filter_dump_buf(&bf, "my_bloom_filter_buf.bin");
    if (BF_ERROR(ret)) {
        fprintf(stderr, "Error(%d): call bloom_filter_dump_buf: %s.\n", ret, bf_result_msg(ret));
        return -1;
    }

    return 0;
}
```
You can use the xxd command to observe the data bits of the saved file.

```shell
$ xxd -bits my_bloom_filter_buf.bin
00000012: 00000000 00000000 00000000 00000000 00000000 00000000  ......
00000018: 00000000 00000000 00000000 00000000 00000000 00000000  ......
0000001e: 00000000 00000000 00000000 00000000 00000000 00000000  ......
00000024: 00000000 00000000 00000000 00100100 00000000 00000000  ...$..
00000030: 00000000 01000000 00000000 00000000 00000000 00000010  .@....
0000003c: 01010000 10100000 00000000 00000000 00000000 01000000  P....@
0000004e: 00000000 00000000 00000000 00000000 00000100 00000000  ......
0000005a: 00000000 00000000 00000000 00000001 00000000 00000000  ......
00000078: 00000000 00001000 00000000 00000000 00000000 00000000  ......
```

## TODO

- Bloom filter dump/restore.
- More unit tests.
- Benchmark test.