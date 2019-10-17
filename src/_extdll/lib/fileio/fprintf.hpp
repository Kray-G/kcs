
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fprintf(fileio* stream, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        int i = ::vfprintf(stream->fp, format, ap);
        va_end(ap);
        return i;
    }

    #else

    inline int fprintf(fileio* stream, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        int i = klib::vfprintf(stream, format, ap);
        va_end(ap);
        return i;
    }

    #endif

} // namespace klib
