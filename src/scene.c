#include "physim/experiment.h"
#include "physim/math.h"
#include <math.h>
#include <string.h>
_Static_assert(sizeof(ps_object)==176 && offsetof(ps_object,parent_id)==172,
               "ABI-3 hierarchy must occupy only former tail padding");

int ps_scene_parent_index(const ps_scene *s,uint32_t index) {
    if(!s || s->count>PS_MAX_OBJECTS || index>=s->count || !s->objects[index].parent_id) return -1;
    for(uint32_t i=0;i<s->count;i++) if(s->objects[i].id==s->objects[index].parent_id) return (int)i;
    return -1;
}

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
    return o && o->shape <= PS_FRAME && ((o->shape!=PS_GROUP && o->shape!=PS_FRAME) || o->id) && finite3(o->a) && finite3(o->b) && isfinite(o->radius) &&
           o->radius >= 0 && isfinite(o->orientation.x) && isfinite(o->orientation.y) &&
           isfinite(o->orientation.z) && isfinite(o->orientation.w) &&
           label_valid(o->text, sizeof o->text) &&
           (o->shape != PS_POLYLINE || (o->point_count >= 2 && o->point_first <= points &&
                                        o->point_count <= points - o->point_first));
}
static bool scene_structure_valid(const ps_scene *s) {
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
        uint32_t seen=0,at=i;
        while(s->objects[at].parent_id) {
            if(seen&(UINT32_C(1)<<at)) return false;
            seen|=UINT32_C(1)<<at;
            int parent=ps_scene_parent_index(s,at);
            if(parent<0) return false;
            at=(uint32_t)parent;
        }
    }
    return true;
}
/* At most 32 ancestors; matrices are computed without heap allocation or a
 * shared cache. The source is immutable, including shared polyline ranges. */
ps_result ps_scene_transforms(const ps_scene *s,ps_mat4 *out) {
    if(!out || !scene_structure_valid(s))return PS_INVALID;
    ps_mat4 local[PS_MAX_OBJECTS],world[PS_MAX_OBJECTS];
    bool frames=false;
    for(uint32_t i=0;i<s->count;i++)frames |= s->objects[i].shape==PS_FRAME;
    if(!frames){for(uint32_t i=0;i<s->count;i++)out[i]=ps_mat4_identity();return PS_OK;}
    for(uint32_t i=0;i<s->count;i++) {
        local[i]=ps_mat4_identity();
        const ps_object *o=&s->objects[i];
        if(o->shape==PS_FRAME) {
            if(!o->b.x || !o->b.y || !o->b.z)return PS_INVALID;
            ps_result result=ps_mat4_trs(o->a,o->orientation,o->b,&local[i]);
            if(result!=PS_OK)return result;
        }
    }
    for(uint32_t i=0;i<s->count;i++) {
        uint32_t chain[PS_MAX_OBJECTS],length=0,slot=i;
        for(;;) {
            chain[length++]=slot;
            int parent=ps_scene_parent_index(s,slot);
            if(parent<0)break;
            slot=(uint32_t)parent;
        }
        ps_mat4 matrix=ps_mat4_identity();
        while(length)matrix=ps_mat4_multiply(matrix,local[chain[--length]]);
        for(unsigned k=0;k<16;k++)if(!isfinite(matrix.m[k]))return PS_NUMERIC;
        /* A renderer needs the inverse-transpose of this basis. Validate it
         * independently of translation, which is not part of a normal. */
        ps_mat3 basis={{matrix.m[0],matrix.m[1],matrix.m[2],matrix.m[4],matrix.m[5],matrix.m[6],
                       matrix.m[8],matrix.m[9],matrix.m[10]}},inverse;
        ps_result result=ps_mat3_inverse(basis,0,&inverse);
        if(result!=PS_OK)return result;
        world[i]=matrix;
    }
    memcpy(out,world,s->count*sizeof *out);return PS_OK;
}
bool ps_scene_valid(const ps_scene *s) {
    ps_mat4 matrices[PS_MAX_OBJECTS];return ps_scene_transforms(s,matrices)==PS_OK;
}
ps_result ps_scene_world_point(const ps_scene *s,uint32_t index,ps_vec3 local,ps_vec3 *out) {
    if(!s || !out || index>=s->count)return PS_INVALID;
    ps_mat4 matrices[PS_MAX_OBJECTS];ps_result result=ps_scene_transforms(s,matrices);
    return result==PS_OK?ps_transform_point(matrices[index],local,out):result;
}
ps_result ps_scene_push(ps_scene *s, const ps_object *o) {
    if (!s || s->count >= PS_MAX_OBJECTS || s->point_count > PS_MAX_SCENE_POINTS ||
        !object_valid(o, s->point_count))
        return PS_INVALID;
    if (o->id)
        for (uint32_t i = 0; i < s->count; i++)
            if (s->objects[i].id == o->id)
                return PS_INVALID;
    if(o->parent_id || o->shape==PS_FRAME) {
        ps_scene next=*s;next.objects[next.count++]=*o;
        if(!ps_scene_valid(&next)) return PS_INVALID;
    }
    s->objects[s->count++] = *o;
    return PS_OK;
}
ps_result ps_scene_set_parent(ps_scene *s,uint32_t child,uint32_t parent) {
    if(!child || !ps_scene_valid(s)) return PS_INVALID;
    for(uint32_t i=0;i<s->count;i++) if(s->objects[i].id==child) {
        uint32_t previous=s->objects[i].parent_id;s->objects[i].parent_id=parent;
        if(ps_scene_valid(s)) return PS_OK;
        s->objects[i].parent_id=previous;return PS_INVALID;
    }
    return PS_INVALID;
}
ps_result ps_scene_group(ps_scene *s,uint32_t id,uint32_t parent,const char *name) {
    if(!id || !name) return PS_INVALID;
    ps_object object={0};object.shape=PS_GROUP;object.id=id;object.parent_id=parent;object.color=UINT32_MAX;
    size_t size=0;while(size<sizeof object.text && name[size]) size++;
    if(!size || size==sizeof object.text) return PS_INVALID;
    memcpy(object.text,name,size+1);return ps_scene_push(s,&object);
}
ps_result ps_scene_frame(ps_scene *s,uint32_t id,uint32_t parent,const char *name,
                         ps_vec3 translation,ps_quat rotation,ps_vec3 scale) {
    if(!id || !name)return PS_INVALID;
    ps_quat normalized;ps_result result=ps_quat_normalize(rotation,&normalized);
    if(result!=PS_OK)return result;
    ps_object object={0};object.shape=PS_FRAME;object.id=id;object.parent_id=parent;
    object.color=UINT32_MAX;object.a=translation;object.b=scale;object.orientation=normalized;
    size_t size=0;while(size<sizeof object.text && name[size])size++;
    if(!size || size==sizeof object.text)return PS_INVALID;
    memcpy(object.text,name,size+1);return ps_scene_push(s,&object);
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
