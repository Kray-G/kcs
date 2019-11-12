#include <stdio.h>
#include <stdlib.h>

int cmp(const void *p, const void *q)
{
    return *(int*)p - *(int*)q;
}

#define N 20
int a[N];

int main(void)
{
    int i;

    printf("Before:");
    for (i = 0; i < N; i++) {
        a[i] = rand() / (RAND_MAX / 100 + 1);
        printf(" %2d", a[i]);
    }
    printf("\n");
    qsort(a, sizeof(a)/sizeof(a[0]), sizeof(a[0]), cmp);
    printf("After: ");
    for (i = 0; i < N; i++) printf(" %2d", a[i]);
    printf("\n");
    return 0;
}
