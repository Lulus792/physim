# C-Referenz: Mechanik und Medien

Erzeuge Körper und Trägheit, summiere Kräfte/Drehmomente, integriere und löse Kontakte beziehungsweise Gelenke. SI-Einheiten und Welt-/Lokalkoordinaten beachten. Geometrische Kontaktprüfung und Impulsantwort sind getrennte Schritte. Der Lerntext enthält vollständige Abläufe für Einzelkontakte, Graphen, Gelenke, Medien und CCD.

[Anleitung und Beispiele](../mechanics.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/mechanics.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_CONTACT_MAX_POINTS 8u
#define PS_CONTACT_GRAPH_MAX_BODIES 128u
#define PS_CONTACT_GRAPH_MAX_CONTACTS 512u
#define PS_CONTACT_WORLD UINT32_MAX
#define PS_CONSTRAINT_GRAPH_MAX_JOINTS 256u
```

## Typen und Funktionen

### ps_body

```c
typedef struct {
    ps_vec3 position_m, velocity_m_s;
    ps_quat orientation;
    ps_vec3 angular_velocity_rad_s;
    double mass_kg;
    ps_vec3 inertia_kg_m2;
} ps_body;
```

SI, world-space position/velocities, body-to-world unit quaternion. The local axes are principal inertia axes. mass=0 is static (zero velocities/inertia). Public values are validated at every operation. No hidden allocations.

## ps_body_sphere

Erzeugt Masse und Hauptträgheit einer homogenen Kugel.

```c
ps_result ps_body_sphere(double mass_kg, double radius_m, ps_body *out);
```

Homogeneous solid sphere/box, initially at rest at origin. Box sizes are full extents, not half sizes. mass>=0, strictly positive radius/extents. Dynamic inertias round the exact binary-input rational formulas once (nearest-even); zero-rounded/overflowed positive inertias return PS_NUMERIC atomically.

## ps_body_box

Erzeugt Masse und Hauptträgheit einer homogenen Box mit vollen Seitenlängen.

```c
ps_result ps_body_box(double mass_kg, ps_vec3 size_m, ps_body *out);
```

## ps_body_validate

Prüft Masse, Hauptträgheit, Pose und Geschwindigkeiten eines Körpers.

```c
ps_result ps_body_validate(const ps_body *body);
```

## ps_body_with_inertia

Erzeugt einen ruhenden Körper mit expliziter Masse und Hauptträgheiten für eigene Formen.

```c
ps_result ps_body_with_inertia(
    double mass_kg,
    ps_vec3 principal_inertia_kg_m2,
    ps_body *out);
```

Explicit mass and principal inertia for caller-defined shapes. Initially at rest at origin with identity orientation. Positive mass requires three finite positive principal inertias; static mass=0 requires zero inertia. No inferred density/center/principal-axis calculation. Invalid inputs preserve out.

## ps_body_kinetic_energy

Berechnet translatorische plus rotatorische kinetische Energie.

```c
ps_result ps_body_kinetic_energy(const ps_body *body, double *joules);
```

Sum of translational and principal-axis rotational energy. Identity-frame terms round once; general rotation uses a normalized quaternion and scaled binary64 rotation before summation. Zero-rounded energy is valid; overflow returns PS_NUMERIC without changing joules. No full-range dynamics guarantee.

## ps_body_point_velocity

Berechnet die Weltgeschwindigkeit eines Punkts einschließlich Rotation.

```c
ps_result ps_body_point_velocity(
    const ps_body *body,
    ps_vec3 point_m,
    ps_vec3 *velocity_m_s);
```

## ps_body_force_torque

Berechnet das Drehmoment einer an einem Weltpunkt angreifenden Kraft.

```c
ps_result ps_body_force_torque(
    const ps_body *body,
    ps_vec3 force_n,
    ps_vec3 point_m,
    ps_vec3 *torque_nm);
```

A force at a world-space point contributes (point-center) x force torque.

## ps_body_apply_impulse

Ändert lineare und rotatorische Geschwindigkeit durch einen Impuls am Weltpunkt.

```c
ps_result ps_body_apply_impulse(ps_body *body, ps_vec3 impulse_ns, ps_vec3 point_m);
```

## ps_body_step

Integriert Körperbewegung unter der vorgegebenen Kraft und dem Drehmoment.

```c
ps_result ps_body_step(ps_body *body, ps_vec3 force_n, ps_vec3 torque_nm, double dt_s);
```

First-order symplectic linear step, explicit gyroscopic angular acceleration, then exponential quaternion update with new angular velocity. dt>0; force and torque in world coordinates. Static bodies remain unchanged. Not an implicit or energy-preserving rotation integrator; choose and refine dt explicitly.

### ps_distance_joint

```c
typedef struct {
    ps_vec3 anchor_a_m, anchor_b_m;
    double length_m;
    double stabilization;
} ps_distance_joint;
```

Body-local; B is world-space when B is NULL.

Strictly positive.

0..1: fraction of length error corrected per dt.

## ps_distance_joint_validate

Prüft endliche lokale Anker, eine positive endliche Soll-Länge und eine Stabilisierung in 0..1. Liefert PS_OK oder PS_INVALID; verändert das Gelenk nicht.

```c
ps_result ps_distance_joint_validate(const ps_distance_joint *joint);
```

Validate finite local anchors, positive length and stabilization in 0..1.

### ps_distance_joint_solution

```c
typedef struct {
    ps_vec3 impulse_on_a_ns;
    double length_error_m;
    double velocity_error_m_s;
} ps_distance_joint_solution;
```

Before solve; positions/orientations are unchanged.

Absolute residual including stabilization target.

## ps_distance_joint_resolve

Löst ein Distanzgelenk über Geschwindigkeitsimpulse und ein Stabilisierungsziel; Positionen bleiben unverändert.

```c
ps_result ps_distance_joint_resolve(
    ps_body *a,
    ps_body *b,
    const ps_distance_joint *joint,
    double dt_s,
    ps_distance_joint_solution *out);
```

One bilateral velocity constraint along the current anchor separation. Solve after external velocity updates and before advancing positions. dt>0. NULL B anchors to the static world. Coincident anchors return PS_SINGULAR. Static/static succeeds with zero impulse and reports the unmet velocity target. Stabilization can add energy; use timestep refinement. No joint/contact graph iteration or warm start. All inputs/outputs must be disjoint; errors are atomic. out is optional.

### ps_contact

```c
typedef struct {
    ps_vec3 point_m, normal;
    double penetration_m;
} ps_contact;
```

unit normal from A toward B; shared impulse point

## ps_contact_spheres

Ermittelt Kontaktgeometrie zwischen zwei Kugeln.

```c
ps_result ps_contact_spheres(
    const ps_body *a,
    double radius_a_m,
    const ps_body *b,
    double radius_b_m,
    ps_contact *out,
    bool *touching);
```

Closed contact (touching counts). No hit sets touching=false and leaves the contact unchanged. Coincident sphere centers use deterministic +X normal.

## ps_contact_sphere_plane

Ermittelt den Kontakt einer Kugel mit einer Ebene.

```c
ps_result ps_contact_sphere_plane(
    const ps_body *sphere,
    double radius_m,
    ps_vec3 plane_point_m,
    ps_vec3 plane_normal,
    ps_contact *out,
    bool *touching);
```

Plane normal points toward free space; the negative half-space is solid.

## ps_contact_sphere_box

Ermittelt den Kontakt einer Kugel mit einer orientierten Box.

```c
ps_result ps_contact_sphere_box(
    const ps_body *sphere,
    double radius_m,
    const ps_body *box,
    ps_vec3 box_size_m,
    ps_contact *out,
    bool *touching);
```

Oriented box, full extents. Sphere is A, box is B. For an interior sphere center, use the nearest box face (ties X, Y, Z; zero chooses positive face). Projection can eject the sphere, but deep initial overlap is not a physical impact.

### ps_contact_manifold

```c
typedef struct {
    uint32_t count;
    ps_contact points[PS_CONTACT_MAX_POINTS];
} ps_contact_manifold;
```

## ps_contacts_box_plane

Erzeugt mehrere Kontaktpunkte zwischen orientierter Box und Ebene.

```c
ps_result ps_contacts_box_plane(
    const ps_body *box,
    ps_vec3 size_m,
    ps_vec3 plane_point_m,
    ps_vec3 plane_normal,
    ps_contact_manifold *out);
```

Up to eight box vertices in/on the solid half-space. Box is A, static plane B. Full extents; plane normal is unit and points toward free space. No contact succeeds with count=0. Transactional output on invalid/unrepresentable input.

## ps_contacts_boxes

Erzeugt ein Kontaktmanifold für zwei orientierte Boxen.

```c
ps_result ps_contacts_boxes(
    const ps_body *a,
    ps_vec3 size_a_m,
    const ps_body *b,
    ps_vec3 size_b_m,
    ps_contact_manifold *out);
```

Two oriented boxes, full extents; normals point from A toward B. SAT over face/edge axes, clipped face manifold (up to 8 points) or one edge-edge point. Contact points lie halfway between the corresponding surfaces. Touching counts; no hit succeeds with count=0. Roundoff tolerance is 64*DBL_EPSILON times the largest half extent; cross axes shorter than 32*DBL_EPSILON are degenerate. Deterministic ties prefer A faces, then B faces, then edges. Initial containment yields an ejection manifold, not a physical impact. Discrete detection, no CCD. Output remains unchanged on failure. Bodies must be distinct and valid.

### ps_contact_solver

```c
typedef struct {
    uint32_t iterations;
    double restitution, friction;
    double bounce_threshold_m_s;
    double penetration_slop_m, correction_fraction;
} ps_contact_solver;
```

1..256, deterministic fixed budget

restitution only below negative threshold

fraction in [0,1]

### PS_CONTACT_SOLVER_DEFAULT

```c
extern const ps_contact_solver PS_CONTACT_SOLVER_DEFAULT;
```

### ps_contact_solution

```c
typedef struct {
    uint32_t count;
    ps_vec3 impulse_on_a_ns[PS_CONTACT_MAX_POINTS];
    double max_normal_error_m_s;
} ps_contact_solution;
```

complementarity residual before position projection

## ps_contacts_resolve

Löst mehrere Kontakte eines Körperpaars iterativ mit akkumulierten Normal-/Reibungsimpulsen.

```c
ps_result ps_contacts_resolve(
    ps_body *a,
    ps_body *b,
    const ps_contact_manifold *contacts,
    const ps_contact_solver *settings,
    ps_contact_solution *out);
```

Pair manifold: accumulated projected normal impulses and a two-dimensional Coulomb cone, repeated for the configured iterations. NULL B is static world. Translation-only penetration projection accounts for prior corrections, so coplanar contacts do not multiply the correction. Does not solve a body graph, warm-start across steps, generate contacts or perform CCD. PS_OK means the fixed iteration budget completed, not guaranteed convergence; inspect residual. All bodies and optional solution stay unchanged on failure.

### ps_contact_constraint

```c
typedef struct {
    uint32_t a, b;
    ps_contact contact;
} ps_contact_constraint;
```

Body array indices; only B may be PS_CONTACT_WORLD.

### ps_contact_graph_solution

```c
typedef struct {
    uint32_t count;
    ps_vec3 impulse_on_a_ns[PS_CONTACT_GRAPH_MAX_CONTACTS];
    double max_normal_error_m_s;
    double max_projection_error_m;
} ps_contact_graph_solution;
```

Before position projection.

Unmet requested translation correction.

## ps_contacts_resolve_graph

Löst zusammenhängende Kontakte mehrerer Körper gemeinsam, etwa Kontaktketten und Stapel.

```c
ps_result ps_contacts_resolve_graph(
    ps_body *bodies,
    size_t body_count,
    const ps_contact_constraint *constraints,
    size_t count,
    const ps_contact_solver *settings,
    ps_contact_graph_solution *out);
```

Simultaneous contact graph with accumulated normal/friction impulses. Deterministic input order; restitution targets use INITIAL contact velocities. Fixed iterations across ALL constraints, then repeated translation projection. Contacts/normals are fixed during solving; regenerate them each physical step. No allocation, warm start, contact generation, CCD or joint constraints. PS_OK means budget completed, not convergence: inspect both residuals. Bodies, constraints, settings and optional output must occupy disjoint storage. Every body is validated, including unused/static bodies. Errors change no output. Zero counts permit NULL arrays. Limits are above; exceeding them is PS_LIMIT.

## ps_contacts_resolve_graph_warm

Wie der Kontaktsolver mit expliziten Startimpulsen auf A. Restitution wird vor sämtlichen Warmimpulsen bestimmt; Startwerte werden auf die aktuelle Normale und den Coulomb-Kegel projiziert. NULL wählt den kalten Pfad.

```c
ps_result ps_contacts_resolve_graph_warm(
    ps_body *bodies,
    size_t body_count,
    const ps_contact_constraint *contacts,
    size_t count,
    const ps_contact_solver *settings,
    const ps_vec3 *initial_impulse_on_a_ns,
    ps_contact_graph_solution *out);
```

Same graph solve, initialized by per-contact world-space impulses ON A. initial may be NULL for the exact cold path. Finite seeds are projected onto the current normal/Coulomb cone before application. Restitution targets use velocities BEFORE all warm impulses. out reports the total applied impulse, including the seed. Caller owns contact matching and timestep scaling. Invalid seeds preserve every body/output. Storage must be disjoint.

### ps_distance_constraint

```c
typedef struct {
    uint32_t a, b;
    ps_distance_joint joint;
} ps_distance_constraint;
```

Only B may be PS_CONTACT_WORLD.

### ps_constraint_graph_solution

```c
typedef struct {
    ps_contact_graph_solution contacts;
    uint32_t joint_count;
    ps_vec3 joint_impulse_on_a_ns[PS_CONSTRAINT_GRAPH_MAX_JOINTS];
    double max_joint_velocity_error_m_s;
    double max_joint_length_error_m;
} ps_constraint_graph_solution;
```

Before contact position projection.

After contact position projection.

## ps_constraints_resolve_graph

Löst gemischte Kontakte und Distanzgelenke mehrerer Körper gemeinsam.

```c
ps_result ps_constraints_resolve_graph(
    ps_body *bodies,
    size_t body_count,
    const ps_contact_constraint *contacts,
    size_t contact_count,
    const ps_distance_constraint *joints,
    size_t joint_count,
    const ps_contact_solver *settings,
    double dt_s,
    ps_constraint_graph_solution *out);
```

Shared velocity iterations: contacts first, then distance joints, in input order. Initial velocities determine restitution; joint stabilization uses dt_s>0. Same body/contact limits as ps_contacts_resolve_graph, plus 256 joints. Contact translation projection follows velocity solving. It can change joint lengths: inspect final length error, or set correction_fraction=0 and use small steps with joint stabilization. No direct joint position/orientation projection. PS_OK is not a convergence guarantee. Conflicting constraints retain residuals. No allocations, warm start or automatic integration. Inputs/outputs disjoint; errors change no bodies/output. NULL arrays allowed only with zero counts. out is optional.

## ps_contact_resolve

Wendet die Impulsantwort für einen Einzelkontakt an.

```c
ps_result ps_contact_resolve(
    ps_body *a,
    ps_body *b,
    const ps_contact *contact,
    double restitution,
    double friction,
    ps_vec3 *impulse_on_a_ns);
```

Single-contact impulse response, e in [0,1], Coulomb mu>=0. NULL B is static world. Resolves closing normal velocity, then one tangential impulse bounded by mu*normal_impulse, then mass-weighted penetration projection. No impulse for separating contact. Does not solve stacks/constraints or perform CCD. Bodies and optional impulse_on_a_ns stay unchanged on any error.

### ps_drag_model

```c
typedef enum { PS_DRAG_NONE, PS_DRAG_STOKES, PS_DRAG_QUADRATIC } ps_drag_model;
```

## ps_buoyancy_force

Berechnet Auftrieb aus Fluiddichte, verdrängtem Volumen und Gravitation.

```c
ps_result ps_buoyancy_force(
    double density_kg_m3,
    double displaced_volume_m3,
    ps_vec3 gravity_m_s2,
    ps_vec3 *force_n);
```

Archimedes force = -density * displaced volume * gravity, in world space. Uniform fluid density>=0, volume>=0, finite gravity. Apply at the centroid of displaced fluid using ps_body_force_torque; weight must be added separately. No surface tension, added mass, waves or fluid solver.

### ps_submersion

```c
typedef struct {
    double volume_m3;
    double centroid_offset_m;
} ps_submersion;
```

from sphere center along outward surface normal

## ps_sphere_submersion

Berechnet eingetauchtes Kugelvolumen und dessen Schwerpunkt relativ zu einer ebenen Wasseroberfläche.

```c
ps_result ps_sphere_submersion(
    double radius_m,
    double center_height_m,
    ps_submersion *out);
```

Sphere cut by a planar surface; fluid occupies its negative half-space. center_height is signed center-to-plane distance, radius>0. Dry returns {0,0}; fully submerged returns full volume and zero centroid offset. Partial immersion returns spherical-cap volume and its centroid. For uniform hydrostatic fluid, surface normal must oppose gravity. Geometry only; no body mutation. Very small results may underflow to zero in double precision.

## ps_sphere_drag

Berechnet Stokes- oder quadratischen Widerstand einer Kugel relativ zum Medium.

```c
ps_result ps_sphere_drag(
    ps_vec3 relative_velocity_m_s,
    ps_medium medium,
    ps_drag_model model,
    double radius_m,
    double drag_coefficient,
    ps_vec3 *force_n);
```

Sphere in a uniform medium; relative velocity = body minus fluid velocity. Stokes: 6*pi*viscosity*r*v, creeping-flow assumption. Quadratic: rho*Cd*A*v²/2. No buoyancy, added mass, automatic Reynolds switching or rotational drag.

## ps_spring_force

Berechnet axiale Feder-/Dämpferkraft zwischen bewegten Endpunkten.

```c
ps_result ps_spring_force(
    ps_vec3 a_m,
    ps_vec3 velocity_a_m_s,
    ps_vec3 b_m,
    ps_vec3 velocity_b_m_s,
    double stiffness_n_m,
    double rest_length_m,
    double damping_ns_m,
    ps_vec3 *force_on_a_n);
```

Axial Hooke spring + axial viscous damper. Returns force on A; B gets -force. Coincident endpoints with active stiffness/damping are PS_SINGULAR.
