#include <stdio.h>
#include "trie.h"
#include "test.h"

static bool test_(void);

static ftest tests[] = {
    test_,
};
//------------------------------------------------------------------------------

static bool test_(void)
{
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
