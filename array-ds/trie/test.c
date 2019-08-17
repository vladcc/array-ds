#include <stdio.h>
#include <string.h>
#include "trie.h"
#include "test.h"
#include "test_trie.h"

static bool test_trie(void);

static ftest tests[] = {
    test_trie,
};
//------------------------------------------------------------------------------

static bool test_trie(void)
{
#define LEN 100
    trie_val vals[LEN];
    trie tre_, * the_trie = &tre_;

    {
        check(trie_make(the_trie) == the_trie);
        char str[] = "a";
        int len = strlen(str);
        trie_str_to_tval(str, vals, len);

        check(trie_insert(the_trie, vals, len) == the_trie);

        const c_vector * rset = trie_get_root_set(the_trie);
        check(c_vect_length(rset) == 1);

        const trie_node * tnode = trie_lookup_first(the_trie, &vals[0]);
        check(tnode);
        check(trie_get_node_set(tnode) == NULL);

        const trie_val * tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == str[0]);
        check(tval->tag == TRIE_TAG_WORD_END);

        tnode = NULL;
        tval = NULL;
        check(!tnode && !tval);
        tnode = trie_lookup(the_trie, vals, len);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == str[0]);
        check(tval->tag == TRIE_TAG_WORD_END);

        trie_destroy(the_trie);
    }

    {
        check(trie_make(the_trie) == the_trie);
        char str[] = "abc";
        int len = strlen(str);
        trie_str_to_tval(str, vals, len);

        check(trie_insert(the_trie, vals, len) == the_trie);

        const c_vector * rset = trie_get_root_set(the_trie);
        check(c_vect_length(rset) == 1);

        int lup_ind = 0;
        const trie_node * tnode = trie_lookup_first(the_trie, &vals[lup_ind]);
        check(tnode);
        const trie_val * tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == str[lup_ind]);
        check(tval->tag == TRIE_TAG_DEFAULT);

        ++lup_ind;
        tnode = trie_lookup_next(tnode, &vals[lup_ind]);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(trie_val_get_val(tval) == str[lup_ind]);
        check(trie_val_get_tag(tval) == TRIE_TAG_DEFAULT);
        check(trie_get_node_set(tnode) != NULL);

        ++lup_ind;
        tnode = trie_lookup_next(tnode, &vals[lup_ind]);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == str[lup_ind]);
        check(tval->tag == TRIE_TAG_WORD_END);
        check(trie_get_node_set(tnode) == NULL);

        tnode = NULL;
        tval = NULL;
        check(!tnode && !tval);
        tnode = trie_lookup(the_trie, vals, len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == str[len-1]);
        check(tval->tag == TRIE_TAG_WORD_END);

        trie_destroy(the_trie);
    }

    {
        check(trie_make(the_trie) == the_trie);
        char str[] = "abc";
        int len = strlen(str);
        trie_str_to_tval(str, vals, len);
        check(vals[len-1].tag == TRIE_TAG_WORD_END);

        check(trie_insert(the_trie, vals, len) == the_trie);

        const c_vector * rset = trie_get_root_set(the_trie);
        check(c_vect_length(rset) == 1);

        const trie_node * tnode;
        const trie_val * tval;

        tnode = trie_lookup(the_trie, vals, len-1);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        const trie_node * remember_b = tnode;

        check(tval->val == 'b');
        check(tval->tag == TRIE_TAG_DEFAULT);

        tnode = trie_lookup(the_trie, vals, len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'c');
        check(tval->tag == TRIE_TAG_WORD_END);

        char str2[] = "ab";
        int len2 = strlen(str2);
        trie_str_to_tval(str2, vals, len2);
        check(vals[len2-1].val == 'b');
        check(vals[len2-1].tag == TRIE_TAG_WORD_END);

        tnode = trie_lookup(the_trie, vals, len2);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == 'b');
        check(tval->tag == TRIE_TAG_DEFAULT);

        check(trie_insert(the_trie, vals, len2) == the_trie);

        tnode = trie_lookup(the_trie, vals, len2);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tnode == remember_b);
        check(tval->val == 'b');
        check(tval->tag == TRIE_TAG_WORD_END);

        tnode = trie_lookup(the_trie, vals, len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'c');
        check(tval->tag == TRIE_TAG_WORD_END);

        char str3[] = "abcdez";
        int len3 = strlen(str3);
        trie_str_to_tval(str3, vals, len3);

        check(trie_insert(the_trie, vals, len3) == the_trie);

        tnode = trie_lookup(the_trie, vals, len3);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        tnode = trie_lookup(the_trie, vals, len2-1);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'a');
        check(tval->tag == TRIE_TAG_DEFAULT);

        tnode = trie_lookup(the_trie, vals, len2);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tnode == remember_b);
        check(tval->val == 'b');
        check(tval->tag == TRIE_TAG_WORD_END);

        tnode = trie_lookup(the_trie, vals, len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'c');
        check(tval->tag == TRIE_TAG_WORD_END);

        tnode = trie_lookup(the_trie, vals, len3-2);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'd');
        check(tval->tag == TRIE_TAG_DEFAULT);

        tnode = trie_lookup(the_trie, vals, len3-1);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'e');
        check(tval->tag == TRIE_TAG_DEFAULT);

        tnode = trie_lookup(the_trie, vals, len3);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'z');
        check(tval->tag == TRIE_TAG_WORD_END);
        check(trie_get_node_set(tnode) == NULL);

        trie_destroy(the_trie);
    }

    {
        check(trie_make(the_trie) == the_trie);
        char apple_str[] = "apple";
        int apple_len = strlen(apple_str);
        trie_str_to_tval(apple_str, vals, apple_len);
        check(trie_insert(the_trie, vals, apple_len) == the_trie);

        char applause_str[] = "applause";
        int applause_len = strlen(applause_str);
        trie_str_to_tval(applause_str, vals, applause_len);
        check(trie_insert(the_trie, vals, applause_len) == the_trie);

        char apprehension_str[] = "apprehension";
        int apprehension_len = strlen(apprehension_str);
        trie_str_to_tval(apprehension_str, vals, apprehension_len);
        check(trie_insert(the_trie, vals, apprehension_len) == the_trie);

        char tank_str[] = "tank";
        int tank_len = strlen(tank_str);
        trie_str_to_tval(tank_str, vals, tank_len);
        check(trie_insert(the_trie, vals, tank_len) == the_trie);

        char tiger_str[] = "tiger";
        int tiger_len = strlen(tiger_str);
        trie_str_to_tval(tiger_str, vals, tiger_len);
        check(trie_insert(the_trie, vals, tiger_len) == the_trie);

        const c_vector * rset = trie_get_root_set(the_trie);
        check(c_vect_length(rset) == 2);

        const trie_node * tnode;
        const trie_val * tval;

        char app_str[] = "app";
        int app_len = strlen(app_str);
        trie_str_to_tval(app_str, vals, app_len);

        tnode = trie_lookup(the_trie, vals, app_len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);

        check(tval->val == 'p');
        check(tval->tag == TRIE_TAG_DEFAULT);

        const c_vector * nset = trie_get_node_set(tnode);
        check(c_vect_length(nset) == 2);

        trie_str_to_tval(apple_str, vals, apple_len);
        tnode = trie_lookup(the_trie, vals, apple_len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == 'e');
        check(tval->tag == TRIE_TAG_WORD_END);

        trie_str_to_tval(applause_str, vals, applause_len);
        tnode = trie_lookup(the_trie, vals, applause_len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == 'e');
        check(tval->tag == TRIE_TAG_WORD_END);

        trie_str_to_tval(apprehension_str, vals, apprehension_len);
        tnode = trie_lookup(the_trie, vals, apprehension_len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == 'n');
        check(tval->tag == TRIE_TAG_WORD_END);

        trie_val _tv = {.val = 't', .tag = TRIE_TAG_DEFAULT};
        tnode = trie_lookup_first(the_trie, &_tv);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == 't');
        check(tval->tag == TRIE_TAG_DEFAULT);

        nset = trie_get_node_set(tnode);
        check(c_vect_length(nset) == 2);

        trie_str_to_tval(tank_str, vals, tank_len);
        tnode = trie_lookup(the_trie, vals, tank_len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == 'k');
        check(tval->tag == TRIE_TAG_WORD_END);

        trie_str_to_tval(tiger_str, vals, tiger_len);
        tnode = trie_lookup(the_trie, vals, tiger_len);
        check(tnode);
        tval = trie_get_node_tval(tnode);
        check(tval);
        check(tval->val == 'r');
        check(tval->tag == TRIE_TAG_WORD_END);

        trie zero_trie;
        memset(&zero_trie, 0, sizeof(zero_trie));
        check(memcmp(the_trie, &zero_trie, sizeof(trie)) != 0);

        trie_destroy(the_trie);
        check(memcmp(the_trie, &zero_trie, sizeof(trie)) == 0);
    }

    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_trie(void)
{
    int i, end = sizeof(tests)/sizeof(*tests);

    passed = 0;
    for (i = 0; i < end; ++i)
        if (tests[i]())
            ++passed;

    if (passed != end)
        putchar('\n');

    failed = end - passed;
    report(passed, failed);
    return;
}
//------------------------------------------------------------------------------

int ttrie_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int ttrie_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
