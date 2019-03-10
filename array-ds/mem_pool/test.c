#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mem_pool.h"
#include "test.h"

static bool test_make_destroy(void);
static bool test_get_reset(void);
static bool test_use(void);

static ftest tests[] = {
    test_make_destroy,
    test_get_reset,
    test_use
};
//------------------------------------------------------------------------------

static bool test_make_destroy(void)
{
    int nsize = sizeof(int);
    int ncap = 8;

    mem_pool mpl_, * mpl = &mpl_;
    memset(mpl, 0xFF, sizeof(mem_pool));

    mem_pool zout;
    memset(&zout, 0, sizeof(mem_pool));

    check(memcmp(&zout, mpl, sizeof(mem_pool)) != 0);
    check(!mpl_make(mpl, 0, ncap));
    check(memcmp(&zout, mpl, sizeof(mem_pool)) == 0);

    memset(mpl, 0xFF, sizeof(mem_pool));
    check(memcmp(&zout, mpl, sizeof(mem_pool)) != 0);
    check(!mpl_make(mpl, -123, ncap));
    check(memcmp(&zout, mpl, sizeof(mem_pool)) == 0);

    memset(mpl, 0xFF, sizeof(mem_pool));
    check(memcmp(&zout, mpl, sizeof(mem_pool)) != 0);
    check(!mpl_make(mpl, nsize, 0));
    check(memcmp(&zout, mpl, sizeof(mem_pool)) == 0);

    memset(mpl, 0xFF, sizeof(mem_pool));
    check(memcmp(&zout, mpl, sizeof(mem_pool)) != 0);
    check(!mpl_make(mpl, nsize, -123));
    check(memcmp(&zout, mpl, sizeof(mem_pool)) == 0);

    check(mpl_make(mpl, nsize, ncap) == mpl);

    check(mpl->elem_size == nsize);
    check(mpl->node_cap == ncap);

    c_vector * the_pool = &(mpl->the_pool);
    check(c_vect_length(the_pool) == 1);

    check(memcmp(&zout, mpl, sizeof(mem_pool)) != 0);
    mpl_destroy(mpl);
    check(memcmp(&zout, mpl, sizeof(mem_pool)) == 0);

    return true;
}

//------------------------------------------------------------------------------

typedef double test_item;
static bool test_get_reset(void)
{
    int nsize = sizeof(test_item);
    int ncap = 4;

    mem_pool mpl_, * mpl = &mpl_;
    check(mpl_make(mpl, nsize, ncap) == mpl);

    check(mpl->elem_size == nsize);
    check(mpl->node_cap == ncap);

    c_vector * the_pool = &(mpl->the_pool);
    check(c_vect_length(the_pool) == 1);

    void * node, * node_last = NULL;
    for (int i = 0; i < ncap; ++i)
    {
        check(node = mpl_get(mpl));

        if (node_last)
            check((node - node_last) == sizeof(test_item));

        node_last = node;

        check(c_vect_length(the_pool) == 1);
    }

    check(node = mpl_get(mpl));
    check(c_vect_length(the_pool) == 2);

    int end = ncap;
    for (int i = 0; i < end; ++i)
        check(node = mpl_get(mpl));
    check(c_vect_length(the_pool) == 3);

    check(mpl_reset(mpl) == mpl);

    the_pool = &(mpl->the_pool);
    check(c_vect_length(the_pool) == 1);
    check(mpl->elem_size == nsize);
    check(mpl->node_cap == ncap);

    end = ncap * (ncap + 1);
    for (int i = 0; i < end; ++i)
        check(node = mpl_get(mpl));
    check(c_vect_length(the_pool) == ncap+1);

    mpl_destroy(mpl);
    return true;
}
//------------------------------------------------------------------------------

static bool test_use(void)
{
    int nsize = sizeof(int);
    int ncap = 6;

    mem_pool mpl_, * mpl = &mpl_;
    check(mpl_make(mpl, nsize, ncap) == mpl);

    {
        check(mpl->elem_size == nsize);
        check(mpl->node_cap == ncap);

        c_vector * the_pool = &(mpl->the_pool);
        check(c_vect_length(the_pool) == 1);

        int end = ncap*4;
        for (int i = 0; i < end; ++i)
            *((int *)mpl_get(mpl)) = i;

        check(c_vect_length(the_pool) == 4);
        *((int *)mpl_get(mpl)) = 777;
        check(c_vect_length(the_pool) == 5);

        int z = 0;
        for (int i = 0; i <= 4-1; ++i)
        {
            int * pi = *((int **)c_vect_get(the_pool, i));
            for (int j = 0; j < ncap; ++j, ++z)
                check(pi[j] == z);
        }
        check(z == end);

        int * pi = *((int **)c_vect_get(the_pool, 5-1));
        *pi = 777;
    }
    check(mpl_reset(mpl) == mpl);

    {
        check(mpl->elem_size == nsize);
        check(mpl->node_cap == ncap);

        c_vector * the_pool = &(mpl->the_pool);
        check(c_vect_length(the_pool) == 1);

        int end = ncap*4;
        for (int i = 0; i < end; ++i)
            *((int *)mpl_get(mpl)) = i;

        check(c_vect_length(the_pool) == 4);
        *((int *)mpl_get(mpl)) = 777;
        check(c_vect_length(the_pool) == 5);

        int z = 0;
        for (int i = 0; i <= 4-1; ++i)
        {
            int * pi = *((int **)c_vect_get(the_pool, i));
            for (int j = 0; j < ncap; ++j, ++z)
                check(pi[j] == z);
        }
        check(z == end);

        int * pi = *((int **)c_vect_get(the_pool, 5-1));
        *pi = 777;
    }

    mpl_destroy(mpl);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_mem_pool(void)
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

int tmem_pool_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tmem_pool_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
