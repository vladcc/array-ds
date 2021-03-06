#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "test.h"
#include "array_pool.h"
#include <limits.h>

static bool test_apl_make(void);
static bool test_apl_alignment(void);

static ftest tests[] = {
    test_apl_make,
    test_apl_alignment,
};
//------------------------------------------------------------------------------

static bool test_apl_alignment(void)
{
// copy from array_pool.c
typedef struct apl_node {
    byte * mem;
    int capacity;
    int bytes_left;
} apl_node;

    array_pool _apl, * apl = &_apl;
    c_vector * the_pool = NULL;
    apl_node * node = NULL;
    byte * result = NULL;

    int seg_size = 9;
    check(apl_make_size(apl, seg_size) == apl);
    the_pool = &(apl->the_pool);
    node = c_vect_get(the_pool, 0);

    check(node->capacity == seg_size);
    check(node->capacity == node->bytes_left);

    char string1[] = "aaa";
    const int len_str1 = strlen(string1);
    char string2[] = "bbbb";
    const int len_str2 = strlen(string2);

    result = (byte *)apl_insert_string(apl, string1, len_str1);
    check(strcmp((char *)result, string1) == 0);
    check(node->bytes_left == node->capacity-sizeof(string1));
    check(result == node->mem);

    byte * old = result;
    result = (byte *)apl_insert_string(apl, string2, len_str2);
    check(strcmp((char *)result, string2) == 0);
    check(node->bytes_left == node->capacity-sizeof(string1)-sizeof(string2));
    check(!node->bytes_left);
    check(old == result-sizeof(string1));

    char string3[] = "The quick brown fox jumped over the lazy dog";
    result = apl_insert(apl, string3, sizeof(string3), 1);
    check(2 == c_vect_length(the_pool));
    check(strcmp((char *)result, string3) == 0);

    node = c_vect_get(the_pool, 1);
    check(result == node->mem);
    check(node->capacity == sizeof(string3));
    check(!node->bytes_left);

    result = apl_get(apl, sizeof(char));
    *result = 'a';
    check(3 == c_vect_length(the_pool));
    node = c_vect_get(the_pool, 2);
    check(node->capacity == seg_size);
    check(node->bytes_left == node->capacity-sizeof(char));

    old = result;
    int sz = sizeof(int);
    result = apl_get_align(apl, sz, sz);
    *(int*)result = 0x626f6f62;
    check(result == old+sizeof(char)+(sz-sizeof(char)));
    check((uintptr_t)result % sz == 0);
    check(node->bytes_left = node->capacity-sizeof(char)-sz);

    check(!apl_get_align(apl, 32, 0));
    check(!apl_get_align(apl, 32, -5));
    check(!apl_get_align(apl, 32, 23));
    check(!apl_get_align(apl, INT_MAX, 2));
    check(!apl_get(apl, INT_MAX));
    check(!apl_get(apl, 0));
    check(!apl_get(apl, -5));

    check(apl_reuse(apl));
    check(3 == c_vect_length(the_pool));

    for (int i = 0; i < 3; ++i)
    {
        node = c_vect_get(the_pool, i);
        check(node->bytes_left == node->capacity);
    }

    check(apl_reset(apl));
    the_pool = &(apl->the_pool);
    node = c_vect_get(the_pool, 0);
    check(1 == c_vect_length(the_pool));
    check(node->bytes_left == node->capacity);
    check(node->capacity == seg_size);

    apl_destroy(apl);

    check(apl_make(apl) == apl);
    the_pool = &(apl->the_pool);
    node = c_vect_get(the_pool, 0);
    check(node->capacity == APL_DEF_SIZE);

    result = apl_get_align(apl, 64, 32);
    check((uintptr_t)result % 32 == 0);

    result = apl_get_align(apl, 1, 1);
    result = apl_get_align(apl, 1, 1);

    old = result;

    int arr[] = {1, 2, 3, 4, 5};
    check((uintptr_t)result % sizeof(int) != 0);
    result = apl_insert(apl, arr, sizeof(arr)/sizeof(*arr), sizeof(*arr));
    check((uintptr_t)result % sizeof(*arr) == 0);
    check(old+3 == result);
    check(memcmp(result, arr, sizeof(arr)) == 0);

    result = apl_get_align(apl, 1, 1);
    result = apl_get_align(apl, 1, 1);
    check((uintptr_t)result % APL_DEF_ALIGN != 0);
    result = apl_get(apl, 1);
    check((uintptr_t)result % APL_DEF_ALIGN == 0);

    result = apl_get_align(apl, 1, 1);
    result = apl_get_align(apl, 1, 1);
    int align = 64;
    check((uintptr_t)result % align != 0);
    result = apl_insert_align(apl, arr,
        sizeof(arr)/sizeof(*arr), sizeof(*arr), align);
    check((uintptr_t)result % align == 0);
    check(memcmp(result, arr, sizeof(arr)) == 0);

    result = apl_get_align(apl, 1, 1);
    result = apl_get_align(apl, 1, 1);
    align = 8;
    check((uintptr_t)result % align != 0);
    result = (byte *)apl_insert_string_align(apl,
        string3, strlen(string3), align
    );
    check((uintptr_t)result % align == 0);
    check(strcmp((char *)result, string3) == 0);

    old = result;
    result = (byte *)apl_insert_string(apl, string1, len_str1);
    check(result > old);
    check(strcmp((char *)result, string1) == 0);
    check(strcmp((char *)old, string3) == 0);

    apl_destroy(apl);
    return true;
}
//------------------------------------------------------------------------------

static bool test_apl_make(void)
{
    array_pool _apl, * apl = &_apl;

    check(!apl_make_size(apl, -1));
    check(!apl_make_size(apl, 0));

    check(apl_make(apl));
    apl_destroy(apl);
    return true;
}

//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_array_pool(void)
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

int tarr_pool_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tarr_pool_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
