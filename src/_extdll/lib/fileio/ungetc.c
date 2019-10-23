#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_ungetc(int c, fileio* stream)
{
    if (klib_flush_buffer(stream) == EOF) {
        return EOF;
    }

    if (stream->flags & 0x10) return EOF;
    stream->flags |= 0x10;
    return stream->ungetc = c;
}

#endif
