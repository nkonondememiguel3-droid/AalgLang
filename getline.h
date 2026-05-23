#ifndef getline_getline_h
#define getline_getline_h

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#if defined(_WIN32) || defined(_WIN64)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#ifndef GETDELIM_INITIAL_SIZE
#define GETDELIM_INITIAL_SIZE 128
#endif

#ifndef GETDELIM_MAX_SIZE
#define GETDELIM_MAX_SIZE ((size_t)-1 / 2)
#endif

/*
 * Flags:
 *  - GD_KEEP_CRLF: do not normalize CRLF when delim == '\n'
 */
enum { GD_DEFAULT = 0, GD_KEEP_CRLF = 1 << 0 };

ssize_t portable_getdelim(char **lineptr, size_t *n, int delim, FILE *stream,
                          int flags);
ssize_t portable_getline(char **lineptr, size_t *n, FILE *stream);

#endif // getline_getline_h
