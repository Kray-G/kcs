#ifndef KCC_STDARG_H
#define KCC_STDARG_H

#include <_builtin.h>

#if defined(__KCC_JIT__)
typedef __builtin_va_list va_list;
#define va_start(list, arg) __builtin_va_start(list, arg)
#define va_arg(list, type) __builtin_va_arg(list, type)
#define va_end(list)
#if __STDC_VERSION__ >= 199901L
# define va_copy(dst, src) (*(dst) = *(src))
#endif
#else
typedef char* va_list;
#define va_start(ap, last)  ap = (char*)&last
#define va_arg(ap, typ)     (*(typ*)(ap -= (sizeof(typ) < 8 ? 8 : sizeof(typ))))
#define va_copy(dst, src)   dst = src
#define va_end(ap)
#endif

#endif
