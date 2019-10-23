#include <stdio.h>
#include <kcc/bigint.h>

void bigint_sample()
{
    char buf[2048]; // needs 1135 digits for 500!.

    /* neat trick to avoid having to write &a, &b, &c */
    bigint_t a[1], b[1];
    bigint_init(a);
    bigint_init(b);

    /* factorial 500 */
    bigint_from_int(b, 1);
    for (int i = 500; i > 0; --i) {
        bigint_from_int(a, i);
        bigint_mul(b, b, a);
    }

    puts(bigint_write(buf, sizeof(buf), b));

    bigint_free(a);
    bigint_free(b);
}

int main()
{
    bigint_sample();
    return 0;
}
