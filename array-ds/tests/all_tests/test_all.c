#include <stdio.h>
#include "../../c_vector/test_c_vect.h"
#include "../../ov_map/test_ov_map.h"
#include "../../ov_set/test_ov_set.h"
#include "../../ov_multimap/test_ov_mmap.h"
#include "../../ov_multiset/test_ov_mset.h"
#include "../../mem_pool/test_mem_pool.h"
#include "../../bit_vector/test_bit_vect.h"
#include "../../oa_htable/test_oa_htbl.h"
#include "../../cv_htable/test_cv_htbl.h"
#include "../../fixed_array/test_f_arr.h"
#include "../../trie/test_trie.h"
#include "../../array_pool/test_array_pool.h"

typedef struct test_funs {
    void (*run_tests)(void);
    int (*get_passed)(void);
    int (*get_failed)(void);
} test_funs;

static test_funs all_tests[] = {
    {run_tests_c_vect, tc_vect_passed, tc_vect_failed},
    {run_tests_ov_map, tov_map_passed, tov_set_failed},
    {run_tests_ov_set, tov_set_passed, tov_set_failed},
    {run_tests_ov_mmap, tov_mmap_passed, tov_mmap_failed},
    {run_tests_ov_mset, tov_mset_passed, tov_mset_failed},
    {run_tests_mem_pool, tmem_pool_passed, tmem_pool_failed},
    {run_tests_bit_vect, tbit_vect_passed, tbit_vec_failed},
    {run_tests_oa_htbl, toa_htbl_passed, toa_htbl_failed},
    {run_tests_cv_htbl, tcv_htbl_passed, tcv_htbl_failed},
    {run_tests_f_array, tf_arr_passed, tf_arr_failed},
    {run_tests_trie, ttrie_passed, ttrie_failed},
    {run_tests_array_pool, tarr_pool_passed, tarr_pool_failed},
};

int main(void)
{
    int passed = 0, failed = 0, end = sizeof(all_tests)/sizeof(*all_tests);

    for (int i = 0; i < end; ++i)
    {
        all_tests[i].run_tests();
        passed += all_tests[i].get_passed();
        failed += all_tests[i].get_failed();
    }

    putchar('\n');
    printf("Files tested: %d\n", end);
    printf("All passed  : %d\n", passed);
    printf("All failed  : %d\n", failed);
    return 0;
}

