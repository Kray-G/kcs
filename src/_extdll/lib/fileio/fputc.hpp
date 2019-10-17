
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fputc(int c, fileio* stream)
    {
        return ::fputc(c, stream->fp);
    }

    #else

    inline int fputc(int c, fileio* stream)
    {
        if (c == EOF) return EOF;
        char p = (char)c;
        return (klib::fwrite(&p, 1, 1, stream) == 1) ? c : EOF;
    }

    #endif

} // namespace klib
