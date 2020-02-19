#include <kcs.h>
#if !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif
#include <lacc/string.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

struct string_free_list_t {
    const char *str;
    struct string_free_list_t *next;
};

struct string_free_list_t string_free_list = {0};

static int printchar(FILE *stream, char ch)
{
    int c = (uint8_t) ch;
    if (isprint(c) && c != '"' && c != '\\') {
        putc(c, stream);
        return 1;
    }

    switch (c) {
    case '\b':
        return fprintf(stream, "\\b");
    case '\t':
        return fprintf(stream, "\\t");
    case '\n':
        return fprintf(stream, "\\n");
    case '\f':
        return fprintf(stream, "\\f");
    case '\r':
        return fprintf(stream, "\\r");
    case '\\':
        return fprintf(stream, "\\\\");
    case '\"':
        return fprintf(stream, "\\\"");
    default:
        return fprintf(stream, "\\%03o", c);
    }
}

INTERNAL const char *dup_string(const char *str)
{
    struct string_free_list_t *n = calloc(1, sizeof(struct string_free_list_t));
    n->next = string_free_list.next;
    string_free_list.next = n;
    n->str = strdup(str);
    return n->str;
}

INTERNAL void clear_string_all()
{
    struct string_free_list_t *p = string_free_list.next;
    while (p) {
        struct string_free_list_t *next = p->next;
        free((void*)p->str);
        free(p);
        p = next;
    }
    string_free_list.next = 0;
}

INTERNAL int fprintstr(FILE *stream, String str)
{
    int n, i;
    const char *raw;

    raw = str_raw(str);
    putc('"', stream);
    for (n = 0, i = 0; i < str.len; ++i) {
        n += printchar(stream, raw[i]);
    }

    putc('"', stream);
    return n + 2;
}

INTERNAL String str_init(const char *str)
{
    String s = {0};

    s.len = strlen(str);
    if (s.len < SHORT_STRING_LEN) {
        memcpy(s.a.str, str, s.len);
    } else {
        s.p.str = dup_string(str);
    }

    return s;
}

INTERNAL int str_cmp(String s1, String s2)
{
    int64_t *a, *b;
    if (s1.len != s2.len) {
        return 1;
    }

    if (s1.len < SHORT_STRING_LEN) {
        a = (int64_t *) ((void *) &s1);
        b = (int64_t *) ((void *) &s2);
        return a[0] != b[0] || a[1] != b[1];
    }

    return memcmp(s1.p.str, s2.p.str, s1.len);
}
