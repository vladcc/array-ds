#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mem_pool.h"

typedef unsigned char byte;
//------------------------------------------------------------------------------

static void * mpl_add_slab(mem_pool * mpl)
{
    void * ret = malloc(mpl->elem_size * mpl->node_cap);

    if (ret)
    {
        c_vect_push(&(mpl->the_pool), &ret);
        mpl->elem_count = 0;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * mpl_make(mem_pool * mpl, int elem_size, int node_cap)
{
    c_vector * the_pool = &(mpl->the_pool);
    void * ret = NULL;

    if (elem_size > 0 && node_cap > 0 &&
        (ret = c_vect_make(the_pool, sizeof(void *), NULL)))
    {
        mpl->elem_count = 0;
        mpl->elem_size = elem_size;
        mpl->node_cap = node_cap;

        if (mpl_add_slab(mpl))
            ret = mpl;
        else
        {
            ret = NULL;
            memset(mpl, 0, sizeof(*mpl));
        }
    }
    else
        memset(mpl, 0, sizeof(*mpl));

    return ret;
}
//------------------------------------------------------------------------------

void * mpl_get(mem_pool * mpl)
{
    if (mpl->elem_count == mpl->node_cap)
        if (!mpl_add_slab(mpl))
            return NULL;

    c_vector * the_pool = &(mpl->the_pool);
    byte * top = *((byte **)c_vect_get(the_pool, c_vect_length(the_pool)-1));

    return top + mpl->elem_size * mpl->elem_count++;
}
//------------------------------------------------------------------------------

void * mpl_reset(mem_pool * mpl)
{
    mpl->elem_count = 0;
    c_vector * the_pool = &(mpl->the_pool);

    for (int i = c_vect_length(the_pool); i > 1; --i)
        free(*((byte **)c_vect_peek_pop(the_pool)));

    return mpl;
}
//------------------------------------------------------------------------------

static void free_elem(void * elem)
{
    free(*((byte **)elem));
}

void mpl_destroy(mem_pool * mpl)
{
    c_vector * the_pool = &(mpl->the_pool);
    c_vect_apply(the_pool, free_elem);
    c_vect_destroy(the_pool);
    memset(mpl, 0, sizeof(*mpl));
}
//------------------------------------------------------------------------------
