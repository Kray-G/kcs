#ifndef KCC_STDDEF_H
#define KCC_STDDEF_H

#include <_builtin.h>
#define offsetof(T, M) ((size_t)&(((T*)0)->M))

typedef unsigned int wchar_t;
typedef long double max_align_t;

#endif  /* KCC_STDDEF_H */
