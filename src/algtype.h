/**
 * AUTHOR: NKONO NDEME Miguel
 * algtype.h - the type define in the algolang language.
 */

#ifndef alglang_type_h
#define alglang_type_h

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  VAL_NIL,
  VAL_BOOL,
  VAL_INT,
  VAL_REAL,
  VAL_OBJECT,
} value_type_t;

typedef struct _object_ object_t;

typedef struct _value_ {
  value_type_t type;
  union {
    bool boolean;
    int64_t integer;
    double real;
    object_t *object;
  } as;
} value_t;

// All heap-allocated types share this header
typedef enum {
  OBJ_STRING,
  OBJ_ARRAY,
  OBJ_STRUCT,
  OBJ_FUNCTION,
  OBJ_CLOSURE,
  OBJ_NATIVE,
} object_type_t;

typedef struct _object_ {
  object_type_t type;
  /* for when implemented garbase collector. */
  /* bool marked; */
  /* struct _object_ *next; */
} object_t;

typedef struct {
  object_t obj;
  int length;
  int capacity;
  char *chars;
  uint32_t hash;
} obj_string_t;

typedef struct {
  object_t obj;
  int length;
  value_t *elements;
} obj_array_t;

#endif // alglang_type_h
