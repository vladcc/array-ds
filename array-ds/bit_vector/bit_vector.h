/*  bit_vector.h -- a bit vector
    v1.1

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-03-02
*/

#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include <stdbool.h>

#define BV_ALL_ONES 0xFF
#define BV_ALL_ZERO 0x00

// Do not use directly
typedef struct bit_vector {
    unsigned char * the_bits;
    int bits_count;
} bit_vector;

void * bit_vect_make(bit_vector * bv, int bits);
/*
Returns: bv on success, NULL otherwise.

Description: Creates a bit vector of bits number of bits, all off. On failure
*bv is zeroed out.

Complexity: O(1)
*/

void bit_vect_destroy(bit_vector * bv);
/*
Returns: Nothing.

Description: Destroys the bit vector, zeroes *bv out.

Complexity: O(1)
*/

bit_vector bit_vect_copy(const bit_vector * src, bool * out_success);
/*
Returns: A deep copy of src on success, a zeroed out bit_vector otherwise.

Description: Makes a deep copy of src. *out_succes is true if the copy was
successful, false if not. out_success can be NULL if not used.

Complexity: O(n) where n is the number of bytes in src
*/

void * bit_vect_set_byte_pattern(bit_vector * bv, unsigned char val);
#define bit_vect_set_all(bv)    bit_vect_set_byte_pattern((bv), BV_ALL_ONES)
#define bit_vect_clear_all(bv)  bit_vect_set_byte_pattern((bv), BV_ALL_ZERO)
/*
Returns: bv

Description: Sets all bytes inside bv to val.

Complexity: O(number of bytes in bv)
*/

void * bit_vect_get(bit_vector * bv, int bit_num, int * out_bit_val);
/*
Returns: bv on success, NULL if bit_num is out of range.

Description: Reads the value of bit bit_num into the variable pointed to by
out_bit_val. If NULL is returned, the value of *out_bit_val is not changed.

Complexity: O(1)
*/

void * bit_vect_set(bit_vector * bv, int bit_num);
/*
Returns: bv on success, NULL if bit_num is out of range.

Description: Sets bit bit_num to 1

Complexity: O(1)
*/

void * bit_vect_clear(bit_vector * bv, int bit_num);
/*
Returns: bv on success, NULL if bit_num is out of range.

Description: Sets bit bit_num to 0

Complexity: O(1)
*/

void * bit_vect_toggle(bit_vector * bv, int bit_num);
/*
Returns: bv on success, NULL if bit_num is out of range.

Description: Sets bit bit_num to 1 if it's 0, or to 0 if it's 1

Complexity: O(1)
*/

int bit_vect_num_of_bits(bit_vector * bv);
/*
Returns: The number of valid bits in bv.

Description: Returns the number of bits.

Complexity: O(1)
*/
#endif
