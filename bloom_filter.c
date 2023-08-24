#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bloom_filter.h"

char* bf_result_msg(bf_result code) {
    switch (code) {
    case BF_RESULT_OK:
        return "ok";
    case BF_ERROR_INVALID_MODE:
        return "invalid bloom filter mode";
    case BF_ERROR_ALLOCATE_BUF:
        return "allocate bloom filter data buffer failed";
    case BF_ERROR_ALLOCATE_HASH_FUNCS:
        return "allocate bloom filter hash functions failed";
    case BF_ERROR_NULL_PTR:
        return "null pointer bloom filter";
    case BF_ERROR_COUNTER_EXCEEDED:
        return "bloom filter counter have exceeded";
    case BF_ERROR_INVALID_FILE_PATH:
        return "invalid file path";
    case BF_ERROR_OPEN_FILE_FAILED:
        return "open file failed";
    case BF_ERROR_DUMP_BUF_FILE:
        return "dump bloom filter buffer failed";
    case BF_ERROR_RESTORE_BUF_FILE:
        return "restore bloom filter buffer failed";
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
    size_t hash_funcs_bs = 0u;

    // Invalid bloom filter mode
    if (IS_VALID_BF_MODE(mode)) {
        bf->mode = mode;
    } else {
        return BF_ERROR_INVALID_MODE;
    }

    // Allocate bloom filter buffer memory
    bf->buf = (byte*)malloc(bf_bs);
    if (bf->buf != NULL) {
        memset(bf->buf, 0, bf_bs);
        bf->buf_bs = bf_bs;
    } else {
        return BF_ERROR_ALLOCATE_BUF;
    }
    
    // Copy hash functions
    hash_funcs_bs = sizeof(hash_func_t) * hash_times;
    bf->hash_funcs = (hash_func_t*)malloc(hash_funcs_bs);
    if (bf->hash_funcs != NULL) {
        memcpy(bf->hash_funcs, hash_funcs, hash_funcs_bs);
        bf->hash_times = hash_times;
    } else {
        return BF_ERROR_ALLOCATE_HASH_FUNCS;
    }

    return BF_RESULT_OK;
}

// Delete a bloom filter instance
int delete_bloom_filter(bloom_filter_t *bf) {
    if (bf->buf == NULL) {
        return BF_ERROR_NULL_PTR;
    }

    // Free bloom filter buffer memory
    free(bf->buf);
    bf->buf = NULL;

    // Free hash functions
    free(bf->hash_funcs);
    bf->hash_funcs = NULL;
    
    return BF_RESULT_OK;
}

// Add a entry to bloom filter
bf_result bloom_filter_add(const bloom_filter_t* bf, const byte* entry, size_t entry_bs) {
    unsigned int i = 0;
    unsigned int hash_value  = 0;
    unsigned int byte_offset = 0;
    unsigned int bit_offset  = 0;

    if (bf == NULL) {
        return BF_ERROR_NULL_PTR;
    }

    switch (bf->mode) {
        case BF_MODE_BIT_MARK:
            for (; i < bf->hash_times; i++) {
                hash_value  = ((hash_func_t)(bf->hash_funcs[i]))(entry, entry_bs);
                byte_offset = BYTE_OFFSET_IN_BF(hash_value / 8, bf->buf_bs);
                bit_offset  = BIT_OFFSET_IN_BYTE(hash_value);

                // Set bit to 1
                bf->buf[byte_offset] |= ((byte)(1 << bit_offset));
            }
            break;

        case BF_MODE_COUNTER:
            for (; i < bf->hash_times; i++) {
                byte_offset = BYTE_OFFSET_IN_BF(hash_value, bf->buf_bs);

                if (bf->buf[byte_offset] == BYTE_MAX) {
                    // Exceeded the maximum value of byte
                    return BF_ERROR_COUNTER_EXCEEDED;
                }
                
                // Byte counter increment
                bf->buf[byte_offset]++;
            }
            break;

        default:
            // Unknown bloom filter mode
            return BF_ERROR_INVALID_MODE;
    }

    return BF_RESULT_OK;
}

// Determine whether a value exists in the bloom filter
bf_result bloom_filter_exist(const bloom_filter_t* bf, const byte* entry, size_t entry_bs) {
    unsigned int i = 0;
    unsigned int hash_value  = 0;
    unsigned int byte_offset = 0;
    unsigned int bit_offset  = 0;

    if (bf == NULL) {
        return BF_ERROR_NULL_PTR;
    }

    switch (bf->mode) {
        case BF_MODE_BIT_MARK:
            for (; i < bf->hash_times; i++) {
                hash_value  = ((hash_func_t)(bf->hash_funcs[i]))(entry, entry_bs);
                byte_offset = BYTE_OFFSET_IN_BF(hash_value / 8, bf->buf_bs);
                bit_offset  = BIT_OFFSET_IN_BYTE(hash_value);

                if (!(bf->buf[byte_offset] & ((byte)(1 << bit_offset))))
                    return BF_ENTRY_NOT_EXISTS;
            }
            break;

        case BF_MODE_COUNTER:
            for (; i < bf->hash_times; i++)
                if (bf->buf[BYTE_OFFSET_IN_BF(hash_value, bf->buf_bs)])
                    return BF_ENTRY_NOT_EXISTS;
            break;

        default:
            // unknown bloom filter mode
            return BF_ERROR_INVALID_MODE;
    }

    return BF_ENTRY_EXISTS;
}

// Reset bloom filter
bf_result bloom_filter_reset(bloom_filter_t* bf) {
    if (bf->buf == NULL) {
        return BF_ERROR_NULL_PTR;
    }

    memset(bf->buf, 0, bf->buf_bs);

    return BF_RESULT_OK;
}

bf_result bloom_filter_dump_buf(
    const bloom_filter_t* bf,
    const char* const     file_path
) {
    FILE* file;
    size_t written_bs = 0;

    if (bf == NULL) return BF_ERROR_NULL_PTR;
    if (file_path == NULL) return BF_ERROR_INVALID_FILE_PATH;

    file = fopen(file_path, "wb");
    if (file == NULL) {
        return BF_ERROR_OPEN_FILE_FAILED;
    }

    written_bs = fwrite(bf->buf, sizeof(byte), bf->buf_bs, file);
    if (written_bs != bf->buf_bs) {
        fclose(file);
        remove(file_path);
        return BF_ERROR_DUMP_BUF_FILE;
    }

    fflush(file);
    fclose(file);

    return BF_RESULT_OK;
}

bf_result bloom_filter_restore_buf(
    const bloom_filter_t* bf,
    const char* const     file_path
) {
    FILE* file;
    size_t read_bs = 0;

    if (bf == NULL) return BF_ERROR_NULL_PTR;
    if (bf->buf == NULL) return BF_ERROR_NULL_BUF;
    if (bf->buf_bs == 0) return BF_ERROR_INVALID_BUF_BS;
    if (file_path == NULL) return BF_ERROR_INVALID_FILE_PATH;

    file = fopen(file_path, "rb");
    if (file == NULL) {
        return BF_ERROR_OPEN_FILE_FAILED;
    }

    read_bs = fread(bf->buf, sizeof(byte), bf->buf_bs, file);
    if (read_bs != bf->buf_bs) {
        fclose(file);
        return BF_ERROR_RESTORE_BUF_FILE;
    }

    fclose(file);

    return BF_RESULT_OK;
}