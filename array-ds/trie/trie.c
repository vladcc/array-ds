#include <string.h>
#include "trie.h"

#define DUMMY_VAL -1

#define get_root_set(ptre) &((ptre)->root)
#define get_node_set(ptn)  ((ptn)->next)
#define get_node_tval(ptn) &((ptn)->what)
#define get_mpl(ptre)      &((ptre)->cv_pool)
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
    c_vector * tset = get_root_set(tre);
    mem_pool * mpl = &(tre->cv_pool);

    tre->orig_cap = cap;
    if (c_vect_make_cap(tset, sizeof(trie_node), compar_tnodes, cap))
    {
        if (mpl_make(mpl, sizeof(c_vector), cap))
            return tre;
        else
            c_vect_destroy(tset);
    }

    memset(tre, 0, sizeof(*tre));
    return NULL;
}
//------------------------------------------------------------------------------

static void free_trie_node(void * node)
{
    c_vector * next = get_node_set((trie_node *)node);

    if (next)
    {
        int tnd_sz = c_vect_length(next);

        for (int i = 0; i < tnd_sz; ++i)
            free_trie_node(c_vect_get(next, i));
        c_vect_destroy(next);
    }
}

void trie_destroy(trie * tre)
{
    c_vector * rset = get_root_set(tre);
    c_vect_apply(rset, free_trie_node);
    c_vect_destroy(rset);
    mpl_destroy(get_mpl(tre));

    memset(tre, 0, sizeof(*tre));
}
//------------------------------------------------------------------------------

trie_node_val trie_make_trie_node_val(int val, void * tag)
{
    trie_node_val ret = {.val = val, .tag = tag};
    return ret;
}
//------------------------------------------------------------------------------

void trie_str_to_tval(const char * str, trie_node_val * tv_arr, int len, void * tag)
{
    for (int i = 0; i < len; ++i)
    {
        tv_arr[i].val = str[i];
        tv_arr[i].tag = TRIE_TAG_NULL;
    }
    tv_arr[len-1].tag = tag;
}
//------------------------------------------------------------------------------

void * trie_insert(trie * tre, const trie_node_val * tv_arr, int len)
{
    c_vector * next_node_set;
    mem_pool * mpl = get_mpl(tre);
    int orig_cap = tre->orig_cap;
    trie_node val_node, dummy, * this_node;

    val_node.next = NULL;
    dummy.what.val = DUMMY_VAL;
    dummy.what.tag = TRIE_TAG_NULL;
    dummy.next = get_root_set(tre);
    this_node = &dummy;

    for (int i = 0, last = len-1; i < len; ++i)
    {
        next_node_set = get_node_set(this_node);

        val_node.what = tv_arr[i];
        if (!next_node_set)
        {
            next_node_set = this_node->next = mpl_get(mpl);
            if (!c_vect_make_cap(
                next_node_set, sizeof(trie_node), compar_tnodes, orig_cap))
                return NULL;

            this_node = c_vect_insert_online(next_node_set, &val_node);
        }
        else
        {
            this_node = c_vect_bsearch(next_node_set, &val_node);
            if (!this_node)
                this_node = c_vect_insert_online(next_node_set, &val_node);
            else if (i == last)
                this_node->what.tag = val_node.what.tag;
        }
    }

    return tre;
}
//------------------------------------------------------------------------------

const trie_node * trie_lookup(const trie * tre, const trie_node_val * arr, int len)
{
    const trie_node * node = trie_lookup_first(tre, arr);

    for (int i = 1; (i < len) && node; ++i)
        node = trie_lookup_next(node, arr+i);

    return node;
}
//------------------------------------------------------------------------------

const trie_node * trie_lookup_first(const trie * tre, const trie_node_val * val)
{
    trie_node val_node, dummy;

    val_node.what = *val;
    val_node.next = NULL;
    dummy.what.val = DUMMY_VAL;
    dummy.what.tag = TRIE_TAG_NULL;
    dummy.next = (c_vector *)get_root_set(tre);

    return c_vect_bsearch(dummy.next, &val_node);
}
//------------------------------------------------------------------------------

const trie_node * trie_lookup_next(const trie_node * tn, const trie_node_val * val)
{
    const trie_node * node = NULL;
    c_vector * nset = (c_vector *)get_node_set(tn);

    if (nset)
    {
        trie_node val_node;
        val_node.what = *val;
        val_node.next = NULL;
        node = c_vect_bsearch(nset, &val_node);
    }

    return node;
}
//------------------------------------------------------------------------------

const c_vector * trie_root_get_set(const trie * tre)
{
    return get_root_set(tre);
}
//------------------------------------------------------------------------------

const trie_node_val * trie_node_get_val(const trie_node * tn)
{
    return get_node_tval(tn);
}
//------------------------------------------------------------------------------

const c_vector * trie_node_get_set(const trie_node * tn)
{
    return get_node_set(tn);
}
//------------------------------------------------------------------------------
