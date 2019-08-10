#include <stdlib.h>
#include <string.h>
#include "ov_multimap.h"

typedef unsigned char byte;

#define pointer void *
#define val_cv_ptr(what, ksz)\
((c_vector *)(((byte *)(what)) + ((ksz) - sizeof(c_vector))))
//------------------------------------------------------------------------------

void * ov_mmap_make_cap(
    ov_mmap * mmap,
    int key_size, fcomp key_compar, int key_capacity,
    int val_size, fcomp val_compar, int val_capacity
)
{
    if (key_size > 0 && val_size > 0 && val_capacity > 0)
    {
        int extra_bytes = key_size % sizeof(pointer);
        int align = (extra_bytes) ? sizeof(pointer) - extra_bytes : 0;
        int real_key_size = key_size + align + sizeof(c_vector);

        mmap->orig_key_size = key_size;
        mmap->real_key_size = real_key_size;
        mmap->val_size = val_size;
        mmap->val_compar = val_compar;
        mmap->val_capacity = val_capacity;

        if (ov_set_make_cap(
                &(mmap->the_map), real_key_size, key_compar, key_capacity
            ))
            return mmap;
    }

    memset(mmap, 0, sizeof(*mmap));
    return NULL;
}
//------------------------------------------------------------------------------

void ov_mmap_destroy(ov_mmap * mmap)
{
    ov_set * the_map = &(mmap->the_map);
    int real_key_size = mmap->real_key_size;
    int all_keys = ov_set_length(the_map);

    const void * node;
    c_vector * cv_vals;
    for(int i = 0; i < all_keys; ++i)
    {
        node = ov_set_get(the_map, i);
        cv_vals = val_cv_ptr(node, real_key_size);
        c_vect_destroy(cv_vals);
    }
    ov_set_destroy(the_map);

    memset(mmap, 0, sizeof(*mmap));
}
//------------------------------------------------------------------------------

ov_mmap ov_mmap_copy(const ov_mmap * src, bool * out_success)
{
    int real_key_size = src->real_key_size;
    const ov_set * src_map = &(src->the_map);

    ov_mmap ret;
    memset(&ret, 0, sizeof(ret));

    bool ok = false;
    ov_set new_map = ov_set_copy(src_map, &ok);
    if (ok)
    {
        int all_elems = ov_set_length((ov_set *)src_map);
        const void * new_node, * src_node;
        c_vector * new_vect, * src_vect, src_vect_cpy;
        bool success = false;
        for (int i = 0; i < all_elems; ++i)
        {
            new_node = ov_set_get(&new_map, i);
            new_vect = val_cv_ptr(new_node, real_key_size);
            src_node = ov_set_get((ov_set *)src_map, i);
            src_vect = val_cv_ptr(src_node, real_key_size);

            src_vect_cpy = c_vect_copy(src_vect, &success);
            if (success)
            {
                *new_vect = src_vect_cpy;
                success = false;
            }
            else
            {
                ok = false;
                break;
            }
        }
    }

    if (ok)
    {
        ret = *src;
        ret.the_map = new_map;
    }

    if (out_success)
        *out_success = ok;

    return ret;
}
//------------------------------------------------------------------------------

#define place_key()\
do\
{\
    int val_size = mmap->val_size;\
    fcomp val_compar = mmap->val_compar;\
    int capacity = mmap->val_capacity;\
    int stack_bsz = (real_key_size < OV_MMAP_STACK_MAX) ?\
        real_key_size : OV_MMAP_STACK_MAX;\
    byte stack_buff[stack_bsz], * buff = stack_buff;\
    \
    if ((real_key_size > OV_MMAP_STACK_MAX) &&\
        !(buff = (byte *)malloc(real_key_size)))\
            return NULL;\
    \
    memcpy(buff, key, real_key_size);\
    c_vector * cv_vals = val_cv_ptr(buff, real_key_size);\
    \
    if (c_vect_make_cap(cv_vals, val_size, val_compar, capacity))\
    {\
        ret = c_vect_push(cv_vals, val);\
        ov_set_insert(ovs_keys, buff);\
    }\
    \
    if (buff != stack_buff)\
        free(buff);\
} while(0)

void * ov_mmap_insert_once(ov_mmap * mmap, const void * key, const void * val)
{
    void * ret = NULL;
    int real_key_size = mmap->real_key_size;
    ov_set * ovs_keys = &(mmap->the_map);

    const void * found = ov_set_has(ovs_keys, key);
    if (!found)
        place_key();
    else
    {
        c_vector * cv_vals = val_cv_ptr(found, real_key_size);
        if (!c_vect_find(cv_vals, val))
            ret = c_vect_push(cv_vals, val);
    }

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mmap_insert(ov_mmap * mmap, const void * key, const void * val)
{
    void * ret = NULL;
    int real_key_size = mmap->real_key_size;
    ov_set * ovs_keys = &(mmap->the_map);

    const void * found = ov_set_has(ovs_keys, key);
    if (!found)
        place_key();
    else
    {
        c_vector * cv_vals = val_cv_ptr(found, real_key_size);
        ret = c_vect_push(cv_vals, val);
    }

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mmap_find(ov_mmap * mmap, const void * key, const void * val)
{
    void * ret = NULL;
    ov_set * ovs_keys = &(mmap->the_map);
    int real_key_size = mmap->real_key_size;

    const void * found = ov_set_has(ovs_keys, key);
    if (found)
    {
        c_vector * cv_vals = val_cv_ptr(found, real_key_size);
        ret = c_vect_find(cv_vals, val);
    }

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mmap_remove(ov_mmap * mmap, const void * key, const void * val)
{
    void * ret = NULL;
    ov_set * the_map = &(mmap->the_map);
    int real_key_size = mmap->real_key_size;

    const void * pkey = ov_set_has(the_map, key);
    if (pkey)
    {
        c_vector * cv_vals = val_cv_ptr(pkey, real_key_size);

        int found = -1;
        if (c_vect_find_ind(cv_vals, val, &found))
        {
            c_vect_remove_at(cv_vals, found);
            if (c_vect_is_empty(cv_vals))
            {
                c_vect_destroy(cv_vals);
                ov_set_remove(the_map, key);
            }
            ret = mmap;
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mmap_remove_by_key(ov_mmap * mmap, const void * key)
{
    void * ret = NULL;
    int real_key_size = mmap->real_key_size;
    ov_set * the_map = &(mmap->the_map);

    const void * pkey = ov_set_has(the_map, key);
    if (pkey)
    {
        c_vector * cv_vals = val_cv_ptr(pkey, real_key_size);
        c_vect_destroy(cv_vals);
        ov_set_remove(the_map, key);
        ret = mmap;
    }

    return ret;
}
//------------------------------------------------------------------------------

int ov_mmap_remove_by_val(ov_mmap * mmap, const void * val)
{
    int ret = 0;
    int real_key_size = mmap->real_key_size;
    int keys_len = ov_mmap_num_of_keys(mmap);

    int index, from;
    const void * key;
    c_vector * cv_vals;
    for (int i = 0; i < keys_len; ++i)
    {
        key = ov_mmap_get_key(mmap, i);
        cv_vals = val_cv_ptr(key, real_key_size);
        index = -1, from = 0;

        while (c_vect_find_ind_from(cv_vals, val, &index, from))
        {
            from = index;
            c_vect_remove_at(cv_vals, index);
            ++ret;
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

int ov_mmap_num_of_vals(const ov_mmap * mmap, const void * key)
{
    int ret = 0;

    c_vector * cv_vals = ov_mmap_get_vals((ov_mmap *)mmap, key);
    if (cv_vals)
        ret = c_vect_length(cv_vals);

    return ret;
}
//------------------------------------------------------------------------------

c_vector * ov_mmap_get_vals(ov_mmap * mmap, const void * key)
{
    void * ret = NULL;
    ov_set * the_map = &(mmap->the_map);
    int real_key_size = mmap->real_key_size;

    const void * found = ov_set_has(the_map, key);
    if (found)
        ret = val_cv_ptr(found, real_key_size);

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mmap_get_value(ov_mmap * mmap, int val_number)
{
    void * ret = NULL;
    int real_key_size = mmap->real_key_size;

    if (val_number >= 0)
    {
        int keys_len = ov_mmap_num_of_keys(mmap);
        int vals_len = 0, real_ind = val_number;

        c_vector * cv_vals;
        for (int i = 0; i < keys_len; ++i)
        {
            cv_vals = val_cv_ptr(ov_mmap_get_key(mmap, i), real_key_size);
            vals_len = c_vect_length(cv_vals);
            if (real_ind < vals_len)
            {
                ret = c_vect_get(cv_vals, real_ind);
                break;
            }
            else
                real_ind -= vals_len;
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mmap_fit(ov_mmap * mmap)
{
    int real_key_size = mmap->real_key_size;
    c_vector * the_map = &(mmap->the_map);

    void * ret = c_vect_trim(the_map);
    if (ret)
    {
        int keys_len = ov_mmap_num_of_keys(mmap);

        c_vector * cv_vals;
        for (int i = 0; i < keys_len; ++i)
        {
           cv_vals = val_cv_ptr(ov_mmap_get_key(mmap, i), real_key_size);
           if (!(ret = c_vect_trim(cv_vals)))
                break;
        }

        if (ret)
            ret = mmap;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * ov_mmap_apply(ov_mmap * mmap, mmap_apply fun)
{
    int real_key_size = mmap->real_key_size;
    int keys_len = ov_mmap_num_of_keys(mmap);
    int vals_len = 0;

    const void * key;
    c_vector * cv_vals;
    for (int i = 0; i < keys_len; ++i)
    {
        key = ov_mmap_get_key(mmap, i);
        cv_vals = val_cv_ptr(key, real_key_size);
        vals_len = c_vect_length(cv_vals);

        for (int j = 0; j < vals_len; ++j)
            fun(key, c_vect_get(cv_vals, j));
    }

    return mmap;
}
//------------------------------------------------------------------------------

void * ov_mmap_apply_args(ov_mmap * mmap, mmap_apply_args fun, void * args)
{
    int real_key_size = mmap->real_key_size;
    int keys_len = ov_mmap_num_of_keys(mmap);
    int vals_len = 0;

    const void * key;
    c_vector * cv_vals;
    for (int i = 0; i < keys_len; ++i)
    {
        key = ov_mmap_get_key(mmap, i);
        cv_vals = val_cv_ptr(key, real_key_size);
        vals_len = c_vect_length(cv_vals);

        for (int j = 0; j < vals_len; ++j)
            fun(key, c_vect_get(cv_vals, j), args);
    }

    return mmap;
}
//------------------------------------------------------------------------------

int ov_mmap_orig_key_size(const ov_mmap * mmap)
{
    return mmap->orig_key_size;
}
//------------------------------------------------------------------------------

int ov_mmap_real_key_size(const ov_mmap * mmap)
{
    return mmap->real_key_size;
}
//------------------------------------------------------------------------------

int ov_mmap_val_size(const ov_mmap * mmap)
{
    return mmap->val_size;
}
//------------------------------------------------------------------------------

int ov_mmap_val_capcity(const ov_mmap * mmap)
{
    return mmap->val_capacity;
}
//------------------------------------------------------------------------------
