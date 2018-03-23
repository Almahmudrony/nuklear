#include "nuklear.h"
#include "nuklear_internal.h"

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
NK_INTERN void* nk_malloc(nk_handle unused, void *old,nk_size size)
{NK_UNUSED(unused); NK_UNUSED(old); return malloc(size);}
NK_INTERN void nk_mfree(nk_handle unused, void *ptr)
{NK_UNUSED(unused); free(ptr);}

NK_API void
nk_buffer_init_default(struct nk_buffer *buffer)
{
    struct nk_allocator alloc;
    alloc.userdata.ptr = 0;
    alloc.alloc = nk_malloc;
    alloc.free = nk_mfree;
    nk_buffer_init(buffer, &alloc, NK_BUFFER_DEFAULT_INITIAL_SIZE);
}
#endif

NK_API void
nk_buffer_init(struct nk_buffer *b, const struct nk_allocator *a,
    nk_size initial_size)
{
    NK_ASSERT(b);
    NK_ASSERT(a);
    NK_ASSERT(initial_size);
    if (!b || !a || !initial_size) return;

    nk_zero(b, sizeof(*b));
    b->type = NK_BUFFER_DYNAMIC;
    b->memory.ptr = a->alloc(a->userdata,0, initial_size);
    b->memory.size = initial_size;
    b->size = initial_size;
    b->grow_factor = 2.0f;
    b->pool = *a;
}

NK_API void
nk_buffer_init_fixed(struct nk_buffer *b, void *m, nk_size size)
{
    NK_ASSERT(b);
    NK_ASSERT(m);
    NK_ASSERT(size);
    if (!b || !m || !size) return;

    nk_zero(b, sizeof(*b));
    b->type = NK_BUFFER_FIXED;
    b->memory.ptr = m;
    b->memory.size = size;
    b->size = size;
}

NK_INTERN void*
nk_buffer_align(void *unaligned, nk_size align, nk_size *alignment,
    enum nk_buffer_allocation_type type)
{
    void *memory = 0;
    switch (type) {
    default:
    case NK_BUFFER_MAX:
    case NK_BUFFER_FRONT:
        if (align) {
            memory = NK_ALIGN_PTR(unaligned, align);
            *alignment = (nk_size)((nk_byte*)memory - (nk_byte*)unaligned);
        } else {
            memory = unaligned;
            *alignment = 0;
        }
        break;
    case NK_BUFFER_BACK:
        if (align) {
            memory = NK_ALIGN_PTR_BACK(unaligned, align);
            *alignment = (nk_size)((nk_byte*)unaligned - (nk_byte*)memory);
        } else {
            memory = unaligned;
            *alignment = 0;
        }
        break;
    }
    return memory;
}

NK_INTERN void*
nk_buffer_realloc(struct nk_buffer *b, nk_size capacity, nk_size *size)
{
    void *temp;
    nk_size buffer_size;

    NK_ASSERT(b);
    NK_ASSERT(size);
    if (!b || !size || !b->pool.alloc || !b->pool.free)
        return 0;

    buffer_size = b->memory.size;
    temp = b->pool.alloc(b->pool.userdata, b->memory.ptr, capacity);
    NK_ASSERT(temp);
    if (!temp) return 0;

    *size = capacity;
    if (temp != b->memory.ptr) {
        NK_MEMCPY(temp, b->memory.ptr, buffer_size);
        b->pool.free(b->pool.userdata, b->memory.ptr);
    }

    if (b->size == buffer_size) {
        /* no back buffer so just set correct size */
        b->size = capacity;
        return temp;
    } else {
        /* copy back buffer to the end of the new buffer */
        void *dst, *src;
        nk_size back_size;
        back_size = buffer_size - b->size;
        dst = nk_ptr_add(void, temp, capacity - back_size);
        src = nk_ptr_add(void, temp, b->size);
        NK_MEMCPY(dst, src, back_size);
        b->size = capacity - back_size;
    }
    return temp;
}

void*
nk_buffer_alloc(struct nk_buffer *b, enum nk_buffer_allocation_type type,
    nk_size size, nk_size align)
{
    int full;
    nk_size alignment;
    void *unaligned;
    void *memory;

    NK_ASSERT(b);
    NK_ASSERT(size);
    if (!b || !size) return 0;
    b->needed += size;

    /* calculate total size with needed alignment + size */
    if (type == NK_BUFFER_FRONT)
        unaligned = nk_ptr_add(void, b->memory.ptr, b->allocated);
    else unaligned = nk_ptr_add(void, b->memory.ptr, b->size - size);
    memory = nk_buffer_align(unaligned, align, &alignment, type);

    /* check if buffer has enough memory*/
    if (type == NK_BUFFER_FRONT)
        full = ((b->allocated + size + alignment) > b->size);
    else full = ((b->size - NK_MIN(b->size,(size + alignment))) <= b->allocated);

    if (full) {
        nk_size capacity;
        if (b->type != NK_BUFFER_DYNAMIC)
            return 0;
        NK_ASSERT(b->pool.alloc && b->pool.free);
        if (b->type != NK_BUFFER_DYNAMIC || !b->pool.alloc || !b->pool.free)
            return 0;

        /* buffer is full so allocate bigger buffer if dynamic */
        capacity = (nk_size)((float)b->memory.size * b->grow_factor);
        capacity = NK_MAX(capacity, nk_round_up_pow2((nk_uint)(b->allocated + size)));
        b->memory.ptr = nk_buffer_realloc(b, capacity, &b->memory.size);
        if (!b->memory.ptr) return 0;

        /* align newly allocated pointer */
        if (type == NK_BUFFER_FRONT)
            unaligned = nk_ptr_add(void, b->memory.ptr, b->allocated);
        else unaligned = nk_ptr_add(void, b->memory.ptr, b->size - size);
        memory = nk_buffer_align(unaligned, align, &alignment, type);
    }
    if (type == NK_BUFFER_FRONT)
        b->allocated += size + alignment;
    else b->size -= (size + alignment);
    b->needed += alignment;
    b->calls++;
    return memory;
}

NK_API void
nk_buffer_push(struct nk_buffer *b, enum nk_buffer_allocation_type type,
    const void *memory, nk_size size, nk_size align)
{
    void *mem = nk_buffer_alloc(b, type, size, align);
    if (!mem) return;
    NK_MEMCPY(mem, memory, size);
}

NK_API void
nk_buffer_mark(struct nk_buffer *buffer, enum nk_buffer_allocation_type type)
{
    NK_ASSERT(buffer);
    if (!buffer) return;
    buffer->marker[type].active = nk_true;
    if (type == NK_BUFFER_BACK)
        buffer->marker[type].offset = buffer->size;
    else buffer->marker[type].offset = buffer->allocated;
}

NK_API void
nk_buffer_reset(struct nk_buffer *buffer, enum nk_buffer_allocation_type type)
{
    NK_ASSERT(buffer);
    if (!buffer) return;
    if (type == NK_BUFFER_BACK) {
        /* reset back buffer either back to marker or empty */
        buffer->needed -= (buffer->memory.size - buffer->marker[type].offset);
        if (buffer->marker[type].active)
            buffer->size = buffer->marker[type].offset;
        else buffer->size = buffer->memory.size;
        buffer->marker[type].active = nk_false;
    } else {
        /* reset front buffer either back to back marker or empty */
        buffer->needed -= (buffer->allocated - buffer->marker[type].offset);
        if (buffer->marker[type].active)
            buffer->allocated = buffer->marker[type].offset;
        else buffer->allocated = 0;
        buffer->marker[type].active = nk_false;
    }
}

NK_API void
nk_buffer_clear(struct nk_buffer *b)
{
    NK_ASSERT(b);
    if (!b) return;
    b->allocated = 0;
    b->size = b->memory.size;
    b->calls = 0;
    b->needed = 0;
}

NK_API void
nk_buffer_free(struct nk_buffer *b)
{
    NK_ASSERT(b);
    if (!b || !b->memory.ptr) return;
    if (b->type == NK_BUFFER_FIXED) return;
    if (!b->pool.free) return;
    NK_ASSERT(b->pool.free);
    b->pool.free(b->pool.userdata, b->memory.ptr);
}

NK_API void
nk_buffer_info(struct nk_memory_status *s, struct nk_buffer *b)
{
    NK_ASSERT(b);
    NK_ASSERT(s);
    if (!s || !b) return;
    s->allocated = b->allocated;
    s->size =  b->memory.size;
    s->needed = b->needed;
    s->memory = b->memory.ptr;
    s->calls = b->calls;
}

NK_API void*
nk_buffer_memory(struct nk_buffer *buffer)
{
    NK_ASSERT(buffer);
    if (!buffer) return 0;
    return buffer->memory.ptr;
}

NK_API const void*
nk_buffer_memory_const(const struct nk_buffer *buffer)
{
    NK_ASSERT(buffer);
    if (!buffer) return 0;
    return buffer->memory.ptr;
}

NK_API nk_size
nk_buffer_total(struct nk_buffer *buffer)
{
    NK_ASSERT(buffer);
    if (!buffer) return 0;
    return buffer->memory.size;
}

NK_INTERN void
nk_start_buffer(struct nk_context *ctx, struct nk_command_buffer *buffer)
{
    NK_ASSERT(ctx);
    NK_ASSERT(buffer);
    if (!ctx || !buffer) return;
    buffer->begin = ctx->memory.allocated;
    buffer->end = buffer->begin;
    buffer->last = buffer->begin;
    buffer->clip = nk_null_rect;
}

NK_INTERN void
nk_start(struct nk_context *ctx, struct nk_window *win)
{
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    nk_start_buffer(ctx, &win->buffer);
}

NK_INTERN void
nk_start_popup(struct nk_context *ctx, struct nk_window *win)
{
    struct nk_popup_buffer *buf;
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if (!ctx || !win) return;

    /* save buffer fill state for popup */
    buf = &win->popup.buf;
    buf->begin = win->buffer.end;
    buf->end = win->buffer.end;
    buf->parent = win->buffer.last;
    buf->last = buf->begin;
    buf->active = nk_true;
}

NK_INTERN void
nk_finish_popup(struct nk_context *ctx, struct nk_window *win)
{
    struct nk_popup_buffer *buf;
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if (!ctx || !win) return;

    buf = &win->popup.buf;
    buf->last = win->buffer.last;
    buf->end = win->buffer.end;
}

NK_INTERN void
nk_finish_buffer(struct nk_context *ctx, struct nk_command_buffer *buffer)
{
    NK_ASSERT(ctx);
    NK_ASSERT(buffer);
    if (!ctx || !buffer) return;
    buffer->end = ctx->memory.allocated;
}

NK_INTERN void
nk_finish(struct nk_context *ctx, struct nk_window *win)
{
    struct nk_popup_buffer *buf;
    struct nk_command *parent_last;
    void *memory;

    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if (!ctx || !win) return;
    nk_finish_buffer(ctx, &win->buffer);
    if (!win->popup.buf.active) return;

    buf = &win->popup.buf;
    memory = ctx->memory.memory.ptr;
    parent_last = nk_ptr_add(struct nk_command, memory, buf->parent);
    parent_last->next = buf->end;
}

NK_INTERN void
nk_build(struct nk_context *ctx)
{
    struct nk_window *it = 0;
    struct nk_command *cmd = 0;
    nk_byte *buffer = 0;

    /* draw cursor overlay */
    if (!ctx->style.cursor_active)
        ctx->style.cursor_active = ctx->style.cursors[NK_CURSOR_ARROW];
    if (ctx->style.cursor_active && !ctx->input.mouse.grabbed && ctx->style.cursor_visible) {
        struct nk_rect mouse_bounds;
        const struct nk_cursor *cursor = ctx->style.cursor_active;
        nk_command_buffer_init(&ctx->overlay, &ctx->memory, NK_CLIPPING_OFF);
        nk_start_buffer(ctx, &ctx->overlay);

        mouse_bounds.x = ctx->input.mouse.pos.x - cursor->offset.x;
        mouse_bounds.y = ctx->input.mouse.pos.y - cursor->offset.y;
        mouse_bounds.w = cursor->size.x;
        mouse_bounds.h = cursor->size.y;

        nk_draw_image(&ctx->overlay, mouse_bounds, &cursor->img, nk_white);
        nk_finish_buffer(ctx, &ctx->overlay);
    }
    /* build one big draw command list out of all window buffers */
    it = ctx->begin;
    buffer = (nk_byte*)ctx->memory.memory.ptr;
    while (it != 0) {
        struct nk_window *next = it->next;
        if (it->buffer.last == it->buffer.begin || (it->flags & NK_WINDOW_HIDDEN)||
            it->seq != ctx->seq)
            goto cont;

        cmd = nk_ptr_add(struct nk_command, buffer, it->buffer.last);
        while (next && ((next->buffer.last == next->buffer.begin) ||
            (next->flags & NK_WINDOW_HIDDEN) || next->seq != ctx->seq))
            next = next->next; /* skip empty command buffers */

        if (next) cmd->next = next->buffer.begin;
        cont: it = next;
    }
    /* append all popup draw commands into lists */
    it = ctx->begin;
    while (it != 0) {
        struct nk_window *next = it->next;
        struct nk_popup_buffer *buf;
        if (!it->popup.buf.active)
            goto skip;

        buf = &it->popup.buf;
        cmd->next = buf->begin;
        cmd = nk_ptr_add(struct nk_command, buffer, buf->last);
        buf->active = nk_false;
        skip: it = next;
    }
    if (cmd) {
        /* append overlay commands */
        if (ctx->overlay.end != ctx->overlay.begin)
            cmd->next = ctx->overlay.begin;
        else cmd->next = ctx->memory.allocated;
    }
}

NK_API const struct nk_command*
nk__begin(struct nk_context *ctx)
{
    struct nk_window *iter;
    nk_byte *buffer;
    NK_ASSERT(ctx);
    if (!ctx) return 0;
    if (!ctx->count) return 0;

    buffer = (nk_byte*)ctx->memory.memory.ptr;
    if (!ctx->build) {
        nk_build(ctx);
        ctx->build = nk_true;
    }
    iter = ctx->begin;
    while (iter && ((iter->buffer.begin == iter->buffer.end) ||
        (iter->flags & NK_WINDOW_HIDDEN) || iter->seq != ctx->seq))
        iter = iter->next;
    if (!iter) return 0;
    return nk_ptr_add_const(struct nk_command, buffer, iter->buffer.begin);
}

NK_API const struct nk_command*
nk__next(struct nk_context *ctx, const struct nk_command *cmd)
{
    nk_byte *buffer;
    const struct nk_command *next;
    NK_ASSERT(ctx);
    if (!ctx || !cmd || !ctx->count) return 0;
    if (cmd->next >= ctx->memory.allocated) return 0;
    buffer = (nk_byte*)ctx->memory.memory.ptr;
    next = nk_ptr_add_const(struct nk_command, buffer, cmd->next);
    return next;
}
