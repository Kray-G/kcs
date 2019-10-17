
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)
    inline long long fsetpos(fileio* stream, klib::fpos_t *pos)
    {
        #ifdef _LARGEFILE_SOURCE
        return ::fsetpos64(stream->fp, pos);
        #else
        return ::fsetpos(stream->fp, pos);
        #endif
    }

    #else

    inline int fsetpos(fileio* stream, klib::fpos_t *pos)
    {
        if (!stream || !stream->handle || !pos) {
            return -1;
        }
        if (klib::fseek(stream, *pos, SEEK_SET) != 0) {
            return -1;
        }
        return 0;
    }

    #endif

} // namespace klib
