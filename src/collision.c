#include "physim/collision.h"
#include <float.h>
#include <math.h>
#include <string.h>
static bool finite3(ps_vec3 v) { return isfinite(v.x) && isfinite(v.y) && isfinite(v.z); }
static ps_result bounds_of(ps_vec3 center, ps_vec3 half, ps_aabb *out) {
    ps_aabb b = {ps_vsub(center, half), ps_vadd(center, half)};
    b.minimum_m.x = nextafter(b.minimum_m.x, -INFINITY);
    b.minimum_m.y = nextafter(b.minimum_m.y, -INFINITY);
    b.minimum_m.z = nextafter(b.minimum_m.z, -INFINITY);
    b.maximum_m.x = nextafter(b.maximum_m.x, INFINITY);
    b.maximum_m.y = nextafter(b.maximum_m.y, INFINITY);
    b.maximum_m.z = nextafter(b.maximum_m.z, INFINITY);
    if (!finite3(b.minimum_m) || !finite3(b.maximum_m))
        return PS_NUMERIC;
    *out = b;
    return PS_OK;
}
ps_result ps_aabb_sphere(const ps_body *body, double radius, ps_aabb *out) {
    if (!out || ps_body_validate(body) != PS_OK || !isfinite(radius) || radius <= 0)
        return PS_INVALID;
    return bounds_of(body->position_m, ps_v3(radius, radius, radius), out);
}
ps_result ps_aabb_box(const ps_body *body, ps_vec3 size, ps_aabb *out) {
    if (!out || ps_body_validate(body) != PS_OK || !finite3(size) || size.x <= 0 || size.y <= 0 ||
        size.z <= 0)
        return PS_INVALID;
    ps_quat q = body->orientation;
    double norm = hypot(hypot(q.x, q.y), hypot(q.z, q.w));
    q.x /= norm;
    q.y /= norm;
    q.z /= norm;
    q.w /= norm;
    ps_vec3 x = ps_quat_rotate(q, ps_v3(1, 0, 0));
    ps_vec3 y = ps_quat_rotate(q, ps_v3(0, 1, 0));
    ps_vec3 z = ps_quat_rotate(q, ps_v3(0, 0, 1));
    ps_vec3 h = ps_vscale(size, .5);
    double pad = 64 * DBL_EPSILON * fmax(size.x, fmax(size.y, size.z));
    ps_vec3 extent = ps_v3(fabs(x.x) * h.x + fabs(y.x) * h.y + fabs(z.x) * h.z + pad,
                           fabs(x.y) * h.x + fabs(y.y) * h.y + fabs(z.y) * h.z + pad,
                           fabs(x.z) * h.x + fabs(y.z) * h.y + fabs(z.z) * h.z + pad);
    if (!finite3(extent))
        return PS_NUMERIC;
    return bounds_of(body->position_m, extent, out);
}
typedef struct {
    double start;
    uint32_t index;
} sweep_entry;
static int order_entry(const void *va, const void *vb) {
    const sweep_entry *a = va, *b = vb;
    if (a->start != b->start)
        return a->start < b->start ? -1 : 1;
    return a->index < b->index ? -1 : a->index > b->index;
}
static int order_pair(const void *va, const void *vb) {
    const ps_collision_pair *a = va, *b = vb;
    if (a->a != b->a)
        return a->a < b->a ? -1 : 1;
    return a->b < b->b ? -1 : a->b > b->b;
}
/* In-place heapsort: unlike a library qsort, this guarantees no hidden heap use. */
static void swap_item(unsigned char *a, unsigned char *b, size_t size) {
    unsigned char tmp[sizeof(sweep_entry)];
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
}
static void sift(unsigned char *data, size_t root, size_t count, size_t size,
                 int (*compare)(const void *, const void *)) {
    while (root < count / 2) {
        size_t child = root * 2 + 1;
        if (child + 1 < count && compare(data + child * size, data + (child + 1) * size) < 0)
            child++;
        if (compare(data + root * size, data + child * size) >= 0)
            break;
        swap_item(data + root * size, data + child * size, size);
        root = child;
    }
}
static void sort(void *data, size_t count, size_t size,
                 int (*compare)(const void *, const void *)) {
    unsigned char *bytes = data;
    for (size_t i = count / 2; i > 0; i--)
        sift(bytes, i - 1, count, size, compare);
    for (size_t i = count; i > 1; i--) {
        swap_item(bytes, bytes + (i - 1) * size, size);
        sift(bytes, 0, i - 1, size, compare);
    }
}
static size_t sweep(const ps_aabb *bounds, const sweep_entry *entries, size_t count,
                    ps_collision_pair *pairs) {
    size_t found = 0;
    for (size_t i = 0; i < count; i++) {
        uint32_t a = entries[i].index;
        const ps_aabb *box = &bounds[a];
        for (size_t j = i + 1; j < count && entries[j].start <= box->maximum_m.x; j++) {
            uint32_t b = entries[j].index;
            const ps_aabb *other = &bounds[b];
            if (box->minimum_m.y > other->maximum_m.y || other->minimum_m.y > box->maximum_m.y ||
                box->minimum_m.z > other->maximum_m.z || other->minimum_m.z > box->maximum_m.z)
                continue;
            if (pairs)
                pairs[found] = a < b ? (ps_collision_pair){a, b} : (ps_collision_pair){b, a};
            found++;
        }
    }
    return found;
}
ps_result ps_broad_phase(const ps_aabb *bounds, size_t count, ps_collision_pair *pairs,
                         size_t capacity, size_t *pair_count) {
    if ((!bounds && count) || (!pairs && capacity) || !pair_count)
        return PS_INVALID;
    if (count > PS_BROAD_PHASE_MAX_BODIES)
        return PS_LIMIT;
    sweep_entry entries[PS_BROAD_PHASE_MAX_BODIES];
    for (size_t i = 0; i < count; i++) {
        ps_aabb b = bounds[i];
        if (!finite3(b.minimum_m) || !finite3(b.maximum_m) || b.minimum_m.x > b.maximum_m.x ||
            b.minimum_m.y > b.maximum_m.y || b.minimum_m.z > b.maximum_m.z)
            return PS_INVALID;
        entries[i] = (sweep_entry){b.minimum_m.x, (uint32_t)i};
    }
    sort(entries, count, sizeof *entries, order_entry);
    size_t required = sweep(bounds, entries, count, NULL);
    if (required > capacity) {
        *pair_count = required;
        return PS_LIMIT;
    }
    if (required) {
        (void)sweep(bounds, entries, count, pairs);
        sort(pairs, required, sizeof *pairs, order_pair);
    }
    *pair_count = required;
    return PS_OK;
}

static ps_vec3 advance(ps_vec3 p, ps_vec3 delta, double fraction) {
    return ps_v3(fma(delta.x, fraction, p.x), fma(delta.y, fraction, p.y),
                 fma(delta.z, fraction, p.z));
}
static double length3(ps_vec3 v) { return hypot(hypot(v.x, v.y), v.z); }
ps_result ps_sweep_sphere_plane(const ps_body *body, double radius, ps_vec3 displacement,
                                ps_vec3 point, ps_vec3 normal, ps_sweep_hit *hit, bool *touching) {
    if (!hit || !touching || !finite3(displacement))
        return PS_INVALID;
    ps_sweep_hit result = {0};
    bool initial;
    ps_result r = ps_contact_sphere_plane(body, radius, point, normal, &result.contact, &initial);
    if (r != PS_OK)
        return r;
    if (initial) {
        *hit = result;
        *touching = true;
        return PS_OK;
    }
    double distance = ps_vdot(ps_vsub(body->position_m, point), normal) - radius;
    double closing = -ps_vdot(displacement, normal);
    if (!isfinite(distance) || !isfinite(closing))
        return PS_NUMERIC;
    if (closing <= 0 || distance > closing) {
        *touching = false;
        return PS_OK;
    }
    result.fraction = distance / closing;
    ps_vec3 center = advance(body->position_m, displacement, result.fraction);
    result.contact =
        (ps_contact){ps_vsub(center, ps_vscale(normal, radius)), ps_vscale(normal, -1), 0};
    if (!finite3(center) || !finite3(result.contact.point_m))
        return PS_NUMERIC;
    *hit = result;
    *touching = true;
    return PS_OK;
}
ps_result ps_sweep_spheres(const ps_body *a, double ra, ps_vec3 da, const ps_body *b, double rb,
                           ps_vec3 db, ps_sweep_hit *hit, bool *touching) {
    if (!hit || !touching || !finite3(da) || !finite3(db))
        return PS_INVALID;
    ps_sweep_hit result = {0};
    bool initial;
    ps_result r = ps_contact_spheres(a, ra, b, rb, &result.contact, &initial);
    if (r != PS_OK)
        return r;
    if (initial) {
        *hit = result;
        *touching = true;
        return PS_OK;
    }
    ps_vec3 separation = ps_vsub(b->position_m, a->position_m), motion = ps_vsub(da, db);
    double speed = length3(motion), scale = length3(separation), radii = ra + rb;
    if (!isfinite(speed) || !finite3(motion))
        return PS_NUMERIC;
    if (!speed) {
        *touching = false;
        return PS_OK;
    }
    ps_vec3 u = ps_v3(motion.x / speed, motion.y / speed, motion.z / speed);
    ps_vec3 p = ps_v3(separation.x / scale, separation.y / scale, separation.z / scale);
    double projection = ps_vdot(p, u), radius = radii / scale;
    if (projection <= 0) {
        *touching = false;
        return PS_OK;
    }
    if (radius == 0)
        return PS_NUMERIC;
    ps_vec3 perpendicular =
        ps_v3(fma(-projection, u.x, p.x), fma(-projection, u.y, p.y), fma(-projection, u.z, p.z));
    double distance = length3(perpendicular);
    if (distance > radius) {
        *touching = false;
        return PS_OK;
    }
    /* Geometric closest approach avoids subtracting nearly equal quadratic
     * discriminant terms for a tiny target on a long, head-on trajectory. */
    double half = sqrt(radius - distance) * sqrt(radius + distance);
    double entry = fmax(0, projection - half), travel = speed / scale;
    if (entry > travel) {
        *touching = false;
        return PS_OK;
    }
    if (!isfinite(travel) || travel == 0)
        return PS_NUMERIC;
    result.fraction = entry / travel;
    ps_vec3 delta = ps_vadd(perpendicular, ps_vscale(u, half));
    double n = length3(delta);
    if (!isfinite(n) || n == 0)
        return PS_NUMERIC;
    result.contact.normal = ps_v3(delta.x / n, delta.y / n, delta.z / n);
    ps_vec3 ca = advance(a->position_m, da, result.fraction),
            cb = advance(b->position_m, db, result.fraction);
    ps_vec3 pa = ps_vadd(ca, ps_vscale(result.contact.normal, ra));
    ps_vec3 pb = ps_vsub(cb, ps_vscale(result.contact.normal, rb));
    result.contact.point_m = ps_vadd(ps_vscale(pa, .5), ps_vscale(pb, .5));
    result.contact.penetration_m = 0;
    if (!finite3(ca) || !finite3(cb) || !finite3(pa) || !finite3(pb) ||
        !finite3(result.contact.point_m))
        return PS_NUMERIC;
    *hit = result;
    *touching = true;
    return PS_OK;
}
ps_result ps_aabb_swept_sphere(const ps_body *body, double radius, ps_vec3 displacement,
                               ps_aabb *out) {
    if (!out || !finite3(displacement))
        return PS_INVALID;
    ps_aabb start, end;
    ps_result r = ps_aabb_sphere(body, radius, &start);
    if (r != PS_OK)
        return r;
    ps_body moved = *body;
    moved.position_m = advance(body->position_m, displacement, 1);
    if (!finite3(moved.position_m))
        return PS_NUMERIC;
    r = ps_aabb_sphere(&moved, radius, &end);
    if (r != PS_OK)
        return r;
    *out = (ps_aabb){
        ps_v3(fmin(start.minimum_m.x, end.minimum_m.x), fmin(start.minimum_m.y, end.minimum_m.y),
              fmin(start.minimum_m.z, end.minimum_m.z)),
        ps_v3(fmax(start.maximum_m.x, end.maximum_m.x), fmax(start.maximum_m.y, end.maximum_m.y),
              fmax(start.maximum_m.z, end.maximum_m.z))};
    return PS_OK;
}
