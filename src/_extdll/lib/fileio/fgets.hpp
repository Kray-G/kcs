
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline char* fgets(char* s, int n, fileio* stream)
    {
        return ::fgets(s, n, stream->fp);
    }

    #else

    inline char* fgets(char* s, int n, fileio* stream)
    {
        char* s0 = s;
        while (--n > 0) {
            int c = klib::fgetc(stream);
            if (c == EOF) {
                if (s0 == s) s0 = NULL;
                break;
            }
            *s++ = c;
            if (c == '\n') break;
        }
        *s = '\0';
        return s0;
    }

    #endif

} // namespace klib
