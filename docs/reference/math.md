# C-Referenz: Vektoren, Matrizen und Kurven

Alle Komponenten sind double. Matrizen sind spaltenweise gespeichert und wirken auf Spaltenvektoren; A*B führt B zuerst aus. Winkel sind Radiant. Verwende für Richtungen, Punkte und Normalen die jeweils passende Transformation. Geprüfte Operationen melden ungültige/nicht darstellbare Ergebnisse.

[Anleitung und Beispiele](../math.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/math.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.

## Typen und Funktionen

### ps_vec2

```c
typedef struct {
    double x, y;
} ps_vec2;
```

Arithmetic uses double and performs no allocation. Like the existing Vec3 arithmetic, unchecked operations may overflow. Matrices are column-major, act on column vectors, and A*B applies B first. Angles are radians.

### ps_vec4

```c
typedef struct {
    double x, y, z, w;
} ps_vec4;
```

### ps_mat3

```c
typedef struct {
    double m[9];
} ps_mat3;
```

## ps_v2

Erzeugt einen 2D-Vektor aus den angegebenen Komponenten.

```c
ps_vec2 ps_v2(double x, double y);
```

## ps_v2add

Komponentenweise Summe zweier Vektoren.

```c
ps_vec2 ps_v2add(ps_vec2 a, ps_vec2 b);
```

## ps_v2sub

Komponentenweise Differenz a-b.

```c
ps_vec2 ps_v2sub(ps_vec2 a, ps_vec2 b);
```

## ps_v2scale

Multipliziert jede Vektorkomponente mit dem Skalar s.

```c
ps_vec2 ps_v2scale(ps_vec2 a, double s);
```

## ps_v2dot

Skalarprodukt; beispielsweise für Projektion oder Energie.

```c
double ps_v2dot(ps_vec2 a, ps_vec2 b);
```

## ps_v2cross

Vorzeichenbehaftete Fläche a.x*b.y-a.y*b.x, die Z-Komponente des 2D-Kreuzprodukts.

```c
double ps_v2cross(ps_vec2 a, ps_vec2 b);
```

## ps_v2length

Euklidische Vektorlänge mit skalierter Berechnung gegen unnötigen Überlauf.

```c
double ps_v2length(ps_vec2 a);
```

signed area, +Z component

## ps_v2normalize

Normiert auf Länge eins; null bleibt null, nichtendliche Eingaben ergeben NaN-Komponenten.

```c
ps_vec2 ps_v2normalize(ps_vec2 a);
```

## ps_v4

Erzeugt einen 4D-Vektor aus den angegebenen Komponenten.

```c
ps_vec4 ps_v4(double x, double y, double z, double w);
```

## ps_v4add

Komponentenweise Summe zweier Vektoren.

```c
ps_vec4 ps_v4add(ps_vec4 a, ps_vec4 b);
```

## ps_v4sub

Komponentenweise Differenz a-b.

```c
ps_vec4 ps_v4sub(ps_vec4 a, ps_vec4 b);
```

## ps_v4scale

Multipliziert jede Vektorkomponente mit dem Skalar s.

```c
ps_vec4 ps_v4scale(ps_vec4 a, double s);
```

## ps_v4dot

Skalarprodukt; beispielsweise für Projektion oder Energie.

```c
double ps_v4dot(ps_vec4 a, ps_vec4 b);
```

## ps_v4length

Euklidische Vektorlänge mit skalierter Berechnung gegen unnötigen Überlauf.

```c
double ps_v4length(ps_vec4 a);
```

## ps_v4normalize

Normiert auf Länge eins; null bleibt null, nichtendliche Eingaben ergeben NaN-Komponenten.

```c
ps_vec4 ps_v4normalize(ps_vec4 a);
```

## ps_close

Prüft zwei Zahlen mit kombinierter absoluter und relativer Toleranz.

```c
bool ps_close(double a, double b, double absolute_tolerance, double relative_tolerance);
```

Length avoids spurious square overflow/underflow. Normalize rescales first, including subnormals and vectors whose length exceeds DBL_MAX. Zero maps to zero; nonfinite input maps to all-NaN. The same contract holds for Vec3.

### ps_bezier3

```c
typedef struct {
    ps_vec3 points[4];
} ps_bezier3;
```

### ps_curve_sample3

```c
typedef struct {
    ps_vec3 position;
    ps_vec3 tangent;
} ps_curve_sample3;
```

derivative with respect to dimensionless parameter t

## ps_bezier3_evaluate

Wertet eine kubische Bézierkurve bei t aus und liefert Position sowie Ableitung nach t.

```c
ps_result ps_bezier3_evaluate(const ps_bezier3 *curve, double t, ps_curve_sample3 *out);
```

Cubic Bezier with four finite world-space control points, t in [0,1]. All coordinates share the caller's length unit. Tangent is d(position)/dt, not a normalized direction or physical velocity. No arc-length parametrization. De Casteljau evaluation preserves endpoints, including degenerate curves. No allocation. Invalid inputs -> PS_INVALID, nonfinite result -> PS_NUMERIC; all outputs remain unchanged on error.

## ps_bezier3_split

Teilt eine kubische Bézierkurve bei t in zwei Kurven mit jeweils eigener Parametrisierung [0,1].

```c
ps_result ps_bezier3_split(
    const ps_bezier3 *curve,
    double t,
    ps_bezier3 *left,
    ps_bezier3 *right);
```

Exact geometric subdivision at t. Each output uses its own parameter [0,1]: left(u)=curve(t*u), right(u)=curve(t+(1-t)*u), up to floating-point roundoff. At t=0/1 one side is a constant curve. left and right must be distinct objects; either may alias curve. No mutation on invalid input.

## ps_mat3_identity

Liefert die Einheitsmatrix.

```c
ps_mat3 ps_mat3_identity(void);
```

## ps_mat3_multiply

Matrixprodukt a*b; bei Anwendung auf Vektoren wirkt b zuerst.

```c
ps_mat3 ps_mat3_multiply(ps_mat3 a, ps_mat3 b);
```

## ps_mat3_transpose

Vertauscht Zeilen und Spalten.

```c
ps_mat3 ps_mat3_transpose(ps_mat3 a);
```

## ps_mat3_apply

Wendet die Matrix auf einen Spaltenvektor an.

```c
ps_vec3 ps_mat3_apply(ps_mat3 a, ps_vec3 v);
```

## ps_mat4_transpose

Vertauscht Zeilen und Spalten.

```c
ps_mat4 ps_mat4_transpose(ps_mat4 a);
```

## ps_mat4_apply

Wendet die Matrix auf einen Spaltenvektor an.

```c
ps_vec4 ps_mat4_apply(ps_mat4 a, ps_vec4 v);
```

## ps_mat3_inverse

Invertiert mit skalierter Pivotisierung; tolerance=0 wählt die Standardtoleranz.

```c
ps_result ps_mat3_inverse(ps_mat3 a, double pivot_tolerance, ps_mat3 *out);
```

Checked operations below preserve *out on every error; input/output aliasing is supported. Invalid inputs -> PS_INVALID; unrepresentable arithmetic -> PS_NUMERIC. Inverse uses scaled partial pivoting; tolerance=0 selects n*eps, otherwise 0<tolerance<1. Rejected pivots -> PS_SINGULAR, not a condition estimate.

## ps_mat4_inverse

Invertiert mit skalierter Pivotisierung; tolerance=0 wählt die Standardtoleranz.

```c
ps_result ps_mat4_inverse(ps_mat4 a, double pivot_tolerance, ps_mat4 *out);
```

## ps_quat_identity

Liefert die Identitätsrotation (0,0,0,1).

```c
ps_quat ps_quat_identity(void);
```

## ps_quat_conjugate

Negiert den Vektoranteil; bei Einheitsquaternion die inverse Rotation.

```c
ps_quat ps_quat_conjugate(ps_quat q);
```

## ps_quat_multiply

Hamilton-Produkt a*b zur Komposition von Rotationen.

```c
ps_quat ps_quat_multiply(ps_quat a, ps_quat b);
```

## ps_quat_normalize

Normiert eine gültige, von null verschiedene Quaternion geprüft.

```c
ps_result ps_quat_normalize(ps_quat q, ps_quat *out);
```

## ps_quat_slerp

Interpoliert Rotationen sphärisch auf dem kürzeren Weg, mit t in [0,1].

```c
ps_result ps_quat_slerp(ps_quat a, ps_quat b, double t, ps_quat *out);
```

Normalize endpoints and interpolate the shortest rotation; t in [0,1]. q and -q describe the same rotation; output sign need not equal endpoint sign.

## ps_mat4_translation

Erzeugt eine homogene Verschiebungsmatrix.

```c
ps_mat4 ps_mat4_translation(ps_vec3 translation);
```

## ps_mat4_scale

Erzeugt eine homogene Skalierungsmatrix.

```c
ps_mat4 ps_mat4_scale(ps_vec3 scale);
```

## ps_mat4_rotation

Erzeugt die homogene Rotationsmatrix einer Quaternion.

```c
ps_result ps_mat4_rotation(ps_quat rotation, ps_mat4 *out);
```

Right-handed active rotation, normalized internally. Zero quaternion invalid. TRS applies local scale, then rotation, then translation. Zero scale allowed.

## ps_mat4_trs

Kombiniert Translation, Rotation und Skalierung; die Skalierung wirkt zuerst.

```c
ps_result ps_mat4_trs(
    ps_vec3 translation,
    ps_quat rotation,
    ps_vec3 scale,
    ps_mat4 *out);
```

## ps_transform_point

Transformiert einen Punkt einschließlich Translation und homogener Division.

```c
ps_result ps_transform_point(ps_mat4 transform, ps_vec3 point, ps_vec3 *out);
```

Point: homogeneous divide by w (zero w -> PS_SINGULAR). Direction/normal require an affine last row [0,0,0,1] exactly. Normal uses inverse-transpose and returns a unit vector; zero normal invalid.

## ps_transform_direction

Transformiert eine Richtung ohne Translation.

```c
ps_result ps_transform_direction(ps_mat4 transform, ps_vec3 direction, ps_vec3 *out);
```

## ps_transform_normal

Transformiert eine Normale mit invers transponiertem linearem Anteil; ungeeignete Transformationen werden abgewiesen.

```c
ps_result ps_transform_normal(ps_mat4 transform, ps_vec3 normal, ps_vec3 *out);
```
