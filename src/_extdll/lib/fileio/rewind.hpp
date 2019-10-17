
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline void rewind(fileio* stream)
    {
        ::rewind(stream->fp);
    }

    #else

    inline void clearerr(fileio* stream)
    {
        stream->flags &= ~0x08;
    }

    inline void rewind(fileio* stream)
    {
        klib::fseek(stream, 0, SEEK_SET);
        klib::clearerr(stream);
    }

    #endif

} // namespace klib
