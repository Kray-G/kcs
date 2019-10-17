
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int feof(fileio* stream)
    {
        return ::feof(stream->fp);
    }

    #else

    inline int feof(fileio* stream)
    {
        if (stream->flags & 0x10) return 0;
        if (stream->flags & 0x08) return 1;
        int c = klib::fgetc(stream);
        if (c == EOF) return 1;

        stream->flags |= 0x10;
        stream->ungetc = c;
        return 0;
    }

    #endif

} // namespace klib
