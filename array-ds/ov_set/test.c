#include <stdio.h>
#include "ov_set.h"
#include "test.h"

static bool test_ov_set(void);

static ftest tests[] = {
    test_ov_set
};
//------------------------------------------------------------------------------

static int compar(const void * one, const void * two)
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

static bool test_ov_set(void)
{
    ov_set set_, * set = &set_;
    ov_set_make(set, sizeof(int), compar);

    int add = 10;
    const int * out;
    for (int i = 0; i < add; ++i)
        check(*(out = ov_set_insert(set, &i)) == i);

    for (int i = 0; i < add; ++i)
    {
        check(!ov_set_insert(set, &i));
        check(ov_set_has(set, &i));
        check(*(out = ov_set_get(set, i)) == i);
    }

    int has_not = -123;
    check(!ov_set_has(set, &has_not));
    has_not = 123;
    check(!ov_set_has(set, &has_not));

    int n = 1;
    check(ov_set_remove(set, &n) == set);
    check(ov_set_remove(set, &n) == NULL);
    check(ov_set_remove(set, &n) == NULL);
    check(ov_set_remove_at(set, 0) == set);

    for (int i = 0, end = add-2, offs = 2; i < end; ++i)
    {
        int val = i+offs;
        check(*(out = ov_set_get(set, i)) == val);
        check(*(out = ov_set_has(set, &val)) == val);
    }

    check(g_elm_count == 0);
    check(ov_set_apply(set, apply) == set);
    check(g_elm_count > 0);
    check(g_elm_count == ov_set_length(set));

    int count = 0;
    check(ov_set_apply_args(set, apply_args, &count) == set);
    check(count > 0);
    check(count == ov_set_length(set));

    ov_set_reset(set);
    check(ov_set_length(set) == 0);

    add = 100;
    for (int i = 100; i > 0; --i)
    {
        check(*(out = ov_set_insert(set, &i)) == i);
        check(!ov_set_insert(set, &i));
        check(*(out = ov_set_has(set, &i)) == i);
    }
    check(ov_set_length(set) == add);

    has_not = -123;
    check(!ov_set_has(set, &has_not));
    has_not = 123;
    check(!ov_set_has(set, &has_not));

    n = 99;
    check(ov_set_remove(set, &n) == set);
    check(ov_set_remove(set, &n) == NULL);
    check(ov_set_remove(set, &n) == NULL);
    n = 1;
    check(ov_set_remove(set, &n) == set);
    check(ov_set_remove(set, &n) == NULL);
    check(ov_set_remove(set, &n) == NULL);
    check(ov_set_remove_at(set, 0) == set);
    check(ov_set_remove_at(set, ov_set_length(set)-1) == set);

    int new_sz = 96;
    check(ov_set_length(set) == new_sz);
    for (int i = 0, offs = 3; i < new_sz; ++i)
    {
        int val = i+offs;
        check(*(out = ov_set_get(set, i)) == val);
        check(*(out = ov_set_has(set, &val)) == val);
    }

    ov_set_destroy(set);

    ov_set_make(set, sizeof(int), compar);

    int iarr[] = {5, 1, 3, 3, 2, 5, 5, 1, 1, 7, 4, 7, 4, 1, 5, 6};
    int iarr_sz = sizeof(iarr)/sizeof(*iarr);

    for (int i = 0; i < iarr_sz; ++i)
        ov_set_insert(set, iarr+i);

    new_sz = 7;
    check(ov_set_length(set) == new_sz);

    for (int i = 0, offs = 1; i < new_sz; ++i)
        check(*(out = ov_set_get(set, i)) == i+offs);

    ov_set_destroy(set);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_ov_set(void)
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

int tov_set_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tov_set_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
