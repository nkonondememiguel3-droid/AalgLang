#include <stdlib.h>

#include "getline.h"

static int grow_buffer(char **buf, size_t *cap, size_t needed) {
  if (*cap >= needed)
    return 0;

  size_t new_cap = *cap ? *cap : GETDELIM_INITIAL_SIZE;

  while (new_cap < needed) {
    if (new_cap > GETDELIM_MAX_SIZE / 2) {
      new_cap = needed;
      break;
    }
    new_cap *= 2;
  }

  char *tmp = realloc(*buf, new_cap);
  if (!tmp)
    return -1;

  *buf = tmp;
  *cap = new_cap;
  return 0;
}

ssize_t portable_getdelim(char **lineptr, size_t *n, int delim, FILE *stream,
                          int flags) {
  if (!lineptr || !n || !stream) {
    errno = EINVAL;
    return -1;
  }

  if (*lineptr == NULL || *n == 0) {
    *n = GETDELIM_INITIAL_SIZE;
    *lineptr = malloc(*n);
    if (!*lineptr) {
      errno = ENOMEM;
      return -1;
    }
  }

  size_t pos = 0;
  int c;

  while (1) {
    c = fgetc(stream);

    if (c == EOF) {
      if (ferror(stream)) {
        return -1; // errno set by stdio
      }
      break; // EOF
    }

    if (grow_buffer(lineptr, n, pos + 2) < 0) {
      errno = ENOMEM;
      return -1;
    }

    (*lineptr)[pos++] = (char)c;

    if (c == delim) {
      break;
    }
  }

  if (pos == 0 && c == EOF) {
    return -1; // true EOF, no bytes read
  }

  /*
   * Optional CRLF normalization ONLY when:
   *  - delimiter is '\n'
   *  - pattern is "\r\n"
   */
  if (!(flags & GD_KEEP_CRLF) && delim == '\n') {
    if (pos >= 2 && (*lineptr)[pos - 2] == '\r' &&
        (*lineptr)[pos - 1] == '\n') {
      (*lineptr)[pos - 2] = '\n';
      pos -= 1;
    }
  }

  (*lineptr)[pos] = '\0'; // convenience only (binary-safe)

  return (ssize_t)pos;
}

ssize_t portable_getline(char **lineptr, size_t *n, FILE *stream) {
  return portable_getdelim(lineptr, n, '\n', stream, GD_KEEP_CRLF);
}

#if 0
int main(int argc, char **argv) {

  (void)argc;
  (void)argv;

  char *buf = NULL;
  size_t cap = 0;

  FILE *fp_read = fopen(argv[1], "rb");
  FILE *fp_write = fopen(argv[2], "wb");
  if (fp_read == NULL || fp_write == NULL)
    exit(EXIT_FAILURE);

  while (1) {
    ssize_t len = portable_getline(&buf, &cap, fp_read);

    if (len == -1) {
      if (feof(fp_read)) {
        break;
      }
      perror("getline");
      break;
    }

    /* if (strncmp(buf, ".exit", 5) == 0) { */
    /*   break; */
    /* } */

    // binary-safe output
    fwrite(buf, 1, len, fp_write);
  }

  if (buf != NULL)
    printf("%zu\n", strlen(buf));

  free(buf);

  return 0;
}
#endif
