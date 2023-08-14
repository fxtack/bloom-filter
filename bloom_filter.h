#include "hash.h"

#ifndef __BLOOM_FILTER_H__
#define __BLOOM_FILTER_H__

#ifdef BYTE_OFFSET_IN_BF
#undef BYTE_OFFSET_IN_BF
#else
#define BYTE_OFFSET_IN_BF(hash_value, bf_byte_size) ((hash_value)%(bf_byte_size))
#endif

#ifdef BIT_OFFSET_IN_BYTE
#undef BIT_OFFSET_IN_BYTE
#else
#define BIT_OFFSET_IN_BYTE(hash_value) ((hash_value)%(8))
#endif

// bloom filter typer
typedef enum {
    bf_mode_minimum = 0,
    bf_mode_bit_mark,
    bf_mode_counter,
    bf_mode_maximum = 2147483647
} bloom_filter_mode;

#define IS_VALID_BF_MODE(mode) (((mode) >= bf_mode_minimum) && ((mode) <= bf_mode_maximum)) 

// bloom filter
typedef struct {
    bloom_filter_mode mode;
    unsigned int      hash_times;
    hash_func_t*      hash_funcs;
    unsigned int      buf_bytes_size;
    byte*             buf;
} bloom_filter_t;

// New a bloom filter instance
int bloom_filter_init (
    bloom_filter_t*          bf,
    const bloom_filter_mode  mode,
    const size_t             bf_bs,
    const hash_func_t* const hash_funcs,
    const unsigned int       hash_funcs_amount
);

// Delete a bloom filter instance
int delete_bloom_filter(bloom_filter_t *bf);

// Add a entry to bloom filter
int bloom_filter_add(const bloom_filter_t* bf, const byte* buf, size_t buf_bs);

// Determine whether a value exists in the bloom filter
int bloom_filter_exist(const bloom_filter_t* bf, const byte* buf, size_t buf_bs);

// Reset bloom filter
int bloom_filter_reset(bloom_filter_t* bf);

typedef enum {
    bf_ok = 0,
    bf_error_invalid_mode = INT_MIN,
    bf_error_allocate_buf,
    bf_error_allocate_hash_funcs,
    bf_error_null_ptr,
    bf_error_counter_exceeded,
} bloom_filter_result;

#define BF_ERROR(error) ((error) < 0)

#endif