# Messung des 3D-Szenenrenderers

`physim-scene-benchmark` ruft den produktiven Szenenrenderer auf. Er misst
Vorbereitung, Geometrieaufbau und OpenGL-Submission getrennt. Der normale
Appbetrieb erzeugt keine GPU-Zeitabfragen und wartet nicht auf deren Ergebnisse.

```sh
python3 tools/build.py --config Release --benchmarks --sdl /pfad/zu/SDL --build-dir build-scene
python3 tools/scene_benchmark.py build-scene/bin/physim-scene-benchmark --output build-scene/results
```

Unter Windows `python` und `physim-scene-benchmark.exe` verwenden. Der neue
Ergebnisordner darf nicht existieren. `--smoke` verkürzt die Messreihe auf sechs
statt 60 Bilder je Fall; fünf Aufwärmbilder bleiben unprotokolliert. `--no-gpu`
deaktiviert die Zeitabfrage ausdrücklich. Der native Aufruf funktioniert ohne
Python: `physim-scene-benchmark neuer-ordner [--smoke] [--no-gpu]`.

## Referenzlasten

Die vier unveränderlichen Szenen verwenden ein unsichtbares SDL-Fenster mit
640 × 480 Pixeln, deaktiviertem VSync, fester Perspektivkamera und ohne Gitter:

| Fall | Inhalt | Erwartete Vertices | Übertragene Indexbytes |
| --- | --- | ---: | ---: |
| empty | leere Szene, Hintergrund und Framebuffer-Resolve | 0 | 0 |
| spheres | 32 Kugeln mit je 24 × 16 × 6 Vertices | 73.728 | 0 |
| translucent | 31 Boxen mit gemischter Deckkraft unter einem rotierten, nichtuniform skalierten Elternframe | 1.116 | 4.464 |
| path | 96-Punkte-Polyline und ein Pfeil, einschließlich Rohrflächen und Endkappen | 18.528 | 0 |

Native Prüfungen vergleichen tatsächliche Vertex-/Indexmengen und übertragenes
Vertexvolumen gegen diese Referenzen. Die früheren Rendererprüfungen für Tiefe,
Transparenz, Picking, Projektion und Größenwechsel bleiben separat erforderlich.
Die aufgenommenen Bilder vor/nach jeder Messreihe müssen bytegleich sein; alle
vier Szenen müssen unterschiedliche Bilder erzeugen. UI-Komposition, Aufnahmen
und Swap liegen außerhalb der gemessenen Szenenintervalle.

## CPU-Zeiten und Speicher

`setup_seconds` umfasst Validierung, Renderziel-/Framebufferbereitstellung,
GL-Zustand und Clear-Submission. `tessellation_seconds` umfasst Elterntransforms,
Normaltransforms, primitive Dreiecke, Pickingbereiche und Kameramatrix.
`submission_seconds` umfasst Vertexupload, transparente Sortierung und Indexupload,
Draw-Aufrufe, Multisample-Resolve, Fehlerprüfung und inverse Pickingmatrix.
Diese CPU-Wandzeiten messen keine GPU-Fertigstellung. Die äußere `total_seconds`
schließt zusätzlich Statistik-/optionale Query-Aufrufe ein. Ihre Aufteilung ist
keine Messung isolierter Hardwarefunktionen.

`vertex_bytes` und `index_bytes` bezeichnen logisch übertragene Geometriebytes.
`retained_bytes` zählt die drei dynamischen CPU-Geometriepuffer einschließlich
ungenutzter Reserve. Grafikstruktur, eingebettetes Kugelmesh, UI-Puffer,
Framebuffer-/Treiber-/GPU-Speicher gehören nicht dazu. Die Vertexreserve beträgt
24.000.000 Bytes; Alpha-/Indexreserve wächst bei Bedarf und bleibt danach
erhalten. Getrennte Kapazitäten berücksichtigen auch teilweise erfolgreiche
Reservevergrößerungen. Ungültige Szenen oder fehlgeschlagene Renderaufrufe
invalidieren die Statistik; ein fehlgeschlagener Getter erhält seinen Output.

Benutzer-/System-CPU-Zeit und Prozess-Peak-RAM verwenden denselben
[Ressourcenvertrag](performance.md#prozessressourcen-und-logischer-datendurchsatz)
wie der Datenbenchmark. Sie umfassen alle Prozessthreads; Peak ist der bisherige
Höchstwert einschließlich früherer Lasten und Aufnahmen, kein Szenenbudget.

## Optionale OpenGL-Zeitabfrage

Der Benchmark prüft Funktionen und Zählerbreite für
[GL_TIME_ELAPSED](https://wikis.khronos.org/opengl/GLAPI/glBeginQuery).
Eine verfügbare Abfrage umschließt den gesamten Szenenaufruf. `gpu_seconds` ist
das verstrichene OpenGL-Serverintervall, einschließlich möglicher Leerlauf-/CPU-/
Treiberstalls innerhalb dieser Befehlsfolge; es ist keine Shaderauslastung und
keine reine Rasterisierungszeit. Es schließt UI-Komposition, Bild-Readback und
Swap aus. Auch ein Software-Renderer kann dieses Serverintervall liefern.

Der Benchmark sammelt jedes Ergebnis nach dem CPU-Intervall und vor dem nächsten
Bild. Diese absichtliche Serialisierung vermeidet Zuordnungsfehler, verändert aber
das Pipelineverhalten gegenüber dem Appbetrieb. `query_wait_seconds` enthält
Flush, Bereitschaftsabfragen und gegebenenfalls Millisekundenpausen; es wird
separat ausgewiesen. Nach fünf Sekunden ohne Ergebnis oder bei drohendem
Zählerüberlauf schlägt der Lauf fehl und erhält die Diagnoseartefakte.
Die Verfügbarkeit wird über
[Zählerbits](https://wikis.khronos.org/opengl/GLAPI/glGetQuery) und
[Ergebnisbereitschaft](https://wikis.khronos.org/opengl/GlGetQueryObject) geprüft.

Fehlende Funktionen, nicht nutzbare Zähler oder `--no-gpu` deaktivieren ausschließlich
die Zeitabfrage. CPU-/Geometrie-/Bildprüfungen laufen weiter. Die CSV-Zeit bleibt
leer, JSON verwendet `gpu_available=false` und `gpu_seconds=null`. Ein fehlender
Messwert wird nicht als null Sekunden ausgegeben. OpenGLfehler während einer
aktivierten Abfrage sind ein fehlgeschlagener Lauf.

## Nachweise und Grenzen

Der Wrapper erhält CSV, stdout/stderr, Binär-/Quellfingerabdrücke und Metadaten.
Median/P95/P99 verwenden Nearest-Rank. Er weist fehlende/doppelte Bilder, ungültige
Zeitwerte, widersprüchliche Verfügbarkeit, wechselnde Geometriemengen,
rückläufige Prozessspitzen und veränderte Referenzbilder ab. Der direkte Build
bindet beide Smokes `scene_rendering` und `scene_rendering_no_gpu` ein.

Keine Builds/Tests gleichzeitig mit einer Referenzmessung ausführen. Die vier
statischen Lasten ersetzen keine vollständigen App-/Runner-/Mehrworkerprofile,
Startzeitmessung, reale Interaktionslatenz oder GPU-Auslastungsanalyse.
Die ausgeführten Umgebungen stehen im [Plattformnachweis](platform-validation.md).


## Lokale Referenz, 8. Oktober 2026

Je 60 Messbilder nach fünf Aufwärmbildern, Release, ohne parallele Builds/Tests.
Die OpenGL-Zeitabfrage ist für diese Reihe aktiviert und wird je Bild gesammelt.
Mediane:

| Fall | macOS Tessellierung ms | macOS Serverintervall ms | Debian Tessellierung ms | Debian Serverintervall ms |
| --- | ---: | ---: | ---: | ---: |
| spheres | 1,513 | 0,433 | 0,969 | 14,814 |
| translucent | 0,451 | 0,253 | 0,467 | 4,491 |
| path | 0,798 | 0,276 | 0,845 | 7,179 |

Intel macOS 14.6.1/Apple Clang 16 verwendet Intel Iris Plus Graphics 655,
OpenGL 4.1 und einen 32-Bit-Zähler. Debian 12/GCC 12.2 läuft in einer VM mit
Mesa 22.3.6/llvmpipe und einem 64-Bit-Zähler. Dessen Serverintervall misst den
Software-Renderer; die Tabelle ist kein Vergleich nativer GPUs. Die leere Szene
kann bei verfügbarer Abfrage einen tatsächlichen Nullwert liefern; dieser ist
vom fehlenden Messwert bei deaktivierter Abfrage getrennt.

Die vollständige Wiederholung mit `--no-gpu` besteht ebenfalls auf beiden
Systemen. Alle vier Bildhashes bleiben je Plattform zwischen aktivierter und
deaktivierter Abfrage gleich. Wegen der veränderten Serialisierung sind die
CPU-Zeiten beider Modi getrennte Referenzlasten, keine Optimierungsbehauptung.
Rohdaten bleiben unter `build/scene-profiling-measurements-final-{mac,linux}`
und den jeweiligen `-no-gpu`-Ordnern erhalten; `*-receipt.json` erfasst sämtliche
Ergebnisdateien mit SHA-256. Das macOS-Bild unter
`build/scene-profiling-preview-mac.png` wurde aus den nativen Aufnahmen konvertiert
und visuell geprüft: Kugeln, transformierte Boxen und Polyline/Pfeil sind sichtbar.
