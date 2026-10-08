# Vektoren, Matrizen und Transformationen

`physim/math.h` ergänzt den Mathematikkern aus `core.h`. Alle Funktionen arbeiten
mit `double`, ohne Heap-Allokation und ohne GUI-Abhängigkeit. Die Erweiterung ändert
keine bestehende Modulstruktur und benötigt keinen neuen ABI-Versionswert.

## Konventionen

- Vektoren: `ps_vec2`, `ps_vec3`, `ps_vec4`. Ein vierdimensionaler Vektor ist keine
  Quaternion; `ps_quat` besitzt einen eigenen Typ mit Komponenten `(x,y,z,w)`.
- Rechtshändiges Koordinatensystem, aktive Rotation, Winkel in Radiant.
- Matrizen: `ps_mat3` und `ps_mat4`, spaltenweise Speicherung. Das Element in Zeile
  `row`, Spalte `column` steht bei `m[column * n + row]`.
- Matrizen wirken auf Spaltenvektoren. `A * B` wendet zuerst B, dann A an.
- Mathematische Funktionen tragen keine Einheiteninformation. Position und
  Translation müssen dieselbe Einheit verwenden; Skalierungsfaktoren sind dimensionslos.
  In der Physim-Szene sind Positionen und Längen Meter.

## Vektoren und Vergleiche

`ps_v2` / `ps_v3` / `ps_v4` erzeugen Vektoren. Für Vec2 und Vec4 heißen die Operationen
`ps_v2add`, `ps_v2sub`, `ps_v2scale`, `ps_v2dot`, `ps_v2length`, `ps_v2normalize`
beziehungsweise `ps_v4add` usw. Die bestehenden Vec3-Funktionen behalten ihre Namen
`ps_vadd`, `ps_vsub`, `ps_vscale`, `ps_vdot`, `ps_vlength`, `ps_vnormalize`.
`ps_v2cross` liefert die vorzeichenbehaftete Fläche beziehungsweise die Z-Komponente
des 3D-Kreuzprodukts; `ps_vcross` liefert einen 3D-Vektor. In 4D ist kein solches
Kreuzprodukt definiert.

Die Länge verwendet `hypot`, damit das Quadrieren großer oder kleiner Komponenten
keinen künstlichen Über- oder Unterlauf erzeugt. Eine mathematische Länge größer
als `DBL_MAX` bleibt unendlich. Normalisierung skaliert vor der Längenberechnung:
Auch ein Vektor aus `DBL_MAX`-Komponenten oder subnormalen Zahlen wird normiert.
Der Nullvektor bleibt Null; eine nichtendliche Komponente liefert ausschließlich NaN.

`ps_close(a,b,absolute,relative)` prüft die symmetrische Bedingung
`abs(a-b) <= absolute + relative * max(abs(a),abs(b))`, für die exakten binären Eingabewerte.
Beide Toleranzen müssen endlich und nichtnegativ sein. Nichtendliche
Vergleichswerte liefern immer `false`, auch zwei gleiche Unendlichkeiten. Die absolute
Toleranz hat die Einheit der Werte, die relative ist dimensionslos. Beide null verlangen
exakte Gleichheit einschließlich signierter Null. Ein konservatives Fehlerintervall
entscheidet gewöhnliche Fälle; an der Grenze und bei Über-/Unterlauf werden
Produkte und Summen mit begrenzten Integerarrays exakt verglichen. Kein Heap und
keine breitere Fließkommapräzision werden benötigt.

Physim bietet denselben Vertrag mit `isClose(left,right,absoluteTolerance,relativeTolerance)`.
Ungültige Werte/Toleranzen ergeben `false`; es gibt keine versteckte Standardtoleranz.
`tests/test_close_range_oracle.py` vergleicht C- und Physim-Ergebnisse mit unabhängigen
rationalen Referenzen einschließlich der benachbarten Double-Werte beiderseits der Grenze.

## Quaternionen

`ps_quat_identity`, `ps_quat_conjugate` und `ps_quat_multiply` liefern Identität,
Konjugierte und Hamilton-Produkt. Bei Einheitsquaternionen ist die Konjugierte die
inverse Rotation. `ps_quat_multiply(a,b)` wendet zuerst die Rotation b, dann a an.
Das Produkt selbst normalisiert nicht.

`ps_quat_axis_angle` normalisiert die Achse, auch bei extremen Größen. Die Nullachse
liefert Identität; nichtendliche Eingaben liefern NaN. `ps_quat_rotate` setzt eine
Einheitsquaternion voraus. Beliebige endliche, von null verschiedene Quaternionen
können vorher mit `ps_quat_normalize` normalisiert werden.

`ps_quat_slerp(a,b,t,&q)` normalisiert beide Eingaben und interpoliert für `0 <= t <= 1`
auf dem kürzeren Rotationsbogen. Fast gleiche Rotationen verwenden normalisierte
lineare Interpolation (Quaternion-Skalarprodukt größer als `1 - 1e-8`). q und -q
repräsentieren dieselbe Rotation; das Vorzeichen des Ergebnisses muss deshalb nicht
mit dem Vorzeichen am Endpunkt übereinstimmen. Bei exakt 180 Grad ist der kürzere
Bogen nicht eindeutig; die Eingabevorzeichen bestimmen die Wahl. Kein Extrapolieren.

## Matrizen und Transformationen

Identität, Produkt, Transponieren und Anwenden sind für 3×3 und 4×4 verfügbar.
`ps_mat3_apply` und `ps_mat4_apply` führen die normale Matrix-Vektor-Multiplikation
aus; die 4×4-Funktion führt noch keine Division durch die homogene Koordinate aus.

`ps_mat3_inverse` und `ps_mat4_inverse` lösen für die Spalten der Inversen mit dem
vorhandenen linearen Solver. Er verwendet skalierte partielle Pivotwahl. Toleranz 0
wählt `n * DBL_EPSILON`, sonst muss sie zwischen 0 und 1 liegen. Ein zu kleiner Pivot
liefert `PS_SINGULAR`. Das ist keine vollständige Konditionsschätzung: Ein erfolgreiches
Ergebnis garantiert bei schlecht konditionierten Eingaben keine hohe Genauigkeit.

`ps_mat4_translation`, `ps_mat4_scale` und `ps_mat4_rotation` erzeugen einzelne
Transformationen. `ps_mat4_trs` kombiniert lokale Skalierung, Rotation und Translation
in dieser Reihenfolge. Die Rotation wird intern normalisiert. Negative und nullwertige
Skalen sind erlaubt; eine Nullskala macht die Transformation nicht invertierbar.

```c
#include <physim/math.h>

ps_mat4 model;
ps_quat turn = ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 2);
ps_result result = ps_mat4_trs(ps_v3(10, 20, 30), turn,
                              ps_v3(2, 3, -4), &model);
if (result == PS_OK) {
    ps_vec3 world;
    result = ps_transform_point(model, ps_v3(1, 2, 3), &world);
    /* world liegt ungefähr bei (4, 22, 18). */
}
```

Es gibt bewusst drei verschiedene Operationen:

- `ps_transform_point`: multipliziert `(x,y,z,1)` und dividiert durch w. Damit sind
  auch projektive Matrizen möglich. w=0 liefert `PS_SINGULAR`. Ein negatives w ist
  mathematisch erlaubt; sichtbares Kamera-Clipping gehört weiterhin zum Renderer.
- `ps_transform_direction`: multipliziert `(x,y,z,0)`. Translation verändert eine
  Richtung nicht. Länge kann sich durch Skalierung ändern.
- `ps_transform_normal`: löst das invers-transponierte lineare 3×3-System und
  normalisiert das Ergebnis. Das erhält die Senkrechtstellung zu transformierten
  Tangenten auch bei Scherung oder ungleichmäßiger Skalierung. Eine Nullnormale ist
  ungültig; eine singuläre lineare Matrix wird abgewiesen. Bei Spiegelung ist eine
  zusätzliche Anpassung der Dreiecks-Windung Aufgabe des Aufrufers.

Richtung und Normale verlangen eine affine letzte Zeile exakt `(0,0,0,1)`.
Die Funktion für Normalen prüft nur den linearen 3×3-Teil auf Invertierbarkeit;
große Translationen ändern die Normale nicht.

## Fehler und Genauigkeit

Funktionen mit Rückgabewert `ps_result` lassen ihre Ausgabe bei **jedem** Fehler
unverändert. Ein- und Ausgabe dürfen dieselbe Variable sein. Null-Ausgabezeiger,
NaN, Unendlichkeiten und verletzte Parametergrenzen ergeben `PS_INVALID`.
Nicht darstellbare Endergebnisse ergeben `PS_NUMERIC`; abgelehnte Pivots oder
w=0 ergeben `PS_SINGULAR`. Unterlauf einzelner Komponenten auf null wird nicht
generell als Fehler behandelt.

Punkte und Richtungen behalten Produkte und Summen ihrer höchstens vier
binären Terme exakt in einem festen Ganzzahlakkumulator. Auslöschungsreste,
auch weit unter den einzelnen Produkten, bleiben bis zur Ausgabe erhalten.
Punkte dividieren den exakten Zähler durch die exakte homogene Koordinate;
das Ergebnis wird einmal auf Double gerundet, bei Gleichstand zur geraden
Mantisse. Dies schließt unnötige Zwischenüberläufe und doppeltes Runden nahe
Null. Gewöhnliche affine Zeilen verwenden kurze, fehlerfreie Gleitkomma-
Entwicklungen aus Produkten und Summen; kleine Produkte oder überlaufende
Zwischenstufen fallen auf den Ganzzahlakkumulator zurück. Keine Heap-Allokation, kein
breiterer Gleitkommatyp; ungeprüfte Matrixarithmetik bleibt gewöhnliche Double-
Arithmetik. Rundung kleiner Endwerte auf null ist zulässig.

Normalen brauchen keine vollständig darstellbare Inverse. Nach Zeilenskalierung
der ursprünglichen Matrix wird das transponierte System gelöst; die separaten
Exponenten seiner Lösung werden erst nach gemeinsamer Skalierung normiert.
Die Pivotprüfung verwendet die relative Standardschwelle `3*DBL_EPSILON` im
zeilenskalierten System. Das kann eine stark ungleichmäßig skalierte Matrix
anders beurteilen als das explizite Invertieren. Schlecht konditionierte oder
durch Koeffizientennormierung informationsarme Matrizen bleiben begrenzt;
es gibt keine allgemeine exakte Normalen- oder Konditionsgarantie.

Ungeprüfte arithmetische Funktionen, Matrixprodukte und Konstruktoren ohne
`ps_result` geben die normalen Gleitkommaergebnisse zurück; sie können überlaufen.

## Kubische räumliche Kurven

`ps_bezier3` enthält vier Kontrollpunkte mit derselben Längeneinheit.
`ps_bezier3_evaluate` wertet die kubische Bézierkurve für einen dimensionslosen
Parameter `t` von 0 bis 1 aus. Die Endpunkte werden exakt übernommen; die mittleren
Kontrollpunkte steuern die Tangenten und liegen im Allgemeinen nicht auf der Kurve.
Die Berechnung verwendet das De-Casteljau-Verfahren aus konvexen Interpolationen.
Es vermeidet einen unnötigen Überlauf der Differenz entgegengesetzt großer Werte.

Die Ausgabe enthält Position und `dPosition/dt`. Die Tangente ist weder normiert
noch eine physikalische Geschwindigkeit. Bei gleichmäßigem Durchlaufen in einer
Dauer `T` ist die Geschwindigkeit `Tangente/T`. Gleiche Parameterabstände sind
keine gleichen Wegabstände; es gibt keine automatische Bogenlängenparametrisierung.

```c
#include "physim/math.h"
#include "physim/experiment.h"

ps_result add_path(ps_scene *scene) {
    const ps_bezier3 curve = {{{0, 0, 0}, {1, 2, 0}, {2, 2, 1}, {3, 0, 1}}};
    ps_vec3 points[33];
    for (unsigned i = 0; i < 33; i++) {
        ps_curve_sample3 sample;
        ps_result r = ps_bezier3_evaluate(&curve, i / 32.0, &sample);
        if (r != PS_OK) return r;
        points[i] = sample.position;
    }
    return ps_scene_polyline(scene, points, 33, .01, 0x70b1eeff);
}
```

`ps_bezier3_split` teilt eine Kurve bei `t` in zwei kubische Kurven. Beide neuen
Parameter laufen wieder von 0 bis 1: `links(u)=original(t*u)` und
`rechts(u)=original(t+(1-t)*u)`, bis auf Rundungsfehler. Daher skalieren ihre
Tangenten mit `t` beziehungsweise `1-t`; sie sind am gemeinsamen Punkt parallel,
aber im Allgemeinen unterschiedlich lang. Bei Unterteilung an einem Endpunkt
wird eine Hälfte konstant. Beide Ausgabeobjekte müssen verschieden sein;
eines darf das Eingabeobjekt ersetzen.

Nichtendliche Kontrollpunkte, ungültige Parameter oder fehlende Ausgabezeiger
ergeben `PS_INVALID`. Eine nicht darstellbare Tangente ergibt `PS_NUMERIC`, auch
wenn die Position noch darstellbar wäre. Alle Ausgaben bleiben bei Fehlern
unverändert. Die Unterteilung benötigt keine Tangenten und kann daher auch in
diesem Fall gelingen. Konstante Kurven und Nulltangenten sind gültig.

Die Kurvenprüfung verwendet die unabhängige Referenz `(t,t²,t³)` samt Ableitung,
1.000 deterministische Unterteilungen mit jeweils elf Auswertungen pro Hälfte,
Endpunkte, degenerierte Kurven, extreme Zahlenwerte und Aliasierung.

## Prüfung weiterer mathematischer Funktionen

Der Test `math` prüft analytische Werte, Subnormalzahlen, `DBL_MAX`, Null- und
Fehlerfälle, unveränderte Ausgaben und Aliasierung. Je 2000 deterministische Fälle
prüfen allgemeine Matrixinversion, Quaternionen gegen eine unabhängige
Rodrigues-Rotation sowie Transformationen einschließlich Scherung und Spiegelung.
Normalen werden gegen zwei unabhängig transformierte Tangenten geprüft. Die
bestehenden Mechanik- und GPU-Tests sichern die Nutzung der verbesserten
Normalisierung in Simulation und Renderer ab.
