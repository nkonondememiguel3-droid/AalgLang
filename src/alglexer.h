/**
 * AUTHOR: NKONO NDEME Miguel
 * lexer.h — AlgLang lexer public API
 *
 * Token stream
 * lexer_next() returns tokens one at a time.  The caller owns the
 * resulting Token (stored in the Arena — never freed individually).
 * The full token array can be materialised with lexer_tokenize_all()
 * which returns a DynArray of Token (fat-pointer, see arena.h).
 */

#ifndef alglang_lexer_h
#define alglang_lexer_h

#include "algarena.h"
#include "algtoken.h"

/* One surface spelling for a keyword. */
typedef struct _kw_entry_ {
  const char *surface;
  token_kind_t kind;
} kw_entry_t;

typedef struct _lang_config_ {
  const char *lang_name;
  char decimal_sep;

  /* Fat-pointer DynArray of KwEntry. */
  kw_entry_t *keywords;
} lang_config_t;

/* Build a default English LangConfig */
lang_config_t langcfg_english(arena_t *a);

/* Build a default French LangConfig. */
lang_config_t langcfg_french(arena_t *a);

/* Parse a simple KEY=VALUE .lang file from `src` */
lang_config_t langcfg_parse(arena_t *a, const char *src, const char *filename);

/* Internal hash-map node. */
typedef struct _kw_map_node_ {
  const char *key;
  token_kind_t kind;
  struct _kw_map_node_ *next;
} kw_map_node_t;

#define KWMAP_BUCKETS 256

typedef struct _lexer_ {
  arena_t *arena;

  /* Source text (arena-owned copy). */
  const char *src;
  size_t src_len;

  /* Scanner position. */
  size_t pos;
  uint32_t line;
  uint32_t col;

  /* File name. */
  const char *filename;

  /* Keyword hash-map: surface_word → TokenKind. */
  kw_map_node_t *kw_map[KWMAP_BUCKETS];

  /* Decimal separator ('.' or ','). */
  char decimal_sep;

  /* Peek buffer: one token of lookahead. */
  token_t peek_buf;
  int has_peek;

  /* Error count. */
  int error_count;
} lexer_t;

/**
 * Initialise the lexer.
 *
 * @param lx       Lexer to initialise (caller-allocated, e.g. on stack).
 * @param a        Arena used for all allocations (source copy, interned
 *                 strings, keyword map nodes).
 * @param src      NUL-terminated source text.  The lexer makes an arena
 *                 copy so the caller may free `src` afterwards.
 * @param filename Filename used in diagnostics (arena-copied).
 * @param cfg      Language configuration (keywords + decimal separator).
 */
void lexer_init(lexer_t *lx, arena_t *a, const char *src, const char *filename,
                const lang_config_t *cfg);

/**
 * Return the next Token and advance the scanner.
 * Returns TK_EOF repeatedly after the input is exhausted.
 */
token_t lexer_next(lexer_t *lx);

/**
 * Peek at the next token without consuming it.
 */
token_t lexer_peek(lexer_t *lx);

/**
 * Consume the next token only if its kind matches `k`.
 * Returns 1 on success, 0 if the kind does not match (token not consumed).
 */
int lexer_eat(lexer_t *lx, token_kind_t k, token_t *out);

/**
 * Consume the next token and assert it has kind `k`.
 * Emits an error and returns a TK_ERROR token on mismatch.
 */
token_t lexer_expect(lexer_t *lx, token_kind_t k);

/**
 * Materialise the full token stream into a DynArray<Token>.
 * Useful for parsers that need random access.
 *
 * Returns a fat-pointer Token* (use da_len() for length).
 * TK_EOF is included as the last element.
 */
token_t *lexer_tokenize_all(lexer_t *lx, arena_t *a);

/* Number of errors emitted so far. */
static inline int lexer_error_count(const lexer_t *lx) {
  return lx->error_count;
}

#endif /* alglang_lexer_h */
