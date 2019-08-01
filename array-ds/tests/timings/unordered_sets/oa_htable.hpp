/*  oa_htable.h -- open address hash table
    v1.11

    Generic open address hash table, uses c_vector as a base. Inserts an element
    only if it's not already inside. In other words, behaves like a set. Grows
    double in size once the load factor is reached.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-08-01
*/

#ifndef OA_HTBL_H
#define OA_HTBL_H

#include "c_vector.hpp"
#include "bit_vector.hpp"

#define OAH_DEFAULT_CAPACITY    1024
#define OAH_DEFAULT_LOAD_FACT   0.7

typedef unsigned int (*fhash)(const unsigned char * key, int length);

// Do not use directly
typedef struct oa_htbl {
    c_vector the_tbl;
    bit_vector is_taken;
    fhash hash_fun;
    int elem_inside;
    int elem_max;
} oa_htbl;

void * oa_htbl_make_cap_hash(
    oa_htbl * oaht, int elem_size, fcomp compar, int capacity, fhash hash
    );
#define oa_htbl_make(oaht, el_size, compar)\
oa_htbl_make_cap_hash((oaht), (el_size), (compar), OAH_DEFAULT_CAPACITY, NULL)

#define oa_htbl_make_cap(oaht, el_size, compar, capacity)\
oa_htbl_make_cap_hash((oaht), (el_size), (compar), (capacity), NULL)

#define oa_htbl_make_hash(oaht, elem_size, compar, hash)\
oa_htbl_make_cap_hash((oaht), (el_size), (compar), OAH_DEFAULT_CAPACITY, (hash))
/*
Returns: oaht on success, NULL otherwise.

Description: Creates the table. On failure *oaht is zeroed out.

Complexity: O(1)
*/

void oa_htbl_destroy(oa_htbl * oaht);
#define oa_htbl_destroy_null(oaht)\
oa_htbl_destroy((oaht)), (oaht) = NULL
/*
Returns: Nothing.

Description: Destroys the table, zeroes *oaht out.

Complexity: O(1)
*/

oa_htbl oa_htbl_copy(const oa_htbl * src, bool * out_success);
/*
Returns: A deep copy of src on success, a zeroed out oa_htbl otherwise.

Description: Makes a deep copy of src. *out_succes is true if the copy was
successful, false if not. out_success can be NULL if not used.

Complexity: O(n) where n is the capacity of the underlying c_vector.
*/


const void * oa_htbl_lookup(const oa_htbl * oaht, const void * key);
/*
Returns: A pointer to key inside oaht if found, NULL otherwise.

Description: Looks up key in oaht.

Complexity: O(1)
*/

const void * oa_htbl_insert(oa_htbl * oaht, const void * key);
/*
Returns: A pointer to key inside oaht on success, NULL otherwise.

Description: Looks up key inside oaht and inserts it if not found.

Complexity: O(1)
*/

void * oa_htbl_remove(oa_htbl * oaht, const void * key);
/*
Returns: oaht

Description: Removes key from oaht if found.

Complexity: O(1)
*/

void * oa_htbl_clear(oa_htbl * oaht);
/*
Returns: oaht

Description: Empties oaht.

Complexity: O(n) where n is the number of bytes in the internal bit_vector.
*/

typedef int oa_htbl_iterator;
#define oa_htbl_iterator_init() ((oa_htbl_iterator)0)

const void * oa_htbl_iterate_(
    oa_htbl * oaht, oa_htbl_iterator start, oa_htbl_iterator * out_so_far
    );
#define oa_htbl_iterate(oaht, start)\
oa_htbl_iterate_((oaht), (start), &(start))
/*
Returns: A pointer to the next element in oaht, NULL if there are no more
elements.

Description: Iterates the elements of oaht beginning from the first. start must
be a variable and out_so_far must point to start, or use the macro. Example:

oa_htbl_iterator iter = oa_htbl_iterator_init();
const your_type * pdata;
while ((pdata = oa_htbl_iterate(oaht, iter)))
    do_something_with(pdata);

Complexity: O(n)
*/

bool oa_htbl_is_empty(const oa_htbl * oaht);
/*
Returns: true if oaht is empty, false otherwise.

Description: Lets you know if oaht is empty.

Complexity: O(1)
*/

int oa_htbl_elem_count(const oa_htbl * oaht);
/*
Returns: The number of elements inside oaht.

Description: Gets the number of elements inside oaht.

Complexity: O(1)
*/

int oa_htbl_elem_max(const oa_htbl * oaht);
/*
Returns: The maximum number of elements oaht can have before it has to grow.

Description: Gets the number of elements oaht can have before it has to grow.

Complexity: O(1)
*/

fhash oa_htbl_get_default_hash(void);
/*
Returns: A pointer to the default hash function.

Description: Gets a pointer to the default hash function.

Complexity: O(1)
*/
#endif
