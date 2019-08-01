/*  ov_multiset.h -- ordered vector multiset
    v1.11

    It's a set which counts how many times an element has been inserted (its
    multiplicity) As such it allocates extra memory for a counter, which is an
    int by default.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-08-01
*/

#ifndef OV_MULTISET_H
#define OV_MULTISET_H

#include "../c_vector/c_vector.h"

#define OV_MSET_STACK_MAX 128
/*
The maximum number of bytes ov_mset is allowed to allocate on the stack when
it needs a buffer. ov_mset needs ov_mset_real_elem_size() number of bytes of
scratch space on every insert. If that's more than the allowed on the stack,
ov_mset will allocated a buffer on the heap, use it, and free() it every time.
*/

// Do not use directly
typedef struct ov_mset {
    c_vector the_vect;
    int orig_elem_size;
} ov_mset;


void * ov_mset_make_cap(
    ov_mset * mset, int elem_size, fcomp compar, int capacity
);
#define ov_mset_make(mset, elem_size, compar)\
ov_mset_make_cap((mset), (elem_size), (compar), C_VECT_DEFAULT_CAPACITY)

/*
Returns: mset on success, NULL otherwise.

Description: Creates a multiset. On failure *mset is zeroed out.

Complexity: O(1)
*/

void ov_mset_destroy(ov_mset * mset);
/*
Returns: Nothing.

Description: Destroys the multiset, zeroes *mset out.

Complexity: O(1)
*/

ov_mset ov_mset_copy(const ov_mset * src, bool * out_success);

#define ov_mset_has(mset, key)\
(const void *)c_vect_bsearch(&(mset)->the_vect, (key))

#define ov_mset_get(mset, index)\
(const void *)c_vect_get(&(mset)->the_vect, (index))

#define ov_mset_resize(mset, new_cap)\
c_vect_resize(&(mset)->the_vect, (new_cap))

#define ov_mset_trim(mset)          c_vect_trim(&(mset)->the_vect)
#define ov_mset_zero_out(mset)      c_vect_zero_out(&(mset)->the_vect)
#define ov_mset_reset(mset)         c_vect_reset(&(mset)->the_vect)
#define ov_mset_is_empty(mset)      c_vect_is_empty(&(mset)->the_vect)

#define ov_mset_data(mset)\
(const void *)c_vect_data(&(mset->the_vect))

#define ov_mset_length(mset)        c_vect_length(&(mset)->the_vect)
#define ov_mset_capacity(mset)      c_vect_capacity(&(mset)->the_vect)

int ov_mset_orig_elem_size(const ov_mset * mset);
/*
Returns: The size of an element inside mset, as passed to ov_mset_make()

Description: ov_mset allocates extra space for a counter after the original
element. This function provides the original element size.

Complexity: O(1)
*/

#define ov_mset_real_elem_size(mset)\
c_vect_elem_size(&(mset)->the_vect)
/*
Returns: The size of an element inside mset + the size of the counter + any
alignment bytes.

Description: ov_mset allocates extra space for a counter after the original
element. This macro provides the size of one whole element as seen by mset.

Complexity: O(1)
*/

// a callback that takes no additional arguments
typedef void (*fapply_const)(const void * elem);

// a callback that takes additional arguments
typedef void (*fapply_const_args)(const void * elem, void * args);

void * ov_mset_apply(ov_mset * mset, fapply_const fun);
/*
Returns: mset

Description: Applies a function of type fapply_const to all its elements.

Complexity: O(n)
*/

void * ov_mset_apply_args(ov_mset * mset, fapply_const_args fun, void * args);
/*
Returns: mset

Description: Applies a function of type fapply_const_args to all its elements.

Complexity: O(n)
*/

const void * ov_mset_insert(ov_mset * mset, const void * what);
/*
Returns: A pointer to what inside mset, NULL if what has already been inserted
before.

Description: Performs binary search for what inside mset. If what is not found,
it's inserted and its multiplicity is set to 1. If what is found, its
multiplicity is incremented.

Complexity: O(log n)
*/

void * ov_mset_remove(ov_mset * mset, const void * what);
/*
Returns: mset if what is removed from the set, NULL not found.

Description: Performs binary search for what, decrements its multiplicity
counter, erases the element when the counter reaches 0.

Complexity: O(log n)
*/

void * ov_mset_erase(ov_mset * mset, const void * what);
/*
Returns: mset if what is erased from the set, NULL not found.

Description: Performs binary search for what, erases it from the list if found
regardless of its multiplicity.

Complexity: O(log n)
*/


int ov_mset_mplicity(const ov_mset * mset, const void * what);
/*
Returns: The multiplicity of what in mset, -1 if what is not a member of mset.

Description: Performs binary search for what and retrieves its multiplicity.

Complexity: O(log n)
*/
#endif
