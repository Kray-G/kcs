
namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    inline int fgetc(fileio* stream)
    {
        return ::fgetc(stream->fp);
    }

    #else

    inline int fgetc(fileio* stream)
    {
        if (stream != fio_stdin && klib::flush_buffer(stream) == EOF) {
            return EOF;
        }

        int c = EOF;
        if (stream->flags & 0x10) {
            stream->flags &= ~0x10;
            return stream->ungetc;
        }
        if ((stream->flags & 0x08) == 0) {
            char p;
            klib::fread(&p, 1, 1, stream);
            c = (int)p;
        }
        if (stream->flags & 0x08) {
            return EOF;
        }
        return c;
    }

    #endif

} // namespace klib
