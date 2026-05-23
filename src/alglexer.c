/**
 * AUTHOR: NKONO NDEME Miguel
 * lexer.c — AlgLang lexer implementation
 */

#include "alglexer.h"
#include "algtoken.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static token_t lex_scan(lexer_t *lx);

static void lex_error(lexer_t *lx, uint32_t line, uint32_t col, const char *fmt,
                      ...) {
  lx->error_count++;
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "%s:%u:%u: lexer error: ", lx->filename, line, col);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

/* djb2 hash. */
static uint32_t kw_hash(const char *s, size_t len) {
  uint32_t h = 5381;
  for (size_t i = 0; i < len; i++)
    h = ((h << 5) + h) + (unsigned char)s[i];
  return h;
}

static void kwmap_insert(lexer_t *lx, const char *surface, token_kind_t kind) {
  /* Intern the key string into the arena */
  char *key = arena_strdup(lx->arena, surface);

  uint32_t idx = kw_hash(key, strlen(key)) & (KWMAP_BUCKETS - 1);
  kw_map_node_t *node =
      (kw_map_node_t *)arena_alloc(lx->arena, sizeof(kw_map_node_t));
  node->key = key;
  node->kind = kind;
  node->next = lx->kw_map[idx];
  lx->kw_map[idx] = node;
}

static token_kind_t kwmap_lookup(const lexer_t *lx, const char *s, size_t len) {
  uint32_t idx = kw_hash(s, len) & (KWMAP_BUCKETS - 1);
  for (const kw_map_node_t *n = lx->kw_map[idx]; n; n = n->next) {
    if (strlen(n->key) == len && memcmp(n->key, s, len) == 0)
      return n->kind;
  }
  return TK_IDENT;
}

/* Macro to fill keyword arrays for a given config */
#define KWDEF(surface, kind)                                                   \
  { (surface), (kind) }

/* English defaults */
static const kw_entry_t ENGLISH_KWS[] = {
    KWDEF("algo", KW_ALGO),
    KWDEF("algorithm", KW_ALGO),
    KWDEF("module", KW_MODULE),
    KWDEF("import", KW_IMPORT),
    KWDEF("begin", KW_BEGIN),
    KWDEF("end", KW_END),
    KWDEF("var", KW_VAR),
    KWDEF("variables", KW_VAR),
    KWDEF("const", KW_CONST),
    KWDEF("constants", KW_CONST),
    KWDEF("type", KW_TYPE),
    KWDEF("struct", KW_STRUCT),
    KWDEF("endstruct", KW_ENDSTRUCT),
    KWDEF("function", KW_FUNCTION),
    KWDEF("endfunction", KW_ENDFUNCTION),
    KWDEF("method", KW_METHOD),
    KWDEF("endmethod", KW_ENDMETHOD),
    KWDEF("return", KW_RETURN),
    KWDEF("ref", KW_REF),
    KWDEF("if", KW_IF),
    KWDEF("then", KW_THEN),
    KWDEF("elseif", KW_ELSEIF),
    KWDEF("else", KW_ELSE),
    KWDEF("endif", KW_ENDIF),
    KWDEF("while", KW_WHILE),
    KWDEF("do", KW_DO),
    KWDEF("endwhile", KW_ENDWHILE),
    KWDEF("repeat", KW_REPEAT),
    KWDEF("until", KW_UNTIL),
    KWDEF("for", KW_FOR),
    KWDEF("to", KW_TO),
    KWDEF("step", KW_STEP),
    KWDEF("endfor", KW_ENDFOR),
    KWDEF("integer", KW_INT),
    KWDEF("int", KW_INT),
    KWDEF("real", KW_REAL),
    KWDEF("float", KW_REAL),
    KWDEF("string", KW_STRING),
    KWDEF("char", KW_CHAR),
    KWDEF("character", KW_CHAR),
    KWDEF("bool", KW_BOOL),
    KWDEF("boolean", KW_BOOL),
    KWDEF("number", KW_NUMBER),
    KWDEF("array", KW_ARRAY),
    KWDEF("of", KW_OF),
    KWDEF("file", KW_FILE),
    KWDEF("socket", KW_SOCKET),
    KWDEF("ptr", KW_PTR),
    KWDEF("pointer", KW_PTR),
    KWDEF("cast", KW_CAST),
    KWDEF("write", KW_WRITE),
    KWDEF("print", KW_WRITE),
    KWDEF("read", KW_READ),
    KWDEF("open_file", KW_FILE_OPEN),
    KWDEF("close_file", KW_FILE_CLOSE),
    KWDEF("write_line", KW_FILE_WRITE_LINE),
    KWDEF("read_line", KW_FILE_READ_LINE),
    KWDEF("seek_file", KW_FILE_SEEK),
    KWDEF("delete_file", KW_FILE_DELETE),
    KWDEF("file_exists", KW_FILE_EXISTS),
    KWDEF("FILE_READ", KW_FILE_READ),
    KWDEF("FILE_WRITE", KW_FILE_WRITE),
    KWDEF("FILE_APPEND", KW_FILE_APPEND),
    KWDEF("FILE_READ_WRITE", KW_FILE_READ_WRITE),
    KWDEF("net_connect", KW_NET_CONNECT),
    KWDEF("net_listen", KW_NET_LISTEN),
    KWDEF("net_accept", KW_NET_ACCEPT),
    KWDEF("net_send", KW_NET_SEND),
    KWDEF("net_receive", KW_NET_RECEIVE),
    KWDEF("net_close", KW_NET_CLOSE),
    KWDEF("http_get", KW_NET_HTTP_GET),
    KWDEF("http_post", KW_NET_HTTP_POST),
    KWDEF("and", KW_AND),
    KWDEF("or", KW_OR),
    KWDEF("not", KW_NOT),
    KWDEF("true", KW_TRUE),
    KWDEF("false", KW_FALSE),
    KWDEF("null", KW_NULL),
    KWDEF("nil", KW_NIL),
    KWDEF("mod", KW_MOD),
    KWDEF("init_window", KW_GFX_INIT_WINDOW),
    KWDEF("close_window", KW_GFX_CLOSE_WINDOW),
    KWDEF("window_closed", KW_GFX_SHOULD_CLOSE),
    KWDEF("set_fps", KW_GFX_SET_FPS),
    KWDEF("get_fps", KW_GFX_GET_FPS),
    KWDEF("begin_draw", KW_GFX_BEGIN_DRAW),
    KWDEF("end_draw", KW_GFX_END_DRAW),
    KWDEF("clear", KW_GFX_CLEAR),
};

lang_config_t langcfg_english(arena_t *a) {
  lang_config_t cfg;
  cfg.lang_name = arena_strdup(a, "english");
  cfg.decimal_sep = '.';
  cfg.keywords = NULL;
  size_t n = sizeof(ENGLISH_KWS) / sizeof(ENGLISH_KWS[0]);
  for (size_t i = 0; i < n; i++) {
    kw_entry_t e = ENGLISH_KWS[i];
    da_push(a, cfg.keywords, e);
  }
  return cfg;
}

/* French surface keywords */
static const kw_entry_t FRENCH_KWS[] = {
    KWDEF("Algorithme", KW_ALGO),
    KWDEF("Module", KW_MODULE),
    KWDEF("Importer", KW_IMPORT),
    KWDEF("Debut", KW_BEGIN),
    KWDEF("Fin", KW_END),
    KWDEF("Variables", KW_VAR),
    KWDEF("const", KW_CONST),
    KWDEF("type", KW_TYPE),
    KWDEF("Structure", KW_STRUCT),
    KWDEF("Finstructure", KW_ENDSTRUCT),
    KWDEF("Fonction", KW_FUNCTION),
    KWDEF("Finfonction", KW_ENDFUNCTION),
    KWDEF("Methode", KW_METHOD),
    KWDEF("Finmethode", KW_ENDMETHOD),
    KWDEF("retourne", KW_RETURN),
    KWDEF("ref", KW_REF),
    KWDEF("si", KW_IF),
    KWDEF("alors", KW_THEN),
    KWDEF("sinonsi", KW_ELSEIF),
    KWDEF("sinon", KW_ELSE),
    KWDEF("finsi", KW_ENDIF),
    KWDEF("tantque", KW_WHILE),
    KWDEF("tant_que", KW_WHILE),
    KWDEF("faire", KW_DO),
    KWDEF("fintantque", KW_ENDWHILE),
    KWDEF("repeter", KW_REPEAT),
    KWDEF("jusqu_a", KW_UNTIL),
    KWDEF("pour", KW_FOR),
    KWDEF("jusqua", KW_TO),
    KWDEF("pas", KW_STEP),
    KWDEF("finpour", KW_ENDFOR),
    KWDEF("entier", KW_INT),
    KWDEF("reel", KW_REAL),
    KWDEF("chaine", KW_STRING),
    KWDEF("chaine_charactere", KW_STRING),
    KWDEF("caractere", KW_CHAR),
    KWDEF("booleen", KW_BOOL),
    KWDEF("nombre", KW_NUMBER),
    KWDEF("tableau", KW_ARRAY),
    KWDEF("de", KW_OF),
    KWDEF("fichier", KW_FILE),
    KWDEF("socket", KW_SOCKET),
    KWDEF("ptr", KW_PTR),
    KWDEF("convertir", KW_CAST),
    KWDEF("ecrire", KW_WRITE),
    KWDEF("lire", KW_READ),
    KWDEF("ouvrir_fichier", KW_FILE_OPEN),
    KWDEF("fermer_fichier", KW_FILE_CLOSE),
    KWDEF("ecrire_ligne", KW_FILE_WRITE_LINE),
    KWDEF("lire_ligne", KW_FILE_READ_LINE),
    KWDEF("chercher_fichier", KW_FILE_SEEK),
    KWDEF("supprimer_fichier", KW_FILE_DELETE),
    KWDEF("fichier_existe", KW_FILE_EXISTS),
    KWDEF("LIRE", KW_FILE_READ),
    KWDEF("ECRIRE", KW_FILE_WRITE),
    KWDEF("AJOUTER", KW_FILE_APPEND),
    KWDEF("LIRE_ECRIRE", KW_FILE_READ_WRITE),
    KWDEF("connecter", KW_NET_CONNECT),
    KWDEF("ecouter", KW_NET_LISTEN),
    KWDEF("accepter", KW_NET_ACCEPT),
    KWDEF("envoyer", KW_NET_SEND),
    KWDEF("recevoir", KW_NET_RECEIVE),
    KWDEF("fermer_socket", KW_NET_CLOSE),
    KWDEF("http_get", KW_NET_HTTP_GET),
    KWDEF("http_post", KW_NET_HTTP_POST),
    KWDEF("et", KW_AND),
    KWDEF("ou", KW_OR),
    KWDEF("non", KW_NOT),
    KWDEF("vrai", KW_TRUE),
    KWDEF("faux", KW_FALSE),
    KWDEF("nul", KW_NULL),
    KWDEF("nil", KW_NIL),
    KWDEF("mod", KW_MOD),
    KWDEF("init_fenetre", KW_GFX_INIT_WINDOW),
    KWDEF("fermer_fenetre", KW_GFX_CLOSE_WINDOW),
    KWDEF("fenetre_fermee", KW_GFX_SHOULD_CLOSE),
    KWDEF("definir_fps", KW_GFX_SET_FPS),
    KWDEF("obtenir_fps", KW_GFX_GET_FPS),
    KWDEF("Debut_dessin", KW_GFX_BEGIN_DRAW),
    KWDEF("Fin_dessin", KW_GFX_END_DRAW),
    KWDEF("effacer", KW_GFX_CLEAR),
};

lang_config_t langcfg_french(arena_t *a) {
  lang_config_t cfg;
  cfg.lang_name = arena_strdup(a, "french");
  cfg.decimal_sep = ',';
  cfg.keywords = NULL;
  size_t n = sizeof(FRENCH_KWS) / sizeof(FRENCH_KWS[0]);
  for (size_t i = 0; i < n; i++) {
    kw_entry_t e = FRENCH_KWS[i];
    da_push(a, cfg.keywords, e);
  }
  return cfg;
}

lang_config_t langcfg_parse(arena_t *a, const char *src, const char *filename) {
  (void)filename;
  lang_config_t cfg;
  cfg.lang_name = arena_strdup(a, "custom");
  cfg.decimal_sep = '.';
  cfg.keywords = NULL;

  /* Build a reverse lookup: keyword name -> TokenKind */
  static const struct {
    const char *name;
    token_kind_t k;
  } NAME2KW[] = {
#define X(n) {#n, n}

    X(KW_ALGO),
    X(KW_MODULE),
    X(KW_IMPORT),
    X(KW_BEGIN),
    X(KW_END),
    X(KW_VAR),
    X(KW_CONST),
    X(KW_TYPE),
    X(KW_STRUCT),
    X(KW_ENDSTRUCT),
    X(KW_FUNCTION),
    X(KW_ENDFUNCTION),
    X(KW_METHOD),
    X(KW_ENDMETHOD),
    X(KW_RETURN),
    X(KW_REF),
    X(KW_IF),
    X(KW_THEN),
    X(KW_ELSEIF),
    X(KW_ELSE),
    X(KW_ENDIF),
    X(KW_WHILE),
    X(KW_DO),
    X(KW_ENDWHILE),
    X(KW_REPEAT),
    X(KW_UNTIL),
    X(KW_FOR),
    X(KW_TO),
    X(KW_STEP),
    X(KW_ENDFOR),
    X(KW_INT),
    X(KW_REAL),
    X(KW_STRING),
    X(KW_CHAR),
    X(KW_BOOL),
    X(KW_NUMBER),
    X(KW_ARRAY),
    X(KW_OF),
    X(KW_FILE),
    X(KW_SOCKET),
    X(KW_PTR),
    X(KW_CAST),
    X(KW_WRITE),
    X(KW_READ),
    X(KW_AND),
    X(KW_OR),
    X(KW_NOT),
    X(KW_TRUE),
    X(KW_FALSE),
    X(KW_NULL),
    X(KW_NIL),
    X(KW_MOD),

#undef X
    {NULL, TK_IDENT}
  };

  const char *p = src;
  while (*p) {
    /* skip whitespace and comments */
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
      p++;
    if (*p == '#') {
      while (*p && *p != '\n')
        p++;
      continue;
    }
    if (!*p)
      break;

    /* read key */
    const char *ks = p;
    while (*p && *p != '=' && *p != '\n')
      p++;
    size_t klen = (size_t)(p - ks);
    while (klen && (ks[klen - 1] == ' ' || ks[klen - 1] == '\t'))
      klen--;
    if (*p != '=')
      continue;
    p++; /* skip '=' */
    while (*p == ' ' || *p == '\t')
      p++;

    /* read value */
    const char *vs = p;
    while (*p && *p != '\n' && *p != '#')
      p++;
    size_t vlen = (size_t)(p - vs);
    while (vlen && (vs[vlen - 1] == ' ' || vs[vlen - 1] == '\t'))
      vlen--;

    /* match key */
    char key[128];
    if (klen >= sizeof(key))
      continue;
    memcpy(key, ks, klen);
    key[klen] = '\0';
    char val[256];
    if (vlen >= sizeof(val))
      continue;
    memcpy(val, vs, vlen);
    val[vlen] = '\0';

    if (strcmp(key, "lang_name") == 0) {
      cfg.lang_name = arena_strdup(a, val);
    } else if (strcmp(key, "decimal_sep") == 0) {
      cfg.decimal_sep = val[0];
    } else {
      /* look up KW name */
      for (int i = 0; NAME2KW[i].name; i++) {
        if (strcmp(NAME2KW[i].name, key) == 0) {
          kw_entry_t e = {arena_strdup(a, val), NAME2KW[i].k};
          da_push(a, cfg.keywords, e);
          break;
        }
      }
    }
  }
  return cfg;
}

void lexer_init(lexer_t *lx, arena_t *a, const char *src, const char *filename,
                const lang_config_t *cfg) {
  memset(lx, 0, sizeof(*lx));
  lx->arena = a;
  lx->src = arena_strdup(a, src);
  lx->src_len = strlen(src);
  lx->pos = 0;
  lx->line = 1;
  lx->col = 1;
  lx->filename = arena_strdup(a, filename);
  lx->decimal_sep = cfg->decimal_sep;
  lx->has_peek = 0;
  lx->error_count = 0;

  /* Populate keyword hash-map */
  size_t nkw = da_len(cfg->keywords);
  for (size_t i = 0; i < nkw; i++) {
    kwmap_insert(lx, cfg->keywords[i].surface, cfg->keywords[i].kind);
  }
}

static inline char lx_cur(const lexer_t *lx) {
  return lx->pos < lx->src_len ? lx->src[lx->pos] : '\0';
}

static inline char lx_peek1(const lexer_t *lx) {
  return (lx->pos + 1) < lx->src_len ? lx->src[lx->pos + 1] : '\0';
}

static inline char lx_peek2(const lexer_t *lx) {
  return (lx->pos + 2) < lx->src_len ? lx->src[lx->pos + 2] : '\0';
}

static inline char lx_advance(lexer_t *lx) {
  char c = lx->src[lx->pos++];
  if (c == '\n') {
    lx->line++;
    lx->col = 1;
  } else {
    lx->col++;
  }
  return c;
}

static inline int lx_match(lexer_t *lx, char c) {
  if (lx_cur(lx) == c) {
    lx_advance(lx);
    return 1;
  }
  return 0;
}

static token_t make_tok(lexer_t *lx, token_kind_t k, size_t start_pos,
                        uint32_t line, uint32_t col) {
  token_t t;
  t.token_type = k;
  t.location.file = lx->filename;
  t.location.line = line;
  t.location.col = col;
  t.start = lx->src + start_pos;
  t.length = lx->pos - start_pos;
  t.integer_value = 0;
  return t;
}

static void lx_skip_ws(lexer_t *lx) {
again:
  while (lx->pos < lx->src_len && (lx_cur(lx) == ' ' || lx_cur(lx) == '\t' ||
                                   lx_cur(lx) == '\r' || lx_cur(lx) == '\n')) {
    lx_advance(lx);
  }
  /* single-line comment */
  if (lx_cur(lx) == '/' && lx_peek1(lx) == '/') {
    while (lx->pos < lx->src_len && lx_cur(lx) != '\n')
      lx_advance(lx);
    goto again;
  }
  /* single-line comment with # (used in examples) */
  if (lx_cur(lx) == '#') {
    while (lx->pos < lx->src_len && lx_cur(lx) != '\n')
      lx_advance(lx);
    goto again;
  }
  /* multi-line comment */
  if (lx_cur(lx) == '/' && lx_peek1(lx) == '*') {
    lx_advance(lx);
    lx_advance(lx); /* consume / */
    while (lx->pos + 1 < lx->src_len) {
      if (lx_cur(lx) == '*' && lx_peek1(lx) == '/') {
        lx_advance(lx);
        lx_advance(lx);
        goto again;
      }
      lx_advance(lx);
    }
    /* unterminated — error */
    lex_error(lx, lx->line, lx->col, "unterminated block comment");
  }
}

static uint32_t lx_escape(lexer_t *lx, uint32_t line, uint32_t col) {
  char c = lx_advance(lx);
  switch (c) {
  case 'n':
    return '\n';
  case 't':
    return '\t';
  case 'r':
    return '\r';
  case '\\':
    return '\\';
  case '"':
    return '"';
  case '\'':
    return '\'';
  case '0':
    return '\0';
  case 'x': {
    /* \xHH */
    uint32_t v = 0;
    for (int i = 0; i < 2; i++) {
      char h = lx_cur(lx);
      if (h >= '0' && h <= '9')
        v = v * 16 + (h - '0');
      else if (h >= 'a' && h <= 'f')
        v = v * 16 + (h - 'a' + 10);
      else if (h >= 'A' && h <= 'F')
        v = v * 16 + (h - 'A' + 10);
      else {
        lex_error(lx, line, col, "bad hex escape");
        break;
      }
      lx_advance(lx);
    }
    return v;
  }
  case 'u': {
    /* \uHHHH */
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
      char h = lx_cur(lx);
      if (h >= '0' && h <= '9')
        v = v * 16 + (h - '0');
      else if (h >= 'a' && h <= 'f')
        v = v * 16 + (h - 'a' + 10);
      else if (h >= 'A' && h <= 'F')
        v = v * 16 + (h - 'A' + 10);
      else {
        lex_error(lx, line, col, "bad unicode escape");
        break;
      }
      lx_advance(lx);
    }
    return v;
  }
  default:
    lex_error(lx, line, col, "unknown escape '\\%c'", c);
    return c;
  }
}

static int64_t parse_int_base(lexer_t *lx, int base) {
  int64_t v = 0;
  while (lx->pos < lx->src_len) {
    char c = lx_cur(lx);
    int digit;
    if (c == '_') {
      lx_advance(lx);
      continue;
    } /* allow _ separators */
    if (c >= '0' && c <= '9')
      digit = c - '0';
    else if (c >= 'a' && c <= 'f')
      digit = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      digit = c - 'A' + 10;
    else
      break;
    if (digit >= base)
      break;
    v = v * base + digit;
    lx_advance(lx);
  }
  return v;
}

static token_t lex_scan(lexer_t *lx) {
  lx_skip_ws(lx);

  if (lx->pos >= lx->src_len) {
    token_t t;
    t.token_type = TK_EOF;
    t.location.file = lx->filename;
    t.location.line = lx->line;
    t.location.col = lx->col;
    t.start = lx->src + lx->pos;
    t.length = 0;
    t.integer_value = 0;
    return t;
  }

  uint32_t start_line = lx->line;
  uint32_t start_col = lx->col;
  size_t start_pos = lx->pos;
  char c = lx_advance(lx);

  if (c == '"') {
    /* We build the decoded string value into a StringBuilder.
     * The token .start/.len refer to the raw source span including quotes.
     * The decoded value is stored in an arena string attached as ival=ptr. */
    char *buf = NULL; /* DynArray<char> */
    while (lx->pos < lx->src_len && lx_cur(lx) != '"') {
      char ch = lx_cur(lx);
      if (ch == '\\') {
        lx_advance(lx);
        uint32_t cp = lx_escape(lx, start_line, start_col);
        /* Encode codepoint to UTF-8 */
        if (cp < 0x80) {
          char byte = (char)cp;
          da_push(lx->arena, buf, byte);
        } else if (cp < 0x800) {
          char b1 = (char)(0xC0 | (cp >> 6));
          char b2 = (char)(0x80 | (cp & 0x3F));
          da_push(lx->arena, buf, b1);
          da_push(lx->arena, buf, b2);
        } else {
          char b1 = (char)(0xE0 | (cp >> 12));
          char b2 = (char)(0x80 | ((cp >> 6) & 0x3F));
          char b3 = (char)(0x80 | (cp & 0x3F));
          da_push(lx->arena, buf, b1);
          da_push(lx->arena, buf, b2);
          da_push(lx->arena, buf, b3);
        }
      } else {
        da_push(lx->arena, buf, ch);
        lx_advance(lx);
      }
    }
    if (lx_cur(lx) == '"')
      lx_advance(lx);
    else
      lex_error(lx, start_line, start_col, "unterminated string");

    /* NUL-terminate the buffer */
    char nul = '\0';
    da_push(lx->arena, buf, nul);

    token_t t = make_tok(lx, TK_STRING_LIT, start_pos, start_line, start_col);
    /* Store pointer to decoded string in ival field (cast) */
    t.integer_value = (int64_t)(uintptr_t)buf;
    return t;
  }

  if (c == '\'') {
    uint32_t cp;
    if (lx_cur(lx) == '\\') {
      lx_advance(lx);
      cp = lx_escape(lx, start_line, start_col);
    } else {
      cp = (uint8_t)lx_advance(lx);
    }
    if (lx_cur(lx) == '\'')
      lx_advance(lx);
    else
      lex_error(lx, start_line, start_col, "unterminated char literal");
    token_t t = make_tok(lx, TK_CHAR_LIT, start_pos, start_line, start_col);
    t.character_value = cp;
    return t;
  }

  if (isdigit((unsigned char)c)) {
    /* Restore position to start of number */
    lx->pos = start_pos;
    lx->line = start_line;
    lx->col = start_col;
    lx_advance(lx); /* re-consume first digit */

    /* Check for base prefix: 0x, 0b, 0o */
    if (c == '0' && lx->pos < lx->src_len) {
      char nxt = lx_cur(lx);
      if (nxt == 'x' || nxt == 'X') {
        lx_advance(lx);
        int64_t v = parse_int_base(lx, 16);
        token_t t = make_tok(lx, TK_INT_LIT, start_pos, start_line, start_col);
        t.integer_value = v;
        return t;
      }
      if (nxt == 'b' || nxt == 'B') {
        lx_advance(lx);
        int64_t v = parse_int_base(lx, 2);
        token_t t = make_tok(lx, TK_INT_LIT, start_pos, start_line, start_col);
        t.integer_value = v;
        return t;
      }
      if (nxt == 'o' || nxt == 'O') {
        lx_advance(lx);
        int64_t v = parse_int_base(lx, 8);
        token_t t = make_tok(lx, TK_INT_LIT, start_pos, start_line, start_col);
        t.integer_value = v;
        return t;
      }
    }

    /* Decimal integer part (already consumed one digit) */
    while (lx->pos < lx->src_len &&
           (isdigit((unsigned char)lx_cur(lx)) || lx_cur(lx) == '_'))
      lx_advance(lx);

    /* Check for decimal separator → REAL */
    char dsep = lx->decimal_sep;
    int is_real = (lx_cur(lx) == dsep) && isdigit((unsigned char)lx_peek1(lx));

    if (is_real) {
      lx_advance(lx); /* consume separator */
      while (lx->pos < lx->src_len &&
             (isdigit((unsigned char)lx_cur(lx)) || lx_cur(lx) == '_'))
        lx_advance(lx);
      /* optional exponent */
      if (lx_cur(lx) == 'e' || lx_cur(lx) == 'E') {
        lx_advance(lx);
        if (lx_cur(lx) == '-' || lx_cur(lx) == '+')
          lx_advance(lx);
        while (lx->pos < lx->src_len && isdigit((unsigned char)lx_cur(lx)))
          lx_advance(lx);
      }
      /* Copy to temp buffer to parse with strtod, replacing dsep with '.' */
      size_t len = lx->pos - start_pos;
      char *tmp = (char *)arena_alloc(lx->arena, len + 1);
      memcpy(tmp, lx->src + start_pos, len);
      /* Remove underscore separators and normalise decimal point */
      size_t w = 0;
      for (size_t i = 0; i < len; i++) {
        if (tmp[i] == '_')
          continue;
        if (tmp[i] == dsep)
          tmp[w++] = '.';
        else
          tmp[w++] = tmp[i];
      }
      tmp[w] = '\0';
      double v = strtod(tmp, NULL);
      token_t t = make_tok(lx, TK_REAL_LIT, start_pos, start_line, start_col);
      t.real_value = v;
      return t;
    }

    /* Plain integer */
    size_t len = lx->pos - start_pos;
    char *tmp = (char *)arena_alloc(lx->arena, len + 1);
    size_t w = 0;
    for (size_t i = 0; i < len; i++)
      if (lx->src[start_pos + i] != '_')
        tmp[w++] = lx->src[start_pos + i];
    tmp[w] = '\0';
    int64_t v = (int64_t)strtoll(tmp, NULL, 10);
    token_t t = make_tok(lx, TK_INT_LIT, start_pos, start_line, start_col);
    t.integer_value = v;
    return t;
  }

  if (isalpha((unsigned char)c) || c == '_') {
    while (lx->pos < lx->src_len &&
           (isalnum((unsigned char)lx_cur(lx)) || lx_cur(lx) == '_'))
      lx_advance(lx);
    size_t len = lx->pos - start_pos;
    token_kind_t k = kwmap_lookup(lx, lx->src + start_pos, len);
    token_t t = make_tok(lx, k, start_pos, start_line, start_col);
    return t;
  }

  switch (c) {
  case ':':
    return make_tok(lx, TK_COLON, start_pos, start_line, start_col);
  case ';':
    return make_tok(lx, TK_SEMICOLON, start_pos, start_line, start_col);
  case ',':
    return make_tok(lx, TK_COMMA, start_pos, start_line, start_col);
  case '(':
    return make_tok(lx, TK_LPAREN, start_pos, start_line, start_col);
  case ')':
    return make_tok(lx, TK_RPAREN, start_pos, start_line, start_col);
  case '[':
    return make_tok(lx, TK_LBRACKET, start_pos, start_line, start_col);
  case ']':
    return make_tok(lx, TK_RBRACKET, start_pos, start_line, start_col);
  case '+':
    return make_tok(lx, TK_PLUS, start_pos, start_line, start_col);
  case '*':
    return make_tok(lx, TK_STAR, start_pos, start_line, start_col);
  case '/':
    return make_tok(lx, TK_SLASH, start_pos, start_line, start_col);
  case '.':
    if (lx_cur(lx) == '.') {
      lx_advance(lx);
      return make_tok(lx, TK_DOTDOT, start_pos, start_line, start_col);
    }
    return make_tok(lx, TK_DOT, start_pos, start_line, start_col);
  case '-':
    return make_tok(lx, TK_MINUS, start_pos, start_line, start_col);
  case '<':
    if (lx_cur(lx) == '-') {
      lx_advance(lx);
      return make_tok(lx, TK_ARROW, start_pos, start_line, start_col);
    }
    if (lx_cur(lx) == '=') {
      lx_advance(lx);
      return make_tok(lx, TK_LTE, start_pos, start_line, start_col);
    }
    return make_tok(lx, TK_LANGLE, start_pos, start_line, start_col);
  case '>':
    if (lx_cur(lx) == '=') {
      lx_advance(lx);
      return make_tok(lx, TK_GTE, start_pos, start_line, start_col);
    }
    return make_tok(lx, TK_RANGLE, start_pos, start_line, start_col);
  case '=':
    if (lx_cur(lx) == '=') {
      lx_advance(lx);
      return make_tok(lx, TK_EQ, start_pos, start_line, start_col);
    }
    /* bare '=' used in const declarations: treat as a single char */
    /* Fall through to error — grammar uses <- for assignment */
    /* but '=' is used in constant section: IDENTIFIER : type = literal */
    {
      /* We return a synthetic token for '=' used in const decl */
      token_t t = make_tok(lx, TK_EQ, start_pos, start_line, start_col);
      /* Override: single '=' in const context is TK_EQ with len=1 */
      t.length = 1;
      /* Rewind so == isn't accidentally matched */
      return t;
    }
  case '!':
    if (lx_cur(lx) == '=') {
      lx_advance(lx);
      return make_tok(lx, TK_NEQ, start_pos, start_line, start_col);
    }
    return make_tok(lx, TK_BANG, start_pos, start_line, start_col);
  default:
    break;
  }

  /* Unknown character */
  lex_error(lx, start_line, start_col, "unexpected character '%c' (0x%02X)",
            (unsigned char)c, (unsigned char)c);
  token_t t = make_tok(lx, TK_ERROR, start_pos, start_line, start_col);
  return t;
}

token_t lexer_next(lexer_t *lx) {
  if (lx->has_peek) {
    lx->has_peek = 0;
    return lx->peek_buf;
  }
  return lex_scan(lx);
}

token_t lexer_peek(lexer_t *lx) {
  if (!lx->has_peek) {
    lx->peek_buf = lex_scan(lx);
    lx->has_peek = 1;
  }
  return lx->peek_buf;
}

int lexer_eat(lexer_t *lx, token_kind_t k, token_t *out) {
  token_t t = lexer_peek(lx);
  if (t.token_type == k) {
    *out = lexer_next(lx);
    return 1;
  }
  return 0;
}

token_t lexer_expect(lexer_t *lx, token_kind_t k) {
  token_t t = lexer_next(lx);
  if (t.token_type != k) {
    lex_error(lx, t.location.line, t.location.col, "expected %s, got %s",
              token_kind_name(k), token_kind_name(t.token_type));
    t.token_type = TK_ERROR;
  }
  return t;
}

token_t *lexer_tokenize_all(lexer_t *lx, arena_t *a) {
  token_t *arr = NULL;
  for (;;) {
    token_t t = lexer_next(lx);
    da_push(a, arr, t);
    if (t.token_type == TK_EOF)
      break;
  }
  return arr;
}

const char *token_kind_name(token_kind_t k) {
  static const char *NAMES[TK_KIND_COUNT] = {
      [TK_INT_LIT] = "integer literal",
      [TK_REAL_LIT] = "real literal",
      [TK_STRING_LIT] = "string literal",
      [TK_CHAR_LIT] = "char literal",
      [TK_IDENT] = "identifier",
      [TK_COLON] = "':'",
      [TK_SEMICOLON] = "';'",
      [TK_COMMA] = "','",
      [TK_DOT] = "'.'",
      [TK_DOTDOT] = "'..'",
      [TK_LPAREN] = "'('",
      [TK_RPAREN] = "')'",
      [TK_LBRACKET] = "'['",
      [TK_RBRACKET] = "']'",
      [TK_LANGLE] = "'<'",
      [TK_RANGLE] = "'>'",
      [TK_ARROW] = "'<-'",
      [TK_PLUS] = "'+'",
      [TK_MINUS] = "'-'",
      [TK_STAR] = "'*'",
      [TK_SLASH] = "'/'",
      [TK_BANG] = "'!'",
      [TK_EQ] = "'=='",
      [TK_NEQ] = "'!='",
      [TK_LTE] = "'<='",
      [TK_GTE] = "'>='",
      [KW_ALGO] = "KW_ALGO",
      [KW_MODULE] = "KW_MODULE",
      [KW_IMPORT] = "KW_IMPORT",
      [KW_BEGIN] = "KW_BEGIN",
      [KW_END] = "KW_END",
      [KW_VAR] = "KW_VAR",
      [KW_CONST] = "KW_CONST",
      [KW_TYPE] = "KW_TYPE",
      [KW_STRUCT] = "KW_STRUCT",
      [KW_ENDSTRUCT] = "KW_ENDSTRUCT",
      [KW_FUNCTION] = "KW_FUNCTION",
      [KW_ENDFUNCTION] = "KW_ENDFUNCTION",
      [KW_METHOD] = "KW_METHOD",
      [KW_ENDMETHOD] = "KW_ENDMETHOD",
      [KW_RETURN] = "KW_RETURN",
      [KW_REF] = "KW_REF",
      [KW_IF] = "KW_IF",
      [KW_THEN] = "KW_THEN",
      [KW_ELSEIF] = "KW_ELSEIF",
      [KW_ELSE] = "KW_ELSE",
      [KW_ENDIF] = "KW_ENDIF",
      [KW_WHILE] = "KW_WHILE",
      [KW_DO] = "KW_DO",
      [KW_ENDWHILE] = "KW_ENDWHILE",
      [KW_REPEAT] = "KW_REPEAT",
      [KW_UNTIL] = "KW_UNTIL",
      [KW_FOR] = "KW_FOR",
      [KW_TO] = "KW_TO",
      [KW_STEP] = "KW_STEP",
      [KW_ENDFOR] = "KW_ENDFOR",
      [KW_INT] = "KW_INT",
      [KW_REAL] = "KW_REAL",
      [KW_STRING] = "KW_STRING",
      [KW_CHAR] = "KW_CHAR",
      [KW_BOOL] = "KW_BOOL",
      [KW_NUMBER] = "KW_NUMBER",
      [KW_ARRAY] = "KW_ARRAY",
      [KW_OF] = "KW_OF",
      [KW_FILE] = "KW_FILE",
      [KW_SOCKET] = "KW_SOCKET",
      [KW_PTR] = "KW_PTR",
      [KW_CAST] = "KW_CAST",
      [KW_WRITE] = "KW_WRITE",
      [KW_READ] = "KW_READ",
      [KW_FILE_OPEN] = "KW_FILE_OPEN",
      [KW_FILE_CLOSE] = "KW_FILE_CLOSE",
      [KW_FILE_WRITE_LINE] = "KW_FILE_WRITE_LINE",
      [KW_FILE_READ_LINE] = "KW_FILE_READ_LINE",
      [KW_FILE_SEEK] = "KW_FILE_SEEK",
      [KW_FILE_DELETE] = "KW_FILE_DELETE",
      [KW_FILE_EXISTS] = "KW_FILE_EXISTS",
      [KW_FILE_READ] = "KW_FILE_READ (mode)",
      [KW_FILE_WRITE] = "KW_FILE_WRITE (mode)",
      [KW_FILE_APPEND] = "KW_FILE_APPEND",
      [KW_FILE_READ_WRITE] = "KW_FILE_READ_WRITE",
      [KW_NET_CONNECT] = "KW_NET_CONNECT",
      [KW_NET_LISTEN] = "KW_NET_LISTEN",
      [KW_NET_ACCEPT] = "KW_NET_ACCEPT",
      [KW_NET_SEND] = "KW_NET_SEND",
      [KW_NET_RECEIVE] = "KW_NET_RECEIVE",
      [KW_NET_CLOSE] = "KW_NET_CLOSE",
      [KW_NET_HTTP_GET] = "KW_NET_HTTP_GET",
      [KW_NET_HTTP_POST] = "KW_NET_HTTP_POST",
      [KW_AND] = "KW_AND",
      [KW_OR] = "KW_OR",
      [KW_NOT] = "KW_NOT",
      [KW_TRUE] = "KW_TRUE",
      [KW_FALSE] = "KW_FALSE",
      [KW_NULL] = "KW_NULL",
      [KW_NIL] = "KW_NIL",
      [KW_MOD] = "KW_MOD",
      [KW_GFX_INIT_WINDOW] = "KW_GFX_INIT_WINDOW",
      [KW_GFX_CLOSE_WINDOW] = "KW_GFX_CLOSE_WINDOW",
      [KW_GFX_SHOULD_CLOSE] = "KW_GFX_SHOULD_CLOSE",
      [KW_GFX_SET_FPS] = "KW_GFX_SET_FPS",
      [KW_GFX_GET_FPS] = "KW_GFX_GET_FPS",
      [KW_GFX_BEGIN_DRAW] = "KW_GFX_BEGIN_DRAW",
      [KW_GFX_END_DRAW] = "KW_GFX_END_DRAW",
      [KW_GFX_CLEAR] = "KW_GFX_CLEAR",
      [TK_EOF] = "<EOF>",
      [TK_ERROR] = "<ERROR>",
  };
  if (k < TK_KIND_COUNT && NAMES[k])
    return NAMES[k];
  return "<?>";
}
