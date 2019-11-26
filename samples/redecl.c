#include <stdio.h>

int test(int n);
int test(int n)
{
    return printf("n = %d\n", n);
}

int test(int n);    /* no error */

/* Should be a compile error. */
int test(int n)
{
    return printf("n = %d\n", n);
}

int main()
{
    return test(12);
}
