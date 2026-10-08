#ifndef PHYSIM_CORE_H
#define PHYSIM_CORE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define PS_PI 3.14159265358979323846
#define PS_API_VERSION 3u
#define PS_ABI_VERSION 3u
typedef enum {
    PS_OK,
    PS_INVALID,
    PS_IO,
    PS_MEMORY,
    PS_VERSION,
    PS_CORRUPT,
    PS_EOF,
    PS_RECOVERED,
    PS_SINGULAR,
    PS_LIMIT,
    PS_NUMERIC
} ps_result;
const char *ps_result_string(ps_result result);
typedef struct {
    double x, y, z;
} ps_vec3;
ps_vec3 ps_v3(double x, double y, double z);
ps_vec3 ps_vadd(ps_vec3 a, ps_vec3 b);
ps_vec3 ps_vsub(ps_vec3 a, ps_vec3 b);
ps_vec3 ps_vscale(ps_vec3 a, double s);
double ps_vdot(ps_vec3 a, ps_vec3 b);
ps_vec3 ps_vcross(ps_vec3 a, ps_vec3 b);
double ps_vlength(ps_vec3 a);
/* Rescales before normalization: zero -> zero; nonfinite -> all NaN. */
ps_vec3 ps_vnormalize(ps_vec3 a);
typedef struct {
    double x, y, z, w;
} ps_quat;
typedef struct {
    double m[16];
} ps_mat4;
/* Right-handed rotation. Axis normalized internally; zero axis -> identity,
 * nonfinite axis/angle -> all NaN. */
ps_quat ps_quat_axis_angle(ps_vec3 axis, double angle_rad);
/* Requires a unit quaternion. Checked normalization is in physim/math.h. */
ps_vec3 ps_quat_rotate(ps_quat q, ps_vec3 v);
ps_mat4 ps_mat4_identity(void);
ps_mat4 ps_mat4_multiply(ps_mat4 a, ps_mat4 b);
/* Exponents: length, mass, time, current, temperature, amount, luminous intensity. */
typedef struct {
    int8_t dimension[7];
    double scale;
    const char *symbol;
} ps_unit;
extern const ps_unit PS_METRE, PS_SECOND, PS_KILOGRAM, PS_RADIAN, PS_JOULE, PS_VELOCITY;
/* Matching dimensions, finite value and positive finite scales required.
 * Convert exact binary value*from.scale/to.scale with one nearest-even rounding
 * in the default floating environment. Identity conversions preserve all bits.
 * Nonfinite final result or nonzero value rounding to zero -> PS_NUMERIC.
 * Every failure preserves output; signed zero and output aliasing supported. */
ps_result ps_convert(double value, ps_unit from, ps_unit to, double *output);
/* Explicit PCG32 state, copied by value; no hidden cache/global random state.
 * Initialize with ps_rng_seed before drawing. Seed fixes the odd increment;
 * saving/restoring both fields preserves the stream. External synchronization
 * is required when sharing one instance. Unchecked helpers require non-NULL
 * initialized state and valid finite distribution parameters. */
typedef struct {
    uint64_t state, increment;
} ps_rng;
void ps_rng_seed(ps_rng *rng, uint64_t seed);
uint32_t ps_rng_u32(ps_rng *rng);
/* Exactly one uint32 draw, mapped to the open interval (0,1). */
double ps_rng_uniform(ps_rng *rng);
/* Box-Muller: radius uniform first, angle uniform second, always two draws
 * (including sd=0). No spare-value cache. Transcendental rounding can differ
 * between libm implementations; checked/degenerate draws use measurement.h. */
double ps_rng_normal(ps_rng *rng, double mean, double standard_deviation);
typedef void (*ps_ode_fn)(double time, const double *state, double *derivative, void *user);
typedef enum { PS_EULER, PS_SYMPLECTIC, PS_RK4, PS_VERLET, PS_RK45 } ps_integrator;
/* RK4/Euler support 1..32 first-order states. Time/dt share caller time units
 * (seconds in experiments); derivative[i] is state[i] per time unit. Finite
 * time/dt, dt>0, finite t+dt/state and complete finite callback outputs required.
 * Scaled weighted arithmetic avoids intermediate product/sum range failures;
 * every evaluated stage and final state must remain representable. Double
 * rounding applies; no general exact-rounding or global-error guarantee.
 * No allocation. Caller state is preserved on every error. */
ps_result ps_ode_step(ps_integrator method, ps_ode_fn fn, void *user, double time, double dt,
                      double *state, size_t n);
/* Symplectic Euler for separable q'=v, v'=a(q). */
void ps_symplectic_step(double *position, double *velocity, double acceleration, double dt);
typedef struct {
    double density_kg_m3, viscosity_pa_s;
    const char *name;
} ps_medium;
typedef struct {
    double density_kg_m3, restitution, friction;
    const char *name;
} ps_material;
extern const ps_medium PS_VACUUM, PS_AIR, PS_WATER;
ps_vec3 ps_drag_force(ps_vec3 velocity_m_s, ps_medium medium, double coefficient, double area_m2);
typedef struct {
    ps_vec3 position_m, velocity_m_s;
    double mass_kg, radius_m;
} ps_particle;
bool ps_collide_spheres(ps_particle *a, ps_particle *b, double restitution);
#endif
