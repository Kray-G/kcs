#include <stdio.h>
#include <setjmp.h>
#include <stddef.h>

jmp_buf buffer;
int longjmp_failed = 1;

int setjmp_test(const int a, const int b)
{
    longjmp_failed = 0;
    printf("a = %d, b = %d\n", a, b);
    longjmp(buffer, 2);

    longjmp_failed = 1;
    printf("a + b is %d\n", a + b);
    return a + b;
}

int main(int argc, char * argv[])
{
    volatile int r, x = 100;
    if ((r = setjmp(buffer)) == 0) {
        x = 200;
        setjmp_test(3, 4); // never returns from here.

        x = 300;
        printf("This text is NOT shown because longjmp is called in the above function.\n");
    }
    printf("r = %d, x = %d\n", r, x);

    if (longjmp_failed) {
        printf("longjmp failed.\n");
    } else {
        printf("longjmp passed.\n");
    }
    return 0;
}
