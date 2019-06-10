/*  c_vector.h -- a dynamic array
    v1.122

    A dynamic array implementation, much like the C++ vector.
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
    of its capacity by C_VECT_GROWTH_RATE. c_vector does not shrink
    automatically.

    c_vector provides ways to insert, remove, copy, apply custom functions to
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

#define C_VECT_DEFAULT_CAPACITY 16
#define C_VECT_GROWTH_RATE      2

#define C_VECT_MAX_STACK_BUFF   64
/*
When a c_vector needs a buffer, it will allocate either element size or
C_VECT_MAX_STACK_BUFF number of bytes on the stack, whichever is smaller. If the
element size is greater than C_VECT_MAX_STACK_BUFF, the vector will malloc() an
additional buffer big enough for a single element, use it, and free() it every
time. As of v1.122, c_vector uses an additional buffer only for c_vect_swap()
*/

// a three way comparison function like compar for qsort()
typedef int (*fcomp)(const void * one, const void * two);

// Do not use directly
typedef struct c_vector {
    void * arr;
    fcomp compar;
    int capacity;
    int elem_count;
    int elem_size;
} c_vector;

void * c_vect_make_cap(
    c_vector * cv, int elem_size, fcomp compar, int capacity
    );
#define c_vect_make(cv, elem_size, compar)\
c_vect_make_cap((cv), (elem_size), (compar), C_VECT_DEFAULT_CAPACITY)
/*
Returns: cv on success, NULL otherwise.

Description: Creates a vector with 0 elements, allocates elem_size * capacity
bytes of memory. elem_size and capacity have to be positive numbers. On failure
*cv is zeroed out.

Complexity: O(1)
*/

void c_vect_destroy(c_vector * cv);
#define c_vect_destroy_null(cv)\
c_vect_destroy((cv)), (cv) = NULL
/*
Returns: Nothing.

Description: Frees the allocated memory, zeros *cv out.

Complexity: O(1)
*/

c_vector c_vect_copy(const c_vector * src, bool * out_success);
/*
Returns: A deep copy of the c_vector pointed to by src. If the function fails,
the variable pointed to by success is set to false and all members of the
returned vector are set to zero.

Description: Makes a deep copy of src. Only the valid elements from src vector
are copied. In result, the capacity of the new vector equals its element count.
success can be NULL if not used.

Complexity: O(n)
*/

void * c_vect_resize(c_vector * cv, int new_capacity);
/*
Returns: cv on success, NULL otherwise.

Description: Resizes the array to new_capacity, reallocates if necessary.
new_capacity has to be a positive number. If new_capacity is less than the
current number of elements in the array, the number of elements in the array is
trimmed to the value of new_capacity.

Complexity: Depends on realloc(), might be O(n)
*/

void * c_vect_push(c_vector * cv, const void * what);
/*
Returns: A pointer to the appended element in the array on success,
NULL otherwise.

Description: Appends the data pointed to by what to the end of the array. If the
array is full at the time of the operation, it gets reallocated and grows
C_VECT_GROWTH_RATE times its current capacity before appending.

Complexity: O(1) amortized.
*/

void * c_vect_pop(c_vector * cv);
/*
Returns: cv on success, NULL if cv is empty.

Description: Removes the last element from the array by decrementing the element
counter by 1.

Complexity: O(1)
*/

void * c_vect_peek(c_vector * cv);
/*
Returns: A pointer to the last element of the array, NULL if the array is empty.

Description: Peeks at the last array element.

Complexity: O(1)
*/

void * c_vect_peek_pop(c_vector * cv);
/*
Returns: A pointer to the popped element from the vector, NULL if the vector is
empty.

Description: Combines peek and pop in one function call with a single range
check.

Complexity: O(1)
*/

void * c_vect_get(c_vector * cv, int index);
/*
Returns: A pointer to the element at index, NULL if index is out of range, or
the array is empty.

Description: Indexes into the array.

Complexity: O(1)
*/

void * c_vect_insert_at(c_vector * cv, int index, const void * key);
/*
Returns: A pointer to the inserted element inside the array on success,
NULL if index is out of range.

Description: Insert the element pointed to by key at index after first moving
all elements from index one to the right.

Complexity: O(n)
*/

void * c_vect_insert_online_ind(
    c_vector * cv, const void * key, int * out_index
);
#define c_vect_insert_online(cv, key)\
c_vect_insert_online_ind((cv), (key), NULL)
/*
Returns: A pointer to the inserted element in cv. If out_index is not NULL,
the variable pointed to by out_index contains the index of the inserted element.

Description: Inserts the element pointed to by key in its proper place in the
ordered array cv, therefore keeping cv sorted. Implemented with binary insertion
sort. Calling this function on an unsorted array results in undefined behavior.

Complexity: O(log n) for finding the insertion point, O(n) for the insertion.
*/

void * c_vect_write_at(c_vector * cv, int index, const void * key);
/*
Returns: A pointer to the element written inside the array on success,
NULL if index is out of range.

Description: Overwrites the element at index with the element pointed to by key.

Complexity: O(1)
*/

void * c_vect_remove_at(c_vector * cv, int index);
/*
Returns: cv on success, NULL if index is out of range.

Description: Removes the element at index by moving all memory on the right of
that index one to the left.

Complexity: O(n)
*/

int c_vect_remove_by_val(c_vector * cv, const void * key);
/*
Returns: The number of elements removed.

Description: Goes through the vector and removes all elements with the same
value as key, that is, all elements for which compar() returns 0 when compared
to key.

Complexity: O(n)
*/

int c_vect_remove_if(c_vector * cv, fcomp condition, const void * key);
/*
Returns: The number of elements removed.

Description: Switches the compar function inside cv with condition, calls
c_vect_remove_by_val(), switches back to the original compar.

Complexity: O(n)
*/

void * c_vect_swap(c_vector * cv, int index_one, int index_two);
/*
Returns: cv on success, NULL otherwise.

Description: Swaps the elements at index_one and index_two. If any of the two
indices is out of range, the function fails.

Complexity: O(1)
*/

void * c_vect_swap_pop_at(c_vector * cv, int index);
/*
Returns: cv on success, NULL otherwise.

Description: Swaps the element at index with the last element and pops it off,
effectively removing it from the vector. Does not preserve the order of the
elements.

Complexity: O(1)
*/

int c_vect_swap_pop_by_val(c_vector * cv, const void * key);
/*
Returns: The number of elements removed.

Description: Swap-pops all elements with the same value as key, that is, all
elements for which compar() returns 0 when compared to key.

Complexity: O(n)
*/

int c_vect_swap_pop_if(c_vector * cv, fcomp condition, const void * key);
/*
Returns: The number of elements removed.

Description: Switches the compar function inside cv with condition, calls
c_vect_swap_pop_by_val(), switches back to the original compar.

Complexity: O(n)
*/

void * c_vect_reverse(c_vector * cv);
/*
Returns: cv

Description: Reverses the order of elements in the vector by swap-popping.

Complexity: O(n/2)
*/

void * c_vect_set_to_val(c_vector * cv, const void * key);
/*
Returns: cv

Description: Sets all elements of the vector to the value of key.

Complexity: O(n)
*/

void * c_vect_remove_range(c_vector * cv, int index_first, int index_last);
/*
Returns: cv on success, NULL otherwise.

Description: Removes all elements between index_first and index_last, both
including. The function fails if any of the two indices is out of range, or
if index_first is greater than index_last.

Complexity: O(n)
*/

void * c_vect_find_ind_from(
    c_vector * cv, const void * key, int * out_index, int from_index
    );
#define c_vect_find_ind(cv, key, out_index)\
c_vect_find_ind_from((cv), (key), (out_index), 0)

#define c_vect_find_from(cv, key, from_index)\
c_vect_find_ind_from((cv), (key), NULL, (from_index))

#define c_vect_find(cv, key)\
c_vect_find_ind_from((cv), (key), NULL, 0)
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

void * c_vect_find_if_ind_from(
    c_vector * cv, fcomp condition, const void * key, int * out_index,
    int from_index
    );
#define c_vect_find_if_ind(cv, condition, key, out_index)\
c_vect_find_if_ind_from((cv), (condition), (key), (out_index), 0)

#define c_vect_find_if_from(cv, condition, key, from_index)\
c_vect_find_if_ind_from((cv), (condition), (key), NULL, (from_index))

#define c_vect_find_if(cv, condition, key)\
c_vect_find_if_ind_from((cv), (condition), (key), NULL, 0)
/*
Returns: A pointer to the first element inside the vector starting from
from_index for which condition returned 0 after comparing it to key.

Description: Switches the compar function inside cv with condition, calls
c_vect_find_ind_from(), switches back to the original compar.

Complexity: O(n)
*/

void * c_vect_bsearch_ind(c_vector * cv, const void * key, int * out_index);
#define c_vect_bsearch(cv, key)\
c_vect_bsearch_ind((cv), (key), NULL)
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

void * c_vect_sort(c_vector * cv);
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

void * c_vect_apply(c_vector * cv, fapply fun);
/*
Returns: cv

Description: Applies a function of type fapply to all elements of the array.

Complexity: O(n)
*/

void * c_vect_apply_args(c_vector * cv, fapply_args fun, void * args);
/*
Returns: cv

Description: Applies a function of type fapply_args to
all elements of the array.

Complexity: O(n)
*/

void * c_vect_append_array(c_vector * cv, const void * arr, int len);
/*
Returns: cv on success, NULL if relocation failed.

Description: Grows cv as needed and appends len elements of the array pointed to
by arr.

Complexity: O(n) for potential relocation, O(n) for the append, where n is len.
*/

void * c_vect_trim(c_vector * cv);
/*
Returns: cv on success, NULL otherwise.

Description: Fits the array to match exactly the space needed for the number of
elements it currently holds. Relocates if necessary.

Complexity: Depends on realloc(), might be O(n)
*/

void * c_vect_zero_out(c_vector * cv);
/*
Returns: cv

Description: Fills with 0s the array memory of cv up to its capacity.

Complexity: O(n) where n is the number of bytes of cv's capacity.
*/

void * c_vect_set_length(c_vector * cv, int len);
/*
Returns: cv on success, NULL if len is out of range.

Description: Changes the number of elements in the array, effectively expanding
or shrinking the number of indices you have random access to. len is out of
range if it's less than 0, or larger than cv's capacity.

Complexity: O(1)
*/

void * c_vect_set_compar(c_vector * cv, fcomp compar);
/*
Returns: cv

Description: Changes the compar function used by cv.

Complexity: O(1)
*/

fcomp c_vect_compar(c_vector * cv);
/*
Returns: The address of the current compar function used by cv.

Description: Gets the compar function of cv.

Complexity: O(1)
*/

void * c_vect_reset(c_vector * cv);
/*
Returns: cv

Description: After calling this function, cv would have exactly 0 elements.

Complexity: O(1)
*/

bool c_vect_is_empty(c_vector * cv);
/*
Returns: true if there are no elements in the array, false otherwise.

Description: Let's you know if cv is empty.

Complexity: O(1)
*/

void * c_vect_is_sorted(c_vector * cv);
/*
Returns: cv if the array is sorted, NULL otherwise.

Description: Checks if the array is sorted.

Complexity: O(n)
*/

void * c_vect_data(c_vector * cv);
/*
Returns: A pointer to the first element of cv.

Description: Gives you the start of the array.

Complexity: O(1)
*/

int c_vect_length(c_vector * cv);
/*
Returns: The number of elements in the array.

Description: Gets the size.

Complexity: O(1)
*/

int c_vect_elem_size(c_vector * cv);
/*
Returns: The size of an array element in byte.

Description: Gets the element size.

Complexity: O(1)
*/

int c_vect_capacity(c_vector * cv);
/*
Returns: The maximum number of elements the array can hold before having to
grow.

Description: Gets the capacity.

Complexity: O(1)
*/
#endif
