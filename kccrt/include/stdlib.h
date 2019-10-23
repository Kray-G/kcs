#ifndef KCC_STDLIB_H
#define KCC_STDLIB_H

#include <_builtin.h>

#define EXIT_SUCCESS (0)
#define EXIT_FAILURE (2)

#define abort()                     __kcc_builtin_abort()
#define exit(code)                  __kcc_builtin_exit(code)

#define malloc(size)                __kcc_builtin_malloc(size)
#define calloc(size, n)             __kcc_builtin_calloc(size, n)
#define realloc(ptr, size)          __kcc_builtin_realloc(ptr, size)
#define free(p)                     __kcc_builtin_free(p)

#define strtol(s, endptr, base)     __kcc_builtin_strtol(s, endptr, base)
#define strtoul(s, endptr, base)    __kcc_builtin_strtoul(s, endptr, base)
#define strtoll(s, endptr, base)    __kcc_builtin_strtoll(s, endptr, base)
#define strtoull(s, endptr, base)   __kcc_builtin_strtoull(s, endptr, base)
#define strtof(s, endptr)           __kcc_builtin_strtof(s, endptr)
#define strtod(s, endptr)           __kcc_builtin_strtod(s, endptr)
#define strtold(s, endptr)          __kcc_builtin_strtod(s, endptr)
    /* strtold is same as strtod because double and long double is the same type. */

#define system(s)                   __kcc_builtin_system(s)

#define RAND_MAX 0x7fffU

void srand(unsigned seed);
int rand(void);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/stdlib.c>
#elif defined(__KCC__)
#pragma import("stdlib");
#endif
#endif

#endif  /* KCC_STDLIB_H */
