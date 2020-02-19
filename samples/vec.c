#include <stdio.h>
#include <kcs/kvec.h>

int main()
{
    int d;
    kvec_t(int) kv;
    kv_init(kv);
    kv_push(int, kv, 1);
    kv_push(int, kv, 10);
    kv_push(int, kv, 20);
    kv_push(int, kv, 30);
    kv_push(int, kv, 40);
    kv_A(kv, 3) = 300;          /* index must exist because of static access */
    kv_a(int, kv, 5) = 500;     /* expanding if needed */
    kv_a(int, kv, 6) = 600;
    printf("size: %d\n", kv.n);
    for (int i = 0; i < kv.n; ++i)
        printf("%d\n", kv_A(kv, i));
    kv_destroy(kv);
    return 0;
}
