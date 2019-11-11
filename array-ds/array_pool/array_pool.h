#ifndef ARRAY_POOL_H
#define ARRAY_POOL_H

#include "../c_vector/c_vector.h"

typedef unsigned char byte;

// Do not use directly
typedef struct array_pool {
    c_vector the_pool;
    int node_cap;
} array_pool;

#define APL_DEF_SIZE 1024
#define APL_DEF_ALIGN sizeof(void*)

void * apl_make_size(array_pool * apl, int segment_size_bytes);
#define apl_make(papl) apl_make_size(papl, APL_DEF_SIZE)

void apl_destroy(array_pool * apl);

void * apl_reuse(array_pool * apl);

void * apl_reset(array_pool * apl);

byte * apl_get_align(array_pool * apl, int bytes, int alignment);
#define apl_get(papl, bytes) apl_get_align((papl), (bytes), APL_DEF_ALIGN)

byte * apl_insert(array_pool * apl,
    const void * arr, int arr_len, int elem_size
);

byte * apl_insert_string(array_pool * apl, const char * str, int str_len);

#endif
