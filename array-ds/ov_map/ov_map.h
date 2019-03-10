/*  ov_map.h -- ordered vector map
    v1.0

    The ov_map is a c_vector which keeps itself sorted and uses binary search
    for lookup.

    One way of looking at it is that it doesn't actually map anything, as the
    mapping property depends entirely on the data you give it. For example, fill
    it with structs with a const key member and have the compar() function
    compare only those. E.g.:

    struct couple {
        const your_key_type key;
        your_data_type data;
    };

    int compar(void * k1, void * k2)
    {
        your_key_type * key1 = (your_key_type *)k1;
        your_key_type * key2 = (your_key_type *)k2;
        ...
    }

    However, there's nothing stopping you from using the map with plain ints, or
    anything else different from structs with key and data members, resulting
    in an ordered vector of X (assuming X can be ordered) But then you can say
    that's a map which maps keys of type X to empty data.

    In any case, no two elements in ov_map can have equivalent keys. Also, be
    careful to not change the value of any key already inside the map, as it may
    result in undefined behavior if the ordering goes out of whack.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-02-23
*/

#ifndef OV_MAP_H
#define OV_MAP_H

#include "../c_vector/c_vector.h"

#define ov_map              c_vector

#define ov_map_make_cap     c_vect_make_cap
#define ov_map_make         c_vect_make
#define ov_map_destroy      c_vect_destroy
#define ov_map_destroy_null c_vect_destroy_null
#define ov_map_find         c_vect_bsearch
#define ov_map_get          c_vect_get
#define ov_map_copy         c_vect_copy
#define ov_map_resize       c_vect_resize
#define ov_map_trim         c_vect_trim
#define ov_map_zero_out     c_vect_zero_out
#define ov_map_reset        c_vect_reset
#define ov_map_is_empty     c_vect_is_empty
#define ov_map_data         c_vect_data
#define ov_map_length       c_vect_length
#define ov_map_elem_size    c_vect_elem_size
#define ov_map_capacity     c_vect_capacity
#define ov_map_remove_at    c_vect_remove_at
#define ov_map_apply        c_vect_apply
#define ov_map_apply_args   c_vect_apply_args

void * ov_map_insert(ov_map * map, const void * what);
/*
Returns: A pointer to the element inside the map, or NULL if insertion fails.

Description: Does a binary search for what and inserts it if not found.

Complexity: O(log n)
*/

void * ov_map_remove(ov_map * map, const void * what);
/*
Returns: map if what is removed, NULL if what wasn't in the map.

Description: Does a binary search for what and removes it if found.

Complexity: O(log n)
*/
#endif
