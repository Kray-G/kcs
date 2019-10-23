#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fputs(const char* s, fileio* stream)
{
    unsigned int l = strlen(s);
    if (klib_fwrite(s, 1, l, stream) == l) {
        return 0;
    }
    return EOF;
}

#endif
