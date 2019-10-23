#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fsetpos(fileio* stream, klib_fpos_t *pos)
{
    if (!stream || !stream->handle || !pos) {
        return -1;
    }
    if (klib_fseek(stream, *pos, SEEK_SET) != 0) {
        return -1;
    }
    return 0;
}

#endif
