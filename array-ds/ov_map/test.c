#include <stdio.h>
#include <string.h>
#include "ov_map.h"
#include "test.h"

static bool test_ov_map_str(void);
static bool test_ov_map_int_d(void);

static ftest tests[] = {
    test_ov_map_str,
    test_ov_map_int_d
};
//------------------------------------------------------------------------------

typedef struct int_d {
    double d;
    int key;
} int_d;

static int int_d_compar(const void * one, const void * two)
{
    int a = ((int_d *)one)->key;
    int b = ((int_d *)two)->key;

    int comp = 0;
    if (a > b)
        comp = 1;
    else if (a < b)
        comp = -1;

    return comp;
}

static bool test_ov_map_int_d(void)
{
    ov_map map_, * map = &map_;
    ov_map_make(map, sizeof(int_d), int_d_compar);

    int end = 100;
    for (int i = end; i >= 1; --i)
    {
        int_d n = {.key = i, .d = end-(i-1)};
        ov_map_insert(map, &n);
    }
    check(ov_map_length(map) == end);

    int_d n = {.key = 1, .d = -5};
    int_d * pn = ov_map_find(map, &n);
    check(pn);
    check(pn->key == 1);
    check(pn->d == end);

    check(ov_map_insert(map, &n) == pn);
    check(pn->key == 1);
    check(pn->d == end);

    for (int i = 0; i < end; ++i)
    {
        int_d * n = ov_map_get(map, i);
        check(n->key == i+1);
        check(n->d == end-i);
    }

    int_d tmp = {.key = -123};
    check(!ov_map_remove(map, &tmp));
    check(ov_map_length(map) == end);
    tmp.key = 123;
    check(!ov_map_remove(map, &tmp));
    check(ov_map_length(map) == end);
    tmp.key = 2;
    check(ov_map_remove(map, &tmp) == map);
    check(ov_map_length(map) == end-1);
    check(ov_map_remove_at(map, 0) == map);
    check(ov_map_length(map) == end-1-1);
    tmp.key = 100;
    check(ov_map_remove(map, &tmp) == map);
    check(ov_map_length(map) == end-1-1-1);
    int new_end = end-3;
    for (int i = 0; i < new_end; ++i)
    {
        int_d * n = ov_map_get(map, i);
        check(n->key == i+3);
        check(n->d == new_end-i+1);
    }

    ov_map_destroy(map);
    return true;
}
//------------------------------------------------------------------------------

typedef struct str_test {
    const char * key;
    int val;
} str_test;

static int str_compar(const void * k1, const void * k2)
{
    const char * p1 = *((const char **)k1);
    const char * p2 = *((const char **)k2);
    return strcmp(p1, p2);
}

static bool test_ov_map_str(void)
{
    static char one[] =  "one";
    static char two[] =  "two";
    static char three[] =  "three";
    static char four[] =  "four";
    static char five[] =  "five";

    enum {ONE, TWO, THREE, FOUR, FIVE, ALL};
    str_test test[ALL] = {
        {.key = one, .val = 0},
        {.key = two, .val = 0},
        {.key= three, .val = 0},
        {.key = four, .val = 0},
        {.key = five, .val = 0}
    };

    ov_map map_, * map = &map_;
    ov_map_make(map, sizeof(str_test), str_compar);

    check(ov_map_length(map) == 0);
    for (int i = 0; i < ALL; ++i)
        ov_map_insert(map, test+i);
    check(ov_map_length(map) == ALL);

    char * strs[] = {
    "badger", "badger badger",
    "one",
    "two", "two",
    "three", "three", "three",
    "four", "four", "four", "four",
    "five", "five", "five", "five", "five",
    "six", "six", "six", "six", "six", "six",
    "banana",
    };

    int strs_sz = sizeof(strs)/sizeof(*strs);
    for (int i = 0; i < strs_sz; ++i)
    {
        str_test * n = ov_map_find(map, strs+i);
        if (n)
            n->val++;
    }

    check(ov_map_length(map) == ALL);
    char * strntbl[] = {one, two, three, four, five};
    for (int i = 0, j = 1; i < ALL; ++i, ++j)
    {
        str_test * n = ov_map_find(map, strntbl+i);
        check(strcmp(n->key, strntbl[i]) == 0);
        check(n->val == j);
    }

    char badger[] = "badger";
    str_test badg = {.key = badger, .val = -1};
    check(!ov_map_remove(map, &badg));
    check(ov_map_length(map) == ALL);
    check(ov_map_remove(map, &test[ONE]) == map);
    check(ov_map_length(map) == ALL-1);

    check(!ov_map_find(map, &test[ONE]));
    for (int i = 1, j = 2; i < ALL; ++i, ++j)
    {
        str_test * n = ov_map_find(map, strntbl+i);
        check(strcmp(n->key, strntbl[i]) == 0);
        check(n->val == j);

        str_test tn = *n;
        tn.val = -123;
        str_test * ptn = ov_map_insert(map, &tn);
        check(ptn == n);
        check(ptn->val == n->val);
    }

    ov_map_destroy(map);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_ov_map(void)
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

int tov_map_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tov_map_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
