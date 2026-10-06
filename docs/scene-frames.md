# Szenen mit lokalen Koordinaten

Ein Koordinatenrahmen beschreibt Translation in Metern, Quaternionrotation und
dimensionslose Skalierung. Seine Nachfahren verwenden lokale Geometriedaten.
Die App setzt die Rahmen vom äußeren zum inneren zusammen und verwendet dieselben
Weltkoordinaten für Meshes, Transparenzsortierung, Picking und Label-Anker. Ein
Rahmen besitzt keine eigene Geometrie. Gruppen organisieren weiter nur Einträge;
auch Körper, die als Eltern dienen, verschieben ihre Kinder nicht zusätzlich.

```c
ps_scene_frame(scene, 100, 0, "Versuch",
               ps_v3(1, 0, 0), ps_quat_axis_angle(ps_v3(0, 0, 1), PS_PI / 2),
               ps_v3(2, 1, 1));
ps_scene_add_id(scene, 1, PS_SPHERE, ps_v3(.25, 0, 0), ps_v3(.25, 0, 0), .1, UINT32_MAX);
ps_scene_set_parent(scene, 1, 100);
```

```text
sceneFrame("Versuch", 100, 0, Vec3(1,0,0),
           Quat.axisAngle(Vec3(0,0,1), 1.5707963267948966), Vec3(2,1,1))
sphere(Vec3(0.25,0,0), 0.1, 0xffffffff, 1)
sceneParent(1,100)
```

Das Kugelzentrum liegt in diesem Beispiel bei Weltposition `(1, .5, 0)`.
Die nichtuniforme Skalierung erzeugt ein Ellipsoid. Komposition folgt `T * R * S`;
jeder Rahmen skaliert zuerst, rotiert dann und verschiebt zuletzt. Negative
Skalierungen spiegeln die Geometrie. Verschachtelte nichtuniforme Skalen mit
Rotation können Scherung erzeugen; der Renderer transformiert deshalb vollständige
Mesh-Vertices und ihre Normalen statt nur Zentren und Radien.

Alle Felder müssen endlich sein; die Quaternion und jede Skalierungskomponente
müssen von null verschieden sein. Die Quaternion wird im Konstruktor normalisiert.
Ein nicht darstellbares oder numerisch singuläres zusammengesetztes System wird
abgewiesen. Geprüfte Konstruktoren und Reparenting lassen die Szene bei Fehlern
unverändert. Fehlende Eltern, doppelte IDs und Zyklen bleiben ungültig. Vorfahren
müssen vor einem neuen Eintrag existieren; validierte Snapshots dürfen beliebig
umgeordnet sein. Die Grenze bleibt 32 Einträge einschließlich Rahmen und Gruppen.

`ps_scene_transforms` liefert ein Local-to-world-Array in Szenenreihenfolge, ohne
Speicherallokation oder gemeinsamen Cache. Bei einem Rahmen enthält dessen Matrix
auch seine eigene TRS; bei Geometrie enthält sie nur die Rahmen seiner Vorfahren,
da der Renderer eine objektspezifische Box-/Ebenenrotation selbst aufbaut.
`ps_scene_world_point` konvertiert einen Punkt in dieser Basis. Physim bindet die
Abfragen als `sceneTransform(index)` und `sceneWorldPoint(index, point)` im
`scene`-Callback; der Szenenslot beginnt bei 0 und folgt der Reihenfolge der
angelegten Einträge. Beide C-Abfragen lassen
Ausgaben bei Fehlern unverändert. Normalen verwenden inverse Transposition der
zusammengesetzten linearen Basis. Die Rendergrenze von 1e12 m wird nach der
Umrechnung angewendet; das Kameraclipping bleibt 0,02 bis 1000 m.

Polyline-Punkte bleiben unverändert im gemeinsamen lokalen Punktpool. Mehrere
Objekte können dieselben Punkte unter verschiedenen Rahmen darstellen.
Sichtbarkeit wird unabhängig von den Koordinaten vererbt. Der Inspektor zeigt
Welt- und Lokalposition der Auswahl sowie die Skalierung eines Rahmens.

## Bindungen, Aufzeichnung und Kompatibilität

C-Module kündigen `PS_EXPERIMENT_SCENE_HIERARCHY | PS_EXPERIMENT_SCENE_FRAMES` an.
Ohne Frame-Capability werden veröffentlichte Rahmen abgewiesen. Die eigene Sprache
bindet `sceneFrame` ab 0.174.0 und prüft den `scene`-Callback zur Laufzeit. Die
[vollständigen C-](../examples/scene_frames/main.c) und
[Physim-Beispiele](../examples/language/scene_frames.phys) enthalten alle acht
Geometrieformen unter verschachtelten Rahmen.

API/ABI 3 und die Größen/Offsets von `ps_object` und `ps_scene` bleiben erhalten.
Die neue Form `PS_FRAME=9` verwendet bestehende Felder: `a` Translation, `b` Scale,
`orientation` Rotation. Snapshotversion 3 bewahrt diese lokalen Werte;
Versionen 1 und 2 werden weiterhin gelesen und behalten ihre Weltkoordinaten.
Version 2 akzeptiert die neue Form ausdrücklich nicht. IPC 5 grenzt die neue
Szeneninterpretation ab; ältere Pipe-Versionen werden abgewiesen. Messdateiformat 1
und aufgezeichnete Messkanäle bleiben erhalten. Wiederöffnung führt keinen
Experimentcode aus. Details stehen im [Dateiformat](data-format.md).

Die Rahmen ändern ausschließlich die Szenendarstellung. Sie setzen keine
physikalischen Kräfte, Constraints oder Kanalwerte und rechnen Messdaten nicht um.
