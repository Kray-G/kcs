#include <kcc.h>
#if !defined(AMALGAMATION) || !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif

#include "vmacpconv.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
static void conv_impl(const char* src, char* dst, int* size, int fromcode, int tocode)
{
    *size = 0;

    const int nu16 =
        MultiByteToWideChar(fromcode,
                            0,
                            (LPCSTR)src,
                            -1,
                            NULL,
                            0);

    char *bufu16 = calloc(nu16 * 2 + 2, sizeof(char));
    MultiByteToWideChar(fromcode,
                        0,
                        (LPCSTR)src,
                        -1,
                        (LPWSTR)bufu16,
                        nu16);

    const int len =
        WideCharToMultiByte(tocode,
                            0,
                            (LPCWSTR)bufu16,
                            -1,
                            NULL,
                            0,
                            NULL,
                            NULL);
    if (!dst) {
        *size = len;
    }
    else {
        char *bufacp = calloc(len * 2, sizeof(char));
        WideCharToMultiByte(tocode,
                            0,
                            (LPCWSTR)bufu16,
                            -1,
                            (LPSTR)bufacp,
                            len,
                            NULL,
                            NULL);

        *size = lstrlenA(bufacp);
        memcpy(dst, bufacp, *size);
        free(bufacp);
    }
    free(bufu16);
}

INTERNAL int len_acp2utf8(const char *src)
{
    int len = 0;
    conv_impl(src, NULL, &len, CP_ACP, CP_UTF8);
    return len;
}

INTERNAL char *conv_acp2utf8(char *dst, int len, const char *src)
{
    conv_impl(src, dst, &len, CP_ACP, CP_UTF8);
    return dst;
}

INTERNAL int len_utf82acp(const char *src)
{
    int len = 0;
    conv_impl(src, NULL, &len, CP_UTF8, CP_ACP);
    return len;
}

INTERNAL char *conv_utf82acp(char *dst, int len, const char *src)
{
    conv_impl(src, dst, &len, CP_UTF8, CP_ACP);
    return dst;
}
#endif
