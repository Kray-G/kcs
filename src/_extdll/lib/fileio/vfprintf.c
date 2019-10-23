#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_vfprintf(fileio* stream, const char* format, va_list arg)
{
    char s[8192] = {0};
    int i = KLIB_VSNPRINTF(s, 8191, format, arg);
    i = klib_fwrite(s, 1, i, stream);
    if (stream == fio_stdout || stream == fio_stderr) {
        klib_fflush(stream);
    }
    return i;
}

#endif
