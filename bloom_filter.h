#include "hash.h"

#ifndef __BLOOM_FILTER_H__
#define __BLOOM_FILTER_H__

#ifdef BYTE_OFFSET_IN_BF
#undef BYTE_OFFSET_IN_BF
#else
#define BYTE_OFFSET_IN_BF(hash_value, bf_bs) ((hash_value)%(bf_bs))
#endif

#ifdef BIT_OFFSET_IN_BYTE
#undef BIT_OFFSET_IN_BYTE
#else
#define BIT_OFFSET_IN_BYTE(hash_value) ((hash_value)%(8))
#endif

// bloom filter typer
typedef enum {
    BF_MODE_UNKNOWN = 0,
    BF_MODE_BIT_MARK,
    BF_MODE_COUNTER,
    BF_MODE_MAXIMUM = INT_MAX
} bloom_filter_mode;

#define IS_VALID_BF_MODE(mode) (((mode) >= BF_MODE_UNKNOWN) && ((mode) <= BF_MODE_MAXIMUM)) 

// bloom filter
typedef struct {
    bloom_filter_mode mode;
    unsigned int      hash_times;
    hash_func_t*      hash_funcs;
    unsigned int      buf_bs;
    byte*             buf;
} bloom_filter_t;

#define BF_META_BYTES_SIZE (sizeof(bloom_filter_t) - sizeof(byte*))

// New a bloom filter instance
int bloom_filter_init (
    bloom_filter_t*          bf,
    const bloom_filter_mode  mode,
    const size_t             bf_bs,
    const hash_func_t* const hash_funcs,
    const unsigned int       hash_funcs_amount
);

typedef enum {
    BF_RESULT_OK = 0,
    BF_ENTRY_EXISTS,
    BF_ENTRY_NOT_EXISTS,
    BF_ERROR_INVALID_MODE = INT_MIN,
    BF_ERROR_ALLOCATE_BUF,
    BF_ERROR_ALLOCATE_HASH_FUNCS,
    BF_ERROR_INVALID_BUF_BS,
    BF_ERROR_NULL_PTR,
    BF_ERROR_NULL_BUF,
    BF_ERROR_COUNTER_EXCEEDED,
    BF_ERROR_INVALID_FILE_PATH,
    BF_ERROR_OPEN_FILE_FAILED,
    BF_ERROR_DUMP_BUF_FILE,
    BF_ERROR_RESTORE_BUF_FILE
} bf_result;

#define BF_ERROR(result) ((result) < 0)
#define BF_SUCCESS(result) ((result) == BF_RESULT_OK)

char* bf_result_msg(bf_result code);

// Delete a bloom filter instance
bf_result delete_bloom_filter(bloom_filter_t *bf);

// Add a entry to bloom filter
bf_result bloom_filter_add(
    const bloom_filter_t* bf,
    const byte*           entry,
    size_t                entry_bs
);

// Determine whether a value exists in the bloom filter
bf_result bloom_filter_exist(
    const bloom_filter_t* bf,
    const byte*           entry,
    size_t                entry_bs
);

// Reset bloom filter
bf_result bloom_filter_reset(bloom_filter_t* bf);

// Only dump bloom filter buffer to file
// This is typically used to observe changes in bloom filter data
bf_result bloom_filter_dump_buf(
    const bloom_filter_t* bf,
    const char* const     file_path
);

// Restore bloom filter buffer from file
bf_result bloom_filter_restore_buf(
    const bloom_filter_t* bf,
    const char* const     file_path
);

#endif