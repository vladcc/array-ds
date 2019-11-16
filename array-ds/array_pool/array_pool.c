#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "array_pool.h"

typedef unsigned char byte;

// Goes into the_pool
typedef struct apl_node {
    byte * mem;
    int capacity;
    int bytes_left;
} apl_node;

#define get_pool(papl) &(papl->the_pool)
//------------------------------------------------------------------------------

#define add_slab_default(papl) add_slab((papl), 0)

static void * add_slab(array_pool * apl, int extern_cap)
{
    void * ret = NULL;

    int capacity = apl->node_cap;

    if (extern_cap > capacity)
        capacity = extern_cap;

    if (capacity > 0)
    {
        ret = malloc(capacity);
        if (ret)
        {
            apl_node node = {
                .mem = ret,
                .capacity = capacity,
                .bytes_left = capacity
            };
            ret = c_vect_push(get_pool(apl), &node);
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

void * apl_make_size(array_pool * apl, int segment_size_bytes)
{
    c_vector * the_pool = get_pool(apl);

    if (segment_size_bytes > 0 && c_vect_make(the_pool, sizeof(apl_node), NULL))
    {
        apl->node_cap = segment_size_bytes;

        if (add_slab_default(apl))
            return apl;
        else
            c_vect_destroy(the_pool);
    }

    return NULL;
}
//------------------------------------------------------------------------------

static void free_elem(void * elem)
{
    free(((apl_node*)elem)->mem);
}

void apl_destroy(array_pool * apl)
{
    c_vector * the_pool = get_pool(apl);
    c_vect_apply(the_pool, free_elem);
    c_vect_destroy(the_pool);
    memset(apl, 0, sizeof(*apl));
}
//------------------------------------------------------------------------------

static void reuse_node(void * elem)
{
    apl_node * node = elem;
    node->bytes_left = node->capacity;
}

void * apl_reuse(array_pool * apl)
{
    c_vector * the_pool = get_pool(apl);
    c_vect_apply(the_pool, reuse_node);
    return apl;
}
//------------------------------------------------------------------------------

void * apl_reset(array_pool * apl)
{
    int segment_size = apl->node_cap;

    apl_destroy(apl);
    return apl_make_size(apl, segment_size);
}
//------------------------------------------------------------------------------

static int find_space(const void * vect_key, const void * my_key)
{
    const apl_node * vect_node = vect_key;
    return !(vect_node->bytes_left >= *((const int *)my_key));
}

static bool is_power_of_2(int n)
{
    return ((n > 0) && !(n & (n - 1)));
}

byte * apl_get_align(array_pool * apl, int bytes, int alignment)
{
#define mod_pwr_2(x, pwr2) ((x) & ((pwr2) - 1))
    void * ret = NULL;

    if (is_power_of_2(alignment)
        && bytes > 0
        && (bytes <= (bytes + (alignment-1))))
    {
        int wanted_size = bytes + (alignment-1);

        apl_node * slab = NULL;
        c_vector * the_pool = get_pool(apl);
        if ((slab = c_vect_find_if(the_pool, find_space, &wanted_size))
            || (slab = add_slab(apl, wanted_size)))
        {
            uintptr_t head = (uintptr_t)slab->mem
                + (slab->capacity - slab->bytes_left);

            int mod_bytes = mod_pwr_2(head, alignment);

            if (mod_bytes)
            {
                int extra = alignment - mod_bytes;
                head += extra;
                bytes += extra;
            }

            slab->bytes_left -= bytes;
            ret = (void *)head;
        }
    }

    return ret;
#undef mod_pwr_2
}
//------------------------------------------------------------------------------

byte * apl_insert_align(array_pool * apl,
    const void * arr, int arr_len, int elem_size, int alignment
)
{
    void * apl_mem = apl_get_align(apl, arr_len*elem_size, alignment);

    if (apl_mem)
        memcpy(apl_mem, arr, arr_len*elem_size);

    return apl_mem;
}
//------------------------------------------------------------------------------

byte * apl_insert(array_pool * apl,
    const void * arr, int arr_len, int elem_size
)
{
    return apl_insert_align(apl, arr, arr_len, elem_size, elem_size);
}
//------------------------------------------------------------------------------

char * apl_insert_string_align(array_pool * apl,
    const char * str, int str_len, int alignment
)
{
    return (char *)apl_insert(apl, str, str_len+1, alignment);
}
//------------------------------------------------------------------------------
