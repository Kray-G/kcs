#include <stdio.h>
#include <kcc/ext.h>

struct val {
    int type;
    int value;
};

void stack_sample()
{
    /* stack sample */
    stack_of_(struct val, x);
    stack_push(x, ((struct val){ .type = 1, .value = 100 }));
    stack_push(x, ((struct val){ .type = 1, .value = 200 }));
    stack_push(x, ((struct val){ .type = 2, .value = 300 }));
    while (stack_size(x) > 0) {
        printf("%d, ", stack_pop(x).value);
    }
    printf("\n");
    stack_free(x);
}

int main()
{
    stack_sample();
    return 0;
}
