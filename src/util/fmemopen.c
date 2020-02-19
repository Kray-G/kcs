#include <kcs.h>
#include <stdio.h>

#if defined(KCCI_USE_FUNOPEN)

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

typedef struct fmemstat_ {
    uint64_t len;
    uint64_t pos;
    char     *buf;
} fmemstat_t;

static int readfn(void *arg, char *buf, int len)
{
    fmemstat_t *st = (fmemstat_t *)arg;
    int64_t sz = (int64_t)(st->len - st->pos);

    if (sz < 0) {
        sz = 0;
    }
    if (len > sz) {
        len = sz;
    }
    memcpy(buf, &st->buf[st->pos], len);
    st->pos += len;

    return len;
}

static int writefn(void *arg, const char *buf, int len)
{
    fmemstat_t *st = (fmemstat_t *)arg;
    int64_t sz = (int)(st->len - st->pos);

    if (sz < 0) {
        sz = 0;
    }
    if (len > sz) {
        len = sz;
    }
    memcpy(&st->buf[st->pos], buf, len);
    st->pos += len;

    return len;
}

static fpos_t seekfn(void *arg, fpos_t offset, int whence)
{
    fpos_t pos;
    fmemstat_t *st = (fmemstat_t *)arg;

    switch (whence) {
    case SEEK_SET:
        pos = offset;
        break;
    case SEEK_END:
        pos = st->len + offset;
        break;
    case SEEK_CUR:
        pos = st->pos + offset;
        break;
    default:
        return -1;
    }

    if (pos < 0 || (size_t)pos > st->len) {
        st->pos = 0;
        return -1;
    }

    return 0;
}

static int closefn(void *arg)
{
    free(arg);
    return 0;
}

FILE *fmemopen(void *buf, size_t len, const char *type)
{
    fmemstat_t *st = (fmemstat_t *)calloc(1, sizeof(*st));
    if (st) {
        st->buf = buf;
        st->len = len;
        st->pos = 0;
        return funopen(st, readfn, writefn, seekfn, closefn);
    }
    return NULL;
}

#elif defined(KCC_WINDOWS)

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <windows.h>

FILE *fmemopen(void *buf, size_t len, const char *type)
{
    char tfn[MAX_PATH + 1] = {0};
    char ifn[MAX_PATH + 1] = {0};

    if (!GetTempPathA(sizeof(tfn), tfn)) {
        return NULL;
    }
    if (!GetTempFileNameA(tfn, "kcsitemp", 0, ifn)) {
        return NULL;
    }

    int fd = _open(ifn, _O_CREAT | _O_RDWR | _O_SHORT_LIVED | _O_TEMPORARY | _O_BINARY, _S_IREAD | _S_IWRITE);
    if (fd == -1) {
        return NULL;
    }

    FILE *fp = _fdopen(fd, "w+");
    if (!fp) {
        _close(fd);
        return NULL;
    }

    fwrite(buf, len, 1, fp);
    rewind(fp);
    return fp;
}

#endif
