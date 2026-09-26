#include "physim/experiment.h"
#include <math.h>
#include <string.h>

static bool finite3(ps_vec3 p) { return isfinite(p.x) && isfinite(p.y) && isfinite(p.z); }
static bool label_valid(const char *text, size_t capacity) {
    size_t at = 0;
    while (at < capacity) {
        uint32_t c = (unsigned char)text[at++];
        if (!c)
            return true;
        if (c < 32 || c == 127)
            return false;
        if (c < 128)
            continue;
        unsigned extra;
        uint32_t minimum;
        if (c >= 0xc2 && c <= 0xdf) {
            extra = 1;
            minimum = 0x80;
            c &= 0x1f;
        } else if (c >= 0xe0 && c <= 0xef) {
            extra = 2;
            minimum = 0x800;
            c &= 0x0f;
        } else if (c >= 0xf0 && c <= 0xf4) {
            extra = 3;
            minimum = 0x10000;
            c &= 7;
        } else
            return false;
        if (extra > capacity - at)
            return false;
        for (unsigned i = 0; i < extra; i++) {
            unsigned char b = (unsigned char)text[at++];
            if ((b & 0xc0) != 0x80)
                return false;
            c = (c << 6) | (b & 0x3f);
        }
        if (c < minimum || c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff))
            return false;
    }
    return false;
}
static bool object_valid(const ps_object *o, uint32_t points) {
    return o && o->shape <= PS_LABEL && finite3(o->a) && finite3(o->b) && isfinite(o->radius) &&
           o->radius >= 0 && isfinite(o->orientation.x) && isfinite(o->orientation.y) &&
           isfinite(o->orientation.z) && isfinite(o->orientation.w) &&
           label_valid(o->text, sizeof o->text) &&
           (o->shape != PS_POLYLINE || (o->point_count >= 2 && o->point_first <= points &&
                                        o->point_count <= points - o->point_first));
}
bool ps_scene_valid(const ps_scene *s) {
    if (!s || s->count > PS_MAX_OBJECTS || s->point_count > PS_MAX_SCENE_POINTS)
        return false;
    for (uint32_t i = 0; i < s->point_count; i++)
        if (!finite3(s->points[i]))
            return false;
    for (uint32_t i = 0; i < s->count; i++) {
        if (!object_valid(&s->objects[i], s->point_count))
            return false;
        if (s->objects[i].id)
            for (uint32_t j = 0; j < i; j++)
                if (s->objects[i].id == s->objects[j].id)
                    return false;
    }
    return true;
}
ps_result ps_scene_push(ps_scene *s, const ps_object *o) {
    if (!s || s->count >= PS_MAX_OBJECTS || s->point_count > PS_MAX_SCENE_POINTS ||
        !object_valid(o, s->point_count))
        return PS_INVALID;
    if (o->id)
        for (uint32_t i = 0; i < s->count; i++)
            if (s->objects[i].id == o->id)
                return PS_INVALID;
    s->objects[s->count++] = *o;
    return PS_OK;
}
ps_result ps_scene_polyline(ps_scene *s, const ps_vec3 *points, size_t count, double radius,
                            uint32_t rgba) {
    return ps_scene_polyline_id(s, 0, points, count, radius, rgba);
}
ps_result ps_scene_polyline_id(ps_scene *s, uint32_t id, const ps_vec3 *points, size_t count,
                               double radius, uint32_t rgba) {
    if (!s || !points || count < 2 || s->count >= PS_MAX_OBJECTS ||
        s->point_count > PS_MAX_SCENE_POINTS || count > PS_MAX_SCENE_POINTS - s->point_count ||
        !isfinite(radius) || radius < 0)
        return PS_INVALID;
    if (id)
        for (uint32_t i = 0; i < s->count; i++)
            if (s->objects[i].id == id)
                return PS_INVALID;
    for (size_t i = 0; i < count; i++)
        if (!finite3(points[i]))
            return PS_INVALID;
    ps_object o = {0};
    o.id = id;
    o.shape = PS_POLYLINE;
    o.radius = radius;
    o.color = rgba;
    o.orientation.w = 1;
    o.point_first = s->point_count;
    o.point_count = (uint32_t)count;
    memmove(s->points + s->point_count, points, count * sizeof *points);
    s->point_count += (uint32_t)count;
    s->objects[s->count++] = o;
    return PS_OK;
}
ps_result ps_scene_label(ps_scene *s, ps_vec3 position, const char *text, uint32_t rgba) {
    return ps_scene_label_id(s, 0, position, text, rgba);
}
ps_result ps_scene_label_id(ps_scene *s, uint32_t id, ps_vec3 position, const char *text,
                            uint32_t rgba) {
    if (!text)
        return PS_INVALID;
    ps_object o = {0};
    o.id = id;
    o.shape = PS_LABEL;
    o.a = position;
    o.color = rgba;
    o.orientation.w = 1;
    size_t n = 0;
    while (n < sizeof o.text && text[n])
        n++;
    if (n == sizeof o.text)
        return PS_INVALID;
    memcpy(o.text, text, n + 1);
    return ps_scene_push(s, &o);
}
