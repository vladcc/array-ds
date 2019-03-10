#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "ov_multiset.h"
#include "test.h"

static bool test_ov_mset(void);
static bool test_mem_align(void);


static ftest tests[] = {
    test_ov_mset,
    test_mem_align
};
//------------------------------------------------------------------------------

static bool test_mem_align(void)
{
    ov_mset mset_, * mset = &mset_;

    for (int i = 1; i <= 4; ++i)
    {
        check(ov_mset_make(mset, i, NULL) == mset);
        int elem_size = ov_mset_real_elem_size(mset);
        check(elem_size % sizeof(int) == 0);
        check(elem_size == 2 * sizeof(int));
        check(ov_mset_orig_elem_size(mset) == i);
        ov_mset_destroy(mset);
    }

    for (int i = 5; i <= 8; ++i)
    {
        ov_mset_make(mset, i, NULL);
        int elem_size = ov_mset_real_elem_size(mset);
        check(elem_size % sizeof(int) == 0);
        check(elem_size == 3 * sizeof(int));
        check(ov_mset_orig_elem_size(mset) == i);
        ov_mset_destroy(mset);
    }

    for (int i = 9; i <= 12; ++i)
    {
        ov_mset_make(mset, i, NULL);
        int elem_size = ov_mset_real_elem_size(mset);
        check(elem_size % sizeof(int) == 0);
        check(elem_size == 4 * sizeof(int));
        check(ov_mset_orig_elem_size(mset) == i);
        ov_mset_destroy(mset);
    }

    for (int i = 13; i <= 16; ++i)
    {
        ov_mset_make(mset, i, NULL);
        int elem_size = ov_mset_real_elem_size(mset);
        check(elem_size % sizeof(int) == 0);
        check(elem_size == 5 * sizeof(int));
        check(ov_mset_orig_elem_size(mset) == i);
        ov_mset_destroy(mset);
    }

    ov_mset_make(mset, 123, NULL);
    int elem_size = ov_mset_real_elem_size(mset);
    check(elem_size % sizeof(int) == 0);
    check(elem_size == 32 * sizeof(int));
    check(ov_mset_orig_elem_size(mset) == 123);
    ov_mset_destroy(mset);

    return true;
}
//------------------------------------------------------------------------------

static int compar_ints(const void * one, const void * two)
{
    int a = *((int *)one);
    int b = *((int *)two);

    int comp = 0;
    if (a > b)
        comp = 1;
    else if (a < b)
        comp = -1;

    return comp;
}

typedef short first_test;
static int compar_ft(const void * one, const void * two)
{
    first_test a = *((first_test *)one);
    first_test b = *((first_test *)two);
    if (a > b) return 1;
    if (a < b) return -1;
    return 0;
}

//------------------------------------------------------------------------------

static int g_elm_count = 0;
static void apply(const void * elm)
{
    ++g_elm_count;
}

static void apply_args(const void * elm, void * arg)
{
    int * ip = arg;
    ++(*ip);
}

typedef struct sint {
    int val;
    double dummy;
} sint;

static bool test_ov_mset(void)
{
    ov_mset mset_, * mset = &mset_;
    memset(mset, 0xFF, sizeof(ov_mset));
    ov_mset zout;
    memset(&zout, 0, sizeof(ov_mset));

    memcmp(&zout, mset, sizeof(ov_mset) != 0);
    check(!ov_mset_make_cap(mset, sizeof(first_test), compar_ft, -2));
    memcmp(&zout, mset, sizeof(ov_mset) == 0);

    memset(mset, 0xFF, sizeof(*mset));
    memcmp(&zout, mset, sizeof(ov_mset) != 0);
    check(!ov_mset_make_cap(mset, sizeof(first_test), compar_ft, 0));
    memcmp(&zout, mset, sizeof(ov_mset) == 0);

    memset(mset, 0xFF, sizeof(*mset));
    memcmp(&zout, mset, sizeof(ov_mset) != 0);
    check(!ov_mset_make(mset, 0, compar_ft));
    memcmp(&zout, mset, sizeof(ov_mset) == 0);

    memset(mset, 0xFF, sizeof(*mset));
    memcmp(&zout, mset, sizeof(ov_mset) != 0);
    check(!ov_mset_make(mset, -213, compar_ft));
    memcmp(&zout, mset, sizeof(ov_mset) == 0);

    ov_mset_make(mset, sizeof(first_test), compar_ft);
    int elem_size = ov_mset_real_elem_size(mset);
    check(elem_size % sizeof(int) == 0);

    if (sizeof(first_test) < sizeof(int))
        check(elem_size = 2 * sizeof(int));

    first_test arr[] = {1, 2, 2, 1, 5, 1, 1, 2, 3, 4, 3};
    int arr_sz = sizeof(arr)/sizeof(*arr);

    check(ov_mset_length(mset) == 0);
    for (int i = 0; i < arr_sz; ++i)
        ov_mset_insert(mset, arr+i);
    check(ov_mset_length(mset) == 5);

    first_test sarr[] = {1, 2, 3, 4, 5, -123, 123};
    check(ov_mset_mplicity(mset, sarr) == 4);
    check(ov_mset_mplicity(mset, sarr+1) == 3);
    check(ov_mset_mplicity(mset, sarr+2) == 2);
    check(ov_mset_mplicity(mset, sarr+3) == 1);
    check(ov_mset_mplicity(mset, sarr+4) == 1);
    check(ov_mset_mplicity(mset, sarr+5) == 0);
    check(ov_mset_mplicity(mset, sarr+6) == 0);

    int sarr_sz = sizeof(sarr)/sizeof(*sarr);
    for (int i = 0; i < sarr_sz; ++i)
        ov_mset_remove(mset, sarr+i);

    check(ov_mset_mplicity(mset, sarr) == 3);
    check(ov_mset_mplicity(mset, sarr+1) == 2);
    check(ov_mset_mplicity(mset, sarr+2) == 1);
    check(ov_mset_mplicity(mset, sarr+3) == 0);
    check(ov_mset_mplicity(mset, sarr+4) == 0);
    check(ov_mset_mplicity(mset, sarr+5) == 0);
    check(ov_mset_mplicity(mset, sarr+6) == 0);
    check(ov_mset_length(mset) == 3);

    for (int i = 0; i < sarr_sz-2; ++i)
        ov_mset_insert(mset, sarr+i);

    check(ov_mset_mplicity(mset, sarr) == 4);
    check(ov_mset_mplicity(mset, sarr+1) == 3);
    check(ov_mset_mplicity(mset, sarr+2) == 2);
    check(ov_mset_mplicity(mset, sarr+3) == 1);
    check(ov_mset_mplicity(mset, sarr+4) == 1);
    check(ov_mset_mplicity(mset, sarr+5) == 0);
    check(ov_mset_mplicity(mset, sarr+6) == 0);


    for (int i = 0; i < sarr_sz; ++i)
        ov_mset_remove(mset, sarr+i);
    for (int i = 0; i < sarr_sz; ++i)
        ov_mset_remove(mset, sarr+i);

    check(ov_mset_mplicity(mset, sarr) == 2);
    check(ov_mset_mplicity(mset, sarr+1) == 1);
    check(ov_mset_mplicity(mset, sarr+2) == 0);
    check(ov_mset_mplicity(mset, sarr+3) == 0);
    check(ov_mset_mplicity(mset, sarr+4) == 0);
    check(ov_mset_mplicity(mset, sarr+5) == 0);
    check(ov_mset_mplicity(mset, sarr+6) == 0);
    check(ov_mset_length(mset) == 2);

    for (int i = 0; i < sarr_sz; ++i)
        ov_mset_remove(mset, sarr+i);
    for (int i = 0; i < sarr_sz; ++i)
        ov_mset_remove(mset, sarr+i);

    check(ov_mset_mplicity(mset, sarr) == 0);
    check(ov_mset_mplicity(mset, sarr+1) == 0);
    check(ov_mset_mplicity(mset, sarr+2) == 0);
    check(ov_mset_mplicity(mset, sarr+3) == 0);
    check(ov_mset_mplicity(mset, sarr+4) == 0);
    check(ov_mset_mplicity(mset, sarr+5) == 0);
    check(ov_mset_mplicity(mset, sarr+6) == 0);
    check(ov_mset_is_empty(mset));

    bool success = false;
    ov_mset cpy_ = ov_mset_copy(mset, &success);
    ov_mset * cpy = &cpy_;
    check(!success);

    check(ov_mset_length(mset) == 0);
    for (int i = 0; i < arr_sz; ++i)
        ov_mset_insert(mset, arr+i);
    check(ov_mset_length(mset) == 5);

    check(ov_mset_mplicity(mset, sarr) == 4);
    check(ov_mset_mplicity(mset, sarr+1) == 3);
    check(ov_mset_mplicity(mset, sarr+2) == 2);
    check(ov_mset_mplicity(mset, sarr+3) == 1);
    check(ov_mset_mplicity(mset, sarr+4) == 1);
    check(ov_mset_mplicity(mset, sarr+5) == 0);
    check(ov_mset_mplicity(mset, sarr+6) == 0);

    success = false;
    cpy_ = ov_mset_copy(mset, &success);
    check(success);
    check(cpy->orig_elem_size == mset->orig_elem_size);

    check(ov_mset_mplicity(cpy, sarr) == 4);
    check(ov_mset_mplicity(cpy, sarr+1) == 3);
    check(ov_mset_mplicity(cpy, sarr+2) == 2);
    check(ov_mset_mplicity(cpy, sarr+3) == 1);
    check(ov_mset_mplicity(cpy, sarr+4) == 1);
    check(ov_mset_mplicity(cpy, sarr+5) == 0);
    check(ov_mset_mplicity(cpy, sarr+6) == 0);

    ov_mset_destroy(mset);
    check(ov_mset_length(mset) == 0);

    check(ov_mset_mplicity(cpy, sarr) == 4);
    check(ov_mset_mplicity(cpy, sarr+1) == 3);
    check(ov_mset_mplicity(cpy, sarr+2) == 2);
    check(ov_mset_mplicity(cpy, sarr+3) == 1);
    check(ov_mset_mplicity(cpy, sarr+4) == 1);
    check(ov_mset_mplicity(cpy, sarr+5) == 0);
    check(ov_mset_mplicity(cpy, sarr+6) == 0);

    check(ov_mset_length(cpy) == 5);

    check(ov_mset_erase(cpy, sarr+1) == cpy);
    check(ov_mset_mplicity(cpy, sarr+1) == 0);
    check(ov_mset_length(cpy) == 4);
    check(!ov_mset_erase(cpy, sarr+1));
    ov_mset_insert(cpy, arr+1);
    check(ov_mset_mplicity(cpy, sarr+1) == 1);

    ov_mset_destroy(cpy);

    ov_mset_make_cap(mset, sizeof(sint), compar_ints, 1);
    elem_size = ov_mset_real_elem_size(mset);
    check(elem_size == sizeof(sint) + sizeof(int));
    check((elem_size % sizeof(int)) == 0);

    int max = 100;
    const sint * elm;
    for (int i = 1; i <= max; ++i)
    {
        sint tmp = {.val = i, .dummy = M_PI};
        elm = ov_mset_insert(mset, &tmp);
        check(elm == ov_mset_get(mset, i-1));
        check(elm->val == i);
        check(elm->dummy == M_PI);
        check(ov_mset_mplicity(mset, &i) == 1);
        check(elm = ov_mset_has(mset, &i));
        check(elm->val == i && elm->dummy == M_PI);
    }


    for (int i = 1; i <= max; ++i)
    {
        check(!ov_mset_insert(mset, &i));
        check(ov_mset_mplicity(mset, &i) == 2);
    }

    for (int i = -50; i <= -1; ++i)
    {
        check(ov_mset_mplicity(mset, &i) == 0);
        check(!ov_mset_remove(mset, &i));
    }

    for (int i = 101; i <= 300; ++i)
    {
        check(ov_mset_mplicity(mset, &i) == 0);
        check(!ov_mset_remove(mset, &i));
    }

    int rem = -123;
    check(!ov_mset_remove(mset, &rem));
    rem = 123;
    check(!ov_mset_remove(mset, &rem));
    rem = 99;
    check(ov_mset_remove(mset, &rem) == mset);

    max = 98;
    for (int i = 1; i <= max; ++i)
        check(ov_mset_mplicity(mset, &i) == 2);

    int tmp = 99;
    check(ov_mset_mplicity(mset, &tmp) == 1);
    check(ov_mset_remove(mset, &rem) == mset);
    check(ov_mset_mplicity(mset, &tmp) == 0);
    tmp = 100;
    check(ov_mset_mplicity(mset, &tmp) == 2);

    max = 97;
    for (int i = 0, offs = 1; i < max; ++i)
    {
        check(elm = ov_mset_get(mset, i));
        check(elm->val == i+offs);
        check(elm->dummy == M_PI);
        check(ov_mset_mplicity(mset, elm) == 2);
    }

    check(g_elm_count == 0);
    check(ov_mset_apply(mset, apply) == mset);
    check(g_elm_count > 0);
    check(g_elm_count == ov_mset_length(mset));

    int count = 0;
    check(ov_mset_apply_args(mset, apply_args, &count) == mset);
    check(count > 0);
    check(count == ov_mset_length(mset));

    ov_mset cpy2 = ov_mset_copy(mset, NULL);
    memcmp(&zout, &cpy2, sizeof(ov_mset) != 0);
    ov_mset_destroy(&cpy2);
    memcmp(&zout, &cpy2, sizeof(ov_mset) == 0);

    memcmp(&zout, mset, sizeof(ov_mset) != 0);
    ov_mset_destroy(mset);
    memcmp(&zout, mset, sizeof(ov_mset) == 0);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_ov_mset(void)
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

int tov_mset_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tov_mset_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
