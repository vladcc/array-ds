#include <string.h>
#include <stdlib.h>
#include "c_vector.h"

typedef unsigned char byte;

#define grow(cv) do {\
    if (cv->elem_count >= cv->capacity &&\
		!c_vect_resize(cv, cv->capacity*C_VECT_GROWTH_RATE))\
		return NULL;\
} while (0)
//------------------------------------------------------------------------------

void * c_vect_resize(c_vector * cv, int new_capacity)
{
    void * result = NULL;

    if (new_capacity > 0)
    {
        result = realloc(cv->arr, new_capacity*cv->elem_size);
        if (result)
        {
            cv->arr = result;
            cv->capacity = new_capacity;
            if (cv->elem_count > new_capacity)
                cv->elem_count = new_capacity;
            result = cv;
        }
    }

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_make_cap(c_vector * cv, int elem_size, fcomp compar, int capacity)
{
    void * vect = NULL;

    if (elem_size > 0 && capacity > 0)
    {
        cv->elem_size = elem_size;
        cv->capacity = capacity;
        cv->elem_count = 0;
        cv->arr = NULL;
        cv->compar = compar;

        if (!(vect = c_vect_resize(cv, capacity)))
            memset(cv, 0, sizeof(*cv));
    }

    return vect;
}
//------------------------------------------------------------------------------

void c_vect_destroy(c_vector * cv)
{
    free(cv->arr);
    memset(cv, 0, sizeof(*cv));
}
//------------------------------------------------------------------------------

c_vector c_vect_copy(const c_vector * src, bool * out_success)
{
    c_vector ret;
    memset(&ret, 0, sizeof(ret));

    bool ok = false;
    fcomp s_compar = src->compar;
    int s_elem_count = src->elem_count;
    int s_elem_size = src->elem_size;
    int copy_size = s_elem_count * s_elem_size;

    void * new_data = NULL;
    if (copy_size && (new_data = malloc(copy_size)))
    {
        ret.arr = NULL;
        ret.compar = s_compar;
        ret.elem_count = ret.capacity = s_elem_count;
        ret.elem_size = s_elem_size;

        memcpy(new_data, src->arr, copy_size);
        ret.arr = new_data;
        ok = true;
    }

    if (out_success)
        *out_success = ok;

    return ret;
}
//------------------------------------------------------------------------------

void * c_vect_push(c_vector * cv, const void * what)
{
    grow(cv);

    void * dest = (byte *)cv->arr + cv->elem_count * cv->elem_size;
    ++cv->elem_count;

    return memcpy(dest, what, cv->elem_size);
}
//------------------------------------------------------------------------------

void * c_vect_pop(c_vector * cv)
{
    void * ret = NULL;

    if (cv->elem_count > 0)
    {
        --cv->elem_count;
        ret = cv;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * c_vect_peek(c_vector * cv)
{
    void * result = NULL;
    int elem_count = cv->elem_count;

    if (elem_count > 0)
        result = (byte *)cv->arr + (elem_count-1) * cv->elem_size;

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_peek_pop(c_vector * cv)
{
    void * result = NULL;
    int elem_count = cv->elem_count;

    if (elem_count > 0)
    {
        result = (byte *)cv->arr + (elem_count-1) * cv->elem_size;
        --cv->elem_count;
    }

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_trim(c_vector * cv)
{
    if (cv->elem_count == cv->capacity)
        return cv;

    return c_vect_resize(cv, cv->elem_count);
}

//------------------------------------------------------------------------------

void * c_vect_find_ind_from(
    c_vector * cv, const void * key, int * out_index, int from_index
    )
{
    byte * cv_key = NULL, * data = (byte *)cv->arr;
    int elem_size = cv->elem_size, elem_count = cv->elem_count, out = -1;
    fcomp compar = cv->compar;

    void * found = NULL;
    if ((unsigned int)from_index < (unsigned int)elem_count)
    {
        for(int i = from_index; i < elem_count; ++i)
        {
            cv_key = data + i * elem_size;
            if (compar(cv_key, key) == 0)
            {
                found = cv_key;
                out = i;
                break;
            }
        }
    }

    if (out_index)
        *out_index = out;

    return found;
}
//------------------------------------------------------------------------------

void * c_vect_find_if_ind_from(
    c_vector * cv, fcomp condition, const void * key, int * out_index,
    int from_index
    )
{
    fcomp back = cv->compar;
    cv->compar = condition;
    void * found = c_vect_find_ind_from(cv, key, out_index, from_index);
    cv->compar = back;
    return found;
}
//------------------------------------------------------------------------------

void * c_vect_is_sorted(const c_vector * cv)
{
    byte * data = (byte *)cv->arr;
    int elem_size = cv->elem_size, elem_count = cv->elem_count;
    fcomp compar = cv->compar;

    void * result = elem_count ? (c_vector *)cv : NULL;

    for (int i = 1, prev = i - 1; i < elem_count; ++i, ++prev)
    {
        if (compar(data + prev * elem_size, data + i * elem_size) > 0)
        {
            result = NULL;
            break;
        }
    }

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_sort(c_vector * cv)
{
    if (!c_vect_is_sorted(cv))
        qsort(cv->arr, cv->elem_count, cv->elem_size, cv->compar);

    return cv;
}
//------------------------------------------------------------------------------

void * c_vect_bsearch_ind(c_vector * cv, const void * key, int * out_index)
{
    byte * data = (byte *)cv->arr, * cv_key = NULL;
    int elem_size = cv->elem_size, elem_count = cv->elem_count, out  = -1;
    fcomp compar = cv->compar;

    void * result = NULL;
    if (elem_count > 0)
    {
        int left = 0, right = elem_count - 1, mid, comp;
        while (left <= right)
        {
            mid = left + ((right - left) / 2);
            cv_key = data + mid * elem_size;

            comp = compar(cv_key, key);
            if (comp < 0)
                left = mid + 1;
            else if (comp > 0)
                right = mid - 1;
            else
            {
                result = cv_key;
                out = mid;
                break;
            }
        }
    }

    if (out_index)
        *out_index = out;

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_insert_online_ind(
    c_vector * cv, const void * key, int * out_index
    )
{
    byte * data = (byte *)cv->arr;
    fcomp compar = cv->compar;
    int elem_count = cv->elem_count, elem_size = cv->elem_size, out = 0;

    void * insert_spot = NULL;
    if (elem_count > 0)
    {
        byte * cv_key = NULL;
        int index = 0, last = elem_count - 1;
        int left = 0, right = last, mid;
        while (left <= right)
        {
            mid = left + ((right - left) / 2);
            cv_key = data + mid * elem_size;

            if (compar(cv_key, key) > 0)
                right = mid - 1;
            else
                left = mid + 1;
        }

        if (left < last)
            index =
            (compar(data + left * elem_size, key) > 0) ? left : left + 1;
        else
            index = left;

        grow(cv);
        data = (byte *)cv->arr;
        insert_spot = data + index * elem_size;
        memmove(data + (index+1) * elem_size, insert_spot,
                (elem_count - index) * elem_size);

        memcpy(insert_spot, key, elem_size);
        out = index;
        ++cv->elem_count;
    }
    else
    {
        insert_spot = c_vect_push(cv, key);
        out = 0;
    }

    if (out_index)
        *out_index = out;

    return insert_spot;
}
//------------------------------------------------------------------------------

void * c_vect_append_array(c_vector * cv, const void * arr, int len)
{
    void * result = cv;
    while (cv->capacity < cv->elem_count + len)
    {
        if (!c_vect_resize(cv, cv->capacity*C_VECT_GROWTH_RATE))
            return NULL;
    }

    memcpy((byte *)cv->arr + cv->elem_count * cv->elem_size,
            arr,
            cv->elem_size * len);

    cv->elem_count += len;
    return result;
}
//------------------------------------------------------------------------------

void * c_vect_get(c_vector * cv, int index)
{
    void * result = NULL;

    if ((unsigned int)index < (unsigned int)cv->elem_count)
        result = (byte *)cv->arr + index * cv->elem_size;

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_write_at(c_vector * cv, int index, const void * key)
{
    void * result = NULL;

    if ((unsigned int)index < (unsigned int)cv->elem_count)
    {
        int elem_size = cv->elem_size;
        result = memcpy((byte *)cv->arr + index * elem_size, key, elem_size);
    }

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_insert_at(c_vector * cv, int index, const void * key)
{
    void * result = NULL;
    int elem_count = cv->elem_count;

    if ((unsigned int)index < (unsigned int)elem_count)
    {
        grow(cv);
        byte * data = (byte *)cv->arr;
        int elem_size = cv->elem_size;
        void * source = data + index * elem_size;

        memmove(data + (index+1) * elem_size, source,
                (elem_count - index) * elem_size);

        c_vect_write_at(cv, index, key);
        ++cv->elem_count;
        result = source;
    }

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_remove_at(c_vector * cv, int index)
{
    void * result = NULL;

    byte * data = (byte *)cv->arr;
    int elem_count = cv->elem_count, elem_size = cv->elem_size;
    if ((unsigned int)index < (unsigned int)elem_count)
    {
        int last = elem_count-1;

        memmove(data + index * elem_size,
                data + (index+1) * elem_size,
                (last - index) * elem_size);

        --cv->elem_count;
        result = cv;
    }

    return result;
}
//------------------------------------------------------------------------------

int c_vect_remove_by_val(c_vector * cv, const void * key)
{
    void * found;
    int out_ind, from_ind = 0, removed = 0;
    while ((found = c_vect_find_ind_from(cv, key, &out_ind, from_ind)))
    {
        c_vect_remove_at(cv, out_ind);
        from_ind = out_ind;
        ++removed;
    }

    return removed;
}
//------------------------------------------------------------------------------

int c_vect_remove_if(c_vector * cv, fcomp condition, const void * key)
{
    fcomp back = cv->compar;
    cv->compar = condition;
    int removed = c_vect_remove_by_val(cv, key);
    cv->compar = back;
    return removed;
}
//------------------------------------------------------------------------------

void * c_vect_remove_range(c_vector * cv, int index_first, int index_last)
{
    void * ret = NULL;

    byte * data = (byte *)cv->arr;
    int elem_count = cv->elem_count, elem_size = cv->elem_size;
    int second = index_last+1;
    if ((unsigned int)index_first < (unsigned int)elem_count &&
        (unsigned int)index_last < (unsigned int)elem_count &&
        index_first < second)
    {
        memmove(data + index_first * elem_size,
                data + second * elem_size,
                (elem_count - second) * elem_size);

        cv->elem_count -= (second - index_first);
        ret = cv;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * c_vect_swap(c_vector * cv, int index_one, int index_two)
{
    void * ret = NULL;

    int elem_count = cv->elem_count;
    if ((unsigned int)index_one < (unsigned int)elem_count &&
        (unsigned int)index_two < (unsigned int)elem_count)
    {
        byte * data = (byte *)cv->arr;
        int elem_size = cv->elem_size;
        int stack_bsz = (elem_size < C_VECT_MAX_STACK_BUFF) ?
            elem_size : C_VECT_MAX_STACK_BUFF;

        byte stack_buff[stack_bsz], * buff = stack_buff;

        if (elem_size > C_VECT_MAX_STACK_BUFF &&
            !(buff = (byte *)malloc(elem_size)))
            return NULL;

        byte * one = data + index_one * cv->elem_size;
        byte * two = data + index_two * cv->elem_size;

        memcpy(buff, one, elem_size);
        memcpy(one, two, elem_size);
        memcpy(two, buff, elem_size);

        if (buff != stack_buff)
            free(buff);

        ret = cv;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * c_vect_swap_pop_at(c_vector * cv, int index)
{
    void * ret = NULL;

    int elem_count = cv->elem_count;
    if ((unsigned int)index < (unsigned int)elem_count)
    {
        if (c_vect_swap(cv, index, elem_count-1))
            ret = c_vect_pop(cv);
    }

    return ret;
}
//------------------------------------------------------------------------------

int c_vect_swap_pop_by_val(c_vector * cv, const void * key)
{
    void * found;
    int out_ind, from_ind = 0, popped = 0;
    while ((found = c_vect_find_ind_from(cv, key, &out_ind, from_ind)))
    {
        c_vect_swap_pop_at(cv, out_ind);
        from_ind = out_ind;
        ++popped;
    }

    return popped;
}
//------------------------------------------------------------------------------

int c_vect_swap_pop_if(c_vector * cv, fcomp condition, const void * key)
{
    fcomp back = cv->compar;
    cv->compar = condition;
    int popped = c_vect_swap_pop_by_val(cv, key);
    cv->compar = back;
    return popped;
}
//------------------------------------------------------------------------------

void * c_vect_reverse(c_vector * cv)
{
    int elem_count = cv->elem_count, end = elem_count/2;
    for(int i = 0, last = elem_count-1; i < end; ++i, --last)
        c_vect_swap(cv, i, last);

    return cv;
}
//------------------------------------------------------------------------------

void * c_vect_set_to_val(c_vector * cv, const void * key)
{
    int len = cv->elem_count;
    for (int i = 0; i < len; ++i)
        c_vect_write_at(cv, i, key);

    return cv;
}
//------------------------------------------------------------------------------

void * c_vect_apply_args(c_vector * cv, fapply_args fun, void * args)
{
    byte * data = (byte *)cv->arr;
    int all = cv->elem_count, elem_size = cv->elem_size;

    for (int i = 0; i < all; ++i)
        fun(data + i * elem_size, args);

    return cv;
}
//------------------------------------------------------------------------------

void * c_vect_apply(c_vector * cv, fapply fun)
{
    byte * data = (byte *)cv->arr;
    int all = cv->elem_count, elem_size = cv->elem_size;

    for (int i = 0; i < all; ++i)
        fun(data + i * elem_size);

    return cv;
}
//------------------------------------------------------------------------------

void * c_vect_zero_out(c_vector * cv)
{
    memset(cv->arr, 0, cv->capacity * cv->elem_size);
    return cv;
}
//------------------------------------------------------------------------------

void * c_vect_set_length(c_vector * cv, int len)
{
    void * result = NULL;

    if ((unsigned int)len <= (unsigned int)cv->capacity)
    {
        cv->elem_count = len;
        result = cv;
    }

    return result;
}
//------------------------------------------------------------------------------

void * c_vect_reset(c_vector * cv)
{
    cv->elem_count = 0;
    return cv;
}
//------------------------------------------------------------------------------

void * c_vect_set_compar(c_vector * cv, fcomp compar)
{
    cv->compar = compar;
    return cv;
}
//------------------------------------------------------------------------------

fcomp c_vect_compar(const c_vector * cv)
{
    return cv->compar;
}
//------------------------------------------------------------------------------

bool c_vect_is_empty(const c_vector * cv)
{
    return !cv->elem_count;
}
//------------------------------------------------------------------------------

void * c_vect_data(const c_vector * cv)
{
    return cv->arr;
}
//------------------------------------------------------------------------------

int c_vect_length(const c_vector * cv)
{
    return cv->elem_count;
}

//------------------------------------------------------------------------------

int c_vect_elem_size(const c_vector * cv)
{
    return cv->elem_size;
}
//------------------------------------------------------------------------------

int c_vect_capacity(const c_vector * cv)
{
    return cv->capacity;
}
//------------------------------------------------------------------------------
