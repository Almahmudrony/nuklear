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

static const struct nk_color nk_red = {255,0,0,255};
static const struct nk_color nk_green = {0,255,0,255};
static const struct nk_color nk_blue = {0,0,255,255};
static const struct nk_color nk_white = {255,255,255,255};
static const struct nk_color nk_black = {0,0,0,255};
static const struct nk_color nk_yellow = {255,255,0,255};


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
void nk_start(struct nk_context *ctx, struct nk_window *win);
void nk_finish(struct nk_context *ctx, struct nk_window *win);
/* Button */
int nk_do_button_symbol(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect bounds,
    enum nk_symbol_type symbol, enum nk_button_behavior behavior,
    const struct nk_style_button *style, const struct nk_input *in,
    const struct nk_user_font *font);
int nk_button_behavior(nk_flags *state, struct nk_rect r,
    const struct nk_input *i, enum nk_button_behavior behavior);
int nk_do_button_text_symbol(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect bounds,
    enum nk_symbol_type symbol, const char *str, int len, nk_flags align,
    enum nk_button_behavior behavior, const struct nk_style_button *style,
    const struct nk_user_font *font, const struct nk_input *in);
int nk_do_button_text_image(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect bounds,
    struct nk_image img, const char* str, int len, nk_flags align,
    enum nk_button_behavior behavior, const struct nk_style_button *style,
    const struct nk_user_font *font, const struct nk_input *in);
int nk_do_button_image(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect bounds,
    struct nk_image img, enum nk_button_behavior b,
    const struct nk_style_button *style, const struct nk_input *in);
int nk_do_button_text(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect bounds,
    const char *string, int len, nk_flags align, enum nk_button_behavior behavior,
    const struct nk_style_button *style, const struct nk_input *in,
    const struct nk_user_font *font);
int nk_nonblock_begin(struct nk_context *ctx,
    nk_flags flags, struct nk_rect body, struct nk_rect header,
    enum nk_panel_type panel_type);
void nk_draw_symbol(struct nk_command_buffer *out, enum nk_symbol_type type,
    struct nk_rect content, struct nk_color background, struct nk_color foreground,
    float border_width, const struct nk_user_font *font);
void nk_draw_button_symbol(struct nk_command_buffer *out,
    const struct nk_rect *bounds, const struct nk_rect *content,
    nk_flags state, const struct nk_style_button *style,
    enum nk_symbol_type type, const struct nk_user_font *font);
/* Checkbox */
int nk_do_toggle(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect r,
    int *active, const char *str, int len, enum nk_toggle_type type,
    const struct nk_style_toggle *style, const struct nk_input *in,
    const struct nk_user_font *font);
/* Draw */
void nk_command_buffer_init(struct nk_command_buffer *cmdbuf, struct nk_buffer *buffer, enum nk_command_clipping clip);
void nk_command_buffer_reset(struct nk_command_buffer *buffer);
/* Edit */
nk_flags nk_do_edit(nk_flags *state, struct nk_command_buffer *out,
    struct nk_rect bounds, nk_flags flags, nk_plugin_filter filter,
    struct nk_text_edit *edit, const struct nk_style_edit *style,
    struct nk_input *in, const struct nk_user_font *font);
/* Font */
struct nk_vec2 nk_text_calculate_text_bounds(const struct nk_user_font *font,
                                             const char *begin, int byte_len,
                                             float row_height,
                                             const char **remaining,
                                             struct nk_vec2 *out_offset,
                                             int *glyphs, int op);
/* Image */
void nk_unify(struct nk_rect *clip, const struct nk_rect *a, float x0, float y0, float x1, float y1);
int nk_text_clamp(const struct nk_user_font *font, const char *text,
    int text_len, float space, int *glyphs, float *text_width,
    nk_rune *sep_list, int sep_count);
/* Layout */
void nk_layout_peek(struct nk_rect *bounds, struct nk_context *ctx);
/* Page element */
struct nk_page_element* nk_create_page_element(struct nk_context *ctx);
void nk_free_page_element(struct nk_context *ctx, struct nk_page_element *elem);
/* Panel */
struct nk_vec2 nk_panel_get_padding(const struct nk_style *style, enum nk_panel_type type);
void* nk_create_panel(struct nk_context *ctx);
void nk_free_panel(struct nk_context *ctx, struct nk_panel *pan);
int nk_panel_begin(struct nk_context *ctx, const char *title, enum nk_panel_type panel_type);
void nk_panel_end(struct nk_context *ctx);
void nk_panel_alloc_space(struct nk_rect *bounds, const struct nk_context *ctx);
void nk_panel_alloc_row(const struct nk_context *ctx, struct nk_window *win);
/* Pool */
struct nk_page_element* nk_pool_alloc(struct nk_pool *pool);
/* Popup */
void nk_start_popup(struct nk_context *ctx, struct nk_window *win);
void nk_finish_popup(struct nk_context *ctx, struct nk_window *win);
/* Scrollbar */
float nk_do_scrollbarv(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect scroll, int has_scrolling,
    float offset, float target, float step, float button_pixel_inc,
    const struct nk_style_scrollbar *style, struct nk_input *in,
    const struct nk_user_font *font);
float nk_do_scrollbarh(nk_flags *state,
    struct nk_command_buffer *out, struct nk_rect scroll, int has_scrolling,
    float offset, float target, float step, float button_pixel_inc,
    const struct nk_style_scrollbar *style, struct nk_input *in,
    const struct nk_user_font *font);
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
void nk_widget_text(struct nk_command_buffer *o, struct nk_rect b,
    const char *string, int len, const struct nk_text *t,
    nk_flags a, const struct nk_user_font *f);
void nk_textedit_clear_state(struct nk_text_edit *state, enum nk_text_edit_type type,
    nk_plugin_filter filter);
void nk_textedit_drag(struct nk_text_edit *state, float x, float y,
    const struct nk_user_font *font, float row_height);
void nk_textedit_click(struct nk_text_edit *state, float x, float y,
    const struct nk_user_font *font, float row_height);
/* Tree */
int nk_tree_state_base(struct nk_context *ctx, enum nk_tree_type type,
    struct nk_image *img, const char *title, enum nk_collapse_states *state);
void nk_textedit_key(struct nk_text_edit *state, enum nk_keys key, int shift_mod,
    const struct nk_user_font *font, float row_height);
/* Util */
struct nk_rect nk_shrink_rect(struct nk_rect r, float amount);
struct nk_rect nk_pad_rect(struct nk_rect r, struct nk_vec2 pad);
void nk_zero(void *ptr, nk_size size);
void* nk_memcopy(void *dst0, const void *src0, nk_size length);
void nk_memset(void *ptr, int c0, nk_size size);
nk_uint nk_round_up_pow2(nk_uint v);
int nk_ifloorf(float x);
int nk_iceilf(float x);
int nk_string_float_limit(char *string, int prec);
char* nk_dtoa(char *s, double n);
char* nk_itoa(char *s, long n);
float nk_inv_sqrt(float number);
float nk_sqrt(float x);
float nk_sin(float x);
float nk_cos(float x);
#ifdef NK_INCLUDE_STANDARD_IO
char* nk_file_load(const char* path, nk_size* siz, struct nk_allocator *alloc);
#endif
/* Widget */
#define nk_widget_state_reset(s)\
    if ((*(s)) & NK_WIDGET_STATE_MODIFIED)\
        (*(s)) = NK_WIDGET_STATE_INACTIVE|NK_WIDGET_STATE_MODIFIED;\
    else (*(s)) = NK_WIDGET_STATE_INACTIVE;
/* Window */
void nk_free_window(struct nk_context *ctx, struct nk_window *win);
void* nk_create_window(struct nk_context *ctx);
void nk_remove_window(struct nk_context *ctx, struct nk_window *win);
void nk_pool_init(struct nk_pool *pool, struct nk_allocator *alloc,
    unsigned int capacity);
void nk_pool_init_fixed(struct nk_pool *pool, void *memory, nk_size size);
void nk_pool_free(struct nk_pool *pool);
