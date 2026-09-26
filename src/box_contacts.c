#include "physim/mechanics.h"
#include <float.h>
#include <math.h>

/* All geometry is relative to A and scaled by the largest half extent. */
typedef struct {
    ps_vec3 center, axis[3];
    double half[3];
} contact_box;
static const double tolerance = 64 * DBL_EPSILON;
static bool finite_vector(ps_vec3 v) { return isfinite(v.x) && isfinite(v.y) && isfinite(v.z); }
static double length(ps_vec3 v) { return hypot(hypot(v.x, v.y), v.z); }
static double clamp(double v, double half) { return fmax(-half, fmin(half, v)); }
static contact_box make_box(const ps_body *body, ps_vec3 size, ps_vec3 center, double scale) {
    ps_quat q = body->orientation;
    double norm = hypot(hypot(q.x, q.y), hypot(q.z, q.w));
    q.x /= norm;
    q.y /= norm;
    q.z /= norm;
    q.w /= norm;
    contact_box result = {0};
    result.center = center;
    result.half[0] = (size.x / scale) * .5;
    result.half[1] = (size.y / scale) * .5;
    result.half[2] = (size.z / scale) * .5;
    result.axis[0] = ps_quat_rotate(q, ps_v3(1, 0, 0));
    result.axis[1] = ps_quat_rotate(q, ps_v3(0, 1, 0));
    result.axis[2] = ps_quat_rotate(q, ps_v3(0, 0, 1));
    return result;
}
static double radius(const contact_box *box, ps_vec3 axis) {
    double value = 0;
    for (unsigned i = 0; i < 3; i++)
        value += box->half[i] * fabs(ps_vdot(axis, box->axis[i]));
    return value;
}
/* Return false only for a separating axis. Degenerate cross products add no test. */
static bool axis_test(const contact_box *a, const contact_box *b, ps_vec3 candidate, unsigned index,
                      double *minimum, unsigned *feature, ps_vec3 *normal) {
    double n = length(candidate);
    if (n <= 32 * DBL_EPSILON)
        return true;
    candidate = ps_v3(candidate.x / n, candidate.y / n, candidate.z / n);
    double distance = ps_vdot(b->center, candidate);
    double depth = radius(a, candidate) + radius(b, candidate) - fabs(distance);
    if (depth < -tolerance)
        return false;
    depth = fmax(0, depth);
    if (depth < *minimum - tolerance) {
        *minimum = depth;
        *feature = index;
        *normal = distance < 0 ? ps_vscale(candidate, -1) : candidate;
    }
    return true;
}
/* Sutherland-Hodgman clipping against one side of the reference face. */
static unsigned clip(const ps_vec3 *input, unsigned count, ps_vec3 *output, ps_vec3 center,
                     ps_vec3 normal, double limit) {
    unsigned n = 0;
    if (!count)
        return 0;
    ps_vec3 previous = input[count - 1];
    double before = ps_vdot(ps_vsub(previous, center), normal) - limit;
    if (fabs(before) <= tolerance)
        before = 0;
    for (unsigned i = 0; i < count; i++) {
        ps_vec3 current = input[i];
        double after = ps_vdot(ps_vsub(current, center), normal) - limit;
        if (fabs(after) <= tolerance)
            after = 0;
        if ((before <= 0) != (after <= 0)) {
            double t = before / (before - after);
            if (n >= 12)
                return 13;
            output[n++] = ps_vadd(previous, ps_vscale(ps_vsub(current, previous), t));
        }
        if (after <= 0) {
            if (n >= 12)
                return 13;
            output[n++] = current;
        }
        previous = current;
        before = after;
    }
    return n;
}
static bool add_contact(ps_contact_manifold *out, ps_vec3 point, ps_vec3 normal, double depth) {
    for (unsigned i = 0; i < out->count; i++)
        if (length(ps_vsub(point, out->points[i].point_m)) <= tolerance)
            return true;
    if (out->count >= PS_CONTACT_MAX_POINTS)
        return false;
    out->points[out->count++] = (ps_contact){point, normal, fmax(0, depth)};
    return true;
}
static bool face_contacts(const contact_box *reference, const contact_box *incident, unsigned face,
                          ps_vec3 outward, ps_vec3 normal, ps_contact_manifold *out) {
    unsigned incident_face = 0;
    for (unsigned i = 1; i < 3; i++)
        if (fabs(ps_vdot(outward, incident->axis[i])) >
            fabs(ps_vdot(outward, incident->axis[incident_face])))
            incident_face = i;
    double sign = ps_vdot(outward, incident->axis[incident_face]) > 0 ? -1 : 1;
    ps_vec3 center = ps_vadd(incident->center, ps_vscale(incident->axis[incident_face],
                                                         sign * incident->half[incident_face]));
    unsigned u = (incident_face + 1) % 3, v = (incident_face + 2) % 3;
    ps_vec3 du = ps_vscale(incident->axis[u], incident->half[u]);
    ps_vec3 dv = ps_vscale(incident->axis[v], incident->half[v]);
    ps_vec3 polygon[2][12];
    polygon[0][0] = ps_vsub(ps_vsub(center, du), dv);
    polygon[0][1] = ps_vsub(ps_vadd(center, du), dv);
    polygon[0][2] = ps_vadd(ps_vadd(center, du), dv);
    polygon[0][3] = ps_vadd(ps_vsub(center, du), dv);
    unsigned count = 4, buffer = 0;
    for (unsigned i = 0; i < 3; i++) {
        if (i == face)
            continue;
        for (int side = -1; side <= 1; side += 2) {
            count = clip(polygon[buffer], count, polygon[1 - buffer], reference->center,
                         ps_vscale(reference->axis[i], side), reference->half[i]);
            if (count > 12)
                return false;
            buffer = 1 - buffer;
        }
    }
    ps_vec3 surface = ps_vadd(reference->center, ps_vscale(outward, reference->half[face]));
    for (unsigned i = 0; i < count; i++) {
        ps_vec3 p = polygon[buffer][i];
        double depth = ps_vdot(ps_vsub(surface, p), outward);
        if (depth >= -tolerance &&
            !add_contact(out, ps_vadd(p, ps_vscale(outward, .5 * depth)), normal, depth))
            return false;
    }
    return out->count != 0;
}
static ps_vec3 edge_center(const contact_box *box, unsigned edge, ps_vec3 toward) {
    ps_vec3 p = box->center;
    for (unsigned i = 0; i < 3; i++)
        if (i != edge)
            p = ps_vadd(p,
                        ps_vscale(box->axis[i], ps_vdot(box->axis[i], toward) < 0 ? -box->half[i]
                                                                                  : box->half[i]));
    return p;
}
static void edge_contact(const contact_box *a, const contact_box *b, unsigned ia, unsigned ib,
                         ps_vec3 normal, double depth, ps_contact_manifold *out) {
    ps_vec3 pa = edge_center(a, ia, normal), pb = edge_center(b, ib, ps_vscale(normal, -1));
    ps_vec3 u = a->axis[ia], v = b->axis[ib], r = ps_vsub(pa, pb);
    double uv = ps_vdot(u, v), ur = ps_vdot(u, r), vr = ps_vdot(v, r);
    ps_vec3 cross = ps_vcross(u, v);
    double denominator = ps_vdot(cross, cross);
    double s = clamp((uv * vr - ur) / denominator, a->half[ia]);
    double t = uv * s + vr;
    if (t < -b->half[ib] || t > b->half[ib]) {
        t = clamp(t, b->half[ib]);
        s = clamp(uv * t - ur, a->half[ia]);
    }
    pa = ps_vadd(pa, ps_vscale(u, s));
    pb = ps_vadd(pb, ps_vscale(v, t));
    out->count = 1;
    out->points[0] = (ps_contact){ps_vscale(ps_vadd(pa, pb), .5), normal, depth};
}
ps_result ps_contacts_boxes(const ps_body *body_a, ps_vec3 size_a, const ps_body *body_b,
                            ps_vec3 size_b, ps_contact_manifold *out) {
    if (!out || body_a == body_b || ps_body_validate(body_a) != PS_OK ||
        ps_body_validate(body_b) != PS_OK || !finite_vector(size_a) || !finite_vector(size_b) ||
        size_a.x <= 0 || size_a.y <= 0 || size_a.z <= 0 || size_b.x <= 0 || size_b.y <= 0 ||
        size_b.z <= 0)
        return PS_INVALID;
    double scale = .5 * fmax(fmax(fmax(size_a.x, size_a.y), size_a.z),
                             fmax(fmax(size_b.x, size_b.y), size_b.z));
    ps_vec3 delta = ps_vsub(body_b->position_m, body_a->position_m);
    if (!isfinite(scale) || scale <= 0 || !finite_vector(delta))
        return PS_NUMERIC;
    delta = ps_v3(delta.x / scale, delta.y / scale, delta.z / scale);
    if (!finite_vector(delta))
        return PS_NUMERIC;
    contact_box a = make_box(body_a, size_a, ps_v3(0, 0, 0), scale);
    contact_box b = make_box(body_b, size_b, delta, scale);
    for (unsigned i = 0; i < 3; i++)
        if (a.half[i] <= 0 || b.half[i] <= 0)
            return PS_NUMERIC;
    double depth = DBL_MAX;
    unsigned feature = 0;
    ps_vec3 normal = {0};
    ps_contact_manifold result = {0};
    for (unsigned i = 0; i < 15; i++) {
        ps_vec3 axis = i < 3   ? a.axis[i]
                       : i < 6 ? b.axis[i - 3]
                               : ps_vcross(a.axis[(i - 6) / 3], b.axis[(i - 6) % 3]);
        if (!axis_test(&a, &b, axis, i, &depth, &feature, &normal)) {
            *out = result;
            return PS_OK;
        }
    }
    if (feature < 6) {
        bool ok = feature < 3
                      ? face_contacts(&a, &b, feature, normal, normal, &result)
                      : face_contacts(&b, &a, feature - 3, ps_vscale(normal, -1), normal, &result);
        if (!ok)
            return PS_NUMERIC;
    } else
        edge_contact(&a, &b, (feature - 6) / 3, (feature - 6) % 3, normal, depth, &result);
    for (unsigned i = 0; i < result.count; i++) {
        ps_contact *c = &result.points[i];
        c->point_m = ps_vadd(body_a->position_m, ps_vscale(c->point_m, scale));
        c->penetration_m *= scale;
        if (!finite_vector(c->point_m) || !finite_vector(c->normal) || !isfinite(c->penetration_m))
            return PS_NUMERIC;
    }
    *out = result;
    return PS_OK;
}
