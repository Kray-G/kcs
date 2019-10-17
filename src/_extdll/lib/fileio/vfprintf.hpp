
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int vfprintf(fileio* stream, const char* format, va_list arg)
    {
        return ::vfprintf(stream->fp, format, arg);
    }

    #else

    inline int vfprintf(fileio* stream, const char* format, va_list arg)
    {
        char s[8192] = {0};
        int i = KLIB_VSNPRINTF(s, 8191, format, arg);
        i = klib::fwrite(s, 1, i, stream);
        if (stream == fio_stdout || stream == fio_stderr) {
            klib::fflush(stream);
        }
        return i;
    }

    #endif

} // namespace klib
