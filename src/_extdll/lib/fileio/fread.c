#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

unsigned int klib_fread(void* ptr, unsigned int size, unsigned int nmemb, fileio* stream)
{
    DWORD ll;
    unsigned int len = 0, bytes = size * nmemb;
    if (bytes == 0) return 0;
    if (stream->flags & 0x08) return 0;
    if (stream->flags & 0x10) {
        *(char*)ptr = stream->ungetc;
        ptr = ((char*)ptr) + 1;
        ++len;
        --bytes;
        stream->flags &= ~0x10;
    }

    if (klib_flush_buffer(stream) == EOF) {
        return 0;
    }
    while (bytes > 0) {
        ReadFile(stream->handle, ptr, bytes, &ll, NULL);
        if (ll == 0) {
            break;
        }
        bytes -= ll;
        len += ll;
        if ((stream->flags & 0x01) == 0 && ll != 0) {
            char* p = (char*)ptr;
            char* p1 = p + ll;
            char* q = p;
            int count = 0;
            for ( ; p < p1; ++p) {
                if (*p == '\r') {
                    ++count;
                }
                else {
                    *q++ = *p;
                }
            }
            bytes += count;
            len -= count;
            ptr = q;
        }
    }

    len /= size;
    if (len != nmemb) {
        stream->flags |= 0x08;  // EOF
    }

    return len;
}

#endif
