#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "oa_htable.h"
#include "cv_htable.h"
using namespace std;

static int compar(const void * k1, const void * k2)
{
    int * i1 = (int *)k1;
    int * i2 = (int *)k2;

    int result = 0;
    if (*i1 > *i2)
        result = 1;

    if (*i1 < *i2)
        result = -1;

    return result;
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

static oa_htbl time_oah(int * arr, int len)
{
	clock_t start, end;
	double time;

	oa_htbl oah_, * oah = &oah_;
	oa_htbl_make(oah, sizeof(int), compar);

	start = clock();

	for (int i = 0; i < len; ++i)
        oa_htbl_insert(oah, arr+i);

    /*
    int half = len/2;
    int * elem;

    for (int i = 0; i < len; ++i)
    {
        if (*(elem = (int *)oa_htbl_lookup(oah, arr+i)) != arr[i])
        {
            fprintf(stderr, "oah lookup failed\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < half; ++i)
        oa_htbl_remove(oah, arr+i);

    for (int i = 0; i < half; ++i)
    {
        oa_htbl_insert(oah, arr+i);
    }

    for (int i = 0; i < len; ++i)
    {
        if (*(elem = (int *)oa_htbl_lookup(oah, arr+i)) != arr[i])
        {
            fprintf(stderr, "oah lookup failed\n");
            exit(EXIT_FAILURE);
        }
    }
    //*/

	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;

	printf("oa_htbl - time: %f sec\n", time);
	return oah_;
}

static cv_htbl time_cvh(int * arr, int len)
{
	clock_t start, end;
	double time;

	cv_htbl cvh_, * cvh = &cvh_;
	cv_htbl_make(cvh, sizeof(int), compar);

	start = clock();

	for (int i = 0; i < len; ++i)
        cv_htbl_insert(cvh, arr+i);

    /*
    int half = len/2;
    int * elem;

    for (int i = 0; i < len; ++i)
    {
        if (*(elem = (int *)cv_htbl_lookup(cvh, arr+i)) != arr[i])
        {
            fprintf(stderr, "cvh lookup failed\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < half; ++i)
        cv_htbl_remove(cvh, arr+i);

    for (int i = 0; i < half; ++i)
    {
        cv_htbl_insert(cvh, arr+i);
    }

    for (int i = 0; i < len; ++i)
    {
        if (*(elem = (int *)cv_htbl_lookup(cvh, arr+i)) != arr[i])
        {
            fprintf(stderr, "cvh lookup failed\n");
            exit(EXIT_FAILURE);
        }
    }
    //*/

	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;

	printf("cv_htbl - time: %f sec\n", time);
	return cvh_;
}

static  unordered_set<int> time_cpp_uset(int * arr, int len)
{
	clock_t start, end;
	double time;

	unordered_set<int> cpps;

	start = clock();
	for (int i = 0; i < len; ++i)
		cpps.insert(arr[i]);

    /*
    int half = len/2;

    for (int i = 0; i < len; ++i)
    {
        if (*cpps.find(arr[i]) != arr[i])
        {
            fprintf(stderr, "oah lookup failed\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < half; ++i)
        cpps.erase(arr[i]);

    for (int i = 0; i < len; ++i)
		cpps.insert(arr[i]);

    for (int i = 0; i < len; ++i)
    {
        if (*cpps.find(arr[i]) != arr[i])
        {
            fprintf(stderr, "oah lookup failed\n");
            exit(EXIT_FAILURE);
        }
    }
    //*/

	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;

	printf("cpps    - time: %f sec\n", time);
	return cpps;
}

int usets_times(int numnums)
{
	int * arr = gen_rand_arr(numnums);
	oa_htbl oah = time_oah(arr, numnums);
	cv_htbl cvh = time_cvh(arr, numnums);
	unordered_set<int> cpps = time_cpp_uset(arr, numnums);

	vector<int> oah_v, us_v, cvh_v;

	int oah_len = oa_htbl_elem_count(&oah);
	int cpps_size = cpps.size();
	int cvh_len = cv_htbl_elem_count(&cvh);
	if (oah_len == cpps_size && oah_len == cvh_len)
	{
        printf("size: %d\n", oah_len);
        for (auto it = cpps.begin(); it != cpps.end(); ++it)
            us_v.push_back(*it);

        const int * p_oah;
        oa_htbl_iterator oa_start = oa_htbl_iterator_init();
        while ((p_oah = (const int *)oa_htbl_iterate(&oah, oa_start)))
            oah_v.push_back(*p_oah);

        const int * p_cvh;
        cv_htbl_iterator cv_start = cv_htbl_iterator_init();
        while ((p_cvh = (const int *)cv_htbl_iterate(&cvh, cv_start)))
            cvh_v.push_back(*p_cvh);

        sort(cvh_v.begin(), cvh_v.end());
        sort(us_v.begin(), us_v.end());
        sort(oah_v.begin(), oah_v.end());
        size_t end1 = us_v.size(), end2 = oah_v.size(), end3 = cvh_v.size();

        if (end1 != end2 || end1 != end3)
        {
            fprintf(stderr, "Err: vector of different lengths\n");
            return -1;
        }

        for (size_t i = 0; i < end1; ++i)
        {
            if (us_v[i] != oah_v[i] || us_v[i] != cvh_v[i])
            {
                fprintf(stderr, "Err: vectors differ\n");
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
    cv_htbl_destroy(&cvh);
	oa_htbl_destroy(&oah);
    return 0;
}
