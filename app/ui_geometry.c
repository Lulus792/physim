#include "ui_geometry.h"
#include <stddef.h>
#include <string.h>

#define INITIAL_CAPACITY 4096u
#define DEFAULT_LIMIT (64u * 1024u * 1024u)

ps_result ps_ui_geometry_init(ps_ui_geometry *g, ps_allocator allocator, size_t byte_limit) {
    if (!g || !ps_allocator_valid(allocator) || (byte_limit && byte_limit < 2 * INITIAL_CAPACITY))
        return PS_INVALID;
    *g = (ps_ui_geometry){0};
    g->allocator = allocator;
    g->byte_limit = byte_limit ? byte_limit : DEFAULT_LIMIT;
    /* Keep buffer sizes within Nuklear's unsigned int counts and GLsizei. */
    if (g->byte_limit > DEFAULT_LIMIT)
        g->byte_limit = DEFAULT_LIMIT;
    return PS_OK;
}
void ps_ui_geometry_destroy(ps_ui_geometry *g) {
    if (!g)
        return;
    for (unsigned i = 0; i < 2; i++)
        ps_memory_free(g->allocator, g->memory[i], g->capacity[i]);
    *g = (ps_ui_geometry){0};
}
void ps_ui_geometry_config(struct nk_convert_config *config,
                           const struct nk_draw_null_texture *null_texture) {
    static const struct nk_draw_vertex_layout_element layout[] = {
        {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, offsetof(ps_ui_vertex, p)},
        {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, offsetof(ps_ui_vertex, uv)},
        {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, offsetof(ps_ui_vertex, color)},
        {NK_VERTEX_LAYOUT_END}};
    *config = (struct nk_convert_config){0};
    config->vertex_layout = layout;
    config->vertex_size = sizeof(ps_ui_vertex);
    config->vertex_alignment = 4;
    config->tex_null = *null_texture;
    config->circle_segment_count = 22;
    config->curve_segment_count = 22;
    config->arc_segment_count = 22;
    config->global_alpha = 1;
    config->shape_AA = NK_ANTI_ALIASING_ON;
    config->line_AA = NK_ANTI_ALIASING_ON;
}
static ps_result reserve(ps_ui_geometry *g, unsigned index, size_t needed) {
    if (needed <= g->capacity[index])
        return PS_OK;
    size_t available = g->byte_limit - g->capacity[1 - index];
    if (needed > available)
        return PS_LIMIT;
    size_t capacity = g->capacity[index] ? g->capacity[index] : INITIAL_CAPACITY;
    while (capacity < needed)
        capacity = capacity > available / 2 ? available : capacity * 2;
    void *memory = NULL;
    ps_result result = ps_memory_allocate(g->allocator, capacity, &memory);
    if (result != PS_OK)
        return result;
    ps_memory_free(g->allocator, g->memory[index], g->capacity[index]);
    g->memory[index] = memory;
    g->capacity[index] = capacity;
    g->allocations++;
    return PS_OK;
}
ps_result ps_ui_geometry_convert(ps_ui_geometry *g, struct nk_context *ui,
                                 struct nk_buffer *commands,
                                 const struct nk_draw_null_texture *null_texture) {
    if (!g || !ui || !commands || !null_texture || !ps_allocator_valid(g->allocator))
        return PS_INVALID;
    for (unsigned i = 0; i < 2; i++) {
        ps_result result = reserve(g, i, INITIAL_CAPACITY);
        if (result != PS_OK)
            return result;
    }
    struct nk_convert_config config;
    ps_ui_geometry_config(&config, null_texture);
    for (;;) {
        nk_buffer_clear(commands);
        nk_buffer_init_fixed(&g->vertices, g->memory[0], g->capacity[0]);
        nk_buffer_init_fixed(&g->indices, g->memory[1], g->capacity[1]);
        nk_flags flags = nk_convert(ui, commands, &g->vertices, &g->indices, &config);
        if (flags == NK_CONVERT_SUCCESS)
            return PS_OK;
        if (flags & NK_CONVERT_INVALID_PARAM)
            return PS_INVALID;
        if (flags & NK_CONVERT_COMMAND_BUFFER_FULL)
            return PS_MEMORY;
        struct nk_buffer *buffers[] = {&g->vertices, &g->indices};
        const nk_flags full[] = {NK_CONVERT_VERTEX_BUFFER_FULL, NK_CONVERT_ELEMENT_BUFFER_FULL};
        for (unsigned i = 0; i < 2; i++) {
            if (!(flags & full[i]))
                continue;
            size_t needed = buffers[i]->needed;
            if (needed <= g->capacity[i])
                needed = g->capacity[i] + 1;
            ps_result result = reserve(g, i, needed);
            if (result != PS_OK)
                return result;
        }
    }
}
