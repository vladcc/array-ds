#include <stdio.h>
#include "./unordered_sets/usets.h"
#include "./vect_times/vector_times.h"
#include "./sets/set_times.h"


int main(void)
{
    //*
    vector_times();
    //*/

    /*
    int times[] = {
        5, 10, 15, 100, 150,
        1500, 15000, 25000, 35000,
        45000, 150000, 350000
    };
    int tsz = sizeof(times)/sizeof(*times);
    for (int i = 0; i < tsz; ++i)
    {
        if (time_sets(times[i]) != 0)
            break;
        putchar('\n');
    }
    */

    /*
    int utimes[] = {
            //5, 10, 15, 100, 150,
            //15000, 25000, 35000, 45000, 150000, 350000,
            550000, 1500000, 5500000,
            //15000000, 45000000, 100000000
    };
    int utsz = sizeof(utimes)/sizeof(*utimes);
    for (int i = 0; i < utsz; ++i)
    {
        if (usets_times(utimes[i]) != 0)
            break;
        putchar('\n');
    }
    */
    return 0;
}
