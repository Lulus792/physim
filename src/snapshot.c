#include "physim/snapshot.h"
#include "physim/data.h"
#include <math.h>
#include <string.h>
_Static_assert(PS_SNAPSHOT_HEADER + PS_MAX_CHANNELS * 8 + PS_MAX_OBJECTS * PS_SNAPSHOT_OBJECT_SIZE +
                       PS_MAX_SCENE_POINTS * 24 <=
                   PS_SNAPSHOT_MAX - 4,
               "Snapshot must fit in a frame");
size_t ps_snapshot_encode(unsigned char *p, const ps_context *c, const ps_scene *s, bool paused) {
    if (!p || !c || c->channel_count > PS_MAX_CHANNELS || !isfinite(c->time_s) ||
        !ps_scene_valid(s))
        return 0;
    for (uint32_t i = 0; i < c->channel_count; i++)
        if (!isfinite(c->values[i]))
            return 0;
    ps_put_f64(p, c->time_s);
    ps_put_u32(p + 8, c->channel_count);
    ps_put_u32(p + 12, s->count);
    ps_put_u32(p + 16, paused ? 1 : 0);
    ps_put_u32(p + 20, s->point_count);
    size_t at = PS_SNAPSHOT_HEADER;
    for (uint32_t i = 0; i < c->channel_count; i++, at += 8)
        ps_put_f64(p + at, c->values[i]);
    for (uint32_t i = 0; i < s->count; i++, at += PS_SNAPSHOT_OBJECT_SIZE) {
        const ps_object *o = &s->objects[i];
        ps_put_u32(p + at, o->shape);
        ps_put_u32(p + at + 4, o->color);
        double v[] = {o->a.x,           o->a.y,           o->a.z,          o->b.x,
                      o->b.y,           o->b.z,           o->radius,       o->orientation.x,
                      o->orientation.y, o->orientation.z, o->orientation.w};
        for (size_t k = 0; k < 11; k++)
            ps_put_f64(p + at + 8 + k * 8, v[k]);
        memset(p + at + 96, 0, 64);
        memcpy(p + at + 96, o->text, strlen(o->text));
        ps_put_u32(p + at + 160, o->point_first);
        ps_put_u32(p + at + 164, o->point_count);
        ps_put_u32(p + at + 168, o->id);
        ps_put_u32(p + at + 172, o->parent_id);
    }
    for (uint32_t i = 0; i < s->point_count; i++, at += 24) {
        ps_put_f64(p + at, s->points[i].x);
        ps_put_f64(p + at + 8, s->points[i].y);
        ps_put_f64(p + at + 16, s->points[i].z);
    }
    return at;
}
bool ps_snapshot_decode(const unsigned char *p, uint32_t n, double *t, double *v, uint32_t *count,
                        ps_scene *s, bool *paused) {
    return ps_snapshot_decode_version(PS_SNAPSHOT_VERSION,p,n,t,v,count,s,paused);
}
bool ps_snapshot_decode_version(uint32_t version,const unsigned char *p,uint32_t n,double *t,
                                double *v,uint32_t *count,ps_scene *s,bool *paused) {
    if(version!=1 && version!=PS_SNAPSHOT_VERSION) return false;
    uint32_t object_size=version==1?172u:PS_SNAPSHOT_OBJECT_SIZE;
    if (!p || !t || !v || !count || !s || !paused || n < PS_SNAPSHOT_HEADER)
        return false;
    uint32_t nc = ps_get_u32(p + 8), ns = ps_get_u32(p + 12), np = ps_get_u32(p + 20),
             pause = ps_get_u32(p + 16);
    if (nc > PS_MAX_CHANNELS || ns > PS_MAX_OBJECTS || np > PS_MAX_SCENE_POINTS || pause > 1 ||
        n != PS_SNAPSHOT_HEADER + nc * 8 + ns * object_size + np * 24)
        return false;
    double time = ps_get_f64(p), values[PS_MAX_CHANNELS] = {0};
    if (!isfinite(time))
        return false;
    ps_scene scene = {0};
    scene.count = ns;
    scene.point_count = np;
    size_t at = PS_SNAPSHOT_HEADER;
    for (uint32_t i = 0; i < nc; i++, at += 8) {
        values[i] = ps_get_f64(p + at);
        if (!isfinite(values[i]))
            return false;
    }
    for (uint32_t i = 0; i < ns; i++, at += object_size) {
        ps_object *o = &scene.objects[i];
        o->shape = ps_get_u32(p + at);
        if(version==1 && o->shape>PS_LABEL) return false;
        o->color = ps_get_u32(p + at + 4);
        double a[11];
        for (size_t k = 0; k < 11; k++)
            a[k] = ps_get_f64(p + at + 8 + k * 8);
        o->a = ps_v3(a[0], a[1], a[2]);
        o->b = ps_v3(a[3], a[4], a[5]);
        o->radius = a[6];
        o->orientation = (ps_quat){a[7], a[8], a[9], a[10]};
        memcpy(o->text, p + at + 96, 64);
        o->point_first = ps_get_u32(p + at + 160);
        o->point_count = ps_get_u32(p + at + 164);
        o->id = ps_get_u32(p + at + 168);
        o->parent_id=version==1?0:ps_get_u32(p+at+172);
    }
    for (uint32_t i = 0; i < np; i++, at += 24)
        scene.points[i] =
            ps_v3(ps_get_f64(p + at), ps_get_f64(p + at + 8), ps_get_f64(p + at + 16));
    if (!ps_scene_valid(&scene))
        return false;
    *s = scene;
    *t = time;
    *count = nc;
    *paused = pause != 0;
    memcpy(v, values, nc * sizeof *v);
    return true;
}
