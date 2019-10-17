
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fseek(fileio* stream, int64_t offset, int origin)
    {
        #ifdef _LARGEFILE_SOURCE
        return ::fseeko(stream->fp, offset, origin);
        #else
        return ::fseek(stream->fp, offset, origin);
        #endif
    }

    #else

    inline int fseek(fileio* stream, int64_t offset, int origin)
    {
        if (klib::flush_buffer(stream) == EOF) {
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

} // namespace klib
