#include "ov_set.h"
//------------------------------------------------------------------------------

const void * ov_set_insert(ov_set * set, const void * what)
{
    return (c_vect_bsearch(set, what)) ? NULL :
        c_vect_insert_online(set, what);
}
//------------------------------------------------------------------------------

void * ov_set_remove(ov_set * set, const void * what)
{
    int index = -1;
    return (c_vect_bsearch_ind(set, what, &index)) ?
        c_vect_remove_at(set, index) : NULL;
}
//------------------------------------------------------------------------------

void * ov_set_apply(ov_set * set, fapply_const fun)
{
    return c_vect_apply(set, (fapply)fun);
}
//------------------------------------------------------------------------------

void * ov_set_apply_args(ov_set * set, fapply_const_args fun, void * args)
{
    return c_vect_apply_args(set, (fapply_args)fun, args);
}
//------------------------------------------------------------------------------
