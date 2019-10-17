
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fputs(const char* s, fileio* stream)
    {
        return ::fputs(s, stream->fp);
    }

    #else

    inline int fputs(const char* s, fileio* stream)
    {
        unsigned int l = strlen(s);
        if (klib::fwrite(s, 1, l, stream) == l) {
            return 0;
        }
        return EOF;
    }

    #endif

} // namespace klib
