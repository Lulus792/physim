# UI-Zeichenpuffer und Messung

Der OpenGL-Renderer hält seine Vertex- und Indexpuffer jetzt am Grafik-Kontext.
`ps_ui_geometry` beginnt ohne Allokation, legt bei der ersten Konvertierung zwei
kleine Puffer an und vergrößert sie bei Bedarf. Die nächste Konvertierung verwendet
dieselben Speicherbereiche. Kleinere Ansichten geben die Reserve nicht pro Bild
wieder frei. Beim Schließen des Grafik-Kontexts wird alles freigegeben.

Die Puffer werden Nuklear als feste Speicherbereiche übergeben. Reicht einer nicht,
wird ausschließlich der betroffene Bereich vergrößert und die Konvertierung mit
leeren Ausgabepuffern wiederholt. Das vermeidet Nuklears dynamischen
Reallokationspfad, der bei Speichermangel einen Assert auslösen und den bisherigen
Speicherzeiger verlieren kann. Änderungen an der eingebundenen Bibliothek sind
nicht nötig. Konvertierungsfehler werden vor dem Zeichnen zurückgemeldet.

Es gilt ein gemeinsames Limit von 64 MiB für die vorgehaltene Vertex-/Indexkapazität.
Beim Vergrößern existieren vorübergehend alter und neuer Bereich. Der separate
Nuklear-Befehlspuffer, die Quellbefehle, Schriften und Treiberspeicher gehören nicht
zu diesem Limit. Die Grafikschicht bleibt an einen Thread und dessen GL-Kontext
gebunden. Wiederholte Konvertierung setzt voraus, dass benutzerdefinierte
Nuklear-Zeichen-Callbacks keine Seiteneffekte auslösen; Physim verwendet solche
Callbacks nicht.

GPU-Pufferobjekte werden weiterhin wiederverwendet. Pro Übertragung reserviert
`glBufferData(..., NULL, GL_STREAM_DRAW)` einen neuen Speicherbereich für das
vorhandene Objekt (Orphaning); `glBufferSubData` überträgt nur die aktuell genutzten
Bytes. Ausstehende Draw-Aufrufe dürfen dadurch den bisherigen Bereich behalten.
Dies ist OpenGL 3.3 ohne Erweiterungen. Es ist keine Garantie für einen bestimmten
Treiber, intern allokationsfrei zu bleiben oder niemals auf die GPU zu warten.

PNG-Exporte teilen sich diese Konvertierungspuffer mit dem Fenster. Jeder Aufruf
beginnt mit leeren Ausgabebefehlen und füllt Vertex-/Indexdaten neu. Der Export
ändert weder die wissenschaftlichen Daten noch die nächste Fensterdarstellung.

## Wiederholbare Messung

```powershell
cmake -S . -B build-ui-perf -DPHYSIM_BUILD_APP=ON -DPHYSIM_BUILD_BENCHMARKS=ON -DPHYSIM_GRAPHICS_TESTS=ON
cmake --build build-ui-perf --config Release --parallel
python tools/ui_benchmark.py build-ui-perf/bin/physim-ui-benchmark.exe --output build-ui-perf/results
```

Der neue Ausgabeordner muss noch nicht existieren. Python benötigt nur die
Standardbibliothek. Unter Linux den Release-Build mit `-DCMAKE_BUILD_TYPE=Release`
konfigurieren und `.exe` weglassen. Dort kann der Aufruf unter Xvfb/Openbox erfolgen,
wie die bestehenden Grafiktests. Der native Aufruf
`physim-ui-benchmark neuer-ordner` funktioniert auch ohne Python.

Die Messfälle verwenden ein unsichtbares 1080 × 740 großes SDL-Fenster, die
mit Nuklear gelieferte Standardschrift, deaktiviertes VSync und je zehn
Aufwärmbilder plus 300 protokollierte Bilder:

- **empty:** leeres Fenster mit Hintergrund;
- **dense:** 108 beschriftete Schaltflächen;
- **plot:** acht Linien mit jeweils 2.048 Punkten, mit Clipping und Kantenglättung.

`frames.csv` enthält CPU-Zeiten für Konvertierung, GPU-Upload-Aufrufe und den
gesamten Aufruf von `nk_sdl_render`, außerdem erfolgreiche Vertex-/Indexallokationen,
übertragene Bytes und vorgehaltene Kapazität. Der Wrapper schreibt Median, P95/P99
(Nearest-Rank), Compiler-/GL-Protokoll, Hostdaten, Binär- und Quellfingerabdrücke.
Zeitmessungen schließen Widgetaufbau, Swap/VSync und GPU-Fertigstellung aus.
Sie messen weder vollständige App-Framezeit noch Eingabelatenz.

Für jedes Messbild nach dem Aufwärmen müssen die Vertex-/Indexallokationen null
sein. Die Bilder am Anfang und Ende jedes Falls müssen bytegleich sein. Nach
einem PNG-Export aus einem zweiten Nuklear-Kontext und nach einem Größenwechsel
auf 640 × 480 und zurück muss die leere Ansicht wieder bytegleich erscheinen.
Bei Abweichungen schlägt der Lauf fehl und erhält seine Diagnoseartefakte.

## Lokale Vorher-/Nachher-Messung, 19. September 2026

Windows 11 (10.0.26200), Xeon W-2123, MSVC 19.38.33145.0 Release,
NVIDIA RTX 2080 Ti, OpenGL 3.3, Treiber 595.79. Keine parallelen Builds/Tests
während der Messläufe. Die Baseline verwendet den bisherigen Konvertierungspfad
mit zusätzlichen Allokationszählern. Medianwerte:

| Fall | Pufferallokationen pro Bild vorher → nachher | Konvertierung vorher → nachher | Renderaufruf vorher → nachher |
| --- | ---: | ---: | ---: |
| empty | 2 → 0 | 0,0045 → 0,0026 ms | 0,0656 → 0,0559 ms |
| dense | 14 → 0 | 1,9163 → 1,2153 ms | 2,2736 → 1,3863 ms |
| plot | 19 → 0 | 16,8560 → 10,9270 ms | 18,3110 → 11,6795 ms |

Die dichte und die Diagrammansicht benötigen in dieser Messung etwa 36 Prozent
weniger Konvertierungszeit. Alle sechs Bilder vor/nach der Änderung sind bytegleich.
Die vorgehaltene CPU-Kapazität beträgt 8 KiB, 512 KiB bzw. 3 MiB. Der Median der
Upload-Aufrufe bleibt ungefähr gleich. Ein pauschaler FPS-Gewinn wird daraus
nicht abgeleitet. Die Änderung erfüllt das Ziel, im stabilen Konvertierungspfad
keine Allokationen zu erzeugen und die Medianzeit des Renderaufrufs gegenüber
der Baseline nicht um mehr als 20 Prozent zu erhöhen.

Rohdaten und Metadaten: [UI-Puffer-Benchmark](benchmarks/2026-09-19/ui-buffers/summary.json).
Die Wiederholungsgrenzen sind absichtlich sichtbar: Ein einzelner lokaler
Vorher-/Nachher-Vergleich ersetzt weder Plattformmessungen noch langfristige
Leistungsüberwachung. App-Startzeit, interaktive Eingabelatenz, GPU-Zeiten,
Szene-Tessellierung und das gesamte RAM-Budget bleiben offene Messstrecken.

## Regressionstests

`ui_geometry` vergleicht alle Vertex-/Indexbytes und die Reihenfolge, Texturen und
Clip-Rechtecke der Zeichenbefehle gegen Nuklears bisherigen dynamischen Pfad.
Leere Befehlslisten, kleine/große/kleine Ansichten, Text und mehr als 65.535 Vertices
sind abgedeckt. Wiederholte Konvertierung darf keine zusätzlichen Allokationen
erzeugen. An allen vier Allokationsstellen des Referenzablaufs wird Speichermangel
injiziert; derselbe Besitzer muss anschließend erfolgreich weiterarbeiten und
bytegenau alles freigeben. Ein ausgeschöpftes Budget muss ebenfalls eine spätere
kleine Ansicht zulassen. Der Test benötigt kein Fenster.

`ui_rendering` führt mit `--smoke` die beschriebenen GPU-Prüfungen mit sechs
Messbildern je Fall aus. Er trägt die Labels `display` und `benchmark`, benötigt
Python sowie einen OpenGL-3.3-Kontext und läuft seriell mit den übrigen Grafiktests.
Die Linux-CI-Konfiguration startet ihn deshalb unter Xvfb. Eine erfolgreiche
lokale Windows-Prüfung ist kein Nachweis einer ausgeführten Linux-CI.
