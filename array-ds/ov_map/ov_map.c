#include "ov_map.h"
//------------------------------------------------------------------------------

void * ov_map_insert(ov_map * map, const void * what)
{

    void * ret = c_vect_bsearch(map, what);
    if (!ret)
        ret = c_vect_insert_online(map, what);
    return ret;
}
//------------------------------------------------------------------------------

void * ov_map_remove(ov_map * map, const void * what)
{
    int index = -1;
    return (c_vect_bsearch_ind(map, what, &index)) ?
        c_vect_remove_at(map, index) : NULL;
}
//------------------------------------------------------------------------------

