#include <stdio.h>
#include <kcc/klist.h>

KLIST_INIT_NOALLOC(list_int, int)

int main()
{
    int d;
    klist_t(list_int) *kl;
    kliter_t(list_int) *p;
    kl = kl_init(list_int);
    *kl_pushp(list_int, kl) = 1;
    *kl_pushp(list_int, kl) = 10;
    *kl_pushp(list_int, kl) = 20;
    *kl_pushp(list_int, kl) = 30;
    *kl_pushp(list_int, kl) = 40;
    printf("size before removal: %d\n", kl->size);
    kl_shift(list_int, kl, &d);
    printf("removed: %d\n", d);
    kl_remove_next(list_int, kl, kl->head->next, &d);
    printf("removed: %d\n", d);
    printf("size after removal:  %d\n", kl->size);
    for (p = kl_begin(kl); p != kl_end(kl); p = kl_next(p))
        printf("%d\n", kl_val(p));
    kl_destroy(list_int, kl);
    return 0;
}
