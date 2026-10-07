# C-Referenz: Kollisionserkennung

Broad Phase liefert mögliche Paare, keine fertigen Kontakte. Prüfe Kandidaten geometrisch und löse anschließend ihre Impulse. Sweep-Funktionen liefern den ersten Kontakt entlang einer vorgegebenen Verschiebung; die verbleibende Bewegung muss der Aufrufer selbst integrieren. Ebenen separat behandeln.

[Anleitung und Beispiele](../mechanics.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/collision.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_BROAD_PHASE_MAX_BODIES 1024u
```

## Typen und Funktionen

### ps_aabb

```c
typedef struct {
    ps_vec3 minimum_m, maximum_m;
} ps_aabb;
```

### ps_collision_pair

```c
typedef struct {
    uint32_t a, b;
} ps_collision_pair;
```

## ps_aabb_sphere

Berechnet eine konservative achsenparallele Hüllbox der aktuellen Kugelpose.

```c
ps_result ps_aabb_sphere(const ps_body *body, double radius_m, ps_aabb *out);
```

World-space bounds. Finite ordered coordinates; zero extent is allowed. Constructors round outward and pad box extents for floating-point error. Invalid/unrepresentable inputs leave out unchanged. Planes are unbounded and must be tested separately. Bounds describe the current pose, not swept motion.

## ps_aabb_box

Berechnet eine konservative achsenparallele Hüllbox einer orientierten Box.

```c
ps_result ps_aabb_box(const ps_body *body, ps_vec3 size_m, ps_aabb *out);
```

### ps_sweep_hit

```c
typedef struct {
    double fraction;
    ps_contact contact;
} ps_sweep_hit;
```

First contact in closed [0,1] of the supplied displacement.

## ps_sweep_spheres

Ermittelt den ersten Kontakt zweier linear verschobener Kugeln als Anteil der Bewegung in [0,1].

```c
ps_result ps_sweep_spheres(
    const ps_body *a,
    double radius_a_m,
    ps_vec3 displacement_a_m,
    const ps_body *b,
    double radius_b_m,
    ps_vec3 displacement_b_m,
    ps_sweep_hit *hit,
    bool *touching);
```

Linear translation over one interval; displacements in metres, independent of stored velocities. Initial touching/overlap reports fraction=0 (even when separating). No hit writes touching=false and preserves hit. Errors preserve both outputs and all bodies. Plane normal is unit and points into free space. Sphere normals point A toward B. No acceleration, response or remaining-time integration; solve the contact and recompute the remainder in caller code. Floating-point precision limits time/position accuracy for extreme scale ratios.

## ps_sweep_sphere_plane

Ermittelt den ersten Kontakt einer linear verschobenen Kugel mit einer Ebene.

```c
ps_result ps_sweep_sphere_plane(
    const ps_body *body,
    double radius_m,
    ps_vec3 displacement_m,
    ps_vec3 plane_point_m,
    ps_vec3 plane_normal,
    ps_sweep_hit *hit,
    bool *touching);
```

## ps_aabb_swept_sphere

Berechnet die Hüllbox über die ganze geradlinige Verschiebung einer Kugel.

```c
ps_result ps_aabb_swept_sphere(
    const ps_body *body,
    double radius_m,
    ps_vec3 displacement_m,
    ps_aabb *out);
```

Bounds for the whole linearly translated sphere, suitable for broad_phase. Current-pose bounds alone can miss CCD candidates. Transactional output.

## ps_broad_phase

Erzeugt deterministisch sortierte Kandidatenpaare überlappender Hüllboxen; noch keine genaue Kontaktprüfung.

```c
ps_result ps_broad_phase(
    const ps_aabb *bounds,
    size_t count,
    ps_collision_pair *pairs,
    size_t capacity,
    size_t *pair_count);
```

Stateless sweep on X, closed overlap on Y/Z. Returns candidate pairs only; a<b are INPUT ARRAY indices, sorted lexicographically, without duplicates. Count <=1024, bounded stack memory, no allocation. Worst-case O(n² log n), including sorting the resulting pairs; sparse X intervals reduce the sweep. Call again after poses change. Static/filter exclusions are caller-owned. count=0 permits NULL bounds. capacity=0 permits NULL pairs (size query). PS_LIMIT for insufficient capacity writes required pair_count but no pairs; other errors leave both outputs unchanged. On success writes pair_count pairs. Exceeding the body-count limit returns PS_LIMIT without changing pair_count. All input/output storage must be disjoint. Touching counts as overlap.
