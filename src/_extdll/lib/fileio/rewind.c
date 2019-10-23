#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

void klib_rewind(fileio* stream)
{
    klib_fseek(stream, 0, SEEK_SET);
    stream->flags &= ~0x08;
}

#endif
