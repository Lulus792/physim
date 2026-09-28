# Decoder-Mutationsprüfungen

## Was ist der Fuzzing-Build?

Ein Fuzzing-Build ist ein Entwickler-Testprogramm, das Leser mit sehr vielen
veränderten, abgeschnittenen oder ungültigen Eingaben auf Robustheit prüft.
Er berechnet keine anderen Physikmodelle und wird für normale Experimente nicht
benötigt. Fehler sollen reproduzierbar gefunden werden, bevor beschädigte Dateien
oder Prozessnachrichten die App zum Absturz bringen.

Physim hat zwei verschiedene Testwege:

- Die drei normalen Tests `protocol_mutations`, `run_mutations` und
  `report_mutations` prüfen festgelegte, reproduzierbare Mutationen und
  Invarianten. Sie laufen über den direkten Builder und weiterhin über CTest.
  Dafür ist keine libFuzzer-Laufzeit nötig.
- `tools/build.py --fuzzer` baut `physim-protocol-libfuzzer`. Clang instrumentiert
  den IPC-Decoder und eine separate Kopie der Kernbibliothek mit libFuzzer und
  AddressSanitizer. libFuzzer nutzt erreichte Codepfade, um weitere Eingaben zu
  erzeugen; AddressSanitizer sucht unter anderem ungültige Speicherzugriffe.
  Dieser optionale Weg betrifft derzeit IPC, nicht die beiden Dateileser.

Der direkte Fuzzer-Build benötigt Python ab 3.10 und Clang einschließlich der
libFuzzer-/Sanitizer-Laufzeiten. Er baut nur den IPC-Harness, seine Kernbibliothek
und `physim-protocol-seeds` zum Erzeugen gültiger Snapshot- und Frame-Eingaben.
SDL, CMake und Ninja sind dafür nicht erforderlich. Die Ausgaben liegen separat
unter `build/native/Debug-fuzzer`; normale App-Builds werden nicht instrumentiert.
Unter Windows verwendet dieser Weg `clang-cl`, unter Linux/macOS `clang`.
MSVC und GCC werden für `--fuzzer` mit einer Diagnose abgewiesen.

**Linux: bauen und eine begrenzte Kampagne prüfen**

```sh
python3 tools/build.py --fuzzer --compiler clang
python3 tests/test_native_fuzzer.py --bin build/native/Debug-fuzzer/bin --work build/native
```

**macOS: LLVM mit libFuzzer installieren und verwenden**

Apple Clang aus Xcode 16.4 enthält auf den CI-Macs keine libFuzzer-Bibliothek.
Für diesen optionalen Test wird deshalb [LLVM 20 über Homebrew](https://formulae.brew.sh/formula/llvm@20)
mit seinem [Mach-O-Linker LLD](https://lld.llvm.org/MachO/index.html) verwendet.
Voraussetzung sind die Xcode-Werkzeuge und Homebrew aus der README.

```sh
brew install llvm@20 lld@20
PATH="$(brew --prefix lld@20)/bin:$PATH" python3 tools/build.py --fuzzer --compiler "$(brew --prefix llvm@20)/bin/clang"
python3 tests/test_native_fuzzer.py --bin build/native/Debug-fuzzer/bin --work build/native
```

**Windows, mit Visual Studio C++ Build Tools und LLVM/ClangCL:**

```powershell
python tools/build.py --fuzzer --compiler clang-cl
python tests/test_native_fuzzer.py --bin build/native/Debug-fuzzer/bin --work build/native
```

Falls ClangCL nicht im Suchpfad liegt, bei `--compiler` den vollständigen Pfad
zu `clang-cl.exe` angeben. Der Builder legt die passenden ASan-DLLs neben die EXE.

Der Prüfer erstellt für jeden Lauf einen neuen Ordner unter
`build/native/fuzzer run <Kennung>`. Er erzeugt beide gültigen Eingaben, spielt
sie mit dem eigenständigen Harness und libFuzzer erneut ab und startet danach
10.000 Durchläufe mit Seed 42. Ein Erfolg erfordert einen vollständigen Lauf,
zusätzlich erreichte Codepfade und keine gespeicherten Fehlerfunde. Die maximale
Eingabelänge von 8213 Bytes umfasst Nutzdaten, Frameheader und Decoderwahl.
`results.json` enthält Programmsignaturen, Ausgangseingaben, Kommandos,
Ausgaben, Exitcodes und Abdeckung. Corpus und mögliche Fehlerartefakte bleiben
für die Reproduktion im selben Ordner erhalten.

Für eine eigene längere Kampagne kann derselbe Corpus an
`physim-protocol-libfuzzer` mit `-max_total_time=60 -max_len=8213` übergeben werden.
`-artifact_prefix=<vorhandener-ordner>/` bestimmt den Speicherort von Fehlerfunden.
Eine einzelne Eingabedatei statt des Corpus-Ordners wiederholt genau diesen Fall.
Unter Windows hat das Programm die Endung `.exe`. Eine begrenzte Kampagne beweist
keine allgemeine Fehlerfreiheit.

**Lokaler Nachweisstand:** Der direkte Build besteht mit ClangCL 19.1.5. Der
Laufzeitstart scheitert auf dem lokalen Windows-Rechner weiterhin mit
`interception_win: unhandled instruction` in ASan, noch bevor der Harness läuft.
`build/native-fuzzer-build.log`, `build/native-fuzzer-run.log` und der JSON-Bericht
dokumentieren diesen Unterschied. Die neue CI führt den begrenzten Lauf mit
Windows ClangCL, Linux Clang und Homebrew LLVM auf beiden Mac-Architekturen aus;
im [Lauf zu `a9accf5`](https://github.com/PhysicSimulator/physim/actions/runs/36492484066)
bestehen Windows ClangCL und Linux Clang bereits die vollständige Kampagnenprüfung.
Der erste Apple-Silicon-Lauf scheiterte an der fehlenden libFuzzer-Bibliothek in
Apple Clang. Mit LLVM 20 besteht die Kampagne auf macOS Intel. Auf Apple Silicon
wies Apples Linker die instrumentierten Objekte mit `invalid r_symbolnum=1` ab.
Der direkte Fuzzer-Build verwendet dort nun LLVMs `ld64.lld`; der erneute
Apple-Silicon-Nachweis steht noch aus.
Der bisherige CMake-Einstieg
`PHYSIM_BUILD_FUZZERS=ON` bleibt bis zum Abschluss des Vergleichs verfügbar.

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
