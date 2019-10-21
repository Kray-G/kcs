#include <stdio.h>
#include <kcc/regex.h>

const char *make_str(const char *s, int b, int e)
{
    static char buf[256] = {0};
    int j = 0;
    for (int i = b; i < e; ++i) {
        buf[j++] = s[i];
    }
    buf[j] = 0;
    return buf;
}

void regex_test()
{
    const char *str = "abbbcabbcccababab";
    regex_t *r = regex_compile("(?:a(b*))b");
    while (regex_search(r, str)) {
        printf("found:\n");
        int n = r->num_regs;
        for (int i = 0; i < n; ++i) {
            printf("  %d: [%2d - %2d] %s\n", i, r->beg[i], r->end[i], make_str(str, r->beg[i], r->end[i]));
        }
    }
    regex_free(r);
}

int main(int ac, char **av)
{
    regex_test();
    return 0;
}

