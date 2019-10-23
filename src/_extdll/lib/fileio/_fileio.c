#include "../fileio.h"

#if defined(KLIB_CONFIG_FILEIO_WIN)

static char stdinbuf[KLIB_FILEIO_BUFSIZE_SMALL]  = {0};
static char stdoutbuf[KLIB_FILEIO_BUFSIZE_SMALL] = {0};
static char stderrbuf[KLIB_FILEIO_BUFSIZE_SMALL] = {0};
static fileio fio_stdin_val  = { NULL, 0x02, 0, stdinbuf,  0, KLIB_FILEIO_BUFSIZE_SMALL, KLIB_FILEIO_BUFSIZE_SMALL, false };
static fileio fio_stdout_val = { NULL, 0x04, 0, stdoutbuf, 0, KLIB_FILEIO_BUFSIZE_SMALL, KLIB_FILEIO_BUFSIZE_SMALL, false };
static fileio fio_stderr_val = { NULL, 0x04, 0, stderrbuf, 0, KLIB_FILEIO_BUFSIZE_SMALL, KLIB_FILEIO_BUFSIZE_SMALL, false };

fileio* fio_stdio(int type)
{
    if (!fio_stdin_val.handle) {
        fio_stdin_val.handle  = GetStdHandle(STD_INPUT_HANDLE);
        fio_stdout_val.handle = GetStdHandle(STD_OUTPUT_HANDLE);
        fio_stderr_val.handle = GetStdHandle(STD_ERROR_HANDLE);
    }
    switch (type) {
    case 0: return &fio_stdin_val;
    case 1: return &fio_stdout_val;
    case 2: return &fio_stderr_val;
    }
    return NULL;
}

int klib_flush_buffer(fileio* stream)
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
