#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_vprintf(const char* format, va_list arg)
{
    return klib_vfprintf(fio_stdout, format, arg);
}

#endif
