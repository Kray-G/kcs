#include <stdio.h>
#include <kcc/ext.h>

void queue_sample()
{
    /* queue sample */
    queue_of_(int, x);
    queue_enqueue(x, 100);
    queue_enqueue(x, 200);
    queue_enqueue(x, 300);
    while (queue_size(x) > 0) {
        printf("%d, ", queue_dequeue(x));
    }
    printf("\n");
    queue_free(x);
}

int main()
{
    queue_sample();
    return 0;
}
