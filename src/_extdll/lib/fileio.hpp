#ifndef KLIB_FILESYSTEM_FILEIO_HPP_
#define KLIB_FILESYSTEM_FILEIO_HPP_

#include <cstdio>
#include <cstdarg>

namespace klib {

    #if !defined(KLIB_CONFIG_FILEIO_WIN)

    #ifdef _LARGEFILE_SOURCE
    typedef ::fpos64_t fpos_t;
    #else
    typedef ::fpos_t fpos_t;
    #endif

    struct fileio {
        FILE* fp;
    };

    inline fileio* fio_stdio(int type)
    {
        static fileio fio_stdin_val  = { stdin  };
        static fileio fio_stdout_val = { stdout };
        static fileio fio_stderr_val = { stderr };
        switch (type) {
        case 0: return &fio_stdin_val;
        case 1: return &fio_stdout_val;
        case 2: return &fio_stderr_val;
        }
        return NULL;
    }

    #else

    typedef long long fpos_t;

    const int KLIB_FILEIO_BUFSIZE_SMALL = (512);
    const int KLIB_FILEIO_BUFSIZE = (4096*16);
    struct fileio {
        HANDLE handle;
        int flags;
            // bit
            //  0:  0 for text, 1 for binary
            //  1:  input enable
            //  2:  output enable
            //  3:  EOF flag
            //  4:  ungetc enable
        int ungetc;
        char* buf;
        int len;
        int bufsize;
        int alcsize;
        bool allocated;
    };

    inline fileio* fio_stdio(int type)
    {
        static char stdinbuf[KLIB_FILEIO_BUFSIZE_SMALL];
        static char stdoutbuf[KLIB_FILEIO_BUFSIZE_SMALL];
        static char stderrbuf[KLIB_FILEIO_BUFSIZE_SMALL];
        static fileio fio_stdin_val  = { GetStdHandle(STD_INPUT_HANDLE),  0x02, 0, stdinbuf,  0,
                                         KLIB_FILEIO_BUFSIZE_SMALL, KLIB_FILEIO_BUFSIZE_SMALL, false };
        static fileio fio_stdout_val = { GetStdHandle(STD_OUTPUT_HANDLE), 0x04, 0, stdoutbuf, 0,
                                         KLIB_FILEIO_BUFSIZE_SMALL, KLIB_FILEIO_BUFSIZE_SMALL, false };
        static fileio fio_stderr_val = { GetStdHandle(STD_ERROR_HANDLE),  0x04, 0, stderrbuf, 0,
                                         KLIB_FILEIO_BUFSIZE_SMALL, KLIB_FILEIO_BUFSIZE_SMALL, false };
        switch (type) {
        case 0: return &fio_stdin_val;
        case 1: return &fio_stdout_val;
        case 2: return &fio_stderr_val;
        }
        return NULL;
    }

    inline int flush_buffer(fileio* stream)
    {
        if (stream->len > 0) {
            DWORD len = 0;
            if (WriteFile(stream->handle, stream->buf, stream->len, &len, NULL) == 0) {
                stream->len = 0;
                return EOF;
            }
            stream->len = 0;
            return len;
        }
        return 0;
    }

    __int64 GetFilePointerEx(HANDLE handle)
    {
        LARGE_INTEGER liOfs = {0};
        LARGE_INTEGER liNew = {0};
        if (!SetFilePointerEx(handle, liOfs, &liNew, FILE_CURRENT)) {
            return EOF;
        }
        return liNew.QuadPart;
    }

    #endif

    #define fio_stdin   klib::fio_stdio(0)
    #define fio_stdout  klib::fio_stdio(1)
    #define fio_stderr  klib::fio_stdio(2)
}

#include "fileio/fopen.hpp"
#include "fileio/fflush.hpp"
#include "fileio/fclose.hpp"
#include "fileio/fwrite.hpp"
#include "fileio/fread.hpp"
#include "fileio/fputc.hpp"
#include "fileio/fputs.hpp"
#include "fileio/fgetc.hpp"
#include "fileio/fgets.hpp"
#include "fileio/ftell.hpp"
#include "fileio/fseek.hpp"
#include "fileio/vfprintf.hpp"
#include "fileio/vprintf.hpp"
#include "fileio/fprintf.hpp"
#include "fileio/feof.hpp"
#include "fileio/rewind.hpp"
#include "fileio/ungetc.hpp"
#include "fileio/fgetpos.hpp"
#include "fileio/fsetpos.hpp"

namespace {
    struct init_stdio_file {
        init_stdio_file() { klib::fio_stdio(-1); }
    } s_init_stdio_file;
}

#endif  // KLIB_FILESYSTEM_FILEIO_HPP_
