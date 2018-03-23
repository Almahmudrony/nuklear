#pragma once

#ifndef NK_POOL_DEFAULT_CAPACITY
#define NK_POOL_DEFAULT_CAPACITY 16
#endif

#ifndef NK_DEFAULT_COMMAND_BUFFER_SIZE
#define NK_DEFAULT_COMMAND_BUFFER_SIZE (4*1024)
#endif

#ifndef NK_BUFFER_DEFAULT_INITIAL_SIZE
#define NK_BUFFER_DEFAULT_INITIAL_SIZE (4*1024)
#endif

/* standard library headers */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
#include <stdlib.h> /* malloc, free */
#endif
#ifdef NK_INCLUDE_STANDARD_IO
#include <stdio.h> /* fopen, fclose,... */
#endif
#ifdef NK_INCLUDE_STANDARD_VARARGS
#include <stdarg.h> /* valist, va_start, va_end, ... */
#endif
#ifndef NK_ASSERT
#include <assert.h>
#define NK_ASSERT(expr) assert(expr)
#endif

#ifndef NK_MEMSET
#define NK_MEMSET nk_memset
#endif
#ifndef NK_MEMCPY
#define NK_MEMCPY nk_memcopy
#endif
#ifndef NK_SQRT
#define NK_SQRT nk_sqrt
#endif
#ifndef NK_SIN
#define NK_SIN nk_sin
#endif
#ifndef NK_COS
#define NK_COS nk_cos
#endif
#ifndef NK_STRTOD
#define NK_STRTOD nk_strtod
#endif
#ifndef NK_DTOA
#define NK_DTOA nk_dtoa
#endif

#define NK_DEFAULT (-1)

#ifndef NK_VSNPRINTF
/* If your compiler does support `vsnprintf` I would highly recommend
 * defining this to vsnprintf instead since `vsprintf` is basically
 * unbelievable unsafe and should *NEVER* be used. But I have to support
 * it since C89 only provides this unsafe version. */
  #if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) ||\
      (defined(__cplusplus) && (__cplusplus >= 201103L)) || \
      (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L)) ||\
      (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE >= 500)) ||\
       defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
      #define NK_VSNPRINTF(s,n,f,a) vsnprintf(s,n,f,a)
  #else
    #define NK_VSNPRINTF(s,n,f,a) vsprintf(s,f,a)
  #endif
#endif

#define NK_SCHAR_MIN (-127)
#define NK_SCHAR_MAX 127
#define NK_UCHAR_MIN 0
#define NK_UCHAR_MAX 256
#define NK_SSHORT_MIN (-32767)
#define NK_SSHORT_MAX 32767
#define NK_USHORT_MIN 0
#define NK_USHORT_MAX 65535
#define NK_SINT_MIN (-2147483647)
#define NK_SINT_MAX 2147483647
#define NK_UINT_MIN 0
#define NK_UINT_MAX 4294967295u

/* Make sure correct type size:
 * This will fire with a negative subscript error if the type sizes
 * are set incorrectly by the compiler, and compile out if not */
NK_STATIC_ASSERT(sizeof(nk_size) >= sizeof(void*));
NK_STATIC_ASSERT(sizeof(nk_ptr) == sizeof(void*));
NK_STATIC_ASSERT(sizeof(nk_flags) >= 4);
NK_STATIC_ASSERT(sizeof(nk_rune) >= 4);
NK_STATIC_ASSERT(sizeof(nk_ushort) == 2);
NK_STATIC_ASSERT(sizeof(nk_short) == 2);
NK_STATIC_ASSERT(sizeof(nk_uint) == 4);
NK_STATIC_ASSERT(sizeof(nk_int) == 4);
NK_STATIC_ASSERT(sizeof(nk_byte) == 1);

extern const struct nk_rect nk_null_rect;
#define NK_FLOAT_PRECISION 0.00000000000001

extern const struct nk_color nk_red;
extern const struct nk_color nk_green;
extern const struct nk_color nk_blue;
extern const struct nk_color nk_white;
extern const struct nk_color nk_black;
extern const struct nk_color nk_yellow;

enum {
    NK_DO_NOT_STOP_ON_NEW_LINE,
    NK_STOP_ON_NEW_LINE
};
enum nk_toggle_type {
    NK_TOGGLE_CHECK,
    NK_TOGGLE_OPTION
};


/* Buffer */
void* nk_buffer_alloc(struct nk_buffer *b, enum nk_buffer_allocation_type type, nk_size size, nk_size align);
/* Font */
struct nk_vec2 nk_text_calculate_text_bounds(const struct nk_user_font *font,
                                             const char *begin, int byte_len,
                                             float row_height,
                                             const char **remaining,
                                             struct nk_vec2 *out_offset,
                                             int *glyphs, int op);
/* Page element */
struct nk_page_element* nk_create_page_element(struct nk_context *ctx);
/* Panel */
struct nk_vec2 nk_panel_get_padding(const struct nk_style *style, enum nk_panel_type type);
void* nk_create_panel(struct nk_context *ctx);
/* Pool */
struct nk_page_element* nk_pool_alloc(struct nk_pool *pool);
/* Table */
void nk_free_table(struct nk_context *ctx, struct nk_table *tbl);
void nk_remove_table(struct nk_window *win, struct nk_table *tbl);
nk_uint* nk_find_value(struct nk_window *win, nk_hash name);
nk_uint* nk_add_value(struct nk_context *ctx, struct nk_window *win, nk_hash name, nk_uint value);
/* Text */
struct nk_text {
    struct nk_vec2 padding;
    struct nk_color background;
    struct nk_color text;
};
/* Util */
struct nk_rect nk_shrink_rect(struct nk_rect r, float amount);
struct nk_rect nk_pad_rect(struct nk_rect r, struct nk_vec2 pad);
/* Window */
void nk_free_window(struct nk_context *ctx, struct nk_window *win);
void* nk_create_window(struct nk_context *ctx);
