
#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

char* klib_fgets(char* s, int n, fileio* stream)
{
    char* s0 = s;
    while (--n > 0) {
        int c = klib_fgetc(stream);
        if (c == EOF) {
            if (s0 == s) s0 = NULL;
            break;
        }
        *s++ = c;
        if (c == '\n') break;
    }
    *s = '\0';
    return s0;
}

#endif
