#include "physim/math.h"
#include "png.h"
#include "ui.h"
#include "ui_geometry.h"
#include <SDL3/SDL_opengl.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Load even GL 1.x entrypoints through SDL: no platform GL import library. */
// clang-format off
#define GL_FUNCTIONS(X) \
    X(const GLubyte *, GetString, (GLenum name)) \
    X(void, GetIntegerv, (GLenum name, GLint * value)) \
    X(GLenum, GetError, (void)) \
    X(void, Viewport, (GLint x, GLint y, GLsizei w, GLsizei h)) \
    X(void, Scissor, (GLint x, GLint y, GLsizei w, GLsizei h)) \
    X(void, Enable, (GLenum cap)) \
    X(void, Disable, (GLenum cap)) \
    X(void, BlendFunc, (GLenum src, GLenum dst)) \
    X(void, BlendFuncSeparate, (GLenum src, GLenum dst, GLenum src_alpha, GLenum dst_alpha)) \
    X(void, DepthMask, (GLboolean enabled)) \
    X(void, DepthFunc, (GLenum fn)) \
    X( void, ClearColor, (GLfloat r, GLfloat g, GLfloat b, GLfloat a)) \
    X(void, Clear, (GLbitfield mask)) \
    X(void, GenTextures, (GLsizei n, GLuint * ids)) \
    X(void, DeleteTextures, (GLsizei n, const GLuint *ids)) \
    X(void, BindTexture, (GLenum target, GLuint id)) \
    X(void, TexParameteri, (GLenum target, GLenum name, GLint value)) \
    X( void, TexImage2D, (GLenum target, GLint level, GLint internal, GLsizei w, GLsizei h, GLint border, GLenum format, GLenum type, const void *data)) \
    X(void, ActiveTexture, (GLenum texture)) \
    X(void, PixelStorei, (GLenum name, GLint value)) \
    X(void, ReadPixels, (GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, void *data)) \
    X(GLuint, CreateShader, (GLenum type)) \
    X(void, ShaderSource, (GLuint id, GLsizei n, const GLchar *const *text, const GLint *length)) \
    X(void, CompileShader, (GLuint id)) \
    X(void, GetShaderiv, (GLuint id, GLenum name, GLint * value)) \
    X( void, GetShaderInfoLog, (GLuint id, GLsizei size, GLsizei * length, GLchar * log)) \
    X(void, DeleteShader, (GLuint id)) \
    X(GLuint, CreateProgram, (void)) \
    X( void, AttachShader, (GLuint program, GLuint shader)) \
    X(void, LinkProgram, (GLuint id)) \
    X(void, GetProgramiv, (GLuint id, GLenum name, GLint * value)) \
    X( void, GetProgramInfoLog, (GLuint id, GLsizei size, GLsizei * length, GLchar * log)) \
    X(void, DeleteProgram, (GLuint id)) \
    X(void, UseProgram, (GLuint id)) \
    X(GLint, GetUniformLocation, (GLuint id, const GLchar *name)) \
    X(void, Uniform1i, (GLint loc, GLint value)) \
    X( void, UniformMatrix4fv, (GLint loc, GLsizei n, GLboolean transpose, const GLfloat *matrix)) \
    X(void, GenVertexArrays, (GLsizei n, GLuint * ids)) \
    X(void, BindVertexArray, (GLuint id)) \
    X( void, DeleteVertexArrays, (GLsizei n, const GLuint * ids)) \
    X(void, GenBuffers, (GLsizei n, GLuint * ids)) \
    X(void, BindBuffer, (GLenum target, GLuint id)) \
    X( void, BufferData, (GLenum target, GLsizeiptr size, const void *data, GLenum usage)) \
    X( void, BufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, const void *data)) \
    X(void, DeleteBuffers, (GLsizei n, const GLuint *ids)) \
    X(void, EnableVertexAttribArray, (GLuint index)) \
    X( void, VertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offset)) \
    X(void, DrawElements, (GLenum mode, GLsizei count, GLenum type, const void * indices)) \
    X(void, DrawArrays, (GLenum mode, GLint first, GLsizei count)) \
    X(void, GenFramebuffers, (GLsizei n, GLuint * ids)) \
    X(void, BindFramebuffer, (GLenum target, GLuint id)) \
    X(void, DeleteFramebuffers, (GLsizei n, const GLuint *ids)) \
    X(void, FramebufferTexture2D, (GLenum target, GLenum attachment, GLenum texture_target, GLuint texture, GLint level)) \
    X(GLenum, CheckFramebufferStatus, (GLenum target)) \
    X(void, GenRenderbuffers, (GLsizei n, GLuint * ids)) \
    X(void, BindRenderbuffer, (GLenum target, GLuint id)) \
    X(void, RenderbufferStorage, (GLenum target, GLenum format, GLsizei w, GLsizei h)) \
    X(void, FramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum buffer_target, GLuint buffer)) \
    X(void, RenderbufferStorageMultisample, (GLenum target, GLsizei samples, GLenum format, GLsizei w, GLsizei h)) \
    X(void, BlitFramebuffer, (GLint sx0, GLint sy0, GLint sx1, GLint sy1, GLint dx0, GLint dy0, GLint dx1, GLint dy1, GLbitfield mask, GLenum filter)) \
    X(void, DeleteRenderbuffers, (GLsizei n, const GLuint *ids))
// clang-format on

typedef struct {
    float p[3], n[3], color[4];
} scene_vertex;
typedef struct {
    double depth;
    uint32_t first;
} alpha_triangle;
typedef ps_ui_vertex ui_vertex;
typedef struct {
    ps_vec3 p, n;
} mesh_vertex;
#define SCENE_CAPACITY (600000u)
_Static_assert(SCENE_CAPACITY >= (PS_MAX_OBJECTS * (PS_MAX_SCENE_POINTS - 1) + 35) * 16 * 12,
               "The scene buffer must fit even overlapping maximal polylines");
struct ps_graphics {
    SDL_Window *window;
    float background[3];
    SDL_GLContext context;
#define GL_FIELD(ret, name, args) ret(APIENTRY *name) args;
    GL_FUNCTIONS(GL_FIELD)
#undef GL_FIELD
    GLuint ui_program, scene_program, ui_vao, scene_vao, ui_vbo, ui_ebo, scene_vbo;
    GLuint framebuffer, resolve_framebuffer, scene_texture, color_buffer, depth;
    GLint ui_projection, ui_flip, scene_projection, max_texture;
    int width, height, samples;
    scene_vertex *vertices;
    size_t count;
    ps_mat4 object_transform;
    ps_mat3 object_normal;
    bool transform_active, object_invalid;
    GLuint scene_ebo;
    alpha_triangle *alpha_triangles;
    uint32_t *scene_indices;
    size_t alpha_capacity;
    size_t alpha_triangle_capacity, scene_index_capacity;
    size_t pick_ranges[PS_MAX_OBJECTS + 1];
    uint32_t pick_count;
    bool pick_valid;
    ps_mat4 pick_inverse;
    mesh_vertex sphere[24 * 16 * 6];
    size_t sphere_count;
    ps_ui_render_stats ui_stats;
    ps_scene_render_stats scene_stats;
    bool scene_stats_valid;
    ps_ui_geometry ui_geometry;
};
void ps_graphics_background(ps_graphics *g, uint8_t red, uint8_t green, uint8_t blue) {
    if (g) {
        g->background[0] = red / 255.f;
        g->background[1] = green / 255.f;
        g->background[2] = blue / 255.f;
    }
}
bool ps_graphics_ui_stats(const ps_graphics *g, ps_ui_render_stats *out) {
    if (!g || !out)
        return false;
    *out = g->ui_stats;
    return true;
}
static bool gl_ok(ps_graphics *g, const char *where) {
    GLenum error = g->GetError();
    if (error == GL_NO_ERROR)
        return true;
    SDL_SetError("OpenGL %s: 0x%x", where, error);
    return false;
}
static GLuint shader(ps_graphics *g, GLenum type, const char *source) {
    GLuint id = g->CreateShader(type);
    if (!id) {
        SDL_SetError("OpenGL could not allocate a shader");
        return 0;
    }
    g->ShaderSource(id, 1, &source, NULL);
    g->CompileShader(id);
    GLint ok = 0;
    g->GetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048] = {0};
        g->GetShaderInfoLog(id, sizeof log, NULL, log);
        SDL_SetError("GLSL: %s", log);
        g->DeleteShader(id);
        return 0;
    }
    return id;
}
static GLuint program(ps_graphics *g, const char *vs, const char *fs) {
    GLuint v = shader(g, GL_VERTEX_SHADER, vs), f = shader(g, GL_FRAGMENT_SHADER, fs);
    if (!v || !f) {
        g->DeleteShader(v);
        g->DeleteShader(f);
        return 0;
    }
    GLuint id = g->CreateProgram();
    if (!id) {
        g->DeleteShader(v);
        g->DeleteShader(f);
        SDL_SetError("OpenGL could not allocate a program");
        return 0;
    }
    g->AttachShader(id, v);
    g->AttachShader(id, f);
    g->LinkProgram(id);
    g->DeleteShader(v);
    g->DeleteShader(f);
    GLint ok = 0;
    g->GetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048] = {0};
        g->GetProgramInfoLog(id, sizeof log, NULL, log);
        SDL_SetError("GLSL link: %s", log);
        g->DeleteProgram(id);
        return 0;
    }
    return id;
}
static ps_vec3 sphere_point(int lat, int lon) {
    double p = PS_PI * lat / 16, t = 2 * PS_PI * lon / 24;
    return ps_v3(sin(p) * cos(t), cos(p), sin(p) * sin(t));
}
ps_graphics *ps_graphics_create(SDL_Window *window) {
    ps_graphics *g = calloc(1, sizeof *g);
    if (!g) {
        SDL_SetError("No memory for graphics");
        return NULL;
    }
    g->window = window;
    ps_graphics_background(g, 20, 21, 24);
    ps_ui_geometry_init(&g->ui_geometry, ps_allocator_default(), 0);
    g->context = SDL_GL_CreateContext(window);
    if (!g->context || !SDL_GL_MakeCurrent(window, g->context))
        goto fail_context;
#define PS_GL_LOAD(ret, name, args)                                                                \
    do {                                                                                           \
        SDL_FunctionPointer address = SDL_GL_GetProcAddress("gl" #name);                           \
        if (!address) {                                                                            \
            SDL_SetError("OpenGL entrypoint gl%s unavailable", #name);                             \
            goto fail_context;                                                                     \
        }                                                                                          \
        memcpy(&g->name, &address, sizeof address);                                                \
    } while (0);
    GL_FUNCTIONS(PS_GL_LOAD)
#undef PS_GL_LOAD
    GLint major = 0, minor = 0, profile = 0;
    g->GetIntegerv(GL_MAJOR_VERSION, &major);
    g->GetIntegerv(GL_MINOR_VERSION, &minor);
    g->GetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if ((major < 3 || (major == 3 && minor < 3)) || !(profile & GL_CONTEXT_CORE_PROFILE_BIT)) {
        SDL_SetError("Physim requires OpenGL 3.3 Core");
        goto fail;
    }
    fprintf(stdout, "OpenGL %s | %s\n", g->GetString(GL_VERSION), g->GetString(GL_RENDERER));
    g->GetIntegerv(GL_MAX_TEXTURE_SIZE, &g->max_texture);
    g->GetIntegerv(GL_MAX_SAMPLES, &g->samples);
    if (g->samples > 4)
        g->samples = 4;
    g->ui_program = program(
        g,
        "#version 330 core\nlayout(location=0) in vec2 p; layout(location=1) in vec2 uv;"
        "layout(location=2) in vec4 c; uniform mat4 projection; out vec2 vuv; out vec4 col;"
        "void main(){vuv=uv;col=c;gl_Position=projection*vec4(p,0,1);}",
        "#version 330 core\nuniform sampler2D image; uniform int flip; in vec2 vuv; in vec4 col;"
        "out vec4 result;void main(){vec2 "
        "t=vuv;if(flip!=0)t.y=1-t.y;result=col*texture(image,t);}");
    g->scene_program = program(
        g,
        "#version 330 core\nlayout(location=0) in vec3 p; layout(location=1) in vec3 n;"
        "layout(location=2) in vec4 c; uniform mat4 projection; out vec3 normal; out vec4 col;"
        "void main(){normal=n;col=c;gl_Position=projection*vec4(p,1);}",
        "#version 330 core\nin vec3 normal; in vec4 col; out vec4 result;"
        "void main(){float light=.32+.68*max(dot(normalize(normal),normalize(vec3(-.4,.8,.6))),0);"
        "result=vec4(col.rgb*light,col.a);}");
    if (!g->ui_program || !g->scene_program)
        goto fail;
    g->ui_projection = g->GetUniformLocation(g->ui_program, "projection");
    g->ui_flip = g->GetUniformLocation(g->ui_program, "flip");
    g->scene_projection = g->GetUniformLocation(g->scene_program, "projection");
    g->UseProgram(g->ui_program);
    g->Uniform1i(g->GetUniformLocation(g->ui_program, "image"), 0);
    g->GenVertexArrays(1, &g->ui_vao);
    g->BindVertexArray(g->ui_vao);
    g->GenBuffers(1, &g->ui_vbo);
    g->BindBuffer(GL_ARRAY_BUFFER, g->ui_vbo);
    g->GenBuffers(1, &g->ui_ebo);
    g->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->ui_ebo);
    for (unsigned i = 0; i < 3; i++)
        g->EnableVertexAttribArray(i);
    g->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex),
                           (void *)offsetof(ui_vertex, p));
    g->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex),
                           (void *)offsetof(ui_vertex, uv));
    g->VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ui_vertex),
                           (void *)offsetof(ui_vertex, color));
    g->GenVertexArrays(1, &g->scene_vao);
    g->BindVertexArray(g->scene_vao);
    g->GenBuffers(1, &g->scene_vbo);
    g->GenBuffers(1, &g->scene_ebo);
    g->BindBuffer(GL_ARRAY_BUFFER, g->scene_vbo);
    for (unsigned i = 0; i < 3; i++)
        g->EnableVertexAttribArray(i);
    g->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(scene_vertex),
                           (void *)offsetof(scene_vertex, p));
    g->VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(scene_vertex),
                           (void *)offsetof(scene_vertex, n));
    g->VertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(scene_vertex),
                           (void *)offsetof(scene_vertex, color));
    g->GenFramebuffers(1, &g->framebuffer);
    g->GenFramebuffers(1, &g->resolve_framebuffer);
    g->GenRenderbuffers(1, &g->depth);
    g->GenRenderbuffers(1, &g->color_buffer);
    g->vertices = malloc(SCENE_CAPACITY * sizeof *g->vertices);
    if (!g->vertices) {
        SDL_SetError("No memory for scene vertices");
        goto fail;
    }
    for (int lat = 0; lat < 16; lat++)
        for (int lon = 0; lon < 24; lon++) {
            ps_vec3 p[4] = {sphere_point(lat, lon), sphere_point(lat + 1, lon),
                            sphere_point(lat + 1, lon + 1), sphere_point(lat, lon + 1)};
            const int indices[6] = {0, 1, 2, 0, 2, 3};
            for (int i = 0; i < 6; i++) {
                ps_vec3 v = p[indices[i]];
                g->sphere[g->sphere_count++] = (mesh_vertex){v, v};
            }
        }
    if (!gl_ok(g, "initialization"))
        goto fail;
    SDL_GL_SetSwapInterval(1);
    return g;
fail:
    ps_graphics_destroy(g);
    return NULL;
fail_context:
    if (g->context)
        SDL_GL_DestroyContext(g->context);
    free(g);
    return NULL;
}
bool ps_graphics_make_current(ps_graphics *g) {
    return g && SDL_GL_MakeCurrent(g->window, g->context);
}
void ps_graphics_destroy(ps_graphics *g) {
    if (!g)
        return;
    g->DeleteTextures(1, &g->scene_texture);
    g->DeleteFramebuffers(1, &g->framebuffer);
    g->DeleteFramebuffers(1, &g->resolve_framebuffer);
    g->DeleteRenderbuffers(1, &g->depth);
    g->DeleteRenderbuffers(1, &g->color_buffer);
    g->DeleteBuffers(1, &g->ui_vbo);
    g->DeleteBuffers(1, &g->ui_ebo);
    g->DeleteBuffers(1, &g->scene_vbo);
    g->DeleteBuffers(1, &g->scene_ebo);
    g->DeleteVertexArrays(1, &g->ui_vao);
    g->DeleteVertexArrays(1, &g->scene_vao);
    g->DeleteProgram(g->ui_program);
    g->DeleteProgram(g->scene_program);
    SDL_GL_DestroyContext(g->context);
    ps_ui_geometry_destroy(&g->ui_geometry);
    free(g->vertices);
    free(g->alpha_triangles);
    free(g->scene_indices);
    free(g);
}
unsigned ps_graphics_texture(ps_graphics *g, const void *rgba, int w, int h) {
    if (w < 1 || h < 1 || w > g->max_texture || h > g->max_texture) {
        SDL_SetError("Invalid texture size");
        return 0;
    }
    GLuint id = 0;
    g->GenTextures(1, &id);
    if (!id) {
        SDL_SetError("OpenGL could not allocate a texture");
        return 0;
    }
    g->BindTexture(GL_TEXTURE_2D, id);
    g->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    g->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    g->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    g->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    g->PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    g->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    if (!gl_ok(g, "texture")) {
        g->DeleteTextures(1, &id);
        return 0;
    }
    return id;
}
void ps_graphics_delete_texture(ps_graphics *g, unsigned texture) {
    g->DeleteTextures(1, &texture);
}
static bool ui_target(ps_graphics *g, struct nk_context *ui, struct nk_buffer *commands,
                      const struct nk_draw_null_texture *null_texture, GLuint framebuffer, int w,
                      int h, int pw, int ph) {
    if (w <= 0 || h <= 0 || pw <= 0 || ph <= 0)
        return true;
    Uint64 convert_start = SDL_GetTicksNS();
    g->ui_stats.upload_seconds = 0;
    ps_result converted = ps_ui_geometry_convert(&g->ui_geometry, ui, commands, null_texture);
    struct nk_buffer *vertices = &g->ui_geometry.vertices, *indices = &g->ui_geometry.indices;
    g->ui_stats.conversion_seconds = (double)(SDL_GetTicksNS() - convert_start) / 1e9;
    g->ui_stats.allocations = g->ui_geometry.allocations;
    g->ui_stats.vertex_bytes = vertices->allocated;
    g->ui_stats.index_bytes = indices->allocated;
    g->ui_stats.retained_bytes = g->ui_geometry.capacity[0] + g->ui_geometry.capacity[1];
    if (converted != PS_OK) {
        SDL_SetError("UI geometry conversion: %s", ps_result_string(converted));
        return false;
    }
    g->BindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    g->Viewport(0, 0, pw, ph);
    g->Disable(GL_DEPTH_TEST);
    g->Disable(GL_CULL_FACE);
    g->Disable(GL_SCISSOR_TEST);
    g->ClearColor(g->background[0], g->background[1], g->background[2], 1);
    g->Clear(GL_COLOR_BUFFER_BIT);
    g->Enable(GL_BLEND);
    g->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    g->Enable(GL_SCISSOR_TEST);
    g->UseProgram(g->ui_program);
    g->BindVertexArray(g->ui_vao);
    float matrix[16] = {2.f / w, 0, 0, 0, 0, -2.f / h, 0, 0, 0, 0, -1, 0, -1, 1, 0, 1};
    g->UniformMatrix4fv(g->ui_projection, 1, GL_FALSE, matrix);
    Uint64 upload_start = SDL_GetTicksNS();
    g->BindBuffer(GL_ARRAY_BUFFER, g->ui_vbo);
    /* Orphan the old store so queued draws can finish without overwriting it.
     * Keep
     * high-water capacities; transfer only this frame's used bytes. */
    g->BufferData(GL_ARRAY_BUFFER, (GLsizeiptr)g->ui_geometry.capacity[0], NULL, GL_STREAM_DRAW);
    if (vertices->allocated)
        g->BufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)vertices->allocated,
                         nk_buffer_memory_const(vertices));
    g->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->ui_ebo);
    g->BufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)g->ui_geometry.capacity[1], NULL,
                  GL_STREAM_DRAW);
    if (indices->allocated)
        g->BufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (GLsizeiptr)indices->allocated,
                         nk_buffer_memory_const(indices));
    g->ui_stats.upload_seconds = (double)(SDL_GetTicksNS() - upload_start) / 1e9;
    g->ActiveTexture(GL_TEXTURE0);
    const struct nk_draw_command *cmd;
    size_t offset = 0;
    float sx = (float)pw / w, sy = (float)ph / h;
    nk_draw_foreach(cmd, ui, commands) {
        if (!cmd->elem_count)
            continue;
        int x = (int)floorf(cmd->clip_rect.x * sx),
            y = (int)floorf((h - cmd->clip_rect.y - cmd->clip_rect.h) * sy);
        int cw = (int)ceilf(cmd->clip_rect.w * sx), ch = (int)ceilf(cmd->clip_rect.h * sy);
        if (cw > 0 && ch > 0) {
            g->Scissor(x, y, cw, ch);
            g->BindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            g->Uniform1i(g->ui_flip, (GLuint)cmd->texture.id == g->scene_texture);
            g->DrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count,
                            sizeof(nk_draw_index) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                            (void *)offset);
        }
        offset += cmd->elem_count * sizeof(nk_draw_index);
    }
    g->Disable(GL_SCISSOR_TEST);
    return gl_ok(g, "UI render");
}
bool ps_graphics_ui(ps_graphics *g, struct nk_context *ui, struct nk_buffer *commands,
                    const struct nk_draw_null_texture *null_texture) {
    int w, h, pw, ph;
    SDL_GetWindowSize(g->window, &w, &h);
    SDL_GetWindowSizeInPixels(g->window, &pw, &ph);
    return ui_target(g, ui, commands, null_texture, 0, w, h, pw, ph);
}
ps_result ps_graphics_ui_png(ps_graphics *g, struct nk_context *ui,
                             const struct nk_draw_null_texture *null_texture, int width, int height,
                             const char *path) {
    return ps_graphics_ui_png_scaled(g, ui, null_texture, width, height, 2, path);
}
ps_result ps_graphics_ui_png_scaled(ps_graphics *g, struct nk_context *ui,
                                    const struct nk_draw_null_texture *null_texture, int width,
                                    int height, unsigned scale, const char *path) {
    if (!g || !ui || !null_texture || !path || width <= 0 || height <= 0 || width > 4096 ||
        height > 4096 || scale < 1 || scale > 4)
        return PS_INVALID;
    int pw = width * (int)scale, ph = height * (int)scale;
    if (pw > 8192 || ph > 8192 || pw > g->max_texture || ph > g->max_texture)
        return PS_LIMIT;
    size_t stride = (size_t)pw * 3;
    unsigned char *pixels = malloc(stride * (size_t)ph), *row = malloc(stride);
    if (!pixels || !row) {
        free(pixels);
        free(row);
        return PS_MEMORY;
    }
    GLuint target = 0, texture = 0;
    GLint previous = 0, pack = 4;
    g->GetIntegerv(GL_FRAMEBUFFER_BINDING, &previous);
    g->GetIntegerv(GL_PACK_ALIGNMENT, &pack);
    g->GenTextures(1, &texture);
    g->BindTexture(GL_TEXTURE_2D, texture);
    g->TexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, pw, ph, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    g->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    g->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    g->GenFramebuffers(1, &target);
    g->BindFramebuffer(GL_FRAMEBUFFER, target);
    g->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    struct nk_buffer commands;
    nk_buffer_init_default(&commands);
    bool ok = g->CheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
              ui_target(g, ui, &commands, null_texture, target, width, height, pw, ph);
    if (ok) {
        g->PixelStorei(GL_PACK_ALIGNMENT, 1);
        g->ReadPixels(0, 0, pw, ph, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        ok = gl_ok(g, "Diagram readback");
    }
    nk_buffer_free(&commands);
    g->PixelStorei(GL_PACK_ALIGNMENT, pack);
    g->BindFramebuffer(GL_FRAMEBUFFER, (GLuint)previous);
    g->DeleteFramebuffers(1, &target);
    g->DeleteTextures(1, &texture);
    ps_result result = PS_IO;
    if (ok) {
        for (int y = 0; y < ph / 2; y++) {
            unsigned char *top = pixels + (size_t)y * stride;
            unsigned char *bottom = pixels + (size_t)(ph - 1 - y) * stride;
            memcpy(row, top, stride);
            memcpy(top, bottom, stride);
            memcpy(bottom, row, stride);
        }
        result = ps_png_write(path, pixels, (unsigned)pw, (unsigned)ph, stride);
    }
    free(pixels);
    free(row);
    return result;
}
static bool finite_vector(ps_vec3 p) {
    return isfinite(p.x) && isfinite(p.y) && isfinite(p.z) && fabs(p.x) < 1e12 &&
           fabs(p.y) < 1e12 && fabs(p.z) < 1e12;
}
static void vertex(ps_graphics *g, ps_vec3 p, ps_vec3 normal, uint32_t color) {
    if (g->count >= SCENE_CAPACITY)
        return;
    if(g->transform_active) {
        ps_vec3 world;
        normal=ps_vnormalize(ps_mat3_apply(g->object_normal,normal));
        if(ps_transform_point(g->object_transform,p,&world)!=PS_OK || !finite_vector(world) ||
           !isfinite(normal.x) || !isfinite(normal.y) || !isfinite(normal.z)) {
            g->object_invalid=true;return;
        }
        p=world;
    }
    scene_vertex *v = &g->vertices[g->count++];
    v->p[0] = (float)p.x;
    v->p[1] = (float)p.y;
    v->p[2] = (float)p.z;
    v->n[0] = (float)normal.x;
    v->n[1] = (float)normal.y;
    v->n[2] = (float)normal.z;
    v->color[0] = (float)(color >> 24) / 255;
    v->color[1] = (float)((color >> 16) & 255) / 255;
    v->color[2] = (float)((color >> 8) & 255) / 255;
    v->color[3] = (color & 255) / 255.f;
}
static void triangle(ps_graphics *g, ps_vec3 a, ps_vec3 b, ps_vec3 c, uint32_t color) {
    ps_vec3 n = ps_vnormalize(ps_vcross(ps_vsub(b, a), ps_vsub(c, a)));
    vertex(g, a, n, color);
    vertex(g, b, n, color);
    vertex(g, c, n, color);
}
static void ball(ps_graphics *g, ps_vec3 center, double radius, uint32_t color) {
    for (size_t i = 0; i < g->sphere_count; i++)
        vertex(g, ps_vadd(center, ps_vscale(g->sphere[i].p, radius)), g->sphere[i].n, color);
}
static ps_quat unit_rotation(ps_quat q) {
    ps_quat result;
    return ps_quat_normalize(q, &result) == PS_OK ? result : ps_quat_identity();
}
static void box(ps_graphics *g, ps_vec3 center, ps_vec3 size, ps_quat rotation, uint32_t color) {
    rotation = unit_rotation(rotation);
    ps_vec3 p[8];
    for (int i = 0; i < 8; i++)
        p[i] = ps_vadd(center, ps_quat_rotate(rotation, ps_v3((i & 1 ? 1 : -1) * size.x * .5,
                                                              (i & 2 ? 1 : -1) * size.y * .5,
                                                              (i & 4 ? 1 : -1) * size.z * .5)));
    static const int faces[6][4] = {{0, 4, 6, 2}, {1, 3, 7, 5}, {0, 1, 5, 4},
                                    {2, 6, 7, 3}, {0, 2, 3, 1}, {4, 5, 7, 6}};
    for (int i = 0; i < 6; i++) {
        const int *f = faces[i];
        triangle(g, p[f[0]], p[f[1]], p[f[2]], color);
        triangle(g, p[f[0]], p[f[2]], p[f[3]], color);
    }
}
static void plane(ps_graphics *g, const ps_object *o) {
    if (o->b.x <= 0 || o->b.z <= 0)
        return;
    ps_vec3 p[4];
    ps_quat rotation = unit_rotation(o->orientation);
    for (int i = 0; i < 4; i++)
        p[i] = ps_vadd(o->a, ps_quat_rotate(rotation, ps_v3((i & 1 ? 1 : -1) * o->b.x * .5, 0,
                                                            (i & 2 ? 1 : -1) * o->b.z * .5)));
    triangle(g, p[0], p[2], p[1], o->color);
    triangle(g, p[1], p[2], p[3], o->color);
}
static void tube(ps_graphics *g, ps_vec3 a, ps_vec3 b, double r0, double r1, uint32_t color) {
    ps_vec3 d = ps_vsub(b, a);
    if (ps_vlength(d) < 1e-10)
        return;
    ps_vec3 n = ps_vnormalize(d),
            u = ps_vnormalize(ps_vcross(n, fabs(n.y) < .9 ? ps_v3(0, 1, 0) : ps_v3(1, 0, 0)));
    ps_vec3 v = ps_vcross(n, u);
    for (int i = 0; i < 16; i++) {
        double t = 2 * PS_PI * i / 16, s = 2 * PS_PI * (i + 1) / 16;
        ps_vec3 x = ps_vadd(ps_vscale(u, cos(t)), ps_vscale(v, sin(t)));
        ps_vec3 y = ps_vadd(ps_vscale(u, cos(s)), ps_vscale(v, sin(s)));
        ps_vec3 p = ps_vadd(a, ps_vscale(x, r0)), q = ps_vadd(a, ps_vscale(y, r0));
        ps_vec3 r = ps_vadd(b, ps_vscale(x, r1)), s1 = ps_vadd(b, ps_vscale(y, r1));
        triangle(g, p, q, r, color);
        if (r1 > 0)
            triangle(g, q, s1, r, color);
        triangle(g, a, q, p, color);
        if (r1 > 0)
            triangle(g, b, r, s1, color);
    }
}
static bool target(ps_graphics *g, int w, int h) {
    if (w < 1 || h < 1 || w > g->max_texture || h > g->max_texture) {
        SDL_SetError("Scene dimensions exceed GPU limits");
        return false;
    }
    g->BindFramebuffer(GL_FRAMEBUFFER, g->framebuffer);
    if (w == g->width && h == g->height)
        return true;
    GLuint texture = ps_graphics_texture(g, NULL, w, h);
    if (!texture)
        return false;
    g->BindRenderbuffer(GL_RENDERBUFFER, g->color_buffer);
    g->RenderbufferStorageMultisample(GL_RENDERBUFFER, g->samples, GL_RGBA8, w, h);
    g->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                               g->color_buffer);
    g->BindRenderbuffer(GL_RENDERBUFFER, g->depth);
    g->RenderbufferStorageMultisample(GL_RENDERBUFFER, g->samples, GL_DEPTH_COMPONENT24, w, h);
    g->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g->depth);
    bool ok = g->CheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    g->BindFramebuffer(GL_FRAMEBUFFER, g->resolve_framebuffer);
    g->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    ok = ok && g->CheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
         gl_ok(g, "framebuffer");
    g->BindFramebuffer(GL_FRAMEBUFFER, g->framebuffer);
    g->DeleteTextures(1, &g->scene_texture);
    g->scene_texture = texture;
    g->width = ok ? w : 0;
    g->height = ok ? h : 0;
    if (!ok)
        SDL_SetError("Scene framebuffer allocation failed");
    return ok;
}
const ps_camera PS_CAMERA_DEFAULT = {.3f, .45f, 4.5f, {0, 0, 0}, false, true, true};
static ps_mat4 camera_matrix(const ps_camera *c, double aspect) {
    ps_vec3 backward =
        ps_v3(sin(c->yaw) * cos(c->pitch), sin(c->pitch), cos(c->yaw) * cos(c->pitch));
    ps_vec3 right = ps_v3(cos(c->yaw), 0, -sin(c->yaw)), up = ps_vcross(backward, right);
    ps_vec3 eye = ps_vadd(c->target, ps_vscale(backward, c->distance));
    ps_mat4 view = ps_mat4_identity();
    view.m[0] = right.x;
    view.m[4] = right.y;
    view.m[8] = right.z;
    view.m[12] = -ps_vdot(right, eye);
    view.m[1] = up.x;
    view.m[5] = up.y;
    view.m[9] = up.z;
    view.m[13] = -ps_vdot(up, eye);
    view.m[2] = backward.x;
    view.m[6] = backward.y;
    view.m[10] = backward.z;
    view.m[14] = -ps_vdot(backward, eye);
    const double near_z = .02, far_z = 1000;
    ps_mat4 projection = {{0}};
    if (c->orthographic) {
        projection.m[0] = 2 / (c->distance * aspect);
        projection.m[5] = 2 / c->distance;
        projection.m[10] = -2 / (far_z - near_z);
        projection.m[14] = -(far_z + near_z) / (far_z - near_z);
        projection.m[15] = 1;
    } else {
        projection.m[0] = 2 / aspect;
        projection.m[5] = 2;
        projection.m[10] = -(far_z + near_z) / (far_z - near_z);
        projection.m[14] = -2 * far_z * near_z / (far_z - near_z);
        projection.m[11] = -1;
    }
    return ps_mat4_multiply(projection, view);
}
bool ps_graphics_project(const ps_camera *camera, ps_vec3 position, double aspect, float *x,
                         float *y) {
    if (!camera || !x || !y || !finite_vector(position) || !isfinite(aspect) || aspect <= 0)
        return false;
    ps_mat4 m = camera_matrix(camera, aspect);
    double p[4] = {position.x, position.y, position.z, 1}, clip[4] = {0};
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            clip[row] += m.m[col * 4 + row] * p[col];
    if (!isfinite(clip[3]) || clip[3] <= 0)
        return false;
    for (int i = 0; i < 3; i++)
        if (!isfinite(clip[i]) || fabs(clip[i]) > clip[3])
            return false;
    *x = (float)(.5 + .5 * clip[0] / clip[3]);
    *y = (float)(.5 - .5 * clip[1] / clip[3]);
    return true;
}
static int alpha_order(const void *left, const void *right) {
    const alpha_triangle *a = left, *b = right;
    if (a->depth != b->depth)
        return a->depth < b->depth ? -1 : 1;
    return a->first < b->first ? -1 : a->first > b->first;
}
static bool draw_scene_geometry(ps_graphics *g, const ps_camera *camera) {
    bool has_alpha = false;
    for (size_t i = 0; i < g->count; i += 3)
        has_alpha |= g->vertices[i].color[3] < 1;
    g->scene_stats.index_bytes = has_alpha ? g->count * sizeof *g->scene_indices : 0;
    if (!has_alpha) {
        g->DrawArrays(GL_TRIANGLES, 0, (GLsizei)g->count);
        return true;
    }
    if (g->alpha_capacity < g->count) {
        alpha_triangle *triangles = realloc(g->alpha_triangles, (g->count / 3) * sizeof *triangles);
        if (!triangles)
            return false;
        g->alpha_triangles = triangles;
        g->alpha_triangle_capacity = g->count / 3;
        uint32_t *indices = realloc(g->scene_indices, g->count * sizeof *indices);
        if (!indices)
            return false;
        g->scene_indices = indices;
        g->scene_index_capacity = g->count;
        g->alpha_capacity = g->count;
    }
    ps_vec3 backward = ps_v3(sin(camera->yaw) * cos(camera->pitch), sin(camera->pitch),
                             cos(camera->yaw) * cos(camera->pitch));
    size_t opaque = 0, transparent = 0;
    for (size_t i = 0; i + 2 < g->count; i += 3) {
        if (g->vertices[i].color[3] >= 1) {
            for (unsigned k = 0; k < 3; k++)
                g->scene_indices[opaque++] = (uint32_t)i + k;
        } else {
            double depth = 0;
            for (unsigned k = 0; k < 3; k++) {
                const float *p = g->vertices[i + k].p;
                depth += backward.x * p[0] + backward.y * p[1] + backward.z * p[2];
            }
            g->alpha_triangles[transparent++] = (alpha_triangle){depth, (uint32_t)i};
        }
    }
    qsort(g->alpha_triangles, transparent, sizeof *g->alpha_triangles, alpha_order);
    size_t count = opaque;
    for (size_t i = 0; i < transparent; i++)
        for (unsigned k = 0; k < 3; k++)
            g->scene_indices[count++] = g->alpha_triangles[i].first + k;
    g->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->scene_ebo);
    g->BufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(count * sizeof *g->scene_indices),
                  g->scene_indices, GL_STREAM_DRAW);
    g->DrawElements(GL_TRIANGLES, (GLsizei)opaque, GL_UNSIGNED_INT, NULL);
    g->Enable(GL_BLEND);
    /* Keep the composited scene texture opaque when Nuklear presents it. */
    g->BlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
    g->DepthMask(GL_FALSE);
    g->DrawElements(GL_TRIANGLES, (GLsizei)(count - opaque), GL_UNSIGNED_INT,
                    (const void *)(uintptr_t)(opaque * sizeof *g->scene_indices));
    g->DepthMask(GL_TRUE);
    g->Disable(GL_BLEND);
    return true;
}
unsigned ps_graphics_scene(ps_graphics *g, const ps_scene *scene, const ps_camera *camera,
                           int width, int height) {
    if (!g) return 0;
    g->scene_stats_valid = false;
    Uint64 started = SDL_GetTicksNS();
    g->pick_valid = false;
    if (!ps_scene_valid(scene) || !camera || !finite_vector(camera->target) ||
        !isfinite(camera->yaw) || !isfinite(camera->pitch) || !isfinite(camera->distance) ||
        camera->distance < .05f || camera->distance > 10000 || fabs(camera->pitch) > 1.571) {
        SDL_SetError("Invalid scene/camera");
        return 0;
    }
    if (!target(g, width, height))
        return 0;
    g->Viewport(0, 0, width, height);
    g->Disable(GL_SCISSOR_TEST);
    g->Disable(GL_BLEND);
    g->Disable(GL_CULL_FACE);
    g->Enable(GL_DEPTH_TEST);
    g->DepthMask(GL_TRUE);
    g->DepthFunc(GL_LESS);
    g->ClearColor(12 / 255.f, 22 / 255.f, 33 / 255.f, 1);
    g->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Uint64 geometry_started = SDL_GetTicksNS();
    g->count = 0;g->transform_active=false;
    if (camera->grid) {
        for (int i = -8; i <= 8; i++) {
            double k = i * .5;
            tube(g, ps_v3(k, -1.65, -4), ps_v3(k, -1.65, 4), .005, .005,
                 i ? 0x304457ff : 0x4289c7ff);
            tube(g, ps_v3(-4, -1.65, k), ps_v3(4, -1.65, k), .005, .005,
                 i ? 0x304457ff : 0xc06068ff);
        }
        tube(g, ps_v3(0, -1.65, 0), ps_v3(0, -1.05, 0), .008, .008, 0x55b99bff);
    }
    ps_mat4 transforms[PS_MAX_OBJECTS];
    if(ps_scene_transforms(scene,transforms)!=PS_OK)return 0;
    for (uint32_t i = 0; i < scene->count; i++) {
        g->pick_ranges[i] = g->count;
        const ps_object *o = &scene->objects[i];
        if (!(o->color & 255))
            continue;
        /* Apply the ancestor transform before narrowing vertices to GPU floats.
         * Complete local meshes support shear, reflected bases and ellipsoids.
         * Picking uses these exact world-space triangles. */
        g->object_invalid=false;g->transform_active=false;
        ps_mat4 identity=ps_mat4_identity();
        if(memcmp(&transforms[i],&identity,sizeof identity)) {
            ps_mat4 transform=transforms[i];
            ps_mat3 basis={{transform.m[0],transform.m[1],transform.m[2],transform.m[4],transform.m[5],transform.m[6],
                           transform.m[8],transform.m[9],transform.m[10]}},inverse;
            if(ps_mat3_inverse(basis,0,&inverse)!=PS_OK)continue;
            g->object_transform=transform;g->object_normal=ps_mat3_transpose(inverse);g->transform_active=true;
        }
        bool local_finite=isfinite(o->a.x) && isfinite(o->a.y) && isfinite(o->a.z) &&
            isfinite(o->b.x) && isfinite(o->b.y) && isfinite(o->b.z);
        if(!local_finite || !isfinite(o->radius) || o->radius<0 ||
           (!g->transform_active && (!finite_vector(o->a) || !finite_vector(o->b) || o->radius>1e10))) {
            g->transform_active=false;continue;
        }
        if (o->shape == PS_SPHERE && o->radius > 0)
            ball(g, o->a, o->radius, o->color);
        if (o->shape == PS_BOX) {
            ps_vec3 size = o->b;
            if (size.x <= 0 || size.y <= 0 || size.z <= 0)
                size = ps_v3(2 * o->radius, 2 * o->radius, 2 * o->radius);
            box(g, o->a, size, o->orientation, o->color);
        }
        if (o->shape == PS_PLANE)
            plane(g, o);
        if (o->shape == PS_POINT)
            ball(g, o->a, o->radius > 0 ? o->radius : .025, o->color);
        double radius = o->radius > 0 ? o->radius : .009;
        if (o->shape == PS_POLYLINE) {
            for (uint32_t j = 1; j < o->point_count; j++) {
                ps_vec3 a = scene->points[o->point_first + j - 1],
                        b = scene->points[o->point_first + j];
                if ((g->transform_active || (finite_vector(a) && finite_vector(b))))
                    tube(g, a, b, radius, radius, o->color);
            }
        }
        if (o->shape == PS_LINE)
            tube(g, o->a, o->b, radius, radius, o->color);
        if (o->shape == PS_ARROW && camera->vectors) {
            ps_vec3 d = ps_vsub(o->b, o->a);
            double length = ps_vlength(d);
            if (length > 1e-9) {
                double head = fmin(length * .3, fmax(radius * 5, .08));
                ps_vec3 neck = ps_vsub(o->b, ps_vscale(d, head / length));
                tube(g, o->a, neck, radius, radius, o->color);
                tube(g, neck, o->b, fmin(head * .4, radius * 3), 0, o->color);
            }
        }
        if(g->object_invalid)g->count=g->pick_ranges[i];
        g->transform_active=false;

    }
    g->pick_ranges[scene->count] = g->count;
    g->pick_count = scene->count;
    ps_mat4 m = camera_matrix(camera, (double)width / height);
    float matrix[16];
    for (int i = 0; i < 16; i++)
        matrix[i] = (float)m.m[i];
    Uint64 submission_started = SDL_GetTicksNS();
    g->UseProgram(g->scene_program);
    g->UniformMatrix4fv(g->scene_projection, 1, GL_FALSE, matrix);
    g->BindVertexArray(g->scene_vao);
    g->BindBuffer(GL_ARRAY_BUFFER, g->scene_vbo);
    g->BufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(g->count * sizeof *g->vertices), g->vertices,
                  GL_STREAM_DRAW);
    if (!draw_scene_geometry(g, camera)) {
        g->BindFramebuffer(GL_FRAMEBUFFER, 0);
        SDL_SetError("No memory for transparent geometry");
        return 0;
    }
    g->BindFramebuffer(GL_READ_FRAMEBUFFER, g->framebuffer);
    g->BindFramebuffer(GL_DRAW_FRAMEBUFFER, g->resolve_framebuffer);
    g->BlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    g->BindFramebuffer(GL_FRAMEBUFFER, 0);
    if (!gl_ok(g, "scene render"))
        return 0;
    g->pick_valid = ps_mat4_inverse(m, 0, &g->pick_inverse) == PS_OK;
    Uint64 ended = SDL_GetTicksNS();
    size_t index_bytes = g->scene_stats.index_bytes;
    g->scene_stats = (ps_scene_render_stats){
        (double)(geometry_started - started) / 1e9,
        (double)(submission_started - geometry_started) / 1e9,
        (double)(ended - submission_started) / 1e9,
        g->count, g->count * sizeof *g->vertices,
        index_bytes,
        SCENE_CAPACITY * sizeof *g->vertices + g->scene_index_capacity * sizeof *g->scene_indices +
            g->alpha_triangle_capacity * sizeof *g->alpha_triangles};
    g->scene_stats_valid = true;
    return g->scene_texture;
}
bool ps_graphics_scene_stats(const ps_graphics *g, ps_scene_render_stats *out) {
    if (!g || !out || !g->scene_stats_valid) return false;
    *out = g->scene_stats;
    return true;
}
static double triangle_hit(const scene_vertex *v, ps_vec3 origin, ps_vec3 direction) {
    ps_vec3 a = ps_v3(v[0].p[0], v[0].p[1], v[0].p[2]);
    ps_vec3 b = ps_v3(v[1].p[0], v[1].p[1], v[1].p[2]);
    ps_vec3 c = ps_v3(v[2].p[0], v[2].p[1], v[2].p[2]);
    ps_vec3 e1 = ps_vsub(b, a), e2 = ps_vsub(c, a);
    ps_vec3 p = ps_vcross(direction, e2), offset = ps_vsub(origin, a);
    double determinant = ps_vdot(e1, p);
    if (!isfinite(determinant) || determinant == 0)
        return INFINITY;
    double u = ps_vdot(offset, p) / determinant;
    ps_vec3 q = ps_vcross(offset, e1);
    double w = ps_vdot(direction, q) / determinant;
    double t = ps_vdot(e2, q) / determinant;
    return isfinite(u) && isfinite(w) && isfinite(t) && u >= 0 && w >= 0 && u + w <= 1 && t >= 0 &&
                   t <= 1
               ? t
               : INFINITY;
}
int ps_graphics_pick(const ps_graphics *g, double x, double y) {
    if (!g || !g->pick_valid || !isfinite(x) || !isfinite(y) || x < 0 || x > 1 || y < 0 || y > 1)
        return -1;
    ps_vec3 ray_origin, ray_end;
    if (ps_transform_point(g->pick_inverse, ps_v3(2 * x - 1, 1 - 2 * y, -1), &ray_origin) !=
            PS_OK ||
        ps_transform_point(g->pick_inverse, ps_v3(2 * x - 1, 1 - 2 * y, 1), &ray_end) != PS_OK)
        return -1;
    ps_vec3 direction = ps_vsub(ray_end, ray_origin);
    double closest = INFINITY;
    int picked = -1;
    size_t begin = 0;
    for (uint32_t range = 0; range <= g->pick_count; range++) {
        size_t end = g->pick_ranges[range];
        for (size_t i = begin; i + 2 < end; i += 3) {
            double t = triangle_hit(g->vertices + i, ray_origin, direction);
            if (t < closest) {
                closest = t;
                picked = (int)range - 1;
            }
        }
        begin = end;
    }
    return picked;
}
static bool capture_framebuffer(ps_graphics *g, GLuint framebuffer, int w, int h,
                                const char *path) {
    SDL_Surface *surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
    if (!surface)
        return false;
    g->BindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    g->PixelStorei(GL_PACK_ALIGNMENT, 4);
    g->ReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    bool ok = gl_ok(g, "capture");
    /* OpenGL's origin is lower left; SDL images are top down. */
    unsigned char *pixels = surface->pixels;
    for (int y = 0; y < h / 2; y++)
        for (int x = 0; x < surface->pitch; x++) {
            unsigned char *a = &pixels[y * surface->pitch + x],
                          *b = &pixels[(h - 1 - y) * surface->pitch + x];
            unsigned char temp = *a;
            *a = *b;
            *b = temp;
        }
    if (ok)
        ok = SDL_SaveBMP(surface, path);
    SDL_DestroySurface(surface);
    g->BindFramebuffer(GL_FRAMEBUFFER, 0);
    return ok;
}
bool ps_graphics_capture(ps_graphics *g, const char *path) {
    int w, h;
    SDL_GetWindowSizeInPixels(g->window, &w, &h);
    return capture_framebuffer(g, 0, w, h, path);
}
bool ps_graphics_present(ps_graphics *g) { return SDL_GL_SwapWindow(g->window); }

static bool test_pixels(ps_graphics *g, const ps_scene *scene, const ps_camera *camera, int w,
                        int h, unsigned char *pixels) {
    if (!ps_graphics_scene(g, scene, camera, w, h))
        return false;
    g->BindFramebuffer(GL_FRAMEBUFFER, g->resolve_framebuffer);
    g->ReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    g->BindFramebuffer(GL_FRAMEBUFFER, 0);
    return gl_ok(g, "test readback");
}
static bool test_transparency(ps_graphics *g) {
    enum { W = 80, H = 60 };
    unsigned char front[W * H * 4], back[W * H * 4], combined[W * H * 4], reversed[W * H * 4];
    ps_camera camera = {0, 0, 4, {0, 0, 0}, false, true, false};
    ps_object red = {0};
    red.shape = PS_PLANE;
    red.b = ps_v3(2, 0, 2);
    red.orientation = ps_quat_axis_angle(ps_v3(1, 0, 0), PS_PI / 2);
    red.color = 0xff0000ff;
    ps_object blue = red;
    blue.color = 0x0000ffff;
    blue.a.z = -.5;
    size_t center = ((H / 2) * W + W / 2) * 4;
    for (unsigned ortho = 0; ortho < 2; ortho++) {
        camera.orthographic = ortho != 0;
        ps_scene scene = {0};
        scene.objects[0] = red;
        scene.count = 1;
        if (!test_pixels(g, &scene, &camera, W, H, front))
            return false;
        scene.objects[0] = blue;
        if (!test_pixels(g, &scene, &camera, W, H, back))
            return false;
        scene.objects[0] = red;
        scene.objects[0].color = 0xff000080;
        scene.objects[1] = blue;
        scene.count = 2;
        if (!test_pixels(g, &scene, &camera, W, H, combined) || ps_graphics_pick(g, .5, .5) != 0)
            return false;
        for (unsigned c = 0; c < 3; c++) {
            double expected = front[center + c] * (128. / 255) + back[center + c] * (127. / 255);
            if (fabs(combined[center + c] - expected) > 2) {
                SDL_SetError("Transparency blend channel %u: got %u expected %.2f", c,
                             combined[center + c], expected);
                return false;
            }
        }
        if (combined[center + 3] != 255)
            return false;
        scene.objects[1].color = 0x0000ff80;
        if (!test_pixels(g, &scene, &camera, W, H, combined))
            return false;
        const double background[] = {12, 22, 33};
        for (unsigned c = 0; c < 3; c++) {
            double a = 128. / 255, b = 1 - a;
            double expected =
                front[center + c] * a + back[center + c] * a * b + background[c] * b * b;
            if (fabs(combined[center + c] - expected) > 3)
                return false;
        }
        ps_object swap = scene.objects[0];
        scene.objects[0] = scene.objects[1];
        scene.objects[1] = swap;
        if (!test_pixels(g, &scene, &camera, W, H, reversed) ||
            memcmp(combined, reversed, sizeof combined))
            return false;
        scene.objects[0].color = blue.color;
        scene.objects[1].color = 0xff000000;
        if (!test_pixels(g, &scene, &camera, W, H, combined) ||
            memcmp(combined, back, sizeof back) || ps_graphics_pick(g, .5, .5) != 0)
            return false;
        scene.objects[0].a.z = .5;
        scene.objects[1].color = 0xff000080;
        if (!test_pixels(g, &scene, &camera, W, H, combined) ||
            memcmp(combined + center, back + center, 4))
            return false;
    }
    return true;
}
bool ps_graphics_test(ps_graphics *g, const char *capture_path) {
    if (!test_transparency(g))
        return false;
    /* Picking follows rendered geometry for all seven mesh primitives. */
    for (unsigned shape = PS_SPHERE; shape <= PS_POLYLINE; shape++) {
        ps_scene pick_scene = {0};
        ps_camera pick_camera = PS_CAMERA_DEFAULT;
        pick_camera.target = ps_v3(0, 0, 0);
        pick_camera.pitch = .4f;
        pick_camera.grid = false;
        pick_camera.vectors = true;
        ps_vec3 start = ps_v3(-.5, 0, 0), end = ps_v3(.5, 0, 0);
        if (shape == PS_POLYLINE) {
            ps_vec3 path[] = {start, end};
            if (ps_scene_polyline(&pick_scene, path, 2, .05, 0xffffffff) != PS_OK)
                return false;
        } else {
            bool segment = shape == PS_LINE || shape == PS_ARROW;
            ps_scene_add(&pick_scene, (ps_shape)shape, segment ? start : ps_v3(0, 0, 0),
                         segment ? end : ps_v3(1, 1, 1), .2, 0xffffffff);
        }
        for (unsigned ortho = 0; ortho < 2; ortho++) {
            pick_camera.orthographic = ortho != 0;
            if (!ps_graphics_scene(g, &pick_scene, &pick_camera, 160, 120) ||
                ps_graphics_pick(g, .5, .5) != 0) {
                SDL_SetError("Picking failed for shape %u, orthographic %u", shape, ortho);
                return false;
            }
        }
        pick_scene.objects[0].id=1;
        if(ps_scene_frame(&pick_scene,100,0,"Outer",ps_v3(.6,.2,0),
                          ps_quat_axis_angle(ps_v3(0,0,1),PS_PI/2),ps_v3(2,3,1))!=PS_OK ||
           ps_scene_frame(&pick_scene,200,100,"Inner",ps_v3(.2,0,0),
                          ps_quat_axis_angle(ps_v3(0,1,0),.35),ps_v3(-1,.5,2))!=PS_OK ||
           ps_scene_set_parent(&pick_scene,1,200)!=PS_OK)return false;
        pick_camera.target=ps_v3(.6,.6,0);
        for(unsigned ortho=0;ortho<2;ortho++) {
            pick_camera.orthographic=ortho!=0;
            if(!ps_graphics_scene(g,&pick_scene,&pick_camera,160,120) || ps_graphics_pick(g,.5,.5)!=0) {
                SDL_SetError("Nested reflected/nonuniform frame picking failed for shape %u",shape);return false;
            }
        }
        if (shape == PS_ARROW) {
            pick_camera.vectors = false;
            if (!ps_graphics_scene(g, &pick_scene, &pick_camera, 160, 120) ||
                ps_graphics_pick(g, .5, .5) != -1)
                return false;
        }
    }
    ps_scene large={0};ps_camera large_camera=PS_CAMERA_DEFAULT;large_camera.target=ps_v3(1,0,0);large_camera.grid=false;
    if(ps_scene_add_id(&large,1,PS_SPHERE,ps_v3(1e15,0,0),ps_v3(1e15,0,0),2e14,UINT32_MAX)!=PS_OK ||
       ps_scene_frame(&large,100,0,"Unit conversion",ps_v3(0,0,0),ps_quat_identity(),ps_v3(1e-15,1e-15,1e-15))!=PS_OK ||
       ps_scene_set_parent(&large,1,100)!=PS_OK || !ps_graphics_scene(g,&large,&large_camera,160,120) ||
       ps_graphics_pick(g,.5,.5)!=0){SDL_SetError("Frame geometry was clipped before conversion to world coordinates");return false;}
    enum { W = 160, H = 120 };
    unsigned char first[W * H * 4], second[W * H * 4];
    ps_camera camera = {0, 0, 4, {0, 0, 0}, false, true, false};
    ps_scene scene = {0};
    ps_scene_add(&scene, PS_BOX, ps_v3(0, 0, 0), ps_v3(1, 1, .5), 0, 0xff2020ff);
    ps_scene_add(&scene, PS_SPHERE, ps_v3(0, 0, -1), ps_v3(0, 0, 0), .4, 0x20ff20ff);
    if (!test_pixels(g, &scene, &camera, W, H, first))
        return false;
    if (ps_graphics_pick(g, .5, .5) != 0 || ps_graphics_pick(g, 0, 0) != -1 ||
        ps_graphics_pick(g, NAN, .5) != -1)
        return false;
    ps_object swap = scene.objects[0];
    scene.objects[0] = scene.objects[1];
    scene.objects[1] = swap;
    if (!test_pixels(g, &scene, &camera, W, H, second))
        return false;
    if (ps_graphics_pick(g, .5, .5) != 1)
        return false;
    size_t center = ((H / 2) * W + W / 2) * 4;
    if (memcmp(first, second, sizeof first) || first[center] < 3 * first[center + 1]) {
        SDL_SetError("Depth test: hidden sphere changed visible box");
        return false;
    }
    camera.orthographic = true;
    if (!test_pixels(g, &scene, &camera, W, H, second) || !memcmp(first, second, sizeof first)) {
        SDL_SetError("Projection test failed");
        return false;
    }
    if (ps_graphics_pick(g, .5, .5) != 1)
        return false;
    camera.yaw = .7f;
    camera.pitch = .4f;
    if (!test_pixels(g, &scene, &camera, W, H, first) ||
        !test_pixels(g, &scene, &camera, 113, 79, second) ||
        !test_pixels(g, &scene, &camera, W, H, second))
        return false;
    if (memcmp(first, second, sizeof first)) {
        SDL_SetError("Resize roundtrip changed scene pixels");
        return false;
    }
    camera.orthographic = false;
    camera.yaw = 0;
    camera.pitch = 0;
    scene.count = 1;
    scene.objects[0].a = ps_v3(0, 0, 6);
    if (!test_pixels(g, &scene, &camera, W, H, first))
        return false;
    for (size_t i = 0; i < sizeof first; i += 4)
        if (first[i] != 12 || first[i + 1] != 22 || first[i + 2] != 33) {
            SDL_SetError("Behind-camera clipping failed");
            return false;
        }
    scene.objects[0].a = ps_v3(1e300, 0, 0);
    if (!test_pixels(g, &scene, &camera, W, H, second) || memcmp(first, second, sizeof first)) {
        SDL_SetError("Extreme-coordinate rejection failed");
        return false;
    }
    camera.pitch = (float)PS_PI / 2;
    scene.objects[0].a = ps_v3(0, 0, 0);
    if (!test_pixels(g, &scene, &camera, W, H, first) || first[center + 1] <= first[center]) {
        SDL_SetError("Top-view camera is singular");
        return false;
    }
    scene.count = 1;
    scene.objects[0] = (ps_object){
        .shape = PS_ARROW, .color = 0xff2020ff, .a = {-1, 0, 0}, .b = {1, 0, 0}, .radius = .05};
    camera.pitch = 0;
    camera.vectors = true;
    if (!test_pixels(g, &scene, &camera, W, H, first))
        return false;
    camera.vectors = false;
    if (!test_pixels(g, &scene, &camera, W, H, second) || !memcmp(first, second, sizeof first)) {
        SDL_SetError("Vector visibility test failed");
        return false;
    }
    scene = (ps_scene){0};
    ps_scene_add(&scene, PS_BOX, ps_v3(0, 0, 0), ps_v3(1, .2, .2), 0, 0xff2020ff);
    camera.vectors = true;
    if (!test_pixels(g, &scene, &camera, W, H, first))
        return false;
    scene.objects[0].orientation = ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 2);
    if (!test_pixels(g, &scene, &camera, W, H, second))
        return false;
    size_t side = ((H / 2) * W + W / 2 + 12) * 4;
    if (first[side] < 3 * first[side + 1] || second[side] != 12) {
        SDL_SetError("Oriented box geometry test failed");
        return false;
    }
    scene.objects[0].orientation.x *= 10;
    scene.objects[0].orientation.y *= 10;
    scene.objects[0].orientation.z *= 10;
    scene.objects[0].orientation.w *= 10;
    if (!test_pixels(g, &scene, &camera, W, H, first) || memcmp(first, second, sizeof first)) {
        SDL_SetError("Quaternion normalization test failed");
        return false;
    }
    scene.objects[0].shape = PS_PLANE;
    scene.objects[0].b = ps_v3(1, 0, 1);
    scene.objects[0].orientation = ps_quat_axis_angle(ps_v3(1, 0, 0), PS_PI / 2);
    if (!test_pixels(g, &scene, &camera, W, H, first) || first[center] < 3 * first[center + 1]) {
        SDL_SetError("Rotated plane test failed");
        return false;
    }
    scene = (ps_scene){0};
    ps_vec3 path[3] = {{-1, -.4, 0}, {0, .4, 0}, {1, -.4, 0}};
    if (ps_scene_polyline(&scene, path, 3, .03, 0xff2020ff) != PS_OK ||
        !test_pixels(g, &scene, &camera, W, H, first))
        return false;
    bool red = false;
    for (size_t i = 0; i < sizeof first; i += 4)
        if (first[i] > 3 * first[i + 1])
            red = true;
    if (!red) {
        SDL_SetError("Polyline geometry missing");
        return false;
    }
    scene = (ps_scene){0};
    ps_scene_add(&scene, PS_POINT, ps_v3(0, 0, 0), ps_v3(0, 0, 0), .08, 0xff2020ff);
    if (!test_pixels(g, &scene, &camera, W, H, first) || first[center] < 3 * first[center + 1]) {
        SDL_SetError("Point geometry missing");
        return false;
    }
    float px, py;
    if (!ps_graphics_project(&camera, ps_v3(0, 0, 0), (double)W / H, &px, &py) ||
        fabsf(px - .5f) > 1e-6f || fabsf(py - .5f) > 1e-6f ||
        ps_graphics_project(&camera, ps_v3(0, 0, 5), (double)W / H, &px, &py)) {
        SDL_SetError("Label projection test failed");
        return false;
    }
    /* A floor at world Y=0 must not hide a resting box in the app's default view. */
    camera = PS_CAMERA_DEFAULT;
    scene = (ps_scene){0};
    ps_scene_add(&scene, PS_BOX, ps_v3(.4, .25, 0), ps_v3(.8, .5, .6), 0, 0xff2020ff);
    ps_scene_add(&scene, PS_PLANE, ps_v3(0, 0, 0), ps_v3(8, 0, 8), 0, 0x364452ff);
    if (!test_pixels(g, &scene, &camera, W, H, first))
        return false;
    unsigned visible_box_pixels = 0;
    for (size_t i = 0; i < sizeof first; i += 4)
        visible_box_pixels += first[i] > 3 * first[i + 1];
    if (visible_box_pixels < 100) {
        SDL_SetError("Default camera hides resting box behind floor");
        return false;
    }
    if (capture_path) {
        scene.count = 0;
        scene.point_count = 0;
        ps_scene_add(&scene, PS_BOX, ps_v3(-.9, -.9, 0), ps_v3(.8, 1.5, .8), 0, 0xf5b868ff);
        scene.objects[0].orientation = ps_quat_axis_angle(ps_v3(0, 1, 0), .3);
        ps_scene_add(&scene, PS_SPHERE, ps_v3(.45, -.9, .1), ps_v3(0, 0, 0), .65, 0x54d9beff);
        ps_scene_add(&scene, PS_ARROW, ps_v3(.45, -.9, .1), ps_v3(1.4, .4, .1), .035, 0x6dacf5ff);
        ps_scene_add(&scene, PS_LINE, ps_v3(-.9, .2, 0), ps_v3(.45, .2, .1), .02, 0xe2eaf2ff);
        ps_scene_add(&scene, PS_PLANE, ps_v3(0, -1.67, 0), ps_v3(4, 0, 3.5), 0, 0x283b4fff);
        ps_vec3 trail[4] = {{-1.4, -1.6, 1}, {-.8, -.6, 1}, {.2, -.3, 1}, {1.2, -.7, 1}};
        (void)ps_scene_polyline(&scene, trail, 4, .012, 0xe69bb8ff);
        ps_scene_add(&scene, PS_POINT, trail[3], ps_v3(0, 0, 0), .04, 0xe69bb8ff);
        camera = (ps_camera){.45f, .35f, 4.5f, {0, -.6, 0}, false, true, true};
        if (!ps_graphics_scene(g, &scene, &camera, 960, 640) ||
            !capture_framebuffer(g, g->resolve_framebuffer, 960, 640, capture_path))
            return false;
        char alpha_path[4096];
        int length = snprintf(alpha_path, sizeof alpha_path, "%s-alpha.bmp", capture_path);
        if (length < 0 || (size_t)length >= sizeof alpha_path) {
            SDL_SetError("Transparency capture path too long");
            return false;
        }
        scene.objects[0].color = 0xf5b86870;
        scene.objects[1].color = 0x54d9be70;
        if (!ps_graphics_scene(g, &scene, &camera, 960, 640) ||
            !capture_framebuffer(g, g->resolve_framebuffer, 960, 640, alpha_path))
            return false;
    }
    puts("GPU TEST: depth, rotated box/plane, points, polylines, label projection, resize, "
         "clipping, transparency, nested coordinate frames and picking PASSED");
    return true;
}
