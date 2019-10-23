#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int64_t klib_ftell(fileio* stream)
{
    if (klib_flush_buffer(stream) == EOF) {
        return EOF;
    }

    klib_fpos_t p = GetFilePointerEx(stream->handle);
    if (p == EOF) {
        return 0;
    }
    if (stream->flags & 0x10) --p;
    return (int64_t)p;
}

#endif
