#include <kcc.h>

#if !defined(EXTERNAL)
# define INTERNAL
# define EXTERNAL extern
#endif

#if defined(KCC_WINDOWS)
# define DLLEXPORT __declspec(dllexport)
#else
# define DLLEXPORT
#endif

#include "../vm.h"
#include "../vminstr.h"
#include "vmacpconv.h"
#include <lacc/array.h>
#include <kcc/dll.h>
#include <math.h>
#include <time.h>
#include <setjmp.h>


/* #define DEBUG_BUILTIN */
#if defined(DEBUG_BUILTIN)
#define CHECK_BUILTIN_ENTER() printf("in builtin: %s\n", __func__);
#define CHECK_BUILTIN_LEAVE() printf("out builtin: %s\n", __func__);
#else
#define CHECK_BUILTIN_ENTER()
#define CHECK_BUILTIN_LEAVE()
#endif

static int vm_call_builtin_onstart(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    STACK_TOPI() = 0ULL;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_onexit(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    STACK_TOPI() = 0ULL;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_abort(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    printf("aborted\n");
    exit(1);
}

static int vm_call_builtin_acpdup(uint8_t *stack, int64_t sp)
{
    #if defined(_WIN32) || defined(_WIN64)
    char *src = (char*)STACK_TOPI_OFFSET(-8);
    int len = len_utf82acp(src);
    char *dst = calloc(len+1, sizeof(char));
    STACK_TOPI() = (uint64_t)conv_utf82acp(dst, len, src);
    #else
    char *src = (char*)STACK_TOPI_OFFSET(-8);
    STACK_TOPI() = (uint64_t)src;
    #endif
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_acpfree(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    #if defined(_WIN32) || defined(_WIN64)
    void *ptr = (void*)STACK_TOPI_OFFSET(-8);
    free(ptr);
    #endif
    STACK_TOPI() = 0ULL;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_putc(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    int ch = (int)STACK_TOPI_OFFSET(-8);
    STACK_TOPI() = putc(ch, stdout);
    fflush(stdout);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_printf_d(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    uint64_t v = (uint64_t)STACK_TOPI_OFFSET(-16);
    STACK_TOPI() = printf(fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_printf_ld(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    if (*(fmt+1) == 'l') {
        uint64_t v = (uint64_t)STACK_TOPI_OFFSET(-16);
        if (*(fmt+2) == 'l') {
            STACK_TOPI() = printf(fmt, v);
        } else {
            char fmtbuf[] = { '%', 'l', 'l', *(fmt+2), 0 };
            STACK_TOPI() = printf(fmtbuf, v);
        }
    } else {
        int v = (int)STACK_TOPI_OFFSET(-16);
        STACK_TOPI() = printf(fmt, v);
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_printf_f(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    double v = (double)STACK_TOPD_OFFSET(-16);
    STACK_TOPI() = printf(fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_printf_lf(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    long double v = (long double)STACK_TOPLD_OFFSET(-24);
    STACK_TOPI() = printf(fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_printf_p(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    void *v = (void*)STACK_TOPI_OFFSET(-16);
    STACK_TOPI() = printf(fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_printf_s(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    char *v = (char*)STACK_TOPI_OFFSET(-16);
    #if defined(_WIN32) || defined(_WIN64)
    int len = len_utf82acp(v);
    char *vv = calloc(len+1, sizeof(char));
    STACK_TOPI() = printf(fmt, conv_utf82acp(vv, len, v));
    free(vv);
    #else
    STACK_TOPI() = printf(fmt, v);
    #endif
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_sprintf_d(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    uint64_t v = (uint64_t)STACK_TOPI_OFFSET(-16);
    char *dst = (char*)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = sprintf(dst, fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_sprintf_ld(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    char *dst = (char*)STACK_TOPI_OFFSET(-24);
    if (*(fmt+1) == 'l') {
        int64_t v = (int64_t)STACK_TOPI_OFFSET(-16);
        if (*(fmt+2) == 'l') {
            STACK_TOPI() = sprintf(dst, fmt, v);
        } else {
            char fmtbuf[] = { '%', 'l', 'l', *(fmt+2), 0 };
            STACK_TOPI() = sprintf(dst, fmtbuf, v);
        }
    } else {
        int v = (int)STACK_TOPI_OFFSET(-16);
        STACK_TOPI() = sprintf(dst, fmt, v);
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_sprintf_f(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    double v = (double)STACK_TOPD_OFFSET(-16);
    char *dst = (char*)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = sprintf(dst, fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_sprintf_lf(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    long double v = (long double)STACK_TOPLD_OFFSET(-24);
    char *dst = (char*)STACK_TOPI_OFFSET(-32);
    STACK_TOPI() = sprintf(dst, fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_sprintf_p(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    void *v = (void*)STACK_TOPI_OFFSET(-16);
    char *dst = (char*)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = sprintf(dst, fmt, v);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_sprintf_s(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *fmt = (const char*)STACK_TOPI_OFFSET(-8);
    char *v = (char*)STACK_TOPI_OFFSET(-16);
    char *dst = (char*)STACK_TOPI_OFFSET(-24);
    #if defined(_WIN32) || defined(_WIN64)
    int len = len_utf82acp(v);
    char *vv = calloc(len+1, sizeof(char));
    STACK_TOPI() = sprintf(dst, fmt, conv_utf82acp(vv, len, v));
    free(vv);
    #else
    STACK_TOPI() = sprintf(dst, fmt, v);
    #endif
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strlen(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *str = (const char*)STACK_TOPI_OFFSET(-8);
    STACK_TOPI() = strlen(str);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strcpy(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    char *dst = (char*)STACK_TOPI_OFFSET(-8);
    const char *src = (const char*)STACK_TOPI_OFFSET(-16);
    STACK_TOPI() = (uint64_t)strcpy(dst, src);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strncpy(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    char *dst = (char*)STACK_TOPI_OFFSET(-8);
    const char *src = (const char*)STACK_TOPI_OFFSET(-16);
    size_t len = (size_t)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (uint64_t)strncpy(dst, src, len);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_memset(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *dst = (void*)STACK_TOPI_OFFSET(-8);
    int val = (int)STACK_TOPI_OFFSET(-16);
    size_t len = (size_t)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (uint64_t)memset(dst, val, len);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_memcpy(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *dst = (void*)STACK_TOPI_OFFSET(-8);
    const void *src = (const void*)STACK_TOPI_OFFSET(-16);
    size_t len = (size_t)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (uint64_t)memcpy(dst, src, len);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_memmove(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *dst = (void*)STACK_TOPI_OFFSET(-8);
    const void *src = (const void*)STACK_TOPI_OFFSET(-16);
    size_t len = (size_t)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (uint64_t)memmove(dst, src, len);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_exit(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    int code = (int)STACK_TOPI_OFFSET(-8);
    CHECK_BUILTIN_LEAVE();
    exit(code);
    return 0;   /* sp += 0; */
}

static int vm_call_builtin_malloc(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    size_t size = (size_t)STACK_TOPI_OFFSET(-8);
    STACK_TOPI() = (uint64_t)malloc(size);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_calloc(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    size_t count = (size_t)STACK_TOPI_OFFSET(-8);
    size_t size = (size_t)STACK_TOPI_OFFSET(-16);
    STACK_TOPI() = (uint64_t)calloc(count, size);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_realloc(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *ptr = (void*)STACK_TOPI_OFFSET(-8);
    size_t size = (size_t)STACK_TOPI_OFFSET(-16);
    STACK_TOPI() = (uint64_t)realloc(ptr, size);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_free(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *ptr = (void*)STACK_TOPI_OFFSET(-8);
    free(ptr);
    STACK_TOPI() = 0ULL;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

/* math.h */

#define KCC_DEF_MATH1(name) \
    static int vm_call_builtin_math_ ## name(uint8_t *stack, int64_t sp)\
    {\
        CHECK_BUILTIN_ENTER();\
        double v1 = STACK_TOPD_OFFSET(-8);\
        STACK_TOPD() = name(v1);\
        CHECK_BUILTIN_LEAVE();\
        return 8;   /* sp += 8; */\
    }\
    /**/
#define KCC_DEF_MATH2(name) \
    static int vm_call_builtin_math_ ## name(uint8_t *stack, int64_t sp)\
    {\
        CHECK_BUILTIN_ENTER();\
        double v1 = STACK_TOPD_OFFSET(-8);\
        double v2 = STACK_TOPD_OFFSET(-16);\
        STACK_TOPD() = name(v1, v2);\
        CHECK_BUILTIN_LEAVE();\
        return 8;   /* sp += 8; */\
    }\
    /**/

KCC_DEF_MATH1(acos)
KCC_DEF_MATH1(asin)
KCC_DEF_MATH1(atan)
KCC_DEF_MATH2(atan2)
KCC_DEF_MATH1(cos)
KCC_DEF_MATH1(sin)
KCC_DEF_MATH1(tan)
KCC_DEF_MATH1(cosh)
KCC_DEF_MATH1(sinh)
KCC_DEF_MATH1(tanh)
KCC_DEF_MATH1(exp)
KCC_DEF_MATH1(log)
KCC_DEF_MATH1(log10)
KCC_DEF_MATH2(pow)
KCC_DEF_MATH1(sqrt)
KCC_DEF_MATH1(ceil)
KCC_DEF_MATH1(fabs)
KCC_DEF_MATH1(floor)
KCC_DEF_MATH2(fmod)

static int vm_call_builtin_math_frexp(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    double v1 = STACK_TOPD_OFFSET(-8);
    int *v2 = (int *)STACK_TOPI_OFFSET(-16);
    STACK_TOPD() = frexp(v1, v2);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_math_ldexp(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    double v1 = STACK_TOPD_OFFSET(-8);
    int v2 = STACK_TOPD_OFFSET(-16);
    STACK_TOPD() = ldexp(v1, v2);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_math_modf(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    double v1 = STACK_TOPD_OFFSET(-8);
    double *v2 = (double *)STACK_TOPI_OFFSET(-16);
    STACK_TOPD() = modf(v1, v2);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

/* dlopen/dlsym/dlclose */
void *load_library(const char *name, const char *envname);
void *get_function(void *h, const char *name);
void unload_library(void *h);
int g_lib_argc;
arg_type_t g_lib_argv[C_MAX_ARGS] = {0};

static int vm_call_builtin_loadlib(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *pathname = (const char*)STACK_TOPI_OFFSET(-8);
    const char *envname = (const char*)STACK_TOPI_OFFSET(-16);
    void *handle = (void *)load_library(pathname, envname);
    STACK_TOPI() = (uint64_t)handle;
    int (*f)(int, arg_type_t*) = (int (*)(int, arg_type_t*))get_function(handle, "initialize");
    if (f) {
        int r = f(0, g_lib_argv);
        if (r) {
            STACK_TOPI() = 0;
        }
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_unloadlib(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *handle = (void*)STACK_TOPI_OFFSET(-8);
    void (*f)(int, arg_type_t*) = (void (*)(int, arg_type_t*))get_function(handle, "finalize");
    if (f) {
        f(0, g_lib_argv);
    }
    unload_library(handle);
    STACK_TOPI() = 0ULL;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_reset_args(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    g_lib_argc = 0;
    STACK_TOPI() = 0ULL;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_add_arg_i(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_INT;
        g_lib_argv[g_lib_argc].value.i = (int64_t)STACK_TOPI_OFFSET(-8);
        ++g_lib_argc;
        STACK_TOPI() = 1ULL;
    } else {
        STACK_TOPI() = 0ULL;
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_add_arg_u(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_UINT;
        g_lib_argv[g_lib_argc].value.u = (uint64_t)STACK_TOPI_OFFSET(-8);
        ++g_lib_argc;
        STACK_TOPI() = 1ULL;
    } else {
        STACK_TOPI() = 0ULL;
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_add_arg_d(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_DBL;
        g_lib_argv[g_lib_argc].value.d = (double)STACK_TOPD_OFFSET(-8);
        ++g_lib_argc;
        STACK_TOPI() = 1ULL;
    } else {
        STACK_TOPI() = 0ULL;
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_add_arg_s(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_STR;
        g_lib_argv[g_lib_argc].value.s = (char *)STACK_TOPI_OFFSET(-8);
        ++g_lib_argc;
        STACK_TOPI() = 1ULL;
    } else {
        STACK_TOPI() = 0ULL;
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_add_arg_p(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_PTR;
        g_lib_argv[g_lib_argc].value.p = (void *)STACK_TOPI_OFFSET(-8);
        ++g_lib_argc;
        STACK_TOPI() = 1ULL;
    } else {
        STACK_TOPI() = 0ULL;
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_call(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *handle = (void*)STACK_TOPI_OFFSET(-8);
    const char *funcname = (const char*)STACK_TOPI_OFFSET(-16);
    void (*f)(int, arg_type_t*) = (void (*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        f(g_lib_argc, g_lib_argv);
    } else {
        vm_call_builtin_abort(NULL, 0);
    }
    STACK_TOPI() = 0ULL;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_call_i(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *handle = (void*)STACK_TOPI_OFFSET(-8);
    const char *funcname = (const char*)STACK_TOPI_OFFSET(-16);
    int (*f)(int, arg_type_t*) = (int (*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        STACK_TOPI() = (uint64_t)f(g_lib_argc, g_lib_argv);
    } else {
        vm_call_builtin_abort(NULL, 0);
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_call_d(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *handle = (void*)STACK_TOPI_OFFSET(-8);
    const char *funcname = (const char*)STACK_TOPI_OFFSET(-16);
    double (*f)(int, arg_type_t*) = (double (*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        STACK_TOPD() = f(g_lib_argc, g_lib_argv);
    } else {
        vm_call_builtin_abort(NULL, 0);
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_call_p(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    void *handle = (void*)STACK_TOPI_OFFSET(-8);
    const char *funcname = (const char*)STACK_TOPI_OFFSET(-16);
    void *(*f)(int, arg_type_t*) = (void * (*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        STACK_TOPI() = (uint64_t)f(g_lib_argc, g_lib_argv);
    } else {
        vm_call_builtin_abort(NULL, 0);
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strtol(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *s = (const char*)STACK_TOPI_OFFSET(-8);
    char **endptr = (char**)STACK_TOPI_OFFSET(-16);
    int base = (int)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (int64_t)strtol(s, endptr, base);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strtoul(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *s = (const char*)STACK_TOPI_OFFSET(-8);
    char **endptr = (char**)STACK_TOPI_OFFSET(-16);
    int base = (int)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (uint64_t)strtoul(s, endptr, base);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strtoll(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *s = (const char*)STACK_TOPI_OFFSET(-8);
    char **endptr = (char**)STACK_TOPI_OFFSET(-16);
    int base = (int)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (int64_t)strtoll(s, endptr, base);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strtoull(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *s = (const char*)STACK_TOPI_OFFSET(-8);
    char **endptr = (char**)STACK_TOPI_OFFSET(-16);
    int base = (int)STACK_TOPI_OFFSET(-24);
    STACK_TOPI() = (uint64_t)strtoll(s, endptr, base);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strtof(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *s = (const char*)STACK_TOPI_OFFSET(-8);
    char **endptr = (char**)STACK_TOPI_OFFSET(-16);
    STACK_TOPD() = (double)strtof(s, endptr);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_strtod(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *s = (const char*)STACK_TOPI_OFFSET(-8);
    char **endptr = (char**)STACK_TOPI_OFFSET(-16);
    STACK_TOPD() = (double)strtod(s, endptr);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_system(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    const char *s = (const char*)STACK_TOPI_OFFSET(-8);
    STACK_TOPI() = (int64_t)system(s);
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_time(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    uint64_t *p = (uint64_t*)STACK_TOPI_OFFSET(-8);
    time_t t;
    if (p) {
        time(&t);
    } else {
        t = time(NULL);
    }
    STACK_TOPI() = (uint64_t)t;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_gmtime_init(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    time_t *t = (time_t *)STACK_TOPI_OFFSET(-8);
    int is_gmtime = (int)STACK_TOPI_OFFSET(-16);
    if (is_gmtime) {
        struct tm *r = gmtime(t);
        STACK_TOPI() = (uint64_t)r;
    } else {
        struct tm *r = localtime(t);
        STACK_TOPI() = (uint64_t)r;
    }
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

static int vm_call_builtin_gmtime(uint8_t *stack, int64_t sp)
{
    CHECK_BUILTIN_ENTER();
    struct tm *t = (struct tm*)STACK_TOPI_OFFSET(-8);
    int flag = (int)STACK_TOPI_OFFSET(-16);
    uint64_t r;
    switch (flag) {
    case 0:
        r = t->tm_sec;
        break;
    case 1:
        r = t->tm_min;
        break;
    case 2:
        r = t->tm_hour;
        break;
    case 3:
        r = t->tm_mday;
        break;
    case 4:
        r = t->tm_mon;
        break;
    case 5:
        r = t->tm_year;
        break;
    case 6:
        r = t->tm_wday;
        break;
    case 7:
        r = t->tm_yday;
        break;
    case 8:
        r = t->tm_isdst;
        break;
    }
    STACK_TOPI() = (uint64_t)r;
    CHECK_BUILTIN_LEAVE();
    return 8;   /* sp += 8; */
}

struct vm_builtin_def_t {
    const char *name;
    vm_builtin_t func;
} vm_builtin[] = {
    { "__kcc_dummy",                NULL                        },
    { "__kcc_builtin_onstart",      vm_call_builtin_onstart     },
    { "__kcc_builtin_onexit",       vm_call_builtin_onexit      },
    { "__kcc_builtin_abort",        vm_call_builtin_abort       },
    { "__kcc_builtin_acpdup",       vm_call_builtin_acpdup      },
    { "__kcc_builtin_acpfree",      vm_call_builtin_acpfree     },
    { "__kcc_builtin_putc",         vm_call_builtin_putc        },
    { "__kcc_builtin_printf_d",     vm_call_builtin_printf_d    },
    { "__kcc_builtin_printf_ld",    vm_call_builtin_printf_ld   },
    { "__kcc_builtin_printf_f",     vm_call_builtin_printf_f    },
    { "__kcc_builtin_printf_lf",    vm_call_builtin_printf_lf   },
    { "__kcc_builtin_printf_p",     vm_call_builtin_printf_p    },
    { "__kcc_builtin_printf_s",     vm_call_builtin_printf_s    },
    { "__kcc_builtin_sprintf_d",    vm_call_builtin_sprintf_d   },
    { "__kcc_builtin_sprintf_ld",   vm_call_builtin_sprintf_ld  },
    { "__kcc_builtin_sprintf_f",    vm_call_builtin_sprintf_f   },
    { "__kcc_builtin_sprintf_lf",   vm_call_builtin_sprintf_lf  },
    { "__kcc_builtin_sprintf_p",    vm_call_builtin_sprintf_p   },
    { "__kcc_builtin_sprintf_s",    vm_call_builtin_sprintf_s   },
    { "__kcc_builtin_strlen",       vm_call_builtin_strlen      },
    { "__kcc_builtin_strcpy",       vm_call_builtin_strcpy      },
    { "__kcc_builtin_strncpy",      vm_call_builtin_strncpy     },
    { "__kcc_builtin_memset",       vm_call_builtin_memset      },
    { "__kcc_builtin_memcpy",       vm_call_builtin_memcpy      },
    { "__kcc_builtin_memmove",      vm_call_builtin_memmove     },
    { "__kcc_builtin_exit",         vm_call_builtin_exit        },
    { "__kcc_builtin_malloc",       vm_call_builtin_malloc      },
    { "__kcc_builtin_calloc",       vm_call_builtin_calloc      },
    { "__kcc_builtin_realloc",      vm_call_builtin_realloc     },
    { "__kcc_builtin_free",         vm_call_builtin_free        },
    { "__kcc_builtin_strtol",       vm_call_builtin_strtol      },
    { "__kcc_builtin_strtoul",      vm_call_builtin_strtoul     },
    { "__kcc_builtin_strtoll",      vm_call_builtin_strtoll     },
    { "__kcc_builtin_strtoull",     vm_call_builtin_strtoull    },
    { "__kcc_builtin_strtof",       vm_call_builtin_strtof      },
    { "__kcc_builtin_strtod",       vm_call_builtin_strtod      },
    { "__kcc_builtin_system",       vm_call_builtin_system      },

    { "__kcc_builtin_math_acos",    vm_call_builtin_math_acos   },
    { "__kcc_builtin_math_asin",    vm_call_builtin_math_asin   },
    { "__kcc_builtin_math_atan",    vm_call_builtin_math_atan   },
    { "__kcc_builtin_math_atan2",   vm_call_builtin_math_atan2  },
    { "__kcc_builtin_math_cos",     vm_call_builtin_math_cos    },
    { "__kcc_builtin_math_sin",     vm_call_builtin_math_sin    },
    { "__kcc_builtin_math_tan",     vm_call_builtin_math_tan    },
    { "__kcc_builtin_math_cosh",    vm_call_builtin_math_cosh   },
    { "__kcc_builtin_math_sinh",    vm_call_builtin_math_sinh   },
    { "__kcc_builtin_math_tanh",    vm_call_builtin_math_tanh   },
    { "__kcc_builtin_math_exp",     vm_call_builtin_math_exp    },
    { "__kcc_builtin_math_frexp",   vm_call_builtin_math_frexp  },
    { "__kcc_builtin_math_ldexp",   vm_call_builtin_math_ldexp  },
    { "__kcc_builtin_math_log",     vm_call_builtin_math_log    },
    { "__kcc_builtin_math_log10",   vm_call_builtin_math_log10  },
    { "__kcc_builtin_math_modf",    vm_call_builtin_math_modf   },
    { "__kcc_builtin_math_pow",     vm_call_builtin_math_pow    },
    { "__kcc_builtin_math_sqrt",    vm_call_builtin_math_sqrt   },
    { "__kcc_builtin_math_ceil",    vm_call_builtin_math_ceil   },
    { "__kcc_builtin_math_fabs",    vm_call_builtin_math_fabs   },
    { "__kcc_builtin_math_floor",   vm_call_builtin_math_floor  },
    { "__kcc_builtin_math_fmod",    vm_call_builtin_math_fmod   },

    { "__kcc_builtin_loadlib",      vm_call_builtin_loadlib     },
    { "__kcc_builtin_unloadlib",    vm_call_builtin_unloadlib   },
    { "__kcc_builtin_reset_args",   vm_call_builtin_reset_args  },
    { "__kcc_builtin_add_arg_i",    vm_call_builtin_add_arg_i   },
    { "__kcc_builtin_add_arg_u",    vm_call_builtin_add_arg_u   },
    { "__kcc_builtin_add_arg_d",    vm_call_builtin_add_arg_d   },
    { "__kcc_builtin_add_arg_s",    vm_call_builtin_add_arg_s   },
    { "__kcc_builtin_add_arg_p",    vm_call_builtin_add_arg_p   },
    { "__kcc_builtin_call",         vm_call_builtin_call        },
    { "__kcc_builtin_call_i",       vm_call_builtin_call_i      },
    { "__kcc_builtin_call_d",       vm_call_builtin_call_d      },
    { "__kcc_builtin_call_p",       vm_call_builtin_call_p      },

    { "__kcc_builtin_time",         vm_call_builtin_time        },
    { "__kcc_builtin_gmtime_init",  vm_call_builtin_gmtime_init },
    { "__kcc_builtin_gmtime",       vm_call_builtin_gmtime      },
};

DLLEXPORT int vm_get_builtin_index(const char *name)
{
    for (int i = 1; i < sizeof(vm_builtin)/sizeof(vm_builtin[0]); ++i) {
        if (strcmp(name, vm_builtin[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

DLLEXPORT vm_builtin_t vm_get_builtin_by_name(const char *name)
{
    for (int i = 1; i < sizeof(vm_builtin)/sizeof(vm_builtin[0]); ++i) {
        if (strcmp(name, vm_builtin[i].name) == 0) {
            return vm_builtin[i].func;
        }
    }
    return NULL;
}

DLLEXPORT vm_builtin_t vm_get_builtin_by_index(int index)
{
    if (index < 1 || sizeof(vm_builtin)/sizeof(vm_builtin[0]) <= index) {
        return NULL;
    }
    return vm_builtin[index].func;
}

DLLEXPORT const char *vm_get_builtin_name_by_index(int index)
{
    if (index < 1 || sizeof(vm_builtin)/sizeof(vm_builtin[0]) <= index) {
        return NULL;
    }
    return vm_builtin[index].name;
}

