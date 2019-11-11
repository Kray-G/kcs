#include <stdio.h>
#include <stdlib.h>

int cmp(const void *p, const void *q)
{
    return *(int*)p - *(int*)q;
}

main()
{
    int values[] = { 49, 2, 180, 23, 77, 13, 29, 88 };
    int n, i;

    n = sizeof(values)/sizeof(int);
    qsort(values, n, sizeof(int), cmp);
    for (i = 0; i < n; i++)
        printf("values[%d] = %d\n", i, values[i]);
}
