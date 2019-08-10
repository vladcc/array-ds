#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "oa_htable.h"
#include "test.h"

#define bit_taken(i)    ((i)*2)
#define bit_vacant(i)   (((i)*2)+1)

static bool test_make_destroy(void);
static bool test_insert_remove(void);
static bool test_clear(void);
static bool test_lookup(void);
static bool test_iterate(void);
static bool test_copy(void);

static ftest tests[] = {
    test_make_destroy,
    test_insert_remove,
    test_clear,
    test_lookup,
    test_iterate,
    test_copy
};

//------------------------------------------------------------------------------

static int compar(const void * k1, const void * k2)
{
    int i1 = *((int *)k1);
    int i2 = *((int *)k2);
    return (i1 > i2) - (i1 < i2);
}
//------------------------------------------------------------------------------

static unsigned int mhash(const unsigned char * what, int length)
{
    return 2;
}
//------------------------------------------------------------------------------

static bool test_make_destroy(void)
{
    oa_htbl oah_, * oah = &oah_, zout;
    c_vector * the_tbl;
    bit_vector * is_taken;

    int el_size = sizeof(int);
    int cap = 7;
    int all_elems = round((double)cap / (double)OAH_DEFAULT_LOAD_FACT);
    {
        memset(oah, 0xFF, sizeof(oa_htbl));
        memset(&zout, 0, sizeof(oa_htbl));

        check(memcmp(oah, &zout, sizeof(oa_htbl)) != 0);
        check(!oa_htbl_make_cap_hash(oah, el_size, compar, -123, mhash));
        check(memcmp(oah, &zout, sizeof(oa_htbl)) == 0);

        memset(oah, 0xFF, sizeof(oa_htbl));
        check(memcmp(oah, &zout, sizeof(oa_htbl)) != 0);
        check(!oa_htbl_make_cap_hash(oah, el_size, compar, 0, mhash));
        check(memcmp(oah, &zout, sizeof(oa_htbl)) == 0);

        memset(oah, 0xFF, sizeof(oa_htbl));
        check(memcmp(oah, &zout, sizeof(oa_htbl)) != 0);
        check(!oa_htbl_make_cap_hash(oah, 0, compar, cap, mhash));
        check(memcmp(oah, &zout, sizeof(oa_htbl)) == 0);

        memset(oah, 0xFF, sizeof(oa_htbl));
        check(memcmp(oah, &zout, sizeof(oa_htbl)) != 0);
        check(!oa_htbl_make_cap_hash(oah, -123, compar, cap, mhash));
        check(memcmp(oah, &zout, sizeof(oa_htbl)) == 0);

        check(oa_htbl_make_cap_hash(oah, el_size, compar, cap, mhash) == oah);

        the_tbl = &(oah->the_tbl);
        is_taken = &(oah->is_taken);

        check(c_vect_elem_size(the_tbl) == el_size);
        check(c_vect_compar(the_tbl) == compar);
        check(c_vect_capacity(the_tbl) == all_elems);
        check(c_vect_length(the_tbl) == all_elems);
        check(bit_vect_num_of_bits(is_taken) == all_elems*2);

        check(oah->hash_fun == mhash);
        check(oah->elem_inside == 0);
        check(oah->elem_max == cap);

        check(memcmp(oah, &zout, sizeof(oa_htbl)) != 0);
        oa_htbl_destroy(oah);
        check(memcmp(oah, &zout, sizeof(oa_htbl)) == 0);
    }

    {
        int cap = 0;
        check(!oa_htbl_make_cap_hash(oah, el_size, compar, cap, mhash));
    }


    {
        check(oa_htbl_make_hash(oah, el_size, compar, mhash) == oah);

        the_tbl = &(oah->the_tbl);
        is_taken = &(oah->is_taken);

        all_elems = round(
            (double)OAH_DEFAULT_CAPACITY / (double)OAH_DEFAULT_LOAD_FACT);

        check(c_vect_elem_size(the_tbl) == el_size);
        check(c_vect_compar(the_tbl) == compar);
        check(c_vect_capacity(the_tbl) == all_elems);
        check(c_vect_length(the_tbl) == all_elems);
        check(bit_vect_num_of_bits(is_taken) == all_elems*2);
        check(oah->hash_fun == mhash);
        check(oah->elem_inside == 0);
        check(oah->elem_max == OAH_DEFAULT_CAPACITY);

        oa_htbl_destroy(oah);
        check(memcmp(oah, &zout, sizeof(oa_htbl)) == 0);
    }

    {
        check(oa_htbl_make(oah, el_size, compar) == oah);

        the_tbl = &(oah->the_tbl);
        is_taken = &(oah->is_taken);

        check(c_vect_elem_size(the_tbl) == el_size);
        check(c_vect_compar(the_tbl) == compar);
        check(c_vect_capacity(the_tbl) == all_elems);
        check(c_vect_length(the_tbl) == all_elems);
        check(bit_vect_num_of_bits(is_taken) == all_elems*2);
        check(oah->hash_fun == oa_htbl_get_default_hash());
        check(oah->elem_inside == 0);
        check(oah->elem_max == OAH_DEFAULT_CAPACITY);

        oa_htbl_destroy_null(oah);
        check(!oah);
    }

    return true;
}
//------------------------------------------------------------------------------

static unsigned int ab_hash(const unsigned char * what, int length)
{
    return *(unsigned int *)what;
}

static bool test_insert_remove(void)
{
    oa_htbl oah_, * oah = &oah_;
    c_vector * the_tbl;
    bit_vector * is_taken;

    int el_size = sizeof(int);
    int cap = 7;
    int all_elems = round(
            (double)cap / (double)OAH_DEFAULT_LOAD_FACT);
    check(all_elems == 10);

    int insert = 5;
    check((ab_hash((unsigned char *)&insert, 0) % cap) == 5);

    {
        check(oa_htbl_make_cap_hash(oah, el_size, compar, cap, ab_hash) == oah);

        the_tbl = &(oah->the_tbl);
        is_taken = &(oah->is_taken);

        c_vect_zero_out(the_tbl);
        check(c_vect_elem_size(the_tbl) == el_size);
        check(c_vect_compar(the_tbl) == compar);
        check(c_vect_capacity(the_tbl) == all_elems);
        check(c_vect_length(the_tbl) == all_elems);
        check(bit_vect_num_of_bits(is_taken) == all_elems*2);

        check(oah->hash_fun == ab_hash);
        check(oah->elem_inside == 0);
        check(oa_htbl_elem_count(oah) == 0);
        check(oah->elem_max == cap);
        check(oa_htbl_elem_max(oah) == cap);

        const int * pdata = NULL;
        int bit_val = 0;
        for (int i = 0; i < cap; ++i)
        {
            check(*(pdata = c_vect_get(the_tbl, i)) == 0);
            check(bit_vect_get(is_taken, i, &bit_val));
            check(!bit_val);
        }

        for (int i = 0; i < cap; ++i)
        {
            check(*(pdata = oa_htbl_insert(oah, &i)) == i);
            check(*(pdata = c_vect_get(the_tbl, i)) == i);
            check(bit_vect_get(is_taken, i*2, &bit_val));
            check(bit_val);
        }

        check(oah->elem_inside == cap);
        check(oa_htbl_elem_count(oah) == cap);
        check(oah->elem_max == cap);
        check(oa_htbl_elem_max(oah) == cap);

        check(!oa_htbl_is_empty(oah));
        oa_htbl_clear(oah);
        check(oa_htbl_is_empty(oah));

        check(oah->elem_inside == 0);
        check(oa_htbl_elem_count(oah) == 0);
        check(oah->elem_max == cap);
        check(oa_htbl_elem_max(oah) == cap);

        for (int i = 0; i < all_elems; ++i)
        {
            check(bit_vect_get(is_taken, i, &bit_val));
            check(!bit_val);
        }
        c_vect_zero_out(the_tbl);

        check(*(pdata = c_vect_get(the_tbl, 0)) == 0);
        check(bit_vect_get(is_taken, bit_taken(0), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(0), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 1)) == 0);
        check(bit_vect_get(is_taken, bit_taken(1), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(1), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 2)) == 0);
        check(bit_vect_get(is_taken, bit_taken(2), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(2), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 3)) == 0);
        check(bit_vect_get(is_taken, bit_taken(3), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(3), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 4)) == 0);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
        check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
        check(bit_vect_get(is_taken, bit_taken(6), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(6), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
        check(bit_vect_get(is_taken, bit_taken(7), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(7), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 8)) == 0);
        check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 9)) == 0);
        check(bit_vect_get(is_taken, bit_taken(9), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(9), &bit_val) && !bit_val);
        check(!c_vect_get(the_tbl, 10));

        c_vector * the_tbl = &(oah->the_tbl);
        int vect_cap = c_vect_capacity(the_tbl);

        int vals[] = {118, 19, 18, 29, 38, 1, 9, 0, -5, 1235};
        for (int i = 0; i < sizeof(vals)/sizeof(*vals); ++i)
            check(!oa_htbl_lookup(oah, vals+i));

        insert = 1;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 1);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);

        check(*(pdata = c_vect_get(the_tbl, 0)) == 0);
        check(bit_vect_get(is_taken, bit_taken(0), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(0), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
        check(bit_vect_get(is_taken, bit_taken(1), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(1), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 2)) == 0);
        check(bit_vect_get(is_taken, bit_taken(2), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(2), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 3)) == 0);
        check(bit_vect_get(is_taken, bit_taken(3), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(3), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 4)) == 0);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
        check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
        check(bit_vect_get(is_taken, bit_taken(6), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(6), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
        check(bit_vect_get(is_taken, bit_taken(7), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(7), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 8)) == 0);
        check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 9)) == 0);
        check(bit_vect_get(is_taken, bit_taken(9), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(9), &bit_val) && !bit_val);
        check(!c_vect_get(the_tbl, 10));

        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);

        check(*(pdata = c_vect_get(the_tbl, 0)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
        check(*(pdata = c_vect_get(the_tbl, 2)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 3)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 4)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 8)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 9)) == 0);
        check(!c_vect_get(the_tbl, 10));

        insert = 19;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 9);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        insert = 9;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 9);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        insert = 29;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 9);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);

        check(*(pdata = c_vect_get(the_tbl, 0)) == 9);
        check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
        check(*(pdata = c_vect_get(the_tbl, 2)) == 29);
        check(*(pdata = c_vect_get(the_tbl, 3)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 4)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 8)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 9)) == 19);
        check(!c_vect_get(the_tbl, 10));

        insert = 118;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 8);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        insert = 38;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 8);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);

        insert = 18;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 8);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);

        check(*(pdata = c_vect_get(the_tbl, 0)) == 9);
        check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
        check(*(pdata = c_vect_get(the_tbl, 2)) == 29);
        check(*(pdata = c_vect_get(the_tbl, 3)) == 38);
        check(*(pdata = c_vect_get(the_tbl, 4)) == 18);
        check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
        check(*(pdata = c_vect_get(the_tbl, 8)) == 118);
        check(*(pdata = c_vect_get(the_tbl, 9)) == 19);
        check(!c_vect_get(the_tbl, 10));

        check(oah->elem_inside == 7);
        check(oa_htbl_elem_count(oah) == 7);

        int look = 0;
        check(!oa_htbl_lookup(oah, &look));
        look = 1000;
        check(!oa_htbl_lookup(oah, &look));

        int look_vals[] = {118, 19, 18, 29, 38, 1, 9};
        for (int i = 0; i < sizeof(look_vals)/sizeof(*look_vals); ++i)
            check(*(pdata = oa_htbl_lookup(oah, look_vals+i)) == look_vals[i]);

        int rem = 38;
        check(*(pdata = oa_htbl_lookup(oah, &rem)) == rem);
        check(oa_htbl_remove(oah, &rem) == oah);
        check(!oa_htbl_lookup(oah, &rem));

        insert = 28;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 8);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);


        check(*(pdata = c_vect_get(the_tbl, 0)) == 9);
        check(bit_vect_get(is_taken, bit_taken(0), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(0), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
        check(bit_vect_get(is_taken, bit_taken(1), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(1), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 2)) == 29);
        check(bit_vect_get(is_taken, bit_taken(2), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(2), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 3)) == 28);
        check(bit_vect_get(is_taken, bit_taken(3), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(3), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 4)) == 18);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
        check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
        check(bit_vect_get(is_taken, bit_taken(6), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(6), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
        check(bit_vect_get(is_taken, bit_taken(7), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(7), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 8)) == 118);
        check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 9)) == 19);
        check(bit_vect_get(is_taken, bit_taken(9), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(9), &bit_val) && !bit_val);
        check(!c_vect_get(the_tbl, 10));

        rem = 18;
        check(*(pdata = oa_htbl_lookup(oah, &rem)) == rem);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);
        check(oa_htbl_remove(oah, &rem) == oah);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && bit_val);
        check(!oa_htbl_lookup(oah, &rem));

        rem = 118;
        check(*(pdata = oa_htbl_lookup(oah, &rem)) == rem);
        check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && !bit_val);
        check(oa_htbl_remove(oah, &rem) == oah);
        check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && bit_val);
        check(!oa_htbl_lookup(oah, &rem));

        check(oah->elem_inside == 5);
        check(oa_htbl_elem_count(oah) == 5);

        insert = 4;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 4);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && bit_val);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);

        insert = 5;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 5);
        check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);

        check(oah->elem_inside == 7);
        check(oa_htbl_elem_count(oah) == 7);
        check(c_vect_capacity(the_tbl) == all_elems);


        check(*(pdata = c_vect_get(the_tbl, 0)) == 9);
        check(bit_vect_get(is_taken, bit_taken(0), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(0), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
        check(bit_vect_get(is_taken, bit_taken(1), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(1), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 2)) == 29);
        check(bit_vect_get(is_taken, bit_taken(2), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(2), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 3)) == 28);
        check(bit_vect_get(is_taken, bit_taken(3), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(3), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 4)) == 4);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 5)) == 5);
        check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
        check(bit_vect_get(is_taken, bit_taken(6), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(6), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
        check(bit_vect_get(is_taken, bit_taken(7), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(7), &bit_val) && !bit_val);

        check(*(pdata = c_vect_get(the_tbl, 8)) == 118);
        check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && bit_val);

        check(*(pdata = c_vect_get(the_tbl, 9)) == 19);
        check(bit_vect_get(is_taken, bit_taken(9), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(9), &bit_val) && !bit_val);
        check(!c_vect_get(the_tbl, 10));

        int so_far[] = {9, 1, 29, 28, 4, 5, 19, 55};

        insert = 55;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 5);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);

        check(oah->elem_inside == 8);
        check(oa_htbl_elem_count(oah) == 8);
        check(c_vect_capacity(the_tbl) == all_elems*2);

        check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
        check(*(pdata = c_vect_get(the_tbl, 4)) == 4);
        check(*(pdata = c_vect_get(the_tbl, 5)) == 5);
        check(*(pdata = c_vect_get(the_tbl, 8)) == 28);
        check(*(pdata = c_vect_get(the_tbl, 9)) == 9);
        check(*(pdata = c_vect_get(the_tbl, 10)) == 29);
        check(*(pdata = c_vect_get(the_tbl, 15)) == 55);
        check(*(pdata = c_vect_get(the_tbl, 19)) == 19);

        check(bit_vect_get(is_taken, bit_taken(1), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(1), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(9), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(9), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(10), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(10), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(15), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(15), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(19), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(19), &bit_val) && !bit_val);

        for (int i = 0; i < sizeof(so_far)/sizeof(*so_far); ++i)
            check(*(pdata = oa_htbl_lookup(oah, so_far+i)) == so_far[i]);

        for (int i = 0; i < sizeof(so_far)/sizeof(*so_far); ++i)
            check(oa_htbl_remove(oah, so_far+i) == oah);

        for (int i = 0; i < sizeof(so_far)/sizeof(*so_far); ++i)
            check(!oa_htbl_lookup(oah, so_far+i));

        check(oah->elem_inside == 0);
        check(oa_htbl_elem_count(oah) == 0);
        check(c_vect_capacity(the_tbl) == all_elems*2);

        int j = 0;
        int hash_ord[] = {1, 4, 5, 28, 9, 29, 55, 19};
        for (int i = 0, end = all_elems*2; i < end; ++i)
        {
            check(bit_vect_get(is_taken, bit_taken(i), &bit_val) && !bit_val);
            if (bit_vect_get(is_taken, bit_vacant(i), &bit_val) && bit_val)
                check(*(pdata = c_vect_get(the_tbl, i)) == hash_ord[j++]);
        }
        check(j == 8);

        for (int i = 0; i < sizeof(so_far)/sizeof(*so_far); ++i)
            check(*(pdata = oa_htbl_insert(oah,so_far+i)) == so_far[i]);

        check(oah->elem_inside == 8);
        check(oa_htbl_elem_count(oah) == 8);
        check((vect_cap  = c_vect_capacity(the_tbl)) == all_elems*2);

        insert = 39;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 19);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(*(pdata = oa_htbl_lookup(oah, &insert)) == insert);

        insert = 59;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 19);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(*(pdata = oa_htbl_lookup(oah, &insert)) == insert);

        check(*(pdata = c_vect_get(the_tbl, 0)) == 39);
        check(*(pdata = c_vect_get(the_tbl, 2)) == 59);
        check(bit_vect_get(is_taken, bit_taken(0), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(0), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(2), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(2), &bit_val) && !bit_val);

        rem = 39;
        check(oa_htbl_remove(oah, &rem) == oah);
        check(!oa_htbl_lookup(oah, &rem));

        int lookup = 59;
        check(*(pdata = oa_htbl_lookup(oah, &lookup)) == lookup);

        lookup = 19;
        check(*(pdata = oa_htbl_lookup(oah, &lookup)) == lookup);

        insert = 39;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 19);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(*(pdata = oa_htbl_lookup(oah, &insert)) == insert);

        rem = 19;
        check(oa_htbl_remove(oah, &rem) == oah);
        check(!oa_htbl_lookup(oah, &rem));
        check(bit_vect_get(is_taken, bit_taken(19), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(19), &bit_val) && bit_val);

        lookup = 59;
        check(*(pdata = oa_htbl_lookup(oah, &lookup)) == lookup);
        lookup = 39;
        check(*(pdata = oa_htbl_lookup(oah, &lookup)) == lookup);

        insert = 15;
        check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
            == 15);
        check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
        check(*(pdata = oa_htbl_lookup(oah, &insert)) == insert);

        check(*(pdata = c_vect_get(the_tbl, 15)) == 55);
        check(*(pdata = c_vect_get(the_tbl, 16)) == 15);
        check(bit_vect_get(is_taken, bit_taken(15), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(15), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(16), &bit_val) && bit_val);
        check(bit_vect_get(is_taken, bit_vacant(16), &bit_val) && !bit_val);

        rem = 55;
        check(oa_htbl_remove(oah, &rem) == oah);
        check(!oa_htbl_lookup(oah, &rem));
        check(bit_vect_get(is_taken, bit_taken(19), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(19), &bit_val) && bit_val);

        lookup = 55;
        check(!oa_htbl_lookup(oah, &lookup));
        lookup = 15;
        check(*(pdata = oa_htbl_lookup(oah, &lookup)) == lookup);

        check(bit_vect_get(is_taken, bit_taken(11), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(11), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(14), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(14), &bit_val) && !bit_val);

        int no_find = 7777;
        c_vect_write_at(the_tbl, 11, &no_find);
        check(*(pdata = c_vect_get(the_tbl, 11)) == no_find);
        c_vect_write_at(the_tbl, 14, &no_find);
        check(*(pdata = c_vect_get(the_tbl, 14)) == no_find);
        check(bit_vect_get(is_taken, bit_taken(11), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(11), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_taken(14), &bit_val) && !bit_val);
        check(bit_vect_get(is_taken, bit_vacant(14), &bit_val) && !bit_val);

        check(!oa_htbl_lookup(oah, &no_find));

        no_find = -1234;
        check(!oa_htbl_lookup(oah, &no_find));
        no_find = 0;
        check(!oa_htbl_lookup(oah, &no_find));
        no_find = 11;
        check(!oa_htbl_lookup(oah, &no_find));

        oa_htbl_destroy(oah);
    }

    {
        check(oa_htbl_make(oah, el_size, compar) == oah);

        int ins[] = {2, 3, 1, 4, 2};
        const int * pdata;
        for (int i = 0; i < sizeof(ins)/sizeof(*ins); ++i)
            check(*(pdata = oa_htbl_insert(oah, ins+i)) == ins[i]);

        check(oa_htbl_elem_count(oah) == 4);

        oa_htbl_destroy(oah);
    }

    return true;
}
//------------------------------------------------------------------------------

static bool test_iterate(void)
{
    oa_htbl oah_, * oah = &oah_;
    c_vector * the_tbl;
    bit_vector * is_taken;

    int el_size = sizeof(int);
    int cap = 7;
    int all_elems = round(
            (double)cap / (double)OAH_DEFAULT_LOAD_FACT);
    check(all_elems == 10);

    check(oa_htbl_make_cap_hash(oah, el_size, compar, cap, ab_hash) == oah);

    the_tbl = &(oah->the_tbl);
    is_taken = &(oah->is_taken);

    c_vect_zero_out(the_tbl);
    check(c_vect_elem_size(the_tbl) == el_size);
    check(c_vect_compar(the_tbl) == compar);
    check(c_vect_capacity(the_tbl) == all_elems);
    check(c_vect_length(the_tbl) == all_elems);
    check(bit_vect_num_of_bits(is_taken) == all_elems*2);

    check(oah->hash_fun == ab_hash);
    check(oah->elem_inside == 0);
    check(oah->elem_max == cap);

    const int * pdata = NULL;

    int insert = 1, vect_cap = all_elems;
    check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
        == 1);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);


    insert = 19;
    check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
        == 9);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
    insert = 9;
    check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
        == 9);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
    insert = 29;
    check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
        == 9);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);


    insert = 118;
    check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
        == 8);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
    insert = 38;
    check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
        == 8);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);

    insert = 18;
    check((ab_hash((unsigned char *)&insert, sizeof(insert)) % vect_cap)
        == 8);
    check(*(pdata = oa_htbl_insert(oah, &insert)) == insert);


    check(*(pdata = c_vect_get(the_tbl, 0)) == 9);
    check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
    check(*(pdata = c_vect_get(the_tbl, 2)) == 29);
    check(*(pdata = c_vect_get(the_tbl, 3)) == 38);
    check(*(pdata = c_vect_get(the_tbl, 4)) == 18);
    check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 8)) == 118);
    check(*(pdata = c_vect_get(the_tbl, 9)) == 19);
    check(!c_vect_get(the_tbl, 10));

    check(oah->elem_inside == 7);

    oa_htbl_iterator no = 1234;
    check(!oa_htbl_iterate_(oah, no, &no));
    no = -1234;
    check(!oa_htbl_iterate(oah, no));

    int so_far[] = {9, 1, 29, 38, 18, 118, 19};

    oa_htbl_iterator elem = oa_htbl_iterator_init();
    int i = 0;
    while ((pdata = oa_htbl_iterate_(oah, elem, &elem)))
        check(*pdata == so_far[i++]);
    check(so_far[--i] == 19);
    check(elem == 10);

    elem = 1, i = elem;
    while ((pdata = oa_htbl_iterate(oah, elem)))
        check(*pdata == so_far[i++]);
    check(so_far[--i] == 19);
    check(elem == 10);

    elem = 5, i = elem;
    while ((pdata = oa_htbl_iterate_(oah, elem, &elem)))
        check(*pdata == so_far[i++]);
    check(so_far[--i] == 19);
    check(elem == 10);

    elem = 6, i = 5;
    while ((pdata = oa_htbl_iterate(oah, elem)))
        check(*pdata == so_far[i++]);
    check(so_far[--i] == 19);
    check(elem == 10);

    elem = 7, i = 5;
    while ((pdata = oa_htbl_iterate_(oah, elem, &elem)))
        check(*pdata == so_far[i++]);
    check(so_far[--i] == 19);
    check(elem == 10);

    elem = 8, i = 5;
    while ((pdata = oa_htbl_iterate(oah, elem)))
        check(*pdata == so_far[i++]);
    check(so_far[--i] == 19);
    check(elem == 10);

    oa_htbl_destroy(oah);
    return true;
}
//------------------------------------------------------------------------------

static bool test_clear(void)
{
    oa_htbl oah_, * oah = &oah_;
    bit_vector * is_taken;

    int el_size = sizeof(int);
    int cap = 10;

    check(oa_htbl_make_cap_hash(oah, el_size, compar, cap, ab_hash) == oah);
    is_taken = &(oah->is_taken);

    check(oah->elem_inside == 0);
    oah->elem_inside = 777;
    check(oah->elem_inside == 777);

    int bit_val = 0;
    for (int i = 0; i < cap; ++i)
    {
        check(bit_vect_get(is_taken, i, &bit_val));
        check(!bit_val);
    }

    check(bit_vect_set_all(is_taken) == is_taken);
    for (int i = 0; i < cap; ++i)
    {
        check(bit_vect_get(is_taken, i, &bit_val));
        check(bit_val);
    }

    oa_htbl_clear(oah);
    for (int i = 0; i < cap; ++i)
    {
        check(bit_vect_get(is_taken, i, &bit_val));
        check(!bit_val);
    }

    check(oah->elem_inside == 0);

    oa_htbl_destroy(oah);
    return true;
}
//------------------------------------------------------------------------------

static bool test_lookup(void)
{
    oa_htbl oah_, * oah = &oah_;
    c_vector * the_tbl;
    bit_vector * is_taken;

    int el_size = sizeof(int);
    int cap = 7;
    int all_elems = round(
            (double)cap / (double)OAH_DEFAULT_LOAD_FACT);

    int insert = 5;
    check((ab_hash((unsigned char *)&insert, 0) % cap) == 5);

    check(oa_htbl_make_cap_hash(oah, el_size, compar, cap, ab_hash) == oah);

    the_tbl = &(oah->the_tbl);
    is_taken = &(oah->is_taken);

    c_vect_zero_out(the_tbl);
    check(c_vect_elem_size(the_tbl) == el_size);
    check(c_vect_compar(the_tbl) == compar);
    check(c_vect_capacity(the_tbl) == all_elems);
    check(c_vect_length(the_tbl) == all_elems);
    check(bit_vect_num_of_bits(is_taken) == all_elems*2);

    check(oah->hash_fun == ab_hash);
    check(oah->elem_inside == 0);
    check(oah->elem_max == cap);

    const int * pdata = NULL;
    int bit_val = 0;
    for (int i = 0; i < cap; ++i)
    {
        check(*(pdata = c_vect_get(the_tbl, i)) == 0);
        check(bit_vect_get(is_taken, i, &bit_val));
        check(!bit_val);
    }
    check(!c_vect_get(the_tbl, 10));

    insert = 19;
    c_vect_write_at(the_tbl, 0, &insert);
    bit_vect_set(is_taken, bit_taken(0));
    insert = 2;
    c_vect_write_at(the_tbl, 2, &insert);
    bit_vect_set(is_taken, bit_taken(2));
    insert = 3;
    c_vect_write_at(the_tbl, 3, &insert);
    bit_vect_set(is_taken, bit_taken(3));
    insert = 6;
    c_vect_write_at(the_tbl, 6, &insert);
    bit_vect_set(is_taken, bit_taken(6));
    insert = 16;
    c_vect_write_at(the_tbl, 7, &insert);
    bit_vect_set(is_taken, bit_taken(7));
    insert = 9;
    c_vect_write_at(the_tbl, 9, &insert);
    bit_vect_set(is_taken, bit_taken(9));

    check(*(pdata = c_vect_get(the_tbl, 0)) == 19);
    check(*(pdata = c_vect_get(the_tbl, 1)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 2)) == 2);
    check(*(pdata = c_vect_get(the_tbl, 3)) == 3);
    check(*(pdata = c_vect_get(the_tbl, 4)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 6)) == 6);
    check(*(pdata = c_vect_get(the_tbl, 7)) == 16);
    check(*(pdata = c_vect_get(the_tbl, 8)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 9)) == 9);
    check(!c_vect_get(the_tbl, 10));

    int look = 0;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 0);
    check(!oa_htbl_lookup(oah, &look));

    look = 1;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 1);
    check(!oa_htbl_lookup(oah, &look));

    look = 2;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 2);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 2);

    look = 3;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 3);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 3);

    look = 4;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 4);
    check(!oa_htbl_lookup(oah, &look));

    look = 5;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 5);
    check(!oa_htbl_lookup(oah, &look));

    look = 6;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 6);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 6);

    look = 16;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 6);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 16);

    look = 7;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 7);
    check(!oa_htbl_lookup(oah, &look));

    look = 8;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 8);
    check(!oa_htbl_lookup(oah, &look));

    look = 9;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 9);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 9);

    look = 19;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 9);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 19);

    insert = 8;
    c_vect_write_at(the_tbl, 8, &insert);
    bit_vect_set(is_taken, bit_taken(8));

    insert = 18;
    c_vect_write_at(the_tbl, 1, &insert);
    bit_vect_set(is_taken, bit_taken(1));

    check(*(pdata = c_vect_get(the_tbl, 0)) == 19);
    check(*(pdata = c_vect_get(the_tbl, 1)) == 18);
    check(*(pdata = c_vect_get(the_tbl, 2)) == 2);
    check(*(pdata = c_vect_get(the_tbl, 3)) == 3);
    check(*(pdata = c_vect_get(the_tbl, 4)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 6)) == 6);
    check(*(pdata = c_vect_get(the_tbl, 7)) == 16);
    check(*(pdata = c_vect_get(the_tbl, 8)) == 8);
    check(*(pdata = c_vect_get(the_tbl, 9)) == 9);
    check(!c_vect_get(the_tbl, 10));

    look = 8;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 8);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 8);

    look = 18;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 8);
    check(*(pdata = oa_htbl_lookup(oah, &look)) == 18);

    look = 0;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 0);
    check(!oa_htbl_lookup(oah, &look));

    look = 1;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 1);
    check(!oa_htbl_lookup(oah, &look));

    look = 4;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 4);
    check(!oa_htbl_lookup(oah, &look));

    look = 13;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 3);
    check(!oa_htbl_lookup(oah, &look));

    look = 10;
    check((ab_hash((unsigned char *)&look, 0) % all_elems) == 0);
    check(!oa_htbl_lookup(oah, &look));

    look = 123943;
    check(!oa_htbl_lookup(oah, &look));

    oa_htbl_destroy(oah);
    return true;
}
//------------------------------------------------------------------------------

static bool test_copy(void)
{
    oa_htbl oah_, * oah = &oah_;
    c_vector * the_tbl = &(oah->the_tbl);
    bit_vector * is_taken = &(oah->is_taken);

    int el_size = sizeof(int);
    int cap = 7;
    int all_elems = round(
            (double)cap / (double)OAH_DEFAULT_LOAD_FACT);
    check(all_elems == 10);

    check(oa_htbl_make_cap_hash(oah, el_size, compar, cap, ab_hash) == oah);

    oa_htbl cpy_, * cpy = &cpy_;
    memset(cpy, 0xFF, sizeof(*cpy));
    oa_htbl zout;
    memset(&zout, 0, sizeof(zout));
    check(memcmp(cpy, &zout, sizeof(zout)) != 0);

    bool success = true;
    cpy_ = oa_htbl_copy(oah, &success);
    check(memcmp(cpy, &zout, sizeof(zout)) == 0);
    check(!success);

    check(c_vect_zero_out(the_tbl) == the_tbl);
    const int * pdata = NULL;
    int bit_val = 0;
    for (int i = 0; i < cap; ++i)
    {
        check(*(pdata = c_vect_get(the_tbl, i)) == 0);
        check(bit_vect_get(is_taken, i, &bit_val));
        check(!bit_val);
    }
    check(!c_vect_get(the_tbl, 10));

    int insert[] = {1, 19, 9, 29, 118, 38, 18};
    int ins_sz = sizeof(insert)/sizeof(*insert);
    pdata = NULL;
    for (int i = 0; i < ins_sz; ++i)
        check(*(pdata = oa_htbl_insert(oah, insert+i)) == insert[i]);

    check(*(pdata = c_vect_get(the_tbl, 0)) == 9);
    check(*(pdata = c_vect_get(the_tbl, 1)) == 1);
    check(*(pdata = c_vect_get(the_tbl, 2)) == 29);
    check(*(pdata = c_vect_get(the_tbl, 3)) == 38);
    check(*(pdata = c_vect_get(the_tbl, 4)) == 18);
    check(*(pdata = c_vect_get(the_tbl, 5)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 6)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 7)) == 0);
    check(*(pdata = c_vect_get(the_tbl, 8)) == 118);
    check(*(pdata = c_vect_get(the_tbl, 9)) == 19);
    check(!c_vect_get(the_tbl, 10));

    bit_val = 0;
    check(bit_vect_get(is_taken, bit_taken(0), &bit_val) && bit_val);
    check(bit_vect_get(is_taken, bit_vacant(0), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(1), &bit_val) && bit_val);
    check(bit_vect_get(is_taken, bit_vacant(1), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(2), &bit_val) && bit_val);
    check(bit_vect_get(is_taken, bit_vacant(2), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(3), &bit_val) && bit_val);
    check(bit_vect_get(is_taken, bit_vacant(3), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(4), &bit_val) && bit_val);
    check(bit_vect_get(is_taken, bit_vacant(4), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(5), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_vacant(5), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(6), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_vacant(6), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(7), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_vacant(7), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(8), &bit_val) && bit_val);
    check(bit_vect_get(is_taken, bit_vacant(8), &bit_val) && !bit_val);
    check(bit_vect_get(is_taken, bit_taken(9), &bit_val) && bit_val);
    check(bit_vect_get(is_taken, bit_vacant(9), &bit_val) && !bit_val);


    success = false;
    *cpy = oa_htbl_copy(oah, &success);
    check(success);

    c_vector * cpy_tbl = &(cpy->the_tbl);
    bit_vector * cpy_taken = &(cpy->is_taken);

    check(cpy->hash_fun == oah->hash_fun);
    check(cpy->elem_inside == oah->elem_inside);
    check(cpy->elem_max == oah->elem_max);

    check(*(pdata = c_vect_get(cpy_tbl, 0)) == 9);
    check(*(pdata = c_vect_get(cpy_tbl, 1)) == 1);
    check(*(pdata = c_vect_get(cpy_tbl, 2)) == 29);
    check(*(pdata = c_vect_get(cpy_tbl, 3)) == 38);
    check(*(pdata = c_vect_get(cpy_tbl, 4)) == 18);
    check(*(pdata = c_vect_get(cpy_tbl, 5)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 6)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 7)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 8)) == 118);
    check(*(pdata = c_vect_get(cpy_tbl, 9)) == 19);
    check(!c_vect_get(cpy_tbl, 10));

    check(bit_vect_get(cpy_taken, bit_taken(0), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(0), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(1), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(1), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(2), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(2), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(3), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(3), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(4), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(4), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(5), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(5), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(6), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(6), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(7), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(7), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(8), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(8), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(9), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(9), &bit_val) && !bit_val);

    check(c_vect_length(the_tbl) == c_vect_length(cpy_tbl));
    check(c_vect_elem_size(the_tbl) == c_vect_elem_size(cpy_tbl));

    void * src_data = c_vect_data(the_tbl);
    void * cpy_data = c_vect_data(cpy_tbl);
    check(memcmp(src_data, cpy_data,
        c_vect_length(the_tbl)*c_vect_elem_size(the_tbl)) == 0);

    src_data = is_taken->the_bits;
    cpy_data = cpy_taken->the_bits;

    int bits = bit_vect_num_of_bits(is_taken);
    check(bits == bit_vect_num_of_bits(cpy_taken));

    int bytes = (bits / CHAR_BIT) + ((bits % CHAR_BIT) ? 1 : 0);

    check(memcmp(src_data, cpy_data, bytes) == 0);

    oa_htbl_destroy(oah);

    check(*(pdata = c_vect_get(cpy_tbl, 0)) == 9);
    check(*(pdata = c_vect_get(cpy_tbl, 1)) == 1);
    check(*(pdata = c_vect_get(cpy_tbl, 2)) == 29);
    check(*(pdata = c_vect_get(cpy_tbl, 3)) == 38);
    check(*(pdata = c_vect_get(cpy_tbl, 4)) == 18);
    check(*(pdata = c_vect_get(cpy_tbl, 5)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 6)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 7)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 8)) == 118);
    check(*(pdata = c_vect_get(cpy_tbl, 9)) == 19);
    check(!c_vect_get(cpy_tbl, 10));

    check(bit_vect_get(cpy_taken, bit_taken(0), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(0), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(1), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(1), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(2), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(2), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(3), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(3), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(4), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(4), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(5), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(5), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(6), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(6), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(7), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(7), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(8), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(8), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(9), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(9), &bit_val) && !bit_val);

    oa_htbl cpy2 = oa_htbl_copy(cpy, NULL);
    oa_htbl_destroy(&cpy2);

    for (int i = 0; i < ins_sz; ++i)
        check(oa_htbl_remove(cpy, insert+i) == cpy);

    check(*(pdata = c_vect_get(cpy_tbl, 0)) == 9);
    check(*(pdata = c_vect_get(cpy_tbl, 1)) == 1);
    check(*(pdata = c_vect_get(cpy_tbl, 2)) == 29);
    check(*(pdata = c_vect_get(cpy_tbl, 3)) == 38);
    check(*(pdata = c_vect_get(cpy_tbl, 4)) == 18);
    check(*(pdata = c_vect_get(cpy_tbl, 5)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 6)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 7)) == 0);
    check(*(pdata = c_vect_get(cpy_tbl, 8)) == 118);
    check(*(pdata = c_vect_get(cpy_tbl, 9)) == 19);
    check(!c_vect_get(cpy_tbl, 10));

    check(bit_vect_get(cpy_taken, bit_taken(0), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(0), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(1), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(1), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(2), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(2), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(3), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(3), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(4), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(4), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(5), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(5), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(6), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(6), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(7), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(7), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(8), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(8), &bit_val) && bit_val);
    check(bit_vect_get(cpy_taken, bit_taken(9), &bit_val) && !bit_val);
    check(bit_vect_get(cpy_taken, bit_vacant(9), &bit_val) && bit_val);

    oa_htbl_destroy(cpy);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_oa_htbl(void)
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

int toa_htbl_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int toa_htbl_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
