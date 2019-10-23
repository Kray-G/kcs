#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fflush(fileio* stream)
{
    if (klib_flush_buffer(stream) == EOF) {
        return EOF;
    }

    return FlushFileBuffers(stream->handle) ? 0 : EOF;
}

#endif
