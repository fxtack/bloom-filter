#ifndef __HASH_FUNCS__
#define __HASH_FUNCS__

#include <stddef.h>
#include <limits.h>

typedef unsigned char byte;

#define BYTE_MAX UCHAR_MAX

unsigned int RSHash(byte* byte, size_t len);
unsigned int JSHash(byte* byte, size_t len);
unsigned int ELFHash(byte* byte, size_t len);
unsigned int BKDRHash(byte* byte, size_t len);
unsigned int SDBMHash(byte* byte, size_t len);
unsigned int DJBHash(byte* byte, size_t len);
unsigned int DEKHash(byte* byte, size_t len);
unsigned int BPHash(byte* byte, size_t len);
unsigned int FNVHash(byte* byte, size_t len);
unsigned int APHash(byte* byte, size_t len);

typedef enum {
    hash_rs,
    hash_js,
    hash_elf,
    hash_bkdr,
    hash_sdbm,
    hash_djb,
    hash_dek,
    hash_bp,
    hash_fnv,
    hash_ap
} hash_class;

// Hash function type
typedef unsigned int (*hash_func)(byte* entry, size_t byte_size);

hash_func get_func(hash_class hc);

#endif