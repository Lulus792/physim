# Bauen und testen

## Projekte in der App bauen

Physim erzeugt `physim.project` beim Anlegen und pflegt die Projekteinstellungen.
F5 startet den mitgelieferten `physim-build`, der C- und Physim-Quellen direkt über
den C17-Compiler in Experiment- und Analysemodule übersetzt. Eine `CMakeLists.txt`
oder CMake-Installation wird dafür nicht benötigt. Bestehende CMake-Dateien
werden nicht gelesen, überschrieben oder automatisch gelöscht.

Das unter **Build-Einstellungen** gewählte Profil wird beim Speichern, Bauen oder
normalen Beenden als `profile=Debug` beziehungsweise `profile=Release` in der
Projektdatei gespeichert. Beim erneuten Öffnen wird es zusammen mit den
Experimentparametern wiederhergestellt. Ältere Projektdateien ohne Profileintrag
verwenden Debug. Auch `physim-build` liest dieses Profil; ein explizites
`--profile` überschreibt es für den jeweiligen Aufruf.
App und Builder verwenden denselben Leser für das Projektformat. Doppelte oder
ungültige bekannte Einträge werden abgewiesen. Kommentare und zusätzliche
Projekteinträge bleiben beim Speichern erhalten; ein unverändertes Speichern
schreibt die Projektdatei nicht erneut. Geänderte Projektdateien werden mit
Sicherungskopie und Prüfung auf Änderungen während des Schreibvorgangs ersetzt.

Auch Zeitschritt und Zufallsseed werden als `simulation.dt` und `simulation.seed`
in der Projektdatei gespeichert und beim Öffnen wiederhergestellt. Ohne diese
Einträge gelten 0,005 Sekunden und Seed 42. Der Zeitschritt muss eine positive,
normale endliche Gleitkommazahl bis einschließlich einer Sekunde sein. Seeds
werden als Dezimalzahl von 0 bis 18446744073709551615 akzeptiert. Ungültige Werte
blockieren Speichern und Laufstart. Änderungen dieser Laufwerte benötigen
keinen neuen Modulbuild; jeder Lauf archiviert die verwendeten Werte.

Unter Windows werden Visual Studio 2022 C++ Build Tools und ein Windows SDK
automatisch gefunden; eine Developer-Konsole ist nicht nötig. Unter Linux und macOS wird
`cc` verwendet. Die Umgebungsvariable `PHYSIM_CC` kann einen anderen Compiler
angeben, etwa `clang-cl.exe` unter Windows oder `clang` unter Linux und macOS.
Auf macOS stellen die Xcode Command Line Tools (`xcode-select --install`) den
Compiler und das System-SDK bereit.

Der Projektordner behält seine Quellen, `physim.project` und Ergebnisse in `runs/`.
`build/Debug` und `build/Release` enthalten Module, generiertes C, Objektdateien,
Debugsymbole und den Buildzustand. Physim vergleicht vorverarbeitete Quellen,
um auch Änderungen an indirekt eingebundenen Headern zu erkennen. Unveränderte
Objekte und Module werden wiederverwendet. Ein fehlgeschlagener Compiler- oder
Linkeraufruf veröffentlicht keine neuen Module; der nächste Build holt das Linken
nach. Gleichzeitige Builds im selben Ausgabeordner werden abgewiesen.
Quellen mit zeitabhängigen Makros wie `__TIME__` können bei jedem Build erneut
übersetzt werden, weil sich ihr vorverarbeiteter Inhalt ändert.

Die nachfolgenden CMake-Anleitungen betreffen den bisherigen Bau von Physim selbst
und die SDK-Anbindung. Der direkte Buildweg ist unten beschrieben; die vollständige
Ablösung einschließlich Paketierung und gesamter Testsuite bleibt ein eigenes Ziel.

## Physim direkt ohne CMake bauen

`tools/build.py` ruft den C17-Compiler und den Archivierer direkt auf. Es benötigt
Python ab 3.10 und dessen Standardbibliothek. Es baut die Bibliotheken, `physimc`,
alle drei Runner-Werkzeuge, `physim-build` und die Oberfläche. Physim bleibt in C17
implementiert; Python steuert nur den Entwicklungsbuild.

Für die Oberfläche zuerst SDL 3.2.30 installieren, wie in der
[README](../README.md#linux) beschrieben. Der Quellbuild von SDL verwendet weiterhin
dessen eigenes CMake-Buildsystem; eine vorhandene SDL-Installation lässt sich direkt
angeben. Für Physim selbst werden weder CMake noch Ninja aufgerufen.

Windows, im Repository mit installierten Visual Studio C++ Build Tools und Python:

```powershell
.\tools\bootstrap-windows.ps1
python tools/build.py --config Debug --test
.\build\native\Debug\bin\physim.exe
```

Der Builder findet die x64-Werkzeuge von Visual Studio automatisch. Alternativ
`--compiler clang-cl` oder den vollständigen Pfad zu `clang-cl.exe` angeben.

Linux und macOS, nach dem lokalen SDL-Build aus der README:

```sh
python3 tools/build.py --config Debug --sdl "$PWD/build-sdl-install" --test
./build/native/Debug/bin/physim
```

`--compiler gcc` beziehungsweise `--compiler clang` wählt einen anderen Compiler.
macOS baut für die Architektur des verwendeten Compilers. Unter Linux/macOS wird
SDL neben die Programme kopiert und über einen relativen Laufzeitpfad gefunden.

Weitere Aufrufe (unter Windows `python` verwenden):

```sh
# Optimierter Build mit Oberfläche:
python3 tools/build.py --config Release --test
# Bibliothek, Sprachcompiler und Runner ohne SDL:
python3 tools/build.py --no-app --build-dir build/native/Core --test
# Den Buildmechanismus mit echten Compiler- und Linkerfehlern prüfen:
python3 tests/test_bootstrap_build.py --work build/native
```

Alle erzeugten Dateien liegen unter `build/native/<Konfiguration>` beziehungsweise
dem gewählten `--build-dir`. Unveränderte Quellen und Programme werden anhand von
Inhaltsprüfsummen wiederverwendet. Eine Änderung an Projekt- oder SDL-Headern baut
konservativ alle Objekte neu. Nach Änderungen am externen System-SDK kann
`--rebuild` einen vollständigen Neubau erzwingen. `--jobs 4` begrenzt die Zahl
gleichzeitig laufender Compiler. Ein exklusives Betriebssystem-Lock verhindert,
dass zwei Builds denselben Ausgabeordner verändern. Ein Compiler-/Linkerfehler
erhält das zuvor veröffentlichte Programm; der nächste Aufruf holt fehlende Schritte nach.

`--test` führt derzeit 493 Tests ohne Fenster aus, mit `--no-app` die
482 Prüfungen ohne SDL-Abhängigkeit. Die 51 C-Referenztests decken Mathematik, Numerik,
Mechanik, Messung, Datenreihen, Speicher, Sprachkern, Protokoll, Berichte und
App-Modelle ab. Auch die bestehenden Mutationsprüfungen und die erwartete
Laufzeitfehler-Diagnose der Sprachspeicherverwaltung bleiben enthalten.
Hinzu kommen 102 übersetzte Sprachprogramme und 71 Gruppen mit insgesamt 698
Compilerprüfungen für Sequenzen, Generics, Überladungen, Strukturinitialisierer,
Kontrollfluss, optionale Werte, Unicode, physikalische Einheiten, Compileraufrufe
und Modulimporte. Diese prüfen 671 erwartete Fehlerfälle sowie gültige Quellen,
erzeugten C-Code, Version, Größenbegrenzung und Abhängigkeitslisten. Die
ausführbaren Fälle schließen 18 erwartete Laufzeitfehler und zwei geladene
Experimentmodule mit Create-/Reset-/Step-/Scene-/Destroy-Prüfung ein. Das
`attempt`-Modul prüft auch die Erholung nach einem Fehler. Der Katalog
`tests/native_language_cases.json` enthält die gemeinsamen Erwartungen für den
direkten Läufer und die verbleibenden CTest-Einstiegspunkte.
Vier weitere Abläufe bauen Experiment-/Analysemodule und führen sie mit den
separaten Runnern aus. Neben Generics prüfen sie das Lesen von Datenreihen,
Quantile und den Export mehrerer CSV-Spalten. Geprüft werden Rückgabecodes,
Diagnosen, erzeugte Dateien, exakter CSV-Inhalt und das Ausbleiben abgelehnter
Exporte. Dateigröße und SHA-256-Prüfsumme der erwarteten Ausgaben stehen im
JSON-Bericht. Ein fehlgeschlagener Schritt stoppt seine Folgeschritte.
Diagnosefälle mit Imports erhalten jeweils einen eigenen Ordner, sodass ihre
Module und Quelldiagnosen voneinander unabhängig bleiben.
Zusätzliche Modulsuchpfade werden auch beim Übersetzen ausführbarer Programme
übergeben. Die Importprüfungen kontrollieren lokale Priorität, die Reihenfolge
der Suchpfade, doppelte Abhängigkeiten und zyklische Imports.

Der Katalog `tests/native_runtime_cases.json` übernimmt außerdem alle 203 Programme
des bisherigen nativen Sprachtests: 29 erfolgreiche Läufe und 174 erwartete
Laufzeitfehler. Bei 56 Programmen kontrolliert derselbe C-Prüfzusatz in beiden
Buildwegen, dass keine belegten Bytes nach Programmende oder Fehlerabbruch bleiben.
Vier weitere C-Tests prüfen diesen Zusatz mit normalen Rückgaben, Fehlerabbrüchen
und absichtlich gesetzten Restbytes. Änderungen an Testheadern bauen die betroffenen
Testprogramme neu.
Zwei zusätzliche Prüfungen verlangen die richtige Physim-Zeile in einem absichtlich
ausgelösten C-Compilerfehler sowie unveränderte C-Quellen und Programme nach
abgelehnter Quellübersetzung. Der letzte vollständige Programmstand muss weiterhin
ausführbar sein; Prüfsummen vor und nach dem Fehler stehen im Ergebnisbericht.

`tests/native_integration_cases.json` beschreibt weitere 56 Prüfungen ohne Fenster mit 86
Ausführungsschritten. Sie bauen 63 Physim-Programme/-Module und 72 C-Module/-Prüfer
und verwenden 14 bereits gebaute Programme und Beispielmodule sowie den aktuellen
Python-Interpreter. Die vorhandenen
C-Prüfer vergleichen unter anderem Pendelintegratoren, Kollisionen, Auftrieb,
Sensoren, Kontakte, Gelenke, gekoppelte Körper und schnelle Kugeln mit den
Physim-Modellen. Sie prüfen Messdateien, Szenen, Analyseberichte, Einheiten,
Parameter und Fehlerbehandlung. Parameterreihen werden mit zwei parallelen
Runnern berechnet und anhand ihrer Messwerte, Metadaten und Berichte geprüft.
Abgelehnte Parameter dürfen keine Laufdateien oder Ergebnisordner anlegen.
Jeder Ablauf erhält einen eigenen Arbeitsordner mit Leerzeichen und Umlaut.
Buildfehler stoppen abhängige Schritte; Ergebnisberichte erfassen außerdem die
Pfade und SHA-256-Prüfsummen aller verwendeten Programme und Module.
Der verbleibende CTest-Einstieg verwendet dieselben Aufrufe und Erwartungen.
Die Runner-Prüfungen schließen Handshake, Pause/Einzelschritt/Fortsetzen, falsche
Modul-ABI, Abstürze, Endlosschleifen und das Beenden paralleler Kindprozesse ein.
Weitere Referenzen vergleichen Energie und Periode des Pendels für RK4, RK45 und
Verlet sowie abgeleitete Daten und Berichte. Vor jedem Vergleich erzeugt der
jeweilige Ablauf seine Messdaten im eigenen Arbeitsordner. Gemeinsam genutzte
CTest-Messdateien und ein gesonderter Bereinigungsschritt entfallen damit.
Auch die C- und Physim-Beispiele aus der Dokumentation werden ausgeführt.

Vier zusätzliche Prüfungen vergleichen elf Codeblöcke der Tutorials mit den
getesteten Quelldateien. Eine weitere kontrolliert die erzeugte API-Referenz.
Der PNG-Ablauf erzeugt seine Bilder selbst und prüft deren dekodierte Pixel.
Der Projektbuild-Test baut und startet C- und Physim-Projekte und prüft unveränderte
Builds, Headeränderungen, Fehlererholung, die Buildsperre und verschobene Ordner.
Diese Python-Prüfungen verwenden in beiden Buildwegen denselben Katalog; für
die vollständige CTest-Suite muss Python ab 3.10 installiert sein.

```sh
# Nur die direkt unterstützten Sprachtests ausführen:
python3 tools/build.py --test --test-filter 'language_*'
# Mehrere Testgruppen gemeinsam auswählen, ohne doppelte Ausführung:
python3 tools/build.py --test --test-filter '*_reference' --test-filter '*_isolation'
# Fehlerbehandlung des Testläufers selbst prüfen:
python3 tests/test_native_test_runner.py --work build/native
```

### Fenster- und Grafiktests direkt ausführen

`--test-display` führt zusätzlich 35 Fenster- und Grafikabläufe aus. Dafür sind
eine grafische Sitzung, SDL und ein geeigneter OpenGL-Treiber erforderlich.
Die beiden Testgruppen werden getrennt gestartet; `--test-display` lässt sich
nicht mit `--no-app` oder `--test` kombinieren.

```sh
# Linux/macOS in einer grafischen Desktop-Sitzung:
python3 tools/build.py --test-display
# Nur kleine und große Menüs sowie Dokumentwiederherstellung prüfen:
python3 tools/build.py --test-display --test-filter 'toolbar_*' --test-filter documents_recovery
```

Unter Windows dieselben Befehle mit `python` ausführen. Für Linux-CI mit Mesa,
Xvfb und Openbox (die Pakete `xvfb`, `openbox` und `x11-utils` müssen installiert sein):

```sh
LIBGL_ALWAYS_SOFTWARE=1 xvfb-run -a -s '-screen 0 1920x1080x24' sh -c \
  'openbox > /tmp/physim-openbox.log 2>&1 & python3 tools/wait-window-manager.py && python3 tools/build.py --test-display'
```

Vor dem App-Start prüft der Aufruf den lebenden Fenstermanager anhand der
[EWMH-Eigenschaft `_NET_SUPPORTING_WM_CHECK`](https://specifications.freedesktop.org/wm/1.5/ar01s03.html).
Der App-Prüfer setzt außerdem `PHYSIM_TEST_TRACE=1`, damit Start und Testphasen
bei einem Timeout in `app-steps.json` und der Fehlerausgabe sichtbar bleiben.

Die Abläufe prüfen Menüs und Tabs bei zwei Fenstergrößen, Dokumentbearbeitung,
Wiederherstellung nach Abbrüchen, Workspace- und Projekteinstellungen,
Diagramme, Stapelläufe, Sprachvorschauen und 13 vollständige Sprachprojekte.
Der gewählte Compiler wird auch für die von der App angelegten Projekte verwendet.
Diagrammtests dekodieren zusätzlich die PNG-Exporte und vergleichen Pixel,
Farben, Legenden und beschnittene Linien, Balken und Punktmengen.
Jeder Ablauf erhält einen frischen Ordner mit Leerzeichen und Umlaut.
`app-steps.json` enthält die einzelnen App-Aufrufe, Rückgabecodes und Ausgaben;
Screenshots und gespeicherte Projekte bleiben daneben erhalten.
Die bisherigen 13 CMake-Workflow-Skripte sind durch einen gemeinsamen Python-Prüfer
ersetzt. Die CTest-Einstiegspunkte verwenden denselben Prüfer und Katalog.

### Ergebnisberichte

Jeder Lauf schreibt nach `build/native/<Konfiguration>/test-results/run-<Kennung>`.
`results.json` enthält für jeden Test Kommando, Ergebnis, Dauer und Ausgabe.
Unterordner isolieren temporäre Testdateien. Erwartete Fehler müssen den passenden
Exitcode und gegebenenfalls den vorgegebenen Text liefern. Ein fehlgeschlagener
Test oder Testbuild verhindert die Ausführung späterer Fälle nicht; der gesamte
Aufruf endet dennoch mit einem Fehlercode. Zeitüberschreitungen werden als Fehler
gespeichert. Ein Filter ohne Treffer ist ebenfalls ein Fehler.

Die drei Benchmark-Prüfungen (`benchmark_smoke`, `benchmark_driver`, `ui_rendering`)
sind ebenfalls übertragen. Der direkte Katalog enthält damit 493 Prüfungen ohne
Fenster und 35 Grafikabläufe. Die vorhandene CMake-CI bleibt für den Vergleich
und ihre bisherigen Sanitizer-/SDK-Prüfungen erhalten; ihre vollständige Ablösung
und der plattformübergreifende Nachweis des erweiterten Katalogs stehen noch aus.

### Benchmarks direkt bauen

`--benchmarks` baut die optionalen Programme `physim-benchmark` und
`physim-ui-benchmark`. Mit `--no-app` entsteht nur der Benchmark ohne Fenster.
Die zugehörigen Tests bauen ihre benötigten Programme auch ohne diese Option.

```sh
python3 tools/build.py --config Release --benchmarks
python3 tools/build.py --config Release --test --test-filter 'benchmark_*'
python3 tools/build.py --config Release --test-display --test-filter ui_rendering
```

Unter Windows `python` statt `python3` verwenden. Messbefehle, Vergleichsgrenzen
und Bedingungen für belastbare Laufzeiten stehen unter
[Leistungsmessung](performance.md) und [UI-Zeichenpuffer](ui-rendering.md).
Compilerkennung und Buildprofil stammen aus dem übersetzten C-Programm.
Die Messskripte erfassen außerdem den direkten Builder und die Quellen als
Prüfsummen und benötigen keine `CMakeLists.txt`.

### SDK und portable Pakete ohne CMake

Der direkte Build erzeugt auch die acht C-Beispielmodule und das Analysemodul.
`--install` stellt Programme, Module, Kernbibliothek, öffentliche Header,
SDK-Quellen, Vorlagen, Dokumentation und Lizenzen in einem neuen Ordner zusammen.
Die Datei `physim-sdk.json` enthält relative Dateipfade und SHA-256-Prüfsummen.
Objekte, Buildcaches, Tests und temporäre Linkerdateien werden nicht installiert.
Vorhandene Zielordner werden nicht überschrieben; ein unvollständiges Paket wird
nicht unter dem gewünschten Zielnamen veröffentlicht.

Windows, mit den oben genannten Voraussetzungen:

```powershell
python tools/build.py --config Release --install build/native/SDK
python tools/verify-native-sdk.py --sdk build/native/SDK --work build/native --app-tests
Compress-Archive -Path build/native/SDK -DestinationPath build/Physim-Windows.zip
```

Windows-Pakete benötigen `Release`; die x64-Laufzeitbibliotheken stammen aus
der installierten Visual-Studio-Weiterverteilung. Debug-Builds bleiben lokale
Entwicklungsbuilds. Nach dem Entpacken startet `SDK/bin/physim.exe` die App.

Linux:

```sh
python3 tools/build.py --config Release --install build/native/SDK
python3 tools/verify-native-sdk.py --sdk build/native/SDK --work build/native --app-tests
tar -czf build/Physim-Linux.tar.gz -C build/native SDK
```

Das entpackte Paket startet mit `./SDK/bin/physim`. SDL liegt neben dem Programm;
der Zielrechner benötigt weiterhin einen kompatiblen Linux-Unterbau und Grafiktreiber.

macOS, einschließlich Finder-App:

```sh
python3 tools/build.py --config Release --install build/native/SDK
python3 tools/verify-native-sdk.py --sdk build/native/SDK --work build/native --app-tests
python3 tools/package-macos.py --sdk build/native/SDK --output build/Physim-native.app
open build/Physim-native.app
ditto -c -k --sequesterRsrc --keepParent build/Physim-native.app build/Physim-macOS.zip
```

Das `.app`-Paket enthält SDL und das SDK und wird lokal ad hoc signiert und geprüft.
Developer-ID-Signierung und Apple-Notarisierung bleiben offen. `--sdk` verwendet
keinen CMake-Aufruf; `--build` im Paketskript unterstützt weiterhin bestehende
CMake-Buildverzeichnisse.

Die SDK-Prüfung kopiert und verschiebt das Paket in einen Pfad mit Leerzeichen
und Umlaut. Sie prüft alle mitgelieferten Module, öffentliche Header einzeln,
15 eigenständige Sprachprogramme und neu gebaute C-/Physim-Projekte samt echten
Runnern und Mess-/Berichtsdateien. `--app-tests` ergänzt zwei vollständige
App-Abläufe und benötigt eine grafische Sitzung. Unter Windows führen die
gehosteten CI-Worker diese Grafikabläufe nicht aus; sie werden lokal geprüft.
Die mitgelieferten CMake-Anbindungsdateien bleiben vorerst für bestehende
SDK-Verbraucher erhalten; der neue Ablauf verwendet sie nicht.

## Toolchains

Windows: CMake 3.24+, Visual Studio 2022 mit C/C++-Desktop-Workload und Windows SDK.
Linux: GCC oder Clang, CMake 3.24+, Make/Ninja. Der Core braucht nur libc, libm
und die jeweilige Betriebssystemschicht. SDL3 und Nuklear werden nur für die App benötigt.
Die App setzt einen OpenGL-3.3-Core-Treiber voraus. Grafiktests auf Linux-Servern
können Mesa mit Xvfb verwenden; der CI-Workflow konfiguriert dies. Für den echten
Maximierungs-/Wiederherstellungstest startet er zusätzlich den Fenstermanager Openbox.

Nuklear ist unter `third_party` eingecheckt. SDL3 für Windows wird aus dem offiziellen
3.2.30-Release bezogen. Das Bootstrap-Skript prüft SHA-256 vor dem Entpacken.

Für Linux und macOS SDL3 aus derselben Version bauen:

```sh
git clone --depth 1 --branch release-3.2.30 https://github.com/libsdl-org/SDL.git build-sdl-source
cmake -S build-sdl-source -B build-sdl -DCMAKE_BUILD_TYPE=Release -DSDL_TESTS=OFF -DCMAKE_INSTALL_PREFIX="$PWD/build-sdl-install"
cmake --build build-sdl --parallel
cmake --install build-sdl
cmake -S . -B build -DCMAKE_PREFIX_PATH="$PWD/build-sdl-install" -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

SDL benötigt systemabhängig X11-/Wayland-Entwicklungspakete. Der CI-Workflow zeigt
die Ubuntu-Pakete. Bibliothek und Runner können mit `PHYSIM_BUILD_APP=OFF` unabhängig
von sämtlichen UI-Paketen gebaut werden.

macOS verwendet Apple Clang, die Xcode Command Line Tools und OpenGL 4.1 Core.
Die obigen Befehle bauen jeweils für die Architektur des Macs (Apple Silicon oder
Intel). Für Fensterprüfungen beim Konfigurieren `-DPHYSIM_GRAPHICS_TESTS=ON`
ergänzen und die Tests in einer angemeldeten grafischen Sitzung ausführen.
Die App startet mit `./build/bin/physim`. Für Speichern, Suche und Editorbefehle
gilt auf macOS **Cmd** anstelle von **Ctrl**. Die aktuellen Plattformnachweise
stehen in [Plattformprüfung](platform-validation.md).

## Tests

Das Handbuch unter `docs/guide.md` und seine Funktionsreferenzen werden mit dem SDK
installiert. Nach Änderungen an öffentlichen Funktionen oder Sprachbindungen:

```powershell
python tools/generate-reference.py
python tools/generate-reference.py --check
```

Der Generator prüft für jede C-Funktion einen deutschen Erklärungstext und übernimmt
Signaturen, Typen und SDK-Verträge. Sprachmethoden werden mit der tatsächlichen
Empfänger-/Fabrikschreibweise dokumentiert. Das C-Tutorial übernimmt seine Codeblöcke
aus `examples/documentation`; `documentation_example` prüft Bewegung, Szene, Reset
und die daraus berechnete Geschwindigkeit. `documentation_reference` erkennt
veraltete Referenzen und Tutorialcode (wenn Python verfügbar ist).
`documentation_window` prüft mit aktivierten Grafiktests alle registrierten Seiten,
Suche und das unabhängige Hilfefenster.

### Installiertes SDK prüfen

Nach einem vollständigen Build prüft dieser plattformunabhängige Ablauf auch
die installierten Quellen und Werkzeuge:

```sh
cmake -DBUILD_DIR=build -DCONFIG=Release -P tools/verify-sdk.cmake
```

`CONFIG` muss zur gebauten Konfiguration passen, beispielsweise `Debug` auf
Linux oder beim entsprechenden Visual-Studio-Build. Generator und Compiler
werden aus dessen CMake-Cache übernommen; bei Ninja muss die ursprüngliche
Compilerumgebung weiterhin eingerichtet sein.

Das Skript installiert in einen neuen Ordner mit Leerzeichen und Umlaut,
verschiebt das SDK und baut einen separaten Verbraucher mit dem installierten
`PhysimExperiment.cmake`. Alle acht Vorlagen und das Analysemodul werden aus den
installierten Quellen neu kompiliert; jeder öffentliche Header wird in einer
eigenen Übersetzungseinheit eingebunden. Die installierten Runner führen jede
Vorlage aus. Eine gegen das SDK gebaute Prüfung liest alle 201 Samples, prüft
Zeitpunkte und endliche Werte und lädt den erzeugten Bericht samt CRC-Prüfung.
Zusätzlich werden alle installierten Sprachbeispiele über `PhysimLanguage.cmake`
neu gebaut, wobei der Compiler aus dem verschobenen SDK gefunden werden muss.
Sieben eigenständige Sprachprogramme laufen mit ihren eingebauten Referenzprüfungen.
Sprach-Pendel, -Wurf und -Feder werden jeweils mit beiden Sprach-Analysemodulen
ausgewertet; die Prüfung kontrolliert Messzeilen und die erwarteten Berichtsplots.
Dies ist eine Paket-/Integrationsprüfung; die physikalischen Referenztests
bleiben zusätzlich erforderlich.

Unter `build/SDK Test ä <Kennung>/` bleiben `verification.log`, erzeugte Dateien
und bei Erfolg `PASSED.txt` erhalten. Wiederholungen verwenden neue Ordner.
Die Windows- und Linux-CI führen den Ablauf aus und archivieren das Protokoll.
Eine konfigurierte CI ist kein Beleg für einen erfolgreich ausgeführten Lauf.

### Fachliche und technische Prüfungen

- Speicher: Multiplikationsüberlauf, Resize-Fehler, exakte Freigabegrößen, Arena-
  Ausrichtung/Grenzen und Wiederverwendung. Ein Test-Allocator löst jeden
  Allokationsfehler im Berichtablauf und Laden aus; bestehende Berichte bleiben
  bytegleich. SVG-/CSV-Export, abgeschnittene Dateien und Analysekontexte mit
  echtem Datensatz werden auf vollständiges Aufräumen geprüft. [Details](memory.md).

- Mathematik: Vec2/3/4 einschließlich subnormaler und maximaler Werte, allgemeine
  Matrixinversion mit beidseitigen Residuen, Quaternionen gegen unabhängige
  Rodrigues-Rotation, TRS und projektive Punkte, Normalen bei Scherung/Spiegelung,
  Aliasierung und unveränderte Ausgaben bei Fehlern. Details: [math.md](math.md).

- Mechanik: Kugel-/Boxträgheit, rotierte Hauptachsen, Drehmoment und Impuls,
  Quaternionrotation, gyroskopischer Term, Konvergenz erster Ordnung,
  Impuls-/Energieerhaltung, inelastischer Stoß, Coulomb-Reibung, Stokes- und
  quadratischer Widerstand, Federn und transaktionale Fehlerbehandlung.
- Stoßreferenz: echte Runner-Läufe im Vakuum, in Luft, eigenem Medium und mit
  Anfangsrotation/Reibung. Alle 401 Samples gegen Impuls-/Energie- und
  Widerstandsreferenzen geprüft, einschließlich Modellmetadaten.
- Boxkontakte: orientierte Kugel–Box-/Box–Ebene-Geometrie, akkumulierte Impulse,
  zweidimensionale Coulomb-Reibung, redundante Punkte und ruhender Bodenkontakt.
  Die echte Boxvorlage läuft zehn Sekunden; alle 2001 Samples werden gegen
  Energiegrenzen, Eindringung und den abschließenden Ruhezustand geprüft.
- Feder–Masse–Dämpfer: echte Läufe für vier Dämpfungsfälle mit je 2001 Samples gegen
  geschlossene Lösungen; Kräfte, Energie, dissipierte Arbeit und Bilanz geprüft.
  RK4-Konvergenz bei Zeitschritthalbierung sowie alle Energiekurven und die
  Bilanzkennzahl im erzeugten Analysebericht werden separat geprüft.

- Core: Quaternionen, Einheiten, reproduzierbarer RNG, Normalverteilungsstatistik,
  RK4-Konvergenz, symplektische Energiegrenze, Impulsaustausch, Ableitung, Integration.
- Daten: CRC-Golden-Vektor, Roundtrip, CSV, Abschneiden an jeder Dateiposition.
- IPC: Teilframes, falsche Größen, Snapshotvalidierung.
- Datenreihen: Blockgrenzen, Einheiten, Handle-Lebensdauer, zeitliche Zuordnung,
  Snapshot-Stabilität, Speicherplatzlimit, Recovery und physikalische CSV-Referenzen.
- Ergebnisberichte: Roundtrip, jedes abgeschnittene Dateipräfix, CRC und 300 Mutationen,
  Einheiten/Skalen, erhaltene Spitzen, vollständige Histogrammzählung und Exporte.
  Der Runner-Bericht wird gegen CSV sowie Energie-/Periodenreferenzen geprüft.
- Dokumentation: Parser, Unicode-Suche und Fehlerfälle. `physim --docs-test <bildordner>`
  prüft zusätzlich das separate Fenster über F1, das Laden aller Themen, Suche über
  SDL-Textereignisse, unabhängige Eingaben, Größenänderung sowie Schließen und
  Wiederöffnen mit erhaltener Leseposition.
- Autosave: gemeinsamer Snapshot beider Quellen und ihrer Ausgangstexte, Unicode,
  maximale Dateigröße, jedes abgeschnittene Präfix, Mutationen und fehlgeschlagene
  temporäre Schreib-/Umbenennungsoperationen. `autosave_workflow` beendet einen
  schreibenden App-Prozess abrupt und startet eine neue Instanz. SDL-Mausereignisse
  prüfen Wiederherstellen/Verwerfen, Konflikthinweis bei externen Änderungen,
  Schließen ohne Auswahl sowie Erhalt beschädigter Sicherungen.
- Runner: echter Handshake, deterministischer Einzelschritt, Pause/Fortsetzung,
  absichtlicher Modulabsturz, Endlosschleife, harte Terminierung und falsche ABI.
- End-to-end: 4000 Pendelschritte und separater Analyse-Runner.
- Laufverwaltung: Unicode-Dateinamen, Sortierung, Dateifilter, Pfadgrenzen und
  Erhalt der alten Liste bei fehlgeschlagener Aktualisierung.
- Mehrlaufanalyse: verschiedene Zeitraster, Ableitungs-/Statistikreferenz,
  interpolierte Differenzkurven und vollständige CSV gegen analytische Referenzen,
  teilweise überlappende Laufzeiten, ein gemeinsamer Zeitpunkt und getrennte Bereiche,
  Eingabemanifest und Ablehnung falscher Dimensionen. Analyseisolation mit echtem
  Absturz, Endlosschleife und kurzem ABI-3-Modul ohne Mehrlauf-Callback.
- Resampling: lineare und quadratische Referenzen, exakte Endpunkte, Blockgrenzen,
  vollständige Monotonieprüfung, ungültige Bereiche/Einheiten, sehr große Werte,
  Einzelpunkt, Lebensdauer und transaktionaler Abbruch bei erschöpftem Plattenbudget.
- Grafik: `physim --renderer-test [bild.bmp]` prüft tatsächliche GPU-Pixel für
  Verdeckung, Projektionen, Größenwechsel, Clipping, Draufsicht und Vektoren.
  Zusätzlich werden UTF-8-Eingabe und Fokusverlust geprüft. Der optionale Bildpfad
  speichert eine Szene mit räumlichen Primitiven einschließlich Rotation und Polyline.
- App: `physim --smoke` zeichnet acht Frames und beendet sich. `physim --self-test
  <neuer-absoluter-projektpfad> [pendulum|projectile|collision|box_floor|spring|uncertain_projectile]` erzeugt ein Projekt, prüft zunächst einen absichtlich
  eingefügten Compilerfehler samt Diagnose, korrigiert und baut es, simuliert, pausiert,
  schaltet einen Einzelschritt, stoppt und analysiert. Editor-, Simulations- und
  Auswertungsansicht werden als BMP im Testprojekt gespeichert. Anschließend prüft
  der Test über injizierte SDL-Ereignisse Arbeitsbereichwechsel, Suchleiste, Protokoll
  und Navigation. Weitere Bilder zeigen Suchleiste und geöffneten Inspector.
  Mit `PHYSIM_TEST_SMALL=1` läuft derselbe Ablauf bei 1080 × 740.
  Er prüft außerdem Ergebnisdiagramme, Tabellen und deren CSV-/SVG-Schaltflächen.
  Anschließend vergleicht er zwei Läufe mit unterschiedlichen Zeitschritten,
  öffnet das Projekt erneut und lädt frühere Berichte sowie Messdaten ohne Build.
  Auswahl, Öffnen und Dateifilter werden über SDL-Maus-/Tastaturereignisse geprüft.
  `PHYSIM_TEST_LONG=1` verlängert die Testsimulation auf 20 s, etwa für mehrere Pendelperioden.

`cmake -S . -B build -DPHYSIM_GRAPHICS_TESTS=ON` nimmt Grafiktest und Fenstersmoke
und Dokumentationsfenster sowie Autosave-Neustarttests und den Monte-Carlo-App-Test
in CTest auf, zusätzlich zu Speicherverwaltung, Mathematik und weiteren Kernprüfungen.
Die konkrete Testliste der gewählten Konfiguration zeigt `ctest -N`.
Standardmäßig ist diese Option aus, sodass
Core-/Runner-Tests ohne Display laufen. Die Grafiktests wurden lokal mit NVIDIA
OpenGL 3.3 ausgeführt. Der Windows-CI-Job prüft Build und Core/Runner; Linux-CI soll
Grafik unter Xvfb/Mesa prüfen. Das CTest-Label `display` erfasst die Tests,
die ein Fenster benötigen: `ctest -LE display` prüft ohne Anzeige, `ctest -L display`
unter Xvfb mit Openbox prüft die Fensterabläufe. Die CI läuft bei jedem Push in
beiden Repositories. Den konkreten Prüfstand zeigt
[GitHub Actions](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Auf dem lokalen Windows-Rechner ist keine WSL-Distribution installiert.

`physim --plot-test <neuer-absoluter-ordner>` prüft Diagrammzoom und Verschieben
über SDL-Ereignisse sowie den PNG-Export per Schaltfläche. Es erzeugt außerdem
`figure-0.png` bis `figure-3.png` und `figure-dense.png` als Bildreferenzen.
Nur in diesem Testmodus werden markierte Test-Mausereignisse verarbeitet;
Desktop-Mausbewegungen und Fokusverlust verändern den Testeingang nicht.
`physim --plot-test-noise <neuer-absoluter-ordner>` streut zusätzlich in jedem
Frame fremde Mausbewegungen, Loslassen der Maustaste und Fokusverlust ein.
CTest führt diesen vollständigen Ablauf als `plot_input_isolation` aus.
Normale App-Eingaben und deren Fokusverlust-Behandlung bleiben unverändert.
`png` prüft den PNG-Schreiber ohne Grafik. Nach CTest lassen sich beide Ergebnisse
mit `python tests/verify_png.py <build-ordner> <plot-test-ordner>` unabhängig
mit Python-zlib und erwarteten Pixelwerten überprüfen; zusätzliche Python-Pakete
sind für diesen Prüfschritt nicht nötig.

`batch_reference` prüft 256 echte Wurfläufe gegen die analytische Endwertverteilung,
dieselbe Serie mit vier Runnern, acht vollständige Wiederholungsläufe mit acht Runnern,
andere Seeds, Statistikreferenzen, Schemafehler, Absturz, Zeitlimit und Abbruch.
Endwert-CSV und numerische Berichtsdaten müssen bei unterschiedlicher Parallelität
bitgleich bleiben. `batch_parallel` erzwingt vier gleichzeitig aktive Prozesse
über eine Barriere sowie einen absichtlich verspäteten ersten Lauf. Er prüft
getrennte Arbeitsordner, lückenhafte Teilergebnisse, Kindprozess-Ende bei Abbruch,
Fehler und starkem stdout-Verkehr mit Zeitlimit sowie die CLI-Option `--workers`.
`physim --batch-test <neuer-absoluter-projektpfad> [2..8]`
baut die Unsicherheitsvorlage und prüft die Laufserie mit injizierten SDL-Klicks
einschließlich Änderung der Parallelität per UI, Bericht, Anleitung, erneutem Start,
Abbruch und Schließen während einer weiteren Serie; Screenshots liegen im Testprojekt.
`batch_workflow` erzeugt dafür bei jeder CTest-Ausführung einen eigenen Projektordner.

`tools\build-clang.cmd` baut die App direkt mit ClangCL und führt die Tests ohne
Fenster aus. Es benötigt Python, SDL und die Visual-Studio-C++-Werkzeuge mit LLVM;
CMake und Ninja werden nicht aufgerufen. Der Builder erkennt Visual Studio
automatisch, der Ausgabeordner ist `build/native/Clang`. Weitere Argumente werden
weitergegeben, etwa `--config Release` oder `--test-filter core`.

RK4-Test: beim Halbieren von dt von 0.1 auf 0.05 im harmonischen Oszillator wird
ein Fehlerquotient zwischen 14 und 18 erwartet (globale Ordnung 4), absoluter Fehler
bei t=1 unter 1e-7. Symplektischer Test: 10000 Schritte mit dt=0.005 und Energiefehler
unter 0.002 bei Anfangsenergie 0.5. RNG-Momententest: 100000 Werte, Toleranz 0.04 für
Mittelwert 2 und Standardabweichung 3. Diese Toleranzen sind keine pauschale Garantie
für beliebige Experimente.

Der zusätzliche Pendel-Referenztest vergleicht den echten Runner-Datensatz mit
`T = 4 sqrt(L/g) K(sin(theta0/2))`. Zehn Glieder der Binomialreihe für das vollständige
elliptische Integral reichen hier für einen Abschneidefehler unter 1e-12. Die
gemessene Periode darf wegen zeitdiskreter Nulldurchgänge um 2e-6 s abweichen;
die maximale Energieabweichung über 20 Sekunden muss unter 1e-8 J bleiben. Das gilt
für RK4 und RK45. Verlet wird bei demselben dt gegen 5e-5 s Periodenfehler und
1e-4 J Energieabweichung geprüft; sein Konvergenztest bestätigt zusätzlich Ordnung 2.
Weitere Verträge und Tests sind in [numerics.md](numerics.md) beschrieben.

`preferences` prüft das Einstellungsformat, Grenzen, Prüfsumme, alle abgeschnittenen
Dateilängen und die Erhaltung der bisherigen Datei bei Fehlern. `settings_workflow`
startet die App siebenmal mit einem eigenen Einstellungsordner. SDL-Ereignisse prüfen
Schriftwahl, Schalter, Übernehmen, Abbrechen, Standardwerte und Ziehen der Panelgrenzen.
Neustarts prüfen Fenstermaße, Maximierung, Sichtbarkeit und Schrift. Eine reale
Autosave-Datei bestätigt das gewählte Intervall; beschädigte Einstellungen bleiben
unverändert. Die normalen Benutzereinstellungen werden von diesen Tests nicht gelesen.

## Sanitizer ohne CMake

`--sanitizers` instrumentiert die direkt gebauten Bibliotheken, Programme,
Testmodule und aus Physim erzeugten C-Quellen. Windows verwendet AddressSanitizer
(ASan); Linux und macOS zusätzlich UndefinedBehaviorSanitizer (UBSan). Ein
erkannter Fehler beendet das betroffene Programm mit einem Fehlercode.
Die Standardausgabe liegt getrennt unter `build/native/Debug-sanitized`.

**Windows, Speicherverwaltung mit MSVC prüfen:**

```powershell
python tools/build.py --sanitizers --no-app --test --test-filter "memory*"
python tests/test_sanitizer_build.py --work build/native
```

Auch `tools\check-memory-asan.cmd` verwendet jetzt diesen direkten Buildweg.
Python ab 3.10, Visual Studio C++ Build Tools und deren ASan-Komponente sind
erforderlich. Der Builder erkennt die Visual-Studio-Installation automatisch und
kopiert die zum gewählten Compiler gehörenden ASan-DLLs neben die Programme.
Sanitizer-Builds verwenden `/MD` auch in Debug, da ClangCL ASan die Debug-CRT
nicht unterstützt; Debugsymbole und deaktivierte Optimierung bleiben erhalten.

**Linux und macOS, alle Tests ohne Fenster:**

```sh
python3 tests/test_sanitizer_build.py --work build/native
python3 tools/build.py --sanitizers --test --sdl "$PWD/build-sdl-install"
```

Mit `--no-app` statt `--sdl ...` entfallen SDL und die App-spezifischen Tests.
`--compiler gcc`, `--compiler clang` beziehungsweise `--compiler clang-cl`
wählt einen anderen Compiler. Der Probe-Test läuft erst fehlerfrei ohne und mit
Instrumentierung; danach muss ASan einen echten Heap-Pufferüberlauf erkennen.
Unter Linux/macOS muss außerdem UBSan einen vorzeichenbehafteten Ganzzahlüberlauf
erkennen. Die Ausgaben und Exitcodes stehen unter
`build/native/sanitizer probe <Kennung>/results.json`.

**Linux, Fensterabläufe mit Instrumentierung:**

```sh
ASAN_OPTIONS=detect_leaks=0 python3 tools/build.py --sanitizers --test-display --sdl "$PWD/build-sdl-install"
```

Hierfür wird eine grafische Sitzung benötigt. Nur für diese Grafikprüfung ist
die Leakprüfung wegen der Lebensdauer globaler SDL-/Mesa-Allokationen ausgenommen;
die separaten Tests ohne Fenster behalten sie bei. Die CI verwendet Xvfb und
Openbox wie bei den normalen Grafiktests.

Lokal bestehen mit MSVC die Instrumentierungsprobe und zehn Core-, Speicher-,
Berichts-, Mutations- und Sprachspeichertests. Der installierte ClangCL 19.1.5
scheitert auf diesem Windows-Rechner bereits beim Start des fehlerfreien
ASan-Minimalprogramms mit `interception_win: unhandled instruction`.
Die neuen CI-Schritte prüfen die Instrumentierung getrennt unter Windows,
Linux und macOS. Im [Lauf zu `c92dbd3`](https://github.com/PhysicSimulator/physim/actions/runs/36491788018)
bestehen die Probe und zehn ausgewählte Tests mit Windows MSVC und ClangCL;
die Ergebnisse für Linux und macOS stehen noch aus.

Die Option betrifft den Repository-Build. Sie ergänzt keine Compileroptionen in
Nutzerprojekten, die die App während eines Ablaufs baut. Ein portables SDK wird
separat ohne `--sanitizers` erstellt; `--sanitizers --install` wird abgelehnt.

Der optionale IPC-Fuzzer wird mit `python3 tools/build.py --fuzzer --compiler clang`
ebenfalls direkt gebaut (Windows: `python` und `--compiler clang-cl`). Dieser
getrennte Modus instrumentiert auch die Kernbibliothek und benötigt kein SDL.
Aufruf, begrenzte Kampagne und Nachweisgrenzen stehen in [Fuzzing](fuzzing.md).

## Portables Paket

Die Sprachvorlagen lassen sich mit dem vollständigen App-Test prüfen. Jeder Test
benötigt einen neuen, noch nicht belegten Projektordner:

```powershell
.\build\bin\physim.exe --self-test D:/PhysimTest/Sprachpendel language_pendulum
.\build\bin\physim.exe --self-test D:/PhysimTest/Sprachwurf language_projectile
.\build\bin\physim.exe --self-test D:/PhysimTest/Sprachworkflow language_full
.\build\bin\physim.exe --self-test D:/PhysimTest/Gemischt language_mixed
```

Der Elternordner muss existieren. Die Tests prüfen den absichtlichen Typfehler,
die ursprüngliche Quelldiagnose, den korrigierten nativen Build, Simulation,
gespeicherte Originalquelle, bestehende C-Auswertung, Exporte und Projektneuladen.
Für die kleinere Oberfläche `PHYSIM_TEST_SMALL=1` setzen. Der Compiler `physimc`
wird als Abhängigkeit der App gebaut und mit dem SDK installiert.
`language_full` prüft zusätzlich einen absichtlichen Fehler in `analysis.phys`,
die Korrektur, den Sprachbericht mit zwei Diagrammen, Exporte und den unveränderten
Analyse-Quellsnapshot sowie beide Sprachdateien nach erneutem Projektöffnen.
`language_mixed` prüft denselben Analyseablauf mit einem C-Pendel als Experiment.

Die reproduzierbare IPC-Mutationskampagne ist als CTest `protocol_mutations`
registriert. Ablauf, Invarianten und Reproduktion stehen in [fuzzing.md](fuzzing.md).

```powershell
cmake --build build --config Release --parallel
cmake --install build --config Release --prefix build/package
cd build
cpack -C Release
```

Das Paket enthält die App, Runner, Beispielmodule, SDK-Quellen und Header, Vorlagen,
Dokumentation und Lizenzen. Ein C17-Compiler muss für Nutzerprojekte auf dem Zielsystem
eingerichtet sein. Ein Clean-Machine-Test und Linux-Distributionspakete sind noch offene Releasegates.

### macOS-App-Paket

Auf einem Mac mit abgeschlossenem Build erzeugt dieser Befehl ein neues App-Paket:

```sh
python3 tools/package-macos.py --build build --config Release --output build/Physim.app
open build/Physim.app
```

Das Ziel darf noch nicht existieren. Das Paket enthält App, Runner, Compiler,
SDL und das SDK; relative Bibliothekspfade erlauben das Verschieben. Die Ressourcen
liegen in `Contents/Resources`. Neue Projekte werden unter `Dokumente/Physim`
vorgeschlagen und können in einem anderen gewählten Ordner angelegt werden.
Die lokale Ad-hoc-Signatur wird geprüft. Für eine öffentliche Verteilung mit
Developer ID und Notarisierung fehlen noch die Apple-Entwicklerzugänge.
Die macOS-CI verschiebt das Paket in einen Pfad mit Leerzeichen und Umlaut und
prüft daraus vollständige C- und Physim-Sprachprojekte.
Erfolgreich geprüfte Pakete stehen im jeweiligen CI-Lauf als
`physim-app-macos-15` (Apple Silicon) oder `physim-app-macos-15-intel` bereit.
