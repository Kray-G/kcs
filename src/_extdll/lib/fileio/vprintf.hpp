
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int vprintf(const char* format, va_list arg)
    {
        return ::vfprintf(stdout, format, arg);
    }

    #else

    inline int vprintf(const char* format, va_list arg)
    {
        return klib::vfprintf(fio_stdout, format, arg);
    }

    #endif

} // namespace klib
