#include <stdio.h>
#include <string.h>
#include <kcs/khash.h>

KHASH_MAP_INIT_STR(str, int)

int main(int argc, char *argv[])
{
    char s[4096]; // max string length: 4095 characters
    khash_t(str) *h;
    int index = 100;
    khint_t k;
    h = kh_init(str);
    while (scanf("%s", s) > 0) {
        if (!strcmp(s, ".")) break;
        int absent;
        k = kh_put(str, h, s, &absent);
        if (absent) {
            kh_key(h, k) = strdup(s);
            kh_val(h, k) = index++;
        }
        // else, the key is not touched; we do nothing
    }
    printf("# of distinct words: %d\n", kh_size(h));

    while (scanf("%s", s) > 0) {
        if (!strcmp(s, ".")) break;
        khint_t it = kh_get(str, h, s);
        if (it != kh_end(h)) {
            printf("%s: %d\n", s, kh_val(h, it));
        } else {
            printf("%s: Not found\n", s);
        }
    }

    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            printf("[%2d]: %s: %d\n", k, (char*)kh_key(h, k), kh_val(h, k));
            free((char*)kh_key(h, k));  /* for strdup. */
        }
    }

    kh_destroy(str, h);
    return 0;
}
