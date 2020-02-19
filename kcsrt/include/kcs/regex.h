#ifndef REGEX_H
#define REGEX_H

#include <_ext.h>

typedef struct regex_ {
    void *h;
    int num_regs;
    int beg[10];
    int end[10];
} regex_t;

regex_t *regex_compile(const char *pattern);
int regex_search(regex_t *regex, const char *str);
void regex_free(regex_t *regex);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcs/ext_regex.c>
#endif
#endif

#endif /* REGEX_H */
