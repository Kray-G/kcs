#ifndef DLLIO_H
#define DLLIO_H

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

#if defined(KCC_WINDOWS)
#define KLIB_CONFIG_FILEIO_WIN
# define DLLEXPORT __declspec(dllexport)
#include <windows.h>
#if !defined(KLIB_SNPRINTF)
#if defined(_MSC_VER)
    #define KLIB_SNPRINTF   _snprintf
    #define KLIB_SNWPRINTF  _snwprintf
    #define KLIB_VSNPRINTF  _vsnprintf
    #define KLIB_VSNWPRINTF _vsnwprintf
#else
    #define KLIB_SNPRINTF   snprintf
    #define KLIB_SNWPRINTF  swprintf
    #define KLIB_VSNPRINTF  vsnprintf
    #define KLIB_VSNWPRINTF vsnwprintf
#endif
#endif
#else
# define DLLEXPORT
/* Using files as 64-bit. */
#define _FILE_OFFSET_BITS 64
#if !defined(KLIB_SNPRINTF)
    #define KLIB_SNPRINTF   snprintf
    #define KLIB_SNWPRINTF  swprintf
    #define KLIB_VSNPRINTF  vsnprintf
    #define KLIB_VSNWPRINTF vswprintf
#endif
#endif

#endif /* DLLIO_H */
