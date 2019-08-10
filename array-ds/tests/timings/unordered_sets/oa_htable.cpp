#include <math.h>
#include <string.h>
#include "oa_htable.hpp"
#include "oah_default_hash.c"
//------------------------------------------------------------------------------

void * oa_htbl_make_cap_hash(
    oa_htbl * oaht, int elem_size, fcomp compar, int capacity, fhash hash
    )
{
    if (elem_size > 0 && capacity > 0)
    {
        c_vector * the_tbl = &(oaht->the_tbl);
        bit_vector * is_taken = &(oaht->is_taken);

        int all_elems = round((double)capacity / (double)OAH_DEFAULT_LOAD_FACT);
        if (all_elems && c_vect_make_cap(the_tbl, elem_size, compar, all_elems))
        {
            c_vect_set_length(the_tbl, all_elems);
            if (bit_vect_make(is_taken, all_elems*2))
            {
                oaht->hash_fun = hash ? hash : OAH_DEFAULT_HASH;
                oaht->elem_inside = 0;
                oaht->elem_max = capacity;
                return oaht;
            }
            else
                c_vect_destroy(the_tbl);
        }
    }

    memset(oaht, 0, sizeof(*oaht));
    return NULL;
}
//------------------------------------------------------------------------------

void oa_htbl_destroy(oa_htbl * oaht)
{
    c_vect_destroy(&(oaht->the_tbl));
    bit_vect_destroy(&(oaht->is_taken));
    memset(oaht, 0, sizeof(*oaht));
}
//------------------------------------------------------------------------------

oa_htbl oa_htbl_copy(const oa_htbl * src, bool * out_success)
{
    bool ok = false;
    oa_htbl ret;
    memset(&ret, 0, sizeof(ret));

    if (src->elem_inside)
    {
        const c_vector * src_tbl = &(src->the_tbl);
        const bit_vector * src_taken = &(src->is_taken);

        bool tbl_cpy = false, taken_cpy = false;
        c_vector new_tbl = c_vect_copy(src_tbl, &tbl_cpy);
        bit_vector new_is_taken = bit_vect_copy(src_taken, &taken_cpy);

        if (tbl_cpy && taken_cpy)
        {
            ret = *src;
            ret.the_tbl = new_tbl;
            ret.is_taken = new_is_taken;
            ok = true;
        }
    }

    if (out_success)
        *out_success = ok;

    return ret;
}
//------------------------------------------------------------------------------

#define OAH_GROWTH_RATE 2

static oa_htbl stop_world_n_rehash(oa_htbl * old_oah, bool * success)
{
    bool ok = false;
    oa_htbl new_htbl;
    fhash hash_fun = old_oah->hash_fun;
    int old_max = old_oah->elem_max;
    c_vector * old_tbl = &(old_oah->the_tbl);
    int old_cap = c_vect_capacity(old_tbl);
    bit_vector * old_is_taken = &(old_oah->is_taken);

    if (oa_htbl_make_cap_hash(
        &new_htbl, c_vect_elem_size(old_tbl),
        c_vect_compar(old_tbl), old_max*OAH_GROWTH_RATE, hash_fun
        ))
    {
        int bit_val = 0;
        for (int i = 0; i < old_cap; ++i)
        {
            bit_vect_get(old_is_taken, i*2, &bit_val);
            if (bit_val)
                oa_htbl_insert(&new_htbl, c_vect_get(old_tbl, i));
        }

        ok = true;
        oa_htbl_destroy(old_oah);
    }

    *success = ok;
    return new_htbl;
}
//------------------------------------------------------------------------------

#define dry_find()\
{\
    btake = i*2;\
    bvacn = btake+1;\
    if (bit_vect_get(is_taken, btake, &bit_val) && bit_val)\
    {\
        if ((data = c_vect_get(the_tbl, i)) && (compar(key, data) == 0))\
        {\
            ret = data;\
            found_index = i;\
            break;\
        }\
    }\
    else if (bit_vect_get(is_taken, bvacn, &bit_val) && bit_val)\
        continue;\
    else\
        break;\
}

static const void * actually_find(
    oa_htbl * oaht, const void * key, int * out_index, unsigned int * key_hash
    )
{
    void * ret = NULL;

    c_vector * the_tbl = &(oaht->the_tbl);
    bit_vector * is_taken = &(oaht->is_taken);
    unsigned int hash = oaht->hash_fun(
        (const unsigned char *)key, c_vect_elem_size(the_tbl)
        );
    int cap = c_vect_length(the_tbl);
    int ins_point = hash % cap;

    void * data = NULL;
    int i, bit_val = 0, found_index = -1, btake, bvacn;
    fcomp compar = c_vect_compar(the_tbl);
    for (i = ins_point; i < cap; ++i)
        dry_find();

    if (i == cap && !ret)
    {
        for (i = 0; i < ins_point; ++i)
            dry_find();
    }

    *out_index = found_index;
    *key_hash = hash;
    return ret;
}
//------------------------------------------------------------------------------

#define dry_insert()\
{\
    btake = i*2;\
    bvacn = btake+1;\
    if (bit_vect_get(is_taken, btake, &bit_val) && !bit_val)\
    {\
        ret = c_vect_write_at(the_tbl, i, key);\
        bit_vect_set(is_taken, btake);\
        bit_vect_clear(is_taken, bvacn);\
        ++oaht->elem_inside;\
        break;\
    }\
}

const void * oa_htbl_insert(oa_htbl * oaht, const void * key)
{
    int found_index;
    unsigned int key_hash;
    const void * ret = actually_find(oaht, key, &found_index, &key_hash);

    if (!ret)
    {
        if (oaht->elem_inside == oaht->elem_max)
        {
            bool ok = false;
            oa_htbl tmp = stop_world_n_rehash(oaht, &ok);

            if (ok)
                *oaht = tmp;
            else
                return NULL;
        }

        c_vector * the_tbl = &(oaht->the_tbl);
        bit_vector * is_taken = &(oaht->is_taken);

        int cap = c_vect_length(the_tbl);
        int ins_point = key_hash % cap;

        int bit_val = 0, i, btake, bvacn;
        for (i = ins_point; i < cap; ++i)
            dry_insert();

        if (!ret)
        {
            for (i = 0, bit_val = 0; i < ins_point; ++i)
                dry_insert();
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

const void * oa_htbl_lookup(const oa_htbl * oaht, const void * key)
{
    int found_index;
    unsigned int key_hash;
    return actually_find((oa_htbl *)oaht, key, &found_index, &key_hash);
}
//------------------------------------------------------------------------------

void * oa_htbl_remove(oa_htbl * oaht, const void * key)
{
    int found_index;
    unsigned int key_hash;
    bit_vector * is_taken = &(oaht->is_taken);
    const void * data = actually_find(oaht, key, &found_index, &key_hash);

    if (data)
    {
        int btake = found_index*2;
        int bvacn = btake+1;
        bit_vect_clear(is_taken, btake);
        bit_vect_set(is_taken, bvacn);
        --oaht->elem_inside;
    }

    return oaht;
}
//------------------------------------------------------------------------------

void * oa_htbl_clear(oa_htbl * oaht)
{
    bit_vect_clear_all(&(oaht->is_taken));
    oaht->elem_inside = 0;
    return oaht;
}
//------------------------------------------------------------------------------

const void * oa_htbl_iterate_(oa_htbl * oaht, int start, int * out_so_far)
{
    void * ret = NULL;
    c_vector * the_tbl = &(oaht->the_tbl);
    bit_vector * is_taken = &(oaht->is_taken);
    int cap = c_vect_capacity(the_tbl);

    if ((unsigned int)start < (unsigned int)cap)
    {
        int bit_val = 0;
        for (int i = start; i < cap; ++i)
        {
            if (bit_vect_get(is_taken, i*2, &bit_val) && bit_val)
            {
                ret = c_vect_get(the_tbl, i);
                *out_so_far = i+1;
                break;
            }
        }
    }

    return ret;
}
//------------------------------------------------------------------------------

bool oa_htbl_is_empty(const oa_htbl * oaht)
{
    return !oaht->elem_inside;
}
//------------------------------------------------------------------------------

int oa_htbl_elem_count(const oa_htbl * oaht)
{
    return oaht->elem_inside;
}
//------------------------------------------------------------------------------

int oa_htbl_elem_max(const oa_htbl * oaht)
{
    return oaht->elem_max;
}
//------------------------------------------------------------------------------

fhash oa_htbl_get_default_hash(void)
{
    return OAH_DEFAULT_HASH;
}
//------------------------------------------------------------------------------
