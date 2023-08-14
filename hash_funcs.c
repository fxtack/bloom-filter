#include "hash_funcs.h"

unsigned int RSHash(byte* buf, size_t len) {
   unsigned int b    = 378551;   
   unsigned int a    = 63689;   
   unsigned int hash = 0;   
   unsigned int i    = 0;   
   for(i = 0; i < len; buf++, i++) {   
      hash = hash * a + (*buf);   
      a = a * b;
   }   
   return hash;
}

unsigned int JSHash(byte* buf, size_t len) {
   unsigned int hash = 1315423911;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash ^= ((hash << 5) + (*buf) + (hash >> 2));
   }
   return hash;
}

unsigned int ELFHash(byte* buf, size_t len) {
   unsigned int hash = 0;
   unsigned int x    = 0;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash = (hash << 4) + (*buf);  
      if((x = hash & 0xF0000000L) != 0) {
         hash ^= (x >> 24);   
      }
      hash &= ~x;   
   }   
   return hash;
}
  
unsigned int BKDRHash(byte* buf, size_t len) {   
   unsigned int seed = 131;
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash = (hash * seed) + (*buf);
   }
   return hash;
}
  
unsigned int SDBMHash(byte* buf, size_t len) {
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash = (*buf) + (hash << 6) + (hash << 16) - hash;
   }
   return hash;
}
  
unsigned int DJBHash(byte* buf, size_t len) {   
   unsigned int hash = 5381;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash = ((hash << 5) + hash) + (*buf);
   }
   return hash;
}

unsigned int DEKHash(byte* buf, size_t len) {
   unsigned int hash = len;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash = ((hash << 5) ^ (hash >> 27)) ^ (*buf);
   }
   return hash;
}

unsigned int BPHash(byte* buf, size_t len) {   
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash = hash << 7 ^ (*buf);
   }
   return hash;
}
  
unsigned int FNVHash(byte* buf, size_t len) {
   const unsigned int fnv_prime = 0x811C9DC5;
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash *= fnv_prime;
      hash ^= (*buf);
   }
   return hash;
}

unsigned int APHash(byte* buf, size_t len) {
   unsigned int hash = 0xAAAAAAAA;
   unsigned int i    = 0;
   for(i = 0; i < len; buf++, i++) {
      hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*buf) * (hash >> 3)) :
                               (~((hash << 11) + (*buf) ^ (hash >> 5)));
   }
   return hash;
}

hash_func get_func(hash_class hc) {
   switch (hc) {
      case hash_rs:
         return RSHash;
      case hash_js:
         return JSHash;
      case hash_elf:
         return ELFHash;
      case hash_bkdr:
         return BKDRHash;
      case hash_sdbm:
         return SDBMHash;
      case hash_djb:
         return DJBHash;
      case hash_dek:
         return DEKHash;
      case hash_bp:
         return BPHash;
      case hash_fnv:
         return FNVHash;
      case hash_ap:
         return APHash;
      default:
         return NULL;
   }
}