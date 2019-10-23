#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fseek(fileio* stream, int64_t offset, int origin)
{
    if (klib_flush_buffer(stream) == EOF) {
        return EOF;
    }

    int method;
    switch (origin) {
    case SEEK_SET:
        method = FILE_BEGIN;
        break;
    case SEEK_CUR:
        method = FILE_CURRENT;
        if ((stream->flags & 0x10) != 0) --offset;
        break;
    case SEEK_END:
        method = FILE_END;
        break;
    default:
        return EOF;
    }

    LARGE_INTEGER p;
    p.QuadPart = offset;
    if (!SetFilePointerEx(stream->handle, p, NULL, method)) {
        return EOF;
    }

    stream->flags &= ~0x18; // removes EOF and ungetc.
    return 0;
}

#endif
