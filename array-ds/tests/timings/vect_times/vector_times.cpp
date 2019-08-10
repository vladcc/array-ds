#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>
#include "../../../c_vector/c_vector.h"
#include "vector_times.h"

using namespace std;

#define check(expr, str) while (!(expr)) pquit((char *)str);

void pquit(char * str)
{
    printf("Err: %s\n", str);
    exit(EXIT_FAILURE);
}

int * gen_rand_arr_int(int len)
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

/*
static void memmove_time(void)
{
    c_vector cv_, * cv = &cv_, cv_2, * cv2 = &cv_2;

    const int kCap = 1000000;
    c_vect_make_cap(cv, sizeof(int), NULL, kCap+1);
    c_vect_set_length(cv, kCap);
    c_vect_make_cap(cv2, sizeof(int), NULL, kCap+1);
    c_vect_set_length(cv2, kCap);

    const int kFirst = 1000000;
    int i;
    for (i = 0; i < kFirst; ++i)
        c_vect_write_at(cv, i, &i);

    int j;
    for (i = 0; i < kFirst; ++i)
        j = i+1, c_vect_write_at(cv2, i, &j);

    printf("### Moving %d ints ###\n", kFirst);
    puts("\nOne to the right");
    clock_t start, end;
    double time = 0.0D;

    int * data = (int *)c_vect_data(cv);
    int * data2 = (int *)c_vect_data(cv2);

    start = clock();
    for (int i = kFirst; i >= 1; --i)
        data[i] = data[i-1];
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;

    for (i = 1; i < kFirst; ++i)
        check(*((int *)c_vect_get(cv, i)) == i-1, "Bad array");
    printf("loop     : %f sec\n", time);

    start = clock();
    memmove(data2+1, data2, kFirst*sizeof(*data2));
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;

    for (i = 1; i < kFirst; ++i)
        check(*((int *)c_vect_get(cv2, i)) == i, "Bad array");
    printf("memmove(): %f sec\n", time);

    puts("\nOne to the left");

    start = clock();
    for (i = 1; i < kFirst; ++i)
        data[i-1] = data[i];
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;

    for (i = 0; i < kFirst-1; ++i)
        check(*((int *)c_vect_get(cv, i)) == i, "Bad array");
    printf("loop     : %f sec\n", time);

    start = clock();
    memmove(data2, data2+1, kFirst*sizeof(*data2));
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;

    for (i = 0; i < kFirst-1; ++i)
        check(*((int *)c_vect_get(cv2, i)) == i+1, "Bad array");
    printf("memmove(): %f sec\n", time);
    putchar('\n');

    c_vect_destroy(cv);
    c_vect_destroy(cv2);
}
*/

static int compar_int(const void * k1, const void * k2)
{
    int i1 = *((int *)k1);
    int i2 = *((int *)k2);
    return (i1 > i2) - (i1 < i2);
}

static void vector_ints(void)
{
    clock_t start, end;
    double time = 0.0D;

    vector<int> vint;

    c_vector cv_, * cv = &cv_;
    c_vect_make_cap(cv, sizeof(int), compar_int, 1);
    const int kMil = 1000000;

    int * rarr = gen_rand_arr_int(kMil);

    printf("### vectors with ints ###\n");
    printf("\nPushing random %d ints\n", kMil);

    start = clock();
    for(int i = 0; i < kMil; ++i)
        vint.push_back(rarr[i]);
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("cpp vect: %f sec\n", time);

    start = clock();
    for(int i = 0; i < kMil; ++i)
        c_vect_push(cv, rarr+i);
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("c   vect: %f sec\n", time);

    for (int i = 0; i < kMil; ++i)
        check(vint[i] == *((int *)c_vect_get(cv, i)), "Arrays differ");

    printf("\nTraversing\n");
    int a_;

    int * data = (int *)c_vect_data(cv);
    start = clock();
    for(int i = 0; i < kMil; ++i)
        a_ += data[i];
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("c vect data*: %f sec\n", time);

    start = clock();
    for(int i = 0; i < kMil; ++i)
        a_ += vint[i];
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("cpp vect[]  : %f sec\n", time);

    start = clock();
    for(int i = 0; i < kMil; ++i)
        a_ += *(int *)c_vect_get(cv, i);
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("c   vect get: %f sec\n", time);

    start = clock();
    for(int i = 0; i < kMil; ++i)
        a_ += vint.at(i);
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("cpp vect at : %f sec\n", time);

    printf("\nSorting\n");
    start = clock();
    std::sort(vint.begin(), vint.end());
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("cpp vect: %f sec\n", time);
    check(std::is_sorted(vint.begin(), vint.end()), "vint not sorted");

    start = clock();
    c_vect_sort(cv);
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("c   vect: %f sec\n", time);
    check(c_vect_is_sorted(cv), "cv not sorted");
    check(c_vect_length(cv) == (int)vint.size() && c_vect_length(cv) == kMil,
        "Arrays of different sizes");
    for (int i = 0; i < kMil; ++i)
        check(vint[i] == *((int *)c_vect_get(cv, i)), "Arrays differ");

    const int kRmv = 5000;
    printf("\nRemoving first %d elements\n", kRmv);
    start = clock();
    for (int i = 0; i < kRmv; ++i)
        vint.erase(vint.begin());
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("cpp vect: %f sec\n", time);

    start = clock();
    for (int i = 0; i < kRmv; ++i)
        c_vect_remove_at(cv, 0);
    end = clock();
    time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("c   vect: %f sec\n", time);

    for (int i = 0; i < kMil-kRmv; ++i)
        check(vint[i] == *((int *)c_vect_get(cv, i)), "Arrays differ");

    free(rarr);
    c_vect_destroy(cv);
}

typedef struct arrst {
    double d;
    int i;
} arrst;

static int compar_arrst(const void * k1, const void * k2)
{
    int i1 = ((arrst *)k1)->i;
    int i2 = ((arrst *)k2)->i;
    return (i1 > i2) - (i1 < i2);
}

typedef struct arrst2 {
    int i;
    double d;
} arrst2;

static int compar_arrst2(const void * k1, const void * k2)
{
    int i1 = ((arrst2 *)k1)->i;
    int i2 = ((arrst2 *)k2)->i;
    return (i1 > i2) - (i1 < i2);
}

static void vector_structs(void)
{

    clock_t start, end;
    double time = 0.0D;

    const int kMil = 1000000;
    int * rarr = gen_rand_arr_int(kMil);

    {
        struct less_than_key
        {
            inline bool operator() (const arrst& key1, const arrst& key2)
            {
                return (key1.i < key2.i);
            }
        };

        arrst proto = {8.5D, 0};
        vector<arrst> vint;

        c_vector cv_, * cv = &cv_;
        c_vect_make_cap(cv, sizeof(arrst), compar_arrst, 1);

        printf("\n### vectors with struct {double, int} ###\n");
        printf("\nPushing random %d\n", kMil);

        start = clock();
        for(int i = 0; i < kMil; ++i)
            proto.i = rarr[i], vint.push_back(proto);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect: %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
            proto.i = rarr[i], c_vect_push(cv, &proto);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect: %f sec\n", time);

        for (int i = 0; i < kMil; ++i)
            check(vint[i].i == (((arrst *)c_vect_get(cv, i))->i), "Arrays differ");

        printf("\nTraversing\n");
        arrst a_;
        int n = 0;
        double d = 0.0D;

        arrst * data = (arrst *)c_vect_data(cv);
        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = data[i];
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c vect data*: %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = vint[i];
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect[]  : %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = *(arrst *)c_vect_get(cv, i);
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect get: %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = vint.at(i);
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect at : %f sec\n", time);


        printf("\nSorting\n");
        start = clock();
        std::sort(vint.begin(), vint.end(), less_than_key());
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect: %f sec\n", time);
        check(std::is_sorted(vint.begin(), vint.end(), less_than_key()),
            "vint not sorted");

        start = clock();
        c_vect_sort(cv);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect: %f sec\n", time);
        check(c_vect_is_sorted(cv), "cv not sorted");
        check(c_vect_length(cv) == (int)vint.size() && c_vect_length(cv) == kMil,
            "Arrays of different sizes");

        for (int i = 0; i < kMil; ++i)
            check(vint[i].i == (((arrst *)c_vect_get(cv, i))->i), "Arrays differ");

        const int kRmv = 1000;
        printf("\nRemoving first %d elements\n", kRmv);
        start = clock();
        for (int i = 0; i < kRmv; ++i)
            vint.erase(vint.begin());
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect: %f sec\n", time);

        start = clock();
        for (int i = 0; i < kRmv; ++i)
            c_vect_remove_at(cv, 0);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect: %f sec\n", time);
        c_vect_destroy(cv);
    }

    {
        struct less_than_key2
        {
            inline bool operator() (const arrst2& key1, const arrst2& key2)
            {
                return (key1.i < key2.i);
            }
        };

        vector<arrst2> vint2;
        arrst2 proto2 = {0, 8.5D};

        c_vector cv_, * cv = &cv_;
        c_vect_make_cap(cv, sizeof(arrst2), compar_arrst2, 1);

        printf("\n### vectors with struct {int, double} ###\n");
        printf("\nPushing random %d\n", kMil);

        start = clock();
        for(int i = 0; i < kMil; ++i)
            proto2.i = rarr[i], vint2.push_back(proto2);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect: %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
            proto2.i = rarr[i], c_vect_push(cv, &proto2);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect: %f sec\n", time);

        for (int i = 0; i < kMil; ++i)
            check(vint2[i].i == (((arrst2 *)c_vect_get(cv, i))->i),
                "Arrays differ");

        printf("\nTraversing\n");
        arrst2 a_;
        int n = 0;
        double d = 0.0D;

        arrst2 * data = (arrst2 *)c_vect_data(cv);
        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = data[i];
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c vect data*: %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = vint2[i];
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect[]  : %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = *(arrst2 *)c_vect_get(cv, i);
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect get: %f sec\n", time);

        start = clock();
        for(int i = 0; i < kMil; ++i)
        {
            a_ = vint2.at(i);
            d += a_.d;
            n += a_.i;
        }
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect at : %f sec\n", time);


        printf("\nSorting\n");
        start = clock();
        std::sort(vint2.begin(), vint2.end(), less_than_key2());
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect: %f sec\n", time);
        check(std::is_sorted(vint2.begin(), vint2.end(), less_than_key2()),
            "vint not sorted");

        start = clock();
        c_vect_sort(cv);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect: %f sec\n", time);
        check(c_vect_is_sorted(cv), "cv not sorted");
        check(c_vect_length(cv) == (int)vint2.size() && c_vect_length(cv) == kMil,
            "Arrays of different sizes");

        for (int i = 0; i < kMil; ++i)
            check(vint2[i].i == (((arrst2 *)c_vect_get(cv, i))->i), "Arrays differ");

        const int kRmv = 1000;
        printf("\nRemoving first %d elements\n", kRmv);
        start = clock();
        for (int i = 0; i < kRmv; ++i)
            vint2.erase(vint2.begin());
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("cpp vect: %f sec\n", time);

        start = clock();
        for (int i = 0; i < kRmv; ++i)
            c_vect_remove_at(cv, 0);
        end = clock();
        time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("c   vect: %f sec\n", time);
        c_vect_destroy(cv);
    }

    free(rarr);

}

void vector_times(void)
{
    //memmove_time();
    vector_ints();
    vector_structs();
}
