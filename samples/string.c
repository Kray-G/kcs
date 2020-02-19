#include <stdio.h>
#include <kcs/ext.h>

int main(void)
{
    int count = 3;
    string_t s = string_init("abc");
    string_t t = string_init("-0123-");
    for (int i = 0; i < 1000; ++i) {
        string_append_cstr(&s, "xyz");
        count += 3;
        if (i % 2) {
            string_append(&s, t);
            count += 6;
        }
    }
    printf("str = %s\n", s.cstr);
    printf("len = %d\n", s.len);
    printf("count = %d\n", count);

    string_clear(&s);
    printf("str = '%s'\n", s.cstr);
    printf("len = %d\n", s.len);

    string_free(&s);

    return 0;
}
