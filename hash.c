#include "hash.h"

unsigned int RSHash(const byte* buf, size_t buf_bs) {
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash = hash * a + (*buf);
      a = a * b;
   }
   return hash;
}

unsigned int JSHash(const byte* buf, size_t buf_bs) {
   unsigned int hash = 1315423911;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash ^= ((hash << 5) + (*buf) + (hash >> 2));
   }
   return hash;
}

unsigned int ELFHash(const byte* buf, size_t buf_bs) {
   unsigned int hash = 0;
   unsigned int x    = 0;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash = (hash << 4) + (*buf);  
      if((x = hash & 0xF0000000L) != 0) {
         hash ^= (x >> 24);   
      }
      hash &= ~x;   
   }   
   return hash;
}
  
unsigned int BKDRHash(const byte* buf, size_t buf_bs) {   
   unsigned int seed = 131;
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash = (hash * seed) + (*buf);
   }
   return hash;
}
  
unsigned int SDBMHash(const byte* buf, size_t buf_bs) {
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash = (*buf) + (hash << 6) + (hash << 16) - hash;
   }
   return hash;
}
  
unsigned int DJBHash(const byte* buf, size_t buf_bs) {   
   unsigned int hash = 5381;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash = ((hash << 5) + hash) + (*buf);
   }
   return hash;
}

unsigned int DEKHash(const byte* buf, size_t buf_bs) {
   unsigned int hash = buf_bs;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash = ((hash << 5) ^ (hash >> 27)) ^ (*buf);
   }
   return hash;
}

unsigned int BPHash(const byte* buf, size_t buf_bs) {   
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash = hash << 7 ^ (*buf);
   }
   return hash;
}
  
unsigned int FNVHash(const byte* buf, size_t buf_bs) {
   const unsigned int fnv_prime = 0x811C9DC5;
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash *= fnv_prime;
      hash ^= (*buf);
   }
   return hash;
}

unsigned int APHash(const byte* buf, size_t buf_bs) {
   unsigned int hash = 0xAAAAAAAA;
   unsigned int i    = 0;
   for(i = 0; i < buf_bs; buf++, i++) {
      hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*buf) * (hash >> 3)) :
                               (~((hash << 11) + (*buf) ^ (hash >> 5)));
   }
   return hash;
}
