# CRC32: kompatible Beschleunigung

`ps_crc32` berechnet die bestehende Prüfsumme jetzt mit einer unveränderlichen
Tabelle aus 256 UInt32-Werten (1 KiB). Pro Eingabebyte ersetzt ein Tabellenzugriff
die bisherigen acht Bitschritte. Es gibt keine Allokation, laufzeitabhängige
Initialisierung oder Hardwarevoraussetzung. Das reflektierte Polynom bleibt
`0xedb88320`, Startwert und abschließendes XOR bleiben `0xffffffff`.
Ein leerer Bereich ergibt weiterhin null; nur dann darf der Datenzeiger NULL sein.

Dateiformate, ABI, CRC-Bytefolge und Flush-/Recovery-Regeln bleiben unverändert.
Die Funktion wird für Run-/Reportdateien, PNG, Einstellungen, Autosave und
Änderungserkennung im Editor verwendet.

## Kompatibilitätsnachweise

- 31 eingefrorene Referenzen aus Python `zlib.crc32`, bis 8 MiB, plus `123456789`.
- Alle 256 Einzelbytewerte, 32 Startausrichtungen mit allen Längen bis 512 Byte
  und 1.000 weitere deterministische Bereiche gegen eine bitweise Referenz.
- Jeder einzelne Bitfehler in einem 136-Byte-Sample verändert die Prüfsumme.
- Ein vor der Änderung erzeugter Analysebericht wird im dauerhaften Test
  `crc_report_roundtrip` geladen und bytegleich neu geschrieben. Herkunft und
  SHA-256 stehen unter `tests/fixtures/README.md`.
- Beim Pendel mit RK4, dt 0,005 s und Seed 42 sind Schema, sämtliche 1.025 Samples
  sowie Abschlussblock einschließlich CRC vor/nach der Änderung bytegleich.
  Die Modul-Fingerabdrücke und Runner-Bauzeit ändern sich erwartungsgemäß.
- Alter und neuer Reader exportieren den neuen Lauf in dieselbe CSV. Fünf
  PNG-Exporte des Plot-Workflows bleiben bytegleich.

Lokal bestanden 52 Release-Tests, sieben gezielte Clang-Debug-Tests sowie beide
CRC-Tests mit MSVC AddressSanitizer. Vorhandene Tests für abgeschnittene und
verfälschte Daten, Berichte, Autosaves, Einstellungen und PNG bleiben aktiv.
Der Selbsttest aus der portablen Installation bestand ebenfalls: ein neues
Experiment mit Sensorunsicherheit aus dem mitgelieferten SDK bauen, einen
absichtlichen Compilerfehler korrigieren, simulieren, auswerten, SVG/CSV
exportieren und das gespeicherte Projekt samt Ergebnissen erneut öffnen.
Die CI baut die neuen Tests auf Windows und Linux; ein neuer Linux-Lauf wurde
auf diesem Windows-Rechner nicht ausgeführt.

## Messung vom 19. September 2026

Windows 11, Xeon W-2123, MSVC 19.38.33145.0 Release; fünf Wiederholungen ohne
parallele Builds/Tests. `physim-crc-tests --benchmark` verarbeitet je Fall rund
16 MiB mit wiederholten Aufrufen auf warmen Daten. Median-Durchsatz:

| Eingabelänge pro Aufruf | Vorher | Nachher |
| --- | ---: | ---: |
| 56 Byte | 75,279 MiB/s | 404,594 MiB/s |
| 136 Byte | 76,240 MiB/s | 365,752 MiB/s |
| 8.192 Byte | 77,916 MiB/s | 356,074 MiB/s |
| 1 MiB | 77,342 MiB/s | 384,093 MiB/s |

Das ist ein CRC-Mikrobenchmark. Der [vollständige Datenbenchmark](performance.md)
mit 100.000 Samples und 16 Kanälen zeigt zusätzlich:

| Arbeit | Vorher | Nachher |
| --- | ---: | ---: |
| Schreiben einschließlich Flush/Abschluss | 1,636317 s | 1,455902 s |
| Vollständiges Lesen und Prüfen | 0,247601 s | 0,102753 s |
| Analyse-Snapshot erstellen | 0,301329 s | 0,138743 s |

Damit sinkt die gemessene Lesezeit um rund 58 Prozent; der Gewinn beim Schreiben
ist wegen der unveränderten Dateisynchronisation kleiner. Dies sind lokale
Messungen, keine Zusage für andere Rechner. Alle acht Datenbenchmark-Fälle
bestanden die bestehende 20-Prozent-Regressionsschwelle. Akzeptanzziel ist
unveränderte Kompatibilität und wenigstens verdoppelter CRC-Durchsatz auf der
Referenzmaschine; beide Kriterien sind erfüllt.

Rohdaten, Metadaten und Vergleich:
[CRC-Benchmark](benchmarks/2026-09-19/crc/crc-summary.json).

```powershell
cmake --build build --config Release --target physim-crc-tests
ctest --test-dir build -C Release -R crc --output-on-failure
build/bin/physim-crc-tests.exe --benchmark
python tools/benchmark.py build/bin/physim-benchmark.exe --output build/crc-results
```
