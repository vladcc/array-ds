/*  ov_multimap.h -- ordered vector multimap
    v1.112

    ov_multimap is an ordered vector set of keys. Every key can be associated
    with multiple values. Additional memory is allocated after every key for a
    c_vector to hold its values. In that sense, ov_multimap is an ordered
    c_vector of c_vectors. The value vectors are not ordered and lookup in them
    is performed in a linear fashion.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-08-10
*/

#ifndef OV_MULTIMAP_H
#define OV_MULTIMAP_H

#include "../c_vector/c_vector.h"
#include "../ov_set/ov_set.h"

#define OV_MMAP_DEFAULT_VAL_CAPACITY 4

#define OV_MMAP_STACK_MAX 64
/*
The maximum number of bytes ov_mmap is allowed to allocate on the stack when
it needs a buffer. ov_mmap needs ov_mmap_real_key_size() number of bytes of
scratch space on every insert. If that's more than the allowed on the stack,
ov_mmap will allocated a buffer on the heap, use it, and free() it every time.
*/

// Do not use directly
typedef struct ov_mmap {
    ov_set the_map;
    fcomp val_compar;
    int orig_key_size;
    int real_key_size;
    int val_size;
    int val_capacity;
} ov_mmap;

void * ov_mmap_make_cap(
    ov_mmap * mmap,
    int key_size, fcomp key_compar, int key_capacity,
    int val_size, fcomp val_compar, int val_capacity
);
#define ov_mmap_make(mmap, key_size, key_compar, val_size, val_compar)\
ov_mmap_make_cap(\
    (mmap),\
    (key_size), (key_compar), C_VECT_DEFAULT_CAPACITY,\
    (val_size), (val_compar), OV_MMAP_DEFAULT_VAL_CAPACITY)
/*
Returns: mmap on success, NULL otherwise.

Description: Creates a multimap. key_capacity is the number of keys mmap will
reserve space for upon creation, val_capacity is the number of key each value
vector will reserve space for upon its creation. When a capacity is reached,
its associated structure grows dynamically. On failure *mmap is zeroed out.

Complexity: O(1)
*/

void ov_mmap_destroy(ov_mmap * mmap);
#define ov_mmap_destroy_null(mmap)\
ov_mmap_destroy((mmap)), mmap = NULL
/*
Returns: Nothing.

Description: Destroys the multimap, zeroes *mmap out.

Complexity: O(1)
*/

ov_mmap ov_mmap_copy(const ov_mmap * src, bool * out_success);
/*
Returns: A deep copy of src on success, a zeroed out ov_mmap otherwise.

Description: Makes a deep copy of src. *out_succes is true if the copy was
successful, false if not. out_success can be NULL if not used.

Complexity: O(n)
*/

void * ov_mmap_insert_once(ov_mmap * mmap, const void * key, const void * val);
/*
Returns: A pointer to val inside mmap on success, NULL otherwise.

Description: Inserts val in mmap only if key isn't already associated with that
value. In other words, does not allow for duplicate values for key.

Complexity: O(log n) + O(m) where n is the number of keys in mmap and m is the
number of values associated with key.
*/

void * ov_mmap_insert(ov_mmap * mmap, const void * key, const void * val);
/*
Returns: A pointer to val inside mmap on success, NULL otherwise.

Description: Inserts val in mmap regardless if key has already been associated
with that value. In other words, allows for duplicate values for key.

Complexity: O(log n) where n is the number of keys in mmap.
*/

void * ov_mmap_find(ov_mmap * mmap, const void * key, const void * val);
/*
Returns: A pointer to val inside mmap on success, NULL otherwise.

Description: Looks for key val pair inside mmap.

Complexity: O(log n) + O(m) where n is the number of keys in mmap and m is the
number of values associated with key.
*/

void * ov_mmap_remove(ov_mmap * mmap, const void * key, const void * val);
/*
Returns: mmap if val is removed, NULL if it's not found.

Description: Looks for key val pair and removes val from the key vector.

Complexity: O(log n) + O(m) where n is the number of keys in mmap and m is the
number of values associated with key.
*/

void * ov_mmap_remove_by_key(ov_mmap * mmap, const void * key);
/*
Returns: mmap if key is removed, NULL if it's not found.

Description: Removes key and all its associated values from mmap.

Complexity: O(log n)
*/

int ov_mmap_remove_by_val(ov_mmap * mmap, const void * val);
/*
Returns: The number of values removed from mmap.

Description: Removes all instances of val for every key from mmap.

Complexity: O(n*m) where n is the number of keys and m is the number of values.
*/

#define ov_mmap_num_of_keys(mmap)   c_vect_length(&(mmap)->the_map)
/*
Returns: The number of keys in mmap.

Description: Gets the number of keys in mmap.

Complexity: O(1)
*/

int ov_mmap_num_of_vals(const ov_mmap * mmap, const void * key);
/*
Returns: The number of values associated with key.

Description: Gets the number of values associated with key.

Complexity: O(log n)
*/

c_vector * ov_mmap_get_vals(ov_mmap * mmap, const void * key);
#define ov_mmap_has_key ov_mmap_get_vals
/*
Returns: A pointer to the c_vector holding all values associated with key, NULL
if key is not found.

Description: Retrieves all values for key.

Complexity: O(log n)
*/

#define ov_mmap_get_key(mmap, index) c_vect_get(&(mmap)->the_map, (index))
/*
Returns: A pointer to the key at position index, NULL if index is out of range.

Description: Gets the key at position index.

Complexity: O(1)
*/

void * ov_mmap_get_value(ov_mmap * mmap, int val_number);
/*
Returns: A pointer to the value at the val_number position inside mmap, NULL if
val_number is out of range.

Description: Retrieves the value at position val_number inside mmap as if mmap
is an array of only values without keys.

Complexity: O(n) where n is the number of keys in mmap.
*/

void * ov_mmap_fit(ov_mmap * mmap);
/*
Returns: mmap on success, NULL otherwise.

Description: Trims all c_vectors inside mmap so mmap takes only as much memory
as it needs.

Complexity: O(n) where n is the number of keys in mmap.
*/

// a callback that takes no additional arguments
typedef void (*mmap_apply)(const void * key, void * val);

// a callback that takes additional arguments
typedef void (*mmap_apply_args)(const void * key, void * val, void * args);

void * ov_mmap_apply(ov_mmap * mmap, mmap_apply fun);
/*
Returns: mmap

Description: Applies an user specified function of type mmap_apply to all keys
and values of mmap.

Complexity: O(n*m) where n is the number of keys and m is the number of values.
*/

void * ov_mmap_apply_args(ov_mmap * mmap, mmap_apply_args fun, void * args);
/*
Returns: mmap

Description: Applies an user specified function of type mmap_apply_args to all
keys and values of mmap.

Complexity: O(n*m) where n is the number of keys and m is the number of values.
*/

#define ov_mmap_is_empty(mmap)      c_vect_is_empty(&(mmap)->the_map)
#define ov_mmap_capacity(mmap)      c_vect_capacity(&(mmap)->the_map)

int ov_mmap_orig_key_size(const ov_mmap * mmap);
/*
Returns: The size in bytes of they key type.

Description: mmap allocates additional spaces after every key, so the memory
used for each key is more than it's sizeof() size.

Complexity: O(1)
*/

int ov_mmap_real_key_size(const ov_mmap * mmap);
/*
Returns: The number of bytes actually used for each key.

Description: mmap allocates additional space after every key in order to hold
the vector of values associated with that key. The real size of each key is
therefore sizeof(key_type) + alignment + sizeof(c_vector)

Complexity: O(1)
*/

int ov_mmap_val_size(const ov_mmap * mmap);
/*
Returns: The size of the value type in bytes.

Description: Retrieves sizeof(val_type)

Complexity: O(1)
*/

int ov_mmap_val_capcity(const ov_mmap * mmap);
/*
Returns: The number of values each key can initially hold before having to grow
its value vector.

Description: Retrieves the starting capacity of the value vectors.

Complexity: O(1)
*/
#endif
