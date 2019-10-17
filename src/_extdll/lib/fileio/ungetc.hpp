
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int ungetc(int c, fileio* stream)
    {
        return ::ungetc(c, stream->fp);
    }

    #else

    inline int ungetc(int c, fileio* stream)
    {
        if (klib::flush_buffer(stream) == EOF) {
            return EOF;
        }

        if (stream->flags & 0x10) return EOF;
        stream->flags |= 0x10;
        return stream->ungetc = c;
    }

    #endif

} // namespace klib
