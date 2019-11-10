#include <stdlib.h>
#include <string.h>
#include "array_pool.h"

typedef unsigned char byte;

// Goes into the_pool
typedef struct apl_node {
    byte * mem;
    int capacity;
    int elems_left;
} apl_node;

#define get_pool(papl) &(papl->the_pool)
//------------------------------------------------------------------------------

static void * add_slab(array_pool * apl, int extern_cap)
{
    int capacity = apl->node_cap;

    if (extern_cap > capacity)
        capacity = extern_cap;

    void * ret = malloc(apl->elem_size * capacity);

    if (ret)
    {
        apl_node node = {
            .mem = ret,
            .capacity = capacity,
            .elems_left = capacity
        };
        ret = c_vect_push(get_pool(apl), &node);
    }

    return ret;
}

//------------------------------------------------------------------------------

void * apl_make_elems(array_pool * apl, int elem_size, int num_of_elems)
{
    c_vector * the_pool = get_pool(apl);

    if (elem_size > 0 && num_of_elems > 0 &&
        c_vect_make(the_pool, sizeof(apl_node), NULL))
    {
        apl->elem_size = elem_size;
        apl->node_cap = num_of_elems;

        if (add_slab(apl, 0))
            return apl;
        else
            c_vect_destroy(the_pool);
    }

    return NULL;
}
//------------------------------------------------------------------------------

static void free_elem(void * elem)
{
    free(((apl_node *)elem)->mem);
}

void apl_destroy(array_pool * apl)
{
    c_vector * the_pool = get_pool(apl);
    c_vect_apply(the_pool, free_elem);
    c_vect_destroy(the_pool);
    memset(apl, 0, sizeof(*apl));
}
//------------------------------------------------------------------------------

static void reset_nodes(void * elem)
{
    apl_node * node = elem;
    node->elems_left = node->capacity;
}

void * apl_reuse(array_pool * apl)
{
    c_vector * the_pool = get_pool(apl);
    c_vect_apply(the_pool, reset_nodes);
    return apl;
}
//------------------------------------------------------------------------------

void * apl_reset(array_pool * apl)
{
    int elem_size = apl->elem_size;
    int num_of_elems = apl->node_cap;

    apl_destroy(apl);
    return apl_make_elems(apl, elem_size, num_of_elems);
}
//------------------------------------------------------------------------------

static int find_space(const void * vect_key, const void * my_key)
{
    const apl_node * vect_node = vect_key;
    return !(vect_node->elems_left >= *((const int *)my_key));
}

void * apl_get(array_pool * apl, int array_len)
{
    void * ret = NULL;

    if (array_len > 0)
    {
        c_vector * the_pool = get_pool(apl);

        apl_node * slab = NULL;
        if ((slab = c_vect_find_if(the_pool, find_space, &array_len))
            || (slab = add_slab(apl, array_len)))
        {
            ret = slab->mem +
                ((slab->capacity - slab->elems_left) * apl->elem_size);
            slab->elems_left -= array_len;
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

void * apl_insert(array_pool * apl, const void * arr, int arr_len)
{
    void * apl_mem = apl_get(apl, arr_len);

    if (apl_mem)
        memcpy(apl_mem, arr, arr_len * apl->elem_size);

    return apl_mem;
}
//------------------------------------------------------------------------------

void * apl_insert_string(array_pool * apl, const char * str, int str_len)
{
    void * ret = NULL;

    if (1 == apl->elem_size)
        ret = apl_insert(apl, str, str_len+1);

    return ret;
}
//------------------------------------------------------------------------------
