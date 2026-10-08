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

/* Convex polyhedron SAT: face normals plus edge cross products. Geometry and
 * closest-feature witnesses are computed in a scaled frame relative to A. */
#define CV_EDGES (3 * PS_CONVEX_MAX_TRIANGLES / 2)
static const double cv_tol = 128 * DBL_EPSILON;
typedef struct {
    ps_vec3 vertices[PS_CONVEX_MAX_VERTICES], normals[PS_CONVEX_MAX_TRIANGLES];
    uint32_t edges[CV_EDGES][2];
    size_t edge_count;
    const ps_convex_mesh *mesh;
    double scale;
} cv_geometry;
static ps_vec3 cv_div(ps_vec3 p, double scale) {
    return ps_v3(p.x / scale, p.y / scale, p.z / scale);
}
static double cv_max(ps_vec3 p) { return fmax(fabs(p.x), fmax(fabs(p.y), fabs(p.z))); }
static ps_result cv_prepare(const ps_convex_mesh *mesh, cv_geometry *g) {
    if (!mesh || !mesh->vertices_m || !mesh->triangles || mesh->vertex_count < 4 ||
        mesh->triangle_count < 4)
        return PS_INVALID;
    if (mesh->vertex_count > PS_CONVEX_MAX_VERTICES || mesh->triangle_count > PS_CONVEX_MAX_TRIANGLES)
        return PS_LIMIT;
    *g = (cv_geometry){0};
    g->mesh = mesh;
    for (size_t i = 0; i < mesh->vertex_count; i++) {
        if (!finite3(mesh->vertices_m[i]))
            return PS_INVALID;
        g->scale = fmax(g->scale, cv_max(mesh->vertices_m[i]));
    }
    if (!(g->scale > 0))
        return PS_INVALID;
    ps_vec3 center = ps_v3(0, 0, 0);
    for (size_t i = 0; i < mesh->vertex_count; i++) {
        g->vertices[i] = cv_div(mesh->vertices_m[i], g->scale);
        center = ps_vadd(center, cv_div(g->vertices[i], (double)mesh->vertex_count));
        for (size_t j = 0; j < i; j++)
            if (length3(ps_vsub(g->vertices[i], g->vertices[j])) <= cv_tol)
                return PS_INVALID;
    }
    unsigned used[PS_CONVEX_MAX_VERTICES] = {0}, incidence[CV_EDGES] = {0};
    for (size_t i = 0; i < mesh->triangle_count; i++) {
        const uint32_t *t = mesh->triangles[i];
        if (t[0] >= mesh->vertex_count || t[1] >= mesh->vertex_count || t[2] >= mesh->vertex_count ||
            t[0] == t[1] || t[1] == t[2] || t[2] == t[0])
            return PS_INVALID;
        ps_vec3 p = g->vertices[t[0]];
        ps_vec3 n = ps_vcross(ps_vsub(g->vertices[t[1]], p), ps_vsub(g->vertices[t[2]], p));
        double length = length3(n);
        if (!(length > cv_tol))
            return PS_INVALID;
        n = cv_div(n, length);
        g->normals[i] = n;
        if (ps_vdot(n, ps_vsub(center, p)) >= -cv_tol)
            return PS_INVALID;
        for (size_t j = 0; j < mesh->vertex_count; j++)
            if (ps_vdot(n, ps_vsub(g->vertices[j], p)) > cv_tol)
                return PS_INVALID;
        for (unsigned j = 0; j < 3; j++) {
            uint32_t a = t[j], b = t[(j + 1) % 3];
            used[a]++;
            size_t e = 0;
            while (e < g->edge_count && !((g->edges[e][0] == a && g->edges[e][1] == b) ||
                                         (g->edges[e][0] == b && g->edges[e][1] == a)))
                e++;
            if (e == g->edge_count) {
                if (e == CV_EDGES)
                    return PS_INVALID;
                g->edges[e][0] = a; g->edges[e][1] = b;
                incidence[e] = 1; g->edge_count++;
            } else {
                if (incidence[e] != 1 || g->edges[e][0] != b)
                    return PS_INVALID;
                incidence[e]++;
            }
        }
    }
    for (size_t e = 0; e < g->edge_count; e++)
        if (incidence[e] != 2)
            return PS_INVALID;
    for (size_t i = 0; i < mesh->vertex_count; i++)
        if (!used[i])
            return PS_INVALID;
    /* A closed convex triangulated sphere has Euler characteristic two. */
    if (mesh->vertex_count + mesh->triangle_count != g->edge_count + 2)
        return PS_INVALID;
    return PS_OK;
}
ps_result ps_convex_validate(const ps_convex_mesh *mesh) {
    cv_geometry g;
    return cv_prepare(mesh, &g);
}
static ps_quat cv_rotation(const ps_body *b) {
    ps_quat q = b->orientation;
    double n = hypot(hypot(q.x, q.y), hypot(q.z, q.w));
    q.x /= n; q.y /= n; q.z /= n; q.w /= n;
    return q;
}
static ps_result cv_world(cv_geometry *g, const ps_body *b, ps_vec3 origin, double scale) {
    ps_vec3 delta = cv_div(ps_vsub(b->position_m, origin), scale);
    if (!finite3(delta))
        return PS_NUMERIC;
    ps_quat q = cv_rotation(b);
    for (size_t i = 0; i < g->mesh->vertex_count; i++) {
        ps_vec3 p = cv_div(g->mesh->vertices_m[i], scale);
        g->vertices[i] = ps_vadd(delta, ps_quat_rotate(q, p));
        if (!finite3(g->vertices[i]))
            return PS_NUMERIC;
    }
    for (size_t i = 0; i < g->edge_count; i++)
        if (length3(ps_vsub(g->vertices[g->edges[i][0]], g->vertices[g->edges[i][1]])) <= cv_tol)
            return PS_NUMERIC;
    for (size_t i = 0; i < g->mesh->triangle_count; i++)
        g->normals[i] = ps_quat_rotate(q, g->normals[i]);
    return PS_OK;
}
static void cv_interval(const cv_geometry *g, ps_vec3 axis, double *lo, double *hi) {
    *lo = *hi = ps_vdot(g->vertices[0], axis);
    for (size_t i = 1; i < g->mesh->vertex_count; i++) {
        double p = ps_vdot(g->vertices[i], axis);
        *lo = fmin(*lo, p); *hi = fmax(*hi, p);
    }
}
static bool cv_axis(const cv_geometry *a, const cv_geometry *b, ps_vec3 axis,
                    double *depth, ps_vec3 *normal) {
    double n = length3(axis);
    if (n <= 32 * DBL_EPSILON)
        return true;
    axis = cv_div(axis, n);
    double al, ah, bl, bh;
    cv_interval(a, axis, &al, &ah); cv_interval(b, axis, &bl, &bh);
    if (bl - ah > cv_tol || al - bh > cv_tol)
        return false;
    double positive = ah - bl, negative = bh - al;
    if (negative < positive) {
        axis = ps_vscale(axis, -1); positive = negative;
    }
    positive = fmax(0, positive);
    if (positive < *depth - cv_tol) {
        *depth = positive; *normal = axis;
    }
    return true;
}
static ps_vec3 cv_segment_point(ps_vec3 p, ps_vec3 a, ps_vec3 b) {
    ps_vec3 d = ps_vsub(b, a);
    double dd = ps_vdot(d, d);
    double t = dd ? fmax(0, fmin(1, ps_vdot(ps_vsub(p, a), d) / dd)) : 0;
    return ps_vadd(a, ps_vscale(d, t));
}
/* Projection onto a triangle, or the nearest of its three segments. */
static ps_vec3 cv_triangle_point(ps_vec3 p, ps_vec3 a, ps_vec3 b, ps_vec3 c, ps_vec3 normal) {
    ps_vec3 projected = ps_vsub(p, ps_vscale(normal, ps_vdot(ps_vsub(p, a), normal)));
    if (ps_vdot(ps_vcross(ps_vsub(b, a), ps_vsub(projected, a)), normal) >= 0 &&
        ps_vdot(ps_vcross(ps_vsub(c, b), ps_vsub(projected, b)), normal) >= 0 &&
        ps_vdot(ps_vcross(ps_vsub(a, c), ps_vsub(projected, c)), normal) >= 0)
        return projected;
    ps_vec3 points[3] = {cv_segment_point(p, a, b), cv_segment_point(p, b, c), cv_segment_point(p, c, a)};
    unsigned best = 0;
    for (unsigned i = 1; i < 3; i++)
        if (length3(ps_vsub(points[i], p)) < length3(ps_vsub(points[best], p)))
            best = i;
    return points[best];
}
static void cv_nearer(ps_vec3 a, ps_vec3 b, double *distance, ps_vec3 *pa, ps_vec3 *pb) {
    double d = length3(ps_vsub(a, b));
    if (d < *distance) { *distance = d; *pa = a; *pb = b; }
}
static void cv_edges_closest(ps_vec3 a, ps_vec3 b, ps_vec3 c, ps_vec3 d,
                             double *distance, ps_vec3 *pa, ps_vec3 *pb) {
    /* Interior line-line candidate plus all endpoint/segment candidates. */
    ps_vec3 u = ps_vsub(b, a), v = ps_vsub(d, c), r = ps_vsub(a, c);
    double aa = ps_vdot(u, u), bb = ps_vdot(u, v), cc = ps_vdot(v, v);
    double ar = ps_vdot(u, r), cr = ps_vdot(v, r);
    double denominator = ps_vdot(ps_vcross(u, v), ps_vcross(u, v));
    if (denominator > 0) {
        double s = (bb * cr - cc * ar) / denominator;
        double t = (aa * cr - bb * ar) / denominator;
        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
            cv_nearer(ps_vadd(a, ps_vscale(u, s)), ps_vadd(c, ps_vscale(v, t)), distance, pa, pb);
    }
    cv_nearer(a, cv_segment_point(a, c, d), distance, pa, pb);
    cv_nearer(b, cv_segment_point(b, c, d), distance, pa, pb);
    cv_nearer(cv_segment_point(c, a, b), c, distance, pa, pb);
    cv_nearer(cv_segment_point(d, a, b), d, distance, pa, pb);
}
static ps_result cv_contact(ps_vec3 origin, double scale, ps_vec3 point, ps_vec3 normal,
                            double depth, ps_contact *out, bool *touching) {
    ps_contact result = {ps_v3(fma(point.x, scale, origin.x), fma(point.y, scale, origin.y),
                              fma(point.z, scale, origin.z)), normal, depth * scale};
    if (!finite3(result.point_m) || !finite3(normal) || !isfinite(result.penetration_m))
        return PS_NUMERIC;
    *out = result; *touching = true;
    return PS_OK;
}
static double cv_nearest_geometry(const cv_geometry *ga,const cv_geometry *gb,ps_vec3 *pa,ps_vec3 *pb) {
    double distance = INFINITY;
    for (size_t i = 0; i < ga->mesh->vertex_count; i++)
        for (size_t j = 0; j < gb->mesh->triangle_count; j++) {
            const uint32_t *t = gb->mesh->triangles[j];
            cv_nearer(ga->vertices[i], cv_triangle_point(ga->vertices[i], gb->vertices[t[0]],
                      gb->vertices[t[1]], gb->vertices[t[2]], gb->normals[j]), &distance, pa, pb);
        }
    for (size_t i = 0; i < gb->mesh->vertex_count; i++)
        for (size_t j = 0; j < ga->mesh->triangle_count; j++) {
            const uint32_t *t = ga->mesh->triangles[j];
            cv_nearer(cv_triangle_point(gb->vertices[i], ga->vertices[t[0]], ga->vertices[t[1]],
                      ga->vertices[t[2]], ga->normals[j]), gb->vertices[i], &distance, pa, pb);
        }
    for (size_t i = 0; i < ga->edge_count; i++)
        for (size_t j = 0; j < gb->edge_count; j++)
            cv_edges_closest(ga->vertices[ga->edges[i][0]], ga->vertices[ga->edges[i][1]],
                             gb->vertices[gb->edges[j][0]], gb->vertices[gb->edges[j][1]],
                             &distance, pa, pb);
    return distance;
}
static ps_result cv_witness(cv_geometry *ga, cv_geometry *gb, ps_vec3 origin,
                             double scale, ps_vec3 normal, double depth,
                             double tolerance, ps_contact *out, bool *touching) {
    /* Translate B by the minimum separating displacement, find surface witnesses,
     * then return their midpoint in the original overlapped configuration. */
    ps_vec3 shift = ps_vscale(normal, depth);
    for (size_t i = 0; i < gb->mesh->vertex_count; i++) gb->vertices[i] = ps_vadd(gb->vertices[i], shift);
    ps_vec3 pa={0},pb={0};double distance=cv_nearest_geometry(ga,gb,&pa,&pb);
    if (!isfinite(depth) || distance > tolerance)
        return PS_NUMERIC;
    return cv_contact(origin, scale, ps_vscale(ps_vsub(ps_vadd(pa, pb), shift), .5),
                      normal, depth, out, touching);
}
ps_result ps_contact_convexes(const ps_body *a, const ps_convex_mesh *ma,
                              const ps_body *b, const ps_convex_mesh *mb,
                              ps_contact *out, bool *touching) {
    if (!out || !touching || a == b || ps_body_validate(a) != PS_OK || ps_body_validate(b) != PS_OK)
        return PS_INVALID;
    cv_geometry ga, gb;
    ps_result result = cv_prepare(ma, &ga);
    if (result != PS_OK) return result;
    result = cv_prepare(mb, &gb);
    if (result != PS_OK) return result;
    double scale = fmax(ga.scale, gb.scale);
    result = cv_world(&ga, a, a->position_m, scale);
    if (result != PS_OK) return result;
    result = cv_world(&gb, b, a->position_m, scale);
    if (result != PS_OK) return result;
    double depth = INFINITY; ps_vec3 normal = ps_v3(0, 0, 0);
    for (size_t i = 0; i < ma->triangle_count; i++)
        if (!cv_axis(&ga, &gb, ga.normals[i], &depth, &normal)) goto separated;
    for (size_t i = 0; i < mb->triangle_count; i++)
        if (!cv_axis(&ga, &gb, gb.normals[i], &depth, &normal)) goto separated;
    for (size_t i = 0; i < ga.edge_count; i++) {
        ps_vec3 u = ps_vsub(ga.vertices[ga.edges[i][1]], ga.vertices[ga.edges[i][0]]);
        u = cv_div(u, length3(u));
        for (size_t j = 0; j < gb.edge_count; j++) {
            ps_vec3 v = ps_vsub(gb.vertices[gb.edges[j][1]], gb.vertices[gb.edges[j][0]]);
            v = cv_div(v, length3(v));
            if (!cv_axis(&ga, &gb, ps_vcross(u, v), &depth, &normal)) goto separated;
        }
    }
    return cv_witness(&ga, &gb, a->position_m, scale, normal, depth, 8 * cv_tol, out, touching);
separated:
    *touching = false;
    return PS_OK;
}
ps_result ps_contact_convex_plane(const ps_body *body, const ps_convex_mesh *mesh,
                                  ps_vec3 point, ps_vec3 normal, ps_contact *out, bool *touching) {
    if (!out || !touching || ps_body_validate(body) != PS_OK || !finite3(point) ||
        !finite3(normal) || fabs(length3(normal) - 1) > 1e-8)
        return PS_INVALID;
    cv_geometry g;
    ps_result result = cv_prepare(mesh, &g);
    if (result != PS_OK) return result;
    result = cv_world(&g, body, point, g.scale);
    if (result != PS_OK) return result;
    size_t nearest = 0;
    double distance = ps_vdot(g.vertices[0], normal);
    for (size_t i = 1; i < mesh->vertex_count; i++) {
        double d = ps_vdot(g.vertices[i], normal);
        if (d < distance) { distance = d; nearest = i; }
    }
    if (distance > cv_tol) { *touching = false; return PS_OK; }
    return cv_contact(point, g.scale, g.vertices[nearest], ps_vscale(normal, -1),
                      fmax(0, -distance), out, touching);
}
ps_result ps_contact_sphere_convex(const ps_body *sphere, double radius,
                                   const ps_body *body, const ps_convex_mesh *mesh,
                                   ps_contact *out, bool *touching) {
    if (!out || !touching || sphere == body || ps_body_validate(sphere) != PS_OK ||
        ps_body_validate(body) != PS_OK || !isfinite(radius) || radius <= 0)
        return PS_INVALID;
    cv_geometry g;
    ps_result result = cv_prepare(mesh, &g);
    if (result != PS_OK) return result;
    double scale = fmax(g.scale, radius);
    result = cv_world(&g, body, sphere->position_m, scale);
    if (result != PS_OK) return result;
    double distance = INFINITY; ps_vec3 closest = {0}; bool inside = true;
    for (size_t i = 0; i < mesh->triangle_count; i++) {
        const uint32_t *t = mesh->triangles[i];
        if (ps_vdot(g.normals[i], ps_vscale(g.vertices[t[0]], -1)) > cv_tol) inside = false;
        ps_vec3 p = cv_triangle_point(ps_v3(0, 0, 0), g.vertices[t[0]], g.vertices[t[1]],
                                      g.vertices[t[2]], g.normals[i]);
        double d = length3(p);
        if (d < distance) { distance = d; closest = p; }
    }
    double r = radius / scale;
    if (!inside && distance > r + cv_tol) { *touching = false; return PS_OK; }
    ps_vec3 normal;
    if (distance > cv_tol) normal = cv_div(closest, inside ? -distance : distance);
    else {
        size_t face = 0;
        double nearest = INFINITY;
        for (size_t i = 0; i < mesh->triangle_count; i++) {
            double d = fabs(ps_vdot(g.normals[i], g.vertices[mesh->triangles[i][0]]));
            if (d < nearest) { nearest = d; face = i; }
        }
        normal = ps_vscale(g.normals[face], -1);
    }
    double depth = inside ? r + distance : fmax(0, r - distance);
    ps_vec3 sphere_point = ps_vscale(normal, r);
    return cv_contact(sphere->position_m, scale, ps_vscale(ps_vadd(sphere_point, closest), .5),
                      normal, depth, out, touching);
}
ps_result ps_aabb_convex(const ps_body *body, const ps_convex_mesh *mesh, ps_aabb *out) {
    if (!out || ps_body_validate(body) != PS_OK) return PS_INVALID;
    cv_geometry g;
    ps_result result = cv_prepare(mesh, &g);
    if (result != PS_OK) return result;
    result = cv_world(&g, body, body->position_m, g.scale);
    if (result != PS_OK) return result;
    ps_aabb bounds = {g.vertices[0], g.vertices[0]};
    for (size_t i = 1; i < mesh->vertex_count; i++) {
        ps_vec3 p = g.vertices[i];
        bounds.minimum_m = ps_v3(fmin(bounds.minimum_m.x, p.x), fmin(bounds.minimum_m.y, p.y), fmin(bounds.minimum_m.z, p.z));
        bounds.maximum_m = ps_v3(fmax(bounds.maximum_m.x, p.x), fmax(bounds.maximum_m.y, p.y), fmax(bounds.maximum_m.z, p.z));
    }
    ps_vec3 lo = bounds.minimum_m, hi = bounds.maximum_m;
    bounds.minimum_m = ps_v3(nextafter(fma(lo.x - cv_tol, g.scale, body->position_m.x), -INFINITY),
                            nextafter(fma(lo.y - cv_tol, g.scale, body->position_m.y), -INFINITY),
                            nextafter(fma(lo.z - cv_tol, g.scale, body->position_m.z), -INFINITY));
    bounds.maximum_m = ps_v3(nextafter(fma(hi.x + cv_tol, g.scale, body->position_m.x), INFINITY),
                            nextafter(fma(hi.y + cv_tol, g.scale, body->position_m.y), INFINITY),
                            nextafter(fma(hi.z + cv_tol, g.scale, body->position_m.z), INFINITY));
    if (!finite3(bounds.minimum_m) || !finite3(bounds.maximum_m)) return PS_NUMERIC;
    *out = bounds;
    return PS_OK;
}

/* Intersect the closed time interval for each stationary-orientation SAT axis.
 * Infinite quotients mean entry/exit outside the finite [0,1] query interval. */
static ps_result cv_sweep_axis(const cv_geometry *a, const cv_geometry *b, ps_vec3 axis,
                               ps_vec3 relative, double *first, double *last,
                               ps_vec3 *normal, bool *possible) {
    double n = length3(axis);
    if (n <= 32 * DBL_EPSILON)
        return PS_OK;
    axis = cv_div(axis, n);
    double al, ah, bl, bh;
    cv_interval(a, axis, &al, &ah); cv_interval(b, axis, &bl, &bh);
    double speed = ps_vdot(relative, axis);
    if (!isfinite(al) || !isfinite(ah) || !isfinite(bl) || !isfinite(bh) || !isfinite(speed))
        return PS_NUMERIC;
    if (speed == 0) {
        if (bl - ah > cv_tol || al - bh > cv_tol)
            *possible = false;
        return PS_OK;
    }
    double enter, leave;
    ps_vec3 direction;
    if (speed > 0) {
        enter = (al - bh) / speed; leave = (ah - bl) / speed;
        direction = ps_vscale(axis, -1);
    } else {
        enter = (ah - bl) / speed; leave = (al - bh) / speed;
        direction = axis;
    }
    if (enter > *first) { *first = enter; *normal = direction; }
    *last = fmin(*last, leave);
    if (*first > *last)
        *possible = false;
    return PS_OK;
}
ps_result ps_sweep_convexes(const ps_body *a, const ps_convex_mesh *ma, ps_vec3 da,
                            const ps_body *b, const ps_convex_mesh *mb, ps_vec3 db,
                            ps_sweep_hit *hit, bool *touching) {
    if (!hit || !touching || !finite3(da) || !finite3(db))
        return PS_INVALID;
    ps_sweep_hit candidate = {0}; bool initial;
    ps_result result = ps_contact_convexes(a, ma, b, mb, &candidate.contact, &initial);
    if (result != PS_OK) return result;
    if (initial) { *hit = candidate; *touching = true; return PS_OK; }
    cv_geometry ga, gb;
    result = cv_prepare(ma, &ga); if (result != PS_OK) return result;
    result = cv_prepare(mb, &gb); if (result != PS_OK) return result;
    double scale = fmax(ga.scale, gb.scale);
    result = cv_world(&ga, a, a->position_m, scale); if (result != PS_OK) return result;
    result = cv_world(&gb, b, a->position_m, scale); if (result != PS_OK) return result;
    ps_vec3 motion_a = cv_div(da, scale), motion_b = cv_div(db, scale);
    ps_vec3 relative = ps_vsub(motion_b, motion_a);
    if (!finite3(motion_a) || !finite3(motion_b) || !finite3(relative)) return PS_NUMERIC;
    double first = 0, last = 1; bool possible = true; ps_vec3 normal = {0};
    for (size_t i = 0; i < ma->triangle_count && possible; i++) {
        result = cv_sweep_axis(&ga, &gb, ga.normals[i], relative, &first, &last, &normal, &possible);
        if (result != PS_OK) return result;
    }
    for (size_t i = 0; i < mb->triangle_count && possible; i++) {
        result = cv_sweep_axis(&ga, &gb, gb.normals[i], relative, &first, &last, &normal, &possible);
        if (result != PS_OK) return result;
    }
    for (size_t i = 0; i < ga.edge_count && possible; i++) {
        ps_vec3 u = ps_vsub(ga.vertices[ga.edges[i][1]], ga.vertices[ga.edges[i][0]]);
        u = cv_div(u, length3(u));
        for (size_t j = 0; j < gb.edge_count && possible; j++) {
            ps_vec3 v = ps_vsub(gb.vertices[gb.edges[j][1]], gb.vertices[gb.edges[j][0]]);
            v = cv_div(v, length3(v));
            result = cv_sweep_axis(&ga, &gb, ps_vcross(u, v), relative, &first, &last, &normal, &possible);
            if (result != PS_OK) return result;
        }
    }
    if (!possible) { *touching = false; return PS_OK; }
    if (!isfinite(first) || first < 0 || first > 1 || length3(normal) == 0) return PS_NUMERIC;
    /* Work in A's moving frame. fma limits cancellation in long relative paths. */
    double uncertainty = cv_tol;
    for (size_t i = 0; i < mb->vertex_count; i++) {
        ps_vec3 p = gb.vertices[i];
        gb.vertices[i] = advance(p, relative, first);
        uncertainty = fmax(uncertainty, DBL_EPSILON * fmax(cv_max(p), cv_max(relative) * first));
        if (!finite3(gb.vertices[i])) return PS_NUMERIC;
    }
    ps_vec3 origin = advance(a->position_m, da, first);
    if (!finite3(origin)) return PS_NUMERIC;
    bool found;
    result = cv_witness(&ga, &gb, origin, scale, normal, 0, 8 * uncertainty, &candidate.contact, &found);
    if (result != PS_OK) return result;
    candidate.fraction = first;
    *hit = candidate; *touching = true;
    return PS_OK;
}
ps_result ps_sweep_convex_plane(const ps_body *body, const ps_convex_mesh *mesh,
                                ps_vec3 displacement, ps_vec3 point, ps_vec3 normal,
                                ps_sweep_hit *hit, bool *touching) {
    if (!hit || !touching || !finite3(displacement)) return PS_INVALID;
    ps_sweep_hit candidate = {0}; bool initial;
    ps_result result = ps_contact_convex_plane(body, mesh, point, normal, &candidate.contact, &initial);
    if (result != PS_OK) return result;
    if (initial) { *hit = candidate; *touching = true; return PS_OK; }
    cv_geometry g;
    result = cv_prepare(mesh, &g); if (result != PS_OK) return result;
    result = cv_world(&g, body, point, g.scale); if (result != PS_OK) return result;
    size_t nearest = 0;
    double distance = ps_vdot(g.vertices[0], normal);
    for (size_t i = 1; i < mesh->vertex_count; i++) {
        double d = ps_vdot(g.vertices[i], normal);
        if (d < distance) { distance = d; nearest = i; }
    }
    ps_vec3 motion = cv_div(displacement, g.scale);
    double closing = -ps_vdot(motion, normal);
    if (!isfinite(distance) || !isfinite(closing) || !finite3(motion)) return PS_NUMERIC;
    if (closing <= 0 || distance > closing) { *touching = false; return PS_OK; }
    candidate.fraction = distance / closing;
    ps_vec3 contact = advance(g.vertices[nearest], motion, candidate.fraction);
    /* Project away root rounding in the plane-normal component. */
    contact = ps_vsub(contact, ps_vscale(normal, ps_vdot(contact, normal)));
    bool found;
    result = cv_contact(point, g.scale, contact, ps_vscale(normal, -1), 0, &candidate.contact, &found);
    if (result != PS_OK) return result;
    *hit = candidate; *touching = true;
    return PS_OK;
}
ps_result ps_aabb_swept_convex(const ps_body *body, const ps_convex_mesh *mesh,
                               ps_vec3 displacement, ps_aabb *out) {
    if (!out || !finite3(displacement)) return PS_INVALID;
    ps_aabb start, end;
    ps_result result = ps_aabb_convex(body, mesh, &start);
    if (result != PS_OK) return result;
    ps_body moved = *body;
    moved.position_m = advance(body->position_m, displacement, 1);
    if (!finite3(moved.position_m)) return PS_NUMERIC;
    result = ps_aabb_convex(&moved, mesh, &end);
    if (result != PS_OK) return result;
    *out = (ps_aabb){
        ps_v3(fmin(start.minimum_m.x, end.minimum_m.x), fmin(start.minimum_m.y, end.minimum_m.y), fmin(start.minimum_m.z, end.minimum_m.z)),
        ps_v3(fmax(start.maximum_m.x, end.maximum_m.x), fmax(start.maximum_m.y, end.maximum_m.y), fmax(start.maximum_m.z, end.maximum_m.z))};
    return PS_OK;
}

/* Entry of a ray moving from zero towards a ball centered at separation.
 * Closest-approach geometry avoids quadratic-discriminant cancellation. */
static ps_result cv_ray_ball(ps_vec3 separation, ps_vec3 motion, double radius,
                             double *fraction, bool *found) {
    double distance = length3(separation), speed = length3(motion);
    *found = false;
    if (!isfinite(distance) || !isfinite(speed)) return PS_NUMERIC;
    if (distance <= radius) { *fraction = 0; *found = true; return PS_OK; }
    if (!speed) return PS_OK;
    ps_vec3 u = cv_div(motion, speed), p = cv_div(separation, distance);
    double along = ps_vdot(p, u), r = radius / distance;
    if (along <= 0) return PS_OK;
    if (!(r > 0)) return PS_NUMERIC;
    ps_vec3 perpendicular = ps_v3(fma(-along,u.x,p.x),fma(-along,u.y,p.y),fma(-along,u.z,p.z));
    double away = length3(perpendicular);
    if (away > r) return PS_OK;
    double half = sqrt(r-away)*sqrt(r+away);
    double entry = fmax(0, along-half), travel = speed/distance;
    if (entry > travel) return PS_OK;
    if (!isfinite(travel) || travel == 0) return PS_NUMERIC;
    *fraction = entry/travel; *found = true;
    return PS_OK;
}
static bool cv_triangle_contains(ps_vec3 p, ps_vec3 a, ps_vec3 b, ps_vec3 c, ps_vec3 n) {
    return ps_vdot(ps_vcross(ps_vsub(b,a),ps_vsub(p,a)),n)>=-cv_tol &&
           ps_vdot(ps_vcross(ps_vsub(c,b),ps_vsub(p,b)),n)>=-cv_tol &&
           ps_vdot(ps_vcross(ps_vsub(a,c),ps_vsub(p,c)),n)>=-cv_tol;
}
ps_result ps_sweep_sphere_convex(const ps_body *sphere, double radius, ps_vec3 ds,
                                 const ps_body *body, const ps_convex_mesh *mesh, ps_vec3 db,
                                 ps_sweep_hit *hit, bool *touching) {
    if (!hit || !touching || !finite3(ds) || !finite3(db)) return PS_INVALID;
    ps_sweep_hit candidate = {0}; bool initial;
    ps_result result = ps_contact_sphere_convex(sphere,radius,body,mesh,&candidate.contact,&initial);
    if (result != PS_OK) return result;
    if (initial) { *hit=candidate; *touching=true; return PS_OK; }
    cv_geometry g;
    result=cv_prepare(mesh,&g); if(result!=PS_OK)return result;
    double scale=fmax(g.scale,radius),r=radius/scale;
    result=cv_world(&g,body,sphere->position_m,scale); if(result!=PS_OK)return result;
    ps_vec3 motion=ps_vsub(cv_div(ds,scale),cv_div(db,scale));
    if (!finite3(motion) || !(r>0)) return PS_NUMERIC;
    double first=INFINITY;
    for(size_t i=0;i<mesh->triangle_count;i++) {
        const uint32_t *t=mesh->triangles[i];ps_vec3 n=g.normals[i],p=g.vertices[t[0]];
        double distance=-ps_vdot(n,p),closing=-ps_vdot(n,motion);
        if(!isfinite(distance)||!isfinite(closing))return PS_NUMERIC;
        if(distance<r || closing<=0)continue;
        double entry=(distance-r)/closing;
        if(entry<0 || entry>1 || entry>=first)continue;
        ps_vec3 center=ps_vscale(motion,entry),point=ps_vsub(center,ps_vscale(n,r));
        if(cv_triangle_contains(point,p,g.vertices[t[1]],g.vertices[t[2]],n))first=entry;
    }
    for(size_t i=0;i<g.edge_count;i++) {
        ps_vec3 a=g.vertices[g.edges[i][0]],b=g.vertices[g.edges[i][1]],edge=ps_vsub(b,a);
        double length=length3(edge);ps_vec3 u=cv_div(edge,length);
        double along=ps_vdot(a,u),velocity=ps_vdot(motion,u);
        ps_vec3 radial_a=ps_v3(fma(-along,u.x,a.x),fma(-along,u.y,a.y),fma(-along,u.z,a.z));
        ps_vec3 radial_v=ps_v3(fma(-velocity,u.x,motion.x),fma(-velocity,u.y,motion.y),fma(-velocity,u.z,motion.z));
        double entry;bool found;
        result=cv_ray_ball(radial_a,radial_v,r,&entry,&found);if(result!=PS_OK)return result;
        if(!found || entry>=first)continue;
        double parameter=fma(velocity,entry,-along);
        if(parameter>=0 && parameter<=length)first=entry;
    }
    for(size_t i=0;i<mesh->vertex_count;i++) {
        double entry;bool found;
        result=cv_ray_ball(g.vertices[i],motion,r,&entry,&found);if(result!=PS_OK)return result;
        if(found && entry<first)first=entry;
    }
    if(!isfinite(first)){*touching=false;return PS_OK;}
    /* Reconstruct the nearest mesh surface at the chosen entry in its frame. */
    ps_vec3 center=ps_vscale(motion,first),closest={0};double distance=INFINITY;
    for(size_t i=0;i<mesh->triangle_count;i++) {
        const uint32_t *t=mesh->triangles[i];
        ps_vec3 point=cv_triangle_point(center,g.vertices[t[0]],g.vertices[t[1]],g.vertices[t[2]],g.normals[i]);
        double d=length3(ps_vsub(point,center));
        if(d<distance){distance=d;closest=point;}
    }
    double uncertainty=8*fmax(cv_tol,DBL_EPSILON*cv_max(center));
    if(!isfinite(distance)||distance==0||fabs(distance-r)>uncertainty)return PS_NUMERIC;
    ps_vec3 normal=cv_div(ps_vsub(closest,center),distance);
    ps_vec3 world_center=advance(sphere->position_m,ds,first);
    ps_vec3 world_point=ps_v3(fma(normal.x,radius,world_center.x),fma(normal.y,radius,world_center.y),fma(normal.z,radius,world_center.z));
    if(!finite3(world_center)||!finite3(world_point))return PS_NUMERIC;
    candidate.fraction=first;candidate.contact=(ps_contact){world_point,normal,0};
    *hit=candidate;*touching=true;
    return PS_OK;
}

const ps_ccd_settings PS_CCD_DEFAULT = {1e-8, 4096};
static bool cv_motion_valid(ps_rigid_motion m) {
    return finite3(m.translation_m) && finite3(m.rotation_rad) && finite3(m.quadratic_m) &&
           isfinite(length3(m.rotation_rad));
}
ps_result ps_body_motion_pose(const ps_body *body, ps_rigid_motion m, double fraction, ps_body *out) {
    if (!out || ps_body_validate(body)!=PS_OK || !cv_motion_valid(m) ||
        !isfinite(fraction) || fraction<0 || fraction>1) return PS_INVALID;
    ps_body result=*body;
    result.orientation=cv_rotation(body);
    double square=fraction*fraction;
    result.position_m=ps_v3(fma(square,m.quadratic_m.x,fma(fraction,m.translation_m.x,body->position_m.x)),
                           fma(square,m.quadratic_m.y,fma(fraction,m.translation_m.y,body->position_m.y)),
                           fma(square,m.quadratic_m.z,fma(fraction,m.translation_m.z,body->position_m.z)));
    double angle=length3(m.rotation_rad);
    if (angle) {
        ps_vec3 v=ps_vscale(cv_div(m.rotation_rad,angle),sin(.5*fraction*angle));
        double w=cos(.5*fraction*angle);ps_quat q=cv_rotation(body);ps_vec3 qv=ps_v3(q.x,q.y,q.z);
        ps_vec3 xyz=ps_vadd(ps_vadd(ps_vscale(qv,w),ps_vscale(v,q.w)),ps_vcross(v,qv));
        result.orientation=(ps_quat){xyz.x,xyz.y,xyz.z,w*q.w-ps_vdot(v,qv)};
        double n=hypot(hypot(result.orientation.x,result.orientation.y),hypot(result.orientation.z,result.orientation.w));
        result.orientation.x/=n;result.orientation.y/=n;result.orientation.z/=n;result.orientation.w/=n;
    }
    if(ps_body_validate(&result)!=PS_OK)return PS_NUMERIC;
    *out=result;return PS_OK;
}
static double cv_mesh_radius(const cv_geometry *g) {
    double radius=0;
    for(size_t i=0;i<g->mesh->vertex_count;i++)radius=fmax(radius,length3(g->vertices[i]));
    return nextafter(radius,INFINITY);
}
static double cv_polynomial(double base,double linear,double quadratic,double t) {
    return fma(t*t,quadratic,fma(t,linear,base));
}
ps_result ps_aabb_motion_convex(const ps_body *body,const ps_convex_mesh *mesh,ps_rigid_motion m,ps_aabb *out) {
    if(!out || ps_body_validate(body)!=PS_OK || !cv_motion_valid(m))return PS_INVALID;
    cv_geometry g;ps_result result=cv_prepare(mesh,&g);if(result!=PS_OK)return result;
    double radius=nextafter((cv_mesh_radius(&g)+4*cv_tol)*g.scale,INFINITY);
    if(!isfinite(radius))return PS_NUMERIC;
    double p[3]={body->position_m.x,body->position_m.y,body->position_m.z};
    double d[3]={m.translation_m.x,m.translation_m.y,m.translation_m.z};
    double a[3]={m.quadratic_m.x,m.quadratic_m.y,m.quadratic_m.z},lo[3],hi[3];
    for(unsigned i=0;i<3;i++) {
        double end=cv_polynomial(p[i],d[i],a[i],1);
        lo[i]=fmin(p[i],end);hi[i]=fmax(p[i],end);
        if(a[i]) {
            double t=-.5*(d[i]/a[i]);
            if(t>0 && t<1) {double v=cv_polynomial(p[i],d[i],a[i],t);lo[i]=fmin(lo[i],v);hi[i]=fmax(hi[i],v);}
        }
        double pad=radius+4*cv_tol*fmax(fabs(p[i]),fmax(fabs(d[i]),fabs(a[i])));
        lo[i]=nextafter(lo[i]-pad,-INFINITY);hi[i]=nextafter(hi[i]+pad,INFINITY);
        if(!isfinite(end)||!isfinite(lo[i])||!isfinite(hi[i]))return PS_NUMERIC;
    }
    *out=(ps_aabb){ps_v3(lo[0],lo[1],lo[2]),ps_v3(hi[0],hi[1],hi[2])};return PS_OK;
}

static double cv_gap_axis(const cv_geometry *a,const cv_geometry *b,ps_vec3 axis) {
    double n=length3(axis);if(n<=32*DBL_EPSILON)return 0;
    axis=cv_div(axis,n);double al,ah,bl,bh;
    cv_interval(a,axis,&al,&ah);cv_interval(b,axis,&bl,&bh);
    return fmax(0,fmax(bl-ah,al-bh));
}
static double cv_gap(const cv_geometry *a,const cv_geometry *b) {
    double gap=0;
    for(size_t i=0;i<a->mesh->triangle_count;i++)gap=fmax(gap,cv_gap_axis(a,b,a->normals[i]));
    for(size_t i=0;i<b->mesh->triangle_count;i++)gap=fmax(gap,cv_gap_axis(a,b,b->normals[i]));
    for(size_t i=0;i<a->edge_count;i++) {
        ps_vec3 u=ps_vsub(a->vertices[a->edges[i][1]],a->vertices[a->edges[i][0]]);u=cv_div(u,length3(u));
        for(size_t j=0;j<b->edge_count;j++) {
            ps_vec3 v=ps_vsub(b->vertices[b->edges[j][1]],b->vertices[b->edges[j][0]]);v=cv_div(v,length3(v));
            gap=fmax(gap,cv_gap_axis(a,b,ps_vcross(u,v)));
        }
    }
    return gap;
}
/* kind 0: mesh/mesh; 1: sphere/mesh; 2: mesh/fixed halfspace. */
static ps_result cv_motion_initial(unsigned kind,const ps_body *a,const ps_convex_mesh *ma,double radius,
    const ps_body *b,const ps_convex_mesh *mb,ps_vec3 point,ps_vec3 normal,ps_contact *contact,bool *touching) {
    if(kind==0)return ps_contact_convexes(a,ma,b,mb,contact,touching);
    if(kind==1)return ps_contact_sphere_convex(a,radius,b,mb,contact,touching);
    return ps_contact_convex_plane(a,ma,point,normal,contact,touching);
}
static ps_result cv_motion_query(unsigned kind,const ps_body *a,const ps_convex_mesh *ma,double radius,
    ps_rigid_motion motion_a,const ps_body *b,const ps_convex_mesh *mb,ps_rigid_motion motion_b,
    ps_vec3 point,ps_vec3 normal,const ps_ccd_settings *supplied,ps_sweep_hit *hit,bool *touching) {
    ps_ccd_settings settings=supplied?*supplied:PS_CCD_DEFAULT;
    if(!hit||!touching||!cv_motion_valid(motion_a)||!cv_motion_valid(motion_b)||
       !isfinite(settings.distance_tolerance_m)||settings.distance_tolerance_m<=0||!settings.max_iterations)
        return PS_INVALID;
    if(settings.max_iterations>PS_CCD_MAX_ITERATIONS)return PS_LIMIT;
    ps_sweep_hit candidate={0};bool initial;
    ps_result result=cv_motion_initial(kind,a,ma,radius,b,mb,point,normal,&candidate.contact,&initial);
    if(result!=PS_OK)return result;
    if(initial){*hit=candidate;*touching=true;return PS_OK;}
    cv_geometry base_a={0},base_b={0};double scale;
    if(kind!=1){result=cv_prepare(ma,&base_a);if(result!=PS_OK)return result;}
    if(kind!=2){result=cv_prepare(mb,&base_b);if(result!=PS_OK)return result;}
    scale=kind==0?fmax(base_a.scale,base_b.scale):kind==1?fmax(radius,base_b.scale):base_a.scale;
    double tol=settings.distance_tolerance_m/scale;
    if(!isfinite(tol)||tol<16*cv_tol)return PS_NUMERIC;
    double angle_a=kind==1?0:length3(motion_a.rotation_rad),angle_b=kind==2?0:length3(motion_b.rotation_rad);
    double ra=kind==1?0:cv_mesh_radius(&base_a)*(base_a.scale/scale);
    double rb=kind==2?0:cv_mesh_radius(&base_b)*(base_b.scale/scale);
    ps_vec3 linear=ps_vsub(cv_div(motion_a.translation_m,scale),cv_div(motion_b.translation_m,scale));
    ps_vec3 quadratic=ps_vsub(cv_div(motion_a.quadratic_m,scale),cv_div(motion_b.quadratic_m,scale));
    double bound=(length3(linear)+2*length3(quadratic)+angle_a*ra+angle_b*rb)*(1+1e-12);
    if(!isfinite(bound))return PS_NUMERIC;
    double pose_coordinates=fmax(cv_max(a->position_m)/scale,
        fmax(cv_max(motion_a.translation_m)/scale,cv_max(motion_a.quadratic_m)/scale));
    if(kind!=2)pose_coordinates=fmax(pose_coordinates,fmax(cv_max(b->position_m)/scale,
        fmax(cv_max(motion_b.translation_m)/scale,cv_max(motion_b.quadratic_m)/scale)));
    else pose_coordinates=fmax(pose_coordinates,cv_max(point)/scale);
    double t=0;
    for(uint32_t iteration=0;iteration<settings.max_iterations;iteration++) {
        ps_body pa,pb={0};
        result=ps_body_motion_pose(a,motion_a,t,&pa);if(result!=PS_OK)return result;
        if(kind!=2){result=ps_body_motion_pose(b,motion_b,t,&pb);if(result!=PS_OK)return result;}
        cv_geometry ga=base_a,gb=base_b;ps_vec3 origin=pa.position_m;
        if(kind!=1){result=cv_world(&ga,&pa,kind==2?point:origin,scale);if(result!=PS_OK)return result;}
        if(kind!=2){result=cv_world(&gb,&pb,origin,scale);if(result!=PS_OK)return result;}
        ps_vec3 wa={0},wb={0},direction={0};double upper,lower,coordinates=fmax(1,3*pose_coordinates);
        if(kind==0) {
            upper=cv_nearest_geometry(&ga,&gb,&wa,&wb);lower=cv_gap(&ga,&gb);
            if(upper>0){direction=cv_div(ps_vsub(wb,wa),upper);lower=fmax(lower,cv_gap_axis(&ga,&gb,direction));}
        } else if(kind==1) {
            double distance=INFINITY;
            for(size_t i=0;i<mb->triangle_count;i++) {
                const uint32_t *tri=mb->triangles[i];
                ps_vec3 p=cv_triangle_point(ps_v3(0,0,0),gb.vertices[tri[0]],gb.vertices[tri[1]],gb.vertices[tri[2]],gb.normals[i]);
                double d=length3(p);if(d<distance){distance=d;wb=p;}
            }
            if(distance==0)return PS_NUMERIC;
            direction=cv_div(wb,distance);wa=ps_vscale(direction,radius/scale);
            upper=fmax(0,distance-radius/scale);double lo,hi;cv_interval(&gb,direction,&lo,&hi);lower=fmax(0,lo-radius/scale);
        } else {
            size_t nearest=0;double distance=ps_vdot(ga.vertices[0],normal);
            for(size_t i=1;i<ma->vertex_count;i++){double d=ps_vdot(ga.vertices[i],normal);if(d<distance){nearest=i;distance=d;}}
            wa=ga.vertices[nearest];wb=ps_vsub(wa,ps_vscale(normal,distance));direction=ps_vscale(normal,-1);
            upper=lower=fmax(0,distance);origin=point;
        }
        if(kind!=1)for(size_t i=0;i<ma->vertex_count;i++)coordinates=fmax(coordinates,cv_max(ga.vertices[i]));
        if(kind!=2)for(size_t i=0;i<mb->vertex_count;i++)coordinates=fmax(coordinates,cv_max(gb.vertices[i]));
        double error=cv_tol*(coordinates+angle_a*ra+angle_b*rb+1);
        if(!isfinite(upper)||!isfinite(lower)||!isfinite(error)||error>tol/8)return PS_NUMERIC;
        if(lower<=error) {
            bool overlapping;ps_contact discrete;
            result=cv_motion_initial(kind,&pa,ma,radius,kind==2?NULL:&pb,mb,point,normal,&discrete,&overlapping);
            if(result!=PS_OK)return result;
            if(overlapping){candidate.contact=discrete;candidate.contact.penetration_m=t?0:discrete.penetration_m;candidate.fraction=t;*hit=candidate;*touching=true;return PS_OK;}
        }
        if(upper<=tol && length3(direction)>0) {
            bool found;
            result=cv_contact(origin,scale,ps_vscale(ps_vadd(wa,wb),.5),direction,0,&candidate.contact,&found);
            if(result!=PS_OK)return result;
            candidate.fraction=t;*hit=candidate;*touching=true;return PS_OK;
        }
        double safe=fmax(0,lower-error);
        if(safe>bound*(1-t) || bound==0){*touching=false;return PS_OK;}
        if(!safe)return PS_LIMIT;
        double next=t+.8*(safe/bound);
        if(!isfinite(next)||next<=t)return PS_LIMIT;
        t=fmin(next,1);
    }
    return PS_LIMIT;
}
ps_result ps_sweep_convexes_motion(const ps_body *a,const ps_convex_mesh *ma,ps_rigid_motion da,
    const ps_body *b,const ps_convex_mesh *mb,ps_rigid_motion db,const ps_ccd_settings *settings,ps_sweep_hit *hit,bool *touching) {
    return cv_motion_query(0,a,ma,0,da,b,mb,db,ps_v3(0,0,0),ps_v3(0,0,0),settings,hit,touching);
}
ps_result ps_sweep_convex_plane_motion(const ps_body *body,const ps_convex_mesh *mesh,ps_rigid_motion motion,
    ps_vec3 point,ps_vec3 normal,const ps_ccd_settings *settings,ps_sweep_hit *hit,bool *touching) {
    double n=length3(normal);if(!finite3(normal)||fabs(n-1)>1e-8)return PS_INVALID;
    return cv_motion_query(2,body,mesh,0,motion,NULL,NULL,(ps_rigid_motion){0},point,cv_div(normal,n),settings,hit,touching);
}
ps_result ps_sweep_sphere_convex_motion(const ps_body *sphere,double radius,ps_rigid_motion ds,
    const ps_body *body,const ps_convex_mesh *mesh,ps_rigid_motion db,const ps_ccd_settings *settings,ps_sweep_hit *hit,bool *touching) {
    return cv_motion_query(1,sphere,NULL,radius,ds,body,mesh,db,ps_v3(0,0,0),ps_v3(0,0,0),settings,hit,touching);
}
