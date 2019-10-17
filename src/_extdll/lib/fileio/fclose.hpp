
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fclose(fileio* stream)
    {
        if (!stream) return EOF;
        int r = ::fclose(stream->fp);

        delete stream;
        return r;
    }

    #else

    inline int fclose(fileio* stream)
    {
        if (!stream) return EOF;

        int ret = 0;
        klib::flush_buffer(stream);
        if (stream->handle != INVALID_HANDLE_VALUE && CloseHandle(stream->handle) == 0) {
            ret = EOF;
        }
        if (stream->allocated) {
            delete [] stream->buf;
        }
        delete stream;
        return ret;
    }

    #endif

} // namespace klib
