#ifndef VMACPCONV_H
#define VMACPCONV_H
#if !defined(AMALGAMATION) || !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif

#if defined(_WIN32) || defined(_WIN64)

INTERNAL int len_acp2utf8(const char *src);

INTERNAL char *conv_acp2utf8(char *dst, int len, const char *src);

INTERNAL int len_utf82acp(const char *src);

INTERNAL char *conv_utf82acp(char *dst, int len, const char *src);

#else   // not windows
#define len_acp2utf8(src) strlen(src)
#define conv_acp2utf8(dst,len,src) (src)
#define len_utf82acp(src) strlen(src)
#define conv_utf82acp(dst,len,src) (src)
#endif
#endif
