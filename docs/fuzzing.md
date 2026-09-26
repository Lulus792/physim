# Decoder-Mutationsprüfungen

## Was ist der Fuzzing-Build?

Ein Fuzzing-Build ist ein Entwickler-Testprogramm, das Leser mit sehr vielen
veränderten, abgeschnittenen oder ungültigen Eingaben auf Robustheit prüft.
Er berechnet keine anderen Physikmodelle und wird für normale Experimente nicht
benötigt. Fehler sollen reproduzierbar gefunden werden, bevor beschädigte Dateien
oder Prozessnachrichten die App zum Absturz bringen.

Physim hat zwei verschiedene Testwege:

- Die drei normalen CTest-Ziele `protocol_mutations`, `run_mutations` und
  `report_mutations` prüfen festgelegte, reproduzierbare Mutationen und
  Invarianten. Dafür ist keine libFuzzer-Laufzeit nötig.
- `PHYSIM_BUILD_FUZZERS=ON` ergänzt `physim-protocol-libfuzzer`. Clang instrumentiert
  den IPC-Decoder und eine separate Kopie der Kernbibliothek mit libFuzzer und
  AddressSanitizer. libFuzzer nutzt erreichte Codepfade, um weitere Eingaben zu
  erzeugen; AddressSanitizer sucht unter anderem ungültige Speicherzugriffe.
  Dieser optionale Weg betrifft derzeit IPC, nicht die beiden Dateileser.

Das optionale Ziel ist in `CMakeLists.txt` implementiert, wird aber nicht automatisch
als CTest-Langzeitkampagne gestartet. Es benötigt den `clang`-/`clang.exe`-Treiber;
MSVC und `clang-cl` werden für dieses Ziel beim Konfigurieren abgewiesen.

Beispiel für eine separate Clang-/Ninja-Buildumgebung (auf Windows aus einer
für Visual Studio und LLVM eingerichteten Entwicklerkonsole):

```text
cmake -S . -B build-fuzz -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPHYSIM_BUILD_APP=OFF -DPHYSIM_BUILD_FUZZERS=ON
cmake --build build-fuzz --target physim-protocol-libfuzzer
cmake -E make_directory build-fuzz/corpus build-fuzz/artifacts
```

Unter Windows benötigt die erzeugte EXE außerdem die passende LLVM-ASan-DLL im
Suchpfad. Danach lautet ein begrenzter Kampagnenaufruf unter Windows:

```text
build-fuzz/bin/physim-protocol-libfuzzer.exe build-fuzz/corpus -max_total_time=60 -max_len=8192 -artifact_prefix=build-fuzz/artifacts/
```

Unter Linux entfällt `.exe`. Funde werden als Corpus-/Artefaktdateien gespeichert
und können dem Programm zur Reproduktion als einzelne Eingabedatei übergeben
werden. Eine begrenzte Kampagne beweist keine allgemeine Fehlerfreiheit.

**Lokaler Nachweisstand am 2026-09-20:** Die vorhandenen Protokolle in
`build-libfuzzer-evidence/` zeigen einen erfolgreichen Build, aber einen
Laufzeitstartfehler (`interception_win: unhandled instruction`) in der verwendeten
Windows-ASan-Umgebung. Sie belegen keine erfolgreich ausgeführte libFuzzer-Kampagne.
Ein kompatibler Sanitizer-/Toolchain-Lauf und eine abdeckungsgeführte Kampagne
bleiben offen. Dies ist von den bestandenen deterministischen Tests zu trennen.

## IPC-Mutationen

`physim-protocol-fuzz` führt eine deterministische IPC-Kampagne ohne externe
Fuzzing-Laufzeit aus. Sie erzeugt einen gültigen Snapshot mit allen Kanal-,
Objekt- und Punktplätzen sowie UTF-8-Label und Polyline und verpackt ihn als Frame.
Für beide Eingaben prüft sie alle Kürzungen und Einzelbitänderungen sowie je
20.000 Vierbyte-Mutationen mit festem Seed `0x70687973`.

```powershell
cmake --build build --config Release --target physim-protocol-fuzz
ctest --test-dir build -C Release --output-on-failure -R protocol_mutations
```

Die aktuelle Kampagne umfasst 183.462 Eingaben. Sie prüft:

- abgewiesene Snapshots lassen Zeit, Werte, Szene und Status unverändert;
- akzeptierte Snapshots enthalten gültige Szenen und endliche Messwerte;
- erneutes Kodieren/Dekodieren erzeugt eine stabile kanonische Darstellung;
- unvollständige/ungültige Frames verändern die Ausgabeparameter nicht;
- verbrauchte Frames hinterlassen genau die restlichen Bytes und erhöhen die Sequenz;
- falsche Sequenzen sowie übergroße Puffer- und Verbrauchslängen werden abgewiesen,
  einschließlich `UINT32_MAX` und des Additionsüberlaufs beim 20-Byte-Header.

Bei einem verletzten Harness-Invariant wird vor dem Abbruch die aktuelle Eingabe
als `protocol-failure.bin` im Arbeitsordner gespeichert. CTest verwendet dafür den
eigenen Unterordner `build/protocol-fuzz`. Ein Absturz im Decoder kann vor dieser
Speicherung auftreten; Sanitizer-Ausgaben und der feste Seed dienen dann zur
Reproduktion der gesamten Kampagne. Eine gespeicherte Eingabe lässt sich direkt
wiederholen:

```powershell
build/bin/physim-protocol-fuzz.exe build/protocol-fuzz/protocol-failure.bin
```

Das erste Byte wählt den Decoder: ungerade für Snapshot, gerade für Frame. Die
restlichen Bytes sind die Eingabe. Die gleiche Harness-Funktion heißt
`LLVMFuzzerTestOneInput`; das vorhandene Ziel `physim-protocol-libfuzzer` verwendet
sie ohne `PS_FUZZ_STANDALONE`. Build und Kampagnenstatus sind oben getrennt beschrieben.

Die Tests laufen durch ihre CTest-Registrierung auch in der bestehenden CI.
Lokal bestehen MSVC Release, Clang Debug und AddressSanitizer; dies ist keine
Linux-Ausführungsabnahme.

## Gespeicherte Läufe

CTest `run_mutations` baut eine gültige Datei mit drei Kanälen und drei Messpunkten
über die öffentliche Writer-API auf. Die Kampagne prüft alle Kürzungen und
Einzelbitänderungen, extreme Chunklängen sowie Nutzdatenänderungen mit neu
berechneter CRC. Letztere erreichen die semantischen Prüfungen hinter der
Prüfsummenkontrolle. Ein gezielt injizierter NaN im letzten Kanal sichert ab, dass
ein abgewiesener Messpunkt weder Zeit noch frühere Kanalwerte in die Ausgabe schreibt.

```powershell
cmake --build build --config Release --target physim-run-fuzz
ctest --test-dir build -C Release --output-on-failure -R run_mutations
```

Jede Mutation wird über eine echte Datei durch `ps_run_open` und `ps_run_next`
gelesen. Erfolgreiche Samples müssen endlich sein und den Samplezähler genau um
eins erhöhen. Fehler/EOF erhalten sämtliche Ausgabeparameter; Finalisierung und
Zähler werden geprüft. Datei-Handles müssen nach Ablehnung oder Schließen frei sein.
Der Test ist dadurch langsamer als die IPC-Kampagne und hat ein 180-Sekunden-Limit.

Bei einem Fehler bleibt `build/run-fuzz/run-mutation.psrun` erhalten, auch bei
einem Decoder-Absturz. Der Harness meldet die deterministische Fallnummer.
Eine Eingabe von höchstens 8192 Bytes kann in einem separaten Arbeitsordner so
wiederholt werden:

```powershell
build/bin/physim-run-fuzz.exe --replay build/run-fuzz/run-mutation.psrun
```

Der Replay liest die Eingabe vollständig, bevor er seine Arbeitsdatei schreibt.
Die Kampagne ist begrenzt und ersetzt weder große Datensatzreferenzen noch
abdeckungsgeführtes Langzeit-Fuzzing. Die Dateiformatversion bleibt unverändert.

## Analyseberichte

CTest `report_mutations` erzeugt einen Bericht mit UTF-8-Titel, Provenienz,
Linien-/Punkt-/Histogrammkurven und einer Tabelle über die öffentliche API.
Alle Kürzungen und Einzelbitänderungen ohne CRC-Anpassung müssen abgewiesen werden.
Anschließend werden alle Nutzdatenbits erneut einzeln verändert, diesmal mit
passender Prüfsumme, um die Inhaltsprüfung zu erreichen. Extreme Größenangaben und
ein zusätzliches Byte nach dem Bericht werden ebenfalls geprüft.

```powershell
cmake --build build --config Release --target physim-report-fuzz
ctest --test-dir build -C Release --output-on-failure -R report_mutations
```

Akzeptierte Berichte werden vollständig über die öffentlichen Lese- und
Konstruktionsfunktionen neu aufgebaut. Bei Fehlern muss der bisherige Bericht als
Ausgabe erhalten bleiben. Ein eigener Allocator protokolliert Größen und Freigaben
und begrenzt den Ladebereich auf 32 MiB. Nach jedem Fall dürfen keine Blöcke übrig
sein. Zusätzlich wird beim gültigen Ausgangsbericht jede Allokationsstelle einzeln
zum Scheitern gebracht; auch dabei müssen Ausgabe und Speicherbereinigung stimmen.

CTest verwendet `build/report-fuzz` und ein 300-Sekunden-Limit. Die zuletzt geprüfte
Eingabe bleibt bei einem Fehler als `report-mutation.psreport` erhalten. Reproduktion
für Eingaben bis 8192 Bytes, am besten in einem eigenen Arbeitsordner:

```powershell
build/bin/physim-report-fuzz.exe --replay build/report-fuzz/report-mutation.psreport
```

Diese Kampagne prüft einen begrenzten Ausgangsbericht mit allen Kurvenarten und
Tabellen. Maximale Berichtsgrößen, lange Zufallssequenzen und abdeckungsgeführtes
Fuzzing erfordern weiterhin eigene Prüfungen.
