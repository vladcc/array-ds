#include <string.h>
#include "cv_htable.hpp"
#include "cvh_default_hash.c"
//------------------------------------------------------------------------------

static void * clean_up(cv_htbl * cvht, int buckets_made)
{
    c_vector * the_tbl = &(cvht->the_tbl);

    for (int i = 0; i < buckets_made; ++i)
        c_vect_destroy((c_vector *)c_vect_get(the_tbl, i));
    c_vect_destroy(the_tbl);

    memset(cvht, 0, sizeof(*cvht));
    return NULL;
}
//------------------------------------------------------------------------------

void * cv_htbl_make_cap_hash(
    cv_htbl * cvht, int elem_size, fcomp compar, int capacity, fhash hash
    )
{
    void * ret = NULL;
    c_vector * the_tbl = &(cvht->the_tbl);

    if (c_vect_make_cap(the_tbl, sizeof(c_vector), NULL, capacity))
    {
        c_vect_set_length(the_tbl, capacity);

        c_vector bucket;
        for (int i = 0; i < capacity; ++i)
        {
            if (c_vect_make_cap(
                &bucket, elem_size, compar, CVH_DEFAULT_LOAD_FACT))
                c_vect_write_at(the_tbl, i, &bucket);
            else
                return clean_up(cvht, i);
        }

        cvht->hash_fun = (hash) ? hash : CVH_DEFAULT_HASH;
        cvht->elem_inside = 0;
        cvht->elem_max = capacity*CVH_DEFAULT_LOAD_FACT;
        cvht->elem_size = elem_size;
        ret = cvht;
    }
    else
        memset(cvht, 0, sizeof(*cvht));

    return ret;
}
//------------------------------------------------------------------------------

void cv_htbl_destroy(cv_htbl * cvht)
{
    c_vector * the_tbl = &(cvht->the_tbl);
    c_vect_apply(the_tbl, (fapply)c_vect_destroy);
    c_vect_destroy(the_tbl);
    memset(cvht, 0, sizeof(*cvht));
}
//------------------------------------------------------------------------------

cv_htbl cv_htbl_copy(const cv_htbl * src, bool * out_success)
{
    bool ok = false;
    cv_htbl ret;
    memset(&ret, 0, sizeof(ret));

    if (src->elem_inside)
    {
        bool cpy_ok = false;
        const c_vector * old_tbl = &(src->the_tbl);
        c_vector new_tbl = c_vect_copy(old_tbl, &cpy_ok);

        if (cpy_ok)
        {
            cpy_ok = false;
            c_vector * bucket;
            int new_tbl_len = c_vect_length(&new_tbl);
            for (int i = 0; i < new_tbl_len; ++i)
            {
                bucket = (c_vector *)c_vect_get(&new_tbl, i);
                if (c_vect_length(bucket))
                {
                    *bucket = c_vect_copy(bucket, &cpy_ok);
                    if (!cpy_ok)
                    {
                        cv_htbl fail = *src;
                        fail.the_tbl = new_tbl;
                        clean_up(&fail, i);
                        goto out;
                    }
                }
                else
                {
                    c_vector new_v;
                    c_vect_make_cap(&new_v,
                        c_vect_elem_size(bucket),
                        c_vect_compar(bucket),
                        c_vect_capacity(bucket));
                    *bucket = new_v;
                }
            }

            ret = *src;
            ret.the_tbl = new_tbl;
            ok = true;
        }
    }

out:
    if (out_success)
        *out_success = ok;

    return ret;
}
//------------------------------------------------------------------------------

static const void * actually_find(
    cv_htbl * cvht, const void * key, int * out_index, unsigned int * out_hash
    )
{
    c_vector * the_tbl = &(cvht->the_tbl);

    unsigned int hash = cvht->hash_fun(
        (const unsigned char *)key, cvht->elem_size
        );
    int index;
    void * found = c_vect_find_ind(
        (c_vector *)c_vect_get(the_tbl, hash%c_vect_length(the_tbl)),
        key, &index
    );

    *out_index = index;
    *out_hash = hash;
    return found;
}
//------------------------------------------------------------------------------

const void * cv_htbl_lookup(const cv_htbl * cvht, const void * key)
{
    int out_index;
    unsigned int out_hash;
    return actually_find((cv_htbl *)cvht, key, &out_index, &out_hash);
}
//------------------------------------------------------------------------------

#define CVH_GROWTH_RATE 2

static cv_htbl stop_world_n_rehash(cv_htbl * old_cvht, bool * success)
{
    bool ok = false;
    int el_size = old_cvht->elem_size;
    c_vector * old_tbl = &(old_cvht->the_tbl);
    fcomp compar = c_vect_compar((c_vector *)c_vect_get(old_tbl, 0));
    int old_cap = c_vect_capacity(old_tbl);
    fhash hash_fun = old_cvht->hash_fun;

    cv_htbl new_cvht;
    if (cv_htbl_make_cap_hash(
        &new_cvht, el_size, compar, old_cap*CVH_GROWTH_RATE, hash_fun
        ))
    {
        c_vector * bucket = NULL;
        for (int i = 0; i < old_cap; ++i)
        {
            if ((bucket = (c_vector *)c_vect_get(old_tbl, i)) &&
                    !c_vect_is_empty(bucket))
            {
                int bucket_len = c_vect_length(bucket);
                for (int i = 0; i < bucket_len; ++i)
                    cv_htbl_insert(&new_cvht, c_vect_get(bucket, i));
            }
        }
        ok = true;
        cv_htbl_destroy(old_cvht);
    }

    *success = ok;
    return new_cvht;
}
//------------------------------------------------------------------------------

const void * cv_htbl_insert(cv_htbl * cvht, const void * key)
{
    int out_index;
    unsigned int out_hash;
    const void * ret = actually_find(cvht, key, &out_index, &out_hash);
    if (!ret)
    {
        if (cvht->elem_inside == cvht->elem_max)
        {
            bool ok = false;
            cv_htbl tmp = stop_world_n_rehash(cvht, &ok);
            if (ok)
                *cvht = tmp;
            else
                return NULL;
        }

        c_vector * the_tbl = &(cvht->the_tbl);
        int nbucket = out_hash % c_vect_length(the_tbl);
        c_vector * the_bucket = (c_vector *)c_vect_get(the_tbl, nbucket);

        ret = c_vect_push(the_bucket, key);
        ++cvht->elem_inside;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * cv_htbl_remove(cv_htbl * cvht, const void * key)
{
    c_vector * the_tbl = &(cvht->the_tbl);

    int out_index;
    unsigned int out_hash;
    const void * found = actually_find(cvht, key, &out_index, &out_hash);

    if (found)
    {
        c_vect_remove_at(
            (c_vector *)c_vect_get(the_tbl, out_hash%c_vect_length(the_tbl)),
            out_index
            );
        --cvht->elem_inside;
    }

    return cvht;
}
//------------------------------------------------------------------------------

static void clear_vect(void * cv)
{
    c_vect_reset((c_vector *)cv);
}

void * cv_htbl_clear(cv_htbl * cvht)
{
    c_vect_apply(&(cvht->the_tbl), clear_vect);
    cvht->elem_inside = 0;
    return cvht;
}
//------------------------------------------------------------------------------

const void * cv_htbl_iterate_(cv_htbl * cvht,
    cv_htbl_iterator start, cv_htbl_iterator * out_so_far
    )
{
    void * ret = NULL;
    c_vector * the_tbl = &(cvht->the_tbl);
    int tbl_len = c_vect_length(the_tbl);

    cv_htbl_iterator it = start;
    if ((unsigned int)it.row < (unsigned int)tbl_len)
    {
        int bucket_len;
        c_vector * bucket;
        for (int i = it.row; i < tbl_len; ++i)
        {
            bucket = (c_vector *)c_vect_get(the_tbl, i);
            bucket_len = c_vect_length(bucket);
            if ((unsigned int)it.col < (unsigned int)bucket_len)
            {
                ret = c_vect_get(bucket, it.col);
                out_so_far->row = i;
                out_so_far->col = it.col+1;
                break;
            }
            else
                it.col = 0;
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

bool cv_htbl_is_empty(const cv_htbl * cvht)
{
    return !cvht->elem_inside;
}
//------------------------------------------------------------------------------

int cv_htbl_elem_count(const cv_htbl * cvht)
{
    return cvht->elem_inside;
}
//------------------------------------------------------------------------------

int cv_htbl_elem_max(const cv_htbl * cvht)
{
    return cvht->elem_max;
}
//------------------------------------------------------------------------------

fhash cv_htbl_get_default_hash(void)
{
    return CVH_DEFAULT_HASH;
}
//------------------------------------------------------------------------------
