/*  mem_pool.h -- memory pool
    v1.11

    A c_vector of memory blocks.
    Allocates a contiguous piece of memory, gives you pointers to a preset
    number of bytes at a time. When it reaches the end of the current memory
    block, allocates another. Allocated memory is only freed at once upon reset,
    or on destroy.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-08-18
*/

#ifndef MEM_POOL_H
#define MEM_POOL_H

#include "../c_vector/c_vector.h"

// Do not use directly
typedef struct mem_pool {
    c_vector the_pool;
    int elem_count;
    int elem_size;
    int node_cap;
} mem_pool;

void * mpl_make(mem_pool * mpl, int elem_size, int node_cap);
/*
Returns: mpl on success, NULL otherwise.

Description: Creates a memory pool of node_cap number of elem_size elements. On
failure *mpl is zeroed out.

Complexity: O(1)
*/

void mpl_destroy(mem_pool * mpl);
/*
Returns: Nothing.

Description: Frees all memory associated with mpl, zeroes *mpl out.

Complexity: O(1)
*/

void * mpl_get(mem_pool * mpl);
/*
Returns: A pointer to the next available location in mpl.

Description: Retrieves a location in memory elem_size number of bytes large.

Complexity: O(1)
*/

void * mpl_reset(mem_pool * mpl);
/*
Returns: mpl on success, NULL otherwise.

Description: Destroys and recreates mpl with only one memory block. As all
memory belonging to mpl before this call is freed, all pointers received from
mpl before this call are now invalid.

Complexity: O(n) where n is the number of memory blocks.
*/
#endif
