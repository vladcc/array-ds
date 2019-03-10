/*  ov_set.h -- ordered vector set
    v1.0

    The ov_set is a c_vector which keeps itself sorted and uses binary search
    for lookup. The main difference between this and ov_map is that ov_set
    considers the whole of its elements const, rather than only fields
    potentially marked as const.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-02-23
*/

#ifndef OV_SET_H
#define OV_SET_H

#include "../c_vector/c_vector.h"

#define ov_set              c_vector

#define ov_set_make_cap     c_vect_make_cap
#define ov_set_make         c_vect_make
#define ov_set_destroy      c_vect_destroy
#define ov_set_destroy_null c_vect_destroy_null
#define ov_set_has          (const void *)c_vect_bsearch
#define ov_set_get          (const void *)c_vect_get
#define ov_set_copy         c_vect_copy
#define ov_set_resize       c_vect_resize
#define ov_set_trim         c_vect_trim
#define ov_set_zero_out     c_vect_zero_out
#define ov_set_reset        c_vect_reset
#define ov_set_is_empty     c_vect_is_empty
#define ov_set_data         (const void *)c_vect_data
#define ov_set_length       c_vect_length
#define ov_set_elem_size    c_vect_elem_size
#define ov_set_capacity     c_vect_capacity
#define ov_set_remove_at    c_vect_remove_at

// a callback that takes no additional arguments
typedef void (*fapply_const)(const void * elem);

// a callback that takes additional arguments
typedef void (*fapply_const_args)(const void * elem, void * args);

void * ov_set_apply(ov_set * set, fapply_const fun);
/*
Returns: set

Description: Applies a function of type fapply_const to all its elements.

Complexity: O(n)
*/

void * ov_set_apply_args(ov_set * set, fapply_const_args fun, void * args);
/*
Returns: set

Description: Applies a function of type fapply_const_args to all its elements.

Complexity: O(n)
*/

const void * ov_set_insert(ov_set * set, const void * what);
/*
Returns: A pointer to the elements inserted in the set, NULL if the element
is already in the set.

Description: Does a binary search for what and inserts it if not found.

Complexity: O(log n)
*/

void * ov_set_remove(ov_set * set, const void * what);
/*
Returns: set if what is removed, NULL if what is not in the set.

Description: Does a binary search for what and removes it if found.

Complexity: O(log n)
*/
#endif
