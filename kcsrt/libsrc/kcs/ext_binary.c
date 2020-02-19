#include <kcs/ext.h>
#include <stdio.h>
#include <string.h>

binary_t binary_init(const uint8_t *src, int len)
{
    unsigned int cap = ((unsigned int)(len / KCC_CAPACITY_UNIT) + 1) * KCC_CAPACITY_UNIT;
    uint8_t *buf = (uint8_t *)calloc(cap, sizeof(uint8_t));
    if (len > 0) {
        memcpy(buf, src, len);
    }
    return (binary_t) {
        .len = len,
        .cap = cap,
        .buf = buf,
    };
}

binary_t binary_copy(const binary_t rhs)
{
    return binary_init(rhs.buf, rhs.len);
}

void binary_append(binary_t* lhs, const binary_t rhs)
{
    if (!lhs) {
        return;
    }
    int len = lhs->len + rhs.len;
    if (len < lhs->cap) {
        memcpy(lhs->buf + lhs->len, rhs.buf, rhs.len);
        lhs->len = len;
    } else {
        unsigned int cap = (lhs->cap < rhs.cap) ? (rhs.cap * 2) : (lhs->cap * 2);
        uint8_t *buf = (uint8_t *)calloc(cap, sizeof(uint8_t));
        memcpy(buf, lhs->buf, lhs->len);
        memcpy(buf + lhs->len, rhs.buf, rhs.len);
        binary_free(lhs);
        lhs->len = len;
        lhs->cap = cap;
        lhs->buf = buf;
    }
}

void binary_append_bytes(binary_t* lhs, const uint8_t *rhs, int rlen)
{
    if (!lhs || !rhs) {
        return;
    }
    int len = lhs->len + rlen;
    if (len < lhs->cap) {
        memcpy(lhs->buf + lhs->len, rhs, rlen);
        lhs->len = len;
    } else {
        unsigned int cap = (lhs->cap < rlen) ? (((unsigned int)(rlen / KCC_CAPACITY_UNIT) + 1) * KCC_CAPACITY_UNIT) : (lhs->cap * 2);
        uint8_t *buf = (uint8_t *)calloc(cap, sizeof(uint8_t));
        memcpy(buf, lhs->buf, lhs->len);
        memcpy(buf + lhs->len, rhs, len);
        binary_free(lhs);
        lhs->len = len;
        lhs->cap = cap;
        lhs->buf = buf;
    }
}

binary_t binary_subbin(const binary_t ary, int start, int len)
{
    unsigned int end = len < 0 ? ary.len : (start + len);
    if (0 < len && ary.len < end) {
        end = ary.len;
    }
    len = end - start;
    unsigned int cap = ary.cap;
    uint8_t *buf = calloc(cap, sizeof(uint8_t));
    memcpy(buf, ary.buf + start, len);
    return (binary_t) {
        .len = len,
        .cap = cap,
        .buf = buf,
    };
}

int binary_print(const binary_t ary)
{
    int oc = 0;
    for (int i = 0; i < ary.len; ++i) {
        if ((i % 16) == 0) {
            printf("%08X: ", i);
        }
        oc += printf("%02X", ary.buf[i]);
        if ((i % 16) == 15 || i == (ary.len - 1)) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
    return oc;
}
