#ifndef TRIE_H
#define TRIE_H

#include "../c_vector/c_vector.h"

#define TRIE_DCAP 8

typedef struct trie_val {
    int val;
    int tag;
} trie_val;

typedef struct trie_node {
    trie_val what;
    c_vector * next;
} trie_node;

typedef struct trie {
    c_vector root;
    int orig_cap;
} trie;

void * trie_make_cap(trie * tre, int cap);
#define trie_make(ptre) trie_make_cap((ptre), TRIE_DCAP)
void trie_destroy(trie * tre);

trie_val trie_make_tval(int val, int tag);
void * trie_insert(trie * tre, const trie_val * tv_arr, int len);
const c_vector * trie_get_root(const trie * tre);
const trie_val * trie_get_node_val(const trie_node * tn);
const c_vector * trie_get_node_set(const trie_node * tn);
const trie_val * trie_lookup(const trie * tre, const trie_val * arr, int len);

#define trie_c_vect_get(pcv, i)\
(const void *)c_vect_get((c_vector *)(pcv), (i))

#endif
