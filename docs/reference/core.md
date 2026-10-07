# C-Referenz: Grundlagen und Zufall

Vektoren, Rotation, Zufallsströme, Basiseinheiten und einfache Integrations-/Kollisionshelfer. Zufallsströme zuerst mit ps_rng_seed initialisieren; derselbe Seed wiederholt den Strom. ps_rng_uniform liefert Werte in (0,1). Winkel werden im Bogenmaß angegeben.

[Anleitung und Beispiele](../api.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/core.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_PI 3.14159265358979323846
#define PS_API_VERSION 3u
#define PS_ABI_VERSION 3u
```

## Typen und Funktionen

### ps_result

```c
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
```

## ps_result_string

Liefert den lesbaren Namen eines Rückgabewerts; der Text gehört der Bibliothek.

```c
const char *ps_result_string(ps_result result);
```

### ps_vec3

```c
typedef struct {
    double x, y, z;
} ps_vec3;
```

## ps_v3

Erzeugt einen 3D-Vektor aus den angegebenen Komponenten.

```c
ps_vec3 ps_v3(double x, double y, double z);
```

## ps_vadd

Komponentenweise Summe zweier Vektoren.

```c
ps_vec3 ps_vadd(ps_vec3 a, ps_vec3 b);
```

## ps_vsub

Komponentenweise Differenz a-b.

```c
ps_vec3 ps_vsub(ps_vec3 a, ps_vec3 b);
```

## ps_vscale

Multipliziert jede Vektorkomponente mit dem Skalar s.

```c
ps_vec3 ps_vscale(ps_vec3 a, double s);
```

## ps_vdot

Skalarprodukt; beispielsweise für Projektion oder Energie.

```c
double ps_vdot(ps_vec3 a, ps_vec3 b);
```

## ps_vcross

Rechtshändiges 3D-Kreuzprodukt a×b, etwa für Drehmoment.

```c
ps_vec3 ps_vcross(ps_vec3 a, ps_vec3 b);
```

## ps_vlength

Euklidische Vektorlänge mit skalierter Berechnung gegen unnötigen Überlauf.

```c
double ps_vlength(ps_vec3 a);
```

## ps_vnormalize

Normiert auf Länge eins; null bleibt null, nichtendliche Eingaben ergeben NaN-Komponenten.

```c
ps_vec3 ps_vnormalize(ps_vec3 a);
```

Rescales before normalization: zero -> zero; nonfinite -> all NaN.

### ps_quat

```c
typedef struct {
    double x, y, z, w;
} ps_quat;
```

### ps_mat4

```c
typedef struct {
    double m[16];
} ps_mat4;
```

## ps_quat_axis_angle

Erzeugt eine rechtshändige Rotation aus Achse und Winkel in Radiant.

```c
ps_quat ps_quat_axis_angle(ps_vec3 axis, double angle_rad);
```

Right-handed rotation. Axis normalized internally; zero axis -> identity, nonfinite axis/angle -> all NaN.

## ps_quat_rotate

Rotiert einen Vektor mit einer Einheitsquaternion.

```c
ps_vec3 ps_quat_rotate(ps_quat q, ps_vec3 v);
```

Requires a unit quaternion. Checked normalization is in physim/math.h.

## ps_mat4_identity

Liefert die Einheitsmatrix.

```c
ps_mat4 ps_mat4_identity(void);
```

## ps_mat4_multiply

Matrixprodukt a*b; bei Anwendung auf Vektoren wirkt b zuerst.

```c
ps_mat4 ps_mat4_multiply(ps_mat4 a, ps_mat4 b);
```

### ps_unit

```c
typedef struct {
    int8_t dimension[7];
    double scale;
    const char *symbol;
} ps_unit;
```

Exponents: length, mass, time, current, temperature, amount, luminous intensity.

### PS_VELOCITY

```c
extern const ps_unit PS_METRE, PS_SECOND, PS_KILOGRAM, PS_RADIAN, PS_JOULE, PS_VELOCITY;
```

## ps_convert

Konvertiert value zwischen dimensionskompatiblen Einheiten und schreibt das Ergebnis nach output.

```c
ps_result ps_convert(double value, ps_unit from, ps_unit to, double *output);
```

### ps_rng

```c
typedef struct {
    uint64_t state, increment;
} ps_rng;
```

## ps_rng_seed

Initialisiert einen PCG32-Zufallsstrom für einen reproduzierbaren Seed.

```c
void ps_rng_seed(ps_rng *rng, uint64_t seed);
```

## ps_rng_u32

Zieht die nächste vorzeichenlose 32-Bit-Zufallszahl und verändert den Stromzustand.

```c
uint32_t ps_rng_u32(ps_rng *rng);
```

## ps_rng_uniform

Zieht die nächste gleichverteilte Double-Zahl in (0,1).

```c
double ps_rng_uniform(ps_rng *rng);
```

## ps_rng_normal

Zieht eine normalverteilte Zahl mit Mittelwert und Standardabweichung.

```c
double ps_rng_normal(ps_rng *rng, double mean, double standard_deviation);
```

### ps_ode_fn

```c
typedef void (*ps_ode_fn)(double time, const double *state, double *derivative, void *user);
```

### ps_integrator

```c
typedef enum { PS_EULER, PS_SYMPLECTIC, PS_RK4, PS_VERLET, PS_RK45 } ps_integrator;
```

## ps_ode_step

Führt einen Euler- oder RK4-Schritt für n Zustandskomponenten aus; state wird bei Erfolg aktualisiert.

```c
ps_result ps_ode_step(
    ps_integrator method,
    ps_ode_fn fn,
    void *user,
    double time,
    double dt,
    double *state,
    size_t n);
```

RK4/Euler support up to 32 first-order states. No allocation, state owned by caller.

## ps_symplectic_step

Aktualisiert zuerst Geschwindigkeit, dann Position mit symplektischem Euler.

```c
void ps_symplectic_step(
    double *position,
    double *velocity,
    double acceleration,
    double dt);
```

Symplectic Euler for separable q'=v, v'=a(q).

### ps_medium

```c
typedef struct {
    double density_kg_m3, viscosity_pa_s;
    const char *name;
} ps_medium;
```

### ps_material

```c
typedef struct {
    double density_kg_m3, restitution, friction;
    const char *name;
} ps_material;
```

### PS_WATER

```c
extern const ps_medium PS_VACUUM, PS_AIR, PS_WATER;
```

## ps_drag_force

Berechnet quadratischen Widerstand entgegen der Geschwindigkeit, aus Medium, Widerstandsbeiwert und Stirnfläche.

```c
ps_vec3 ps_drag_force(
    ps_vec3 velocity_m_s,
    ps_medium medium,
    double coefficient,
    double area_m2);
```

### ps_particle

```c
typedef struct {
    ps_vec3 position_m, velocity_m_s;
    double mass_kg, radius_m;
} ps_particle;
```

## ps_collide_spheres

Einfache Stoßantwort für zwei Partikelkugeln; verändert die Partikel und meldet, ob ein Kontakt behandelt wurde.

```c
bool ps_collide_spheres(ps_particle *a, ps_particle *b, double restitution);
```
