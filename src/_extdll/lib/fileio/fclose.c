#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fclose(fileio* stream)
{
    if (!stream) return EOF;

    int ret = 0;
    klib_flush_buffer(stream);
    if (stream->handle != INVALID_HANDLE_VALUE && CloseHandle(stream->handle) == 0) {
        ret = EOF;
    }
    if (stream->allocated) {
        free(stream->buf);
    }
    free(stream);
    return ret;
}

#endif
