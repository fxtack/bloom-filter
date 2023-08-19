#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bloom_filter.h"

char* bf_result_msg(bf_result code) {
    switch (code) {
    case bf_ok:
        return "ok";
    case bf_error_invalid_mode:
        return "invalid bloom filter mode";
    case bf_error_allocate_buf:
        return "allocate bloom filter data buffer failed";
    case bf_error_allocate_hash_funcs:
        return "allocate bloom filter hash functions failed";
    case bf_error_null_ptr:
        return "null pointer bloom filter";
    case bf_error_counter_exceeded:
        return "bloom filter counter have exceeded";
    case bf_error_invalid_file_path:
        return "invalid file path";
    case bf_error_open_file_failed:
        return "open file failed";
    case bf_error_dump_file_failed:
        return "dump bloom filter failed";
    case bf_error_restore_failed:
        return "restore bloom filter failed";
    default:
        return "unknown result";
    }
}

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
        return bf_error_invalid_mode;
    }

    // Allocate bloom filter buffer memory
    bf->buf = (byte*)malloc(bf_bs);
    if (bf->buf != NULL) {
        memset(bf->buf, 0, bf_bs);
        bf->buf_bytes_size = bf_bs;
    } else {
        return bf_error_allocate_buf;
    }
    
    // Copy hash functions
    hash_funcs_bytes_size = sizeof(hash_func_t) * hash_times;
    bf->hash_funcs = (hash_func_t*)malloc(hash_funcs_bytes_size);
    if (bf->hash_funcs != NULL) {
        memcpy(bf->hash_funcs, hash_funcs, hash_funcs_bytes_size);
        bf->hash_times = hash_times;
    } else {
        return bf_error_allocate_hash_funcs;
    }

    return bf_ok;
}

// Delete a bloom filter instance
int delete_bloom_filter(bloom_filter_t *bf) {
    if (bf->buf == NULL) {
        return bf_error_null_ptr;
    }

    // Free bloom filter buffer memory
    free(bf->buf);
    bf->buf = NULL;

    // Free hash functions
    free(bf->hash_funcs);
    bf->hash_funcs = NULL;
    
    return bf_ok;
}

// Add a entry to bloom filter
bf_result bloom_filter_add(const bloom_filter_t* bf, const byte* buf, size_t buf_bs) {
    unsigned int i = 0;
    unsigned int hash_value  = 0;
    unsigned int byte_offset = 0;
    unsigned int bit_offset  = 0;

    if (bf == NULL) {
        return bf_error_null_ptr;
    }

    switch (bf->mode) {
        case bf_mode_bit_mark:
            for (; i < bf->hash_times; i++) {
                hash_value  = ((hash_func_t)(bf->hash_funcs[i]))(buf, buf_bs);
                byte_offset = BYTE_OFFSET_IN_BF(hash_value / 8, bf->buf_bytes_size);
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
                    return bf_error_counter_exceeded;
                }
                
                // Byte counter increment
                bf->buf[byte_offset]++;
            }
            break;

        default:
            // Unknown bloom filter mode
            return bf_error_invalid_mode;
    }

    return bf_ok;
}

// Determine whether a value exists in the bloom filter
bf_result bloom_filter_exist(const bloom_filter_t* bf, const byte* buf, size_t buf_bs) {
    unsigned int i = 0;
    unsigned int hash_value  = 0;
    unsigned int byte_offset = 0;
    unsigned int bit_offset  = 0;
    int result = 1;

    if (bf == NULL) {
        return bf_error_null_ptr;
    }

    switch (bf->mode) {
        case bf_mode_bit_mark:
            for (; i < bf->hash_times; i++) {
                hash_value  = ((hash_func_t)(bf->hash_funcs[i]))(buf, buf_bs);
                byte_offset = BYTE_OFFSET_IN_BF(hash_value / 8, bf->buf_bytes_size);
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
            return bf_error_invalid_mode;
    }

    return result;
}

// Reset bloom filter
bf_result bloom_filter_reset(bloom_filter_t* bf) {
    if (bf->buf == NULL) {
        return bf_error_null_ptr;
    }

    memset(bf->buf, 0, bf->buf_bytes_size);

    return bf_ok;
}

// Dump bloom filter from file
bf_result bloom_filter_dump(
    const bloom_filter_t* bf,
    const char* const     file_path
) {
    FILE* file;
    byte* write_buf;
    size_t write_buf_bs = 0;
    size_t bf_meta_bs = BF_META_BYTES_SIZE;
    size_t written_bs = 0;

    // Invalid input parameter
    if (bf == NULL) return bf_error_null_ptr;
    if (file_path == NULL) return bf_error_invalid_file_path;

    // Open file
    file = fopen(file_path, "wb");
    if (file == NULL) {
        return bf_error_open_file_failed;
    }

    // Allocate write buffer
    write_buf_bs = bf_meta_bs + bf->buf_bytes_size;
    write_buf = malloc(write_buf_bs);

    // Copy bloom filter metadata to write buffer
    memcpy(write_buf, bf, bf_meta_bs);

    // Copy bloom filter data to write buffer
    write_buf += bf_meta_bs;
    memcpy(write_buf, bf->buf, bf->buf_bytes_size);
    write_buf -= bf_meta_bs;

    // Write bloom filter data
    written_bs = fwrite(write_buf, sizeof(byte), write_buf_bs, file);
    if (written_bs != write_buf_bs) {

        // Dump bloom filter failed, roll back
        fclose(file);
        remove(file_path);
        return bf_error_dump_file_failed;
    }

    fflush(file);

    fclose(file);

    return bf_ok;
}
