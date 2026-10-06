# Persistente Kontakte und Warmstart

`physim/contact_world.h` ergänzt eine explizite Kontaktverwaltung für Kugeln,
orientierte Boxen und statische Ebenen. Sie verbindet die vorhandene AABB-Broad-
Phase, geometrische Narrow Phase und den gemeinsamen Kontaktsolver. Ein
`ps_contact_world` gehört dem Aufrufer, besitzt keine geliehenen Zeiger und wird
ohne Heapallokation verwendet. Eine Kopie besitzt eine unabhängige Historie.
Die API integriert weder Körper noch Kräfte automatisch.

```c
ps_contact_world world;
ps_contact_world_init(&world, sizeof world, NULL);
ps_body bodies[2];
ps_body_sphere(0, .5, &bodies[0]); // static floor body
ps_body_sphere(1, .5, &bodies[1]);
bodies[1].position_m.y = .5;
ps_collider colliders[] = {
    {1, 0, PS_COLLIDER_PLANE, {0,0,0}, {0,1,0}},
    {2, 1, PS_COLLIDER_SPHERE, {.5,0,0}, {0,0,0}}
};
ps_contact_world_result result;
ps_contact_world_solve(&world, bodies, 2, colliders, 2,
    &PS_CONTACT_SOLVER_DEFAULT, .005, &result);
```

Ein Collider hat eine stabile, positive ID und verweist auf genau einen Körper.
Die Körper-/Colliderarrays dürfen umgeordnet werden, solange diese Zuordnung
erhalten bleibt. Ein Körper hat höchstens einen Collider. Kugeln verwenden
`size_m={radius,0,0}`, Boxen volle positive Ausdehnungen. Ebenen verwenden einen
statischen Körper mit `size_m=0`; seine Position liegt auf der Ebene, seine
Quaternionrotation dreht die lokale Einheitsnormale in den freien Halbraum.
Andere Collider haben `plane_normal=0`. Statisch/statisch wird ausgeschlossen.

Der Zustand ordnet Collider nach IDs und Kontakte nach ihren kanonischen
Colliderpaaren. Beide Körperlokalanker müssen innerhalb `match_distance_m`
liegen, und die Weltnormalen müssen `minimum_normal_dot` erfüllen. Zuordnung
ist eins zu eins: Ein alter Kontakt kann nicht mehrere neue Punkte wärmen.
Form-/Größen-/Masse-/Trägheitsänderungen verwerfen die betreffende Zuordnung.
Fehlende Kontakte laufen nach einem erfolgreichen Aufruf sofort aus. Die IDs
gehören denselben physischen Objekten; nach Teleports, Modellreset oder Austausch
von Objekten muss der Aufrufer `ps_contact_world_reset` verwenden.

Die vier Einstellungen sind explizit: Ankertoleranz in Metern (Standard .02),
minimaler Normalendot (.95), maximaler Zeitschrittfaktor (4) und Warmstartanteil
(1). Werte werden geprüft; `warm_fraction=0` deaktiviert den Startimpuls.
Impulse werden mit `dt / previous_dt` skaliert. Außerhalb der zulässigen
Schrittverhältnisse oder bei deutlicher Trennung über der Bounce-Schwelle bleibt
die geometrische Zuordnung erhalten, der Startimpuls wird jedoch verworfen.

Alle Restitutionsziele werden aus den **ursprünglichen Geschwindigkeiten vor
jedem Warmimpuls** berechnet. Der alte Impuls wird auf eine nichtnegative
Normalkomponente und den aktuellen Coulomb-Kegel projiziert. Der iterative
Solver verbessert danach die akkumulierten Impulse. Sein Ergebnis beschreibt
den insgesamt in diesem Schritt angewendeten Impuls, einschließlich Warmstart.
Die separat nutzbare Funktion `ps_contacts_resolve_graph_warm` nimmt explizite
Startimpulse auf A entgegen; bei `initial=NULL` gilt der bisherige kalte Pfad.
Kontaktzuordnung und Zeitskalierung bleiben bei dieser Funktion caller-eigen.

`result.count`, `matched`, `created`, `ended` und `warmed` beschreiben die
Lebensdauer der aktuellen Kontaktpunkte. `solution` enthält die einzelnen
Impulse und die bisherigen Normal-/Projektionsreste. `PS_OK` bestätigt das
Abarbeiten des Iterationsbudgets und garantiert keine Konvergenz. Die gespeicherten
Kontaktpunkte gehören zur Geometrie vor der anschließenden Translationsprojektion.
Alle Körper, Cache und optionalen Ergebnisse bleiben bei Fehlern unverändert,
auch bei 512 überschrittenen Kontakten oder nicht darstellbaren Zahlen.

Grenzen: 128 Körper/Collider, 512 Kontaktpunkte, maximal 256 Solveriterationen.
Die Broad Phase berücksichtigt bis zu 8128 endliche Colliderpaare; lokale
Zuordnung ist im schlechtesten Fall quadratisch in der Kontaktzahl. Der begrenzte
Scratchspeicher liegt auf dem Stack. Die Funktion ist synchron; Zustand und
Körper werden nicht nebenläufig verwendet. Der allgemeine Graph-/Gelenksolver
und bestehende Struktur-/Modul-ABI bleiben unverändert.

Das [C-Stapelbeispiel](../examples/contact_stack/main.c) integriert vier Boxen
unter Gravitation und lässt Kontakte automatisch erzeugen. `warmStart=0..1`
und `iterations=1..256` sind normale Experimentparameter. Kanäle zeigen Höhen,
Geschwindigkeit, kinetische Energie, Kontaktlebensdauer und Solverreste;
aufgezeichnete Szenen zeigen Körper, Kontaktpunkte und Normalen. Die App-Prüfung
baut dieses Projekt und führt kontrollierte Einzelschritte aus.

Dieser Durchstich ist diskret und besitzt noch keine CCD, Compound-/konvexen
Collider, Kontaktinseln, persistente Feature-IDs, gemeinsame Gelenk-Warmstarts
oder nichtlineare Rotations-/Positionsprojektion. Fachlicher Hintergrund zur Wiederverwendung
akkumulierter Impulse: [Box2D-Dokumentation](https://box2d.org/documentation/md_simulation.html)
und [Catto: Understanding Constraints](https://box2d.org/files/ErinCatto_UnderstandingConstraints_GDC2014.pdf).
## Physim-Werte (0.177.0)

`Collider.sphere(id, body, radius)`, `Collider.box(id, body, size)` und
`Collider.plane(id, body, normal)` erzeugen kopierbare Colliderwerte. IDs liegen
in 1..4294967295, Körperindizes in 0..127; Maße/Normalen folgen dem C-Vertrag.
`id()`, `body()`, `shape()`, `size()` und `normal()` lesen die Werte.
`shape()` verwendet Kugel=1, Box=2, Ebene=3.

`ContactWorld.defaults()` verwendet die C-Standardwerte.
`ContactWorld(matchDistance, minimumNormalDot, maximumDtRatio, warmFraction)`
erlaubt dieselben expliziten Einstellungen. Der Zustand enthält keine impliziten
Kräfte oder Integration. Ein Schritt erzeugt einen neuen Snapshot:

```physim
var world = ContactWorld.defaults()
var bodies = [Body.sphere(0,0.5), Body.sphere(1,0.5)]
bodies[1].setState(Vec3(0,0.5,0),Vec3(0,-0.1,0),Quat(0,0,0,1),Vec3(0,0,0))
let colliders = [Collider.plane(1,0,Vec3(0,1,0)), Collider.sphere(2,1,0.5)]
world = world.solve(bodies,colliders,ContactSolver.defaults(),0.01)
bodies = world.bodies()
assert(world.contactCount() == 1)
```

`solve(bodies, colliders, solver, dt)` und `reset()` ändern ihren Receiver nicht.
Das Ergebnis ist wieder ein `ContactWorld`, damit die nächste Zuweisung den
Kontaktverlauf fortsetzt. Kopien teilen nur unveränderliche Speichersnapshots;
jeder nächste Schritt oder Reset ist unabhängig. Arrays, optionale Werte,
Structs, Rückgaben und Closures übernehmen automatische Besitzregeln.
`reset()` erhält die Einstellungen und entfernt Körper/Collider/Kontakte aus
dem Snapshot. Eingabekörper werden nie verändert; `bodies()` liefert einen
unabhängigen Körperarraywert. Fehler und Allokationsfehler erhalten sämtliche
Eingaben und gespeicherte Kopien.

| Methoden | Ergebnis |
| --- | --- |
| `bodyCount()`, `colliderCount()`, `contactCount()` | Anzahlen im Snapshot |
| `matched()`, `created()`, `ended()`, `warmed()` | Lebenszyklus des letzten erfolgreichen Schritts |
| `dt()`, `maxNormalError()`, `maxProjectionError()` | Letzte Schrittweite und sichtbare Solverreste, SI |
| `body(index)`, `bodies()` | Körperwert beziehungsweise unabhängiger Arraywert |
| `contactIdA(index)`, `contactIdB(index)` | Stabile Collider-IDs in kanonischer Reihenfolge |
| `contactPoint(index)`, `contactNormal(index)`, `contactPenetration(index)` | Geometrie vor Positionsprojektion |
| `localAnchorA(index)`, `localAnchorB(index)` | Beide vor Projektion gespeicherten Körperlokalanker |
| `contactImpulse(index)` | Gesamter Impuls auf A einschließlich Warmstart, N s |

Indizes sind nullbasiert und werden geprüft. Leere Konstruktor-/Resetzustände
melden null Anzahlen, Schrittweite und Reste; Körper-/Kontaktzugriff bleibt
außerhalb dieser Anzahlen ein Laufzeitfehler. Der Sprachzustand benötigt eine
begrenzte Allokation pro erfolgreichem Snapshot innerhalb des gemeinsamen
64-MiB-Budgets. Dauerhaft gespeicherte Verläufe zählen zu diesem Budget.
Der C-Core selbst bleibt ohne Heap; Rechen- und Kapazitätsgrenzen sind identisch.

`ContactSolver.solveWarm(bodies, contacts, initialImpulses)` bindet den manuellen
C-Kontaktgraphen mit `[ContactConstraint]` und `[Vec3]`. Genau ein endlicher
Seed pro Kontakt ist erforderlich; das Ergebnis ist ein `ConstraintResult`
mit null Gelenken und totalen Kontaktimpulsen. Normalseeds werden auf nichtnegative
Impulse und Tangentenseeds auf den aktuellen Reibungskegel projiziert.
Restitution bezieht sich auf die Geschwindigkeiten vor sämtlichen Startimpulsen.
Die bisherige `solve()`-Bindung bleibt unverändert.

[Werte-/Besitzbeispiel](../examples/language/contact_world.phys) und
[Physim-Stapel](../examples/language/contact_stack.phys) sind ausführbar.
Der Stapel verwendet dieselben Parameter, 11 Messkanäle und vollständigen
Szenen wie das C-Beispiel. Ein unabhängiger Parser vergleicht beide Modi über
je 1001 Messungen und alle aufgezeichneten Objektfelder mit 1e-12 relativer/
absoluter Toleranz; Quaternionnormalisierung kann letzte Bits verändern.
