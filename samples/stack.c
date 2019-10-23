#include <stdio.h>
#include <kcc/ext.h>

void stack_sample()
{
    /* stack sample */
    stack_of_(int, x);
    stack_push(x, 100);
    stack_push(x, 200);
    stack_push(x, 300);
    while (stack_size(x) > 0) {
        printf("%d, ", stack_pop(x));
    }
    printf("\n");
    stack_free(x);
}

int main()
{
    stack_sample();
    return 0;
}
