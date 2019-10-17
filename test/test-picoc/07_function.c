#include <stdio.h>

int myfunc(int x)
{
    return x * x;
}

void vfunc(int a)
{
    printf("a=%d\n", a);
}

void qfunc()
{
    printf("qfunc()\n");
}

void main() {
    printf("%d\n", myfunc(3));
    printf("%d\n", myfunc(4));

    vfunc(1234);

    qfunc();
}
