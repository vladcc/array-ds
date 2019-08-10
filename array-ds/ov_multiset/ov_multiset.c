#include <stdlib.h>
#include <string.h>
#include "ov_multiset.h"

typedef unsigned char byte;

#define counter_type int
#define count_ptr(what, esz)\
((counter_type *)(((byte *)(what)) + ((esz) - sizeof(counter_type))))
//------------------------------------------------------------------------------

void * ov_mset_make_cap(
    ov_mset * mset, int elem_size, fcomp compar, int capacity
)
{
    if (elem_size > 0 && capacity > 0)
    {
        mset->orig_elem_size = elem_size;
        c_vector * the_vect = &(mset->the_vect);
        int extra_bytes = elem_size % sizeof(counter_type);
        int align = (extra_bytes) ? sizeof(counter_type) - extra_bytes : 0;

        if (c_vect_make_cap(
                    the_vect,
                    elem_size+align+sizeof(counter_type), compar, capacity
                ))
            return mset;

    }

    memset(mset, 0, sizeof(*mset));
    return NULL;
}
//------------------------------------------------------------------------------

void ov_mset_destroy(ov_mset * mset)
{
    c_vect_destroy(&(mset->the_vect));
    memset(mset, 0, sizeof(*mset));
}
//------------------------------------------------------------------------------

ov_mset ov_mset_copy(const ov_mset * src, bool * out_success)
{
    const c_vector * src_vect = &(src->the_vect);
    int src_orig_esz = src->orig_elem_size;

    ov_mset ret;
    memset(&ret, 0, sizeof(ret));

    bool ok = false;
    c_vector new_vect = c_vect_copy(src_vect, &ok);
    if (ok)
    {
        ret.the_vect = new_vect;
        ret.orig_elem_size = src_orig_esz;
    }

    if (out_success)
        *out_success = ok;

    return ret;
}
//------------------------------------------------------------------------------

const void * ov_mset_insert(ov_mset * mset, const void * what)
{
    c_vector * the_vect = &(mset->the_vect);
    int orig_elem_size = mset->orig_elem_size;
    int real_elem_size = c_vect_elem_size(the_vect);

    void * ret = c_vect_bsearch(the_vect, what);
    if (!ret)
    {
        int stack_bsz = (real_elem_size < OV_MSET_STACK_MAX) ?
            real_elem_size : OV_MSET_STACK_MAX;

        byte stack_buff[stack_bsz], * buff = stack_buff;

        if (real_elem_size > OV_MSET_STACK_MAX &&
            !(buff = (byte *)malloc(real_elem_size)))
            return NULL;

        memcpy(buff, what, orig_elem_size);
        counter_type * count = count_ptr(buff, real_elem_size);
        *count = 1;
        ret = c_vect_insert_online(the_vect, buff);

        if (buff != stack_buff)
            free(buff);
    }
    else
    {
        counter_type * count = count_ptr(ret, real_elem_size);
        ++(*count);
        ret = NULL;
    }
    return ret;
}
//------------------------------------------------------------------------------

void * ov_mset_remove(ov_mset * mset, const void * what)
{
    void * ret = NULL;

    int index = -1;
    c_vector * the_vect = &(mset->the_vect);
    int real_elem_size = c_vect_elem_size(the_vect);

    void * found = NULL;
    if ((found = c_vect_bsearch_ind(the_vect, what, &index)))
    {
        counter_type * count = count_ptr(found, real_elem_size);

        if (!(--*count))
            c_vect_remove_at(the_vect, index);

        ret = mset;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mset_erase(ov_mset * mset, const void * what)
{
    void * ret = NULL;

    int index = -1;
    c_vector * the_vect = &(mset->the_vect);

    void * found = NULL;
    if ((found = c_vect_bsearch_ind(the_vect, what, &index)))
    {
        c_vect_remove_at(the_vect, index);
        ret = mset;
    }

    return ret;
}
//------------------------------------------------------------------------------

int ov_mset_mplicity(const ov_mset * mset, const void * what)
{
    const c_vector * the_vect = &(mset->the_vect);
    int real_elem_size = c_vect_elem_size(the_vect);
    int ret = 0;

    void * pelm = c_vect_bsearch((c_vector *)the_vect, what);
    if (pelm)
        ret = *(count_ptr(pelm, real_elem_size));

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mset_apply(ov_mset * mset, fapply_const fun)
{
    c_vect_apply(&(mset->the_vect), (fapply)fun);
    return mset;
}
//------------------------------------------------------------------------------

void * ov_mset_apply_args(ov_mset * mset, fapply_const_args fun, void * args)
{
    c_vect_apply_args(&(mset->the_vect), (fapply_args)fun, args);
    return mset;
}
//------------------------------------------------------------------------------

int ov_mset_orig_elem_size(const ov_mset * mset)
{
    return mset->orig_elem_size;
}
//------------------------------------------------------------------------------
