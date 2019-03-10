#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "bit_vector.h"
#include "test.h"

typedef unsigned char byte;

static bool test_make(void);
static bool test_set_bytes(void);
static bool test_get(void);
static bool test_set(void);
static bool test_clear(void);
static bool test_toggle(void);
static bool test_copy(void);

static ftest tests[] = {
    test_make,
    test_set_bytes,
    test_get,
    test_set,
    test_clear,
    test_toggle,
    test_copy
};
//------------------------------------------------------------------------------

static bool test_make(void)
{
    bit_vector bv_, * bv = &bv_;
    memset(bv, 0xFF, sizeof(bit_vector));

    bit_vector zout;
    memset(&zout, 0, sizeof(bit_vector));

    check(memcmp(&zout, bv, sizeof(bit_vector)) != 0);
    check(!bit_vect_make(bv, 0));
    check(memcmp(&zout, bv, sizeof(bit_vector)) == 0);

    memset(bv, 0xFF, sizeof(bit_vector));
    check(memcmp(&zout, bv, sizeof(bit_vector)) != 0);
    check(!bit_vect_make(bv, -123));
    check(memcmp(&zout, bv, sizeof(bit_vector)) == 0);

    int end = 133;
    for (int i = 1; i <= end; ++i)
    {
        check(bit_vect_make(bv, i) == bv);
        check(bv->bits_count == i);

        int bytes = i / CHAR_BIT;
        int extra = ((i % CHAR_BIT) ? 1 : 0);
        int all_b = bytes + extra;

        check((bv->bits_count/CHAR_BIT + ((bv->bits_count % CHAR_BIT) ? 1 : 0))
            == all_b);

        byte * data = bv->the_bits;
        for (int j = 0; j < all_b; ++j)
            check(data[j] == 0);

        bit_vect_destroy(bv);
    }

    return true;
}

//------------------------------------------------------------------------------

static bool test_set_bytes(void)
{
    bit_vector bv_, * bv = &bv_;
    int bits = 257;

    check(bit_vect_make(bv, bits) == bv);

    byte * data = bv->the_bits;
    int bytes = bits / CHAR_BIT;
    int extra = ((bits % CHAR_BIT) ? 1 : 0);
    int all_b = bytes + extra;

    check(all_b == 33);
    check((bv->bits_count/CHAR_BIT + ((bv->bits_count % CHAR_BIT) ? 1 : 0))
            == all_b);

    static unsigned char vals[] = {0x00, 0xFF, 0x0A, 0xA0, 0x55, 0xAA, 0x12};
    int vals_sz = sizeof(vals)/sizeof(*vals);

    for (int i = 0; i < vals_sz; ++i)
    {
        check(bit_vect_set_byte_pattern(bv, vals[i]) == bv);
        for (int j = 0; j < all_b; ++j)
            check(data[j] == vals[i]);
    }

    bit_vect_destroy(bv);

    check(bit_vect_make(bv, bits) == bv);
    data = bv->the_bits;

    check(bit_vect_set_all(bv) == bv);
    for (int j = 0; j < all_b; ++j)
            check(data[j] == 0xFF);

    check(bit_vect_clear_all(bv) == bv);
    for (int j = 0; j < all_b; ++j)
        check(data[j]  == 0x00);

    bit_vect_destroy(bv);
    return true;
}
//------------------------------------------------------------------------------

static bool test_get(void)
{
    bit_vector bv_, * bv = &bv_;
    int bits = 257;

    check(bit_vect_make(bv, bits) == bv);

    byte * data = bv->the_bits;
    int bytes = bits / CHAR_BIT;
    int extra = ((bits % CHAR_BIT) ? 1 : 0);
    int all_b = bytes + extra;

    check(all_b == 33);
    check((bv->bits_count/CHAR_BIT + ((bv->bits_count % CHAR_BIT) ? 1 : 0))
            == all_b);

    int bit_val;
    check(!bit_vect_get(bv, -123, &bit_val));
    check(!bit_vect_get(bv, 9999, &bit_val));

    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        check(bit_val == 0);
    }

    check(bit_vect_set_all(bv) == bv);
    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        check(bit_val == 1);
    }

    int patt = 0x55; //0101 0101
    check(bit_vect_set_byte_pattern(bv, patt) == bv);
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);

    int i;
    int toggle_val = 0;
    for (i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        toggle_val ^= 1;
        check(bit_val == toggle_val);
    }
    check(!bit_vect_get(bv, i, &bit_val));

    patt = 0xAA; //1010 1010
    check(bit_vect_set_byte_pattern(bv, patt) == bv);
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);

    toggle_val = 1;
    for (i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        toggle_val ^= 1;
        check(bit_val == toggle_val);
    }
    check(!bit_vect_get(bv, i, &bit_val));

    patt = 0xBC; //1011 1100
    check(bit_vect_set_byte_pattern(bv, patt) == bv);
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);

    int end = bits-1;
    for (i = 0; i < end; i += CHAR_BIT)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        check(bit_val == 0);
        check(bit_vect_get(bv, i+1, &bit_val) == bv);
        check(bit_val == 0);
        check(bit_vect_get(bv, i+2, &bit_val) == bv);
        check(bit_val == 1);
        check(bit_vect_get(bv, i+3, &bit_val) == bv);
        check(bit_val == 1);
        check(bit_vect_get(bv, i+4, &bit_val) == bv);
        check(bit_val == 1);
        check(bit_vect_get(bv, i+5, &bit_val) == bv);
        check(bit_val == 1);
        check(bit_vect_get(bv, i+6, &bit_val) == bv);
        check(bit_val == 0);
        check(bit_vect_get(bv, i+7, &bit_val) == bv);
        check(bit_val == 1);
    }

    check(bit_vect_get(bv, end, &bit_val) == bv);
    check(bit_val == 0);

    check(!bit_vect_get(bv, end+1, &bit_val));

    bit_vect_destroy(bv);
    return true;
}
//------------------------------------------------------------------------------

static bool test_set(void)
{
    bit_vector bv_, * bv = &bv_;
    int bits = 256;

    check(bit_vect_make(bv, bits) == bv);

    byte * data = bv->the_bits;
    int bytes = bits / CHAR_BIT;
    int extra = ((bits % CHAR_BIT) ? 1 : 0);
    int all_b = bytes + extra;

    check(all_b == 32);
    check((bv->bits_count/CHAR_BIT + ((bv->bits_count % CHAR_BIT) ? 1 : 0))
            == all_b);

    int patt = 0x00;
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);

    int bit_val = 0;
    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        check(bit_val == 0);
        check(bit_vect_set(bv, i) == bv);
        check(bit_vect_get(bv, i, &bit_val) == bv);
        check(bit_val == 1);
    }

    check(bit_vect_clear_all(bv) == bv);

    enum { VAL1 = 0, VAL2 = 1, VAL3 = 255, VAL4 = 37, VAL5 = 127, VAL6 = 128,
        VAL7 = 251};

    check(bit_vect_get(bv, VAL1, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_set(bv, VAL1) == bv);
    check(bit_vect_get(bv, VAL1, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL2, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_set(bv, VAL2) == bv);
    check(bit_vect_get(bv, VAL2, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL3, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_set(bv, VAL3) == bv);
    check(bit_vect_get(bv, VAL3, &bit_val) == bv);
    check(bit_val == 1);

    check(bit_vect_get(bv, VAL4, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_set(bv, VAL4) == bv);
    check(bit_vect_get(bv, VAL4, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL5, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_set(bv, VAL5) == bv);
    check(bit_vect_get(bv, VAL5, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL6, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_set(bv, VAL6) == bv);
    check(bit_vect_get(bv, VAL6, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL7, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_set(bv, VAL7) == bv);
    check(bit_vect_get(bv, VAL7, &bit_val) == bv);
    check(bit_val == 1);

    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        switch (i)
        {
            case VAL1:
            case VAL2:
            case VAL3:
            case VAL4:
            case VAL5:
            case VAL6:
            case VAL7:
                check(bit_val == 1);
                break;
            default:
                check(bit_val == 0);
                break;
        }
    }

    bit_vect_destroy(bv);
    return true;
}
//------------------------------------------------------------------------------

static bool test_clear(void)
{
    bit_vector bv_, * bv = &bv_;
    int bits = 257;

    check(bit_vect_make(bv, bits) == bv);

    byte * data = bv->the_bits;
    int bytes = bits / CHAR_BIT;
    int extra = ((bits % CHAR_BIT) ? 1 : 0);
    int all_b = bytes + extra;

    check(all_b == 33);
    check((bv->bits_count/CHAR_BIT + ((bv->bits_count % CHAR_BIT) ? 1 : 0))
            == all_b);

    check(bit_vect_set_all(bv) == bv);

    int patt = 0xFF;
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);


    enum { VAL1 = 0, VAL2 = 1, VAL3 = 256, VAL4 = 37, VAL5 = 127, VAL6 = 128,
        VAL7 = 251};

    check(!bit_vect_clear(bv, -9123));
    check(!bit_vect_clear(bv, 9123));

    int bit_val;
    check(bit_vect_get(bv, VAL1, &bit_val) == bv);
    check(bit_val == 1);
    check(bit_vect_clear(bv, VAL1) == bv);
    check(bit_vect_get(bv, VAL1, &bit_val) == bv);
    check(bit_val == 0);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL2, &bit_val) == bv);
    check(bit_val == 1);
    check(bit_vect_clear(bv, VAL2) == bv);
    check(bit_vect_get(bv, VAL2, &bit_val) == bv);
    check(bit_val == 0);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL3, &bit_val) == bv);
    check(bit_val == 1);
    check(bit_vect_clear(bv, VAL3) == bv);
    check(bit_vect_get(bv, VAL3, &bit_val) == bv);
    check(bit_val == 0);

    check(bit_vect_get(bv, VAL4, &bit_val) == bv);
    check(bit_val == 1);
    check(bit_vect_clear(bv, VAL4) == bv);
    check(bit_vect_get(bv, VAL4, &bit_val) == bv);
    check(bit_val == 0);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL5, &bit_val) == bv);
    check(bit_val == 1);
    check(bit_vect_clear(bv, VAL5) == bv);
    check(bit_vect_get(bv, VAL5, &bit_val) == bv);
    check(bit_val == 0);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL6, &bit_val) == bv);
    check(bit_val == 1);
    check(bit_vect_clear(bv, VAL6) == bv);
    check(bit_vect_get(bv, VAL6, &bit_val) == bv);
    check(bit_val == 0);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL7, &bit_val) == bv);
    check(bit_val == 1);
    check(bit_vect_clear(bv, VAL7) == bv);
    check(bit_vect_get(bv, VAL7, &bit_val) == bv);
    check(bit_val == 0);

    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        switch (i)
        {
            case VAL1:
            case VAL2:
            case VAL3:
            case VAL4:
            case VAL5:
            case VAL6:
            case VAL7:
                check(bit_val == 0);
                break;
            default:
                check(bit_val == 1);
                break;
        }
    }

    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_clear(bv, i) == bv);
        check(bit_vect_get(bv, i, &bit_val) == bv);
        check(bit_val == 0);
    }

    patt = 0x00;
    int j, end = all_b-1;
    for (j = 0; j < end; ++j)
        check(data[j] == patt);
    check(data[j] == 0xFE);

    bit_vect_destroy(bv);
    return true;
}
//------------------------------------------------------------------------------

static bool test_toggle(void)
{
    bit_vector bv_, * bv = &bv_;
    int bits = 259;
    check(bit_vect_make(bv, bits) == bv);

    byte * data = bv->the_bits;
    int bytes = bits / CHAR_BIT;
    int extra = ((bits % CHAR_BIT) ? 1 : 0);
    int all_b = bytes + extra;

    check(all_b == 33);
    check((bv->bits_count/CHAR_BIT + ((bv->bits_count % CHAR_BIT) ? 1 : 0))
            == all_b);

    check(bit_vect_num_of_bits(bv) == bits);

    int patt = 0x00;
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);

    for (int i = 0; i < bits; ++i)
        check(bit_vect_toggle(bv, i) == bv);

    patt = 0xFF;
    int end = all_b-1, j;
    for (j = 0; j < end; ++j)
        check(data[j] == patt);
    check(data[j] == 0x07);

    for (int i = 0; i < bits; ++i)
        check(bit_vect_toggle(bv, i) == bv);

    patt = 0x00;
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);

    enum { VAL1 = 0, VAL2 = 1, VAL3 = 256, VAL4 = 37, VAL5 = 127, VAL6 = 128,
        VAL7 = 251};

    check(!bit_vect_toggle(bv, -9123));
    check(!bit_vect_toggle(bv, 9123));

    int bit_val;
    check(bit_vect_get(bv, VAL1, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_toggle(bv, VAL1) == bv);
    check(bit_vect_get(bv, VAL1, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL2, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_toggle(bv, VAL2) == bv);
    check(bit_vect_get(bv, VAL2, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL3, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_toggle(bv, VAL3) == bv);
    check(bit_vect_get(bv, VAL3, &bit_val) == bv);
    check(bit_val == 1);

    check(bit_vect_get(bv, VAL4, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_toggle(bv, VAL4) == bv);
    check(bit_vect_get(bv, VAL4, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL5, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_toggle(bv, VAL5) == bv);
    check(bit_vect_get(bv, VAL5, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL6, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_toggle(bv, VAL6) == bv);
    check(bit_vect_get(bv, VAL6, &bit_val) == bv);
    check(bit_val == 1);
    check(!bit_vect_get(bv, bits, &bit_val));

    check(bit_vect_get(bv, VAL7, &bit_val) == bv);
    check(bit_val == 0);
    check(bit_vect_toggle(bv, VAL7) == bv);
    check(bit_vect_get(bv, VAL7, &bit_val) == bv);
    check(bit_val == 1);

    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        switch (i)
        {
            case VAL1:
            case VAL2:
            case VAL3:
            case VAL4:
            case VAL5:
            case VAL6:
            case VAL7:
                check(bit_val == 1);
                break;
            default:
                check(bit_val == 0);
                break;
        }
    }

    for (int i = 0; i < bits; ++i)
        check(bit_vect_toggle(bv, i) == bv);

    for (int i = 0; i < bits; ++i)
    {
        check(bit_vect_get(bv, i, &bit_val) == bv);
        switch (i)
        {
            case VAL1:
            case VAL2:
            case VAL3:
            case VAL4:
            case VAL5:
            case VAL6:
            case VAL7:
                check(bit_val == 0);
                break;
            default:
                check(bit_val == 1);
                break;
        }
    }

    bit_vect_destroy(bv);
    return true;
}
//------------------------------------------------------------------------------

static bool test_copy(void)
{
    bit_vector bv_, * bv = &bv_;
    int bits = 259;
    check(bit_vect_make(bv, bits) == bv);

    byte * data = bv->the_bits;
    int bytes = bits / CHAR_BIT;
    int extra = ((bits % CHAR_BIT) ? 1 : 0);
    int all_b = bytes + extra;

    check(all_b == 33);
    check((bv->bits_count/CHAR_BIT + ((bv->bits_count % CHAR_BIT) ? 1 : 0))
            == all_b);

    check(bit_vect_num_of_bits(bv) == bits);

    for (int j = 0; j < all_b; ++j)
        check(data[j] == 0);

    int patt = 0xCB; //1100 1011
    check(bit_vect_set_byte_pattern(bv, patt) == bv);
    for (int j = 0; j < all_b; ++j)
        check(data[j] == patt);

    bool success = false;
    bit_vector cpy = bit_vect_copy(bv, &success);

    byte * data2 = cpy.the_bits;
    check(memcmp(data, data2, all_b) == 0);

    bit_vect_destroy(bv);

    for (int j = 0; j < all_b; ++j)
        check(data2[j] == patt);

    bit_vector cpy2 = bit_vect_copy(&cpy, NULL);
    bit_vect_destroy(&cpy2);
    bit_vect_destroy(&cpy);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_bit_vect(void)
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

int tbit_vect_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tbit_vec_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
