# Standard C Library

Here is a current status, but I have no useful test set.
If you are interested in it, please make it and send it to me.

## assert.h

|      Name       | Implementation | Remark |
| :-------------- | :------------: | :----- |
| `assert`        |       O        |        |
| `static_assert` |       O        | C11    |

## ctype.h

|    Name    | Implementation | Remark |
| :--------- | :------------: | :----- |
| `isalnum`  |       O        |        |
| `isalpha`  |       O        |        |
| `iscntrl`  |       O        |        |
| `isdigit`  |       O        |        |
| `isgraph`  |       O        |        |
| `islower`  |       O        |        |
| `isprint`  |       O        |        |
| `ispunct`  |       O        |        |
| `isspace`  |       O        |        |
| `isupper`  |       O        |        |
| `isxdigit` |       O        |        |
| `tolower`  |       O        |        |
| `toupper`  |       O        |        |

## errno.h

|   Name   | Implementation | Remark |
| :------- | :------------: | :----- |
| `errno`  |                |        |
| `EDOM`   |                |        |
| `ERANGE` |                |        |
| `EILSEQ` |                |        |

## float.h

|       Name        | Implementation |         Remark          |
| :---------------- | :------------: | :---------------------- |
| `FLT_RADIX`       |       O        | 2                       |
| `FLT_ROUNDS`      |       O        | -1                      |
| `FLT_MANT_DIG`    |       O        | 24                      |
| `FLT_MAX_EXP`     |       O        | 128                     |
| `FLT_MIN_EXP`     |       O        | -125                    |
| `FLT_MAX_10_EXP`  |       O        | 38                      |
| `FLT_MIN_10_EXP`  |       O        | -37                     |
| `FLT_MAX`         |       O        | 3.40282347E+38F         |
| `FLT_MIN`         |       O        | 1.17549435E-38F         |
| `FLT_EPSILON`     |       O        | 1.19209290E-07F         |
| `DBL_MANT_DIG`    |       O        | 53                      |
| `DBL_MAX_EXP`     |       O        | 1024                    |
| `DBL_MIN_EXP`     |       O        | -1021                   |
| `DBL_MAX_10_EXP`  |       O        | 308                     |
| `DBL_MIN_10_EXP`  |       O        | -307                    |
| `DBL_MAX`         |       O        | 1.7976931348623157E+308 |
| `DBL_MIN`         |       O        | 2.2250738585072014E-308 |
| `DBL_EPSILON`     |       O        | 2.2204460492503131E-16  |
| `LDBL_MANT_DIG`   |       O        | DBL_MANT_DIG            |
| `LDBL_MAX_EXP`    |       O        | DBL_MAX_EXP             |
| `LDBL_MIN_EXP`    |       O        | DBL_MIN_EXP             |
| `LDBL_MAX_10_EXP` |       O        | DBL_MAX_10_EXP          |
| `LDBL_MIN_10_EXP` |       O        | DBL_MIN_10_EXP          |
| `LDBL_MAX`        |       O        | DBL_MAX                 |
| `LDBL_MIN`        |       O        | DBL_MIN                 |
| `LDBL_EPSILON`    |       O        | DBL_EPSILON             |

## limits.h

|     Name     | Implementation |           Remark            |
| :----------- | :------------: | :-------------------------- |
| `CHAR_BIT`   |       O        | 8                           |
| `MB_LEN_MAX` |       O        | 6                           |
| `CHAR_MAX`   |       O        | 127                         |
| `CHAR_MIN`   |       O        | -128                        |
| `SCHAR_MAX`  |       O        | 127                         |
| `SCHAR_MIN`  |       O        | -128                        |
| `UCHAR_MAX`  |       O        | 255                         |
| `SHRT_MAX`   |       O        | 32767                       |
| `SHRT_MIN`   |       O        | -32768                      |
| `USHRT_MAX`  |       O        | 65535                       |
| `INT_MAX`    |       O        | 2147483647                  |
| `INT_MIN`    |       O        | (-2147483647 - 1)           |
| `UINT_MAX`   |       O        | 4294967295                  |
| `LONG_MAX`   |       O        | 9223372036854775807L        |
| `LONG_MIN`   |       O        | (-922337203685477580L - 1L) |
| `ULONG_MAX`  |       O        | 18446744073709551615UL      |

## locale.h

|      Name      | Implementation | Remark |
| :------------- | :------------: | :----- |
| `struct lconv` |                |        |
| `localeconv`   |                |        |
| `setlocale`    |                |        |

## math.h

|    Name    | Implementation | Remark |
| :--------- | :------------: | :----- |
| `HUGE_VAL` |       O        |        |
| `acos`     |       O        |        |
| `asin`     |       O        |        |
| `atan`     |       O        |        |
| `atan2`    |       O        |        |
| `ceil`     |       O        |        |
| `cos`      |       O        |        |
| `cosh`     |       O        |        |
| `exp`      |       O        |        |
| `fabs`     |       O        |        |
| `floor`    |       O        |        |
| `fmod`     |       O        |        |
| `frexp`    |       O        |        |
| `ldexp`    |       O        |        |
| `log`      |       O        |        |
| `log10`    |       O        |        |
| `modf`     |       O        |        |
| `pow`      |       O        |        |
| `sin`      |       O        |        |
| `sinh`     |       O        |        |
| `sqrt`     |       O        |        |
| `tan`      |       O        |        |
| `tanh`     |       O        |        |

## setjmp.h

|   Name    | Implementation | Remark |
| :-------- | :------------: | :----- |
| `jmp_buf` |       O        |        |
| `setjmp`  |       O        |        |
| `longjmp` |       O        |        |

## signal.h

|      Name      | Implementation | Remark |
| :------------- | :------------: | :----- |
| `sig_atomic_t` |                |        |
| `raise`        |                |        |
| `signal`       |                |        |

## stdarg.h

|    Name    | Implementation | Remark |
| :--------- | :------------: | :----- |
| `va_list`  |       O        |        |
| `va_arg`   |       O        |        |
| `va_start` |       O        |        |
| `va_end`   |       O        |        |

## stdalign.h

|   Name    | Implementation |  Remark  |
| :-------- | :------------: | :------- |
| `alignof` |       O        | _Alignof |

## stdbool.h

|              Name               | Implementation | Remark |
| :------------------------------ | :------------: | :----- |
| `bool`                          |       O        | _Bool  |
| `true`                          |       O        | 1      |
| `false`                         |       O        | 0      |
| `__bool_true_false_are_defined` |       O        | 1      |

## stddef.h

|    Name     | Implementation | Remark |
| :---------- | :------------: | :----- |
| `ptrdiff_t` |       O        |        |
| `size_t`    |       O        |        |
| `wchar_t`   |       O        |        |
| `NULL`      |       O        |        |
| `offsetof`  |       O        |        |

## stdio.h

|    Name    | Implementation |             Remark             |
| :--------- | :------------: | :----------------------------- |
| `FILE`     |       O        |                                |
| `fpos_t`   |       O        |                                |
| `clearerr` |                |                                |
| `fclose`   |       O        |                                |
| `feof`     |       O        |                                |
| `ferror`   |                |                                |
| `fgetc`    |       O        |                                |
| `fgetpos`  |       O        |                                |
| `fgets`    |       O        |                                |
| `fopen`    |       O        |                                |
| `fprintf`  |       O        |                                |
| `fputc`    |       O        |                                |
| `fputs`    |       O        |                                |
| `fread`    |       O        |                                |
| `fscanf`   |       O        |                                |
| `fseek`    |       O        |                                |
| `fsetpos`  |       O        |                                |
| `ftell`    |       O        |                                |
| `fwrite`   |       O        |                                |
| `getc`     |       O        |                                |
| `getchar`  |       O        |                                |
| `gets`     |       -        | needs to define `KCC_USE_GETS` |
| `perror`   |                |                                |
| `printf`   |       O        |                                |
| `putc`     |       O        |                                |
| `putchar`  |       O        |                                |
| `puts`     |       O        |                                |
| `remove`   |                |                                |
| `rename`   |                |                                |
| `rewind`   |       O        |                                |
| `scanf`    |       O        |                                |
| `setbuf`   |                |                                |
| `setvbuf`  |                |                                |
| `sprintf`  |       O        |                                |
| `sscanf`   |       O        |                                |
| `tmpfile`  |                |                                |
| `tmpnam`   |                |                                |
| `vprintf`  |       O        |                                |
| `vscanf`   |       O        |                                |

## stdlib.h

|     Name     | Implementation | Remark  |
| :----------- | :------------: | :------ |
| `div_t`      |                |         |
| `ldiv_t`     |                |         |
| `MB_CUR_MAX` |                |         |
| `RAND_MAX`   |       O        | 0x7fffU |
| `abort`      |       O        |         |
| `abs`        |                |         |
| `atexit`     |                |         |
| `atof`       |                |         |
| `atoi`       |                |         |
| `atol`       |                |         |
| `bsearch`    |                |         |
| `calloc`     |       O        |         |
| `div`        |                |         |
| `exit`       |       O        |         |
| `free`       |       O        |         |
| `getenv`     |                |         |
| `labs`       |                |         |
| `ldiv`       |                |         |
| `malloc`     |       O        |         |
| `mblen`      |                |         |
| `mbstowcs`   |                |         |
| `mbtowc`     |                |         |
| `qsort`      |                |         |
| `rand`       |       O        |         |
| `realloc`    |       O        |         |
| `srand`      |       O        |         |
| `strtod`     |       O        |         |
| `strtol`     |       O        |         |
| `strtoul`    |       O        |         |
| `system`     |       O        |         |
| `wcstombs`   |                |         |
| `wctomb`     |                |         |

## string.h

|    Name    | Implementation | Remark |
| :--------- | :------------: | :----- |
| `memchr`   |                |        |
| `memcmp`   |       O        |        |
| `memcpy`   |       O        |        |
| `memmove`  |       O        |        |
| `memset`   |       O        |        |
| `strchr`   |       O        |        |
| `strcat`   |       O        |        |
| `strcmp`   |       O        |        |
| `strcpy`   |       O        |        |
| `strcspn`  |                |        |
| `strerror` |                |        |
| `strcoll`  |                |        |
| `strlen`   |       O        |        |
| `strncat`  |       O        |        |
| `strncmp`  |       O        |        |
| `strncpy`  |       O        |        |
| `strpbrk`  |                |        |
| `strrchr`  |       O        |        |
| `strspn`   |                |        |
| `strstr`   |                |        |
| `strtok`   |                |        |
| `strxfrm`  |                |        |

## time.h

|       Name       | Implementation | Remark |
| :--------------- | :------------: | :----- |
| `clock_t`        |                |        |
| `time_t`         |       O        |        |
| `struct tm`      |       O        |        |
| `CLOCKS_PER_SEC` |       O        |        |
| `asctime`        |       O        |        |
| `clock`          |       O        |        |
| `ctime`          |                |        |
| `difftime`       |       O        |        |
| `gmtime`         |       O        |        |
| `localtime`      |       O        |        |
| `mktime`         |       O        |        |
| `strftime`       |                |        |
| `time`           |       O        |        |

## complex.h

|   Name    | Implementation | Remark |
| :-------- | :------------: | :----- |
| `complex` |                |        |
| `I`       |                |        |
| `cabs`    |                |        |
| `carg`    |                |        |
| `cacos`   |                |        |
| `cacosh`  |                |        |
| `casin`   |                |        |
| `casinh`  |                |        |
| `catan`   |                |        |
| `catanh`  |                |        |
| `ccos`    |                |        |
| `ccosh`   |                |        |
| `cexp`    |                |        |
| `cfabs`   |                |        |
| `cimag`   |                |        |
| `clog`    |                |        |
| `clog10`  |                |        |
| `conj`    |                |        |
| `cpow`    |                |        |
| `cproj`   |                |        |
| `creal`   |                |        |
| `csin`    |                |        |
| `csinh`   |                |        |
| `csqrt`   |                |        |
| `ctan`    |                |        |
| `ctanh`   |                |        |
