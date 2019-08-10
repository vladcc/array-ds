#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ov_multimap.h"
#include "test.h"

static bool test_make_destroy(void);
static bool test_insert(void);
static bool test_find(void);
static bool test_mapping_str_val(void);
static bool test_copy(void);

static ftest tests[] = {
    test_make_destroy,
    test_insert,
    test_find,
    test_mapping_str_val,
    test_copy
};

//------------------------------------------------------------------------------

typedef int test_val;

typedef struct test_str {
    int john;
    int jack;
    int jill;
    int zzeben;
} test_str;

test_str gtest;
//------------------------------------------------------------------------------

static void apply_no_args(const void * key, void * val)
{
    if (strcmp(key, "John") == 0)
        gtest.john += *((test_val *)val);
    else if (strcmp(key, "Jack") == 0)
        gtest.jack += *((test_val *)val);
    else if (strcmp(key, "Jill") == 0)
        gtest.jill += *((test_val *)val);
}
//------------------------------------------------------------------------------

static void apply_args(const void * key, void * val, void * args)
{
    test_str * ltest = args;
    if (strcmp(key, "John") == 0)
        ltest->john += *((test_val *)val);
    else if (strcmp(key, "Jack") == 0)
        ltest->jack += *((test_val *)val);
    else if (strcmp(key, "Zzeben") == 0)
        ltest->zzeben += *((test_val *)val);
}
//------------------------------------------------------------------------------

static int compar_key(const void * one, const void * two)
{
    return strcmp((char *)one, (char *)two);
}
//------------------------------------------------------------------------------

static int compar_val(const void * one, const void * two)
{
    test_val a = *((test_val *)one);
    test_val b = *((test_val *)two);
    if (a > b) return 1;
    if (a < b) return -1;
    return 0;
}
//------------------------------------------------------------------------------

#define MAX_STR 13
static bool test_make_destroy(void)
{
    ov_mmap mmap_, * mmap = &mmap_;
    memset(mmap, 0xFF, sizeof(ov_mmap));

    ov_mmap zout;
    memset(&zout, 0, sizeof(ov_mmap));

    int key_sz = MAX_STR, start_cap = 2;

    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        key_sz, compar_key, 0,
        sizeof(test_val), compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        0, compar_key, 33,
        sizeof(test_val), compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        0, compar_key, 33,
        sizeof(test_val), compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        key_sz, compar_key, 33,
        sizeof(test_val), compar_val, 0));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        key_sz, compar_key, 33,
        0, compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        key_sz, compar_key, -123,
        sizeof(test_val), compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        -123, compar_key, 33,
        sizeof(test_val), compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        -123, compar_key, 33,
        sizeof(test_val), compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        key_sz, compar_key, 33,
        sizeof(test_val), compar_val, -123));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    memset(mmap, 0xFF, sizeof(ov_mmap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    check(!ov_mmap_make_cap(mmap,
        key_sz, compar_key, 33,
        -123, compar_val, start_cap));
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);

    check(ov_mmap_make_cap(mmap,
        key_sz, compar_key, 33,
        sizeof(test_val), compar_val, start_cap) == mmap);

    int extra_bytes = key_sz % sizeof(void *);
    int align = (extra_bytes) ? sizeof(void *) - extra_bytes : 0;

    check(align == 3);
    check(mmap->orig_key_size == key_sz);
    check(mmap->real_key_size == key_sz + align + sizeof(ov_set));
    check((mmap->real_key_size % sizeof(void *)) == 0);
    check(mmap->val_size == sizeof(test_val));
    check(mmap->val_compar == compar_val);
    check(mmap->val_capacity == start_cap);
    check(c_vect_capacity(&(mmap->the_map)) == 33);
    check(ov_mmap_capacity(mmap) == 33);

    ov_set * the_map = &(mmap->the_map);
    check(the_map->elem_size == key_sz + align + sizeof(ov_set));
    check(the_map->compar == compar_key);
    check(the_map->capacity == 33);

    check(memcmp(&zout, mmap, sizeof(ov_mmap)) != 0);
    ov_mmap_destroy(mmap);
    check(memcmp(&zout, mmap, sizeof(ov_mmap)) == 0);
    return true;
}

//------------------------------------------------------------------------------

#define LNAME 17
static bool test_insert(void)
{
    static char names[][MAX_STR] = {
        "John", "Jack", "Jill", "Zzeben"
    };

    static char lnames[][LNAME] = {"Ben", "Dover", "Filet Mignon"};

    ov_mmap mmap_, * mmap = &mmap_;
    int key_sz = MAX_STR;
    int val_sz = LNAME;
    check(ov_mmap_make(mmap,
        key_sz, (fcomp)strcmp, val_sz, (fcomp)strcmp) == mmap
        );

    int nm_sz = sizeof(names)/sizeof(*names);
    int ln_sz = sizeof(lnames)/sizeof(*lnames);

    const test_val * elm;
    check(ov_mmap_num_of_keys(mmap) == 0);
    check(ov_mmap_is_empty(mmap));
    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < ln_sz; ++j)
        {
            check(*(elm = ov_mmap_insert_once(mmap, names+i, lnames+j)));
            check(strcmp((char *)elm, (char *)(lnames+j)) == 0);
        }
    }

    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < ln_sz; ++j)
            check(!(elm = ov_mmap_insert_once(mmap, names+i, lnames+j)));
    }

    check(ov_mmap_num_of_keys(mmap) == nm_sz);
    check(!ov_mmap_is_empty(mmap));

    for (int i = 0; i < nm_sz; ++i)
    {
        c_vector * cv_vals = ov_mmap_get_vals(mmap, names+i);

        check(c_vect_length(cv_vals) == ln_sz);
        check(ov_mmap_num_of_vals(mmap, names+i) == ln_sz);

        for (int j = 0; j < ln_sz; ++j)
        {
            check(*(elm = c_vect_get(cv_vals, j)));
            check(strcmp((char *)elm, (char *)(lnames+j)) == 0);
        }
    }

    for (int i = 0; i < nm_sz; ++i)
        check(ov_mmap_find(mmap, names+i, lnames+2));

    check(ov_mmap_remove_by_val(mmap, lnames+2) == ov_mmap_num_of_keys(mmap));

    for (int i = 0; i < nm_sz; ++i)
        check(!ov_mmap_find(mmap, names+i, lnames+2));

    check(ov_mmap_remove_by_key(mmap, names) == mmap);
    --nm_sz;
    for (int i = 0; i < nm_sz; ++i)
    {
        check(ov_mmap_find(mmap, names+i+1, lnames+1));
        check(ov_mmap_num_of_vals(mmap, names+i+1) == ln_sz-1);
    }

    check(ov_mmap_remove_by_val(mmap, lnames+1) == ov_mmap_num_of_keys(mmap));

    for (int i = 0; i < nm_sz; ++i)
    {
        check(!ov_mmap_find(mmap, names+i+1, lnames+1));
        check(ov_mmap_num_of_vals(mmap, names+i+1) == ln_sz-1-1);
    }

    ov_mmap_destroy(mmap);

    check(ov_mmap_make(mmap,
        key_sz, (fcomp)strcmp, val_sz, (fcomp)strcmp) == mmap
        );

    check(ov_mmap_num_of_keys(mmap) == 0);
    check(ov_mmap_is_empty(mmap));
    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < ln_sz; ++j)
        {
            check(*(elm = ov_mmap_insert(mmap, names+i, lnames+j)));
            check(strcmp((char *)elm, (char *)(lnames+j)) == 0);
        }
    }

    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < ln_sz; ++j)
        {
            check(*(elm = ov_mmap_insert(mmap, names+i, lnames+j)));
            check(strcmp((char *)elm, (char *)(lnames+j)) == 0);
        }
    }


    check(ov_mmap_num_of_keys(mmap) == nm_sz);
    check(!ov_mmap_is_empty(mmap));

    for (int i = 0; i < nm_sz; ++i)
    {
        c_vector * cv_vals = ov_mmap_get_vals(mmap, names+i);

        check(c_vect_length(cv_vals) == ln_sz*2);
        check(ov_mmap_num_of_vals(mmap, names+i) == ln_sz*2);

        int j, end = ln_sz;
        for (j = 0; j < end; ++j)
        {
            check(*(elm = c_vect_get(cv_vals, j)));
            check(strcmp((char *)elm, (char *)(lnames+j)) == 0);
        }

        for (int k = 0; k < end; ++k)
        {
            check(*(elm = c_vect_get(cv_vals, j+k)));
            check(strcmp((char *)elm, (char *)(lnames+k)) == 0);
        }
    }

    for (int i = 0; i < nm_sz; ++i)
    {
        check(ov_mmap_find(mmap, names+i, lnames+2));
        check(ov_mmap_num_of_vals(mmap, names+i) == ln_sz*2);
    }

    check(ov_mmap_remove_by_val(mmap, lnames+2) == ov_mmap_num_of_keys(mmap)*2);

    for (int i = 0; i < nm_sz; ++i)
    {
        check(!ov_mmap_find(mmap, names+i, lnames+2));
        check(ov_mmap_num_of_vals(mmap, names+i) == ln_sz*2-2);
    }

    check(ov_mmap_remove_by_key(mmap, names) == mmap);
    --nm_sz;
    for (int i = 0; i < nm_sz; ++i)
    {
        check(ov_mmap_find(mmap, names+i+1, lnames+1));
        check(ov_mmap_num_of_vals(mmap, names+i+1) == ln_sz*2-2);
    }

    check(ov_mmap_remove_by_val(mmap, lnames+1) == ov_mmap_num_of_keys(mmap)*2);

    for (int i = 0; i < nm_sz; ++i)
    {
        check(!ov_mmap_find(mmap, names+i+1, lnames+1));
        check(ov_mmap_num_of_vals(mmap, names+i+1) == ln_sz*2-2-2);
    }

    ov_mmap_destroy(mmap);
    return true;
}
//------------------------------------------------------------------------------

static bool test_find(void)
{
    static char names[][MAX_STR] = {
        "John", "Jack", "Jill", "Zzeben"
    };

    static test_val heights[] = {165, 202, 178, 190, 188};

    ov_mmap mmap_, * mmap = &mmap_;
    int key_sz = MAX_STR;
    check(ov_mmap_make(mmap,
        key_sz, (fcomp)strcmp/*compar_key*/, sizeof(test_val), compar_val
        ) == mmap);

    int nm_sz = sizeof(names)/sizeof(*names);
    int heig_sz = sizeof(heights)/sizeof(*heights);

    const test_val * elm;
    check(ov_mmap_num_of_keys(mmap) == 0);
    check(ov_mmap_is_empty(mmap));
    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < heig_sz; ++j)
        {
            check(*(elm = ov_mmap_insert_once(mmap, names+i, heights+j))
                  == heights[j]);
            check(!ov_mmap_insert_once(mmap, names+i, heights+j));
        }
    }

    static char notin[MAX_STR] = "Herc";
    static char notin2[MAX_STR] = "Xena";
    test_val hnotin = 120, hnotin2 = 210, hnotin3 = -213;

    check(!ov_mmap_get_vals(mmap, notin));
    check(!ov_mmap_get_vals(mmap, notin2));

    for (int i = 0; i < nm_sz; ++i)
    {
        check(!ov_mmap_find(mmap, notin, &hnotin));
        check(!ov_mmap_find(mmap, notin, &hnotin2));
        check(!ov_mmap_find(mmap, notin, &hnotin3));
        check(!ov_mmap_find(mmap, notin2, &hnotin));
        check(!ov_mmap_find(mmap, notin2, &hnotin2));
        check(!ov_mmap_find(mmap, notin2, &hnotin3));
        check(!ov_mmap_find(mmap, notin, heights+i));
        check(!ov_mmap_find(mmap, names+i, &hnotin));
        check(!ov_mmap_find(mmap, names+i, &hnotin2));
        check(!ov_mmap_find(mmap, names+i, &hnotin3));
        check(ov_mmap_find(mmap, names+i, heights+i));
    }

    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < heig_sz; ++j)
            check(*(elm = ov_mmap_find(mmap, names+i, heights+j))
                  == heights[j]);
    }

    test_val hin = 165, hin2 = 178, hin3 = 188;

#define NAME "John"
    check(!ov_mmap_remove(mmap, NAME, &hnotin));
    check(!ov_mmap_remove(mmap, NAME, &hnotin2));
    check(!ov_mmap_remove(mmap, NAME, &hnotin3));

    check(ov_mmap_remove(mmap, NAME, &hin) == mmap);
    check(ov_mmap_remove(mmap, NAME, &hin2) == mmap);
    check(ov_mmap_remove(mmap, NAME, &hin3) == mmap);

    check(!ov_mmap_remove(mmap, NAME, &hin));
    check(!ov_mmap_remove(mmap, NAME, &hin2));
    check(!ov_mmap_remove(mmap, NAME, &hin3));

    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3);

    test_val hleft = 202, hleft2 = 190;
    check(!ov_mmap_find(mmap, NAME, &hin));
    check(!ov_mmap_find(mmap, NAME, &hin2));
    check(!ov_mmap_find(mmap, NAME, &hin3));
    check(*(elm = ov_mmap_find(mmap, NAME, &hleft)) == hleft);
    check(*(elm = ov_mmap_find(mmap, NAME, &hleft2)) == hleft2);

    check(*(elm = ov_mmap_insert_once(mmap, NAME, &hin)) == hin);
    check(!ov_mmap_insert_once(mmap, NAME, &hin));

    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3+1);
    check(*(elm = ov_mmap_find(mmap, NAME, &hin)) == hin);

    check(ov_mmap_remove(mmap, NAME, &hin) == mmap);
    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3);
    check(!ov_mmap_find(mmap, NAME, &hin));

    check(!ov_mmap_remove(mmap, NAME, &hin));
    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3);
    check(!ov_mmap_find(mmap, NAME, &hin));

    check(ov_mmap_has_key(mmap, NAME));
    check(ov_mmap_remove_by_key(mmap, NAME) == mmap);
    check(!ov_mmap_remove_by_key(mmap, NAME));

    check(!ov_mmap_remove_by_key(mmap, NAME));
    check(!ov_mmap_find(mmap, NAME, &hin));
    check(!ov_mmap_find(mmap, NAME, &hin2));
    check(!ov_mmap_find(mmap, NAME, &hin3));
    check(!ov_mmap_remove(mmap, NAME, &hnotin));
    check(!ov_mmap_remove(mmap, NAME, &hnotin2));
    check(!ov_mmap_remove(mmap, NAME, &hnotin3));

    check(!ov_mmap_has_key(mmap, NAME));
    check(!ov_mmap_num_of_vals(mmap, NAME));
    check(ov_mmap_num_of_keys(mmap) == nm_sz-1);
#undef NAME

#define NAME "Jill"
    check(ov_mmap_has_key(mmap, NAME));
    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz);

    for (int i = 0; i < heig_sz; ++i)
        check(ov_mmap_remove(mmap, NAME, heights+i));

     for (int i = 0; i < heig_sz; ++i)
        check(!ov_mmap_remove(mmap, NAME, heights+i));

    check(!ov_mmap_has_key(mmap, NAME));
    check(!ov_mmap_num_of_vals(mmap, NAME));
    check(ov_mmap_num_of_keys(mmap) == nm_sz-2);
#undef NAME

    check(ov_mmap_num_of_vals(mmap, "Jack") == heig_sz);
    check(ov_mmap_num_of_vals(mmap, "Zzeben") == heig_sz);
    check(*(elm = ov_mmap_find(mmap, "Jack", &hin)) == hin);
    check(*(elm = ov_mmap_find(mmap, "Zzeben", &hin)) == hin);
    check(*(elm = ov_mmap_find(mmap, "Jack", &hin2)) == hin2);
    check(*(elm = ov_mmap_find(mmap, "Zzeben", &hin2)) == hin2);
    check(*(elm = ov_mmap_find(mmap, "Jack", &hin3)) == hin3);
    check(*(elm = ov_mmap_find(mmap, "Zzeben", &hin3)) == hin3);

    check(ov_mmap_remove_by_val(mmap, &hin) == ov_mmap_num_of_keys(mmap));
    check(ov_mmap_remove_by_val(mmap, &hin2) == ov_mmap_num_of_keys(mmap));
    check(ov_mmap_remove_by_val(mmap, &hin3) == ov_mmap_num_of_keys(mmap));

    check(!ov_mmap_remove_by_val(mmap, &hin));
    check(!ov_mmap_remove_by_val(mmap, &hin2));
    check(!ov_mmap_remove_by_val(mmap, &hin3));

    check(ov_mmap_num_of_vals(mmap, "Jack") == heig_sz-3);
    check(ov_mmap_num_of_vals(mmap, "Zzeben") == heig_sz-3);

    check(!ov_mmap_find(mmap, "Jack", &hin));
    check(!ov_mmap_find(mmap, "Zzeben", &hin));
    check(!ov_mmap_find(mmap, "Jack", &hin2));
    check(!ov_mmap_find(mmap, "Zzeben", &hin2));
    check(!ov_mmap_find(mmap, "Jack", &hin3));
    check(!ov_mmap_find(mmap, "Zzeben", &hin3));

    ov_mmap_destroy(mmap);
    return true;
}
//------------------------------------------------------------------------------

static bool test_mapping_str_val(void)
{
    static char names[][MAX_STR] = {
        "John", "Jack", "Jill", "Zzeben"
    };

    static test_val heights[] = {165, 202, 178, 190, 188};

    ov_mmap mmap_, * mmap = &mmap_;
    int key_sz = MAX_STR;
    check(ov_mmap_make(mmap,
        key_sz, (fcomp)strcmp/*compar_key*/, sizeof(test_val), compar_val
        ) == mmap);

    int extra_bytes = key_sz % sizeof(void *);
    int align = (extra_bytes) ? sizeof(void *) - extra_bytes : 0;

    check(align == 3);
    check(ov_mmap_orig_key_size(mmap) == key_sz);
    check((ov_mmap_real_key_size(mmap) % sizeof(void *)) == 0);
    check(ov_mmap_real_key_size(mmap) == key_sz + align + sizeof(ov_set));
    check(ov_mmap_val_size(mmap) == sizeof(test_val));
    check(ov_mmap_val_capcity(mmap) == OV_MMAP_DEFAULT_VAL_CAPACITY);

    int nm_sz = sizeof(names)/sizeof(*names);
    int heig_sz = sizeof(heights)/sizeof(*heights);

    int heigh_sum = 0;
    const test_val * elm;
    check(ov_mmap_num_of_keys(mmap) == 0);
    check(ov_mmap_is_empty(mmap));
    for (int i = 0; i < nm_sz; ++i)
    {
        heigh_sum = 0;
        for (int j = 0; j < heig_sz; ++j)
        {
            check(*(elm = ov_mmap_insert_once(mmap, names+i, heights+j))
                  == heights[j]);
            heigh_sum += heights[j];
        }
    }

    check(gtest.john == 0);
    check(gtest.jack == 0);
    check(gtest.jill == 0);
    check(gtest.zzeben == 0);
    check(ov_mmap_apply(mmap, apply_no_args) == mmap);
    check(gtest.john == heigh_sum);
    check(gtest.jack == heigh_sum);
    check(gtest.jill == heigh_sum);
    check(gtest.zzeben == 0);

    test_str ltest;
    memset(&ltest, 0, sizeof(ltest));
    check(ltest.john == 0);
    check(ltest.jack == 0);
    check(ltest.jill == 0);
    check(ltest.zzeben == 0);
    check(ov_mmap_apply_args(mmap, apply_args, &ltest) == mmap);
    check(ltest.john == heigh_sum);
    check(ltest.jack == heigh_sum);
    check(ltest.jill == 0);
    check(ltest.zzeben == heigh_sum);

    int all_vals = nm_sz * heig_sz;
    for (int i = 0; i < all_vals; ++i)
        check(*(elm = ov_mmap_get_value(mmap, i)) == heights[i % heig_sz]);

    static char names_o[][MAX_STR] = {
        "Jack", "Jill", "John", "Zzeben"
    };

    for (int i = 0; i < nm_sz; ++i)
    {
        check(strcmp(ov_mmap_get_key(mmap, i), (char *)(names_o+i)) == 0);
        c_vector * cv_vals = ov_mmap_get_vals(mmap, names_o+i);
        c_vector * cv_vals2 = ov_mmap_get_vals(mmap, ov_mmap_get_key(mmap, i));
        check(cv_vals == cv_vals2);
    }

    check(ov_mmap_capacity(mmap) == C_VECT_DEFAULT_CAPACITY);

    int n_keys = ov_mmap_num_of_keys(mmap);
    for (int i = 0; i < n_keys; ++i)
    {
        c_vector * vals = ov_mmap_get_vals(mmap, ov_mmap_get_key(mmap, i));
        check(c_vect_capacity(vals) == OV_MMAP_DEFAULT_VAL_CAPACITY*2);
    }

    check(ov_mmap_fit(mmap) == mmap);
    check(ov_mmap_capacity(mmap) == nm_sz);
    for (int i = 0; i < n_keys; ++i)
    {
        c_vector * vals = ov_mmap_get_vals(mmap, ov_mmap_get_key(mmap, i));
        check(c_vect_capacity(vals) == heig_sz);
    }


    check(ov_mmap_num_of_keys(mmap) == nm_sz);
    check(!ov_mmap_is_empty(mmap));

    for (int i = 0; i < nm_sz; ++i)
    {
        c_vector * cv_vals = ov_mmap_get_vals(mmap, names+i);

        check(c_vect_length(cv_vals) == heig_sz);
        check(ov_mmap_num_of_vals(mmap, names+i) == heig_sz);

        for (int j = 0; j < heig_sz; ++j)
            check(*(elm = c_vect_get(cv_vals, j)) == heights[j]);
    }

    static char notin[MAX_STR] = "Herc";
    static char notin2[MAX_STR] = "Xena";
    test_val hnotin = 120, hnotin2 = 210, hnotin3 = -213;

    check(!ov_mmap_get_vals(mmap, notin));
    check(!ov_mmap_get_vals(mmap, notin2));

    for (int i = 0; i < nm_sz; ++i)
    {
        check(!ov_mmap_find(mmap, notin, &hnotin));
        check(!ov_mmap_find(mmap, notin, &hnotin2));
        check(!ov_mmap_find(mmap, notin, &hnotin3));
        check(!ov_mmap_find(mmap, notin2, &hnotin));
        check(!ov_mmap_find(mmap, notin2, &hnotin2));
        check(!ov_mmap_find(mmap, notin2, &hnotin3));
        check(!ov_mmap_find(mmap, names+i, &hnotin));
        check(!ov_mmap_find(mmap, names+i, &hnotin2));
        check(!ov_mmap_find(mmap, names+i, &hnotin3));
    }

    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < heig_sz; ++j)
            check(*(elm = ov_mmap_find(mmap, names+i, heights+j))
                  == heights[j]);
    }

    test_val hin = 165, hin2 = 178, hin3 = 188;

#define NAME "John"
    check(!ov_mmap_remove(mmap, NAME, &hnotin));
    check(!ov_mmap_remove(mmap, NAME, &hnotin2));
    check(!ov_mmap_remove(mmap, NAME, &hnotin3));

    check(ov_mmap_remove(mmap, NAME, &hin) == mmap);
    check(ov_mmap_remove(mmap, NAME, &hin2) == mmap);
    check(ov_mmap_remove(mmap, NAME, &hin3) == mmap);

    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3);

    test_val hleft = 202, hleft2 = 190;
    check(!ov_mmap_find(mmap, NAME, &hin));
    check(!ov_mmap_find(mmap, NAME, &hin2));
    check(!ov_mmap_find(mmap, NAME, &hin3));
    check(*(elm = ov_mmap_find(mmap, NAME, &hleft)) == hleft);
    check(*(elm = ov_mmap_find(mmap, NAME, &hleft2)) == hleft2);

    check(*(elm = ov_mmap_insert_once(mmap, NAME, &hin)) == hin);
    check(!ov_mmap_insert_once(mmap, NAME, &hin));

    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3+1);
    check(*(elm = ov_mmap_find(mmap, NAME, &hin)) == hin);

    check(ov_mmap_remove(mmap, NAME, &hin) == mmap);
    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3);
    check(!ov_mmap_find(mmap, NAME, &hin));

    check(!ov_mmap_remove(mmap, NAME, &hin));
    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz-3);
    check(!ov_mmap_find(mmap, NAME, &hin));

    check(ov_mmap_has_key(mmap, NAME));
    check(ov_mmap_remove_by_key(mmap, NAME) == mmap);
    check(!ov_mmap_has_key(mmap, NAME));
    check(!ov_mmap_num_of_vals(mmap, NAME));
    check(ov_mmap_num_of_keys(mmap) == nm_sz-1);

#undef NAME
#define NAME "Jill"

    check(ov_mmap_has_key(mmap, NAME));
    check(ov_mmap_num_of_vals(mmap, NAME) == heig_sz);

    for (int i = 0; i < heig_sz; ++i)
        check(ov_mmap_remove(mmap, NAME, heights+i));

    check(!ov_mmap_has_key(mmap, NAME));
    check(!ov_mmap_num_of_vals(mmap, NAME));
    check(ov_mmap_num_of_keys(mmap) == nm_sz-2);

    check(ov_mmap_num_of_vals(mmap, "Jack") == heig_sz);
    check(ov_mmap_num_of_vals(mmap, "Zzeben") == heig_sz);
    check(*(elm = ov_mmap_find(mmap, "Jack", &hin)) == hin);
    check(*(elm = ov_mmap_find(mmap, "Zzeben", &hin)) == hin);
    check(*(elm = ov_mmap_find(mmap, "Jack", &hin2)) == hin2);
    check(*(elm = ov_mmap_find(mmap, "Zzeben", &hin2)) == hin2);
    check(*(elm = ov_mmap_find(mmap, "Jack", &hin3)) == hin3);
    check(*(elm = ov_mmap_find(mmap, "Zzeben", &hin3)) == hin3);

    check(ov_mmap_remove_by_val(mmap, &hin) == ov_mmap_num_of_keys(mmap));
    check(ov_mmap_remove_by_val(mmap, &hin2) == ov_mmap_num_of_keys(mmap));
    check(ov_mmap_remove_by_val(mmap, &hin3) == ov_mmap_num_of_keys(mmap));

    check(ov_mmap_num_of_vals(mmap, "Jack") == heig_sz-3);
    check(ov_mmap_num_of_vals(mmap, "Zzeben") == heig_sz-3);

    check(!ov_mmap_find(mmap, "Jack", &hin));
    check(!ov_mmap_find(mmap, "Zzeben", &hin));
    check(!ov_mmap_find(mmap, "Jack", &hin2));
    check(!ov_mmap_find(mmap, "Zzeben", &hin2));
    check(!ov_mmap_find(mmap, "Jack", &hin3));
    check(!ov_mmap_find(mmap, "Zzeben", &hin3));

    ov_mmap_destroy_null(mmap);
    check(!mmap);
    return true;
}
//------------------------------------------------------------------------------

static bool test_copy(void)
{
    static char names[][MAX_STR] = {
        "John", "Jack", "Jill", "Zzeben"
    };

    static test_val heights[] = {165, 202, 178, 190, 188};

    ov_mmap mmap_, * mmap = &mmap_;
    int key_sz = MAX_STR;
    check(ov_mmap_make(mmap,
        key_sz, (fcomp)strcmp/*compar_key*/,
        sizeof(test_val), compar_val
        ) == mmap);

    int nm_sz = sizeof(names)/sizeof(*names);
    int heig_sz = sizeof(heights)/sizeof(*heights);

    ov_mmap cpy_, * cpy = &cpy_;
    memset(&cpy_, 0xFF, sizeof(cpy_));

    ov_mmap zout;
    memset(&zout, 0, sizeof(zout));
    check(memcmp(cpy, &zout, sizeof(zout)) != 0);

    bool success = true;
    cpy_ = ov_mmap_copy(mmap, &success);
    check(!success);

    check(memcmp(cpy, &zout, sizeof(zout)) == 0);

    const test_val * elm;
    check(ov_mmap_num_of_keys(mmap) == 0);
    check(ov_mmap_is_empty(mmap));
    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < heig_sz; ++j)
            check(*(elm = ov_mmap_insert(mmap, names+i, heights+j))
                  == heights[j]);
    }
    check(ov_mmap_num_of_keys(mmap) == nm_sz);

    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < heig_sz; ++j)
            check(*(elm = ov_mmap_find(mmap, names+i, heights+j))
                  == heights[j]);
    }

    success = false;
    check(cpy->val_compar != mmap->val_compar);
    check(cpy->orig_key_size != mmap->orig_key_size);
    check(cpy->real_key_size != mmap->real_key_size);
    check(cpy->val_size != mmap->val_size);
    check(cpy->val_capacity != mmap->val_capacity);

    cpy_ = ov_mmap_copy(mmap, &success);

    check(success);

    check(cpy->val_compar == mmap->val_compar);
    check(cpy->orig_key_size == mmap->orig_key_size);
    check(cpy->real_key_size == mmap->real_key_size);
    check(cpy->val_size == mmap->val_size);
    check(cpy->val_capacity == mmap->val_capacity);

    const test_val * elm2;
    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < heig_sz; ++j)
        {
            check(*(elm = ov_mmap_find(cpy, names+i, heights+j))
                  == heights[j]);
            check(*(elm2 = ov_mmap_find(cpy, names+i, heights+j))
                  == *(elm = ov_mmap_find(mmap, names+i, heights+j)));
        }
    }

    check(ov_mmap_num_of_keys(&cpy_) == ov_mmap_num_of_keys(mmap));
    check(ov_mmap_num_of_keys(cpy) == ov_mmap_num_of_keys(&cpy_));
    int keys = ov_mmap_num_of_keys(mmap);
    for (int i = 0; i < keys; ++i)
        check(strcmp(ov_mmap_get_key(cpy, i), ov_mmap_get_key(mmap, i)) == 0);

    int i = 0;
    while ((elm = ov_mmap_get_value(cpy, i)))
    {
        check(*elm == *(elm2 = ov_mmap_get_value(mmap, i)));
        ++i;
    }

    ov_mmap_destroy(mmap);

    for (int i = 0; i < nm_sz; ++i)
    {
        for (int j = 0; j < heig_sz; ++j)
            check(*(elm = ov_mmap_find(cpy, names+i, heights+j))
                  == heights[j]);
    }

    ov_mmap cpy2 = ov_mmap_copy(cpy, NULL);
    ov_mmap_destroy(&cpy2);
    ov_mmap_destroy(cpy);
    return true;
}
//------------------------------------------------------------------------------

static int passed,failed;
void run_tests_ov_mmap(void)
{
    int i, end = sizeof(tests)/sizeof(*tests);

    passed = 0;
    for (i = 0; i < end; ++i)
        if (tests[i]())
            ++passed;

    if (passed != end)
        putchar('\n');

    failed = end - passed;
    report(passed, failed);
    return;
}
//------------------------------------------------------------------------------

int tov_mmap_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tov_mmap_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
