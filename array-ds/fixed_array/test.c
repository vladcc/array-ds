#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "f_array.h"
#include "test.h"

static bool test_make_cap_vect(void);
static bool test_destroy_vect(void);
static bool test_push(void);
static bool test_get(void);
static bool test_pop(void);
static bool test_peek(void);
static bool test_find_rem_by_val(void);
static bool test_is_sorted(void);
static bool test_sort(void);
static bool test_bsearch(void);
static bool test_insert_online(void);
static bool test_reset_empty(void);
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
    test_pop,
    test_peek,
    test_find_rem_by_val,
    test_is_sorted,
    test_sort,
    test_bsearch,
    test_insert_online,
    test_reset_empty,
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

#define CAP 16

static int compar(const void * k1, const void * k2)
{
    int i1 = *(int *)k1;
    int i2 = *(int *)k2;

    int result = 0;
    if (i1 > i2)
        result = 1;
    else if (i1 < i2)
        result = -1;

    return result;
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
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    int arr[] = {1, 5, 5, 3, 5, 4, 6};
    int sz = sizeof(arr)/sizeof(*arr);

    int * elm;
    for (int i = 0; i < sz; ++i)
        check(*(elm = f_arr_push(vect, arr+i)) == arr[i]);

    int what = 510;
    check(!f_arr_swap_pop_if(vect, equal, &what));
    what = 5;
    check(f_arr_swap_pop_if(vect, less, &what) == 3);
    check(f_arr_length(vect) == 4);
    check(f_arr_compar(vect) == compar);
    check(f_arr_swap_pop_if(vect, equal, &what) == 3);
    check(f_arr_compar(vect) == compar);
    check(f_arr_length(vect) == 1);
    check(f_arr_swap_pop_if(vect, greater, &what) == 1);
    check(f_arr_is_empty(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_remove_if(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    int arr[] = {1, 5, 5, 3, 5, 4, 6};
    int sz = sizeof(arr)/sizeof(*arr);

    int * elm;
    for (int i = 0; i < sz; ++i)
        check(*(elm = f_arr_push(vect, arr+i)) == arr[i]);

    int what = 5;
    check(f_arr_compar(vect) == compar);
    check(f_arr_remove_if(vect, less, &what) == 3);
    what = 510;
    check(!f_arr_remove_if(vect, greater, &what));
    what = 5;
    check(f_arr_compar(vect) == compar);
    check(f_arr_length(vect) == 4);
    check(f_arr_is_sorted(vect));
    check(f_arr_remove_if(vect, equal, &what) == 3);
    check(f_arr_length(vect) == 1);
    check(f_arr_remove_if(vect, greater, &what) == 1);
    check(f_arr_is_empty(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_find_if(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    check(f_arr_compar(vect) == compar);
    check(f_arr_set_compar(vect, NULL) == vect);
    check(f_arr_compar(vect) == NULL);
    check(f_arr_set_compar(vect, compar) == vect);
    check(f_arr_compar(vect) == compar);

    int end = 10;
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);

    int * elm;
    check(*(elm = f_arr_get(vect, 0)) == 0);
    check(*(elm = f_arr_get(vect, 3)) == 3);
    check(*(elm = f_arr_get(vect, 5)) == 5);
    check(*(elm = f_arr_get(vect, 9)) == 9);

    int from = 0, ifound = 0, what = -5;
    void * pfound = f_arr_find_if_ind_from(vect, equal, &what, &ifound, from);
    check(!pfound);
    check(ifound == -1);

    what = 2000;
    check(f_arr_compar(vect) == compar);
    pfound = f_arr_find_if_ind_from(vect, equal, &what, &ifound, from);
    check(!pfound);
    check(ifound == -1);
    check(f_arr_compar(vect) == compar);

    what = 5;
    check(pfound = f_arr_find_if_ind_from(vect, equal, &what, &ifound, from));
    check(*(elm = pfound) == 5);
    check(ifound == 5);

    check(f_arr_is_sorted(vect));

    what = 9;
    check(pfound = f_arr_find_if_ind(vect, equal, &what, &ifound));
    check(*(elm = pfound) == 9);
    check(ifound == 9);

    what = 7;
    check(pfound = f_arr_find_if_from(vect, equal, &what, 0));
    check(*(elm = pfound) == 7);
    check(ifound == 9);

    what = 0;
    check(pfound =
        f_arr_find_if_ind_from(vect, not_equal, &what, &ifound, from));
    check(*(elm = pfound) == 1);
    check(ifound == 1);

    what = 4;
    check(pfound =
        f_arr_find_if_ind_from(vect, not_equal, &what, &ifound, from));
    check(*(elm = pfound) == 0);
    check(ifound == 0);

    check(f_arr_reset(vect) == vect);
    int same[] = {5, 5, 5, 5, 5};
    for (int i = 0; i < 5; ++i)
        f_arr_push(vect, same+i);

    what = 5;
    pfound = f_arr_find_if_ind_from(vect, not_equal, &what, &ifound, from);
    check(!pfound);
    check(ifound == -1);

    pfound = f_arr_find_if_ind_from(vect, equal, &what, &ifound, from);
    check(*(elm = pfound) == 5);
    check(ifound == 0);

    what = 1;
    pfound = f_arr_find_if_ind_from(vect, not_equal, &what, &ifound, from);
    check(*(elm = pfound) == 5);
    check(ifound == 0);

    check(f_arr_reset(vect) == vect);
    int arr[] = {1, 5, 5, 3, 5, 4, 6};
    int sz = sizeof(arr)/sizeof(*arr);

    for (int i = 0; i < sz; ++i)
        check(*(elm = f_arr_push(vect, arr+i)) == arr[i]);

    what = 5;
    pfound = f_arr_find_if_ind_from(vect, less, &what, &ifound, from);
    check(*(elm = pfound) == 1);
    check(ifound == 0);

    from = ifound+1;
    pfound = f_arr_find_if_ind_from(vect, less, &what, &ifound, from);
    check(*(elm = pfound) == 3);
    check(ifound == 3);

    from = ifound+1;
    pfound = f_arr_find_if_ind_from(vect, less, &what, &ifound, from);
    check(*(elm = pfound) == 4);
    check(ifound == 5);

    from = ifound+1;
    pfound = f_arr_find_if_ind_from(vect, less, &what, &ifound, from);
    check(!pfound);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_remove_range(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;

    int arr[] = {1111, 22222, 33333, 44444, 55555, 66666, 77777};
    int arrsz = sizeof(arr)/sizeof(*arr);
    check(f_arr_make(vect, esz, compar, arrsz) == vect);

    check(f_arr_reset(vect) == vect);
    check(f_arr_is_empty(vect));

    int * elm;
    for (int i = 0; i < arrsz; ++i)
        check(*(elm = f_arr_push(vect, arr+i)) == arr[i]);
    check(f_arr_length(vect) == arrsz);

    check(!f_arr_remove_range(vect, -1, 3));
    check(!f_arr_remove_range(vect, 3, 2));
    check(!f_arr_remove_range(vect, -1, -5));
    check(!f_arr_remove_range(vect, 0, 100));
    check(!f_arr_remove_range(vect, 30, 100));

    check(f_arr_remove_range(vect, 1, 5) == vect);
    check(f_arr_length(vect) == 2);
    check(*(elm = f_arr_get(vect, 0)) == 1111);
    check(*(elm = f_arr_get(vect, 1)) == 77777);

    check(f_arr_remove_range(vect, 1, 1) == vect);
    check(f_arr_length(vect) == 1);
    check(*(elm = f_arr_get(vect, 0)) == 1111);

    check(vect == f_arr_set_length(vect, f_arr_length(vect)+1));
    check(f_arr_length(vect) == 2);
    check(*(elm = f_arr_get(vect, 1)) == 77777);
    check(vect == f_arr_set_length(vect, f_arr_length(vect)-1));

    check(f_arr_remove_range(vect, 0, 0) == vect);
    check(f_arr_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        check(*(elm = f_arr_push(vect, arr+i)) == arr[i]);
    check(f_arr_length(vect) == arrsz);
    check(f_arr_remove_range(vect, 0, 6) == vect);
    check(f_arr_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        check(*(elm = f_arr_push(vect, arr+i)) == arr[i]);
    check(f_arr_length(vect) == arrsz);

    check(f_arr_remove_range(vect, 0, 0) == vect);
    check(f_arr_length(vect) == arrsz-1);
    for (int i = 0; i < arrsz-1; ++i)
        check(*(elm = f_arr_get(vect, i)) == arr[i+1]);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_set_to_val(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    int arr[] = {1, 2, 3, 4, 5, 6, 7};

    int arrsz = sizeof(arr)/sizeof(*arr);
    check(f_arr_reset(vect) == vect);
    check(f_arr_is_empty(vect));

    int * elm;
    for (int i = 0; i < arrsz; ++i)
        check(*(elm = f_arr_push(vect, arr+i)) == arr[i]);
    check(f_arr_length(vect) == arrsz);

    int n = 5;
    check(f_arr_set_to_val(vect, &n) == vect);

    for (int i = 0; i < arrsz; ++i)
        check(*(elm = f_arr_get(vect, i)) == n);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_swap_pop_by_val(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    int n = 5;
    check(f_arr_swap_pop_by_val(vect, &n) == 0);

    int arr[] = {4, 4, 5, 4, 4};
    int arrsz = sizeof(arr)/sizeof(*arr);

    check(f_arr_is_empty(vect));
    for (int i = 0; i < arrsz; ++i)
        f_arr_push(vect, arr+i);

    check(f_arr_length(vect) == arrsz);
    check(f_arr_swap_pop_by_val(vect, &n) == 1);
    check(f_arr_length(vect) == arrsz-1);


    int * elm;
    for (int i = 0; i < arrsz-1; ++i)
        check(*(elm = f_arr_get(vect, i)) == 4);

    int arr2[] = {5, 4, 4, 5, 4, 4, 5, 5};
    arrsz = sizeof(arr2)/sizeof(*arr2);
    check(f_arr_reset(vect) == vect);
    check(f_arr_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        f_arr_push(vect, arr2+i);

    check(f_arr_length(vect) == arrsz);
    check(f_arr_swap_pop_by_val(vect, &n) == 4);
    check(f_arr_length(vect) == arrsz-4);

    for (int i = 0; i < arrsz-4; ++i)
        check(*(elm = f_arr_get(vect, i)) == 4);

    n = 4;
    check(f_arr_swap_pop_by_val(vect, &n) == 4);
    check(f_arr_is_empty(vect));

    int arr3[] = {4, 4, 4, 4, 4, 4, 4, 4};

    arrsz = sizeof(arr3)/sizeof(*arr3);
    check(f_arr_reset(vect) == vect);
    check(f_arr_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        f_arr_push(vect, arr3+i);

    check(f_arr_length(vect) == arrsz);
    check(f_arr_swap_pop_by_val(vect, &n) == arrsz);
    check(f_arr_is_empty(vect));

    f_arr_push(vect, &n);
    check(f_arr_length(vect) == 1);
    check(f_arr_swap_pop_by_val(vect, &n) == 1);
    check(f_arr_is_empty(vect));

    int arr4[] = {1, 2, 3, 4};

    arrsz = sizeof(arr4)/sizeof(*arr4);
    check(f_arr_reset(vect) == vect);
    check(f_arr_is_empty(vect));

    for (int i = 0; i < arrsz; ++i)
        f_arr_push(vect, arr4+i);
    check(f_arr_length(vect) == arrsz);

    n = 1;
    check(f_arr_swap_pop_by_val(vect, &n) == 1);
    check(f_arr_length(vect) == 3);
    check(*(elm = f_arr_get(vect, 0)) == 4);
    check(*(elm = f_arr_get(vect, 1)) == 2);
    check(*(elm = f_arr_get(vect, 2)) == 3);

    n = 2;
    check(f_arr_swap_pop_by_val(vect, &n) == 1);
    check(f_arr_length(vect) == 2);
    check(*(elm = f_arr_get(vect, 0)) == 4);
    check(*(elm = f_arr_get(vect, 1)) == 3);

    n = 3;
    check(f_arr_swap_pop_by_val(vect, &n) == 1);
    check(f_arr_length(vect) == 1);
    check(*(elm = f_arr_get(vect, 0)) == 4);

    n = 4;
    check(f_arr_swap_pop_by_val(vect, &n) == 1);
    check(f_arr_is_empty(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_peek_pop(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    check(!f_arr_swap_pop_at(vect, -1));
    check(!f_arr_swap_pop_at(vect, 1000));

    int end = 10;
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);

    check(f_arr_length(vect) == end);
    int * peek = f_arr_peek(vect);
    int * elm;
    check(*(elm = f_arr_peek_pop(vect)) == end-1);
    check(peek == elm);
    check(f_arr_length(vect) == end-1);
    check(*(elm = f_arr_peek_pop(vect)) == end-2);
    check(f_arr_length(vect) == end-2);

    while(f_arr_peek_pop(vect))
        continue;

    check(f_arr_is_empty(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_swap_pop_reverse(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    check(!f_arr_swap_pop_at(vect, -1));
    check(!f_arr_swap_pop_at(vect, 1000));

    int end = 10;
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);

    check(f_arr_length(vect) == end);
    check(f_arr_swap_pop_at(vect, 0) == vect);
    check(f_arr_length(vect) == end-1);
    int * elm;
    check(*(elm = f_arr_get(vect, 0)) == end-1);
    check(f_arr_swap_pop_at(vect, end-2) == vect);
    check(f_arr_length(vect) == end-2);
    check(*(elm = f_arr_get(vect, end-3)) == end-3);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, CAP) == vect);

    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);

    check(f_arr_length(vect) == end);
    check(f_arr_swap_pop_at(vect, 2) == vect);
    check(f_arr_length(vect) == end-1);
    check(*(elm = f_arr_get(vect, 2)) == end-1);

    while (f_arr_swap_pop_at(vect, 0))
        continue;
    check(f_arr_is_empty(vect));

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, CAP) == vect);
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);
    check(f_arr_reverse(vect) == vect);

    for(int i = 0; i < end; ++i)
        check(*(elm = f_arr_get(vect, i)) == end-1-i);

    f_arr_destroy(vect);

    --end;
    check(f_arr_make(vect, esz, compar, CAP) == vect);
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);
    check(f_arr_reverse(vect) == vect);

    for(int i = 0; i < end; ++i)
        check(*(elm = f_arr_get(vect, i)) == end-1-i);


    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_swap(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    int n = 1;
    f_arr_push(vect, &n);
    n = 2;
    f_arr_push(vect, &n);

    int * elm;
    check(*(elm = f_arr_get(vect, 0)) == 1);
    check(*(elm = f_arr_get(vect, 1)) == 2);
    check(f_arr_swap(vect, 0, 1) == vect);
    check(*(elm = f_arr_get(vect, 0)) == 2);
    check(*(elm = f_arr_get(vect, 1)) == 1);
    check(f_arr_swap(vect, 1, 0) == vect);
    check(*(elm = f_arr_get(vect, 0)) == 1);
    check(*(elm = f_arr_get(vect, 1)) == 2);

    f_arr_reset(vect);
    int end = 10;
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);

    for (int i = 0; i < end; ++i)
        check(*(elm = f_arr_get(vect, i)) == i);

    check(!f_arr_swap(vect, -1, 4));
    check(!f_arr_swap(vect, 200, 4));
    check(!f_arr_swap(vect, 2, -5));
    check(!f_arr_swap(vect, 2, 99919));
    check(!f_arr_swap(vect, 22, 33));
    check(!f_arr_swap(vect, -9341, -321));

    int end2 = end/2, last = f_arr_length(vect) - 1;
    for (int i = 0; i < end2; ++i)
        check(f_arr_swap(vect, i, last-i) == vect);

    for (int i = 0, j = end-1; i < end; ++i, --j)
        check(*(elm = f_arr_get(vect, i)) == j);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, CAP) == vect);

    end = 9;
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);

    for (int i = 0; i < end; ++i)
        check(*(elm = f_arr_get(vect, i)) == i);

    end2 = end/2, last = f_arr_length(vect) - 1;
    for (int i = 0; i < end2; ++i)
        check(f_arr_swap(vect, i, last-i) == vect);

    for (int i = 0, j = end-1; i < end; ++i, --j)
        check(*(elm = f_arr_get(vect, i)) == j);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_copy(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, CAP) == vect);

    bool success = true;
    f_array  vtry_;
    f_array  vno_, * vno = &vno_;

    memset(vno, 0, sizeof(*vno));
    vtry_ = f_arr_copy(vect, &success);
    check(memcmp(&vtry_, vno, sizeof(f_array )) == 0);
    check(!success);

    success = false;
    int end = f_arr_capacity(vect) - 5;
    for (int i = 0; i < end; ++i)
        f_arr_push(vect, &i);

    f_array  cpy_, * cpy = &cpy_;
    cpy->elem_count = -1;
    cpy->elem_size = -1;
    cpy->arr = NULL;
    cpy->capacity = -1;
    cpy->compar = NULL;

    check(f_arr_length(cpy) != f_arr_length(vect));
    check(f_arr_elem_size(cpy) != f_arr_elem_size(vect));
    check(f_arr_data(cpy) != f_arr_data(vect));
    check(f_arr_capacity(cpy) != f_arr_capacity(vect));
    check(cpy->compar != vect->compar);

    cpy_ = f_arr_copy(vect, &success);
    check(success);
    check(f_arr_length(cpy) == f_arr_length(vect));
    check(f_arr_elem_size(cpy) == f_arr_elem_size(vect));
    check(f_arr_data(cpy) != f_arr_data(vect));
    check(f_arr_capacity(cpy) == f_arr_length(vect));
    check(f_arr_compar(cpy) == f_arr_compar(vect));
    check(cpy->compar == vect->compar);

    end = f_arr_length(cpy);
    for (int i = 0; i < end; ++i)
        check(*((int *)f_arr_get(cpy, i)) == *((int *)f_arr_get(vect, i)));

    int old_cpy_cap = f_arr_capacity(cpy);

    check(f_arr_capacity(cpy) == old_cpy_cap);
    check(!f_arr_push(cpy, &old_cpy_cap));
    check(f_arr_capacity(cpy) == old_cpy_cap);

    f_arr_destroy(cpy);

    check(f_arr_length(cpy) != f_arr_length(vect));
    check(f_arr_elem_size(cpy) != f_arr_elem_size(vect));
    check(f_arr_data(cpy) != f_arr_data(vect));
    check(f_arr_capacity(cpy) != f_arr_capacity(vect));
    check(cpy->compar != vect->compar);

    success = false;
    cpy_ = f_arr_copy(vect, &success);
    check(success);
    check(f_arr_length(cpy) == f_arr_length(vect));
    check(f_arr_elem_size(cpy) == f_arr_elem_size(vect));
    check(f_arr_data(cpy) != f_arr_data(vect));
    check(f_arr_capacity(cpy) == f_arr_length(vect));
    check(f_arr_compar(cpy) == f_arr_compar(vect));
    check(cpy->compar == vect->compar);

    end = f_arr_length(cpy);
    for (int i = 0; i < end; ++i)
        check(*((int *)f_arr_get(cpy, i)) == *((int *)f_arr_get(vect, i)));

    f_arr_destroy(vect);

    for (int i = 0; i < end; ++i)
        check(*((int *)f_arr_get(cpy, i)) == i);

    f_array  cpy2 = f_arr_copy(cpy, NULL);
    f_arr_destroy(&cpy2);
    f_arr_destroy(cpy);
    return true;
}

//------------------------------------------------------------------------------

static bool test_set_length(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    int cap = 4;
    check(f_arr_make(vect, esz, NULL, cap) == vect);

    check(f_arr_length(vect) == 0);
    int len = 2;
    check(f_arr_set_length(vect, -200) == NULL);
    check(f_arr_set_length(vect, 100) == NULL);
    check(f_arr_set_length(vect, len) == vect);
    check(f_arr_length(vect) == len);
    check(f_arr_capacity(vect) == cap);
    check(f_arr_set_length(vect, -200) == NULL);
    check(f_arr_set_length(vect, 100) == NULL);
    check(f_arr_set_length(vect, cap) == vect);
    check(f_arr_length(vect) == cap);
    check(!f_arr_push(vect, &len));
    check(f_arr_length(vect) == cap);
    check(f_arr_pop(vect));
    check(f_arr_length(vect) == cap-1);
    check(f_arr_set_length(vect, -200) == NULL);
    check(f_arr_set_length(vect, 100) == NULL);
    check(f_arr_capacity(vect) == cap);

    check(f_arr_zero_out(vect) == vect);
    int all = f_arr_length(vect);
    for (int i = 0; i < all; ++i)
        check(*((int *)f_arr_get(vect, i)) == 0);

    byte * data = f_arr_data(vect);
    all = f_arr_capacity(vect) * f_arr_elem_size(vect);
    for (int i = 0; i < all; ++i)
        check(data[i] == 0);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, NULL, cap) == vect);
    check(f_arr_zero_out(vect) == vect);

    data = f_arr_data(vect);
    all = f_arr_capacity(vect) * f_arr_elem_size(vect);
    for (int i = 0; i < all; ++i)
        check(data[i] == 0);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_zero_out(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;

    check(f_arr_make(vect, esz, NULL, CAP) == vect);

    int i, all = 5;
    for (i = 0; i < all; ++i)
    {
        f_arr_push(vect, &i);
        check(*((int *)f_arr_get(vect, i)) == i);
    }

    check(f_arr_length(vect) == all);
    check(f_arr_zero_out(vect) == vect);
    for (i = 0; i < all; ++i)
        check(*((int *)f_arr_get(vect, i)) == 0);

    f_arr_push(vect, &all);
    check(f_arr_length(vect) == all+1);
    check(*((int *)f_arr_get(vect, all)) == all);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, NULL, CAP) == vect);
    int cap = f_arr_capacity(vect);

    for (i = 0; i < cap; ++i)
    {
        f_arr_push(vect, &i);
        check(*((int *)f_arr_get(vect, i)) == i);
    }
    check(f_arr_capacity(vect) == cap);
    check(f_arr_length(vect) == cap);
    check(f_arr_pop(vect) == vect);
    check(f_arr_pop(vect) == vect);
    check(f_arr_pop(vect) == vect);
    check(f_arr_length(vect) <= cap);

    check(f_arr_zero_out(vect) == vect);
    byte * data = f_arr_data(vect);
    all = f_arr_capacity(vect) * f_arr_elem_size(vect);
    for (int i = 0; i < all; ++i)
        check(data[i] == 0);


    f_arr_destroy(vect);
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
    f_array  vect_, * vect = &vect_;
    int cap = CAP;
    check(f_arr_make(vect, esz, NULL, cap) == vect);

    int max = 10;
    for (int i = 0; i < max; ++i)
        f_arr_push(vect, &i);

    for (int i = 0; i < max; ++i)
        check(*((int *)f_arr_get(vect, i)) == i);

    int m = 5;
    check(f_arr_apply(vect, add5) == vect);
    for (int i = 0; i < max; ++i)
        check(*((int *)f_arr_get(vect, i)) == i+m);

    f_arr_destroy(vect);
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
    f_array  vect_, * vect = &vect_;
    int cap = CAP;
    check(f_arr_make(vect, esz, NULL, cap) == vect);

    int max = 10;
    for (int i = 0; i < max; ++i)
        f_arr_push(vect, &i);

    for (int i = 0; i < max; ++i)
        check(*((int *)f_arr_get(vect, i)) == i);

    int n = 1;
    add_args aa = {n};
    check(f_arr_apply_args(vect, addn, &aa) == vect);
    for (int i = 0; i < max; ++i)
        check(*((int *)f_arr_get(vect, i)) == i+n);

    int m = 5;
    aa.n = m;
    check(f_arr_apply_args(vect, addn, &aa) == vect);
    for (int i = 0; i < max; ++i)
        check(*((int *)f_arr_get(vect, i)) == i+n+m);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_remove_at_index(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    int cap = CAP;
    check(f_arr_make(vect, esz, NULL, cap) == vect);

    int max = 10;
    for (int i = 0; i < max; ++i)
        f_arr_push(vect, &i);

    check(f_arr_remove_at(vect, -123) == NULL);
    check(f_arr_remove_at(vect, 123) == NULL);

    int rind = 4;
    int * n = f_arr_get(vect, rind);
    check(*n == 4);
    check(f_arr_remove_at(vect, rind) == vect);
    check(*n == 5);


    rind = 0;
    n = f_arr_get(vect, rind);
    check(*n == 0);
    check(f_arr_remove_at(vect, rind) == vect);
    check(*n == 1);

    rind = f_arr_length(vect) - 1;
    n = f_arr_get(vect, rind);
    check(*n == 9);
    check(f_arr_remove_at(vect, rind) == vect);
    check(f_arr_length(vect) == 7);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_insert_at_index(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, NULL, CAP) == vect);

    check(f_arr_capacity(vect) == CAP);

    int i, offs = 5;
    for (i = 1; i < 5; ++i)
    {
        int j = i * 2 + offs;
        f_arr_push(vect, &j);
    }

    check(f_arr_capacity(vect) == CAP);
    int * first = f_arr_get(vect, 0);
    check(*first == 2+offs);

    int n = 0;
    check(f_arr_insert_at(vect, -123, &n) == NULL);
    check(f_arr_insert_at(vect, 123, &n) == NULL);

    check(f_arr_insert_at(vect, 0, &n) == f_arr_data(vect));
    first = f_arr_get(vect, 0);
    check(*first == n);
    check(f_arr_capacity(vect) == CAP);

    n = 12;
    int last = f_arr_length(vect) - 1, * pe;
    check(f_arr_insert_at(vect, last, &n) == (int *)vect->arr + last);
    check(((int *)vect->arr)[last] == n);
    check(*(pe = f_arr_get(vect, last)) == n);

    int mid = 2;
    n = 8;
    check(f_arr_insert_at(vect, mid, &n) == (int *)vect->arr + mid);
    check(((int *)vect->arr)[mid] == n);
    check(*(pe = f_arr_get(vect, mid)) == n);

    check(f_arr_set_compar(vect, compar) == vect);
    check(f_arr_is_sorted(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_write_at_index(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, NULL, CAP) == vect);

    int n = 6;
    check(f_arr_write_at(vect, -1, &n) == NULL);
    check(f_arr_write_at(vect, -123, &n) == NULL);
    check(f_arr_write_at(vect, 124, &n) == NULL);

    int cap = f_arr_capacity(vect);
    check(f_arr_write_at(vect, cap, &n) == NULL);

    check(vect->arr == f_arr_data(vect));
    check(f_arr_is_empty(vect));
    check(f_arr_write_at(vect, 0, &n) == NULL);

    int i;
    for (i = 0; i < 8; ++i)
        f_arr_push(vect, &i);

    check(f_arr_write_at(vect, -1, &n) == NULL);
    check(f_arr_write_at(vect, -123, &n) == NULL);
    check(f_arr_write_at(vect, 124, &n) == NULL);

    check(f_arr_write_at(vect, 0, &n) == f_arr_data(vect));
    check(((int *)vect->arr)[0] == n);
    int * pe;
    check(*(pe = f_arr_get(vect, 0)) == n);

    int len = f_arr_length(vect);
    check(f_arr_write_at(vect, len, &n) == NULL);
    --len;
    check(f_arr_write_at(vect, len, &n) == ((int *)vect->arr) + len);
    check(((int *)vect->arr)[len] == n);
    check(*(pe = f_arr_get(vect, len)) == n);

    int mid = 5;
    check(f_arr_write_at(vect, mid, &n) == ((int *)vect->arr) + mid);
    check(((int *)vect->arr)[mid] == n);
    check(*(pe = f_arr_get(vect, mid)) == n);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_set_compar(void)
{
    int esz = sizeof(int);
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, NULL, CAP) == vect);

    check(vect->compar == NULL);
    check(f_arr_set_compar(vect, compar) == vect);
    check(vect->compar == compar);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_reset_empty(void)
{
    int esz = sizeof(int);
    int cap = 1;
    f_array  vect_, * vect = &vect_;

    check(f_arr_make(vect, esz, compar, cap) == vect);
    check(f_arr_is_empty(vect));

    for (int i = 0; i < 10; ++i)
        f_arr_push(vect, &i);
    check(!f_arr_is_empty(vect));

    check(f_arr_reset(vect) == vect);
    check(f_arr_is_empty(vect));

    for (int i = 0; i < 10; ++i)
        f_arr_push(vect, &i);
    check(!f_arr_is_empty(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_insert_online(void)
{
    int esz = sizeof(int);

    int cap = CAP+6;
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, cap) == vect);

    int n = 1, * ins = NULL;
    check(*(ins = f_arr_insert_online(vect, &n)) == n);
    check(f_arr_is_sorted(vect) == vect);
    ++n;
    check(*(ins = f_arr_insert_online(vect, &n)) == n);
    check(f_arr_is_sorted(vect) == vect);

    int arr[] = {5, 4, 5, 3, 3, 5, 2, 1, 0, 1, 0, 0, 2, 1, 5, 5, -100};
    int arr_size = sizeof(arr)/sizeof(*arr);

    int i = 0, ind;
    for (; i < arr_size; ++i)
    {
        check(*(ins = f_arr_insert_online_ind(vect, arr + i, &ind)) == arr[i]);
        check(*(ins = f_arr_get(vect, ind)) == arr[i]);
        check(f_arr_is_sorted(vect) == vect);
    }
    check(2 + arr_size == f_arr_length(vect));

    n = 10;
    check(*(ins = f_arr_insert_online(vect, &n)) == n);
    check(f_arr_is_sorted(vect) == vect);

    check(*(ins = f_arr_insert_online_ind(vect, &n, &ind)) == n);
    check(*(ins = f_arr_get(vect, ind)) == n);
    check(f_arr_is_sorted(vect) == vect);

    check(*(ins = f_arr_insert_online(vect, &n)) == n);
    check(f_arr_is_sorted(vect) == vect);

    int * elem;
    check(*(elem = f_arr_get(vect, arr_size + 2)) == n);
    check(*(elem = f_arr_get(vect, arr_size + 1 + 2)) == n);
    check(*(elem = f_arr_get(vect, arr_size + 2 + 2)) == n);

    check(((int *)vect->arr)[arr_size + 2] == n);
    check(((int *)vect->arr)[arr_size + 1 + 2] == n);
    check(((int *)vect->arr)[arr_size + 2 + 2] == n);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, cap) == vect);
    int newarr[] = {7, 8, 0, 1, 3, 6, 0, 4, 5, 3};

    arr_size = sizeof(newarr)/sizeof(*newarr);
    for (int i = 0; i < arr_size; ++i)
    {
        int ind;
        if (!f_arr_bsearch_ind(vect, newarr + i, &ind))
            check(*(ins = f_arr_insert_online(vect, newarr + i)) == newarr[i]);
    }

    check(f_arr_is_sorted(vect));
    int checkarr[] = {0, 1, 3, 4, 5, 6, 7, 8};

    arr_size = sizeof(checkarr)/sizeof(*checkarr);
    for (int i = 0; i < arr_size; ++i)
        check(*(elem = f_arr_get(vect, i)) == checkarr[i]);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, cap) == vect);

    int newarr2[] = {5, 5, 5, 5};
    arr_size = sizeof(newarr2)/sizeof(*newarr2);

    check(f_arr_is_empty(vect));
    for (int i = 0; i < arr_size; ++i)
        f_arr_push(vect, newarr2+i);

    check(f_arr_is_sorted(vect));
    n = 5;
    check(*(ins = f_arr_insert_online(vect, &n)) == n);
    n = 1;
    check(*(ins = f_arr_insert_online(vect, &n)) == n);
    n = 10;
    check(*(ins = f_arr_insert_online(vect, &n)) == n);
    check(f_arr_is_sorted(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_bsearch(void)
{
    int esz = sizeof(int);
    int cap = CAP;
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, cap) == vect);

    int * key;
    check(!f_arr_bsearch_ind(vect, NULL, NULL));

    for (int i = 0; i < 10; ++i)
        f_arr_push(vect, &i);

    int n = 9;
    f_arr_push(vect, &n);
    n = 123;
    f_arr_push(vect, &n);
    check(f_arr_is_sorted(vect) == vect);

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
        void * res = f_arr_bsearch_ind(vect, key, NULL);
        check((bool)res == tbl[i].res);
        if (res)
            check(compar(res, key) == 0);

        int pos;
        res = f_arr_bsearch_ind(vect, key, &pos);
        check((bool)res == tbl[i].res);

        bool right_pos = (pos == tbl[i].pos);
        if (!right_pos)
            printf("n: %d, pos :%d\n", tbl[i].n, tbl[i].pos);

        check(right_pos);
    }

    void * first = f_arr_get(vect, 0);
    void * last = f_arr_peek(vect);
    int ind;
    void * ffirst = f_arr_bsearch_ind(vect, first, &ind);
    check(ffirst == first);
    check(0 == ind);
    check(compar(first, ffirst) == 0);

    void * flast = f_arr_bsearch_ind(vect, last, &ind);
    check(flast == last);
    check(f_arr_length(vect)-1 == ind);
    check(compar(flast, last) == 0);

    int less = -93471, more = 946242;
    int * out = f_arr_bsearch_ind(vect, &less, &ind);
    check(NULL == out);
    check(-1 == ind);

    out = f_arr_bsearch_ind(vect, &more, &ind);
    check(NULL == out);
    check(-1 == ind);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, cap) == vect);
    n = 1;
    int i;
    for (i = 0; i < 10; ++i)
        f_arr_push(vect, &n);

    out = f_arr_bsearch_ind(vect, &n, &ind);
    check(out == f_arr_bsearch(vect, &n));
    check(*out == n);
    check(ind == (i-1)/2);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_is_sorted(void)
{
    int esz = sizeof(int);
    int cap = CAP;
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, cap) == vect);

    check(f_arr_is_sorted(vect) == NULL);
    int n = 10;
    f_arr_push(vect, &n);
    check(f_arr_length(vect) == 1);
    check(f_arr_is_sorted(vect) == vect);

    n = 9;
    f_arr_push(vect, &n);
    check(f_arr_is_sorted(vect) == NULL);
    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, cap) == vect);

    for (int i = 0; i < 10; ++i)
        f_arr_push(vect, &i);
    check(f_arr_is_sorted(vect) == vect);

    n = 9;
    f_arr_push(vect, &n);
    check(f_arr_is_sorted(vect) == vect);

    n = 5;
    f_arr_push(vect, &n);
    check(f_arr_is_sorted(vect) == NULL);

    n = 19;
    f_arr_push(vect, &n);
    check(f_arr_is_sorted(vect) == NULL);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_sort(void)
{
    int esz = sizeof(int);
    int cap = CAP;
    f_array  vect_, * vect = &vect_;
    check(f_arr_make(vect, esz, compar, cap) == vect);

    for (int i = 10; i >= 0; --i)
        f_arr_push(vect, &i);
    check(f_arr_is_sorted(vect) == NULL);

    check(f_arr_sort(vect) == vect);
    check(f_arr_is_sorted(vect) == vect);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_find_rem_by_val(void)
{
    int esz = sizeof(int);
    int cap = CAP;
    f_array  vect_, * vect = &vect_;

    check(f_arr_make(vect, esz, compar, cap) == vect);
    check(f_arr_elem_size(vect)== esz);
    check(f_arr_capacity(vect) == cap);
    check(vect->compar == compar);

    int pos = -1;
    for (int i = 10, j = 0; i >= 0; --i, ++j)
    {

        int prev_elem_count = vect->elem_count;
        void * target = f_arr_push(vect, &i);
        void * dest = (byte *)vect->arr + prev_elem_count * vect->elem_size;
        check(target == dest);
        check(*((int*)target) == i);
        check(f_arr_find_ind(vect, &i, NULL) == dest);
        check(f_arr_find(vect, &i) == dest);
        check(f_arr_find_ind(vect, &i, &pos) == dest);
        check(pos == j);
    }

    int n = 10;
    check(f_arr_find_ind(vect, &n, NULL) == f_arr_data(vect));
    check(f_arr_find_ind(vect, &n, &pos));
    check(pos == 0);

    n = -123;
    check(f_arr_find_ind(vect, &n, NULL) == NULL);
    check(f_arr_find_ind(vect, &n, &pos) == NULL);
    check(pos == -1);

    n = 35290;
    check(f_arr_find_ind(vect, &n, NULL) == NULL);
    check(f_arr_find_ind(vect, &n, &pos) == NULL);
    check(pos == -1);

    f_arr_destroy(vect);

    check(f_arr_make(vect, esz, compar, cap) == vect);

    int arr[] = {1, 2, 1, 3, 4, 5, 5, 5, 8};
    int asz = sizeof(arr)/sizeof(*arr);

    int * elm;
    for (int i = 0; i < asz; ++i)
    {
        check(
            f_arr_push(vect, arr+i) == f_arr_get(vect, f_arr_length(vect)-1)
            );
        check(*(elm = f_arr_get(vect, i)) == arr[i]);
        check(f_arr_find_from(vect, arr+i, i) == f_arr_get(vect, i));
        check(*(elm = f_arr_find_from(vect, arr+i, i)) == arr[i]);
    }

    int oi = 100;
    check(!f_arr_find_ind_from(vect, arr, &oi, -1341));
    check(!f_arr_find_from(vect, arr, -1));
    check(!f_arr_find_ind_from(vect, arr, &oi, 32));
    check(!f_arr_find_from(vect, arr, 1000));
    check(oi == -1);

    check(f_arr_find_from(vect, arr, 0) == f_arr_get(vect, 0));
    check(oi == -1);
    check(f_arr_find_ind_from(vect, arr, &oi, 0) == f_arr_get(vect, 0));
    check(oi == 0);
    check(f_arr_find_ind_from(vect, arr, &oi, 1) == f_arr_get(vect, 2));
    check(oi == 2);
    check(f_arr_find_from(vect, arr, 1) == f_arr_get(vect, 2));

    int val = -123;
    check(!f_arr_remove_by_val(vect, &val));
    val = 1234;
    check(!f_arr_remove_by_val(vect, &val));

    val = 1;
    check(f_arr_remove_by_val(vect, &val) == 2);
    val = 5;
    check(f_arr_remove_by_val(vect, &val) == 3);
    val = 8;
    check(f_arr_remove_by_val(vect, &val) == 1);

    check(f_arr_length(vect) == 3);
    check(*(elm = f_arr_get(vect, 0)) == 2);
    check(*(elm = f_arr_get(vect, 1)) == 3);
    check(*(elm = f_arr_get(vect, 2)) == 4);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_make_cap_vect(void)
{
    f_array  vect_, * vect = &vect_;
    f_array  zout;
    memset(&zout, 0, sizeof(f_array ));

    memset(vect, 0xFF, sizeof(f_array ));
    memcmp(&zout, vect, sizeof(f_array ) != 0);
    check(!f_arr_make(vect, 5, NULL, 0));
    memcmp(&zout, vect, sizeof(f_array ) == 0);

    memset(vect, 0xFF, sizeof(f_array ));
    memcmp(&zout, vect, sizeof(f_array ) != 0);
    check(!f_arr_make(vect, -1, NULL, 5));
    memcmp(&zout, vect, sizeof(f_array ) == 0);

    memset(vect, 0xFF, sizeof(f_array ));
    memcmp(&zout, vect, sizeof(f_array ) != 0);
    check(!f_arr_make(vect, 5, NULL, -1));
    memcmp(&zout, vect, sizeof(f_array ) == 0);

    memset(vect, 0xFF, sizeof(f_array ));
    memcmp(&zout, vect, sizeof(f_array ) != 0);
    check(!f_arr_make(vect, -23, NULL, 5));
    memcmp(&zout, vect, sizeof(f_array ) == 0);

    memset(vect, 0xFF, sizeof(f_array ));
    memcmp(&zout, vect, sizeof(f_array ) != 0);
    check(!f_arr_make(vect, 0, NULL, 5));
    memcmp(&zout, vect, sizeof(f_array ) == 0);

    int elem_size = sizeof(double);
    const int capacity = 5;
    check(f_arr_make(vect, elem_size, compar, capacity) == vect);
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
    f_array  vect_, * vect = &vect_;
    f_array  zout;
    memset(&zout, 0, sizeof(f_array ));

    check(f_arr_make(vect, elem_size, NULL, capacity) == vect);
    check(vect->arr);
    check(f_arr_elem_size(vect) == elem_size);
    check(f_arr_capacity(vect) == capacity);
    check(f_arr_length(vect) == 0);

    memcmp(&zout, vect, sizeof(f_array ) != 0);
    f_arr_destroy(vect);
    memcmp(&zout, vect, sizeof(f_array ) == 0);

    check(vect->capacity == 0);
    check(vect->elem_count == 0);
    check(vect->elem_size == 0);
    check(vect->arr == 0);

    check(f_arr_make(vect, elem_size, NULL, capacity) == vect);
    check(vect->arr);
    check(f_arr_elem_size(vect) == elem_size);
    check(f_arr_capacity(vect) == capacity);
    check(f_arr_length(vect) == 0);
    f_arr_destroy_null(vect);
    check(!vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_push(void)
{
    int esz = sizeof(double);
    int cap = 3;
    f_array  vect_, * vect = &vect_;

    check(f_arr_make(vect, esz, NULL, cap) == vect);
    check(f_arr_elem_size(vect)== esz);
    check(f_arr_capacity(vect) == cap);
    check(f_arr_length(vect) == 0);

    double n = 5;
    void * dest = (byte *)vect->arr + vect->elem_count * vect->elem_size;
    check(f_arr_push(vect, &n) == dest);
    check(f_arr_length(vect) == 1);
    check(f_arr_capacity(vect) == cap);

    void * target = f_arr_push(vect, &n);
    dest = (byte *)vect->arr + (vect->elem_count-1) * vect->elem_size;
    check(target == dest);
    check(f_arr_length(vect) == 2);

    check(f_arr_push(vect, &n));
    check(!f_arr_push(vect, &n));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_get(void)
{
    int esz = sizeof(double);
    int cap = CAP;
    f_array  vect_, * vect = &vect_;

    check(f_arr_make(vect, esz, NULL, cap) == vect);
    check(f_arr_elem_size(vect)== esz);
    check(f_arr_capacity(vect) == cap);
    check(f_arr_length(vect) == 0);
    check(f_arr_get(vect, -1) == NULL);
    check(f_arr_get(vect, 0) == NULL);
    check(f_arr_get(vect, 1) == NULL);
    check(f_arr_get(vect, 5) == NULL);
    check(f_arr_get(vect, 3000) == NULL);

    double n = 5;
    void * result = f_arr_push(vect, &n);
    check(result);

    n = 6;
    result = f_arr_push(vect, &n);
    check(result);

    n = 7;
    result = f_arr_push(vect, &n);
    check(result);

    check(f_arr_length(vect) == 3);
    double * ip = f_arr_get(vect, -1);
    check(!ip);
    ip = f_arr_get(vect, 100);
    check(!ip);
    ip = f_arr_get(vect, 2);
    check(ip);
    check(*ip == 7);
    ip = f_arr_get(vect, 1);
    check(ip);
    check(*ip == 6);
    ip = f_arr_get(vect, 0);
    check(ip);
    check(*ip == 5);

    check(f_arr_data(vect) == ip);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_pop(void)
{
    int esz = sizeof(double);
    int cap = 5;
    f_array  vect_, * vect = &vect_;

    check(f_arr_make(vect, esz, NULL, cap) == vect);
    check(f_arr_elem_size(vect)== esz);
    check(f_arr_capacity(vect) == cap);
    check(f_arr_length(vect) == 0);

    check(!f_arr_pop(vect));
    check(f_arr_length(vect) == 0);

    double n = 5;
    void * result = f_arr_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(f_arr_length(vect) == 1);
    check(f_arr_capacity(vect) == cap);

    n = 6;
    result = f_arr_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(f_arr_length(vect) == 2);
    check(f_arr_capacity(vect) == cap);

    check(f_arr_pop(vect) == vect);
    check(f_arr_length(vect) == 1);
    check(f_arr_capacity(vect) == cap);

    check(f_arr_pop(vect) == vect);
    check(f_arr_length(vect) == 0);
    check(f_arr_capacity(vect) == cap);

    check(!f_arr_pop(vect));
    check(f_arr_length(vect) == 0);

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static bool test_peek(void)
{
    int esz = sizeof(double);
    int cap = 5;
    f_array  vect_, * vect = &vect_;

    check(f_arr_make(vect, esz, NULL, cap) == vect);
    check(f_arr_elem_size(vect)== esz);
    check(f_arr_capacity(vect) == cap);
    check(f_arr_length(vect) == 0);

    void * back = f_arr_peek(vect);
    check(!back);

    double n = 5;
    void * result = f_arr_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(f_arr_length(vect) == 1);
    check(f_arr_capacity(vect) == cap);
    back = f_arr_peek(vect);
    check(back == result);
    check(*((double *)back) == n);


    n = 6;
    result = f_arr_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(f_arr_length(vect) == 2);
    back = f_arr_peek(vect);
    check(back == result);
    check(*((double *)back) == n);

    n = 7;
    result = f_arr_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(f_arr_length(vect) == 3);
    back = f_arr_peek(vect);
    check(back == result);
    check(*((double *)back) == n);

    n = 8;
    result = f_arr_push(vect, &n);
    check(result);
    check(*((double *)result) == n);
    check(f_arr_length(vect) == 4);
    back = f_arr_peek(vect);
    check(back == result);
    check(*((double *)back) == n);

    f_arr_pop(vect);
    back = f_arr_peek(vect);
    check(f_arr_length(vect) == 3);
    check(*((double *)back) == n-1);


    f_arr_pop(vect);
    back = f_arr_peek(vect);
    check(f_arr_length(vect) == 2);
    check(*((double *)back) == n-2);
    check(f_arr_capacity(vect) == 5);

    while (f_arr_pop(vect))
        continue;

    check(!f_arr_peek(vect));

    f_arr_destroy(vect);
    return true;
}
//------------------------------------------------------------------------------

static int passed, failed;
void run_tests_f_array(void)
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

int tf_arr_passed(void)
{
    return passed;
}
//------------------------------------------------------------------------------

int tf_arr_failed(void)
{
    return failed;
}
//------------------------------------------------------------------------------
