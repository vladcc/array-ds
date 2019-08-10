#include <string.h>
#include <stdlib.h>
#include "f_array.h"

typedef unsigned char byte;

void * f_arr_make(f_array * far, int elem_size, fcomp compar, int capacity)
{
    if (elem_size > 0 && capacity > 0)
    {
        far->elem_size = elem_size;
        far->capacity = capacity;
        far->elem_count = 0;
        far->compar = compar;

        if ((far->arr = malloc(elem_size * capacity)))
            return far;

    }

    memset(far, 0, sizeof(*far));
    return NULL;
}
//------------------------------------------------------------------------------

void f_arr_destroy(f_array * far)
{
    free(far->arr);
    memset(far, 0, sizeof(*far));
}
//------------------------------------------------------------------------------

f_array f_arr_copy(const f_array * src, bool * out_success)
{
    f_array ret;
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

void * f_arr_push(f_array * far, const void * what)
{
    void * ret = NULL;

    if (far->elem_count < far->capacity)
    {
        void * dest = (byte *)far->arr + far->elem_count * far->elem_size;
        ++far->elem_count;
        ret = memcpy(dest, what, far->elem_size);
    }

    return ret;
}
//------------------------------------------------------------------------------

void * f_arr_pop(f_array * far)
{
    void * ret = NULL;

    if (far->elem_count > 0)
    {
        --far->elem_count;
        ret = far;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * f_arr_peek(f_array * far)
{
    void * result = NULL;
    int elem_count = far->elem_count;

    if (elem_count > 0)
        result = (byte *)far->arr + (elem_count-1) * far->elem_size;

    return result;
}
//------------------------------------------------------------------------------

void * f_arr_peek_pop(f_array * far)
{
    void * result = NULL;
    int elem_count = far->elem_count;

    if (elem_count > 0)
    {
        result = (byte *)far->arr + (elem_count-1) * far->elem_size;
        --far->elem_count;
    }

    return result;
}
//------------------------------------------------------------------------------

void * f_arr_find_ind_from(
    f_array * far, const void * key, int * out_index, int from_index
    )
{
    byte * far_key = NULL, * data = (byte *)far->arr;
    int elem_size = far->elem_size, elem_count = far->elem_count, out = -1;
    fcomp compar = far->compar;

    void * found = NULL;
    if ((unsigned int)from_index < (unsigned int)elem_count)
    {
        for(int i = from_index; i < elem_count; ++i)
        {
            far_key = data + i * elem_size;
            if (compar(far_key, key) == 0)
            {
                found = far_key;
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

void * f_arr_find_if_ind_from(
    f_array * far, fcomp condition, const void * key, int * out_index,
    int from_index
    )
{
    fcomp back = far->compar;
    far->compar = condition;
    void * found = f_arr_find_ind_from(far, key, out_index, from_index);
    far->compar = back;
    return found;
}
//------------------------------------------------------------------------------

void * f_arr_is_sorted(const f_array * far)
{
    byte * data = (byte *)far->arr;
    int elem_size = far->elem_size, elem_count = far->elem_count;
    fcomp compar = far->compar;

    void * result = elem_count ? (f_array *)far : NULL;

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

void * f_arr_sort(f_array * far)
{
    if (!f_arr_is_sorted(far))
        qsort(far->arr, far->elem_count, far->elem_size, far->compar);

    return far;
}
//------------------------------------------------------------------------------

void * f_arr_bsearch_ind(f_array * far, const void * key, int * out_index)
{
    byte * data = (byte *)far->arr, * far_key = NULL;
    int elem_size = far->elem_size, elem_count = far->elem_count, out  = -1;
    fcomp compar = far->compar;

    void * result = NULL;
    if (elem_count > 0)
    {
        int left = 0, right = elem_count - 1, mid, comp;
        while (left <= right)
        {
            mid = left + ((right - left) / 2);
            far_key = data + mid * elem_size;

            comp = compar(far_key, key);
            if (comp < 0)
                left = mid + 1;
            else if (comp > 0)
                right = mid - 1;
            else
            {
                result = far_key;
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

void * f_arr_insert_online_ind(
    f_array * far, const void * key, int * out_index
    )
{
    byte * data = (byte *)far->arr;
    fcomp compar = far->compar;
    int elem_count = far->elem_count, elem_size = far->elem_size, out = 0;

    if (elem_count >= far->capacity)
        return NULL;

    void * insert_spot = NULL;
    if (elem_count > 0)
    {
        byte * far_key = NULL;
        int index = 0, last = elem_count - 1;
        int left = 0, right = last, mid;
        while (left <= right)
        {
            mid = left + ((right - left) / 2);
            far_key = data + mid * elem_size;

            if (compar(far_key, key) > 0)
                right = mid - 1;
            else
                left = mid + 1;
        }

        if (left < last)
            index =
            (compar(data + left * elem_size, key) > 0) ? left : left + 1;
        else
            index = left;

        data = (byte *)far->arr;
        insert_spot = data + index * elem_size;
        memmove(data + (index+1) * elem_size, insert_spot,
                (elem_count - index) * elem_size);

        memcpy(insert_spot, key, elem_size);
        out = index;
        ++far->elem_count;
    }
    else
    {
        insert_spot = f_arr_push(far, key);
        out = 0;
    }

    if (out_index)
        *out_index = out;

    return insert_spot;
}
//------------------------------------------------------------------------------

void * f_arr_get(f_array * far, int index)
{
    void * result = NULL;

    if ((unsigned int)index < (unsigned int)far->elem_count)
        result = (byte *)far->arr + index * far->elem_size;

    return result;
}
//------------------------------------------------------------------------------

void * f_arr_write_at(f_array * far, int index, const void * key)
{
    void * result = NULL;

    if ((unsigned int)index < (unsigned int)far->elem_count)
    {
        int elem_size = far->elem_size;
        result = memcpy((byte *)far->arr + index * elem_size, key, elem_size);
    }

    return result;
}
//------------------------------------------------------------------------------

void * f_arr_insert_at(f_array * far, int index, const void * key)
{
    void * result = NULL;
    int elem_count = far->elem_count;

    if (elem_count < far->capacity &&
        (unsigned int)index < (unsigned int)elem_count)
    {
        byte * data = (byte *)far->arr;
        int elem_size = far->elem_size;
        void * source = data + index * elem_size;

        memmove(data + (index+1) * elem_size, source,
                (elem_count - index) * elem_size);

        f_arr_write_at(far, index, key);
        ++far->elem_count;
        result = source;
    }

    return result;
}
//------------------------------------------------------------------------------

void * f_arr_remove_at(f_array * far, int index)
{
    void * result = NULL;

    byte * data = (byte *)far->arr;
    int elem_count = far->elem_count, elem_size = far->elem_size;
    if ((unsigned int)index < (unsigned int)elem_count)
    {
        int last = elem_count-1;

        memmove(data + index * elem_size,
                data + (index+1) * elem_size,
                (last - index) * elem_size);

        --far->elem_count;
        result = far;
    }

    return result;
}
//------------------------------------------------------------------------------

int f_arr_remove_by_val(f_array * far, const void * key)
{
    void * found;
    int out_ind, from_ind = 0, removed = 0;
    while ((found = f_arr_find_ind_from(far, key, &out_ind, from_ind)))
    {
        f_arr_remove_at(far, out_ind);
        from_ind = out_ind;
        ++removed;
    }

    return removed;
}
//------------------------------------------------------------------------------

int f_arr_remove_if(f_array * far, fcomp condition, const void * key)
{
    fcomp back = far->compar;
    far->compar = condition;
    int removed = f_arr_remove_by_val(far, key);
    far->compar = back;
    return removed;
}
//------------------------------------------------------------------------------

void * f_arr_remove_range(f_array * far, int index_first, int index_last)
{
    void * ret = NULL;

    byte * data = (byte *)far->arr;
    int elem_count = far->elem_count, elem_size = far->elem_size;
    int second = index_last+1;
    if ((unsigned int)index_first < (unsigned int)elem_count &&
        (unsigned int)index_last < (unsigned int)elem_count &&
        index_first < second)
    {
        memmove(data + index_first * elem_size,
                data + second * elem_size,
                (elem_count - second) * elem_size);

        far->elem_count -= (second - index_first);
        ret = far;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * f_arr_swap(f_array * far, int index_one, int index_two)
{
    void * ret = NULL;

    int elem_count = far->elem_count;
    if ((unsigned int)index_one < (unsigned int)elem_count &&
        (unsigned int)index_two < (unsigned int)elem_count)
    {
        byte * data = (byte *)far->arr;
        int elem_size = far->elem_size;
        int stack_bsz = (elem_size < F_ARR_MAX_STACK_BUFF) ?
            elem_size : F_ARR_MAX_STACK_BUFF;

        byte stack_buff[stack_bsz], * buff = stack_buff;

        if (elem_size > F_ARR_MAX_STACK_BUFF &&
            !(buff = (byte *)malloc(elem_size)))
            return NULL;

        byte * one = data + index_one * far->elem_size;
        byte * two = data + index_two * far->elem_size;

        memcpy(buff, one, elem_size);
        memcpy(one, two, elem_size);
        memcpy(two, buff, elem_size);

        if (buff != stack_buff)
            free(buff);

        ret = far;
    }

    return ret;
}
//------------------------------------------------------------------------------

void * f_arr_swap_pop_at(f_array * far, int index)
{
    void * ret = NULL;

    int elem_count = far->elem_count;
    if ((unsigned int)index < (unsigned int)elem_count)
    {
        if (f_arr_swap(far, index, elem_count-1))
            ret = f_arr_pop(far);
    }

    return ret;
}
//------------------------------------------------------------------------------

int f_arr_swap_pop_by_val(f_array * far, const void * key)
{
    void * found;
    int out_ind, from_ind = 0, popped = 0;
    while ((found = f_arr_find_ind_from(far, key, &out_ind, from_ind)))
    {
        f_arr_swap_pop_at(far, out_ind);
        from_ind = out_ind;
        ++popped;
    }

    return popped;
}
//------------------------------------------------------------------------------

int f_arr_swap_pop_if(f_array * far, fcomp condition, const void * key)
{
    fcomp back = far->compar;
    far->compar = condition;
    int popped = f_arr_swap_pop_by_val(far, key);
    far->compar = back;
    return popped;
}
//------------------------------------------------------------------------------

void * f_arr_reverse(f_array * far)
{
    int elem_count = far->elem_count, end = elem_count/2;
    for(int i = 0, last = elem_count-1; i < end; ++i, --last)
        f_arr_swap(far, i, last);

    return far;
}
//------------------------------------------------------------------------------

void * f_arr_set_to_val(f_array * far, const void * key)
{
    int len = far->elem_count;
    for (int i = 0; i < len; ++i)
        f_arr_write_at(far, i, key);

    return far;
}
//------------------------------------------------------------------------------

void * f_arr_apply_args(f_array * far, fapply_args fun, void * args)
{
    byte * data = (byte *)far->arr;
    int all = far->elem_count, elem_size = far->elem_size;

    for (int i = 0; i < all; ++i)
        fun(data + i * elem_size, args);

    return far;
}
//------------------------------------------------------------------------------

void * f_arr_apply(f_array * far, fapply fun)
{
    byte * data = (byte *)far->arr;
    int all = far->elem_count, elem_size = far->elem_size;

    for (int i = 0; i < all; ++i)
        fun(data + i * elem_size);

    return far;
}
//------------------------------------------------------------------------------

void * f_arr_zero_out(f_array * far)
{
    memset(far->arr, 0, far->capacity * far->elem_size);
    return far;
}
//------------------------------------------------------------------------------

void * f_arr_set_length(f_array * far, int len)
{
    void * result = NULL;

    if ((unsigned int)len <= (unsigned int)far->capacity)
    {
        far->elem_count = len;
        result = far;
    }

    return result;
}
//------------------------------------------------------------------------------

void * f_arr_reset(f_array * far)
{
    far->elem_count = 0;
    return far;
}
//------------------------------------------------------------------------------

void * f_arr_set_compar(f_array * far, fcomp compar)
{
    far->compar = compar;
    return far;
}
//------------------------------------------------------------------------------

fcomp f_arr_compar(const f_array * far)
{
    return far->compar;
}
//------------------------------------------------------------------------------

bool f_arr_is_empty(const f_array * far)
{
    return !far->elem_count;
}
//------------------------------------------------------------------------------

void * f_arr_data(const f_array * far)
{
    return far->arr;
}
//------------------------------------------------------------------------------

int f_arr_length(const f_array * far)
{
    return far->elem_count;
}

//------------------------------------------------------------------------------

int f_arr_elem_size(const f_array * far)
{
    return far->elem_size;
}
//------------------------------------------------------------------------------

int f_arr_capacity(const f_array * far)
{
    return far->capacity;
}
//------------------------------------------------------------------------------
