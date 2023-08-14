#ifndef __HASH_H__
#define __HASH_H__

#include <stddef.h>
#include <limits.h>

typedef unsigned char byte;

#define BYTE_MAX UCHAR_MAX

unsigned int RSHash(const byte* buf, size_t buf_bs);
unsigned int JSHash(const byte* buf, size_t buf_bs);
unsigned int ELFHash(const byte* buf, size_t buf_bs);
unsigned int BKDRHash(const byte* buf, size_t buf_bs);
unsigned int SDBMHash(const byte* buf, size_t buf_bs);
unsigned int DJBHash(const byte* buf, size_t buf_bs);
unsigned int DEKHash(const byte* buf, size_t buf_bs);
unsigned int BPHash(const byte* buf, size_t buf_bs);
unsigned int FNVHash(const byte* buf, size_t buf_bs);
unsigned int APHash(const byte* buf, size_t buf_bs);

// Hash function type
typedef unsigned int (*hash_func_t)(const byte* buf, size_t buf_bs);

#endif