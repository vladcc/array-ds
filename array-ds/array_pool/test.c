#include "test.h"

static bool test_apl_make(void);

static ftest tests[] = {
    test_apl_make,
};
//------------------------------------------------------------------------------

static bool test_apl_make(void)
{
    check(0);
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
