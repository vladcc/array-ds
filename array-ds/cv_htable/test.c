#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cv_htable.h"
#include "test.h"

static bool test_create_destroy(void);
static bool test_insert_lookup_remove_clear(void);
static bool test_copy(void);
static bool test_iterate(void);

static ftest tests[] = {
    test_create_destroy,
    test_insert_lookup_remove_clear,
    test_copy,
    test_iterate
};
//------------------------------------------------------------------------------

static int compar(const void * k1, const void * k2)
{
    int i1 = *((int *)k1);
    int i2 = *((int *)k2);
    return (i1 > i2) - (i1 < i2);
}

static unsigned int ab_hash(const unsigned char * what, int length)
{
    return *(unsigned int *)what;
}
//------------------------------------------------------------------------------

static bool test_iterate(void)
{
    cv_htbl cvht_, * cvht = &cvht_;
    c_vector * the_tbl = &(cvht_.the_tbl);

    int el_size = sizeof(int);
    const int cap = 10;

    check(cv_htbl_make_cap_hash(cvht, el_size, compar, cap, ab_hash) == cvht);
    check(cv_htbl_is_empty(cvht));
    check(cvht->elem_inside == 0);
    check(cvht->elem_max == (cap*CVH_DEFAULT_LOAD_FACT));
    check(cv_htbl_elem_max(cvht) == (cap*CVH_DEFAULT_LOAD_FACT));
    check(cv_htbl_elem_count(cvht) == 0);
    check(cvht->hash_fun == ab_hash);
    check(cvht->elem_size == el_size);
    check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
    check(c_vect_capacity(the_tbl) == cap);

    int elem_max = cap*CVH_DEFAULT_LOAD_FACT;
    for (int i = 0; i < elem_max; ++i)
        cv_htbl_insert(cvht, &i);
    check(cvht->elem_inside == elem_max);
    check(cv_htbl_elem_count(cvht) == elem_max);
    check(!cv_htbl_is_empty(cvht));
    check(40 == elem_max);

    int inside[] = {
        0, 10, 20, 30,
        1, 11, 21, 31,
        2, 12, 22, 32,
        3, 13, 23, 33,
        4, 14, 24, 34,
        5, 15, 25, 35,
        6, 16, 26, 36,
        7, 17, 27, 37,
        8, 18, 28, 38,
        9, 19, 29, 39,
    };

    cv_htbl_iterator it = cv_htbl_iterator_init();
    const int * pdata;
    int i = 0;
    while ((pdata = cv_htbl_iterate(cvht, it)))
        check(*pdata == inside[i++]);

    int rem_size = -1;
    {
        int remove[] = {0, 10, 20, 30};
        rem_size = sizeof(remove)/sizeof(*remove);
        for (int i = 0; i < rem_size; ++i)
            check(cv_htbl_remove(cvht, remove+i) == cvht);
    }
    check(cvht->elem_inside == elem_max-4);

    it = cv_htbl_iterator_init();
    i = 0;
    while ((pdata = cv_htbl_iterate(cvht, it)))
        check(*pdata == (inside+4)[i++]);

    rem_size = -1;
    {
        int remove[] = {5, 15, 25, 35, 9, 19, 29, 39,};
        rem_size = sizeof(remove)/sizeof(*remove);
        for (int i = 0; i < rem_size; ++i)
            check(cv_htbl_remove(cvht, remove+i) == cvht);
    }
    check(cvht->elem_inside == elem_max-12);

    int new_inside[] = {
        1, 11, 21, 31,
        2, 12, 22, 32,
        3, 13, 23, 33,
        4, 14, 24, 34,
        6, 16, 26, 36,
        7, 17, 27, 37,
        8, 18, 28, 38,
    };

    it = cv_htbl_iterator_init();
    i = 0;
    while ((pdata = cv_htbl_iterate(cvht, it)))
        check(*pdata == new_inside[i++]);

    int ins = 101;
    cv_htbl_insert(cvht, &ins);

    int new_inside2[] = {
        1, 11, 21, 31, 101,
        2, 12, 22, 32,
        3, 13, 23, 33,
        4, 14, 24, 34,
        6, 16, 26, 36,
        7, 17, 27, 37,
        8, 18, 28, 38,
    };

    it = cv_htbl_iterator_init();
    i = 0;
    while ((pdata = cv_htbl_iterate(cvht, it)))
        check(*pdata == new_inside2[i++]);

    ins = 305;
    cv_htbl_insert(cvht, &ins);
    ins = 67;
    cv_htbl_insert(cvht, &ins);
    ins = 107;
    cv_htbl_insert(cvht, &ins);

    int new_inside3[] = {
        1, 11, 21, 31, 101,
        2, 12, 22, 32,
        3, 13, 23, 33,
        4, 14, 24, 34,
        305,
        6, 16, 26, 36,
        7, 17, 27, 37, 67, 107,
        8, 18, 28, 38,
    };

    it = cv_htbl_iterator_init();
    i = 0;
    while ((pdata = cv_htbl_iterate(cvht, it)))
        check(*pdata == new_inside3[i++]);
    check(40 == elem_max);

    int offst = 500;
    for (int i = 500, end = elem_max+offst; i < end; ++i)
        cv_htbl_insert(cvht, &i);
    check(80 == cvht->elem_max);

    int all[] = {
        500, 510, 520, 530,
        1, 11, 21, 31, 101, 501, 511, 521, 531,
        2, 12, 22, 32, 502, 512, 522, 532,
        3, 13, 23, 33, 503, 513, 523, 533,
        4, 14, 24, 34, 504, 514, 524, 534,
        305, 505, 515, 525, 535,
        6, 16, 26, 36, 506, 516, 526, 536,
        7, 17, 27, 37, 67, 107, 507, 517, 527, 537,
        8, 18, 28, 38, 508, 518, 528, 538,
        509, 519, 529, 539,
    };

    c_vector vect_tbl;
    c_vect_make(&vect_tbl, sizeof(int), compar);

    it = cv_htbl_iterator_init();
    i = 0;
    while ((pdata = cv_htbl_iterate(cvht, it)))
        c_vect_push(&vect_tbl, pdata);
    c_vect_sort(&vect_tbl);

    c_vector vect;
    c_vect_make(&vect, sizeof(int), compar);

    for (int i = 0; i < sizeof(all)/sizeof(*all); ++i)
        c_vect_push(&vect, all+i);
    c_vect_sort(&vect);


    int vect_len = c_vect_length(&vect);
    check(vect_len == c_vect_length(&vect_tbl));
    int * pdata2;
    for (int i = 0; i < vect_len; ++i)
    {
        pdata = c_vect_get(&vect, i);
        pdata2 = c_vect_get(&vect_tbl, i);
        check(*pdata == *pdata2);
    }

    c_vect_destroy(&vect_tbl);
    c_vect_destroy(&vect);
    cv_htbl_destroy(cvht);
    check(cv_htbl_is_empty(cvht));
    return true;
}
//------------------------------------------------------------------------------

static bool test_copy(void)
{
    cv_htbl cvht_, * cvht = &cvht_;
    c_vector * the_tbl = &(cvht_.the_tbl);

    int el_size = sizeof(int);
    const int cap = 10;

    check(cv_htbl_make_cap_hash(cvht, el_size, compar, cap, ab_hash) == cvht);
    check(cvht->elem_inside == 0);
    check(cvht->elem_max == (cap*CVH_DEFAULT_LOAD_FACT));
    check(cvht->hash_fun == ab_hash);
    check(cvht->elem_size == el_size);
    check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
    check(c_vect_capacity(the_tbl) == cap);

    int elem_max = cap*CVH_DEFAULT_LOAD_FACT;
    for (int i = 0; i < elem_max; ++i)
        cv_htbl_insert(cvht, &i);
    check(cvht->elem_inside == elem_max);

    c_vector * bucket;
    for (int i = 0; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }

    bool cpy_ok = false;
    cv_htbl cpy_ = cv_htbl_copy(cvht, &cpy_ok);
    cv_htbl * cpy = &cpy_;
    check(cpy_ok);
    check(cpy->elem_inside == cvht->elem_inside);
    check(cpy->elem_max == cvht->elem_max);
    check(cpy->hash_fun == cvht->hash_fun);
    check(cpy->elem_size == cvht->elem_size);


    check(cv_htbl_elem_count(cvht) == cvht->elem_inside);
    check(cv_htbl_elem_max(cvht) == cvht->elem_max);

    c_vector * the_tbl_cpy = &(cpy->the_tbl);
    c_vector * bucket_cpy;
    const int * pdata, * pdata_cpy;
    for (int i = 0; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        bucket_cpy = c_vect_get(the_tbl_cpy, i);

        int bucket_len = c_vect_length(bucket);
        check(bucket_len == c_vect_length(bucket_cpy));
        for (int j = 0; j < bucket_len; ++j)
            check(*(pdata = c_vect_get(bucket, j)) ==
                *(pdata_cpy = c_vect_get(bucket_cpy, j)));
    }

    for (int i = 0; i < cap; ++i)
        check(*(pdata_cpy = cv_htbl_lookup(cpy, &i)) == i);

    cv_htbl_destroy(cvht);

    for (int i = 0; i < cap; ++i)
        check(*(pdata_cpy = cv_htbl_lookup(cpy, &i)) == i);

    cv_htbl_destroy(cpy);

    {
        cv_htbl cvht_, * cvht = &cvht_;
        c_vector * the_tbl = &(cvht_.the_tbl);

        int el_size = sizeof(int);
        const int cap = 10;

        check(cv_htbl_make_cap_hash(cvht, el_size, compar, cap, ab_hash) == cvht);
        check(cvht->elem_inside == 0);
        check(cvht->elem_max == (cap*CVH_DEFAULT_LOAD_FACT));
        check(cvht->hash_fun == ab_hash);
        check(cvht->elem_size == el_size);
        check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
        check(c_vect_capacity(the_tbl) == cap);

        int elem_max = cap*CVH_DEFAULT_LOAD_FACT;
        for (int i = 0; i < elem_max; ++i)
            cv_htbl_insert(cvht, &i);
        check(cvht->elem_inside == elem_max);
        check(40 == elem_max);

        int inside[] = {
            0, 10, 20, 30,
            1, 11, 21, 31,
            2, 12, 22, 32,
            3, 13, 23, 33,
            4, 14, 24, 34,
            5, 15, 25, 35,
            6, 16, 26, 36,
            7, 17, 27, 37,
            8, 18, 28, 38,
            9, 19, 29, 39,
        };

        cv_htbl_iterator it = cv_htbl_iterator_init();
        const int * pdata;
        int i = 0;
        while ((pdata = cv_htbl_iterate(cvht, it)))
            check(*pdata == inside[i++]);

        int rem_size = -1;
        {
            int remove[] = {0, 10, 20, 30};
            rem_size = sizeof(remove)/sizeof(*remove);
            for (int i = 0; i < rem_size; ++i)
                check(cv_htbl_remove(cvht, remove+i) == cvht);

            int remove2[] = {9, 19, 29, 39};
            rem_size = sizeof(remove2)/sizeof(*remove2);
            for (int i = 0; i < rem_size; ++i)
                check(cv_htbl_remove(cvht, remove2+i) == cvht);


            int remove3[] = {6, 16, 26, 36};
            rem_size = sizeof(remove3)/sizeof(*remove3);
            for (int i = 0; i < rem_size; ++i)
                check(cv_htbl_remove(cvht, remove3+i) == cvht);
        }
        check(cvht->elem_inside == elem_max-12);

        int ins = 808;
        cv_htbl_insert(cvht, &ins);

        bool cpy_ok = false;
        cv_htbl cpy_ = cv_htbl_copy(cvht, &cpy_ok);
        cv_htbl * cpy = &cpy_;
        check(cpy_ok);
        check(cpy->elem_inside == cvht->elem_inside);
        check(cpy->elem_max == cvht->elem_max);
        check(cpy->hash_fun == cvht->hash_fun);
        check(cpy->elem_size == cvht->elem_size);


        check(cv_htbl_elem_count(cvht) == cvht->elem_inside);
        check(cv_htbl_elem_max(cvht) == cvht->elem_max);

        c_vector * the_tbl_cpy = &(cpy->the_tbl);
        c_vector * bucket_cpy;
        const int * pdata_cpy;
        for (int i = 0; i < cap; ++i)
        {
            bucket = c_vect_get(the_tbl, i);
            bucket_cpy = c_vect_get(the_tbl_cpy, i);

            int bucket_len = c_vect_length(bucket);
            check(bucket_len == c_vect_length(bucket_cpy));
            for (int j = 0; j < bucket_len; ++j)
                check(*(pdata = c_vect_get(bucket, j)) ==
                    *(pdata_cpy = c_vect_get(bucket_cpy, j)));
        }

        int inside2[] = {
            //0, 10, 20, 30,
            1, 11, 21, 31,
            2, 12, 22, 32,
            3, 13, 23, 33,
            4, 14, 24, 34,
            5, 15, 25, 35,
            //6, 16, 26, 36,
            7, 17, 27, 37,
            8, 18, 28, 38, 808
            //9, 19, 29, 39,
        };

        for (int i = 0; i < sizeof(inside2)/sizeof(*inside2); ++i)
            check(*(pdata_cpy = cv_htbl_lookup(cpy, inside2+i)) == inside2[i]);

        cv_htbl_destroy(cvht);

        for (int i = 0; i < sizeof(inside2)/sizeof(*inside2); ++i)
            check(*(pdata_cpy = cv_htbl_lookup(cpy, inside2+i)) == inside2[i]);

        cv_htbl_destroy(cpy);
    }

    return true;
}
//------------------------------------------------------------------------------

static bool test_insert_lookup_remove_clear(void)
{
    cv_htbl cvht_, * cvht = &cvht_;
    c_vector * the_tbl = &(cvht_.the_tbl);

    int el_size = sizeof(int);
    const int cap = 10;

    check(cv_htbl_make_cap_hash(cvht, el_size, compar, cap, ab_hash) == cvht);
    check(cvht->elem_inside == 0);
    check(cvht->elem_max == (cap*CVH_DEFAULT_LOAD_FACT));
    check(cvht->hash_fun == ab_hash);
    check(cvht->elem_size == el_size);
    check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
    check(c_vect_capacity(the_tbl) == cap);

    int lup = 1234;
    check(!cv_htbl_lookup(cvht, &lup));

    lup = 0;
    check(!cv_htbl_lookup(cvht, &lup));

    lup = -1234;
    check(!cv_htbl_lookup(cvht, &lup));

    c_vector * bucket = NULL;
    for (int i = 0; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_is_empty(bucket));
        check(c_vect_elem_size(bucket) == el_size);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }

    const int * pdata = NULL;
    int end = CVH_DEFAULT_LOAD_FACT;
    for (int i = 0, j = 0; i < end; ++i)
    {
        j = i*10+0;
        check(!cv_htbl_lookup(cvht, &j));
        check(*(pdata = cv_htbl_insert(cvht, &j)) == j);
        check(*(pdata = cv_htbl_lookup(cvht, &j)) == j);
    }

    bucket = c_vect_get(the_tbl, 0);
    check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT);
    for (int i = 0; i < end; ++i)
    {
        int * pdata = c_vect_get(bucket, i);
        check(*pdata == i*10+0);
    }

    for (int i = 1; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_is_empty(bucket));
        check(c_vect_elem_size(bucket) == el_size);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }
    check(cvht->elem_inside == CVH_DEFAULT_LOAD_FACT);

    for (int i = 0, j = 0; i < end; ++i)
    {
        j = i*10+1;
        check(!cv_htbl_lookup(cvht, &j));
        check(*(pdata = cv_htbl_insert(cvht, &j)) == j);
        check(*(pdata = cv_htbl_lookup(cvht, &j)) == j);
    }

    bucket = c_vect_get(the_tbl, 1);
    check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT);
    for (int i = 0; i < end; ++i)
    {
        pdata = c_vect_get(bucket, i);
        check(*pdata == i*10+1);
    }

    for (int i = 2; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_is_empty(bucket));
        check(c_vect_elem_size(bucket) == el_size);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }
    check(cvht->elem_inside == CVH_DEFAULT_LOAD_FACT*2);

    // does not insert again
    for (int i = 0, j = 0; i < end; ++i)
    {
        j = i*10+1;
        check(*(pdata = cv_htbl_insert(cvht, &j)) == j);
    }

    bucket = c_vect_get(the_tbl, 1);
    check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT);
    for (int i = 0; i < end; ++i)
    {
        pdata = c_vect_get(bucket, i);
        check(*pdata == i*10+1);
    }

    for (int i = 2; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_is_empty(bucket));
        check(c_vect_elem_size(bucket) == el_size);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }
    check(cvht->elem_inside == CVH_DEFAULT_LOAD_FACT*2);

    int ins = 50;
    check(*(pdata = cv_htbl_insert(cvht, &ins)) == ins);
    bucket = c_vect_get(the_tbl, 0);
    check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT+1);
    check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT*2);
    check(cvht->elem_inside == CVH_DEFAULT_LOAD_FACT*2+1);

    ins = 60;
    check(*(pdata = cv_htbl_insert(cvht, &ins)) == ins);
    bucket = c_vect_get(the_tbl, 0);
    check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT+2);
    check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT*2);
    check(cvht->elem_inside == CVH_DEFAULT_LOAD_FACT*2+2);

    bucket = c_vect_get(the_tbl, 3);
    check(c_vect_is_empty(bucket));
    check(c_vect_elem_size(bucket) == el_size);
    check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);

    ins = 63;
    check(*(pdata = cv_htbl_insert(cvht, &ins)) == ins);
    bucket = c_vect_get(the_tbl, 3);
    check(c_vect_length(bucket) == 1);
    check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    check(cvht->elem_inside == CVH_DEFAULT_LOAD_FACT*2+3);

    cv_htbl_destroy(cvht);

    check(cv_htbl_make_cap_hash(cvht, el_size, compar, cap, ab_hash) == cvht);

    check(cvht->elem_inside == 0);
    check(cvht->elem_max == (cap*CVH_DEFAULT_LOAD_FACT));
    check(cvht->hash_fun == ab_hash);
    check(cvht->elem_size == el_size);
    check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
    check(c_vect_capacity(the_tbl) == cap);

    int elem_max = cap*CVH_DEFAULT_LOAD_FACT;
    for (int i = 0; i < elem_max; ++i)
        cv_htbl_insert(cvht, &i);
    check(cvht->elem_inside == elem_max);

    for (int i = 0; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }

    for (int i = 0; i < elem_max; ++i)
        cv_htbl_insert(cvht, &i);
    check(cvht->elem_inside == elem_max);

    for (int i = 0; i < cap; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_length(bucket) == CVH_DEFAULT_LOAD_FACT);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }

    ins = 40;
    check(*(pdata = cv_htbl_insert(cvht, &ins)) == ins);
    check(cvht->elem_inside == elem_max+1);
    check(cvht->elem_inside == ins+1);
    check(cvht->elem_max == elem_max*2);

    for (int i = 0; i <= ins; ++i)
        check(*(pdata = cv_htbl_lookup(cvht, &i)) == i);
    check(cvht->elem_inside == elem_max+1);
    check(cvht->elem_inside == ins+1);
    check(cvht->elem_max == elem_max*2);

    the_tbl = &(cvht->the_tbl);
    check(c_vect_length(the_tbl) == cap*2); //10*2
    end = cap*2;

    bucket = c_vect_get(the_tbl, 0);
    check(c_vect_length(bucket) == 3);
    for (int i = 1; i < end; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_length(bucket) == 2);
    }

    int rem = 123;
    cv_htbl_remove(cvht, &rem);
    check(cvht->elem_inside == elem_max+1);
    check(cvht->elem_max == elem_max*2);

    rem = -123;
    cv_htbl_remove(cvht, &rem);
    check(cvht->elem_inside == elem_max+1);
    check(cvht->elem_max == elem_max*2);

    rem = 40;
    cv_htbl_remove(cvht, &rem);
    check(cvht->elem_inside == elem_max);
    check(cvht->elem_max == elem_max*2);

    check(!cv_htbl_is_empty(cvht));
    for (int i = 0; i < elem_max; ++i)
        cv_htbl_remove(cvht, &i);
    check(cvht->elem_inside == 0);
    check(cvht->elem_max == elem_max*2);
    check(cv_htbl_is_empty(cvht));

    int tbl_len = c_vect_length(the_tbl);
    for (int i = 0; i < tbl_len; ++i)
        check(c_vect_is_empty(c_vect_get(the_tbl, i)));

    check(cv_htbl_is_empty(cvht));
    for (int i = 0; i < elem_max; ++i)
    {
        cv_htbl_insert(cvht, &i);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }
    check(c_vect_length(the_tbl) == tbl_len);
    check(cvht->elem_inside == elem_max);
    check(!cv_htbl_is_empty(cvht));

    check(cv_htbl_clear(cvht) == cvht);
    for (int i = 0; i < tbl_len; ++i)
    {
        bucket = c_vect_get(the_tbl, i);
        check(c_vect_is_empty(bucket));
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
    }

    check(cv_htbl_is_empty(cvht));

    cv_htbl_destroy(cvht);
    return true;
}
//------------------------------------------------------------------------------

static bool test_create_destroy(void)
{
    cv_htbl cvht_, * cvht = &cvht_;
    memset(cvht, 0xFF, sizeof(*cvht));

    cv_htbl zout;
    memset(&zout, 0, sizeof(zout));

    c_vector * the_tbl = &(cvht_.the_tbl);

    int el_size = sizeof(int);
    int cap = 7;

    check(memcmp(&zout, cvht, sizeof(cv_htbl)) != 0);
    check(!cv_htbl_make_cap(cvht, 0, compar, cap));
    check(memcmp(&zout, cvht, sizeof(cv_htbl)) == 0);

    memset(cvht, 0xFF, sizeof(*cvht));
    check(memcmp(&zout, cvht, sizeof(cv_htbl)) != 0);
    check(!cv_htbl_make_cap(cvht, -123, compar, cap));
    check(memcmp(&zout, cvht, sizeof(cv_htbl)) == 0);

    check(cv_htbl_make_cap(cvht, el_size, compar, cap) == cvht);
    check(cvht->elem_inside == 0);
    check(cvht->elem_max == (cap*CVH_DEFAULT_LOAD_FACT));
    check(cvht->hash_fun == cv_htbl_get_default_hash());
    check(cvht->elem_size == el_size);
    check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
    check(c_vect_capacity(the_tbl) == cap);

    for (int i = 0; i < cap; ++i)
    {
        c_vector * bucket = c_vect_get(the_tbl, i);
        check(c_vect_length(bucket) == 0);
        check(c_vect_capacity(bucket) == CVH_DEFAULT_LOAD_FACT);
        check(c_vect_compar(bucket) == compar);
    }

    check(memcmp(&zout, cvht, sizeof(cv_htbl)) != 0);
    cv_htbl_destroy(cvht);
    check(memcmp(&zout, cvht, sizeof(cv_htbl)) == 0);

    check(cv_htbl_make(cvht, el_size, compar) == cvht);
    check(cvht->elem_inside == 0);
    check(cvht->elem_size == el_size);
    check(cvht->elem_max == (CVH_DEFAULT_CAPACITY*CVH_DEFAULT_LOAD_FACT));
    check(cvht->hash_fun == cv_htbl_get_default_hash());
    check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
    check(c_vect_capacity(the_tbl) == CVH_DEFAULT_CAPACITY);

    cv_htbl_destroy(cvht);

    check(cv_htbl_make_cap_hash(cvht, el_size, compar, cap, ab_hash) == cvht);
    check(cvht->elem_inside == 0);
    check(cvht->elem_size == el_size);
    check(cvht->elem_max == (cap*CVH_DEFAULT_LOAD_FACT));
    check(cvht->hash_fun == ab_hash);
    check(c_vect_length(the_tbl) == c_vect_capacity(the_tbl));
    check(c_vect_capacity(the_tbl) == cap);

    cv_htbl_destroy(cvht);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_cv_htbl(void)
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

int tcv_htbl_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tcv_htbl_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
