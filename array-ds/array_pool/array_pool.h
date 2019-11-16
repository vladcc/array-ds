/*  array_pool.h -- memory pool of contiguous memory with little overhead
    v1.0

    You can request memory with certain size and alignment, guaranteed to be
    contiguous.  Memory is allocated in slabs with a default size. If a request
    for larger than the default size is made, a new slab with the larger size
    is allocated.  Alignment values must be powers of 2. Reusing an array pool
    only makes it appear empty and memory is not freed until the array pool is
    either destroyed or reset.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-11-16
*/
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
#define apl_make(papl) apl_make_size((papl), APL_DEF_SIZE)
/*
Returns: apl on success, NULL otherwise.

Description: Creates the array pool pointed to by apl. The array pool has
exactly one slab segment_size_bytes large. segment_size_bytes must be > 0.

Complexity: O(1)
*/

void apl_destroy(array_pool * apl);
/*
Returns: Nothing.

Description: Frees the array pool pointed to by apl, zeroes out *apl.

Complexity: O(1)
*/

void * apl_reuse(array_pool * apl);
/*
Returns: apl

Description: 'Empties' apl by 'forgetting' all data.

Complexity: O(n), where n is the number of slabs.
*/

void * apl_reset(array_pool * apl);
/*
Returns: apl on success, NULL otherwise.

Description: Frees and recreates apl. The new array pool has exactly one slab
with a default segment size as large as the original.

Complexity: O(n), where n is the number of slabs.
*/

byte * apl_get_align(array_pool * apl, int bytes, int alignment);
#define apl_get(papl, bytes) apl_get_align((papl), (bytes), APL_DEF_ALIGN)
/*
Returns: A pointer to an array of size bytes aligned to alignment on success,
NULL otherwise.

Description: Goes through all segments allocated by apl and looks for
bytes + (alignment - 1) bytes of available space. If that is not found, a new
slab is allocated with either the default segment size for apl, or of size
bytes + (alignment - 1), whichever is greater. bytes must be > 0, alignment
must be a power of 2.

Complexity: O(n), where n is the number of slabs.
*/

byte * apl_insert_align(array_pool * apl,
    const void * arr, int arr_len, int elem_size, int alignment
);
/*
Returns: A pointer to a copy of arr aligned to alignment on success, NULL
otherwise.

Description: Shorthand for writing an array to the pool.

Complexity: O(n+m), where n is the number of slabs and m is the size of arr.
*/

byte * apl_insert(array_pool * apl,
    const void * arr, int arr_len, int elem_size
);
/*
Returns: A pointer to a copy of arr aligned to elem_size on success, NULL
otherwise.

Description: Shorthand for writing an array to the pool.

Complexity: O(n+m), where n is the number of slabs and m is the size of arr.
*/

char * apl_insert_string_align(array_pool * apl,
    const char * str, int str_len, int alignment
);
/*
Returns: A pointer to a copy of str aligned to alignment on success, NULL
otherwise.

Description: Shorthand for writing a string to the pool.

Complexity: O(n+m), where n is the number of slabs and m is the length of str.
*/

#define apl_insert_string(papl, pstr, str_len)\
apl_insert_string_align((papl), (pstr), (str_len), sizeof(*(pstr)))
/*
Returns: A pointer to a copy of str aligned to sizeof(*str)

Description: Shorthand for writing a string to the pool.

Complexity: O(n+m), where n is the number of slabs and m is the length of str.
*/
#endif
