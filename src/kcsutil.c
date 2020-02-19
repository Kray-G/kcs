#include <kcs.h>
#if !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif

#include <stdlib.h>
#include <string.h>
#include <lacc/string.h>

#if !defined(KCC_WINDOWS)
#include <dlfcn.h>
#endif

/*
 *  Path utilities.
 */

#if defined(KCC_WINDOWS)
#include <windows.h>
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

char* get_exe_path(void)
{
    static char s_result[2048] = {0};

    if (!s_result[0]) {
        char* p;
        int  len;
        char exe_full_path[PATH_MAX];

        len = GetModuleFileNameA(NULL, exe_full_path, PATH_MAX);
        if (len > 0) {
            strncpy(s_result, exe_full_path, 2040);
            p = strrchr(s_result, '\\');
            if (p) *p = 0;
        }
    }

    return s_result;
}
#elif defined(KCC_LINUX)

#include <xunistd.h>
#include <linux/limits.h>

char* get_exe_path(void)
{
    static char s_result[2048] = {0};

    if (!s_result[0]) {
        char* p;
        char exe_full_path[PATH_MAX];
        readlink("/proc/self/exe", exe_full_path, PATH_MAX);
        strncpy(s_result, exe_full_path, 2040);
        p = strrchr(s_result, '/');
        if (p) *p = 0;
    }

    return s_result;
}
#elif defined(KCC_SOLARIS)
char* get_exe_path(void)
{
    static char s_result[2048] = {0};

    if (!s_result[0]) {
        char* p;
        strncpy(s_result, getexecname(), 2040);
        p = strrchr(s_result, '/');
        if (p) *p = 0;
    }

    return s_result;
}
#endif

const char* make_path(const char* base, const char* name)
{
    char buf[4096] = {0};
    strcpy(buf, base);
    strcat(buf, "/");
    strcat(buf, name);
    return dup_string(buf);
}

const char *make_builtin_path(const char* base, const char* name)
{
    char buf[4096] = {0};
    strcpy(buf, base);
    strcat(buf, "/");
    strcat(buf, name);
    return dup_string(buf);
}

#if defined(KCC_WINDOWS)
void *load_library(const char *name, const char *envname)
{
    char libname[PATH_MAX] = {0};
    strcpy(libname, name);
    strcat(libname, ".dll");
    return (void*)LoadLibrary(make_builtin_path(get_exe_path(), libname));
}

void *get_function(void *h, const char *name)
{
    return GetProcAddress((HINSTANCE)h, name);
}

void unload_library(void *h)
{
    FreeLibrary((HINSTANCE)h);
}
#else
void *load_library(const char *name, const char *envname)
{
    char libname[PATH_MAX] = {0};
    strcpy(libname, name);
    strcat(libname, ".so");
    return dlopen(make_builtin_path(get_exe_path(), libname), RTLD_LAZY);
}

void * get_function(void *h, const char *name)
{
    return dlsym(h, name);
}

void unload_library(void *h)
{
    dlclose(h);
}
#endif

void __kcc_assert(const char *message, const char *file, unsigned int line)
{
    printf("Assertion failed: %s, file %s, line %u\n", message, file, line);
    exit(1);
}

