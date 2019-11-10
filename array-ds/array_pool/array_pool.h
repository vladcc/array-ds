#ifndef ARRAY_POOL_H
#define ARRAY_POOL_H

#include "../c_vector/c_vector.h"

// Do not use directly
typedef struct array_pool {
    c_vector the_pool;
    int elem_size;
    int node_cap;
} array_pool;

#define APL_DEF_ELEMS 1024

void * apl_make_elems(array_pool * apl, int elem_size, int num_of_elems);
#define apl_make(papl, elem_size)\
apl_make_elems((papl), (elem_size), APL_DEF_ELEMS)

void apl_destroy(array_pool * apl);

void * apl_reuse(array_pool * apl);

void * apl_reset(array_pool * apl);

void * apl_get(array_pool * apl, int array_len);

void * apl_insert(array_pool * apl, const void * arr, int arr_len);

void * apl_insert_string(array_pool * apl, const char * str, int str_len);

#endif
