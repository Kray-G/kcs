#ifndef KCC_ASSERT_H
#define KCC_ASSERT_H

#include <_builtin.h>

#ifdef NDEBUG
#define assert(expr)    ((void)0)
#else
void __kcc_builtin_abort(void);
void __kcc_assert_fail(const char *file, int line, const char *msg);
#define assert(expr)    ((expr) ? (void)0 : __kcc_assert_fail(__FILE__, __LINE__, #expr))

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/assert.c>
#elif defined(__KCC__)
#pragma import("assert");
#endif
#endif

#endif

#define static_assert   _Static_assert

#endif  /* KCC_ASSERT_H */
