#include <kcs.h>

#if !defined(EXTERNAL)
# define INTERNAL
# define EXTERNAL extern
#endif

#if defined(KCC_WINDOWS)
# define DLLEXPORT __declspec(dllexport)
#else
# define DLLEXPORT
#endif

#include "../jit.h"
#include "../../vm/builtin/vmacpconv.h"
#include <lacc/array.h>
#include <kcs/dll.h>
#include <math.h>
#include <time.h>

/* #define DEBUG_BUILTIN */
#if defined(DEBUG_BUILTIN)
#define CHECK_BUILTIN_ENTER() printf("in builtin: %s\n", __func__);
#define CHECK_BUILTIN_LEAVE() printf("out builtin: %s\n", __func__);
#else
#define CHECK_BUILTIN_ENTER()
#define CHECK_BUILTIN_LEAVE()
#endif

static void jit_call_builtin_onstart(void)
{
}

static void jit_call_builtin_onexit(void)
{
}

static void jit_call_builtin_abort(void)
{
    printf("aborted\n");
    exit(1);
}

static char *jit_call_builtin_acpdup(char *src)
{
    #if defined(_WIN32) || defined(_WIN64)
    int len = len_utf82acp(src);
    char *dst = calloc(len+1, sizeof(char));
    return conv_utf82acp(dst, len, src);
    #else
    return src;
    #endif
}

static void jit_call_builtin_acpfree(void *ptr)
{
    #if defined(_WIN32) || defined(_WIN64)
    free(ptr);
    #endif
}

static int jit_call_builtin_putc(int ch)
{
    return putc(ch, stdout);
}

static int jit_call_builtin_printf_d(const char *fmt, int64_t v)
{
    return printf(fmt, v);
}

static int jit_call_builtin_printf_ld(const char *fmt, int64_t v)
{
    int i = 0;
    char fmtbuf[64] = {0};
    char *p = fmtbuf;
    for ( ; i < 64; ++i) {
        if (*fmt != '%' && (*fmt < '0' || '9' < *fmt)) {
            break;
        }
        *p++ = *fmt++;
    }
    *p++ = 'l';
    *p++ = 'l';
    for ( ; i < 64; ++i, ++fmt) {
        if (*fmt != 'l') {
            *p = *fmt;
            break;
        }
    }
    return printf(fmtbuf, v);
}

static int jit_call_builtin_printf_f(const char *fmt, double v)
{
    return printf(fmt, v);
}

static int jit_call_builtin_printf_lf(const char *fmt, long double v)
{
    return printf(fmt, v);
}

static int jit_call_builtin_printf_p(const char *fmt, void *v)
{
    return printf(fmt, v);
}

static int jit_call_builtin_printf_s(const char *fmt, char *v)
{
    #if defined(_WIN32) || defined(_WIN64)
    int len = len_utf82acp(v);
    char *vv = calloc(len+1, sizeof(char));
    int r = printf(fmt, conv_utf82acp(vv, len, v));
    free(vv);
    return r;
    #else
    return printf(fmt, v);
    #endif
}

static int jit_call_builtin_sprintf_d(const char *fmt, int64_t v, char *dst)
{
    return sprintf(dst, fmt, v);
}

static int jit_call_builtin_sprintf_ld(const char *fmt, int64_t v, char *dst)
{
    int i = 0;
    char fmtbuf[64] = {0};
    char *p = fmtbuf;
    for ( ; i < 64; ++i) {
        if (*fmt != '%' && (*fmt < '0' || '9' < *fmt)) {
            break;
        }
        *p++ = *fmt++;
    }
    *p++ = 'l';
    *p++ = 'l';
    for ( ; i < 64; ++i, ++fmt) {
        if (*fmt != 'l') {
            *p = *fmt;
            break;
        }
    }
    return sprintf(dst, fmtbuf, v);
}

static int jit_call_builtin_sprintf_f(const char *fmt, double v, char *dst)
{
    return sprintf(dst, fmt, v);
}

static int jit_call_builtin_sprintf_lf(const char *fmt, long double v, char *dst)
{
    return sprintf(dst, fmt, v);
}

static int jit_call_builtin_sprintf_p(const char *fmt, void *v, char *dst)
{
    return sprintf(dst, fmt, v);
}

static int jit_call_builtin_sprintf_s(const char *fmt, char *v, char *dst)
{
    #if defined(_WIN32) || defined(_WIN64)
    int len = len_utf82acp(v);
    char *vv = calloc(len+1, sizeof(char));
    int r = sprintf(dst, fmt, conv_utf82acp(vv, len, v));
    free(vv);
    return r;
    #else
    return sprintf(dst, fmt, v);
    #endif
}

static int jit_call_builtin_strlen(const char *str)
{
    return strlen(str);
}

static char *jit_call_builtin_strcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
}

static char *jit_call_builtin_strncpy(char *dst, const char *src, size_t len)
{
    return strncpy(dst, src, len);
}

static void *jit_call_builtin_memset(void *dst, int val, size_t size)
{
    return memset(dst, val, size);
}

static void *jit_call_builtin_memcpy(void *dst, void *src, size_t size)
{
    return memcpy(dst, src, size);
}

static void *jit_call_builtin_memmove(void *dst, void *src, size_t size)
{
    return memmove(dst, src, size);
}

static void jit_call_builtin_exit(int status)
{
    exit(status);
}

/* dlopen/dlsym/dlclose */
void *load_library(const char *name, const char *envname);
void *get_function(void *h, const char *name);
void unload_library(void *h);
int g_lib_argc;
arg_type_t g_lib_argv[C_MAX_ARGS] = {0};

static void *jit_call_builtin_load_library(const char *name, const char *envname)
{
    void *h = load_library(name, envname);
    int (*f)(int, arg_type_t*) = (int (*)(int, arg_type_t*))get_function(h, "initialize");
    if (f) {
        int r = f(0, g_lib_argv);
        if (r) {
            return NULL;
        }
    }
    return h;
}

static void jit_call_builtin_unload_library(void *h)
{
    void (*f)(int, arg_type_t*) = (void (*)(int, arg_type_t*))get_function(h, "finalize");
    if (f) {
        f(0, g_lib_argv);
    }
    unload_library(h);
}

static void jit_call_builtin_reset_args(void)
{
    g_lib_argc = 0;
}

static int jit_call_builtin_add_arg_i(int64_t i)
{
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_INT;
        g_lib_argv[g_lib_argc].value.i = i;
        ++g_lib_argc;
        return 1;
    } else {
        return 0;
    }
}

static int jit_call_builtin_add_arg_u(uint64_t u)
{
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_UINT;
        g_lib_argv[g_lib_argc].value.u = u;
        ++g_lib_argc;
        return 1;
    } else {
        return 0;
    }
}

static int jit_call_builtin_add_arg_d(double d)
{
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_DBL;
        g_lib_argv[g_lib_argc].value.d = d;
        ++g_lib_argc;
        return 1;
    } else {
        return 0;
    }
}

static int jit_call_builtin_add_arg_s(char *s)
{
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_STR;
        g_lib_argv[g_lib_argc].value.s = s;
        ++g_lib_argc;
        return 1;
    } else {
        return 0;
    }
}

static int jit_call_builtin_add_arg_p(void *p)
{
    if (g_lib_argc < C_MAX_ARGS) {
        g_lib_argv[g_lib_argc].type = C_PTR;
        g_lib_argv[g_lib_argc].value.p = p;
        ++g_lib_argc;
        return 1;
    } else {
        return 0;
    }
}

static void jit_call_builtin_call(void *handle, const char *funcname)
{
    void (*f)(int, arg_type_t*) = (void (*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        f(g_lib_argc, g_lib_argv);
    } else {
        jit_call_builtin_abort();
    }
}

static int jit_call_builtin_call_i(void *handle, const char *funcname)
{
    int (*f)(int, arg_type_t*) = (int (*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        return f(g_lib_argc, g_lib_argv);
    } else {
        jit_call_builtin_abort();
    }
    return 0;
}

static double jit_call_builtin_call_d(void *handle, const char *funcname)
{
    double (*f)(int, arg_type_t*) = (double (*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        return f(g_lib_argc, g_lib_argv);
    } else {
        jit_call_builtin_abort();
    }
    return 0.0;
}

static void *jit_call_builtin_call_p(void *handle, const char *funcname)
{
    void *(*f)(int, arg_type_t*) = (void *(*)(int, arg_type_t*))get_function(handle, funcname);
    if (f) {
        return f(g_lib_argc, g_lib_argv);
    } else {
        jit_call_builtin_abort();
    }
    return NULL;
}

static uint64_t jit_call_builtin_time(uint64_t *timer)
{
    if (timer) {
        time_t t;
        time(&t);
        *timer = (uint64_t)t;
        return *timer;
    }
    time_t t = time(NULL);
    return (uint64_t)t;
}

static struct tm *jit_call_builtin_gmtime_init(uint64_t *timer, int is_gmtime)
{
    if (is_gmtime) {
        return gmtime(timer);
    }
    return localtime(timer);
}

static uint64_t jit_call_builtin_gmtime(struct tm *t, int flag)
{
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
    return r;
}

static uint64_t jit_call_builtin_clock(void)
{
    return (uint64_t)clock();
}

static uint64_t jit_call_builtin_clocks_per_sec(void)
{
    return (uint64_t)CLOCKS_PER_SEC;
}

struct jit_builtin_def_t {
    const char *name;
    void *func;
    uint8_t args;
    uint8_t flbit;
} vm_builtin[] = {
    { "__kcc_dummy",                NULL                                            },
    { "__kcc_builtin_onstart",      jit_call_builtin_onstart,           0,  0x00,   },
    { "__kcc_builtin_onexit",       jit_call_builtin_onexit,            0,  0x00,   },
    { "__kcc_builtin_abort",        jit_call_builtin_abort,             0,  0x00,   },
    { "__kcc_builtin_acpdup",       jit_call_builtin_acpdup,            1,  0x00,   },
    { "__kcc_builtin_acpfree",      jit_call_builtin_acpfree,           1,  0x00,   },
    { "__kcc_builtin_putc",         jit_call_builtin_putc,              1,  0x00,   },
    { "__kcc_builtin_printf_d",     jit_call_builtin_printf_d,          2,  0x00,   },
    { "__kcc_builtin_printf_ld",    jit_call_builtin_printf_ld,         2,  0x00,   },
    { "__kcc_builtin_printf_f",     jit_call_builtin_printf_f,          2,  0x02,   },
    { "__kcc_builtin_printf_lf",    jit_call_builtin_printf_lf,         2,  0x20,   },
    { "__kcc_builtin_printf_p",     jit_call_builtin_printf_p,          2,  0x00,   },
    { "__kcc_builtin_printf_s",     jit_call_builtin_printf_s,          2,  0x00,   },
    { "__kcc_builtin_sprintf_d",    jit_call_builtin_sprintf_d,         3,  0x00,   },
    { "__kcc_builtin_sprintf_ld",   jit_call_builtin_sprintf_ld,        3,  0x00,   },
    { "__kcc_builtin_sprintf_f",    jit_call_builtin_sprintf_f,         3,  0x02,   },
    { "__kcc_builtin_sprintf_lf",   jit_call_builtin_sprintf_lf,        3,  0x20,   },
    { "__kcc_builtin_sprintf_p",    jit_call_builtin_sprintf_p,         3,  0x00,   },
    { "__kcc_builtin_sprintf_s",    jit_call_builtin_sprintf_s,         3,  0x00,   },
    { "__kcc_builtin_strlen",       jit_call_builtin_strlen,            1,  0x00,   },
    { "__kcc_builtin_strcpy",       jit_call_builtin_strcpy,            2,  0x00,   },
    { "__kcc_builtin_strncpy",      jit_call_builtin_strncpy,           3,  0x00,   },
    { "__kcc_builtin_memset",       jit_call_builtin_memset,            3,  0x00,   },
    { "__kcc_builtin_memcpy",       jit_call_builtin_memcpy,            3,  0x00,   },
    { "__kcc_builtin_memmove",      jit_call_builtin_memmove,           3,  0x00,   },
    { "__kcc_builtin_exit",         jit_call_builtin_exit,              1,  0x00,   },
    { "__kcc_builtin_malloc",       malloc,                             1,  0x00,   },
    { "__kcc_builtin_realloc",      realloc,                            2,  0x00,   },
    { "__kcc_builtin_calloc",       calloc,                             2,  0x00,   },
    { "__kcc_builtin_free",         free,                               1,  0x00,   },
    { "__kcc_builtin_strtol",       strtol,                             3,  0x00,   },
    { "__kcc_builtin_strtoul",      strtoul,                            3,  0x00,   },
    { "__kcc_builtin_strtoll",      strtoll,                            3,  0x00,   },
    { "__kcc_builtin_strtoull",     strtoull,                           3,  0x00,   },
    { "__kcc_builtin_strtof",       strtof,                             2,  0x00,   },
    { "__kcc_builtin_strtod",       strtod,                             2,  0x00,   },
    { "__kcc_builtin_system",       system,                             1,  0x00,   },

    { "__kcc_builtin_math_acos",    acos,                               1,  0x01,   },
    { "__kcc_builtin_math_asin",    asin,                               1,  0x01,   },
    { "__kcc_builtin_math_atan",    atan,                               1,  0x01,   },
    { "__kcc_builtin_math_atan2",   atan2,                              2,  0x03,   },
    { "__kcc_builtin_math_cos",     cos,                                1,  0x01,   },
    { "__kcc_builtin_math_sin",     sin,                                1,  0x01,   },
    { "__kcc_builtin_math_tan",     tan,                                1,  0x01,   },
    { "__kcc_builtin_math_cosh",    cosh,                               1,  0x01,   },
    { "__kcc_builtin_math_sinh",    sinh,                               1,  0x01,   },
    { "__kcc_builtin_math_tanh",    tanh,                               1,  0x01,   },
    { "__kcc_builtin_math_exp",     exp,                                1,  0x01,   },
    { "__kcc_builtin_math_log",     log,                                1,  0x01,   },
    { "__kcc_builtin_math_log10",   log10,                              1,  0x01,   },
    { "__kcc_builtin_math_pow",     pow,                                2,  0x03,   },
    { "__kcc_builtin_math_sqrt",    sqrt,                               1,  0x01,   },
    { "__kcc_builtin_math_ceil",    ceil,                               1,  0x01,   },
    { "__kcc_builtin_math_fabs",    fabs,                               1,  0x01,   },
    { "__kcc_builtin_math_floor",   floor,                              1,  0x01,   },
    { "__kcc_builtin_math_fmod",    fmod,                               2,  0x03,   },
    // { "__kcc_builtin_math_frexp",   jit_call_builtin_math_frexp },
    // { "__kcc_builtin_math_ldexp",   jit_call_builtin_math_ldexp },
    // { "__kcc_builtin_math_modf",    jit_call_builtin_math_modf  },

    { "__kcc_builtin_loadlib",      jit_call_builtin_load_library,      2,  0x00,   },
    { "__kcc_builtin_unloadlib",    jit_call_builtin_unload_library,    1,  0x00,   },
    { "__kcc_builtin_reset_args",   jit_call_builtin_reset_args,        0,  0x00    },
    { "__kcc_builtin_add_arg_i",    jit_call_builtin_add_arg_i,         1,  0x00    },
    { "__kcc_builtin_add_arg_u",    jit_call_builtin_add_arg_u,         1,  0x00    },
    { "__kcc_builtin_add_arg_d",    jit_call_builtin_add_arg_d,         1,  0x01    },
    { "__kcc_builtin_add_arg_s",    jit_call_builtin_add_arg_s,         1,  0x00    },
    { "__kcc_builtin_add_arg_p",    jit_call_builtin_add_arg_p,         1,  0x00    },
    { "__kcc_builtin_call",         jit_call_builtin_call,              2,  0x00    },
    { "__kcc_builtin_call_i",       jit_call_builtin_call_i,            2,  0x00    },
    { "__kcc_builtin_call_d",       jit_call_builtin_call_d,            2,  0x00    },
    { "__kcc_builtin_call_p",       jit_call_builtin_call_p,            2,  0x00    },

    { "__kcc_builtin_time",         time,                               1,  0x00    },
    { "__kcc_builtin_gmtime_init",  jit_call_builtin_gmtime_init,       2,  0x00    },
    { "__kcc_builtin_gmtime",       jit_call_builtin_gmtime,            2,  0x00    },
    { "__kcc_builtin_clock",        jit_call_builtin_clock,             0,  0x00    },
    { "__kcc_builtin_clocks_ps",    jit_call_builtin_clocks_per_sec,    0,  0x00    },
};

DLLEXPORT int jit_get_builtin_index(const char *name)
{
    for (int i = 1; i < sizeof(vm_builtin)/sizeof(vm_builtin[0]); ++i) {
        if (strcmp(name, vm_builtin[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

DLLEXPORT void *jit_get_builtin_by_name(const char *name)
{
    for (int i = 1; i < sizeof(vm_builtin)/sizeof(vm_builtin[0]); ++i) {
        if (strcmp(name, vm_builtin[i].name) == 0) {
            return vm_builtin[i].func;
        }
    }
    return NULL;
}

DLLEXPORT void *jit_get_builtin_by_index(int index)
{
    if (index < 1 || sizeof(vm_builtin)/sizeof(vm_builtin[0]) <= index) {
        return NULL;
    }
    return vm_builtin[index].func;
}

DLLEXPORT const char *jit_get_builtin_name_by_index(int index)
{
    if (index < 1 || sizeof(vm_builtin)/sizeof(vm_builtin[0]) <= index) {
        return NULL;
    }
    return vm_builtin[index].name;
}

DLLEXPORT uint8_t jit_get_builtin_args_by_index(int index)
{
    if (index < 1 || sizeof(vm_builtin)/sizeof(vm_builtin[0]) <= index) {
        return 0;
    }
    return vm_builtin[index].args;
}

DLLEXPORT uint8_t jit_get_builtin_flbit_by_index(int index)
{
    if (index < 1 || sizeof(vm_builtin)/sizeof(vm_builtin[0]) <= index) {
        return 0x00;
    }
    return vm_builtin[index].flbit;
}
