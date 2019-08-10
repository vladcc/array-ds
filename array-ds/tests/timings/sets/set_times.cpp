#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../../ov_set/ov_set.h"
#include <set>
using namespace std;

static int compar(const void * k1, const void * k2)
{
    int i1 = *((int *)k1);
    int i2 = *((int *)k2);
    return (i1 > i2) - (i1 < i2);
}

static int * gen_rand_arr(int len)
{
	int * arr = (int *)malloc(len * sizeof(int));
	if (!arr)
	{
		fprintf(stderr, "Err: Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));
	for (int i = 0; i < len; ++i)
		arr[i] = rand() % len;

	return arr;
}

static ov_set time_bset(int * arr, int len)
{
	clock_t start, end;
	double time;

	ov_set ovs_, * ovs = &ovs_;
	ov_set_make(ovs, sizeof(int), compar);

	start = clock();
	for (int i = 0; i < len; ++i)
		ov_set_insert(ovs, arr+i);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;

	printf("ov_set - size: %d, time: %f sec\n", ov_set_length(ovs), time);
	return ovs_;
}

static  set<int> time_cpp_set(int * arr, int len)
{
	clock_t start, end;
	double time;

	set<int> cpps;

	start = clock();
	for (int i = 0; i < len; ++i)
		cpps.insert(arr[i]);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;

	printf("cpps   - size: %lu, time: %f sec\n", cpps.size(), time);
	return cpps;
}

/*
static void print_arr(int * arr, int len)
{
	for (int i = 0; i < len; ++i)
		printf("%d ", arr[i]);
	putchar('\n');
}
*/

int time_sets(int numnums)
{
	int * arr = gen_rand_arr(numnums);
	ov_set ovs = time_bset(arr, numnums);
	set<int> cpps = time_cpp_set(arr, numnums);

	int * povsd = (int *)ov_set_data(&ovs);
	int ovslen = ov_set_length(&ovs);
	if (ovslen == (int)cpps.size())
	{
		auto it = cpps.begin();
		for (int i = 0; i < ovslen; ++i, ++it)
		{
			if (povsd[i] != *it)
			{
				fprintf(stderr, "Err: sets differ\n");
				return -1;
			}
		}
	}
	else
	{
		fprintf(stderr, "Err: sets of different sizes\n");
		return -1;
	}

	free(arr);
	ov_set_destroy(&ovs);

	return 0;
}
