# C-Referenz: Persistente Kontakte und Warmstart

Ein expliziter, caller-eigener Kontaktzustand erzeugt diskrete Kugel-/Box-/Ebenenkontakte. Stabile Collider-IDs und lokale Anker ordnen Kontakte zwischen erfolgreichen Schritten zu; alte Impulse werden zeitabhängig skaliert und im aktuellen Coulomb-Kegel gelöst. Keine Heapallokation, keine automatische Integration oder CCD. Körper, Cache und Ergebnisse bleiben bei Fehlern unverändert.

[Anleitung und Beispiele](../contact-world.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/contact_world.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_CONTACT_WORLD_VERSION 1u
```

## Typen und Funktionen

### ps_collider_shape

```c
typedef enum {PS_COLLIDER_SPHERE=1,PS_COLLIDER_BOX=2,PS_COLLIDER_PLANE=3} ps_collider_shape;
```

### ps_collider

```c
typedef struct {
    uint32_t id,body;
    ps_collider_shape shape;
    ps_vec3 size_m,plane_normal;
} ps_collider;
```

One collider per body. Stable nonzero ID, distinct body array index. Sphere: size_m={radius,0,0}; box: full positive extents; plane: size_m=0, plane_normal is a body-local unit normal into free space. Plane body is static; its position is on the plane and its orientation rotates the normal. Non-plane plane_normal is zero. No offsets/compound or convex shapes.

### ps_contact_world_settings

```c
typedef struct {
    double match_distance_m,minimum_normal_dot;
    double maximum_dt_ratio,warm_fraction;
} ps_contact_world_settings;
```

### PS_CONTACT_WORLD_DEFAULT

```c
extern const ps_contact_world_settings PS_CONTACT_WORLD_DEFAULT;
```

### ps_contact_world_model

```c
typedef struct {
    ps_collider collider;
    double mass_kg;
    ps_vec3 inertia_kg_m2;
} ps_contact_world_model;
```

### ps_cached_contact

```c
typedef struct {
    uint32_t id_a,id_b;
    ps_vec3 local_a_m,local_b_m,normal,impulse_on_a_ns;
    ps_contact_constraint constraint;
} ps_cached_contact;
```

### ps_contact_world

```c
typedef struct {
    uint32_t struct_size,version,model_count,body_count,count;
    ps_contact_world_settings settings;
    double dt_s;
    ps_contact_world_model models[PS_CONTACT_GRAPH_MAX_BODIES];
    ps_cached_contact contacts[PS_CONTACT_GRAPH_MAX_CONTACTS];
} ps_contact_world;
```

Caller-owned, bounded, independent value. Treat all fields as read-only except through init/reset/solve. No heap, global cache or implicit body integration. Arrays are canonical by stable collider ID; contact order is deterministic.

### ps_contact_world_result

```c
typedef struct {
    uint32_t count,matched,created,ended,warmed;
    ps_contact_graph_solution solution;
} ps_contact_world_result;
```

## ps_contact_world_init

Initialisiert den begrenzten Kontaktzustand mit Größenprüfung und validierten Match-/Warmstart-Einstellungen; Fehler erhalten den bisherigen Wert.

```c
ps_result ps_contact_world_init(
    ps_contact_world *world,size_t size,
    const ps_contact_world_settings *settings);
```

size must be >=sizeof *world; failure preserves world. settings=NULL selects defaults. Reset preserves validated settings and removes all history.

## ps_contact_world_reset

Entfernt die gesamte Kontakthistorie und behält gültige Einstellungen; für Modellreset, Teleports oder ersetzte Objekte verwenden.

```c
ps_result ps_contact_world_reset(ps_contact_world *world);
```

## ps_contact_world_solve

Erzeugt und löst aktuelle diskrete Kontakte, ordnet alte lokale Anker zu und aktualisiert Körper, Cache und Ergebnis atomar.

```c
ps_result ps_contact_world_solve(
    ps_contact_world *world,ps_body *bodies,size_t body_count,
    const ps_collider *colliders,size_t count,
    const ps_contact_solver *solver,double dt_s,
    ps_contact_world_result *out);
```

Generate discrete contacts through AABB broad phase and sphere/box/plane narrow phase, excluding static/static pairs. Canonical IDs survive body and collider array reorder. Match BOTH local anchors one-to-one and reject large normal changes or changed shape/mass/inertia. History contains only the prior successful call; absent contacts expire immediately. dt scales warm impulses; ratios outside [1/maximum_dt_ratio,maximum_dt_ratio] discard them. Separating contacts above bounce_threshold also discard warm impulses. Restitution uses pre-warm velocities and friction projects onto the CURRENT cone. Fixed solver budget, no convergence guarantee; inspect residuals. All bodies, world and optional result remain unchanged on failure, including capacity or numerical failures. Inputs/outputs must be disjoint. Zero counts permit NULL arrays. Limits: 128 bodies/colliders and 512 contacts. No CCD or joint solve. Use stable IDs for the same physical objects; reset after teleports, scene resets or object replacement. All state copies are independent.
