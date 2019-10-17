#include <kcc/ext.h>
#include <string.h>

string_t string_init(const char *cstr)
{
    unsigned int len = cstr ? strlen(cstr) : 0;
    if (len > 0) {
        unsigned int cap = ((unsigned int)(len / KCC_CAPACITY_UNIT) + 1) * KCC_CAPACITY_UNIT;
        char *buf = (char *)calloc(cap, sizeof(char));
        strcpy(buf, cstr);
        return (string_t) {
            .len = len,
            .cap = cap,
            .cstr = buf,
        };
    }
    return (string_t) {
        .len = 0,
        .cap = 0,
        .cstr = NULL,
    };
}

string_t string_copy(const string_t rhs)
{
    if (rhs.len > 0) {
        // resizes a capacity.
        unsigned int cap = ((unsigned int)(rhs.len / KCC_CAPACITY_UNIT) + 1) * KCC_CAPACITY_UNIT;
        char *buf = (char *)calloc(cap, sizeof(char));
        strcpy(buf, rhs.cstr);
        return (string_t) {
            .len = rhs.len,
            .cap = cap,
            .cstr = buf,
        };
    }
    return (string_t) {
        .len = 0,
        .cap = 0,
        .cstr = NULL,
    };
}

void string_append(string_t* lhs, const string_t rhs)
{
    if (!lhs) {
        return;
    }
    int len = lhs->len + rhs.len;
    if (len < lhs->cap) {
        lhs->len = len;
        strcat(lhs->cstr, rhs.cstr);
    } else {
        unsigned int cap = (lhs->cap < rhs.cap) ? (rhs.cap * 2) : (lhs->cap * 2);
        char *buf = (char *)calloc(cap, sizeof(char));
        strcpy(buf, lhs->cstr);
        strcat(buf, rhs.cstr);
        string_free(*lhs);
        lhs->len = len;
        lhs->cap = cap;
        lhs->cstr = buf;
    }
}

void string_append_cstr(string_t* lhs, const char *rhs)
{
    if (!lhs || !rhs) {
        return;
    }
    int rlen = strlen(rhs);
    int len = lhs->len + rlen;
    if (len < lhs->cap) {
        lhs->len = len;
        strcat(lhs->cstr, rhs);
    } else {
        unsigned int cap = (lhs->cap < rlen) ? (((unsigned int)(rlen / KCC_CAPACITY_UNIT) + 1) * KCC_CAPACITY_UNIT) : (lhs->cap * 2);
        char *buf = (char *)calloc(cap, sizeof(char));
        strcpy(buf, lhs->cstr);
        strcat(buf, rhs);
        string_free(*lhs);
        lhs->len = len;
        lhs->cap = cap;
        lhs->cstr = buf;
    }
}

string_t string_substr(const string_t str, int start, int len)
{
    unsigned int end = len < 0 ? str.len : (start + len);
    if (0 < len && str.len < end) {
        end = str.len;
    }
    len = end - start;
    unsigned int cap = str.cap;
    char *buf = calloc(cap, sizeof(char));
    strncpy(buf, str.cstr + start, len);
    return (string_t) {
        .len = len,
        .cap = cap,
        .cstr = buf,
    };
}
