
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int64_t ftell(fileio* stream)
    {
        #ifdef _LARGEFILE_SOURCE
        return ::ftello(stream->fp);
        #else
        return ::ftell(stream->fp);
        #endif
    }

    #else

    inline int64_t ftell(fileio* stream)
    {
        if (klib::flush_buffer(stream) == EOF) {
            return EOF;
        }

        fpos_t p = GetFilePointerEx(stream->handle);
        if (p == EOF) {
            return 0;
        }
        if (stream->flags & 0x10) --p;
        return (int64_t)p;
    }

    #endif

} // namespace klib
