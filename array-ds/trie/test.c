#include <stdio.h>
#include <string.h>
#include "trie.h"
#include "test.h"

static bool test_trie(void);

static ftest tests[] = {
    test_trie,
};
//------------------------------------------------------------------------------

static void prepare_vals(const char * str, trie_val * arr, int len)
{
    for (int i = 0; i < len; ++i)
    {
        arr[i].val = str[i];
        arr[i].tag = 0;
    }
}

static bool test_trie(void)
{
#define LEN 100
    trie_val vals[LEN];
    trie tre_, * the_trie = &tre_;

    check(trie_make(the_trie) == the_trie);

    int len;
    len = strlen("abc");
    prepare_vals("abc", vals, len);

    check(trie_insert(the_trie, vals, len) == the_trie);

    trie_destroy(the_trie);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_trie(void)
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

int ttrie_set_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int ttrie_set_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
