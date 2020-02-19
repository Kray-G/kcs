#include <stdio.h>
#include <kcs/ext.h>

void vector_sample()
{
    /* vector sample */
    vector_of_(int, x);
    vector_push(x, 100);
    vector_push(x, 200);
    vector_push(x, 300);
    for (int i = 0; i < vector_size(x); ++i) {
        printf("%d, ", x[i]);
    }
    printf("\n");
    vector_free(x);
}

int main()
{
    vector_sample();
    return 0;
}
