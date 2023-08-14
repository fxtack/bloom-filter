#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bloom_filter.h"

// New a bloom filter instance
int bloom_filter_init(
    bloom_filter_t*          bf,
    const bloom_filter_mode  mode,
    const size_t             bf_bs,
    const hash_func_t* const hash_funcs,
    const unsigned int       hash_times
) {
    size_t hash_funcs_bytes_size = 0u;

    // Invalid bloom filter mode
    if (IS_VALID_BF_MODE(mode)) {
        bf->mode = mode;
    } else {
        return -1;
    }

    // Allocate bloom filter buffer memory
    bf->buf = (byte*)malloc(bf_bs);
    if (bf->buf != NULL) {
        memset(bf->buf, 0, bf_bs);
        bf->buf_bytes_size = bf_bs;
    } else {
        return -1;
    }
    
    // Copy hash functions
    hash_funcs_bytes_size = sizeof(hash_func_t) * hash_times;
    bf->hash_funcs = (hash_func_t*)malloc(hash_funcs_bytes_size);
    if (bf->hash_funcs != NULL) {
        memcpy(bf->hash_funcs, hash_funcs, hash_funcs_bytes_size);
        bf->hash_times = hash_times;
    } else {
        return -1;
    }

    return 0;
}

// Delete a bloom filter instance
int delete_bloom_filter(bloom_filter_t *bf) {
    if (bf->buf == NULL) {
        return -1;
    }

    // Free bloom filter buffer memory
    free(bf->buf);
    bf->buf = NULL;

    // Free hash functions
    free(bf->hash_funcs);
    bf->hash_funcs = NULL;
    
    return 0;
}

// Add a entry to bloom filter
int bloom_filter_add(const bloom_filter_t* bf, const byte* buf, size_t buf_bs) {
    unsigned int i = 0;
    unsigned int hash_value  = 0;
    unsigned int byte_offset = 0;
    unsigned int bit_offset  = 0;

    if (bf == NULL) {
        return -1;
    }

    switch (bf->mode) {
        case bf_mode_bit_mark:
            for (; i < bf->hash_times; i++) {
                hash_value  = ((hash_func_t)(bf->hash_funcs[i]))(buf, buf_bs);
                byte_offset = BYTE_OFFSET_IN_BF(hash_value, bf->buf_bytes_size);
                bit_offset  = BIT_OFFSET_IN_BYTE(hash_value);

                // Set bit to 1
                bf->buf[byte_offset] |= ((byte)(1 << bit_offset));
            }
            break;

        case bf_mode_counter:
            for (; i < bf->hash_times; i++) {
                byte_offset = BYTE_OFFSET_IN_BF(hash_value, bf->buf_bytes_size);

                if (bf->buf[byte_offset] == BYTE_MAX) {
                    // Exceeded the maximum value of byte
                    return -1;
                }
                
                // Byte counter increment
                bf->buf[byte_offset]++;
            }
            break;

        default:
            // Unknown bloom filter mode
            return -1;
    }

    return 0;
}

// Determine whether a value exists in the bloom filter
int bloom_filter_exist(const bloom_filter_t* bf, const byte* buf, size_t buf_bs) {
    unsigned int i = 0;
    unsigned int hash_value  = 0;
    unsigned int byte_offset = 0;
    unsigned int bit_offset  = 0;
    int result = 1;

    if (bf == NULL) {
        return -1;
    }

    switch (bf->mode) {
        case bf_mode_bit_mark:
            for (; i < bf->hash_times; i++) {
                hash_value  = ((hash_func_t)(bf->hash_funcs[i]))(buf, buf_bs);
                byte_offset = BYTE_OFFSET_IN_BF(hash_value, bf->buf_bytes_size);
                bit_offset  = BIT_OFFSET_IN_BYTE(hash_value);

                if (!(bf->buf[byte_offset] & ((byte)(1 << bit_offset))))
                    return 0;
            }
            break;

        case bf_mode_counter:
            for (; i < bf->hash_times; i++)
                if (bf->buf[BYTE_OFFSET_IN_BF(hash_value, bf->buf_bytes_size)])
                    return 0;
            break;

        default:
            // unknown bloom filter mode
            return -1;
    }

    return result;
}

// Reset bloom filter
int bloom_filter_reset(bloom_filter_t* bf) {
    if (bf->buf == NULL) {
        return -1;
    }

    memset(bf->buf, 0, bf->buf_bytes_size);

    return 0;
}