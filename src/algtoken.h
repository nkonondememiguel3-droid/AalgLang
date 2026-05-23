/**
 * AUTHOR: NKONO NDEME Miguel
 * token.h — Token kinds and the Token struct for AlgLang
 *
 * Keywords are language-agnostic (KW_*).  The actual surface words
 * ("si", "alors", "if", "then" …) come from a .lang mapping file and
 * are resolved by the lexer at startup; internally every keyword is
 * represented by its KW_ constant.
 */

#ifndef alglang_token_h
#define alglang_token_h

#include <stddef.h>
#include <stdint.h>

typedef enum TokenKind {
  TK_INT_LIT,
  TK_REAL_LIT,
  TK_STRING_LIT,
  TK_CHAR_LIT,

  TK_IDENT,

  TK_COLON,
  TK_SEMICOLON,
  TK_COMMA,
  TK_DOT,
  TK_DOTDOT,
  TK_LPAREN,
  TK_RPAREN,
  TK_LBRACKET,
  TK_RBRACKET,
  TK_LANGLE,
  TK_RANGLE,
  TK_ARROW,
  TK_PLUS,
  TK_MINUS,
  TK_STAR,
  TK_SLASH,
  TK_BANG,
  TK_EQ,
  TK_NEQ,
  TK_LTE,
  TK_GTE,

  KW_ALGO,
  KW_MODULE,
  KW_IMPORT,
  KW_BEGIN,
  KW_END,
  KW_VAR,
  KW_CONST,
  KW_TYPE,
  KW_STRUCT,
  KW_ENDSTRUCT,

  KW_FUNCTION,
  KW_ENDFUNCTION,
  KW_METHOD,
  KW_ENDMETHOD,
  KW_RETURN,
  KW_REF,

  KW_IF,
  KW_THEN,
  KW_ELSEIF,
  KW_ELSE,
  KW_ENDIF,
  KW_WHILE,
  KW_DO,
  KW_ENDWHILE,
  KW_REPEAT,
  KW_UNTIL,
  KW_FOR,
  KW_TO,
  KW_STEP,
  KW_ENDFOR,

  KW_INT,
  KW_REAL,
  KW_STRING,
  KW_CHAR,
  KW_BOOL,
  KW_NUMBER,
  KW_ARRAY,
  KW_OF,
  KW_FILE,
  KW_SOCKET,
  KW_PTR,
  KW_CAST,

  KW_WRITE,
  KW_READ,

  KW_FILE_OPEN,
  KW_FILE_CLOSE,
  KW_FILE_WRITE_LINE,
  KW_FILE_READ_LINE,
  KW_FILE_SEEK,
  KW_FILE_DELETE,
  KW_FILE_EXISTS,
  KW_FILE_READ,
  KW_FILE_WRITE,
  KW_FILE_APPEND,
  KW_FILE_READ_WRITE,

  KW_NET_CONNECT,
  KW_NET_LISTEN,
  KW_NET_ACCEPT,
  KW_NET_SEND,
  KW_NET_RECEIVE,
  KW_NET_CLOSE,
  KW_NET_HTTP_GET,
  KW_NET_HTTP_POST,

  KW_AND,
  KW_OR,
  KW_NOT,
  KW_TRUE,
  KW_FALSE,
  KW_NULL,
  KW_NIL,
  KW_MOD,

  KW_GFX_INIT_WINDOW,
  KW_GFX_CLOSE_WINDOW,
  KW_GFX_SHOULD_CLOSE,
  KW_GFX_SET_FPS,
  KW_GFX_GET_FPS,
  KW_GFX_BEGIN_DRAW,
  KW_GFX_END_DRAW,
  KW_GFX_CLEAR,

  TK_EOF,
  TK_ERROR,

  TK_KIND_COUNT
} token_kind_t;

typedef struct {
  const char *file;
  uint32_t line;
  uint32_t col;
} src_loc_t;

typedef struct {
  token_kind_t token_type;
  src_loc_t location;

  const char *start;
  size_t length;

  union {
    int64_t integer_value;
    double real_value;
    uint32_t character_value;
  };
} token_t;

const char *token_kind_name(token_kind_t k);

#endif /* alglang_token_h */
