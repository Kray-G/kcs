#ifndef KCC_H
#define KCC_H

#if defined(_WIN32) || defined(_WIN64)
#if !defined(KCC_WINDOWS)
#define KCC_WINDOWS
#endif
#pragma warning(disable:4477)
#elif defined(__linux__)
#if !defined(KCC_LINUX)
#define KCC_LINUX
#include <xunistd.h>
#endif
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef LONGLONG_MAX
#define LONGLONG_MAX    (0x7FFFFFFFFFFFFFFFLL)
#endif
#ifndef LONGLONG_MIN
#define LONGLONG_MIN    (0x8000000000000000LL)
#endif
#ifndef ULONGLONG_MAX
#define ULONGLONG_MAX   (0xFFFFFFFFFFFFFFFFLL)
#endif
#ifndef ULONGLONG_MIN
#define ULONGLONG_MIN   (0x0000000000000000LL)
#endif

#define PAD_N(size,n)               (size < (n) ? (n) : (size + (((n) - (size % (n))) % (n))))
#define PAD4(size)                  PAD_N(size,4)
#define PAD8(size)                  PAD_N(size,8)

extern char* get_exe_path(void);
extern const char* make_path(const char* base, const char* name);
extern void *load_library(const char *name, const char *envname);
extern void *get_function(void *h, const char *name);
extern void unload_library(void *h);
extern FILE *fmemopen(void *buf, size_t len, const char *type);

extern int kcc_argc;
extern int kcc_argx;
extern uint64_t *kcc_argv;

#if !defined(DLLEXPORT)
#if defined(KCC_WINDOWS)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
#endif

#if defined(KCC_WINDOWS)
#if defined(KCC_WINDOWS_DEBUG)
#define DEBUG 1
#define _DEBUG 1
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#endif
