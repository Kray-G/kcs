#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fputc(int c, fileio* stream)
{
    if (c == EOF) return EOF;
    char p = (char)c;
    return (klib_fwrite(&p, 1, 1, stream) == 1) ? c : EOF;
}

#endif
