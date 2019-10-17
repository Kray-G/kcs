
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)
    inline long long fgetpos(fileio* stream, klib::fpos_t *pos)
    {
        #ifdef _LARGEFILE_SOURCE
        return ::fgetpos64(stream->fp, pos);
        #else
        return ::fgetpos(stream->fp, pos);
        #endif
    }

    #else

    inline int fgetpos(fileio* stream, klib::fpos_t *pos)
    {
        if (!stream || !stream->handle || !pos) {
            return -1;
        }
        *pos = GetFilePointerEx(stream->handle);
        return 0;
    }

    #endif

} // namespace klib
