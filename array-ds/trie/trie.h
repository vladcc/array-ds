#ifndef TRIE_H
#define TRIE_H

#include "../c_vector/c_vector.h"

#define TRIE_DCAP 8

typedef struct trie_val {
    int val;
    int tag;
} trie_val;

typedef struct trie {
    c_vector root;
    int orig_cap;
} trie;

typedef struct trie_node {
    trie_val what;
    c_vector * next;
} trie_node;

void * trie_make_cap(trie * tre, int cap);
#define trie_make(ptre) trie_make_cap((ptre), TRIE_DCAP)
void trie_destroy(trie * tre);

trie_val trie_make_tval(int val, int tag);
void * trie_insert(trie * tre, trie_val * tv_arr, int len);
#endif
