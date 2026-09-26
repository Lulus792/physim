#ifndef PHYSIM_EXPERIMENT_H
#define PHYSIM_EXPERIMENT_H
#include "core.h"
#define PS_MAX_CHANNELS 16
#define PS_MAX_OBJECTS 32
#define PS_MAX_SCENE_POINTS 96
#define PS_MAX_PARAMETERS 16
#ifdef _WIN32
#define PS_EXPORT __declspec(dllexport)
#else
#define PS_EXPORT __attribute__((visibility("default")))
#endif
typedef struct {
    char name[48], unit[16], description[96];
    int8_t dimension[7];
} ps_channel;
typedef enum {
    PS_SPHERE,
    PS_LINE,
    PS_BOX,
    PS_ARROW,
    PS_POINT,
    PS_PLANE,
    PS_POLYLINE,
    PS_LABEL
} ps_shape;
/* Metres, Y up. Sphere: a=center, radius. Box: a=center, b=full XYZ extents;
 * if an extent is nonpositive, a cube with half-size radius is used.
 * Line/arrow: a=start, b=end, radius=shaft radius (0 selects 0.009 m).
 * Color is RRGGBBAA. Alpha 0 hides geometry and labels, including picking.
 * Mesh alpha 1..254 uses back-to-front triangle blending; intersecting
 * transparent surfaces may show sorting artifacts. Alpha 255 is opaque. */
typedef struct {
    uint32_t shape, color;
    ps_vec3 a, b;
    double radius;
    ps_quat orientation; /* Local-to-world rotation for box/plane. Zero means identity. */
    char text[64];       /* UTF-8 label, terminated; empty for other shapes. */
    uint32_t point_first, point_count; /* Polyline range in the scene point pool. */
    uint32_t id; /* Optional stable ID within a run; 0 = anonymous. Unique per scene. */
} ps_object;
typedef struct {
    uint32_t count, point_count;
    ps_object objects[PS_MAX_OBJECTS];
    ps_vec3 points[PS_MAX_SCENE_POINTS];
} ps_scene;
typedef struct {
    char name[48], description[96];
    double value, default_value, minimum, maximum;
    bool defined;
} ps_parameter;
typedef struct ps_context {
    uint32_t struct_size, api_version;
    void *user;
    double time_s, dt_s;
    uint64_t seed;
    ps_rng rng;
    uint32_t channel_count;
    ps_channel channels[PS_MAX_CHANNELS];
    double values[PS_MAX_CHANNELS];
    char error[256];
    char model_metadata[2048];
    /* Optional ABI-3 context extension; check struct_size before accessing it. */
    uint32_t parameter_count;
    ps_parameter parameters[PS_MAX_PARAMETERS];
} ps_context;
typedef struct {
    uint32_t struct_size, abi_version;
    uint64_t capabilities;
    const char *name;
    ps_result (*create)(ps_context *context);
    ps_result (*reset)(ps_context *context);
    ps_result (*step)(ps_context *context, double dt_s);
    void (*build_scene)(ps_context *context, ps_scene *scene);
    void (*destroy)(ps_context *context);
} ps_experiment_api;
typedef const ps_experiment_api *(*ps_experiment_entry)(void);
/* Export ps_get_experiment from each module. Context and scene are owned by host.
 * Module owns context->user and releases it in destroy, including failed create. */
int ps_channel_add(ps_context *context, const char *name, ps_unit unit, const char *description);
ps_result ps_parameter_override(ps_context *context, const char *name, double value);
ps_result ps_parameter_define(ps_context *context, const char *name, const char *description,
                              double default_value, double minimum, double maximum,
                              double *value);
ps_result ps_parameter_finalize(const ps_context *context);
void ps_scene_add(ps_scene *scene, ps_shape shape, ps_vec3 a, ps_vec3 b, double radius,
                  uint32_t rgba);
/* Checked helpers are atomic on failure. Clear the whole scene before building it.
 * Plane: a=center, b.x/b.z=full side lengths in local XZ; orientation rotates it.
 * Polyline coordinates are in world space; label positions are annotation anchors. */
ps_result ps_scene_push(ps_scene *scene, const ps_object *object);
ps_result ps_scene_polyline(ps_scene *scene, const ps_vec3 *points, size_t count, double radius,
                            uint32_t rgba);
ps_result ps_scene_label(ps_scene *scene, ps_vec3 position, const char *text, uint32_t rgba);
bool ps_scene_valid(const ps_scene *scene);
/* ID-bearing constructors. ID 0 is anonymous; duplicate nonzero IDs fail
 * without changing the scene, including its polyline point pool. */
ps_result ps_scene_add_id(ps_scene *scene, uint32_t id, ps_shape shape, ps_vec3 a, ps_vec3 b,
                          double radius, uint32_t rgba);
ps_result ps_scene_polyline_id(ps_scene *scene, uint32_t id, const ps_vec3 *points, size_t count,
                               double radius, uint32_t rgba);
ps_result ps_scene_label_id(ps_scene *scene, uint32_t id, ps_vec3 position, const char *text,
                            uint32_t rgba);
#endif
