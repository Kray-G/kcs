#ifndef KCC__BUILTIN_H
#define KCC__BUILTIN_H

typedef unsigned long long  uint64_t;
#if defined(__KCC__)
typedef unsigned long       size_t;
typedef int                 ptrdiff_t;
#endif
#if !defined(NULL)
#define NULL                ((void*)0)
#endif

void __kcc_builtin_onstart(void);
void __kcc_builtin_onexit(void);

char* __kcc_builtin_acpdup(const char *src);
void __kcc_builtin_acpfree(void *p);

void *__kcc_builtin_loadlib(const char *name, const char *envname);
void __kcc_builtin_unloadlib(void *h);
void __kcc_builtin_reset_args(void);
int __kcc_builtin_add_arg_i(long long i);
int __kcc_builtin_add_arg_u(unsigned long long i);
int __kcc_builtin_add_arg_d(double d);
int __kcc_builtin_add_arg_s(char *s);
int __kcc_builtin_add_arg_p(void *p);
void __kcc_builtin_call(void *h, const char *name);
int __kcc_builtin_call_i(void *h, const char *name);
double __kcc_builtin_call_d(void *h, const char *name);
void *__kcc_builtin_call_p(void *h, const char *name);

int __kcc_builtin_putc(int ch);
int __kcc_builtin_printf_d(const char *fmt, int v);
int __kcc_builtin_printf_ld(const char *fmt, long v);
int __kcc_builtin_printf_f(const char *fmt, double v);
int __kcc_builtin_printf_lf(const char *fmt, long double v);
int __kcc_builtin_printf_p(const char *fmt, void *v);
int __kcc_builtin_printf_s(const char *fmt, char *v);
int __kcc_builtin_sprintf_d(const char *fmt, int v, char *buf);
int __kcc_builtin_sprintf_ld(const char *fmt, long v, char *buf);
int __kcc_builtin_sprintf_f(const char *fmt, double v, char *buf);
int __kcc_builtin_sprintf_lf(const char *fmt, long double v, char *buf);
int __kcc_builtin_sprintf_p(const char *fmt, void *v, char *buf);
int __kcc_builtin_sprintf_s(const char *fmt, char *v, char *buf);

size_t __kcc_builtin_strlen(const char *str);
char *__kcc_builtin_strcpy(char *dst, const char *src);
char *__kcc_builtin_strncpy(char *dst, const char *src, int n);
void *__kcc_builtin_memset(void *dst, int val, size_t len);
void *__kcc_builtin_memcpy(void *dst, void *src, size_t len);
void *__kcc_builtin_memmove(void *dst, void *src, size_t len);

int atexit(void (*kcc_atexit_func)(void));
void __kcc_call_atexit_funcs(void);

void __kcc_builtin_abort(void);
void __kcc_builtin_exit(int code);
void *__kcc_builtin_malloc(size_t size);
void *__kcc_builtin_realloc(void *ptr, size_t size);
void *__kcc_builtin_calloc(size_t count, size_t size);
void __kcc_builtin_free(void *p);

long __kcc_builtin_strtol(const char *s, char **endptr, int base);
long long __kcc_builtin_strtoll(const char *s, char **endptr, int base);
unsigned long __kcc_builtin_strtoul(const char *s, char **endptr, int base);
unsigned long long __kcc_builtin_strtoull(const char *s, char **endptr, int base);
float __kcc_builtin_strtof(const char *s, char **endptr);
double __kcc_builtin_strtod(const char *s, char **endptr);

int __kcc_builtin_system(const char *cmdline);

/* math.h */

double __kcc_builtin_math_acos(double x);
double __kcc_builtin_math_asin(double x);
double __kcc_builtin_math_atan(double x);
double __kcc_builtin_math_cos(double x);
double __kcc_builtin_math_sin(double x);
double __kcc_builtin_math_tan(double x);
double __kcc_builtin_math_cosh(double x);
double __kcc_builtin_math_sinh(double x);
double __kcc_builtin_math_tanh(double x);
double __kcc_builtin_math_exp(double x);
double __kcc_builtin_math_log(double x);
double __kcc_builtin_math_log10(double x);
double __kcc_builtin_math_sqrt(double x);
double __kcc_builtin_math_ceil(double x);
double __kcc_builtin_math_fabs(double x);
double __kcc_builtin_math_floor(double x);
double __kcc_builtin_math_pow(double x, double y);
double __kcc_builtin_math_atan2(double y, double x);
double __kcc_builtin_math_fmod(double x, double y);
double __kcc_builtin_math_frexp(double value, int *exp);
double __kcc_builtin_math_ldexp(double x, int exp);
double __kcc_builtin_math_modf(double value, double *iptr);

/* time.h */

uint64_t __kcc_builtin_time(uint64_t *timer);
void *__kcc_builtin_gmtime_init(uint64_t *timer, int is_gmtime);
int __kcc_builtin_gmtime(void *p, int type);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/_builtin.c>
#endif
#endif

#endif
