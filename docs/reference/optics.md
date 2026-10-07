# C-Referenz: Geometrische Optik

Reflexion, Snell-Brechung mit ausdrücklicher Totalreflexion und paraxiale dünne Linsen. Unit-Richtungen und Normalen, explizite Brechungsindizes und signierte Bildweiten. Keine automatische Strahlverfolgung, Fresnelamplituden oder Beugung.

[Anleitung und Beispiele](../waves-optics.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/optics.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.

## Typen und Funktionen

## ps_ray_reflect

Reflektiert eine validierte Unit-Richtung an einer orientierten Unit-Normale.

```c
ps_result ps_ray_reflect(ps_vec3 incident, ps_vec3 normal, ps_vec3 *reflected);
```

Pure geometric optics; no allocation or hidden state. Finite inputs only. All errors preserve output. PS_INVALID: bad input/NULL output; PS_NUMERIC: unrepresentable result; PS_SINGULAR: total internal reflection or focal plane. Directions/normals must be unit vectors within 1e-10; accepted values are normalized. Normal points into incident medium; incident dot normal<=1e-10. Homogeneous isotropic media, sharp interface, no Fresnel amplitude, polarization, absorption, diffraction, interference or automatic ray tracing.

## ps_ray_refract

Berechnet Snell-Brechung oder meldet ausdrücklich Totalreflexion ohne Ausgabeänderung.

```c
ps_result ps_ray_refract(
    ps_vec3 incident,
    ps_vec3 normal,
    double incident_index,
    double transmitted_index,
    ps_vec3 *transmitted);
```

Snell's law with positive indices n1/n2. Critical-angle sin(theta2) within 32*DBL_EPSILON above one is clamped to one. Beyond it PS_SINGULAR denotes TIR; call reflection explicitly if that is the desired physical branch.

## ps_thin_lens_image

Berechnet signierte Bildweite und Vergrößerung einer paraxialen dünnen Linse.

```c
ps_result ps_thin_lens_image(
    double focal_length_m,
    double object_distance_m,
    ps_vec2 *out);
```

Paraxial thin lens: signed nonzero focal length f, positive real-object distance d, both m. out.x image distance=f*d/(d-f), out.y magnification=-f/(d-f). Negative image distance means virtual; negative magnification means inverted. d==f has image at infinity and returns PS_SINGULAR. No thick lenses/aberration.
