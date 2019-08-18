/*  trie.h -- a prefix tree
    v1.1

    The c_vectors are treated as sets and are kept sorted in the internal
    structure of the trie. They are used instead of ov_sets because of
    performance reasons. Do not change their data manually. In this document the
    words c_vector and set are used interchangeably.

    TRIE_DEFAULT_CAP specifies the initial size of the root c_vector as well as
    the capacity for each mem_pool slab. The mem_pool is used so malloc() and
    free() are not called for each and every new c_vector needed by the trie.
    It makes sense for TRIE_DEFAULT_CAP to be at least a double digit number.

    Each trie node contains a trie_val part with the value of that node as well
    as a tag for that value and a pointer to a c_vector. The tag is a void * so
    it could be used as a flag and point to something useful. If the c_vector
    pointer is NULL, this means the node is a leaf. If it's not NULL, the
    c_vector it points to is an ordered set of all nodes which continue the
    word. The nodes hold a c_vector * instead of a c_vector struct so the leaves
    of the trie do not waste memory. On lookup only the val part of the trie_val
    is compared.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-08-18
*/

#ifndef TRIE_H
#define TRIE_H

#include "../c_vector/c_vector.h"
#include "../mem_pool/mem_pool.h"

#define TRIE_TAG_NULL     NULL
#define TRIE_DEFAULT_CAP  26

// Use macros or at own risk
typedef struct trie_val {
    int val;
    void * tag;
} trie_val;

// Do not use directly
typedef struct trie_node {
    trie_val what;
    c_vector * next;
} trie_node;

// Do not use directly
typedef struct trie {
    c_vector root;
    int orig_cap;
    mem_pool cv_pool;
} trie;

void * trie_make_cap(trie * tre, int cap);
#define trie_make(ptre) trie_make_cap((ptre), TRIE_DEFAULT_CAP)
/*
Returns: tre on success, NULL otherwise.

Description: Creates the trie. On failure tre is zeroed out.

Complexity: O(1)
*/

void trie_destroy(trie * tre);
/*
Returns: Nothing.

Description: Releases all memory associated with the trie and zeroes out
the structure.

Complexity: O(1)
*/

void trie_str_to_tval(const char * str, trie_val * tv_arr, int len, void * tag);
/*
Returns: Nothing.

Description: Populates tv_arr with the content of str. The tag of the last
element is set to the tag parameter, the tags for all others to
TRIE_TAG_NULL. len is the length of str. tv_arr is expected to be of at least
the same length.

Complexity: O(n)
*/

void * trie_insert(trie * tre, const trie_val * tv_arr, int len);
/*
Returns: tre on success, NULL otherwise.

Description: Inserts tv_arr of length len into tre. If the last element of
tv_arr already exists in the trie, its tag is overwritten with the tag value
of the element of tv_arr.

Complexity: O(n)
*/

const trie_node * trie_lookup(
    const trie * tre, const trie_val * tv_arr, int len
    );
/*
Returns: A pointer to the node matching the value of the last element of tv_arr
on success, NULL if the sequence of values in tv_arr does not exist in tre.

Description: Looks ups a tv_arr of length len in the trie.

Complexity: O(n)
*/

const trie_node * trie_lookup_first(const trie * tre, const trie_val * val);
/*
Returns: A pointer to the node matching the value of val on success,
NULL if not found.

Description: Looks for val in the root set of the trie.

Complexity: O(1)
*/

const trie_node * trie_lookup_next(const trie_node * tn, const trie_val * val);
/*
Returns: A pointer to the node matching the value of val on success,
NULL if not found.

Description: Looks for val in any node that is not the root of the trie.

Complexity: O(1)
*/

const c_vector * trie_get_root_set(const trie * tre);
/*
Returns: A pointer to the root set of the trie.

Description: Gets you the c_vector containing the first nodes of all words in
the trie.

Complexity: O(1)
*/

const trie_val * trie_get_node_tval(const trie_node * tn);
/*
Returns: A pointer to the trie_val part of node tn.

Description: Gets you trie_val part of tn.

Complexity: O(1)
*/

const c_vector * trie_get_node_set(const trie_node * tn);
/*
Returns: A pointer to the set part of node tn.

Description: Gets you the c_vector part of tn.

Complexity: O(1)
*/

trie_val trie_make_tval(int val, void * tag);
/*
Returns: A trie_val struct with val and tag members set to the val and tag
parameters, respectively.

Description: Creates an initialized trie_val struct.

Complexity: O(1)
*/

#define trie_val_get_val(ptval) ((int)(ptval)->val)
#define trie_val_get_tag(ptval) ((void *)(ptval)->tag)
/*
Returns: The val and tag from the trie_val pointer to by ptval, respectively.

Description: Macro getters for trie_val.

Complexity: O(1)
*/

#define trie_val_set_val(ptval, the_val) ((ptval)->val = (the_val))
#define trie_val_set_tag(ptval, the_tag) ((ptval)->tag = (the_tag))
/*
Returns: Nothing.

Description: Set the val and tag members of the trie_val pointed to by ptval,
respectively.

Complexity: O(1)
*/

#define trie_c_vect_get(pcv, i)\
(const trie_node *)c_vect_get((c_vector *)(pcv), (i))
/*
Returns: A pointer to the trie_node at index i inside the c_vector pointed to
by pcv, if i is in range. NULL otherwise.

Description: Keeps the const trie_node * promise.

Complexity: O(1)
*/

#endif
