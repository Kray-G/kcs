
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fwrite(const void* ptr, unsigned int size, unsigned int nmemb, fileio* stream)
    {
        return ::fwrite(ptr, size, nmemb, stream->fp);
    }

    #else

    inline unsigned int fwrite(const void* ptr, unsigned int size, unsigned int nmemb, fileio* stream)
    {
        DWORD len = 0, ll;
        unsigned int bytes = size * nmemb;
        if (bytes == 0) return 0;

        int alcsize = stream->alcsize;
        if (stream->flags & 0x01) {
            int chk = stream->len + bytes;
            if (chk < alcsize) {
                memcpy(stream->buf + stream->len, (char*)ptr, bytes);
                stream->len += bytes;
            }
            else {
                int r = alcsize - stream->len;
                memcpy(stream->buf + stream->len, (char*)ptr, r);
                if (WriteFile(stream->handle, stream->buf, alcsize, &ll, NULL) == 0) {
                    len += ll;
                }
                else {
                    stream->len = bytes - r;
                    ptr = (char*)ptr + r;
                    if (stream->len > alcsize) {
                        int n = stream->len % alcsize;
                        int l = stream->len - n;
                        if (WriteFile(stream->handle, ptr, l, &ll, NULL) == 0) {
                            len += ll;
                        }
                        else {
                            ptr = (char*)ptr + l;
                            stream->len = n;
                        }
                    }
                    if (stream->len > 0) {
                        memcpy(stream->buf, ptr, stream->len);
                    }
                    len = bytes;
                }
            }
        }
        else {
            int maxsize = alcsize - 1;
            while (bytes > 0) {
                int count = 0, len = stream->len;
                char* b = stream->buf + len;
                char* p = (char*)ptr;
                char* p1 = p + bytes;
                while (len < alcsize && p < p1) {
                    if (*p == '\n') {
                        if (len >= maxsize) {
                            break;
                        }
                        *b++ = '\r';
                        ++len;
                    }
                    if (len < alcsize) {
                        *b++ = *p++;
                        ++len;
                        ++count;
                    }
                }
                stream->len = len;
                if (len >= maxsize) {
                    if (WriteFile(stream->handle, stream->buf, len, &ll, NULL) == 0) {
                        // error.
                        stream->len = 0;
                        break;
                    }
                    stream->len = 0;
                }
                len += count;
                bytes -= count;
                ptr = (char*)ptr + count;
            }
        }

        return len / size;
    }

    #endif

} // namespace klib
