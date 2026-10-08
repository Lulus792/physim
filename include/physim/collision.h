#ifndef PHYSIM_COLLISION_H
#define PHYSIM_COLLISION_H
#include "mechanics.h"

typedef struct {
    ps_vec3 minimum_m, maximum_m;
} ps_aabb;
typedef struct {
    uint32_t a, b;
} ps_collision_pair;
#define PS_BROAD_PHASE_MAX_BODIES 1024u
/* World-space bounds. Finite ordered coordinates; zero extent is allowed.
 * Constructors round outward and pad box extents for floating-point error.
 * Invalid/unrepresentable inputs leave out unchanged. Planes are unbounded and
 * must be tested separately. Bounds describe the current pose, not swept motion. */
ps_result ps_aabb_sphere(const ps_body *body, double radius_m, ps_aabb *out);
ps_result ps_aabb_box(const ps_body *body, ps_vec3 size_m, ps_aabb *out);
#define PS_CONVEX_MAX_VERTICES 64u
#define PS_CONVEX_MAX_TRIANGLES 128u
/* Borrowed body-local SI vertices and outward-wound triangle indices. The mesh
 * must be a closed, connected, strictly three-dimensional convex surface. Every
 * vertex is used; each undirected edge has exactly two opposite incidences.
 * Coplanar face triangulation is allowed. Caller retains all storage; no hull
 * generation, hidden allocation or inferred mass/inertia. Counts are bounded.
 * Validation uses coordinates scaled by their largest absolute component and
 * 128*DBL_EPSILON tolerance; unresolved thin/degenerate geometry is rejected. */
typedef struct {
    const ps_vec3 *vertices_m;
    size_t vertex_count;
    const uint32_t (*triangles)[3];
    size_t triangle_count;
} ps_convex_mesh;
ps_result ps_convex_validate(const ps_convex_mesh *mesh);
/* Discrete SAT over face normals and all edge cross products, including full
 * containment. One representative shared contact, normal A toward B, suitable
 * for ps_contact_resolve. It is not a multi-point resting manifold. Touching is
 * closed within scale-dependent roundoff tolerance. No hit changes only touching;
 * errors preserve both outputs. Inputs/output storage must be disjoint.
 * Bodies, mesh storage and plane geometry are unchanged. Discrete query. */
ps_result ps_contact_convexes(const ps_body *a, const ps_convex_mesh *mesh_a,
                              const ps_body *b, const ps_convex_mesh *mesh_b,
                              ps_contact *out, bool *touching);
ps_result ps_contact_convex_plane(const ps_body *body, const ps_convex_mesh *mesh,
                                  ps_vec3 plane_point_m, ps_vec3 plane_normal,
                                  ps_contact *out, bool *touching);
/* Sphere is A, convex mesh B. Interior centers use the nearest exit surface.
 * Deep initial containment is geometric overlap, not a physical impact. */
ps_result ps_contact_sphere_convex(const ps_body *sphere, double radius_m,
                                   const ps_body *body, const ps_convex_mesh *mesh,
                                   ps_contact *out, bool *touching);
/* Bounds contain every transformed mesh vertex, padded and rounded outward. */
ps_result ps_aabb_convex(const ps_body *body, const ps_convex_mesh *mesh, ps_aabb *out);

typedef struct {
    double fraction; /* First contact in closed [0,1] of the supplied displacement. */
    ps_contact contact;
} ps_sweep_hit;
/* Linear translation over one interval; displacements in metres, independent
 * of stored velocities. Initial touching/overlap reports fraction=0 (even when
 * separating). No hit writes touching=false and preserves hit. Errors preserve
 * both outputs and all bodies. Plane normal is unit and points into free space.
 * Sphere normals point A toward B. No acceleration, response or remaining-time
 * integration; solve the contact and recompute the remainder in caller code.
 * Floating-point precision limits time/position accuracy for extreme scale ratios. */
ps_result ps_sweep_spheres(const ps_body *a, double radius_a_m, ps_vec3 displacement_a_m,
                           const ps_body *b, double radius_b_m, ps_vec3 displacement_b_m,
                           ps_sweep_hit *hit, bool *touching);
ps_result ps_sweep_sphere_plane(const ps_body *body, double radius_m, ps_vec3 displacement_m,
                                ps_vec3 plane_point_m, ps_vec3 plane_normal, ps_sweep_hit *hit,
                                bool *touching);
/* Convex sweeps translate the supplied meshes with FIXED orientations. Stored
 * angular/linear velocities are not integrated. Initial overlap follows the
 * discrete query (fraction=0); new hits have zero penetration. Face and edge
 * interval entry/exit times are intersected, including grazing contact. One
 * representative contact. Same ownership, error and no-hit rules as above.
 * Rotation, acceleration and multi-body event scheduling require caller work. */
ps_result ps_sweep_convexes(const ps_body *a, const ps_convex_mesh *mesh_a,
                            ps_vec3 displacement_a_m, const ps_body *b,
                            const ps_convex_mesh *mesh_b, ps_vec3 displacement_b_m,
                            ps_sweep_hit *hit, bool *touching);
ps_result ps_sweep_convex_plane(const ps_body *body, const ps_convex_mesh *mesh,
                                ps_vec3 displacement_m, ps_vec3 plane_point_m,
                                ps_vec3 plane_normal, ps_sweep_hit *hit, bool *touching);
/* Sphere is A; both bodies translate and the mesh orientation stays fixed.
 * Tests face, edge-cylinder and vertex-ball entry, then returns one contact. */
ps_result ps_sweep_sphere_convex(const ps_body *sphere, double radius_m,
                                 ps_vec3 sphere_displacement_m, const ps_body *body,
                                 const ps_convex_mesh *mesh, ps_vec3 mesh_displacement_m,
                                 ps_sweep_hit *hit, bool *touching);
ps_result ps_aabb_swept_convex(const ps_body *body, const ps_convex_mesh *mesh,
                               ps_vec3 displacement_m, ps_aabb *out);

/* Explicit path over fraction t in [0,1]: position=p0+t*translation+t*t*quadratic;
 * orientation=exp(t*rotation_rad)*q0. The rotation vector is a WORLD axis times
 * total radians, so full turns are preserved rather than endpoint-slerped away.
 * Stored velocities, masses and inertia do not define this geometric path. */
typedef struct {
    ps_vec3 translation_m, rotation_rad, quadratic_m;
} ps_rigid_motion;
typedef struct {
    double distance_tolerance_m;
    uint32_t max_iterations;
} ps_ccd_settings;
#define PS_CCD_MAX_ITERATIONS 65536u
extern const ps_ccd_settings PS_CCD_DEFAULT;
/* Pose only; all other body fields are copied. Static poses may be prescribed.
 * Invalid/unrepresentable inputs preserve out. fraction must be in [0,1]. */
ps_result ps_body_motion_pose(const ps_body *body, ps_rigid_motion motion,
                              double fraction, ps_body *out);
/* Conservative advancement with an explicit distance envelope and work budget.
 * One contact at separation <=distance_tolerance_m (or initial overlap at zero).
 * No hit means the WHOLE remaining path was excluded by a separation/speed bound.
 * PS_LIMIT means unresolved, never a no-hit result: all outputs stay unchanged.
 * Numeric uncertainty exceeding the requested envelope returns PS_NUMERIC.
 * Constant world-axis rotation and quadratic translation; no force integration
 * or automatic event response. Both meshes may rotate through multiple turns.
 * settings=NULL chooses PS_CCD_DEFAULT. Same borrowed meshes/atomic outputs. */
ps_result ps_sweep_convexes_motion(const ps_body *a, const ps_convex_mesh *mesh_a,
                                   ps_rigid_motion motion_a, const ps_body *b,
                                   const ps_convex_mesh *mesh_b, ps_rigid_motion motion_b,
                                   const ps_ccd_settings *settings, ps_sweep_hit *hit, bool *touching);
ps_result ps_sweep_convex_plane_motion(const ps_body *body, const ps_convex_mesh *mesh,
                                      ps_rigid_motion motion, ps_vec3 plane_point_m,
                                      ps_vec3 plane_normal, const ps_ccd_settings *settings,
                                      ps_sweep_hit *hit, bool *touching);
ps_result ps_sweep_sphere_convex_motion(const ps_body *sphere, double radius_m,
                                        ps_rigid_motion sphere_motion, const ps_body *body,
                                        const ps_convex_mesh *mesh, ps_rigid_motion mesh_motion,
                                        const ps_ccd_settings *settings, ps_sweep_hit *hit, bool *touching);
/* Bounding sphere around the mesh origin contains every rotated vertex.
 * Quadratic translation extrema on every coordinate are included. */
ps_result ps_aabb_motion_convex(const ps_body *body, const ps_convex_mesh *mesh,
                                ps_rigid_motion motion, ps_aabb *out);

/* Bounds for the whole linearly translated sphere, suitable for broad_phase.
 * Current-pose bounds alone can miss CCD candidates. Transactional output. */
ps_result ps_aabb_swept_sphere(const ps_body *body, double radius_m, ps_vec3 displacement_m,
                               ps_aabb *out);
/* Stateless sweep on X, closed overlap on Y/Z. Returns candidate pairs only;
 * a<b are INPUT ARRAY indices, sorted lexicographically, without duplicates.
 * Count <=1024, bounded stack memory, no allocation. Worst-case O(n² log n),
 * including sorting the resulting pairs; sparse X intervals reduce the sweep.
 * Call again after poses change. Static/filter exclusions are caller-owned.
 * count=0 permits NULL bounds. capacity=0 permits NULL pairs (size query).
 * PS_LIMIT for insufficient capacity writes required pair_count but no pairs;
 * other errors leave both outputs unchanged. On success writes pair_count pairs.
 * Exceeding the body-count limit returns PS_LIMIT without changing pair_count.
 * All input/output storage must be disjoint. Touching counts as overlap. */
ps_result ps_broad_phase(const ps_aabb *bounds, size_t count, ps_collision_pair *pairs,
                         size_t capacity, size_t *pair_count);
#endif
