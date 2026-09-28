# Reproduzierbare Leistungsmessung

Weitere Messungen: [UI-Zeichenpuffer](ui-rendering.md) und
[CRC32 mit unverändertem Dateiformat](crc.md).

`physim-benchmark` misst produktive C-APIs mit deterministischen Referenzdaten.
Der Benchmark verändert keine Simulations-, Speicher- oder Recovery-Einstellungen.
Er ist optional und benötigt keine Oberfläche oder zusätzlichen C-Bibliotheken.

```powershell
python tools/build.py --no-app --benchmarks --config Release --build-dir build-perf --test --test-filter 'benchmark_*'
python tools/benchmark.py build-perf/bin/physim-benchmark.exe --output build-perf/results --samples 100000 --repeats 5
```

Unter Linux und macOS dieselben Befehle mit `python3` statt `python` und den
Programmnamen ohne `.exe` verwenden. Der direkte Build benötigt Python 3.10+
und einen C17-Compiler; SDL, CMake und Ninja werden nicht benötigt.
Der Python-Wrapper verwendet nur die Standardbibliothek. Nach dem Build funktioniert
`physim-benchmark 100000 5` direkt; es schreibt CSV auf stdout und Diagnosen auf stderr.

## Messfälle und Referenzen

| Messfall | Arbeit und Prüfung |
| --- | --- |
| write_16_channels | Erstellen, Schreiben und Finalisieren eines Runs mit 16 Kanälen; bestehende Flush-Regeln bleiben aktiv |
| read_validate_16_channels | Alle Zeiten und Kanalwerte sowie endgültige Samplezahl und Abschlussmarker prüfen |
| analysis_snapshot | Vollständigen Run in den diskgestützten Analysekontext laden |
| series_statistics | Anzahl, Min/Max und Mittelwert einer linearen Referenzreihe prüfen |
| series_derivative | Ableitung über Blockgrenzen; sämtliche abgeleiteten Werte müssen 16 ergeben |
| report_preview_8_curves | Acht reduzierte Linien erzeugen; Quelle, Punktlimit und Endpunkte prüfen |
| report_curve_copy | 80.000 kopierende Kurvenzugriffe; jeweils Endwert konsumieren |
| report_curve_view | Dieselben Zugriffe ohne Kopie; vorher alle Kurven bytegleich gegen die Kopie prüfen |

Die Zeitachse ist `i/8` Sekunden, Kanal `c` enthält `i*(c+1)/8` Meter.
Diese Werte sind im erlaubten Bereich binär exakt darstellbar. 257 Samples prüfen
im CI-Smoke einen Blockübergang; 100.000 Samples erzeugen reduzierte Vorschauen.
Für den großen Dateifall kann `--samples 1000000` verwendet werden.

Der native Benchmark legt ausschließlich neue Arbeitsordner und Dateien an.
Nach Erfolg entfernt er die eigenen Run-Dateien; Analyse-Scratchdateien werden
beim Schließen entfernt. Leere Arbeitsordner bleiben erhalten. Bei Fehlern bleiben
Run-Dateien als Diagnosematerial erhalten. Existierende Ergebnisordner des Wrappers
werden abgewiesen. Ein nicht-null Exitcode bedeutet einen fehlgeschlagenen Lauf,
eine ungültige Eingabe oder einen fehlgeschlagenen Vergleich.

## Auswertung und Vergleich

Der Wrapper archiviert Rohdaten, stderr, Median/Minimum/Maximum, Scratchvolumen,
OS/CPU-Identifikation, Compiler, Buildkonfiguration sowie den SHA-256-Fingerabdruck
der ausführbaren Datei. Zusätzlich erfasst er Hashes des aktuellen Quellstands;
diese ersetzen keinen Buildnachweis. Vor einer Abnahmemessung stets neu bauen.
Die Compilerkennung kommt unmittelbar vom übersetzenden Compiler. Die neue
Kennung kann von älteren CMake-Messungen abweichen; in diesem Fall eine neue
Baseline erstellen. Quellpfade in den Metadaten verwenden auf allen Plattformen `/`.
Alle Wiederholungen einschließlich der ersten bleiben enthalten. Es werden keine
OS-Caches geleert; die Leser laufen nach dem Schreiben mit typischerweise warmem Cache.

```powershell
python tools/benchmark.py build-perf/bin/physim-benchmark.exe --output build-perf/candidate --baseline build-perf/results --max-regression 0.20
```

Der Vergleich verlangt passende Plattform, CPU, Samplezahl, Compiler und
Konfiguration. Bei mehr als 20 Prozent längerer Medianlaufzeit eines bestehenden
Messfalls schlägt er fehl und bewahrt die Ergebnisse auf. Neue Messfälle werden
weiter protokolliert, fehlende alte Messfälle werden abgewiesen. Diese Schwelle
ist ein Untersuchungsanlass; Hintergrundlast, Datenträger und Energiesparzustand
müssen zusätzlich gleich gehalten werden. Keine Builds oder Tests gleichzeitig
mit einer Abnahmemessung ausführen. Debug-/Sanitizer-Zeiten sind keine Release-Baseline.

Für die Kurvenoptimierung gilt: bytegleiche Inhalte, unveränderte Exporte und
Diagramme, keine zusätzliche Allokation und mindestens halbierte Dauer des
gemessenen Kurvenzugriffs gegenüber der Kopie. Das ist ein API-Mikrobenchmark;
er misst weder vollständige UI-Framezeit noch FPS. Die Oberfläche hält die
Views nur während eines Zeichenaufrufs. Lebensdauer und Synchronisation stehen
in der [Berichtsreferenz](reference/report.md) und unter [Berichte](reports.md).

## Noch offene Messabdeckung

PERF-001 ist damit teilweise umgesetzt. Ein zusätzlicher
[UI-Benchmark](ui-rendering.md) misst inzwischen den CPU-Konvertierungs-/Uploadpfad
und prüft wiederverwendbare Zeichenpuffer. Native Szenentessellierung/GPU-Zeiten,
UI-P95/P99 bei echter Interaktion, Startzeit, Peak-RAM und Acht-Worker-Batches
benötigen eigene Messstrecken. `scratch_bytes` misst den logischen Payload des
Analysekontexts, nicht Peak-RAM oder physisch belegte Datenträgerblöcke.
Der Benchmark-Smoke wird in der Windows-/Linux-CI konfiguriert; ein lokal
ausgeführter Windows-Test ist kein Nachweis eines Linux-CI-Laufs.

## Lokale Referenz vom 19. September 2026

Windows 11 (10.0.26200), Intel Xeon W-2123, acht logische CPUs,
MSVC 19.38.33145.0, Release. Je fünf Wiederholungen mit 100.000 Samples,
ohne parallele Builds oder Tests. Medianwerte:

| Messfall | Sekunden |
| --- | ---: |
| Schreiben, 16 Kanäle | 1,672774 |
| Vollständiges Lesen und Prüfen | 0,251891 |
| Analyse-Snapshot | 0,287752 |
| Statistik | 0,012371 |
| Ableitung | 0,018805 |
| Acht reduzierte Berichtskurven | 0,227496 |
| 80.000 Kurvenkopien | 0,121473 |
| 80.000 Kurven-Views | 0,000473 |

Die View benötigt in diesem API-Mikrobenchmark rund 0,4 Prozent der Kopierzeit.
Die Halbierungsschwelle ist erfüllt. Daraus folgt kein entsprechender Faktor
für die gesamte Oberfläche. Der Analyse-Scratch-Payload beträgt 13.600.000 Bytes
nach dem Laden und 14.400.000 Bytes nach der Ableitung.
Die bestehenden Messfälle bestanden den 20-Prozent-Vergleich gegen die zuvor
isoliert gemessene Baseline. Rohdaten und Metadaten liegen unter
[benchmarks/2026-09-19/report-view](benchmarks/2026-09-19/report-view/summary.json).
Die vorherige Messung ist unter
[baseline](benchmarks/2026-09-19/baseline/summary.json) archiviert.
Der portable App-Test bestätigt zusätzlich fünf bytegleiche PNG-Exporte und
pixelgleiche Diagrammbereiche in elf Screenshots gegenüber dem bisherigen Build.
Projektname und Statuszeile liegen außerhalb des verglichenen Diagrammbereichs.
