
#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fprintf(fileio* stream, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    int i = klib_vfprintf(stream, format, ap);
    va_end(ap);
    return i;
}

#endif
