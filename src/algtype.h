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
} _value_type_t_;

typedef struct _object_ _object_t_;

typedef struct _value_ {
  _value_type_t_ type;
  union {
    bool boolean;
    int64_t integer;
    double real;
    _object_t_ *object;
  } as;
} _value_t_;

// All heap-allocated types share this header
typedef enum {
  OBJ_STRING,
  OBJ_ARRAY,
  OBJ_STRUCT,
  OBJ_FUNCTION,
  OBJ_CLOSURE,
  OBJ_NATIVE,
} _object_type_t_;

typedef struct _object_ {
  _object_type_t_ type;
  bool marked;
  struct _object_ *next;
} _object_t_;

typedef struct {
  _object_t_ obj;
  int length;
  int capacity;
  char *chars;
  uint32_t hash;
} _obj_string_t_;

typedef struct {
  _object_t_ obj;
  int length;
  _value_t_ *elements;
} _obj_array_t_;

#endif // alglang_type_h
