/*  cv_htable.h -- vector chained hash table
    v1.0

    A hash table chained with c_vectors. Behaves like a set, doubles its
    bucket count once the load factor is reached. All buckets are created with
    the initial capacity of CVH_DEFAULT_LOAD_FACT.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-03-10
*/

#ifndef CV_HTBL_H
#define CV_HTBL_H

#include "../c_vector/c_vector.h"

#define CVH_DEFAULT_CAPACITY    1024
#define CVH_DEFAULT_LOAD_FACT   4

typedef unsigned int (*fhash)(const unsigned char * key, int length);

// Do not use directly
typedef struct cv_htbl {
    c_vector the_tbl;
    fhash hash_fun;
    int elem_size;
    int elem_inside;
    int elem_max;
} cv_htbl;

void * cv_htbl_make_cap_hash(
    cv_htbl * cvht, int elem_size, fcomp compar, int capacity, fhash hash
    );
#define cv_htbl_make(cvht, el_size, compar)\
cv_htbl_make_cap_hash((cvht), (el_size), (compar), CVH_DEFAULT_CAPACITY, NULL)

#define cv_htbl_make_cap(cvht, el_size, compar, capacity)\
cv_htbl_make_cap_hash((cvht), (el_size), (compar), (capacity), NULL)

#define cv_htbl_make_hash(cvht, elem_size, compar, hash)\
cv_htbl_make_cap_hash((cvht), (el_size), (compar), CVH_DEFAULT_CAPACITY, (hash))
/*
Returns: cvht on success, NULL otherwise.

Description: Creates the table. On failure *cvht is zeroed out.

Complexity:O(1)
*/

void cv_htbl_destroy(cv_htbl * cvht);
#define cv_htbl_destroy_null(cvht)\
cv_htbl_destroy((cvht)), (cvht) = NULL
/*
Returns: Nothing.

Description: Destroys the table, zeroes out *cvht.

Complexity: O(1)
*/

cv_htbl cv_htbl_copy(const cv_htbl * src, bool * out_success);
/*
Returns: A deep copy of src on success, a zeroed out cv_htbl otherwise.

Description: Makes a deep copy of src. *out_succes is true if the copy was
successful, false if not. out_success can be NULL if not used.

Complexity: O(n) where n is the number of buckets in src.
*/

const void * cv_htbl_lookup(cv_htbl * cvht, const void * key);
/*
Returns: A pointer to key inside cvht if found, NULL otherwise.

Description: Looks up key in cvht.

Complexity: O(1)
*/

const void * cv_htbl_insert(cv_htbl * cvht, const void * key);
/*
Returns: A pointer to key inside cvht on success, NULL otherwise.

Description: Looks up key inside cvht and inserts it if not found.

Complexity: O(1)
*/

void * cv_htbl_remove(cv_htbl * cvht, const void * key);
/*
Returns: cvht

Description: Removes key from cvht if found.

Complexity: O(1)
*/

void * cv_htbl_clear(cv_htbl * cvht);
/*
Returns: cvht

Description: Empties cvht.

Complexity: O(1)
*/

typedef struct cv_htbl_iterator {int row, col;} cv_htbl_iterator;
#define cv_htbl_iterator_init() (cv_htbl_iterator){.row = 0, .col = 0}

const void * cv_htbl_iterate_(cv_htbl * cvht,
    cv_htbl_iterator start, cv_htbl_iterator * out_so_far
    );
#define cv_htbl_iterate(cvht, start)\
cv_htbl_iterate_((cvht), (start), &(start))
/*
Returns: A pointer to the next element in oaht, NULL if there are no more
elements.

Description: Iterates the elements of cvht beginning from the first. start must
be a variable and out_so_far must point to start, or use the macro. Example:

cv_htbl_iterator it = cv_htbl_iterator_init();
const your_type * pdata;
while ((pdata = cv_htbl_iterate(cvht, it)))
    do_something_with(pdata);

Complexity: O(n)
*/

bool cv_htbl_is_empty(cv_htbl * cvht);
/*
Returns: true if cvht is empty, false otherwise.

Description: Lets you know if cvht is empty.

Complexity: O(1)
*/

int cv_htbl_elem_count(cv_htbl * cvht);
/*
Returns: The number of elements inside cvht.

Description: Gets the number of elements inside cvht.

Complexity: O(1)
*/

int cv_htbl_elem_max(cv_htbl * cvht);
/*
Returns: The maximum number of elements cvht can have before it has to grow.

Description: Gets the number of elements cvht can have before it has to grow.

Complexity: O(1)
*/

fhash cv_htbl_get_default_hash(void);
/*
Returns: A pointer to the default hash function.

Description: Gets a pointer to the default hash function.

Complexity: O(1)
*/
#endif
