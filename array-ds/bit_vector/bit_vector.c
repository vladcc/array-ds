#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "bit_vector.h"

typedef unsigned char byte;
//------------------------------------------------------------------------------

void * bit_vect_make(bit_vector * bv, int bits)
{
    void * ret = NULL;
    int bytes = (bits / CHAR_BIT) + ((bits % CHAR_BIT) ? 1 : 0);

    if (bits > 0 && (ret = calloc(bytes, sizeof(byte))))
    {
        bv->the_bits = ret;
        bv->bits_count = bits;
        ret = bv;
    }
    else
        memset(bv, 0, sizeof(*bv));

    return ret;
}
//------------------------------------------------------------------------------

void bit_vect_destroy(bit_vector * bv)
{
    free(bv->the_bits);
    memset(bv, 0, sizeof(*bv));
}
//------------------------------------------------------------------------------

bit_vector bit_vect_copy(const bit_vector * src, bool * out_success)
{
    bool ok = false;

    const unsigned char * src_mem = src->the_bits;
    int bits = src->bits_count;
    int bytes = (bits / CHAR_BIT) + ((bits % CHAR_BIT) ? 1 : 0);

    bit_vector ret;
    memset(&ret, 0, sizeof(ret));

    unsigned char * new_bv = malloc(bytes);
    if (new_bv)
    {
        memcpy(new_bv, src_mem, bytes);
        ret.the_bits = new_bv;
        ret.bits_count = bits;
        ok = true;
    }

    if (out_success)
        *out_success = ok;

    return ret;
}
//------------------------------------------------------------------------------

void * bit_vect_set_byte_pattern(bit_vector * bv, unsigned char val)
{
    int bits = bv->bits_count;
    int bytes = (bits / CHAR_BIT) + ((bits % CHAR_BIT) ? 1 : 0);
    memset(bv->the_bits, val, bytes);
    return bv;
}
//------------------------------------------------------------------------------

void * bit_vect_get(const bit_vector * bv, int bit_num, int * out_bit_val)
{
    void * ret = NULL;
    int all_bits = bv->bits_count;
    byte * the_bits = bv->the_bits;

    if ((unsigned int)bit_num < all_bits)
    {
        int target = bit_num % CHAR_BIT;
        *out_bit_val = (the_bits[bit_num/CHAR_BIT] & (1 << target)) >> target;
        ret = (void *)bv;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * bit_vect_set(bit_vector * bv, int bit_num)
{
    void * ret = NULL;
    int all_bits = bv->bits_count;
    byte * the_bits = bv->the_bits;

    if ((unsigned int)bit_num < all_bits)
    {
        the_bits[bit_num/CHAR_BIT] |= (1 << (bit_num % CHAR_BIT));
        ret = bv;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * bit_vect_clear(bit_vector * bv, int bit_num)
{
    void * ret = NULL;
    int all_bits = bv->bits_count;
    byte * the_bits = bv->the_bits;

    if ((unsigned int)bit_num < all_bits)
    {
        the_bits[bit_num/CHAR_BIT] &= ~(1 << (bit_num % CHAR_BIT));
        ret = bv;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * bit_vect_toggle(bit_vector * bv, int bit_num)
{
    void * ret = NULL;
    int all_bits = bv->bits_count;
    byte * the_bits = bv->the_bits;

    if ((unsigned int)bit_num < all_bits)
    {
        the_bits[bit_num/CHAR_BIT] ^= (0x01 << (bit_num % CHAR_BIT));
        ret = bv;
    }

    return ret;
}
//------------------------------------------------------------------------------

int bit_vect_num_of_bits(const bit_vector * bv)
{
    return bv->bits_count;
}
//------------------------------------------------------------------------------
