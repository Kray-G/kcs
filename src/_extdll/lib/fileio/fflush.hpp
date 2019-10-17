
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fflush(fileio* stream)
    {
        return ::fflush(stream->fp);
    }

    #else

    inline int fflush(fileio* stream)
    {
        if (klib::flush_buffer(stream) == EOF) {
            return EOF;
        }

        return FlushFileBuffers(stream->handle) ? 0 : EOF;
    }

    #endif

} // namespace klib
