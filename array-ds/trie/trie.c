#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

#define get_root_set(ptre) &((ptre)->root)
#define get_node_set(ptn) ((ptn)->next)
//------------------------------------------------------------------------------

static int compar_tnodes(const void * anode, const void * bnode)
{
    const trie_node * one = anode;
    const trie_node * two = bnode;
    return (one->what.val > two->what.val) - (one->what.val < two->what.val);
}
//------------------------------------------------------------------------------

void * trie_make_cap(trie * tre, int cap)
{
    void * ret = NULL;
    c_vector * tset = get_root_set(tre);

    tre->orig_cap = cap;
    if (c_vect_make_cap(tset, sizeof(trie_node), compar_tnodes, cap))
        ret = tre;
    else
        memset(tre, 0, sizeof(*tre));

    return ret;
}
//------------------------------------------------------------------------------

static void free_trie_node(trie_node * node)
{
    c_vector * next = get_node_set(node);

    if (next)
    {
        int tnd_sz = c_vect_length(next);

        for (int i = 0; i < tnd_sz; ++i)
            free_trie_node(c_vect_get(next, i));

        c_vect_destroy(next);
        free(next);
    }
}

void trie_destroy(trie * tre)
{
    c_vector * tset = get_root_set(tre);
    int tset_sz = c_vect_length(tset);

    for (int i = 0; i < tset_sz; ++i)
        free_trie_node(c_vect_get(tset, i));

    c_vect_destroy(tset);
}
//------------------------------------------------------------------------------

trie_val trie_make_tval(int val, int tag)
{
    trie_val ret = {.val = val, .tag = tag};
    return ret;
}
//------------------------------------------------------------------------------

static c_vector * make_set(int cap)
{
    c_vector * ret = malloc(sizeof(*ret));

    if (!ret)
    {
        // replace with mem pool, remove stdio.h include
        fprintf(stderr, "Err: malloc() failed in %s()\n", __func__);
        exit(EXIT_FAILURE);
    }
    c_vect_make_cap(ret, sizeof(trie_node), compar_tnodes, cap);

    return ret;
}

void * trie_insert(trie * tre, trie_val * tv_arr, int len)
{
    c_vector * next_node_set;
    int orig_cap = tre->orig_cap;
    trie_node val_node, dummy, * this_node;

    val_node.next = NULL;
    dummy.what.val = dummy.what.tag = -1;
    dummy.next = get_root_set(tre);
    this_node = &dummy;

    for (int i = 0; i < len; ++i)
    {
        next_node_set = get_node_set(this_node);

        if (!next_node_set)
                this_node->next = make_set(orig_cap);

        val_node.what = tv_arr[i];
        this_node = c_vect_bsearch(next_node_set, &val_node);

        if (!this_node)
            this_node = c_vect_insert_online(next_node_set, &val_node);
    }

    return tre;
}
//------------------------------------------------------------------------------
