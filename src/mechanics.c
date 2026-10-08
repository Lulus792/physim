#include "physim/mechanics.h"
#include "body_numeric.h"
#include <math.h>
#include <string.h>

static bool finite3(ps_vec3 v) { return isfinite(v.x) && isfinite(v.y) && isfinite(v.z); }
static double norm(ps_vec3 v) { return hypot(hypot(v.x, v.y), v.z); }
static ps_vec3 direction(ps_vec3 v, double length) {
    return ps_v3(v.x / length, v.y / length, v.z / length);
}
static bool positive3(ps_vec3 v) { return finite3(v) && v.x > 0 && v.y > 0 && v.z > 0; }
static bool zero3(ps_vec3 v) { return v.x == 0 && v.y == 0 && v.z == 0; }
static double quat_norm(ps_quat q) { return hypot(hypot(q.x, q.y), hypot(q.z, q.w)); }
ps_result ps_body_validate(const ps_body *b) {
    if (!b || !finite3(b->position_m) || !finite3(b->velocity_m_s) ||
        !finite3(b->angular_velocity_rad_s) || !isfinite(b->mass_kg) || b->mass_kg < 0 ||
        !isfinite(quat_norm(b->orientation)) || fabs(quat_norm(b->orientation) - 1) > 1e-8)
        return PS_INVALID;
    if (b->mass_kg == 0)
        return zero3(b->inertia_kg_m2) && zero3(b->velocity_m_s) && zero3(b->angular_velocity_rad_s)
                   ? PS_OK
                   : PS_INVALID;
    return positive3(b->inertia_kg_m2) ? PS_OK : PS_INVALID;
}
ps_result ps_body_sphere(double mass, double radius, ps_body *out) {
    if (!out || !isfinite(mass) || mass < 0 || !isfinite(radius) || radius <= 0)
        return PS_INVALID;
    ps_body b = {0};
    b.mass_kg = mass;
    b.orientation.w = 1;
    double inertia = mass ? body_inertia(mass, radius, 0, true) : 0;
    b.inertia_kg_m2 = ps_v3(inertia, inertia, inertia);
    if (ps_body_validate(&b) != PS_OK)
        return PS_NUMERIC;
    *out = b;
    return PS_OK;
}
ps_result ps_body_box(double mass, ps_vec3 size, ps_body *out) {
    if (!out || !isfinite(mass) || mass < 0 || !positive3(size))
        return PS_INVALID;
    ps_body b = {0};
    b.mass_kg = mass;
    b.orientation.w = 1;
    if (mass) {
        b.inertia_kg_m2 = ps_v3(body_inertia(mass, size.y, size.z, false),
                                body_inertia(mass, size.x, size.z, false),
                                body_inertia(mass, size.x, size.y, false));
    }
    if (ps_body_validate(&b) != PS_OK)
        return PS_NUMERIC;
    *out = b;
    return PS_OK;
}
static ps_vec3 inertia_world(const ps_body *b, ps_vec3 v, bool inverse) {
    if (!b || !b->mass_kg)
        return ps_v3(0, 0, 0);
    ps_quat q = b->orientation, conjugate = {-q.x, -q.y, -q.z, q.w};
    ps_vec3 local = ps_quat_rotate(conjugate, v), d = b->inertia_kg_m2;
    local = inverse ? ps_v3(local.x / d.x, local.y / d.y, local.z / d.z)
                    : ps_v3(local.x * d.x, local.y * d.y, local.z * d.z);
    return ps_quat_rotate(q, local);
}
static ps_vec3 point_velocity(const ps_body *b, ps_vec3 p) {
    return b ? ps_vadd(b->velocity_m_s,
                       ps_vcross(b->angular_velocity_rad_s, ps_vsub(p, b->position_m)))
             : ps_v3(0, 0, 0);
}
ps_result ps_body_point_velocity(const ps_body *b, ps_vec3 p, ps_vec3 *out) {
    if (ps_body_validate(b) != PS_OK || !finite3(p) || !out)
        return PS_INVALID;
    ps_vec3 v = point_velocity(b, p);
    if (!finite3(v))
        return PS_NUMERIC;
    *out = v;
    return PS_OK;
}
ps_result ps_body_force_torque(const ps_body *b, ps_vec3 f, ps_vec3 p, ps_vec3 *out) {
    if (ps_body_validate(b) != PS_OK || !finite3(p) || !finite3(f) || !out)
        return PS_INVALID;
    ps_vec3 torque = ps_vcross(ps_vsub(p, b->position_m), f);
    if (!finite3(torque))
        return PS_NUMERIC;
    *out = torque;
    return PS_OK;
}
ps_result ps_body_kinetic_energy(const ps_body *b, double *out) {
    if (ps_body_validate(b) != PS_OK || !out)
        return PS_INVALID;
    uint32_t sum[BODY_WORDS] = {0};
    double velocity[3] = {b->velocity_m_s.x, b->velocity_m_s.y, b->velocity_m_s.z};
    for (unsigned i = 0; i < 3; i++) {
        double factors[3] = {b->mass_kg, velocity[i], velocity[i]};
        body_add_product(sum, factors, 3, 1);
    }
    if (b->orientation.x == 0 && b->orientation.y == 0 && b->orientation.z == 0) {
        double angular[3] = {b->angular_velocity_rad_s.x, b->angular_velocity_rad_s.y,
                             b->angular_velocity_rad_s.z};
        double inertia[3] = {b->inertia_kg_m2.x, b->inertia_kg_m2.y, b->inertia_kg_m2.z};
        for (unsigned i = 0; i < 3; i++) {
            double factors[3] = {inertia[i], angular[i], angular[i]};
            body_add_product(sum, factors, 3, 1);
        }
    } else {
        double scale = fmax(fabs(b->angular_velocity_rad_s.x),
                            fmax(fabs(b->angular_velocity_rad_s.y), fabs(b->angular_velocity_rad_s.z)));
        if (scale) {
            ps_quat q = b->orientation;
            double qlength = quat_norm(q);
            q.x /= qlength; q.y /= qlength; q.z /= qlength; q.w /= qlength;
            ps_quat conjugate = {-q.x, -q.y, -q.z, q.w};
            ps_vec3 normalized = ps_v3(b->angular_velocity_rad_s.x / scale,
                                       b->angular_velocity_rad_s.y / scale,
                                       b->angular_velocity_rad_s.z / scale);
            ps_vec3 local = ps_quat_rotate(conjugate, normalized);
            double components[3] = {local.x, local.y, local.z};
            double inertia[3] = {b->inertia_kg_m2.x, b->inertia_kg_m2.y, b->inertia_kg_m2.z};
            for (unsigned i = 0; i < 3; i++) {
                double factors[5] = {inertia[i], scale, scale, components[i], components[i]};
                body_add_product(sum, factors, 5, 1);
            }
        }
    }
    double energy = body_value(sum, 2);
    if (!isfinite(energy))
        return PS_NUMERIC;
    *out = energy;
    return PS_OK;
}
static void impulse(ps_body *b, ps_vec3 j, ps_vec3 p) {
    if (!b || !b->mass_kg)
        return;
    b->velocity_m_s = ps_vadd(b->velocity_m_s, ps_vscale(j, 1 / b->mass_kg));
    b->angular_velocity_rad_s = ps_vadd(
        b->angular_velocity_rad_s, inertia_world(b, ps_vcross(ps_vsub(p, b->position_m), j), true));
}
ps_result ps_body_apply_impulse(ps_body *b, ps_vec3 j, ps_vec3 p) {
    if (ps_body_validate(b) != PS_OK || !finite3(j) || !finite3(p))
        return PS_INVALID;
    ps_body candidate = *b;
    impulse(&candidate, j, p);
    if (ps_body_validate(&candidate) != PS_OK)
        return PS_NUMERIC;
    *b = candidate;
    return PS_OK;
}
ps_result ps_body_step(ps_body *b, ps_vec3 force, ps_vec3 torque, double dt) {
    if (ps_body_validate(b) != PS_OK || !finite3(force) || !finite3(torque) || !isfinite(dt) ||
        dt <= 0)
        return PS_INVALID;
    if (!b->mass_kg)
        return PS_OK;
    ps_body next = *b;
    next.velocity_m_s = ps_vadd(b->velocity_m_s, ps_vscale(force, dt / b->mass_kg));
    next.position_m = ps_vadd(b->position_m, ps_vscale(next.velocity_m_s, dt));
    ps_vec3 omega = b->angular_velocity_rad_s;
    ps_vec3 gyroscopic = ps_vcross(omega, inertia_world(b, omega, false));
    ps_vec3 acceleration = inertia_world(b, ps_vsub(torque, gyroscopic), true);
    omega = ps_vadd(omega, ps_vscale(acceleration, dt));
    next.angular_velocity_rad_s = omega;
    double speed = norm(omega), angle = speed * dt;
    if (!isfinite(angle))
        return PS_NUMERIC;
    if (speed > 0) {
        ps_vec3 v = ps_vscale(omega, sin(angle / 2) / speed);
        double w = cos(angle / 2);
        ps_quat q = b->orientation;
        ps_vec3 qv = ps_v3(q.x, q.y, q.z);
        ps_vec3 rotated = ps_vadd(ps_vadd(ps_vscale(qv, w), ps_vscale(v, q.w)), ps_vcross(v, qv));
        next.orientation = (ps_quat){rotated.x, rotated.y, rotated.z, w * q.w - ps_vdot(v, qv)};
        double length = quat_norm(next.orientation);
        next.orientation.x /= length;
        next.orientation.y /= length;
        next.orientation.z /= length;
        next.orientation.w /= length;
    }
    if (ps_body_validate(&next) != PS_OK)
        return PS_NUMERIC;
    *b = next;
    return PS_OK;
}
static bool unit_normal(ps_vec3 n) { return finite3(n) && fabs(norm(n) - 1) <= 1e-8; }
ps_result ps_contact_spheres(const ps_body *a, double ra, const ps_body *b, double rb,
                             ps_contact *out, bool *touching) {
    if (a == b || ps_body_validate(a) != PS_OK || ps_body_validate(b) != PS_OK || !out ||
        !touching || !isfinite(ra) || !isfinite(rb) || ra <= 0 || rb <= 0)
        return PS_INVALID;
    ps_vec3 delta = ps_vsub(b->position_m, a->position_m);
    double distance = norm(delta), radii = ra + rb;
    if (!isfinite(distance) || !isfinite(radii))
        return PS_NUMERIC;
    if (distance > radii) {
        *touching = false;
        return PS_OK;
    }
    ps_vec3 n = distance ? direction(delta, distance) : ps_v3(1, 0, 0);
    ps_vec3 point_a = ps_vadd(a->position_m, ps_vscale(n, ra));
    ps_vec3 point_b = ps_vsub(b->position_m, ps_vscale(n, rb));
    ps_contact contact = {ps_vadd(ps_vscale(point_a, .5), ps_vscale(point_b, .5)), n,
                          radii - distance};
    if (!finite3(contact.point_m) || !unit_normal(n))
        return PS_NUMERIC;
    *out = contact;
    *touching = true;
    return PS_OK;
}
ps_result ps_contact_sphere_plane(const ps_body *a, double radius, ps_vec3 p, ps_vec3 n,
                                  ps_contact *out, bool *touching) {
    if (ps_body_validate(a) != PS_OK || !isfinite(radius) || radius <= 0 || !finite3(p) ||
        !unit_normal(n) || !out || !touching)
        return PS_INVALID;
    double distance = ps_vdot(ps_vsub(a->position_m, p), n);
    if (!isfinite(distance) || !isfinite(radius - distance))
        return PS_NUMERIC;
    if (distance > radius) {
        *touching = false;
        return PS_OK;
    }
    ps_contact contact = {ps_vsub(a->position_m, ps_vscale(n, radius)), ps_vscale(n, -1),
                          radius - distance};
    if (!finite3(contact.point_m))
        return PS_NUMERIC;
    *out = contact;
    *touching = true;
    return PS_OK;
}
ps_result ps_contact_sphere_box(const ps_body *a, double radius, const ps_body *b, ps_vec3 size,
                                ps_contact *out, bool *touching) {
    if (a == b || ps_body_validate(a) != PS_OK || ps_body_validate(b) != PS_OK || !out ||
        !touching || !isfinite(radius) || radius <= 0 || !positive3(size))
        return PS_INVALID;
    ps_quat q = b->orientation, inverse = {-q.x, -q.y, -q.z, q.w};
    ps_vec3 local = ps_quat_rotate(inverse, ps_vsub(a->position_m, b->position_m));
    ps_vec3 half = ps_vscale(size, .5);
    if (!finite3(local) || !positive3(half))
        return PS_NUMERIC;
    ps_vec3 closest =
        ps_v3(fmax(-half.x, fmin(half.x, local.x)), fmax(-half.y, fmin(half.y, local.y)),
              fmax(-half.z, fmin(half.z, local.z)));
    ps_vec3 delta = ps_vsub(local, closest);
    double distance = norm(delta);
    if (!isfinite(distance))
        return PS_NUMERIC;
    if (distance > radius) {
        *touching = false;
        return PS_OK;
    }
    ps_vec3 outward;
    double depth;
    if (distance > 0) {
        outward = direction(delta, distance);
        depth = radius - distance;
    } else {
        double gaps[] = {half.x - fabs(local.x), half.y - fabs(local.y), half.z - fabs(local.z)};
        int axis = gaps[1] < gaps[0] ? 1 : 0;
        if (gaps[2] < gaps[axis])
            axis = 2;
        outward = ps_v3(0, 0, 0);
        if (axis == 0) {
            outward.x = local.x < 0 ? -1 : 1;
            closest.x = outward.x * half.x;
        }
        if (axis == 1) {
            outward.y = local.y < 0 ? -1 : 1;
            closest.y = outward.y * half.y;
        }
        if (axis == 2) {
            outward.z = local.z < 0 ? -1 : 1;
            closest.z = outward.z * half.z;
        }
        depth = radius + gaps[axis];
    }
    ps_contact c = {ps_vadd(b->position_m, ps_quat_rotate(q, closest)),
                    ps_vscale(ps_quat_rotate(q, outward), -1), depth};
    if (!finite3(c.point_m) || !unit_normal(c.normal) || !isfinite(depth))
        return PS_NUMERIC;
    *out = c;
    *touching = true;
    return PS_OK;
}
ps_result ps_contacts_box_plane(const ps_body *b, ps_vec3 size, ps_vec3 p, ps_vec3 n,
                                ps_contact_manifold *out) {
    if (ps_body_validate(b) != PS_OK || !positive3(size) || !finite3(p) || !unit_normal(n) || !out)
        return PS_INVALID;
    ps_vec3 half = ps_vscale(size, .5);
    if (!positive3(half))
        return PS_NUMERIC;
    ps_contact_manifold result = {0};
    for (unsigned i = 0; i < 8; i++) {
        ps_vec3 local =
            ps_v3(i & 1 ? half.x : -half.x, i & 2 ? half.y : -half.y, i & 4 ? half.z : -half.z);
        ps_vec3 vertex = ps_vadd(b->position_m, ps_quat_rotate(b->orientation, local));
        double distance = ps_vdot(ps_vsub(vertex, p), n);
        if (!finite3(vertex) || !isfinite(distance))
            return PS_NUMERIC;
        if (distance <= 0)
            result.points[result.count++] = (ps_contact){vertex, ps_vscale(n, -1), -distance};
    }
    *out = result;
    return PS_OK;
}
static double inverse_mass(const ps_body *b) { return b && b->mass_kg ? 1 / b->mass_kg : 0; }
static double effective_mass(const ps_body *b, ps_vec3 p, ps_vec3 n) {
    if (!b || !b->mass_kg)
        return 0;
    ps_vec3 r = ps_vsub(p, b->position_m);
    return inverse_mass(b) + ps_vdot(ps_vcross(inertia_world(b, ps_vcross(r, n), true), r), n);
}
ps_result ps_contact_resolve(ps_body *a, ps_body *b, const ps_contact *contact, double e, double mu,
                             ps_vec3 *out) {
    if (a == b || ps_body_validate(a) != PS_OK || (b && ps_body_validate(b) != PS_OK) || !contact ||
        !finite3(contact->point_m) || !unit_normal(contact->normal) ||
        !isfinite(contact->penetration_m) || contact->penetration_m < 0 || !isfinite(e) || e < 0 ||
        e > 1 || !isfinite(mu) || mu < 0)
        return PS_INVALID;
    ps_body ca = *a, cb = {0};
    if (b)
        cb = *b;
    ps_body *pb = b ? &cb : NULL;
    ps_vec3 p = contact->point_m, n = contact->normal, total = {0};
    double ia = inverse_mass(a), ib = inverse_mass(b), mass_sum = ia + ib;
    if (!isfinite(mass_sum))
        return PS_NUMERIC;
    if (mass_sum) {
        ps_vec3 relative = ps_vsub(point_velocity(pb, p), point_velocity(&ca, p));
        double closing = ps_vdot(relative, n);
        if (!finite3(relative) || !isfinite(closing))
            return PS_NUMERIC;
        if (closing < 0) {
            double denominator = effective_mass(&ca, p, n) + effective_mass(pb, p, n);
            if (!isfinite(denominator) || denominator <= 0)
                return PS_NUMERIC;
            double normal_impulse = -(1 + e) * closing / denominator;
            if (!isfinite(normal_impulse))
                return PS_NUMERIC;
            total = ps_vscale(n, normal_impulse);
            impulse(&ca, ps_vscale(total, -1), p);
            impulse(pb, total, p);
            relative = ps_vsub(point_velocity(pb, p), point_velocity(&ca, p));
            ps_vec3 tangent = ps_vsub(relative, ps_vscale(n, ps_vdot(relative, n)));
            double speed = norm(tangent);
            if (!isfinite(speed))
                return PS_NUMERIC;
            if (speed > 0 && mu > 0) {
                tangent = direction(tangent, speed);
                denominator = effective_mass(&ca, p, tangent) + effective_mass(pb, p, tangent);
                if (!isfinite(denominator) || denominator <= 0)
                    return PS_NUMERIC;
                double jt = -fmin(speed / denominator, mu * normal_impulse);
                ps_vec3 friction = ps_vscale(tangent, jt);
                impulse(&ca, ps_vscale(friction, -1), p);
                impulse(pb, friction, p);
                total = ps_vadd(total, friction);
            }
        }
        ca.position_m =
            ps_vsub(ca.position_m, ps_vscale(n, contact->penetration_m * (ia / mass_sum)));
        if (pb)
            cb.position_m =
                ps_vadd(cb.position_m, ps_vscale(n, contact->penetration_m * (ib / mass_sum)));
    }
    if (ps_body_validate(&ca) != PS_OK || (b && ps_body_validate(&cb) != PS_OK) || !finite3(total))
        return PS_NUMERIC;
    *a = ca;
    if (b)
        *b = cb;
    if (out)
        *out = ps_vscale(total, -1);
    return PS_OK;
}
ps_result ps_distance_joint_validate(const ps_distance_joint *joint) {
    return joint && finite3(joint->anchor_a_m) && finite3(joint->anchor_b_m) &&
                   isfinite(joint->length_m) && joint->length_m > 0 &&
                   isfinite(joint->stabilization) && joint->stabilization >= 0 &&
                   joint->stabilization <= 1
               ? PS_OK : PS_INVALID;
}
ps_result ps_distance_joint_resolve(ps_body *a, ps_body *b, const ps_distance_joint *joint,
                                    double dt, ps_distance_joint_solution *out) {
    if (!a || a == b || ps_body_validate(a) != PS_OK || (b && ps_body_validate(b) != PS_OK) ||
        ps_distance_joint_validate(joint) != PS_OK || !isfinite(dt) || dt <= 0)
        return PS_INVALID;
    ps_body ca = *a, cb = {0};
    if (b)
        cb = *b;
    ps_body *pb = b ? &cb : NULL;
    ps_vec3 pa = ps_vadd(a->position_m, ps_quat_rotate(a->orientation, joint->anchor_a_m));
    ps_vec3 p_b = b ? ps_vadd(b->position_m, ps_quat_rotate(b->orientation, joint->anchor_b_m))
                    : joint->anchor_b_m;
    ps_vec3 separation = ps_vsub(p_b, pa);
    double length = norm(separation);
    if (!finite3(pa) || !finite3(p_b) || !isfinite(length))
        return PS_NUMERIC;
    if (!length)
        return PS_SINGULAR;
    ps_vec3 n = direction(separation, length);
    ps_distance_joint_solution result = {0};
    result.length_error_m = length - joint->length_m;
    double target = -joint->stabilization * result.length_error_m / dt;
    double speed = ps_vdot(ps_vsub(point_velocity(pb, p_b), point_velocity(&ca, pa)), n);
    double denominator = effective_mass(&ca, pa, n) + effective_mass(pb, p_b, n);
    if (!isfinite(target) || !isfinite(speed) || !isfinite(denominator) || denominator < 0)
        return PS_NUMERIC;
    if (a->mass_kg || (b && b->mass_kg)) {
        if (denominator <= 0)
            return PS_NUMERIC;
        result.impulse_on_a_ns = ps_vscale(n, (speed - target) / denominator);
        if (!finite3(result.impulse_on_a_ns))
            return PS_NUMERIC;
        impulse(&ca, result.impulse_on_a_ns, pa);
        impulse(pb, ps_vscale(result.impulse_on_a_ns, -1), p_b);
    }
    result.velocity_error_m_s =
        fabs(ps_vdot(ps_vsub(point_velocity(pb, p_b), point_velocity(&ca, pa)), n) - target);
    if (ps_body_validate(&ca) != PS_OK || (b && ps_body_validate(&cb) != PS_OK) ||
        !isfinite(result.velocity_error_m_s))
        return PS_NUMERIC;
    *a = ca;
    if (b)
        *b = cb;
    if (out)
        *out = result;
    return PS_OK;
}
const ps_contact_solver PS_CONTACT_SOLVER_DEFAULT = {40, 0, .5, .5, 1e-5, .8};
typedef struct {
    ps_vec3 t1, t2;
    double normal_mass, target, normal, tangent1, tangent2;
    double scale, tangent_eigenvalue;
} contact_iteration;
static ps_vec3 impulse_response(const ps_body *b, ps_vec3 point, ps_vec3 axis) {
    if (!b || !b->mass_kg)
        return ps_v3(0, 0, 0);
    ps_vec3 r = ps_vsub(point, b->position_m);
    return ps_vadd(ps_vscale(axis, inverse_mass(b)),
                   ps_vcross(inertia_world(b, ps_vcross(r, axis), true), r));
}
static ps_vec3 pair_response(const ps_body *a, const ps_body *b, ps_vec3 p, ps_vec3 axis) {
    return ps_vadd(impulse_response(a, p, axis), impulse_response(b, p, axis));
}
static ps_result prepare_contact(const ps_body *a, const ps_body *b, const ps_contact *p,
                                 const ps_contact_solver *settings, contact_iteration *s) {
    ps_vec3 n = p->normal;
    ps_vec3 axis = fabs(n.x) < .577   ? ps_v3(1, 0, 0)
                   : fabs(n.y) < .577 ? ps_v3(0, 1, 0)
                                      : ps_v3(0, 0, 1);
    s->t1 = ps_vcross(n, axis);
    s->t1 = direction(s->t1, norm(s->t1));
    s->t2 = ps_vcross(n, s->t1);
    s->normal_mass = ps_vdot(n, pair_response(a, b, p->point_m, n));
    ps_vec3 response1 = pair_response(a, b, p->point_m, s->t1);
    ps_vec3 response2 = pair_response(a, b, p->point_m, s->t2);
    double k11 = ps_vdot(s->t1, response1);
    double k12 = ps_vdot(s->t2, response1);
    double k22 = ps_vdot(s->t2, response2);
    s->scale = fmax(k11, k22);
    if (!isfinite(s->normal_mass) || s->normal_mass <= 0 || !isfinite(s->scale) || s->scale <= 0)
        return PS_NUMERIC;
    k11 /= s->scale;
    k12 /= s->scale;
    k22 /= s->scale;
    double determinant = k11 * k22 - k12 * k12;
    if (!isfinite(determinant) || determinant <= 0)
        return PS_NUMERIC;
    s->tangent_eigenvalue = .5 * (k11 + k22 + hypot(k11 - k22, 2 * k12));
    double initial =
        ps_vdot(ps_vsub(point_velocity(b, p->point_m), point_velocity(a, p->point_m)), n);
    if (!isfinite(initial))
        return PS_NUMERIC;
    s->target = initial < -settings->bounce_threshold_m_s ? -settings->restitution * initial : 0;
    return PS_OK;
}
static ps_result iterate_contact(ps_body *a, ps_body *b, const ps_contact *p,
                                 const ps_contact_solver *settings, contact_iteration *s) {
    ps_vec3 velocity = ps_vsub(point_velocity(b, p->point_m), point_velocity(a, p->point_m));
    double next_normal =
        fmax(0, s->normal + (s->target - ps_vdot(velocity, p->normal)) / s->normal_mass);
    if (!finite3(velocity) || !isfinite(next_normal))
        return PS_NUMERIC;
    ps_vec3 normal_delta = ps_vscale(p->normal, next_normal - s->normal);
    impulse(a, ps_vscale(normal_delta, -1), p->point_m);
    impulse(b, normal_delta, p->point_m);
    s->normal = next_normal;
    velocity = ps_vsub(point_velocity(b, p->point_m), point_velocity(a, p->point_m));
    double v1 = ps_vdot(velocity, s->t1) / s->scale, v2 = ps_vdot(velocity, s->t2) / s->scale;
    /* Projected gradient with 1/lambda_max step. Radially clamping an
     * inverse-mass block solve would produce incorrect sliding directions
     * for anisotropic tangential mass. This fixed point opposes slip. */
    double next1 = s->tangent1 - v1 / s->tangent_eigenvalue;
    double next2 = s->tangent2 - v2 / s->tangent_eigenvalue;
    double length = hypot(next1, next2), limit = settings->friction * s->normal;
    if (!isfinite(length))
        return PS_NUMERIC;
    if (length > limit) {
        next1 *= limit / length;
        next2 *= limit / length;
    }
    ps_vec3 tangent_delta =
        ps_vadd(ps_vscale(s->t1, next1 - s->tangent1), ps_vscale(s->t2, next2 - s->tangent2));
    impulse(a, ps_vscale(tangent_delta, -1), p->point_m);
    impulse(b, tangent_delta, p->point_m);
    s->tangent1 = next1;
    s->tangent2 = next2;
    return PS_OK;
}
static ps_result report_contact(const ps_body *a, const ps_body *b, const ps_contact *p,
                                const contact_iteration *s, ps_vec3 *out, double *residual) {
    *out = ps_vscale(ps_vadd(ps_vscale(p->normal, s->normal),
                             ps_vadd(ps_vscale(s->t1, s->tangent1), ps_vscale(s->t2, s->tangent2))),
                     -1);
    double velocity =
        ps_vdot(ps_vsub(point_velocity(b, p->point_m), point_velocity(a, p->point_m)), p->normal);
    double error = s->normal > 0 ? fabs(velocity - s->target) : fmax(0, s->target - velocity);
    if (!isfinite(error) || !finite3(*out))
        return PS_NUMERIC;
    *residual = fmax(*residual, error);
    return PS_OK;
}
static ps_result project_contact(ps_body *a, ps_body *b, ps_vec3 original_a, ps_vec3 original_b,
                                 const ps_contact *p, const ps_contact_solver *settings) {
    double ia = inverse_mass(a), ib = inverse_mass(b), mass_sum = ia + ib;
    ps_vec3 shift_a = ps_vsub(a->position_m, original_a);
    ps_vec3 shift_b = b ? ps_vsub(b->position_m, original_b) : ps_v3(0, 0, 0);
    double applied = ps_vdot(ps_vsub(shift_b, shift_a), p->normal);
    double target =
        settings->correction_fraction * fmax(0, p->penetration_m - settings->penetration_slop_m);
    if (!isfinite(applied))
        return PS_NUMERIC;
    double remaining = fmax(0, target - applied);
    a->position_m = ps_vsub(a->position_m, ps_vscale(p->normal, remaining * (ia / mass_sum)));
    if (b)
        b->position_m = ps_vadd(b->position_m, ps_vscale(p->normal, remaining * (ib / mass_sum)));
    return PS_OK;
}
static bool solver_valid(const ps_contact_solver *settings) {
    return !(!settings || !settings->iterations || settings->iterations > 256 ||
             !isfinite(settings->restitution) || settings->restitution < 0 ||
             settings->restitution > 1 || !isfinite(settings->friction) || settings->friction < 0 ||
             !isfinite(settings->bounce_threshold_m_s) || settings->bounce_threshold_m_s < 0 ||
             !isfinite(settings->penetration_slop_m) || settings->penetration_slop_m < 0 ||
             !isfinite(settings->correction_fraction) || settings->correction_fraction < 0 ||
             settings->correction_fraction > 1);
}
ps_result ps_contacts_resolve(ps_body *a, ps_body *b, const ps_contact_manifold *contacts,
                              const ps_contact_solver *settings, ps_contact_solution *out) {
    if (a == b || ps_body_validate(a) != PS_OK || (b && ps_body_validate(b) != PS_OK) ||
        !contacts || contacts->count > PS_CONTACT_MAX_POINTS || !solver_valid(settings))
        return PS_INVALID;
    for (uint32_t i = 0; i < contacts->count; i++) {
        const ps_contact *p = &contacts->points[i];
        if (!finite3(p->point_m) || !unit_normal(p->normal) || !isfinite(p->penetration_m) ||
            p->penetration_m < 0)
            return PS_INVALID;
    }
    ps_body ca = *a, cb = {0};
    if (b)
        cb = *b;
    ps_body *pb = b ? &cb : NULL;
    ps_contact_solution result = {0};
    result.count = contacts->count;
    double ia = inverse_mass(a), ib = inverse_mass(b), mass_sum = ia + ib;
    if (!isfinite(mass_sum))
        return PS_NUMERIC;
    contact_iteration state[PS_CONTACT_MAX_POINTS] = {0};
    if (mass_sum) {
        for (uint32_t i = 0; i < contacts->count; i++) {
            const ps_contact *p = &contacts->points[i];
            contact_iteration *s = &state[i];
            ps_result r = prepare_contact(&ca, pb, p, settings, s);
            if (r != PS_OK)
                return r;
        }
        for (uint32_t iteration = 0; iteration < settings->iterations; iteration++) {
            for (uint32_t i = 0; i < contacts->count; i++) {
                const ps_contact *p = &contacts->points[i];
                contact_iteration *s = &state[i];
                ps_result r = iterate_contact(&ca, pb, p, settings, s);
                if (r != PS_OK)
                    return r;
            }
        }
        for (uint32_t i = 0; i < contacts->count; i++) {
            const ps_contact *p = &contacts->points[i];
            const contact_iteration *s = &state[i];
            ps_result r = report_contact(&ca, pb, p, s, &result.impulse_on_a_ns[i],
                                         &result.max_normal_error_m_s);
            if (r != PS_OK)
                return r;
        }
        /* Project translation once per constraint, accounting for shifts already made. */
        for (uint32_t i = 0; i < contacts->count; i++) {
            const ps_contact *p = &contacts->points[i];
            ps_result r = project_contact(&ca, pb, a->position_m,
                                          b ? b->position_m : ps_v3(0, 0, 0), p, settings);
            if (r != PS_OK)
                return r;
        }
    }
    if (ps_body_validate(&ca) != PS_OK || (b && ps_body_validate(&cb) != PS_OK))
        return PS_NUMERIC;
    *a = ca;
    if (b)
        *b = cb;
    if (out)
        *out = result;
    return PS_OK;
}
static ps_result resolve_graph(ps_body *bodies, size_t body_count,
                               const ps_contact_constraint *constraints, size_t count,
                               const ps_contact_solver *settings, ps_contact_graph_solution *out,
                               const ps_distance_constraint *joints, size_t joint_count, double dt,
                               ps_constraint_graph_solution *mixed_out,const ps_vec3 *initial) {
    if ((!bodies && body_count) || (!constraints && count) || !solver_valid(settings) ||
        (!joints && joint_count) || !isfinite(dt) || dt <= 0)
        return PS_INVALID;
    if (body_count > PS_CONTACT_GRAPH_MAX_BODIES || count > PS_CONTACT_GRAPH_MAX_CONTACTS ||
        joint_count > PS_CONSTRAINT_GRAPH_MAX_JOINTS)
        return PS_LIMIT;
    for (size_t i = 0; i < body_count; i++)
        if (ps_body_validate(&bodies[i]) != PS_OK)
            return PS_INVALID;
    for (size_t i = 0; i < count; i++) {
        const ps_contact_constraint *c = &constraints[i];
        const ps_contact *p = &c->contact;
        if (c->a >= body_count || (c->b != PS_CONTACT_WORLD && c->b >= body_count) ||
            c->a == c->b || !finite3(p->point_m) || !unit_normal(p->normal) ||
            !isfinite(p->penetration_m) || p->penetration_m < 0)
            return PS_INVALID;
        if(initial && !finite3(initial[i]))return PS_INVALID;
    }
    for (size_t i = 0; i < joint_count; i++)
        if (joints[i].a >= body_count ||
            (joints[i].b != PS_CONTACT_WORLD && joints[i].b >= body_count) ||
            joints[i].a == joints[i].b)
            return PS_INVALID;
    ps_vec3 joint_impulses[PS_CONSTRAINT_GRAPH_MAX_JOINTS] = {0};
    double joint_velocity_error = 0, joint_length_error = 0;
    ps_body working[PS_CONTACT_GRAPH_MAX_BODIES];
    if (body_count)
        memcpy(working, bodies, body_count * sizeof *working);
    contact_iteration state[PS_CONTACT_GRAPH_MAX_CONTACTS] = {0};
    ps_contact_graph_solution result = {0};
    result.count = (uint32_t)count;
    for (size_t i = 0; i < count; i++) {
        const ps_contact_constraint *c = &constraints[i];
        ps_body *a = &working[c->a], *b = c->b == PS_CONTACT_WORLD ? NULL : &working[c->b];
        double mass = inverse_mass(a) + inverse_mass(b);
        if (!isfinite(mass))
            return PS_NUMERIC;
        if (mass) {
            ps_result r = prepare_contact(a, b, &c->contact, settings, &state[i]);
            if (r != PS_OK)
                return r;
        }
    }
    /* All restitution targets above precede any warm velocity update. */
    if(initial)for(size_t i=0;i<count;i++) {
        if(!state[i].normal_mass)continue;
        const ps_contact_constraint *c=&constraints[i];contact_iteration *s=&state[i];
        s->normal=fmax(0,-ps_vdot(initial[i],c->contact.normal));
        s->tangent1=-ps_vdot(initial[i],s->t1);s->tangent2=-ps_vdot(initial[i],s->t2);
        double length=hypot(s->tangent1,s->tangent2),limit=settings->friction*s->normal;
        if(!isfinite(s->normal) || !isfinite(length) || !isfinite(limit))return PS_NUMERIC;
        if(length>limit){s->tangent1*=limit/length;s->tangent2*=limit/length;}
        ps_vec3 applied=ps_vadd(ps_vscale(c->contact.normal,s->normal),
            ps_vadd(ps_vscale(s->t1,s->tangent1),ps_vscale(s->t2,s->tangent2)));
        if(!finite3(applied))return PS_NUMERIC;
        ps_body *a=&working[c->a],*b=c->b==PS_CONTACT_WORLD?NULL:&working[c->b];
        impulse(a,ps_vscale(applied,-1),c->contact.point_m);impulse(b,applied,c->contact.point_m);
        if(ps_body_validate(a)!=PS_OK || (b && ps_body_validate(b)!=PS_OK))return PS_NUMERIC;
    }
    for (uint32_t iteration = 0; iteration < settings->iterations; iteration++) {
        for (size_t i = 0; i < count; i++) {
            const ps_contact_constraint *c = &constraints[i];
            if (!state[i].normal_mass)
                continue;
            ps_body *a = &working[c->a], *b = c->b == PS_CONTACT_WORLD ? NULL : &working[c->b];
            ps_result r = iterate_contact(a, b, &c->contact, settings, &state[i]);
            if (r != PS_OK)
                return r;
        }
        for (size_t i = 0; i < joint_count; i++) {
            const ps_distance_constraint *j = &joints[i];
            ps_body *a = &working[j->a], *b = j->b == PS_CONTACT_WORLD ? NULL : &working[j->b];
            ps_distance_joint_solution step;
            ps_result r = ps_distance_joint_resolve(a, b, &j->joint, dt, &step);
            if (r != PS_OK)
                return r;
            joint_impulses[i] = ps_vadd(joint_impulses[i], step.impulse_on_a_ns);
            if (!finite3(joint_impulses[i]))
                return PS_NUMERIC;
        }
    }
    /* Evaluate all joints against the same final velocities, not their individual
       last
     * solve: later constraints can invalidate earlier constraints. */
    for (size_t i = 0; i < joint_count; i++) {
        const ps_distance_constraint *j = &joints[i];
        const ps_body *a = &working[j->a], *b = j->b == PS_CONTACT_WORLD ? NULL : &working[j->b];
        ps_vec3 pa = ps_vadd(a->position_m, ps_quat_rotate(a->orientation, j->joint.anchor_a_m));
        ps_vec3 pb = b ? ps_vadd(b->position_m, ps_quat_rotate(b->orientation, j->joint.anchor_b_m))
                       : j->joint.anchor_b_m;
        ps_vec3 delta = ps_vsub(pb, pa);
        double length = norm(delta);
        double target = -j->joint.stabilization * (length - j->joint.length_m) / dt;
        double error = fabs(ps_vdot(ps_vsub(point_velocity(b, pb), point_velocity(a, pa)),
                                    direction(delta, length)) -
                            target);
        if (!isfinite(error))
            return PS_NUMERIC;
        joint_velocity_error = fmax(joint_velocity_error, error);
    }
    for (size_t i = 0; i < count; i++) {
        const ps_contact_constraint *c = &constraints[i];
        if (!state[i].normal_mass)
            continue;
        ps_body *a = &working[c->a], *b = c->b == PS_CONTACT_WORLD ? NULL : &working[c->b];
        ps_result r = report_contact(a, b, &c->contact, &state[i], &result.impulse_on_a_ns[i],
                                     &result.max_normal_error_m_s);
        if (r != PS_OK)
            return r;
    }
    for (uint32_t iteration = 0; iteration < settings->iterations; iteration++)
        for (size_t i = 0; i < count; i++) {
            const ps_contact_constraint *c = &constraints[i];
            if (!state[i].normal_mass)
                continue;
            ps_body *a = &working[c->a], *b = c->b == PS_CONTACT_WORLD ? NULL : &working[c->b];
            ps_vec3 original_b = b ? bodies[c->b].position_m : ps_v3(0, 0, 0);
            ps_result r =
                project_contact(a, b, bodies[c->a].position_m, original_b, &c->contact, settings);
            if (r != PS_OK)
                return r;
        }
    for (size_t i = 0; i < count; i++) {
        const ps_contact_constraint *c = &constraints[i];
        ps_vec3 shift_a = ps_vsub(working[c->a].position_m, bodies[c->a].position_m);
        ps_vec3 shift_b = c->b == PS_CONTACT_WORLD
                              ? ps_v3(0, 0, 0)
                              : ps_vsub(working[c->b].position_m, bodies[c->b].position_m);
        double applied = ps_vdot(ps_vsub(shift_b, shift_a), c->contact.normal);
        double target = settings->correction_fraction *
                        fmax(0, c->contact.penetration_m - settings->penetration_slop_m);
        if (!isfinite(applied))
            return PS_NUMERIC;
        result.max_projection_error_m =
            fmax(result.max_projection_error_m, fmax(0, target - applied));
    }
    for (size_t i = 0; i < joint_count; i++) {
        const ps_distance_constraint *j = &joints[i];
        const ps_body *a = &working[j->a], *b = j->b == PS_CONTACT_WORLD ? NULL : &working[j->b];
        ps_vec3 pa = ps_vadd(a->position_m, ps_quat_rotate(a->orientation, j->joint.anchor_a_m));
        ps_vec3 pb = b ? ps_vadd(b->position_m, ps_quat_rotate(b->orientation, j->joint.anchor_b_m))
                       : j->joint.anchor_b_m;
        double error = fabs(norm(ps_vsub(pb, pa)) - j->joint.length_m);
        if (!finite3(pa) || !finite3(pb) || !isfinite(error))
            return PS_NUMERIC;
        joint_length_error = fmax(joint_length_error, error);
    }
    for (size_t i = 0; i < body_count; i++)
        if (ps_body_validate(&working[i]) != PS_OK)
            return PS_NUMERIC;
    if (body_count)
        memcpy(bodies, working, body_count * sizeof *bodies);
    if (out)
        *out = result;
    if (mixed_out) {
        mixed_out->contacts = result;
        mixed_out->joint_count = (uint32_t)joint_count;
        memcpy(mixed_out->joint_impulse_on_a_ns, joint_impulses, sizeof joint_impulses);
        mixed_out->max_joint_velocity_error_m_s = joint_velocity_error;
        mixed_out->max_joint_length_error_m = joint_length_error;
    }
    return PS_OK;
}
ps_result ps_contacts_resolve_graph(ps_body *bodies, size_t body_count,
                                    const ps_contact_constraint *contacts, size_t count,
                                    const ps_contact_solver *settings,
                                    ps_contact_graph_solution *out) {
    return resolve_graph(bodies, body_count, contacts, count, settings, out, NULL, 0, 1, NULL,NULL);
}
ps_result ps_contacts_resolve_graph_warm(ps_body *bodies,size_t body_count,
    const ps_contact_constraint *contacts,size_t count,const ps_contact_solver *settings,
    const ps_vec3 *initial,ps_contact_graph_solution *out) {
    return resolve_graph(bodies,body_count,contacts,count,settings,out,NULL,0,1,NULL,initial);
}
ps_result ps_constraints_resolve_graph(ps_body *bodies, size_t body_count,
                                       const ps_contact_constraint *contacts, size_t contact_count,
                                       const ps_distance_constraint *joints, size_t joint_count,
                                       const ps_contact_solver *settings, double dt,
                                       ps_constraint_graph_solution *out) {
    return resolve_graph(bodies, body_count, contacts, contact_count, settings, NULL, joints,
                         joint_count, dt, out,NULL);
}
/* Exponent scaling prevents intermediate overflow/underflow for finite factors. */
static double product4(double a, double b, double c, double d) {
    int ea, eb, ec, ed;
    double m = frexp(a, &ea) * frexp(b, &eb) * frexp(c, &ec) * frexp(d, &ed);
    return scalbn(m, ea + eb + ec + ed);
}
ps_result ps_buoyancy_force(double density, double volume, ps_vec3 gravity, ps_vec3 *out) {
    if (!out || !isfinite(density) || density < 0 || !isfinite(volume) || volume < 0 ||
        !finite3(gravity))
        return PS_INVALID;
    ps_vec3 force =
        ps_v3(product4(density, volume, -gravity.x, 1), product4(density, volume, -gravity.y, 1),
              product4(density, volume, -gravity.z, 1));
    if (!finite3(force))
        return PS_NUMERIC;
    *out = force;
    return PS_OK;
}
ps_result ps_sphere_submersion(double radius, double height, ps_submersion *out) {
    if (!out || !isfinite(radius) || radius <= 0 || !isfinite(height))
        return PS_INVALID;
    ps_submersion result = {0};
    if (height < radius) {
        double q = height <= -radius ? 2
                   : height > 0      ? (radius - height) / radius
                                     : 1 - height / radius;
        /* q is cap height / radius. Integrate circular horizontal sections. */
        double factor = PS_PI * q * q * (1 - q / 3);
        result.volume_m3 = product4(radius, radius, radius, factor);
        result.centroid_offset_m = -radius * ((2 - q) * (2 - q) / (4 * (1 - q / 3)));
        if (!isfinite(result.volume_m3) || !isfinite(result.centroid_offset_m))
            return PS_NUMERIC;
    }
    *out = result;
    return PS_OK;
}
ps_result ps_sphere_drag(ps_vec3 v, ps_medium medium, ps_drag_model model, double radius, double cd,
                         ps_vec3 *out) {
    if (!out || !finite3(v) || !isfinite(medium.density_kg_m3) || medium.density_kg_m3 < 0 ||
        !isfinite(medium.viscosity_pa_s) || medium.viscosity_pa_s < 0 || model < PS_DRAG_NONE ||
        model > PS_DRAG_QUADRATIC || !isfinite(radius) || radius <= 0 || !isfinite(cd) || cd < 0)
        return PS_INVALID;
    if (model == PS_DRAG_NONE || (model == PS_DRAG_STOKES && medium.viscosity_pa_s == 0) ||
        (model == PS_DRAG_QUADRATIC && (medium.density_kg_m3 == 0 || cd == 0))) {
        *out = ps_v3(0, 0, 0);
        return PS_OK;
    }
    double coefficient = model == PS_DRAG_NONE ? 0
                         : model == PS_DRAG_STOKES
                             ? 6 * PS_PI * medium.viscosity_pa_s * radius
                             : .5 * medium.density_kg_m3 * cd * PS_PI * radius * radius * norm(v);
    ps_vec3 force = ps_vscale(v, -coefficient);
    if (!finite3(force))
        return PS_NUMERIC;
    *out = force;
    return PS_OK;
}
ps_result ps_spring_force(ps_vec3 a, ps_vec3 va, ps_vec3 b, ps_vec3 vb, double k, double rest,
                          double damping, ps_vec3 *out) {
    if (!out || !finite3(a) || !finite3(b) || !finite3(va) || !finite3(vb) || !isfinite(k) ||
        k < 0 || !isfinite(rest) || rest < 0 || !isfinite(damping) || damping < 0)
        return PS_INVALID;
    ps_vec3 delta = ps_vsub(b, a);
    double length = norm(delta);
    if (!isfinite(length))
        return PS_NUMERIC;
    if (!length && (k || damping))
        return PS_SINGULAR;
    ps_vec3 n = length ? direction(delta, length) : ps_v3(0, 0, 0);
    ps_vec3 force = ps_vscale(n, k * (length - rest) + damping * ps_vdot(ps_vsub(vb, va), n));
    if (!finite3(force))
        return PS_NUMERIC;
    *out = force;
    return PS_OK;
}
