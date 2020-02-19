#ifndef KCC_STDLIB_RAND_C
#define KCC_STDLIB_RAND_C

#define KCC_NO_IMPORT
#include <stdlib.h>
#undef KCC_NO_IMPORT

static unsigned long __kcc_rand_seed = 1;

void srand(unsigned seed)
{
    __kcc_rand_seed = seed;
}

int rand(void)
{
    __kcc_rand_seed = __kcc_rand_seed * 1103515245L + 12345;
    return (unsigned)(__kcc_rand_seed / 65536L) % (RAND_MAX+1);
}

#define SWAP(x, y, size)            \
    do {                            \
        size_t s = (size);          \
        char *a = (x), *b = (y);    \
        do {                        \
            char t = *a;            \
            *a++ = *b;              \
            *b++ = t;               \
        } while (--s > 0);          \
    } while (0)                     \
    /**/

#define KCCQ(a, i)    ((char *)a + (i) * size)

void __quicksort(int first, int last, void *base, size_t size, int (*comp)(const void *, const void *))
{
    int i, j;
    void *x = KCCQ(base, (first + last) / 2);
    i = first;  j = last;
    for ( ; ; ) {
        while (comp(x, KCCQ(base, i)) > 0) i++;
        while (comp(KCCQ(base, j), x) > 0) j--;
        if (i >= j) break;
        SWAP(KCCQ(base,i), KCCQ(base,j), size);
        i++;
        j--;
    }
    if (first  < i - 1) __quicksort(first , i - 1, base, size, comp);
    if (j + 1 < last) __quicksort(j + 1, last, base, size, comp);
}

#undef KCCQ

void qsort(void *base, size_t n, size_t size, int (*comp)(const void *, const void *))
{
    __quicksort(0, n - 1, base, size, comp);
}

#endif
