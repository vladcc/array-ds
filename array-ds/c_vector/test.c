#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "c_vector.h"
#include "test.h"

static bool test_make_cap_vect(void);
static bool test_destroy_vect(void);
static bool test_push(void);
static bool test_get(void);
static bool test_trim(void);
static bool test_resize(void);
static bool test_pop(void);
static bool test_peek(void);
static bool test_find_rem_by_val(void);
static bool test_is_sorted(void);
static bool test_sort(void);
static bool test_bsearch(void);
static bool test_insert_online(void);
static bool test_reset_empty(void);
static bool test_append_array(void);
static bool test_write_at_index(void);
static bool test_set_compar(void);
static bool test_insert_at_index(void);
static bool test_remove_at_index(void);
static bool test_apply_args(void);
static bool test_apply(void);
static bool test_zero_out(void);
static bool test_set_length(void);
static bool test_copy(void);
static bool test_swap(void);
static bool test_swap_pop_reverse(void);
static bool test_peek_pop(void);
static bool test_swap_pop_by_val(void);
static bool test_set_to_val(void);
static bool test_remove_range(void);
static bool test_find_if(void);
static bool test_remove_if(void);
static bool test_swap_pop_if(void);

static ftest tests[] = {
    test_make_cap_vect,
    test_destroy_vect,
    test_push,
    test_get,
    test_trim,
    test_resize,
    test_pop,
    test_peek,
    test_find_rem_by_val,
    test_is_sorted,
    test_sort,
    test_bsearch,
    test_insert_online,
    test_reset_empty,
    test_append_array,
    test_write_at_index,
    test_set_compar,
    test_insert_at_index,
    test_remove_at_index,
    test_apply_args,
    test_apply,
    test_zero_out,
    test_set_length,
    test_copy,
    test_swap,
    test_swap_pop_reverse,
    test_swap_pop_by_val,
    test_peek_pop,
    test_set_to_val,
    test_remove_range,
    test_find_if,
    test_remove_if,
    test_swap_pop_if
};

typedef unsigned char byte;
//------------------------------------------------------------------------------

static int compar(const void * k1, const void * k2)
{
    int i1 = *((int *)k1);
    int i2 = *((int *)k2);
    return (i1 > i2) - (i1 < i2);
}

static int equal(const void * k1, const void * k2)
{
    return !(*(int *)k1 == *(int *)k2);
}

static int not_equal(const void * k1, const void * k2)
{
    return !(*(int *)k1 != *(int *)k2);
}

static int less(const void * k1, const void * k2)
{
    return !(*(int *)k1 < *(int *)k2);
}

static int greater(const void * k1, const void * k2)
{
    return !(*(int *)k1 > *(int *)k2);
}
//------------------------------------------------------------------------------

static bool test_swap_pop_if(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    int arr[] = {1, 5, 5, 3, 5, 4, 6};
    int sz = sizeof(arr)/sizeof(*arr);

    int * elm;
    for (int i = 0; i < sz; ++i)
        check(*(elm = c_vect_push(vect, arr+i)) == arr[i]);

    int what = 510;
    check(!c_vect_swap_pop_if(vect, equal, &what));
    what = 5;
    check(c_vect_swap_pop_if(vect, less, &what) == 3);
    check(c_vect_length(vect) == 4);
    check(c_vect_compar(vect) == compar);
    check(c_vect_swap_pop_if(vect, equal, &what) == 3);
    check(c_vect_compar(vect) == compar);
    check(c_vect_length(vect) == 1);
    check(c_vect_swap_pop_if(vect, greater, &what) == 1);
    check(c_vect_is_empty(vect));

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_remove_if(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    int arr[] = {1, 5, 5, 3, 5, 4, 6};
    int sz = sizeof(arr)/sizeof(*arr);

    int * elm;
    for (int i = 0; i < sz; ++i)
        check(*(elm = c_vect_push(vect, arr+i)) == arr[i]);

    int what = 5;
    check(c_vect_compar(vect) == compar);
    check(c_vect_remove_if(vect, less, &what) == 3);
    what = 510;
    check(!c_vect_remove_if(vect, greater, &what));
    what = 5;
    check(c_vect_compar(vect) == compar);
    check(c_vect_length(vect) == 4);
    check(c_vect_is_sorted(vect));
    check(c_vect_remove_if(vect, equal, &what) == 3);
    check(c_vect_length(vect) == 1);
    check(c_vect_remove_if(vect, greater, &what) == 1);
    check(c_vect_is_empty(vect));

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_find_if(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    check(c_vect_compar(vect) == compar);
    check(c_vect_set_compar(vect, NULL) == vect);
    check(c_vect_compar(vect) == NULL);
    check(c_vect_set_compar(vect, compar) == vect);
    check(c_vect_compar(vect) == compar);

    int end = 10;
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);

    int * elm;
    check(*(elm = c_vect_get(vect, 0)) == 0);
    check(*(elm = c_vect_get(vect, 3)) == 3);
    check(*(elm = c_vect_get(vect, 5)) == 5);
    check(*(elm = c_vect_get(vect, 9)) == 9);

    int from = 0, ifound = 0, what = -5;
    void * pfound = c_vect_find_if_ind_from(vect, equal, &what, &ifound, from);
    check(!pfound);
    check(ifound == -1);

    what = 2000;
    check(c_vect_compar(vect) == compar);
    pfound = c_vect_find_if_ind_from(vect, equal, &what, &ifound, from);
    check(!pfound);
    check(ifound == -1);
    check(c_vect_compar(vect) == compar);

    what = 5;
    check(pfound = c_vect_find_if_ind_from(vect, equal, &what, &ifound, from));
    check(*(elm = pfound) == 5);
    check(ifound == 5);

    check(c_vect_is_sorted(vect));

    what = 9;
    check(pfound = c_vect_find_if_ind(vect, equal, &what, &ifound));
    check(*(elm = pfound) == 9);
    check(ifound == 9);

    what = 7;
    check(pfound = c_vect_find_if_from(vect, equal, &what, 0));
    check(*(elm = pfound) == 7);
    check(ifound == 9);

    what = 0;
    check(pfound =
        c_vect_find_if_ind_from(vect, not_equal, &what, &ifound, from));
    check(*(elm = pfound) == 1);
    check(ifound == 1);

    what = 4;
    check(pfound =
        c_vect_find_if_ind_from(vect, not_equal, &what, &ifound, from));
    check(*(elm = pfound) == 0);
    check(ifound == 0);

    check(c_vect_reset(vect) == vect);
    int same[] = {5, 5, 5, 5, 5};
    for (int i = 0; i < 5; ++i)
        c_vect_push(vect, same+i);

    what = 5;
    pfound = c_vect_find_if_ind_from(vect, not_equal, &what, &ifound, from);
    check(!pfound);
    check(ifound == -1);

    pfound = c_vect_find_if_ind_from(vect, equal, &what, &ifound, from);
    check(*(elm = pfound) == 5);
    check(ifound == 0);

    what = 1;
    pfound = c_vect_find_if_ind_from(vect, not_equal, &what, &ifound, from);
    check(*(elm = pfound) == 5);
    check(ifound == 0);

    check(c_vect_reset(vect) == vect);
    int arr[] = {1, 5, 5, 3, 5, 4, 6};
    int sz = sizeof(arr)/sizeof(*arr);

    for (int i = 0; i < sz; ++i)
        check(*(elm = c_vect_push(vect, arr+i)) == arr[i]);

    what = 5;
    pfound = c_vect_find_if_ind_from(vect, less, &what, &ifound, from);
    check(*(elm = pfound) == 1);
    check(ifound == 0);

    from = ifound+1;
    pfound = c_vect_find_if_ind_from(vect, less, &what, &ifound, from);
    check(*(elm = pfound) == 3);
    check(ifound == 3);

    from = ifound+1;
    pfound = c_vect_find_if_ind_from(vect, less, &what, &ifound, from);
    check(*(elm = pfound) == 4);
    check(ifound == 5);

    from = ifound+1;
    pfound = c_vect_find_if_ind_from(vect, less, &what, &ifound, from);
    check(!pfound);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_remove_range(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;

    int arr[] = {1111, 22222, 33333, 44444, 55555, 66666, 77777};
    int arrsz = sizeof(arr)/sizeof(*arr);
    check(c_vect_make_cap(vect, esz, compar, arrsz) == vect);

    check(c_vect_reset(vect) == vect);
    check(c_vect_is_empty(vect));

    int * elm;
    for (int i = 0; i < arrsz; ++i)
        check(*(elm = c_vect_push(vect, arr+i)) == arr[i]);
    check(c_vect_length(vect) == arrsz);
    check(c_vect_trim(vect) == vect);

    check(!c_vect_remove_range(vect, -1, 3));
    check(!c_vect_remove_range(vect, 3, 2));
    check(!c_vect_remove_range(vect, -1, -5));
    check(!c_vect_remove_range(vect, 0, 100));
    check(!c_vect_remove_range(vect, 30, 100));

    check(c_vect_remove_range(vect, 1, 5) == vect);
    check(c_vect_length(vect) == 2);
    check(*(elm = c_vect_get(vect, 0)) == 1111);
    check(*(elm = c_vect_get(vect, 1)) == 77777);

    check(c_vect_remove_range(vect, 1, 1) == vect);
    check(c_vect_length(vect) == 1);
    check(*(elm = c_vect_get(vect, 0)) == 1111);

    check(c_vect_remove_range(vect, 0, 0) == vect);
    check(c_vect_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        check(*(elm = c_vect_push(vect, arr+i)) == arr[i]);
    check(c_vect_length(vect) == arrsz);
    check(c_vect_remove_range(vect, 0, 6) == vect);
    check(c_vect_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        check(*(elm = c_vect_push(vect, arr+i)) == arr[i]);
    check(c_vect_length(vect) == arrsz);

    check(c_vect_remove_range(vect, 0, 0) == vect);
    check(c_vect_length(vect) == arrsz-1);
    for (int i = 0; i < arrsz-1; ++i)
        check(*(elm = c_vect_get(vect, i)) == arr[i+1]);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_set_to_val(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);


    int arr[] = {1, 2, 3, 4, 5, 6, 7};

    int arrsz = sizeof(arr)/sizeof(*arr);
    check(c_vect_reset(vect) == vect);
    check(c_vect_is_empty(vect));

    int * elm;
    for (int i = 0; i < arrsz; ++i)
        check(*(elm = c_vect_push(vect, arr+i)) == arr[i]);
    check(c_vect_length(vect) == arrsz);

    int n = 5;
    check(c_vect_set_to_val(vect, &n) == vect);

    for (int i = 0; i < arrsz; ++i)
        check(*(elm = c_vect_get(vect, i)) == n);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_swap_pop_by_val(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    int n = 5;
    check(c_vect_swap_pop_by_val(vect, &n) == 0);

    int arr[] = {4, 4, 5, 4, 4};
    int arrsz = sizeof(arr)/sizeof(*arr);

    check(c_vect_is_empty(vect));
    for (int i = 0; i < arrsz; ++i)
        c_vect_push(vect, arr+i);

    check(c_vect_length(vect) == arrsz);
    check(c_vect_swap_pop_by_val(vect, &n) == 1);
    check(c_vect_length(vect) == arrsz-1);


    int * elm;
    for (int i = 0; i < arrsz-1; ++i)
        check(*(elm = c_vect_get(vect, i)) == 4);

    int arr2[] = {5, 4, 4, 5, 4, 4, 5, 5};
    arrsz = sizeof(arr2)/sizeof(*arr2);
    check(c_vect_reset(vect) == vect);
    check(c_vect_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        c_vect_push(vect, arr2+i);

    check(c_vect_length(vect) == arrsz);
    check(c_vect_swap_pop_by_val(vect, &n) == 4);
    check(c_vect_length(vect) == arrsz-4);

    for (int i = 0; i < arrsz-4; ++i)
        check(*(elm = c_vect_get(vect, i)) == 4);

    n = 4;
    check(c_vect_swap_pop_by_val(vect, &n) == 4);
    check(c_vect_is_empty(vect));

    int arr3[] = {4, 4, 4, 4, 4, 4, 4, 4};

    arrsz = sizeof(arr3)/sizeof(*arr3);
    check(c_vect_reset(vect) == vect);
    check(c_vect_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        c_vect_push(vect, arr3+i);

    check(c_vect_length(vect) == arrsz);
    check(c_vect_swap_pop_by_val(vect, &n) == arrsz);
    check(c_vect_is_empty(vect));

    c_vect_push(vect, &n);
    check(c_vect_length(vect) == 1);
    check(c_vect_swap_pop_by_val(vect, &n) == 1);
    check(c_vect_is_empty(vect));

    int arr4[] = {1, 2, 3, 4};

    arrsz = sizeof(arr4)/sizeof(*arr4);
    check(c_vect_reset(vect) == vect);
    check(c_vect_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        c_vect_push(vect, arr4+i);
    check(c_vect_length(vect) == arrsz);

    n = 1;
    check(c_vect_swap_pop_by_val(vect, &n) == 1);
    check(c_vect_length(vect) == 3);
    check(*(elm = c_vect_get(vect, 0)) == 4);
    check(*(elm = c_vect_get(vect, 1)) == 2);
    check(*(elm = c_vect_get(vect, 2)) == 3);

    n = 2;
    check(c_vect_swap_pop_by_val(vect, &n) == 1);
    check(c_vect_length(vect) == 2);
    check(*(elm = c_vect_get(vect, 0)) == 4);
    check(*(elm = c_vect_get(vect, 1)) == 3);

    n = 3;
    check(c_vect_swap_pop_by_val(vect, &n) == 1);
    check(c_vect_length(vect) == 1);
    check(*(elm = c_vect_get(vect, 0)) == 4);

    n = 4;
    check(c_vect_swap_pop_by_val(vect, &n) == 1);
    check(c_vect_is_empty(vect));

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_peek_pop(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    check(!c_vect_swap_pop_at(vect, -1));
    check(!c_vect_swap_pop_at(vect, 1000));

    int end = 10;
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);

    check(c_vect_length(vect) == end);
    int * peek = c_vect_peek(vect);
    int * elm;
    check(*(elm = c_vect_peek_pop(vect)) == end-1);
    check(peek == elm);
    check(c_vect_length(vect) == end-1);
    check(*(elm = c_vect_peek_pop(vect)) == end-2);
    check(c_vect_length(vect) == end-2);

    while(c_vect_peek_pop(vect))
        continue;

    check(c_vect_is_empty(vect));

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_swap_pop_reverse(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    check(!c_vect_swap_pop_at(vect, -1));
    check(!c_vect_swap_pop_at(vect, 1000));

    int end = 10;
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);

    check(c_vect_length(vect) == end);
    check(c_vect_swap_pop_at(vect, 0) == vect);
    check(c_vect_length(vect) == end-1);
    int * elm;
    check(*(elm = c_vect_get(vect, 0)) == end-1);
    check(c_vect_swap_pop_at(vect, end-2) == vect);
    check(c_vect_length(vect) == end-2);
    check(*(elm = c_vect_get(vect, end-3)) == end-3);

    c_vect_destroy(vect);

    check(c_vect_make(vect, esz, compar) == vect);

    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);

    check(c_vect_length(vect) == end);
    check(c_vect_swap_pop_at(vect, 2) == vect);
    check(c_vect_length(vect) == end-1);
    check(*(elm = c_vect_get(vect, 2)) == end-1);

    while (c_vect_swap_pop_at(vect, 0))
        continue;
    check(c_vect_is_empty(vect));

    c_vect_destroy(vect);

    check(c_vect_make(vect, esz, compar) == vect);
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);
    check(c_vect_reverse(vect) == vect);

    for(int i = 0; i < end; ++i)
        check(*(elm = c_vect_get(vect, i)) == end-1-i);

    c_vect_destroy(vect);

    --end;
    check(c_vect_make(vect, esz, compar) == vect);
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);
    check(c_vect_reverse(vect) == vect);

    for(int i = 0; i < end; ++i)
        check(*(elm = c_vect_get(vect, i)) == end-1-i);


    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_swap(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    int n = 1;
    c_vect_push(vect, &n);
    n = 2;
    c_vect_push(vect, &n);

    int * elm;
    check(*(elm = c_vect_get(vect, 0)) == 1);
    check(*(elm = c_vect_get(vect, 1)) == 2);
    check(c_vect_swap(vect, 0, 1) == vect);
    check(*(elm = c_vect_get(vect, 0)) == 2);
    check(*(elm = c_vect_get(vect, 1)) == 1);
    check(c_vect_swap(vect, 1, 0) == vect);
    check(*(elm = c_vect_get(vect, 0)) == 1);
    check(*(elm = c_vect_get(vect, 1)) == 2);

    c_vect_reset(vect);
    int end = 10;
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);

    for (int i = 0; i < end; ++i)
        check(*(elm = c_vect_get(vect, i)) == i);

    check(!c_vect_swap(vect, -1, 4));
    check(!c_vect_swap(vect, 200, 4));
    check(!c_vect_swap(vect, 2, -5));
    check(!c_vect_swap(vect, 2, 99919));
    check(!c_vect_swap(vect, 22, 33));
    check(!c_vect_swap(vect, -9341, -321));

    int end2 = end/2, last = c_vect_length(vect) - 1;
    for (int i = 0; i < end2; ++i)
        check(c_vect_swap(vect, i, last-i) == vect);

    for (int i = 0, j = end-1; i < end; ++i, --j)
        check(*(elm = c_vect_get(vect, i)) == j);

    c_vect_destroy(vect);

    check(c_vect_make(vect, esz, compar) == vect);

    end = 9;
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);

    for (int i = 0; i < end; ++i)
        check(*(elm = c_vect_get(vect, i)) == i);

    end2 = end/2, last = c_vect_length(vect) - 1;
    for (int i = 0; i < end2; ++i)
        check(c_vect_swap(vect, i, last-i) == vect);

    for (int i = 0, j = end-1; i < end; ++i, --j)
        check(*(elm = c_vect_get(vect, i)) == j);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_copy(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, compar) == vect);

    bool success = true;
    c_vector vtry_;
    c_vector vno_, * vno = &vno_;

    memset(vno, 0, sizeof(*vno));
    vtry_ = c_vect_copy(vect, &success);
    check(memcmp(&vtry_, vno, sizeof(c_vector)) == 0);
    check(!success);

    success = false;
    int end = c_vect_capacity(vect) - 5;
    for (int i = 0; i < end; ++i)
        c_vect_push(vect, &i);

    c_vector cpy_, * cpy = &cpy_;
    cpy->elem_count = -1;
    cpy->elem_size = -1;
    cpy->arr = NULL;
    cpy->capacity = -1;
    cpy->compar = NULL;

    check(c_vect_length(cpy) != c_vect_length(vect));
    check(c_vect_elem_size(cpy) != c_vect_elem_size(vect));
    check(c_vect_data(cpy) != c_vect_data(vect));
    check(c_vect_capacity(cpy) != c_vect_capacity(vect));
    check(cpy->compar != vect->compar);

    cpy_ = c_vect_copy(vect, &success);
    check(success);
    check(c_vect_length(cpy) == c_vect_length(vect));
    check(c_vect_elem_size(cpy) == c_vect_elem_size(vect));
    check(c_vect_data(cpy) != c_vect_data(vect));
    check(c_vect_capacity(cpy) == c_vect_length(vect));
    check(c_vect_compar(cpy) == c_vect_compar(vect));
    check(cpy->compar == vect->compar);

    end = c_vect_length(cpy);
    for (int i = 0; i < end; ++i)
        check(*((int *)c_vect_get(cpy, i)) == *((int *)c_vect_get(vect, i)));

    int old_cpy_cap = c_vect_capacity(cpy);

    check(c_vect_capacity(cpy) == old_cpy_cap);
    c_vect_push(cpy, &old_cpy_cap);
    check(c_vect_capacity(cpy) == old_cpy_cap*2);

    c_vect_destroy(cpy);

    check(c_vect_length(cpy) != c_vect_length(vect));
    check(c_vect_elem_size(cpy) != c_vect_elem_size(vect));
    check(c_vect_data(cpy) != c_vect_data(vect));
    check(c_vect_capacity(cpy) != c_vect_capacity(vect));
    check(cpy->compar != vect->compar);

    success = false;
    cpy_ = c_vect_copy(vect, &success);
    check(success);
    check(c_vect_length(cpy) == c_vect_length(vect));
    check(c_vect_elem_size(cpy) == c_vect_elem_size(vect));
    check(c_vect_data(cpy) != c_vect_data(vect));
    check(c_vect_capacity(cpy) == c_vect_length(vect));
    check(c_vect_compar(cpy) == c_vect_compar(vect));
    check(cpy->compar == vect->compar);

    end = c_vect_length(cpy);
    for (int i = 0; i < end; ++i)
        check(*((int *)c_vect_get(cpy, i)) == *((int *)c_vect_get(vect, i)));

    c_vect_destroy(vect);

    for (int i = 0; i < end; ++i)
        check(*((int *)c_vect_get(cpy, i)) == i);

    c_vector cpy2 = c_vect_copy(cpy, NULL);
    c_vect_destroy(&cpy2);
    c_vect_destroy(cpy);
    return true;
}

//------------------------------------------------------------------------------

static bool test_set_length(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    int cap = 4;
    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);

    check(c_vect_length(vect) == 0);
    int len = 2;
    check(c_vect_set_length(vect, -200) == NULL);
    check(c_vect_set_length(vect, 100) == NULL);
    check(c_vect_set_length(vect, len) == vect);
    check(c_vect_length(vect) == len);
    check(c_vect_capacity(vect) == cap);
    check(c_vect_set_length(vect, -200) == NULL);
    check(c_vect_set_length(vect, 100) == NULL);
    check(c_vect_set_length(vect, cap) == vect);
    check(c_vect_length(vect) == cap);
    check(c_vect_push(vect, &len) == c_vect_peek(vect));
    check(c_vect_length(vect) == cap+1);
    check(c_vect_pop(vect));
    check(c_vect_length(vect) == cap);
    check(c_vect_set_length(vect, -200) == NULL);
    check(c_vect_set_length(vect, 100) == NULL);
    check(c_vect_capacity(vect) == cap*2);

    check(c_vect_zero_out(vect) == vect);
    int all = c_vect_length(vect);
    for (int i = 0; i < all; ++i)
        check(*((int *)c_vect_get(vect, i)) == 0);

    byte * data = c_vect_data(vect);
    all = c_vect_capacity(vect) * c_vect_elem_size(vect);
    for (int i = 0; i < all; ++i)
        check(data[i] == 0);

    c_vect_destroy(vect);

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    check(c_vect_zero_out(vect) == vect);

    data = c_vect_data(vect);
    all = c_vect_capacity(vect) * c_vect_elem_size(vect);
    for (int i = 0; i < all; ++i)
        check(data[i] == 0);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_zero_out(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;

    check(c_vect_make(vect, esz, NULL) == vect);

    int i, all = 5;
    for (i = 0; i < all; ++i)
    {
        c_vect_push(vect, &i);
        check(*((int *)c_vect_get(vect, i)) == i);
    }

    check(c_vect_length(vect) == all);
    check(c_vect_zero_out(vect) == vect);
    for (i = 0; i < all; ++i)
        check(*((int *)c_vect_get(vect, i)) == 0);

    c_vect_push(vect, &all);
    check(c_vect_length(vect) == all+1);
    check(*((int *)c_vect_get(vect, all)) == all);

    c_vect_destroy(vect);

    check(c_vect_make(vect, esz, NULL) == vect);
    int cap = c_vect_capacity(vect);

    for (i = 0; i < cap; ++i)
    {
        c_vect_push(vect, &i);
        check(*((int *)c_vect_get(vect, i)) == i);
    }
    check(c_vect_capacity(vect) == cap);
    check(c_vect_length(vect) == cap);
    check(c_vect_pop(vect) == vect);
    check(c_vect_pop(vect) == vect);
    check(c_vect_pop(vect) == vect);
    check(c_vect_length(vect) <= cap);

    check(c_vect_zero_out(vect) == vect);
    byte * data = c_vect_data(vect);
    all = c_vect_capacity(vect) * c_vect_elem_size(vect);
    for (int i = 0; i < all; ++i)
        check(data[i] == 0);


    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static void add5(void * arr_elm)
{
    int * elm = arr_elm;
    *elm += 5;
}

static bool test_apply(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    int cap = 4;
    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);

    int max = 10;
    for (int i = 0; i < max; ++i)
        c_vect_push(vect, &i);

    for (int i = 0; i < max; ++i)
        check(*((int *)c_vect_get(vect, i)) == i);

    int m = 5;
    check(c_vect_apply(vect, add5) == vect);
    for (int i = 0; i < max; ++i)
        check(*((int *)c_vect_get(vect, i)) == i+m);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

typedef struct add_args {
    int n;
} add_args;

static void addn(void * arr_elm, void * args)
{
    int * elm = arr_elm;
    add_args * padd = args;
    *elm += padd->n;
}

static bool test_apply_args(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    int cap = 4;
    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);

    int max = 10;
    for (int i = 0; i < max; ++i)
        c_vect_push(vect, &i);

    for (int i = 0; i < max; ++i)
        check(*((int *)c_vect_get(vect, i)) == i);

    int n = 1;
    add_args aa = {n};
    check(c_vect_apply_args(vect, addn, &aa) == vect);
    for (int i = 0; i < max; ++i)
        check(*((int *)c_vect_get(vect, i)) == i+n);

    int m = 5;
    aa.n = m;
    check(c_vect_apply_args(vect, addn, &aa) == vect);
    for (int i = 0; i < max; ++i)
        check(*((int *)c_vect_get(vect, i)) == i+n+m);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_remove_at_index(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    int cap = 4;
    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);

    int max = 10;
    for (int i = 0; i < max; ++i)
        c_vect_push(vect, &i);

    check(c_vect_remove_at(vect, -123) == NULL);
    check(c_vect_remove_at(vect, 123) == NULL);

    int rind = 4;
    int * n = c_vect_get(vect, rind);
    check(*n == 4);
    check(c_vect_remove_at(vect, rind) == vect);
    check(*n == 5);


    rind = 0;
    n = c_vect_get(vect, rind);
    check(*n == 0);
    check(c_vect_remove_at(vect, rind) == vect);
    check(*n == 1);

    rind = c_vect_length(vect) - 1;
    n = c_vect_get(vect, rind);
    check(*n == 9);
    check(c_vect_remove_at(vect, rind) == vect);
    check(c_vect_length(vect) == 7);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_insert_at_index(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    int cap = 4;
    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);

    check(c_vect_capacity(vect) == cap);

    int i, offs = 5;
    for (i = 1; i < cap; ++i)
    {
        int j = i * 2 + offs;
        c_vect_push(vect, &j);
    }

    check(c_vect_capacity(vect) == cap);
    int * first = c_vect_get(vect, 0);
    check(*first == 2+offs);

    int n = 0;
    check(c_vect_insert_at(vect, -123, &n) == NULL);
    check(c_vect_insert_at(vect, 123, &n) == NULL);

    check(c_vect_insert_at(vect, 0, &n) == c_vect_data(vect));
    first = c_vect_get(vect, 0);
    check(*first == n);
    check(c_vect_capacity(vect) == cap);

    n = 10;
    int last = c_vect_length(vect) - 1, * pe;
    check(c_vect_insert_at(vect, last, &n) == (int *)vect->arr + last);
    check(((int *)vect->arr)[last] == n);
    check(*(pe = c_vect_get(vect, last)) == n);
    check(c_vect_capacity(vect) == cap*2);

    int mid = 2;
    n = 8;
    check(c_vect_insert_at(vect, mid, &n) == (int *)vect->arr + mid);
    check(((int *)vect->arr)[mid] == n);
    check(*(pe = c_vect_get(vect, mid)) == n);

    check(c_vect_set_compar(vect, compar) == vect);
    check(c_vect_is_sorted(vect));
    check(c_vect_capacity(vect) == cap*2);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_write_at_index(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, NULL) == vect);

    int n = 6;
    check(c_vect_write_at(vect, -1, &n) == NULL);
    check(c_vect_write_at(vect, -123, &n) == NULL);
    check(c_vect_write_at(vect, 124, &n) == NULL);

    int cap = c_vect_capacity(vect);
    check(c_vect_write_at(vect, cap, &n) == NULL);

    check(vect->arr == c_vect_data(vect));
    check(c_vect_is_empty(vect));
    check(c_vect_write_at(vect, 0, &n) == NULL);

    int i;
    for (i = 0; i < 8; ++i)
        c_vect_push(vect, &i);

    check(c_vect_write_at(vect, -1, &n) == NULL);
    check(c_vect_write_at(vect, -123, &n) == NULL);
    check(c_vect_write_at(vect, 124, &n) == NULL);

    check(c_vect_write_at(vect, 0, &n) == c_vect_data(vect));
    check(((int *)vect->arr)[0] == n);
    int * pe;
    check(*(pe = c_vect_get(vect, 0)) == n);

    int len = c_vect_length(vect);
    check(c_vect_write_at(vect, len, &n) == NULL);
    --len;
    check(c_vect_write_at(vect, len, &n) == ((int *)vect->arr) + len);
    check(((int *)vect->arr)[len] == n);
    check(*(pe = c_vect_get(vect, len)) == n);

    int mid = 5;
    check(c_vect_write_at(vect, mid, &n) == ((int *)vect->arr) + mid);
    check(((int *)vect->arr)[mid] == n);
    check(*(pe = c_vect_get(vect, mid)) == n);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_append_array(void)
{
    int esz = sizeof(int);
    int cap = 1;
    c_vector vect_, * vect = &vect_;
    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);

    int iarr[] = {4, 5, 6, 7, 8};
    int size_iarr = sizeof(iarr)/sizeof(*iarr);

    check(c_vect_capacity(vect) == cap);
    check(c_vect_append_array(vect, iarr, size_iarr));
    check(c_vect_capacity(vect) == 8);

    int i, * elem;
    for(i = 0; i < size_iarr; ++i)
        check(*(elem = c_vect_get(vect, i)) == iarr[i]);

    c_vect_destroy(vect);

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    int iend = 4;
    for (i = 0; i < iend; ++i)
        c_vect_push(vect, &i);

    check(c_vect_length(vect) == i);
    check(c_vect_capacity(vect) == i);
    check(c_vect_append_array(vect, iarr, size_iarr));
    check(c_vect_capacity(vect) == 16);

    for (i = 0; i < iend; ++i)
        check(*(elem = c_vect_get(vect, i)) == i);

    for(int n = 0; n < size_iarr; ++n, ++i)
        check(*(elem = c_vect_get(vect, i)) == iarr[n]);

    check(c_vect_length(vect) == iend+size_iarr);

    check(c_vect_cap_size_bytes(vect) == 16*esz);
    check(c_vect_cap_size_bytes(vect) ==
        (c_vect_capacity(vect) * c_vect_elem_size(vect)));
    check(c_vect_data_size_bytes(vect) == i*esz);
    check(c_vect_data_size_bytes(vect) ==
        (c_vect_length(vect) * c_vect_elem_size(vect)));

    c_vect_push(vect, &i);
    for (i = 0; i < iend; ++i)
        check(*(elem = c_vect_get(vect, i)) == i);

    for(int n = 0; n < size_iarr; ++n, ++i)
        check(*(elem = c_vect_get(vect, i)) == iarr[n]);

    check(*(elem = c_vect_get(vect, c_vect_length(vect)-1)) == i);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_set_compar(void)
{
    int esz = sizeof(int);
    c_vector vect_, * vect = &vect_;
    check(c_vect_make(vect, esz, NULL) == vect);

    check(vect->compar == NULL);
    check(c_vect_set_compar(vect, compar) == vect);
    check(vect->compar == compar);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_reset_empty(void)
{
    int esz = sizeof(int);
    int cap = 1;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, compar, cap) == vect);
    check(c_vect_is_empty(vect));

    for (int i = 0; i < 10; ++i)
        c_vect_push(vect, &i);
    check(!c_vect_is_empty(vect));

    check(c_vect_reset(vect) == vect);
    check(c_vect_is_empty(vect));

    for (int i = 0; i < 10; ++i)
        c_vect_push(vect, &i);
    check(!c_vect_is_empty(vect));

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_insert_online(void)
{
    int esz = sizeof(int);
    int cap = 1;
    c_vector vect_, * vect = &vect_;
    check(c_vect_make_cap(vect, esz, compar, cap) == vect);

    int n = 1, * ins = NULL;
    check(*(ins = c_vect_insert_online(vect, &n)) == n);
    check(c_vect_is_sorted(vect) == vect);
    ++n;
    check(*(ins = c_vect_insert_online(vect, &n)) == n);
    check(c_vect_is_sorted(vect) == vect);

    int arr[] = {5, 4, 5, 3, 3, 5, 2, 1, 0, 1, 0, 0, 2, 1, 5, 5, -100};
    int arr_size = sizeof(arr)/sizeof(*arr);

    int i = 0, ind;
    for (; i < arr_size; ++i)
    {
        check(*(ins = c_vect_insert_online_ind(vect, arr + i, &ind)) == arr[i]);
        check(*(ins = c_vect_get(vect, ind)) == arr[i]);
        check(c_vect_is_sorted(vect) == vect);
    }
    check(2 + arr_size == c_vect_length(vect));

    n = 10;
    check(*(ins = c_vect_insert_online(vect, &n)) == n);
    check(c_vect_is_sorted(vect) == vect);

    check(*(ins = c_vect_insert_online_ind(vect, &n, &ind)) == n);
    check(*(ins = c_vect_get(vect, ind)) == n);
    check(c_vect_is_sorted(vect) == vect);

    check(*(ins = c_vect_insert_online(vect, &n)) == n);
    check(c_vect_is_sorted(vect) == vect);

    int * elem;
    check(*(elem = c_vect_get(vect, arr_size + 2)) == n);
    check(*(elem = c_vect_get(vect, arr_size + 1 + 2)) == n);
    check(*(elem = c_vect_get(vect, arr_size + 2 + 2)) == n);

    check(((int *)vect->arr)[arr_size + 2] == n);
    check(((int *)vect->arr)[arr_size + 1 + 2] == n);
    check(((int *)vect->arr)[arr_size + 2 + 2] == n);

    c_vect_destroy(vect);

    check(c_vect_make_cap(vect, esz, compar, cap) == vect);
    int newarr[] = {7, 8, 0, 1, 3, 6, 0, 4, 5, 3};

    arr_size = sizeof(newarr)/sizeof(*newarr);
    for (int i = 0; i < arr_size; ++i)
    {
        int ind;
        if (!c_vect_bsearch_ind(vect, newarr + i, &ind))
            check(*(ins = c_vect_insert_online(vect, newarr + i)) == newarr[i]);
    }

    check(c_vect_is_sorted(vect));
    int checkarr[] = {0, 1, 3, 4, 5, 6, 7, 8};

    arr_size = sizeof(checkarr)/sizeof(*checkarr);
    for (int i = 0; i < arr_size; ++i)
        check(*(elem = c_vect_get(vect, i)) == checkarr[i]);

    c_vect_destroy(vect);

    check(c_vect_make_cap(vect, esz, compar, cap) == vect);

    int newarr2[] = {5, 5, 5, 5};
    arr_size = sizeof(newarr2)/sizeof(*newarr2);

    check(c_vect_is_empty(vect));
    for (int i = 0; i < arr_size; ++i)
        c_vect_push(vect, newarr2+i);

    check(c_vect_is_sorted(vect));
    n = 5;
    check(*(ins = c_vect_insert_online(vect, &n)) == n);
    n = 1;
    check(*(ins = c_vect_insert_online(vect, &n)) == n);
    n = 10;
    check(*(ins = c_vect_insert_online(vect, &n)) == n);
    check(c_vect_is_sorted(vect));

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_bsearch(void)
{
    int esz = sizeof(int);
    int cap = 1;
    c_vector vect_, * vect = &vect_;
    check(c_vect_make_cap(vect, esz, compar, cap) == vect);

    int * key;
    check(!c_vect_bsearch_ind(vect, NULL, NULL));

    for (int i = 0; i < 10; ++i)
        c_vect_push(vect, &i);

    int n = 9;
    c_vect_push(vect, &n);
    n = 123;
    c_vect_push(vect, &n);
    check(c_vect_is_sorted(vect) == vect);

    struct search {
        int n;
        int pos;
        bool res;
    } tbl[] = {
        {-864, -1, false}, {89, -1, false}, {0, 0, true},
        {9, 10, true}, {123, 11, true}, {5, 5, true}
    };

    for (unsigned int i = 0; i < sizeof(tbl)/sizeof(*tbl); ++i)
    {
        key = &(tbl[i].n);
        void * res = c_vect_bsearch_ind(vect, key, NULL);
        check((bool)res == tbl[i].res);
        if (res)
            check(compar(res, key) == 0);

        int pos;
        res = c_vect_bsearch_ind(vect, key, &pos);
        check((bool)res == tbl[i].res);

        bool right_pos = (pos == tbl[i].pos);
        if (!right_pos)
            printf("n: %d, pos :%d\n", tbl[i].n, tbl[i].pos);

        check(right_pos);
    }

    void * first = c_vect_get(vect, 0);
    void * last = c_vect_peek(vect);
    int ind;
    void * ffirst = c_vect_bsearch_ind(vect, first, &ind);
    check(ffirst == first);
    check(0 == ind);
    check(compar(first, ffirst) == 0);

    void * flast = c_vect_bsearch_ind(vect, last, &ind);
    check(flast == last);
    check(c_vect_length(vect)-1 == ind);
    check(compar(flast, last) == 0);

    int less = -93471, more = 946242;
    int * out = c_vect_bsearch_ind(vect, &less, &ind);
    check(NULL == out);
    check(-1 == ind);

    out = c_vect_bsearch_ind(vect, &more, &ind);
    check(NULL == out);
    check(-1 == ind);

    c_vect_destroy(vect);

    check(c_vect_make_cap(vect, esz, compar, cap) == vect);
    n = 1;
    int i;
    for (i = 0; i < 10; ++i)
        c_vect_push(vect, &n);

    out = c_vect_bsearch_ind(vect, &n, &ind);
    check(out == c_vect_bsearch(vect, &n));
    check(*out == n);
    check(ind == (i-1)/2);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_is_sorted(void)
{
    int esz = sizeof(int);
    int cap = 1;
    c_vector vect_, * vect = &vect_;
    check(c_vect_make_cap(vect, esz, compar, cap) == vect);

    check(c_vect_is_sorted(vect) == NULL);
    int n = 10;
    c_vect_push(vect, &n);
    check(c_vect_length(vect) == 1);
    check(c_vect_is_sorted(vect) == vect);

    n = 9;
    c_vect_push(vect, &n);
    check(c_vect_is_sorted(vect) == NULL);
    c_vect_destroy(vect);

    check(c_vect_make_cap(vect, esz, compar, cap) == vect);

    for (int i = 0; i < 10; ++i)
        c_vect_push(vect, &i);
    check(c_vect_is_sorted(vect) == vect);

    n = 9;
    c_vect_push(vect, &n);
    check(c_vect_is_sorted(vect) == vect);

    n = 5;
    c_vect_push(vect, &n);
    check(c_vect_is_sorted(vect) == NULL);

    n = 19;
    c_vect_push(vect, &n);
    check(c_vect_is_sorted(vect) == NULL);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_sort(void)
{
    int esz = sizeof(int);
    int cap = 1;
    c_vector vect_, * vect = &vect_;
    check(c_vect_make_cap(vect, esz, compar, cap) == vect);

    for (int i = 10; i >= 0; --i)
        c_vect_push(vect, &i);
    check(c_vect_is_sorted(vect) == NULL);

    check(c_vect_sort(vect) == vect);
    check(c_vect_is_sorted(vect) == vect);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_find_rem_by_val(void)
{
    int esz = sizeof(int);
    int cap = 1;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, compar, cap) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == cap);
    check(vect->compar == compar);

    int pos = -1;
    for (int i = 10, j = 0; i >= 0; --i, ++j)
    {

        int prev_elem_count = vect->elem_count;
        void * target = c_vect_push(vect, &i);
        void * dest = (byte *)vect->arr + prev_elem_count * vect->elem_size;
        check(target == dest);
        check(*((int*)target) == i);
        check(c_vect_find_ind(vect, &i, NULL) == dest);
        check(c_vect_find(vect, &i) == dest);
        check(c_vect_find_ind(vect, &i, &pos) == dest);
        check(pos == j);
    }

    int n = 10;
    check(c_vect_find_ind(vect, &n, NULL) == c_vect_data(vect));
    check(c_vect_find_ind(vect, &n, &pos));
    check(pos == 0);

    n = -123;
    check(c_vect_find_ind(vect, &n, NULL) == NULL);
    check(c_vect_find_ind(vect, &n, &pos) == NULL);
    check(pos == -1);

    n = 35290;
    check(c_vect_find_ind(vect, &n, NULL) == NULL);
    check(c_vect_find_ind(vect, &n, &pos) == NULL);
    check(pos == -1);

    c_vect_destroy(vect);

    check(c_vect_make_cap(vect, esz, compar, cap) == vect);

    int arr[] = {1, 2, 1, 3, 4, 5, 5, 5, 8};
    int asz = sizeof(arr)/sizeof(*arr);

    int * elm;
    for (int i = 0; i < asz; ++i)
    {
        check(
            c_vect_push(vect, arr+i) == c_vect_get(vect, c_vect_length(vect)-1)
            );
        check(*(elm = c_vect_get(vect, i)) == arr[i]);
        check(c_vect_find_from(vect, arr+i, i) == c_vect_get(vect, i));
        check(*(elm = c_vect_find_from(vect, arr+i, i)) == arr[i]);
    }

    int oi = 100;
    check(!c_vect_find_ind_from(vect, arr, &oi, -1341));
    check(!c_vect_find_from(vect, arr, -1));
    check(!c_vect_find_ind_from(vect, arr, &oi, 32));
    check(!c_vect_find_from(vect, arr, 1000));
    check(oi == -1);

    check(c_vect_find_from(vect, arr, 0) == c_vect_get(vect, 0));
    check(oi == -1);
    check(c_vect_find_ind_from(vect, arr, &oi, 0) == c_vect_get(vect, 0));
    check(oi == 0);
    check(c_vect_find_ind_from(vect, arr, &oi, 1) == c_vect_get(vect, 2));
    check(oi == 2);
    check(c_vect_find_from(vect, arr, 1) == c_vect_get(vect, 2));

    int val = -123;
    check(!c_vect_remove_by_val(vect, &val));
    val = 1234;
    check(!c_vect_remove_by_val(vect, &val));

    val = 1;
    check(c_vect_remove_by_val(vect, &val) == 2);
    val = 5;
    check(c_vect_remove_by_val(vect, &val) == 3);
    val = 8;
    check(c_vect_remove_by_val(vect, &val) == 1);

    check(c_vect_length(vect) == 3);
    check(*(elm = c_vect_get(vect, 0)) == 2);
    check(*(elm = c_vect_get(vect, 1)) == 3);
    check(*(elm = c_vect_get(vect, 2)) == 4);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_make_cap_vect(void)
{
    c_vector vect_, * vect = &vect_;
    c_vector zout;
    memset(&zout, 0, sizeof(c_vector));

    memset(vect, 0xFF, sizeof(c_vector));
    check(memcmp(&zout, vect, sizeof(c_vector)) != 0);
    check(!c_vect_make_cap(vect, 5, NULL, 0));
    check(memcmp(&zout, vect, sizeof(c_vector)) == 0);

    memset(vect, 0xFF, sizeof(c_vector));
    check(memcmp(&zout, vect, sizeof(c_vector)) != 0);
    check(!c_vect_make_cap(vect, -1, NULL, 5));
    check(memcmp(&zout, vect, sizeof(c_vector)) == 0);

    memset(vect, 0xFF, sizeof(c_vector));
    check(memcmp(&zout, vect, sizeof(c_vector)) != 0);
    check(!c_vect_make_cap(vect, 5, NULL, -1));
    check(memcmp(&zout, vect, sizeof(c_vector)) == 0);

    memset(vect, 0xFF, sizeof(c_vector));
    check(memcmp(&zout, vect, sizeof(c_vector)) != 0);
    check(!c_vect_make_cap(vect, -23, NULL, 5));
    check(memcmp(&zout, vect, sizeof(c_vector)) == 0);

    memset(vect, 0xFF, sizeof(c_vector));
    check(memcmp(&zout, vect, sizeof(c_vector)) != 0);
    check(!c_vect_make_cap(vect, 0, NULL, 5));
    check(memcmp(&zout, vect, sizeof(c_vector)) == 0);

    int elem_size = sizeof(double);
    const int capacity = 5;
    check(c_vect_make_cap(vect, elem_size, compar, capacity) == vect);
    check(vect->arr);
    check(vect->compar == compar);
    check(vect->elem_size == elem_size);
    check(vect->capacity == capacity);
    check(vect->elem_count == 0);

    free(vect->arr);
    return true;
}
//------------------------------------------------------------------------------

static bool test_destroy_vect(void)
{
    int elem_size = sizeof(double);
    const int capacity = 5;
    c_vector vect_, * vect = &vect_;
    c_vector zout;
    memset(&zout, 0, sizeof(c_vector));

    check(c_vect_make_cap(vect, elem_size, NULL, capacity) == vect);
    check(vect->arr);
    check(c_vect_elem_size(vect) == elem_size);
    check(c_vect_capacity(vect) == capacity);
    check(c_vect_length(vect) == 0);

    check(memcmp(&zout, vect, sizeof(c_vector)) != 0);
    c_vect_destroy(vect);
    check(memcmp(&zout, vect, sizeof(c_vector)) == 0);

    check(vect->capacity == 0);
    check(vect->elem_count == 0);
    check(vect->elem_size == 0);
    check(vect->arr == 0);

    check(c_vect_make_cap(vect, elem_size, NULL, capacity) == vect);
    check(vect->arr);
    check(c_vect_elem_size(vect) == elem_size);
    check(c_vect_capacity(vect) == capacity);
    check(c_vect_length(vect) == 0);
    c_vect_destroy_null(vect);
    check(!vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_push(void)
{
    int esz = sizeof(double);
    int cap = 1;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == cap);
    check(c_vect_length(vect) == 0);

    double n = 5;
    void * dest = (byte *)vect->arr + vect->elem_count * vect->elem_size;
    check(c_vect_push(vect, &n) == dest);
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == cap);

    void * target = c_vect_push(vect, &n);
    dest = (byte *)vect->arr + (vect->elem_count-1) * vect->elem_size;
    check(target == dest);
    check(c_vect_length(vect) == 2);

    int gr = C_VECT_GROWTH_RATE;
    check(c_vect_capacity(vect) == cap*gr);

    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 3);
    check(c_vect_capacity(vect) == cap*gr*gr);

    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 4);
    check(c_vect_capacity(vect) == cap*gr*gr);

    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 5);
    check(c_vect_capacity(vect) == cap*gr*gr*gr);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_get(void)
{
    int esz = sizeof(double);
    int cap = 1;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == cap);
    check(c_vect_length(vect) == 0);
    check(c_vect_get(vect, -1) == NULL);
    check(c_vect_get(vect, 0) == NULL);
    check(c_vect_get(vect, 1) == NULL);
    check(c_vect_get(vect, 5) == NULL);
    check(c_vect_get(vect, 3000) == NULL);

    double n = 5;
    void * result = c_vect_push(vect, &n);
    check(result);

    n = 6;
    result = c_vect_push(vect, &n);
    check(result);

    n = 7;
    result = c_vect_push(vect, &n);
    check(result);

    check(c_vect_length(vect) == 3);
    double * ip = c_vect_get(vect, -1);
    check(!ip);
    ip = c_vect_get(vect, 100);
    check(!ip);
    ip = c_vect_get(vect, 2);
    check(ip);
    check(*ip == 7);
    ip = c_vect_get(vect, 1);
    check(ip);
    check(*ip == 6);
    ip = c_vect_get(vect, 0);
    check(ip);
    check(*ip == 5);

    check(c_vect_data(vect) == ip);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_trim(void)
{
    int esz = sizeof(double);
    int cap = 1;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == cap);
    check(c_vect_length(vect) == 0);
    check(c_vect_trim(vect) == NULL);

    double n = 5;
    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == cap);

    n = 6;
    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 2);
    check(c_vect_capacity(vect) == cap*C_VECT_GROWTH_RATE);
    double * get = c_vect_get(vect, 1);
    check(*get == n);

    check(c_vect_trim(vect) == vect);
    check(c_vect_capacity(vect) == cap*C_VECT_GROWTH_RATE);
    get = c_vect_get(vect, 1);
    check(*get == n);

    n = 7;
    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 3);
    check(c_vect_capacity(vect) == cap*C_VECT_GROWTH_RATE*C_VECT_GROWTH_RATE);
    get = c_vect_get(vect, 2);
    check(*get == n);

    check(c_vect_trim(vect) == vect);
    check(c_vect_capacity(vect) == 3);
    check(c_vect_capacity(vect) == c_vect_length(vect));
    get = c_vect_get(vect, 2);
    check(*get == n);

    n = 8;
    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 4);
    check(c_vect_capacity(vect) == 3*C_VECT_GROWTH_RATE);
    get = c_vect_get(vect, 3);
    check(*get == n);

    n = 9;
    check(c_vect_push(vect, &n));
    check(c_vect_length(vect) == 5);
    check(c_vect_capacity(vect) == 3*C_VECT_GROWTH_RATE);
    get = c_vect_get(vect, 4);
    check(*get == n);

    check(c_vect_trim(vect) == vect);
    check(c_vect_capacity(vect) == 5);
    check(c_vect_capacity(vect) == c_vect_length(vect));
    get = c_vect_get(vect, 2);
    check(*get == 7);
    get = c_vect_get(vect, 4);
    check(*get == n);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_resize(void)
{
    int esz = sizeof(double);
    int cap = 1;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == cap);
    check(c_vect_length(vect) == 0);

    check(!c_vect_resize(vect, 0));

    double n = 5;
    void * result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == cap);

    check(!c_vect_resize(vect, -1));
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == cap);

    int new_sz = 5;
    check(c_vect_resize(vect, new_sz));
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == new_sz);
    double * val = c_vect_get(vect, 0);
    check(val);
    check(*val == n);

    n = 6;
    result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 2);
    check(c_vect_capacity(vect) == new_sz);

    check(!c_vect_resize(vect, 0));
    check(c_vect_length(vect) == 2);
    check(c_vect_capacity(vect) == new_sz);

    check(!c_vect_resize(vect, -200));
    check(c_vect_length(vect) == 2);
    check(c_vect_capacity(vect) == new_sz);

    n = 7;
    result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 3);
    check(c_vect_capacity(vect) == new_sz);

    new_sz = 3;
    check(c_vect_resize(vect, new_sz));
    check(c_vect_length(vect) == 3);
    check(c_vect_capacity(vect) == new_sz);
    val = c_vect_get(vect, 2);
    check(val);
    check(*val == n);

    new_sz = 2;
    check(c_vect_resize(vect, new_sz));
    check(c_vect_length(vect) == new_sz);
    check(c_vect_capacity(vect) == new_sz);
    val = c_vect_get(vect, 1);
    check(val);
    check(*val == n-1);

    c_vect_destroy(vect);

    check(c_vect_make(vect, esz, NULL) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == C_VECT_DEFAULT_CAPACITY);
    check(c_vect_length(vect) == 0);

    new_sz = 5;
    check(c_vect_resize(vect, new_sz));
    check(c_vect_length(vect) == 0);
    check(c_vect_capacity(vect) == new_sz);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_pop(void)
{
    int esz = sizeof(double);
    int cap = 5;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == cap);
    check(c_vect_length(vect) == 0);

    check(!c_vect_pop(vect));
    check(c_vect_length(vect) == 0);

    double n = 5;
    void * result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == cap);

    n = 6;
    result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 2);
    check(c_vect_capacity(vect) == cap);

    check(c_vect_pop(vect) == vect);
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == cap);

    check(c_vect_pop(vect) == vect);
    check(c_vect_length(vect) == 0);
    check(c_vect_capacity(vect) == cap);

    check(!c_vect_pop(vect));
    check(c_vect_length(vect) == 0);

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_peek(void)
{
    int esz = sizeof(double);
    int cap = 5;
    c_vector vect_, * vect = &vect_;

    check(c_vect_make_cap(vect, esz, NULL, cap) == vect);
    check(c_vect_elem_size(vect)== esz);
    check(c_vect_capacity(vect) == cap);
    check(c_vect_length(vect) == 0);

    void * back = c_vect_peek(vect);
    check(!back);

    double n = 5;
    void * result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 1);
    check(c_vect_capacity(vect) == cap);
    back = c_vect_peek(vect);
    check(back == result);
    check(*((double *)back) == n);


    n = 6;
    result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 2);
    back = c_vect_peek(vect);
    check(back == result);
    check(*((double *)back) == n);

    n = 7;
    result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 3);
    back = c_vect_peek(vect);
    check(back == result);
    check(*((double *)back) == n);

    n = 8;
    result = c_vect_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(c_vect_length(vect) == 4);
    back = c_vect_peek(vect);
    check(back == result);
    check(*((double *)back) == n);

    c_vect_pop(vect);
    back = c_vect_peek(vect);
    check(c_vect_length(vect) == 3);
    check(*((double *)back) == n-1);


    c_vect_pop(vect);
    back = c_vect_peek(vect);
    check(c_vect_length(vect) == 2);
    check(*((double *)back) == n-2);
    check(c_vect_capacity(vect) == 5);

    c_vect_resize(vect, 1);
    back = c_vect_peek(vect);
    check(c_vect_length(vect) == 1);
    check(*((double *)back) == n-3);
    check(c_vect_capacity(vect) == 1);

    while (c_vect_pop(vect))
        continue;

    check(!c_vect_peek(vect));

    c_vect_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_c_vect(void)
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

int tc_vect_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tc_vect_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
