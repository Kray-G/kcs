
#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

int klib_fgetpos(fileio* stream, klib_fpos_t *pos)
{
    if (!stream || !stream->handle || !pos) {
        return -1;
    }
    *pos = GetFilePointerEx(stream->handle);
    return 0;
}

#endif
