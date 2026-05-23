/**
 * AUTHOR: NKONO NDEME Miguel
 * arena.h — Region-based memory allocator for AlgLang compiler
 */

#ifndef alglang_arena_h
#define alglang_arena_h

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define ARENA_ALIGN 16
#define ARENA_ALIGN 8 // for a 32 bit machine

static inline size_t arena_align_up(size_t n) {
  return (n + (ARENA_ALIGN - 1)) & ~(size_t)(ARENA_ALIGN - 1);
}

#define ARENA_DEFAULT_CHUNK (1024 * 1024)

typedef struct _arena_chunk_ {
  struct _arena_chunk_ *next;
  size_t size;
  size_t used;
} arena_chunk_t;

typedef struct _arena_ {
  arena_chunk_t *head;
  size_t chunk_size;
} arena_t;

/* Create a new arena.  Pass 0 for chunk_size to get the default. */
static inline arena_t arena_new(size_t chunk_size) {
  arena_t a;
  a.chunk_size = chunk_size != 0 ? chunk_size : ARENA_DEFAULT_CHUNK;
  a.head = NULL;
  return a;
}

/* Allocate a fresh chunk and link it at the front of the arena linked list. */
static inline arena_chunk_t *arena_grow(arena_t *a, size_t needed) {
  size_t sz = a->chunk_size > needed ? a->chunk_size : needed;
  sz = arena_align_up(sz); /* align the size needed to be a multiple of 8 bit */

  arena_chunk_t *c = (arena_chunk_t *)malloc(sizeof(arena_chunk_t) + sz);
  if (!c) {
    fputs("alglang: out of memory\n", stderr);
    abort();
  }

  c->size = sz;
  c->used = 0;

  /* insert the new allocated chunk of memory at the top of the arena memory
   * linked list. */
  c->next = a->head;
  a->head = c;

  return c;
}

/* Allocate `size` bytes from the arena (zero-initialised). */
static inline void *arena_alloc(arena_t *a, size_t size) {
  size = arena_align_up(size);

  arena_chunk_t *c = a->head;
  if (!c ||
      c->used + size >
          c->size) { /* if the arena is empty or the there is no more size. */
    c = arena_grow(a, size);
  }

  /* get 'size' bytes of memory */
  void *ptr = (char *)(c + 1) + c->used;
  c->used += size;
  memset(ptr, 0, size);
  return ptr;
}

/* Allocate and copy a string into the arena. */
static inline char *arena_strdup(arena_t *a, const char *s) {
  size_t len = strlen(s) + 1; /* +1 for `\0` */
  char *p = (char *)arena_alloc(a, len);
  memcpy(p, s, len);
  return p;
}

/* Allocate and copy a string slice [begin, end) into the arena. */
static inline char *arena_strndup(arena_t *a, const char *s, size_t len) {
  char *p = (char *)arena_alloc(a, len + 1);
  memcpy(p, s, len);
  p[len] = '\0';
  return p;
}

/* Checkpoint / reset: free all chunks *after* a saved head pointer.
 * Use for temporary scratch allocations within a phase.
 * This is a snapchot of the arena at a certain time. */
typedef struct _arena_checkpoint_ {
  arena_chunk_t *head;
  size_t used;
} arena_checkpoint_t;

static inline arena_checkpoint_t arena_checkpoint(arena_t *a) {
  arena_checkpoint_t cp;
  cp.head = a->head;
  cp.used = a->head ? a->head->used : 0;
  return cp;
}

static inline void arena_reset_to(arena_t *a, arena_checkpoint_t cp) {
  /* Free any chunks allocated after the checkpoint */
  while (a->head && a->head != cp.head) {
    arena_chunk_t *dead = a->head;
    a->head = dead->next;
    free(dead);
  }

  if (a->head)
    a->head->used = cp.used;
}

/* Destroy the arena entirely and free all chunks. */
static inline void arena_destroy(arena_t *a) {
  arena_chunk_t *c = a->head;

  while (c) {
    arena_chunk_t *next = c->next;
    free(c);
    c = next;
  }

  a->head = NULL;
}

/*
 * FAT-POINTER DYNAMIC ARRAY  (DynArray)
 *
 * The header lives in the same allocation as the data, BEFORE it.
 * The user-facing pointer T* points past the header.
 *
 * Naming convention: da_* macros take the **user pointer** (T*), not the
 * header.  NULL is a valid empty array (da_len == 0, da_cap == 0).
 **/

typedef struct _dyn_array_header_ {
  size_t length;
  size_t capacity;
} dyn_array_header_t;

/* Retrieve the header given the user pointer. */
#define da_hdr(arr) ((dyn_array_header_t *)(arr) - 1)

/* Number of elements currently stored. */
#define da_len(arr) ((arr) ? da_hdr(arr)->length : (size_t)0)

/* Current capacity. */
#define da_cap(arr) ((arr) ? da_hdr(arr)->capacity : (size_t)0)

/* Internal: grow the backing allocation via the arena.
 * Returns the new user pointer (may differ from old). */
static inline void *_da_grow(arena_t *a, void *arr, size_t elem_size,
                             size_t new_cap) {
  size_t array_length = da_len(arr);
  size_t alloc = sizeof(dyn_array_header_t) + new_cap * elem_size;
  alloc = arena_align_up(alloc);

  /* Arena never frees, so we always get a fresh block and copy data. */
  dyn_array_header_t *header = (dyn_array_header_t *)arena_alloc(a, alloc);
  header->length = array_length;
  header->capacity = new_cap;
  if (arr && array_length > 0)
    memcpy(header + 1, arr, array_length * elem_size);

  return (void *)(header + 1);
}

/* Ensure at least `min_cap` slots are available. */
#define da_reserve(a, arr, min_cap)                                            \
  do {                                                                         \
    size_t _mc = (size_t)(min_cap);                                            \
    if (da_cap(arr) < _mc) {                                                   \
      size_t _nc = da_cap(arr) ? da_cap(arr) * 2 : 8;                          \
      if (_nc < _mc)                                                           \
        _nc = _mc;                                                             \
      (arr) = _da_grow((a), (arr), sizeof(*(arr)), _nc);                       \
    }                                                                          \
  } while (0)

/* Push a single element onto the array. */
#define da_push(a, arr, val)                                                   \
  do {                                                                         \
    da_reserve((a), (arr), da_len(arr) + 1);                                   \
    (arr)[da_hdr(arr)->length++] = (val);                                      \
  } while (0)

/* Pop the last element (does NOT shrink allocation). */
#define da_pop(arr) ((arr)[--da_hdr(arr)->length])

/* Direct index (no bounds checking in release). */
#define da_at(arr, i) ((arr)[i])

/* Reset length to zero (keeps capacity). */
#define da_clear(arr)                                                          \
  do {                                                                         \
    if (arr)                                                                   \
      da_hdr(arr)->length = 0;                                                 \
  } while (0)

/* Iterate: for (size_t i = 0; i < da_len(arr); i++) { arr[i] ... } */

/*
 * fat-pointer string builder
 * Simple growable string backed by an arena.
 **/
typedef char *string_builder_t;

static inline void sb_append(arena_t *a, string_builder_t *sb, const char *s,
                             size_t len) {
  size_t old = da_len(*sb);
  da_reserve(a, *sb, old + len + 1);
  memcpy((*sb) + old, s, len);
  da_hdr(*sb)->length += len;
  (*sb)[da_hdr(*sb)->length] = '\0';
}

#define sb_append_cstr(a, sb, s) sb_append((a), (sb), (s), strlen(s))
#define sb_append_char(a, sb, c)                                               \
  do {                                                                         \
    char _c = (c);                                                             \
    sb_append((a), (sb), &_c, 1);                                              \
  } while (0)

#define sb_to_cstr(sb) ((sb) ? (sb) : "")

#endif /* alglang_arena_h */
