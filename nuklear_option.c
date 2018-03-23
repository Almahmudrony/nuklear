#include "nuklear.h"
#include "nuklear_internal.h"

NK_API int
nk_option_text(struct nk_context *ctx, const char *text, int len, int is_active)
{
    struct nk_window *win;
    struct nk_panel *layout;
    const struct nk_input *in;
    const struct nk_style *style;

    struct nk_rect bounds;
    enum nk_widget_layout_states state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
        return is_active;

    win = ctx->current;
    style = &ctx->style;
    layout = win->layout;

    state = nk_widget(&bounds, ctx);
    if (!state) return state;
    in = (state == NK_WIDGET_ROM || layout->flags & NK_WINDOW_ROM) ? 0 : &ctx->input;
    nk_do_toggle(&ctx->last_widget_state, &win->buffer, bounds, &is_active,
        text, len, NK_TOGGLE_OPTION, &style->option, in, style->font);
    return is_active;
}

NK_API int
nk_radio_text(struct nk_context *ctx, const char *text, int len, int *active)
{
    int old_value;
    NK_ASSERT(ctx);
    NK_ASSERT(text);
    NK_ASSERT(active);
    if (!ctx || !text || !active) return 0;
    old_value = *active;
    *active = nk_option_text(ctx, text, len, old_value);
    return old_value != *active;
}

NK_API int
nk_option_label(struct nk_context *ctx, const char *label, int active)
{return nk_option_text(ctx, label, nk_strlen(label), active);}

NK_API int
nk_radio_label(struct nk_context *ctx, const char *label, int *active)
{return nk_radio_text(ctx, label, nk_strlen(label), active);}
