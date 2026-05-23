#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "algarena.h"
#include "alglexer.h"
#include "algtoken.h"
#include "algtype.h"
#include "getline.h"

void repl(void);
void run_script(arena_t *arena, lang_config_t *cfg, char **files);

int main(int argc, char **argv) {

  arena_t arena = arena_new(0);
  lang_config_t cfg = langcfg_french(&arena);

  if (argc == 1)
    repl();
  else if (argc > 1)
    run_script(&arena, &cfg, argv);

  arena_destroy(&arena);
  return 0;
}

void repl(void) { printf("running the repl\n"); }

static inline bool ext_match(const char *name, const char *ext) {
  size_t nl = strlen(name), el = strlen(ext);
  return nl >= el && !strcmp(name + nl - el, ext);
}

void run_script(arena_t *arena, lang_config_t *cfg, char **files) {
  if (files == NULL)
    return;

  /* size_t cap = 0; */
  /* char *buf = NULL; */
  lexer_t lexer;

  while (*++files) {
    /* first check if the file end and '.al' extension. */
    if (!ext_match(*files, ".al")) {
      fprintf(stderr, "%s not supported.\n", *files);
      break;
    }

    errno = 0;
    FILE *fp = fopen(*files, "r");
    if (fp == NULL) {
      perror("failed to open file");
      fprintf(stderr, "Error code: %d - %s\n", errno, strerror(errno));
      break;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = arena_alloc(arena, fsize + 1);
    fread(buf, 1, fsize, fp);

    lexer_init(&lexer, arena, buf, *files, cfg);
    token_t token;
    while ((token = lexer_next(&lexer)).token_type != TK_EOF) {
      if (token.token_type == TK_STRING_LIT) {
        const char *str = (const char *)(uintptr_t)token.integer_value;
        printf("%s at %u:%u: \"%s\"\n", token_kind_name(token.token_type),
               token.location.line, token.location.col, str);
      } else {
        printf("%s at %u:%u\n", token_kind_name(token.token_type),
               token.location.line, token.location.col);
      }
    }

    fclose(fp);
  }
}
