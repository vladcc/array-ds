/*  f_array.h -- a dynamic array
    v1.122

    A dynamic array implementation, much like the C++ arrayor.
    It copies whatever you provide it with inside itself. As such it can contain
    pointers to data allocated somewhere else, as well as actual copies of the
    data. Memory operations are implemented with memcpy() and memmove()

    The structure keeps track of the number of elements it currently has, along
    with the maximum number of elements it can have before it has to grow,
    which is its capacity. When any random access operation is performed, the
    array checks if the requested index is in the range of 0 to the number of
    elements minus 1, the index of its current last element, including. When
    the capacity is reached, the array grows at the very next insert or push
    operation. The new size is determined by the result of the multiplication
    of its capacity by C_VECT_GROWTH_RATE. f_array does not shrink
    automatically.

    f_array provides ways to insert, remove, copy, apply custom functions to
    its elements, a stack interface, sort, linear and binary search, conditional
    search and remove, constant time swap-pop removal, and online insertion.

    Author: Vladimir Dinev
    vld.dinev@gmail.com
    2019-03-02
*/

#ifndef C_VECTOR_H
#define C_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#define F_ARR_MAX_STACK_BUFF   64
/*
When a f_array needs a buffer, it will allocate either element size or
F_ARR_MAX_STACK_BUFF number of bytes on the stack, whichever is smaller. If the
element size is greater than F_ARR_MAX_STACK_BUFF, the arrayor will malloc() an
additional buffer big enough for a single element, use it, and free() it every
time. As of v1.122, f_array uses an additional buffer only for f_arr_swap()
*/

// a three way comparison function like compar for qsort()
typedef int (*fcomp)(const void * one, const void * two);

// Do not use directly
typedef struct f_array {
    void * arr;
    fcomp compar;
    int capacity;
    int elem_count;
    int elem_size;
} f_array;

void * f_arr_make(
    f_array * far, int elem_size, fcomp compar, int capacity
    );
/*
Returns: far on success, NULL otherwise.

Description: Creates a arrayor with 0 elements, allocates elem_size * capacity
bytes of memory. elem_size and capacity have to be positive numbers. On failure
*cv is zeroed out.

Complexity: O(1)
*/

void f_arr_destroy(f_array * far);
#define f_arr_destroy_null(far)\
f_arr_destroy((far)), (far) = NULL
/*
Returns: Nothing.

Description: Frees the allocated memory, zeros *cv out.

Complexity: O(1)
*/

f_array f_arr_copy(const f_array * src, bool * out_success);
/*
Returns: A deep copy of the f_array pointed to by src. If the function fails,
the variable pointed to by success is set to false and all members of the
returned arrayor are set to zero.

Description: Makes a deep copy of src. Only the valid elements from src arrayor
are copied. In result, the capacity of the new arrayor equals its element count.
success can be NULL if not used.

Complexity: O(n)
*/

void * f_arr_push(f_array * far, const void * what);
/*
Returns: A pointer to the appended element in the array on success,
NULL otherwise.

Description: Appends the data pointed to by what to the end of the array. If the
array is full at the time of the operation, it gets reallocated and grows
C_VECT_GROWTH_RATE times its current capacity before appending.

Complexity: O(1) amortized.
*/

void * f_arr_pop(f_array * far);
/*
Returns: far on success, NULL if far is empty.

Description: Removes the last element from the array by decrementing the element
counter by 1.

Complexity: O(1)
*/

void * f_arr_peek(f_array * far);
/*
Returns: A pointer to the last element of the array, NULL if the array is empty.

Description: Peeks at the last array element.

Complexity: O(1)
*/

void * f_arr_peek_pop(f_array * far);
/*
Returns: A pointer to the popped element from the arrayor, NULL if the arrayor is
empty.

Description: Combines peek and pop in one function call and a single range
check.

Complexity: O(1)
*/

void * f_arr_get(f_array * far, int index);
/*
Returns: A pointer to the element at index, NULL if index is out of range, or
the array is empty.

Description: Indexes into the array.

Complexity: O(1)
*/

void * f_arr_insert_at(f_array * far, int index, const void * key);
/*
Returns: A pointer to the inserted element inside the array on success,
NULL if index is out of range.

Description: Insert the element pointed to by key at index after first moving
all elements from index one to the right.

Complexity: O(n)
*/

void * f_arr_insert_online_ind(
    f_array * far, const void * key, int * out_index
);
#define f_arr_insert_online(far, key)\
f_arr_insert_online_ind((far), (key), NULL)
/*
Returns: A pointer to the inserted element in cv. If out_index is not NULL,
the variable pointed to by out_index contains the index of the inserted element.

Description: Inserts the element pointed to by key in its proper place in the
ordered array cv, therefore keeping far sorted. Implemented with binary insertion
sort. Calling this function on an unsorted array results in undefined behavior.

Complexity: O(log n) for finding the insertion point, O(n) for the insertion.
*/

void * f_arr_write_at(f_array * far, int index, const void * key);
/*
Returns: A pointer to the element written inside the array on success,
NULL if index is out of range.

Description: Overwrites the element at index with the element pointed to by key.

Complexity: O(1)
*/

void * f_arr_remove_at(f_array * far, int index);
/*
Returns: far on success, NULL if index is out of range.

Description: Removes the element at index by moving all memory on the right of
that index one to the left.

Complexity: O(n)
*/

int f_arr_remove_by_val(f_array * far, const void * key);
/*
Returns: The number of elements removed.

Description: Goes through the arrayor and removes all elements with the same
value as key, that is, all elements for which compar() returns 0 when compared
to key.

Complexity: O(n)
*/

int f_arr_remove_if(f_array * far, fcomp condition, const void * key);
/*
Returns: The number of elements removed.

Description: Switches the compar function inside far with condition, calls
f_arr_remove_by_val(), switches back to the original compar.

Complexity: O(n)
*/

void * f_arr_swap(f_array * far, int index_one, int index_two);
/*
Returns: far on success, NULL otherwise.

Description: Swaps the elements at index_one and index_two. If any of the two
indices is out of range, the function fails.

Complexity: O(1)
*/

void * f_arr_swap_pop_at(f_array * far, int index);
/*
Returns: far on success, NULL otherwise.

Description: Swaps the element at index with the last element and pops it off,
effectively removing it from the arrayor. Does not preserve the order of the
elements.

Complexity: O(1)
*/

int f_arr_swap_pop_by_val(f_array * far, const void * key);
/*
Returns: The number of elements removed.

Description: Swap-pops all elements with the same value as key, that is, all
elements for which compar() returns 0 when compared to key.

Complexity: O(n)
*/

int f_arr_swap_pop_if(f_array * far, fcomp condition, const void * key);
/*
Returns: The number of elements removed.

Description: Switches the compar function inside far with condition, calls
f_arr_swap_pop_by_val(), switches back to the original compar.

Complexity: O(n)
*/

void * f_arr_reverse(f_array * far);
/*
Returns: cv

Description: Reverses the order of elements in the arrayor by swap-popping.

Complexity: O(n/2)
*/

void * f_arr_set_to_val(f_array * far, const void * key);
/*
Returns: cv

Description: Sets all elements of the arrayor to the value of key.

Complexity: O(n)
*/

void * f_arr_remove_range(f_array * far, int index_first, int index_last);
/*
Returns: far on success, NULL otherwise.

Description: Removes all elements between index_first and index_last, both
including. The function fails if any of the two indices is out of range, or
if index_first is greater than index_last.

Complexity: O(n)
*/

void * f_arr_find_ind_from(
    f_array * far, const void * key, int * out_index, int from_index
    );
#define f_arr_find_ind(far, key, out_index)\
f_arr_find_ind_from((far), (key), (out_index), 0)

#define f_arr_find_from(far, key, from_index)\
f_arr_find_ind_from((far), (key), NULL, (from_index))

#define f_arr_find(far, key)\
f_arr_find_ind_from((far), (key), NULL, 0)
/*
Returns: A pointer to the element inside the array if found, NULL if the
element is not found, or from_index is out of range. If out_index is not NULL
and NULL is returned, the value of the variable pointed to by out_index is -1.
If out_index is not NULL and key is found, it's the index of the element found
inside the array.

Description: Performs a linear search for key. If out_index is not used, it can
be set to NULL. out_index must point to a variable different than from_index.

Complexity: O(n)
*/

void * f_arr_find_if_ind_from(
    f_array * far, fcomp condition, const void * key, int * out_index,
    int from_index
    );
#define f_arr_find_if_ind(far, condition, key, out_index)\
f_arr_find_if_ind_from((far), (condition), (key), (out_index), 0)

#define f_arr_find_if_from(far, condition, key, from_index)\
f_arr_find_if_ind_from((far), (condition), (key), NULL, (from_index))

#define f_arr_find_if(far, condition, key)\
f_arr_find_if_ind_from((far), (condition), (key), NULL, 0)
/*
Returns: A pointer to the first element inside the arrayor starting from
from_index for which condition returned 0 after comparing it to key.

Description: Switches the compar function inside far with condition, calls
f_arr_find_ind_from(), switches back to the original compar.

Complexity: O(n)
*/

void * f_arr_bsearch_ind(f_array * far, const void * key, int * out_index);
#define f_arr_bsearch(far, key)\
f_arr_bsearch_ind((far), (key), NULL)
/*
Returns: A pointer to the element inside the array if found, NULL otherwise.
If out_index is not NULL and NULL is returned, the value of the variable
pointed to by out_index is -1. If out_index is not NULL and key is found, it's
the index of the element found inside the array.

Description: Performs binary search on a sorted array. If there are several
elements with the same value inside the array, the pointer returned may point
to any one of them. If out_index is not used, it can be set to NULL. Calling
this function on an unsorted array results in undefined behavior.

Complexity: O(log n)
*/

void * f_arr_sort(f_array * far);
/*
Returns: cv

Description: Sorts the array if it is not sorted.

Complexity: O(n) if it's already sorted, otherwise calls qsort(), which is
stated as approximately O(n log n).
*/

// a callback that takes no additional arguments
typedef void (*fapply)(void * elem);

// a callback that takes additional arguments
typedef void (*fapply_args)(void * elem, void * args);

void * f_arr_apply(f_array * far, fapply fun);
/*
Returns: cv

Description: Applies a function of type fapply to all elements of the array.

Complexity: O(n)
*/

void * f_arr_apply_args(f_array * far, fapply_args fun, void * args);
/*
Returns: cv

Description: Applies a function of type fapply_args to
all elements of the array.

Complexity: O(n)
*/

void * f_arr_zero_out(f_array * far);
/*
Returns: cv

Description: Fills with 0s the array memory of far up to its capacity.

Complexity: O(n) where n is the number of bytes of cv's capacity.
*/

void * f_arr_set_length(f_array * far, int len);
/*
Returns: far on success, NULL if len is out of range.

Description: Changes the number of elements in the array, effectively expanding
or shrinking the number of indices you have random access to. len is out of
range if it's less than 0, or larger than cv's capacity.

Complexity: O(1)
*/

void * f_arr_set_compar(f_array * far, fcomp compar);
/*
Returns: cv

Description: Changes the compar function used by cv.

Complexity: O(1)
*/

fcomp f_arr_compar(f_array * far);
/*
Returns: The address of the current compar function used by cv.

Description: Gets the compar function of cv.

Complexity: O(1)
*/

void * f_arr_reset(f_array * far);
/*
Returns: cv

Description: After calling this function, far would have exactly 0 elements.

Complexity: O(1)
*/

bool f_arr_is_empty(f_array * far);
/*
Returns: true if there are no elements in the array, false otherwise.

Description: Let's you know if far is empty.

Complexity: O(1)
*/

void * f_arr_is_sorted(f_array * far);
/*
Returns: far if the array is sorted, NULL otherwise.

Description: Checks if the array is sorted.

Complexity: O(n)
*/

void * f_arr_data(f_array * far);
/*
Returns: A pointer to the first element of cv.

Description: Gives you the start of the array.

Complexity: O(1)
*/

int f_arr_length(f_array * far);
/*
Returns: The number of elements in the array.

Description: Gets the size.

Complexity: O(1)
*/

int f_arr_elem_size(f_array * far);
/*
Returns: The size of an array element in byte.

Description: Gets the element size.

Complexity: O(1)
*/

int f_arr_capacity(f_array * far);
/*
Returns: The maximum number of elements the array can hold before having to
grow.

Description: Gets the capacity.

Complexity: O(1)
*/
#endif
