# Plattformprüfung

Stand: 28. September 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

## Linux

**Ubuntu 24.04 mit GCC und Clang ist für die geprüften Arbeitsabläufe bestätigt.**
Die erfolgreichen Jobs für
[GCC](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553336)
und [Clang](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054552812)
prüfen den Quellstand `0a27cef` mit AddressSanitizer und UndefinedBehaviorSanitizer:

- 276 Tests ohne Fenster, einschließlich Sprachcompiler, Physik, Daten,
  Runner, nativen Projektbuilds und Fehlerbehandlung.
- 35 Grafik- und Fensterabläufe mit X11, Xvfb, Openbox und Mesa: darunter
  Menüwechsel, Tabs, Fensterknöpfe, Workspace, Dokumente, Wiederherstellung,
  Projekteinstellungen und C-/Physim-Projekte.
- Verschobenes installiertes SDK: Vorlagen und Sprachbeispiele neu bauen,
  Experimente und Analysen ausführen, Messdateien und Berichte prüfen.
- Acht vollständige C-App-Abläufe: Pendel, Wurf, Stoß, Bodenkontakt, Feder,
  unsicherer Wurf, Boxstoß und Auftrieb. Die Projektpfade enthalten Leerzeichen
  und Unicode. Der Ablauf umfasst Buildfehler, korrigierten Build, Simulation,
  Pause/Einzelschritt, Analyse, Exporte und Wiederöffnung gespeicherter Ergebnisse.

Nutzerprojekte werden direkt mit `physim-build` gebaut und benötigen keine
`CMakeLists.txt`. Quellen und `physim.project` bleiben im Projektordner;
Buildprodukte liegen in `build/Debug` beziehungsweise `build/Release`.

Wayland, weitere Distributionen, reale Linux-Grafiktreiber und ein fertiges
Installationspaket auf einem frischen Zielsystem sind noch nicht abgenommen.
Äußere Fensterecken hängen unter Linux vom Desktop ab; die Windows-DWM-Rundung
ist kein plattformübergreifender Nachweis.

## Windows

Die [CI-Matrix für `0a27cef`](https://github.com/PhysicSimulator/physim/actions/runs/36459604565)
bestand in allen vier Windows-Jobs: MSVC und ClangCL jeweils in Debug und Release,
einschließlich Tests und installiertem SDK. Fenster- und Grafiktests werden
zusätzlich lokal unter Windows ausgeführt; die GitHub-Windows-Worker garantieren
keinen OpenGL-3.3-Treiber.

## macOS

macOS auf Apple Silicon und Intel ist ein verbindliches Plattformziel.
Die CI verwendet `macos-15` und `macos-15-intel` mit Apple Clang und SDL 3.2.30.
Für `0a27cef` haben der
[Apple-Silicon-Job](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553102)
und der [Intel-Job](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553310)
alle 276 Tests ohne Fenster, 35 Grafik-/Fenstertests, das verschobene SDK,
und die acht C-App-Abläufe bestanden. Die Paket-Signierreihenfolge wurde anschließend
für Intel korrigiert. Der separate
[Paketlauf für `2de20b6`](https://github.com/PhysicSimulator/physim/actions/runs/36462932066)
bestand auf beiden Architekturen: neu gebaut, signiert, Signatur geprüft,
in einen Pfad mit Leerzeichen und Umlaut verschoben und daraus vollständige
C- und Physim-Sprachprojekte gebaut, simuliert und ausgewertet.

Die Implementierung berücksichtigt Darwins Programmpfaderkennung, Mach-O-Module,
lokalisierte Zahlenkonvertierung, macOS-Systemschriften und OpenGL 4.1 Core.
`tools/package-macos.py` erzeugt ein verschiebbares `.app`-Paket mit SDL und SDK.
Die lokale Ad-hoc-Signatur wird geprüft; Developer-ID-Signierung und Notarisierung
für eine öffentliche Verteilung sind noch offen. Die CI prüft vollständige C- und
Physim-Sprachprojekte aus dem verschobenen Paket.
Die geprüften ZIP-Dateien stehen im Paketlauf als `physim-app-macos-15`
(Apple Silicon) und `physim-app-macos-15-intel` bereit.
Die CI-Pakete sind Debug-Entwicklungsstände. Weitere macOS-Versionen, echte
Mac-Grafikhardware und Installation auf einem frischen Mac bleiben separate
Abnahmen; die gehosteten Grafiktests verwenden Apples Software Renderer.

## Direkter Build von Physim

Der Quellstand `7f05668` ergänzt `tools/build.py`. Im
[CI-Lauf](https://github.com/PhysicSimulator/physim/actions/runs/36467398347)
hat der Schritt **Direct compiler build without CMake for Physim** auf allen acht
Kombinationen bestanden: Windows mit MSVC/ClangCL jeweils Debug/Release,
Ubuntu 24.04 mit GCC/Clang sowie macOS 15 auf Apple Silicon/Intel.
Dies belegt diesen abgeschlossenen Schritt; die übrigen Schritte des Laufs
waren beim Erfassen dieses Nachweises noch aktiv.

Die Prüfung baut Bibliotheken, Sprachcompiler, Runner, Projektbuilder und App
mit direkten Compiler-/Archiviereraufrufen. Drei Referenzprogramme prüfen Core,
Numerik und Mechanik. Ein separater Test mit echten Compilerprozessen prüft
Unicodepfade, Headeränderungen, unveränderte Ausgaben, Fehlerwiederaufnahme,
beschädigte Ausgabedateien und die Buildsperre. Linux und beide Macs führen
zusätzlich vollständige Pendel- und Physim-Sprachprojekte in der direkt gebauten
App aus. Windows-Grafikabläufe werden lokal geprüft: beide vollständigen
Projektabläufe mit MSVC Debug sowie Renderer und Eingaberegression mit Clang.
Der lokale Projektbuilder-Test besteht mit den direkt gebauten Werkzeugen für
C- und Physim-Projekte in Debug/Release, einschließlich Projektverschiebung.

Die vollständige Testsuite und SDK-/App-Paketierung sind weiterhin CMake-gestützt.
SDL wird für diese CI-Prüfung zuvor mit seinem eigenen CMake-Buildsystem gebaut.
Der direkte Physim-Build ist daher noch keine vollständige Abnahme des Ziels,
CMake aus der gesamten Entwicklung und Auslieferung zu entfernen.

## Direkte SDK- und App-Pakete

Für `d9f920e` hat der Schritt **Native SDK package and relocation** im
[Paketprüflauf](https://github.com/PhysicSimulator/physim/actions/runs/36470030478)
auf allen vorgesehenen Plattformen bestanden:

| Umgebung | Nachweis |
| --- | --- |
| Windows MSVC Release | [Job 109089656019](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089656019) |
| Windows ClangCL Release | [Job 109089656123](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089656123) |
| Ubuntu 24.04 GCC | [Job 109089655982](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655982) |
| Ubuntu 24.04 Clang | [Job 109089655923](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655923) |
| macOS 15 Apple Silicon | [Job 109089655991](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655991) |
| macOS 15 Intel | [Job 109089655751](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655751) |

Der direkte Installer verwendet kein CMake. Die SDK-Prüfung verschiebt das Paket,
prüft seine Inhaltsprüfsummen, bindet alle öffentlichen Header einzeln ein und
kompiliert einen unabhängigen Prüfer gegen die installierte Kernbibliothek.
Alle acht mitgelieferten C-Beispielmodule und 15 neu übersetzte Sprachprogramme
werden ausgeführt. Anschließend baut `physim-build` alle acht C-Vorlagen und ein
Physim-Projekt aus den installierten Quellen neu. Der Prüfer kontrolliert die
Messdateien mit jeweils 201 Zeilen und die erzeugten Berichte.

Linux und beide Macs bestehen zusätzlich vollständige C-/Physim-App-Abläufe aus
dem verschobenen SDK. Beide Mac-Jobs erzeugen daraus eine `.app`, prüfen deren
Ad-hoc-Signatur und führen die App-Abläufe erneut aus diesem Paket aus.
Windows besteht die Grafikabläufe lokal auf der RTX 2080 Ti; die gehosteten
Windows-Paketprüfungen verwenden die Kommandozeilenprogramme.

Die heruntergeladenen Linux-/Mac-Archive wurden zusätzlich auf Dateiintegrität,
ausführbare Dateirechte, Architektur und SDK-Inhalt geprüft. Mac-Bundles enthalten
die relative SDK-Verknüpfung und SDL. Artefakte im Lauf:
`physim-native-windows-v143`, `physim-native-windows-ClangCL`,
`physim-native-linux-gcc`, `physim-native-linux-clang`,
`physim-native-macos-15`, `physim-native-macos-15-intel`.
Die Mac-/Linux-CI-Pakete sind Debug-Builds; Windows verwendet Release.
Weitere Betriebssystemversionen, Clean-Machine-Abnahme und öffentliche
Mac-Notarisierung bleiben offen.

## Direkter Testläufer

Der direkte Katalog umfasst **493 Tests ohne Fenster**, davon 482 ohne SDL,
und zusätzlich **35 Fenster- und Grafiktests** unter `--test-display`.
Sieben zuvor übertragene Prüfungen
für Tutorialquellen, API-Referenz, PNG-Dekodierung und Projektbuild bestehen lokal
mit MSVC Debug, Clang Release und über CTest. Zum Stand `a051ea0` besteht auch der
direkte Build- und Testschritt in allen acht CI-Kombinationen.
Die vorherige Erweiterung übernimmt 21 Runner-, Physik- und Dokumentationsabläufe.
Alle 21 bestehen lokal mit
MSVC Debug und Clang Release; alle 47 gemeinsamen CTest-Integrationsprüfungen
bestehen ebenfalls. Für den CTest-Vergleich wurden die vorhandenen Runner neu
gebaut, nachdem zwei Crash-Tests mit veralteten Programmen fehlgeschlagen waren.
Die 419 direkten Sprachtests bestanden bereits zum vorherigen Stand `b95e119` mit
beiden Compilern. Zum Stand `7e38cf1` bestehen alle 484 damaligen Tests in allen
acht CI-Kombinationen. Der anschließende Build ohne Tests scheitert dort an der
Argumentprüfung für `--test-filter`; der direkte CI-Schritt ist daher fehlgeschlagen.
Die Korrektur besteht lokal für normale Builds mit MSVC und Clang. Ein zusätzlicher
Regressionstest prüft normale Builds, SDK-Installation und Testfilter über den
Kommandozeileneinstieg. Die Korrektur ist mit dem direkten CI-Schritt zu `a051ea0`
auf allen acht Kombinationen bestätigt.

Alle 34 neuen Grafikabläufe sind lokal mit MSVC Debug auf der RTX 2080 Ti geprüft.
Der erste Lauf bestand 30 Fälle; nach Korrektur der UTF-8-Ausgabe und der Vorbereitung
zweier Testordner bestanden die vier betroffenen Fälle ebenfalls. Die Prüfungen
behalten Menü-, Dokument-, Wiederherstellungs-, Projekteinstellungs- und Sprachabläufe
bei. Beide Diagrammtests prüfen zusätzlich alle PNG-Exporte mit unabhängigen
Pixelvergleichen. Acht ausgewählte Abläufe bestehen außerdem mit der Clang-Release-App:
Auftrieb, Diagramme, Stapelläufe, Dokumentbuild, Projekteinstellungen,
Dokumentwiederherstellung, Autosave und der vollständige Sprachablauf.
Die Projekt-Buildprotokolle bestätigen dabei Clang als Compiler.
Drei zusätzliche CTest-Vergleiche für `toolbar_small`, `plot_workflow` und
`language_mixed_workflow` bestehen mit den neu gebauten MSVC-Debug-Programmen.
Im [CI-Lauf zu `a621b5b`](https://github.com/PhysicSimulator/physim/actions/runs/36488141869)
besteht der direkte Grafikschritt auf macOS Apple Silicon. Linux GCC und Clang
bestehen jeweils 33 von 34 Fällen; der erste Auftriebstest endet nach 150 Sekunden
ohne App-Ausgabe im Timeout. Die Ursache ist noch offen. Der Linux-Aufruf wartet
nun vor dem App-Start auf den Fenstermanager; zusätzliche App-Testprotokolle halten
Initialisierung und Phasenwechsel auch bei Zeitüberschreitung fest. Die erneute
Linux-CI und der Grafiknachweis für macOS Intel standen bei diesem Stand aus.

Im [Folgelauf zu `37eef01`](https://github.com/PhysicSimulator/physim/actions/runs/36490044371)
besteht der direkte Grafikschritt mit allen 35 Fällen unter Linux GCC und Clang
sowie macOS Apple Silicon und Intel. Damit besteht dort auch der neue UI-Benchmark.
Der erste Linux-Auftriebstest läuft mit der Fenstermanager-Wartebedingung durch;
ein eindeutiger Ursachennachweis für den vorherigen Timeout folgt daraus nicht.
Auch der direkte Build- und Testschritt mit 493 Tests besteht auf allen acht
Plattformkombinationen dieses Stands.

Die drei ergänzten Benchmark-Prüfungen bestehen lokal mit MSVC Debug, Clang Release
und über CTest. Der SDL-freie MSVC-Release-Build besteht beide Prüfungen ohne Fenster.
Die Referenzbilder für leere Ansicht, viele Bedienelemente und acht Kurven sind
zwischen den beiden Compilern bytegleich. In allen drei Messfällen entstehen nach
dem Aufwärmen keine weiteren Zeichenpuffer-Allokationen. Die bisherigen Prüfungen
für Export, Größenwechsel, Referenzwerte und Vergleichsfehler bleiben erhalten.
Dies sind Funktionsprüfungen; die parallel zu Builds gemessenen Zeiten sind keine
Performance-Baseline. Der direkte Grafikschritt des Folgelaufs bestätigt den
UI-Benchmark inzwischen auf den oben genannten vier CI-Kombinationen.

Der [Linux-GCC-Lauf zu `c92dbd3`](https://github.com/PhysicSimulator/physim/actions/runs/36491788018)
besteht erneut alle 34 App-Abläufe, scheitert aber im UI-Benchmark am Größenwechsel.
Das archivierte `empty-restored.bmp` hat noch 640 × 480 statt 1080 × 740 Pixel.
Der Benchmark wartet nun mit `SDL_SyncWindow` auf die Größenänderung und prüft
die tatsächliche Fenstergröße, bevor er zeichnet. Lokal besteht die korrigierte
Prüfung mit MSVC Debug und Clang Release. Der erneute Linux-Nachweis steht noch aus.

Der direkte Builder unterstützt zusätzlich `--sanitizers`: ASan unter Windows,
ASan und UBSan unter Linux/macOS. Lokal bestehen mit MSVC die positive
Instrumentierungsprobe (fehlerfreier Lauf und erkannter Heap-Pufferüberlauf) sowie
zehn Tests für Core, Speicherbesitz, Berichte, Protokoll-/Messdateimutationen und
die vier Sprachspeicherprüfungen. Die Ergebnisdatei liegt unter
`build/native/Debug-sanitized/test-results/run-1t_t37re/results.json`.
ClangCL 19.1.5 scheitert lokal schon beim Start des sicheren ASan-Probeprogramms
in seiner Interception-Laufzeit. Deshalb ist dafür keine lokale Abnahme belegt.
Im [CI-Lauf zu `c92dbd3`](https://github.com/PhysicSimulator/physim/actions/runs/36491788018)
bestehen die Sanitizer-Probe und alle zehn ausgewählten Tests unter Windows
mit MSVC und ClangCL. Die Linux-/macOS-Sanitizer-Abnahme steht noch aus.

Der [CI-Lauf zu `a9accf5`](https://github.com/PhysicSimulator/physim/actions/runs/36492484066)
bestätigt außerdem den direkt gebauten IPC-libFuzzer unter Windows ClangCL und
Linux Clang. Beide Prüfungen erzeugen gültige Eingaben, spielen sie erneut ab
und bestehen 10.000 Durchläufe mit zusätzlicher Codeabdeckung ohne Fehlerfund.
Der Apple-Silicon-Job scheitert mit Apple Clang aus Xcode 16.4 am Linken, weil
`libclang_rt.fuzzer_osx.a` in der Toolchain fehlt. Für die Fuzzer-Prüfung wird nun
Homebrew LLVM 20 auf beiden Mac-Architekturen verwendet. Normale App- und
Sanitizer-Builds verwenden weiter Apple Clang.

Im [Lauf zu `0d10940`](https://github.com/PhysicSimulator/physim/actions/runs/36493259775)
besteht die Fuzzer-Kampagne mit LLVM 20 auf macOS Intel. Auf
Apple Silicon meldet Apples Linker `invalid r_symbolnum=1` für instrumentierte
Objekte. Der direkte Fuzzer-Build verwendet nun LLVMs Mach-O-Linker `ld64.lld`;
im [Lauf zu `f6cedbf`](https://github.com/PhysicSimulator/physim/actions/runs/36494602802)
bestehen damit beide Mac-Fuzzer-Kampagnen einschließlich 10.000 Durchläufen.

Die direkte Linux-Clang-Sanitizer-Suite zu `c92dbd3` besteht 492 von 493 Tests.
UBSan findet im Clipboard-Test einen `memcmp`-Aufruf mit Nullzeiger bei Länge null.
Der Vergleich behandelt leere Texte nun vor dem Speichervergleich; lokal bestehen
die Clipboard-Prüfungen mit MSVC Debug und Clang Release. Der Mac-ARM-Lauf desselben
Stands besteht 479 von 482 Sanitizer-Tests. Beim erneuten Laden von Modulen melden
zwei Fälle doppelt registrierte ASan-Globals; `template_ids` bricht in der
ASan-Registrierung ab. Diese Mac-Sanitizer-Ursache bleibt in Untersuchung.
Eine zusätzliche Minimalprüfung lädt zwei instrumentierte Module je 32-mal,
kontrolliert das Zurücksetzen veränderter Globals und verlangt anschließend
einen erkannten globalen Pufferüberlauf. Lokal besteht sie mit MSVC unter
`build/native/sanitizer probe ä edntxgm2`; der Mac-Nachweis steht noch aus.

Die korrigierte UI-Größenprüfung besteht im direkten Grafikschritt unter Linux
GCC zu `0d10940`. Im vorherigen GCC-Lauf zu `49efcbb` besteht der UI-Benchmark,
aber der erste App-Ablauf hängt erneut vor der Meldung „window and OpenGL ready“.
Zusätzliche Tracepunkte unterscheiden nun SDL-Initialisierung, Fenstererzeugung
und GL-Kontext. Der Wartepunkt auf den Fenstermanager allein behebt diesen
sporadischen Startfehler damit nicht vollständig.

Die direkte SDK-Prüfung umfasst jetzt auch alle zuvor nur im CMake-SDK-Vergleich
gebauten Sprachmodule: 27 Module, neun Sprachexperimente mit beiden allgemeinen
Analysen, spezielle Sensoranalyse und sechs C-/Physim-Kombinationen.
Die installierten Core-Quellen und alle acht C-Vorlagen samt Analyse werden
unabhängig von den ausgelieferten Bibliotheken neu gebaut. Ein SDL-freies
MSVC-Release-SDK besteht die vollständige erweiterte Prüfung unter
`build/native/Native SDK ä cnycd4y3`. Das Clang-Release-SDK besteht zusätzlich
neun Projektbuilds und beide grafischen Abläufe unter
`build/native/Native SDK ä elwayubq`. Im Lauf zu `f6cedbf` besteht der erweiterte
direkte SDK-Schritt bereits unter Linux GCC/Clang, macOS Apple Silicon und
Windows ClangCL Release. Die übrigen Plattformjobs laufen noch.

Die Pendelreferenzen prüfen jeweils 4001 Messpunkte, Energiedrift und Periodendauer
gegen eine analytische Referenz. MSVC und Clang liefern für RK4 eine Periodendauer
von 2,48880586925 s bei einer Referenz von 2,48880587159 s. Die Prüfung abgeleiteter
Daten vergleicht vollständiges CSV und Bericht; der maximale Geschwindigkeitsfehler
beträgt dabei rund 5,59e-5 m/s. Crash-, Hang- und Batch-Prüfer kontrollieren zusätzlich
Prozessende, Datenwiederherstellung und das Aufräumen paralleler Kindprozesse.

Die Analyseprüfungen vergleichen den vollständigen CSV-Inhalt und verlangen,
dass ungültige Exporte keine CSV-Datei anlegen. Der Läufertest prüft erwartete
Exitcodes und Diagnosen, Zeitüberschreitungen, fehlende Programme, Emissions-/
Buildfehler, fehlende Ausgaben, falsche Inhalte und ungültiges UTF-8. Fehlerhafte
Schritte verhindern die Ausführung davon abhängiger Programme. Die Ergebnisberichte
enthalten für erwartete Dateien außerdem Größe und SHA-256-Prüfsumme.
Die Integrationsprüfungen erfassen zusätzlich die Prüfsummen aller verwendeten
Programme und Module. Ihre C-Prüfer kontrollieren Messwerte, Metadaten, Szenen,
Berichte und das Verhalten bei ungültigen Parametern. Tests der Ablaufsteuerung
verhindern Erfolge durch veraltete Prüfer nach C-/Physim-Buildfehlern oder fehlende
Ausgabedateien.

Die folgende Tabelle dokumentiert abgeschlossene **direkte Build- und Testschritte**.
Sie nimmt spätere SDK-, Grafik- oder CTest-Schritte desselben CI-Laufs nicht vorweg.
Die acht Kombinationen sind Windows MSVC/ClangCL jeweils Debug/Release, Linux
GCC/Clang sowie macOS auf Apple Silicon/Intel.

| Stand | Direkte Tests | Ausgeführter CI-Nachweis |
| --- | --- | --- |
| `a55d5e0` | 51 C-Tests | Alle acht Kombinationen: [Lauf 36471876255](https://github.com/PhysicSimulator/physim/actions/runs/36471876255) |
| `199a8f7` | 73 Tests | Alle acht Kombinationen: [Lauf 36473305112](https://github.com/PhysicSimulator/physim/actions/runs/36473305112) |
| `430df15` | 134 Tests | Alle acht Kombinationen: [Lauf 36474491263](https://github.com/PhysicSimulator/physim/actions/runs/36474491263) |
| `0a937f8` | 152 Tests | Alle acht Kombinationen: [Lauf 36475496512](https://github.com/PhysicSimulator/physim/actions/runs/36475496512) |
| `d7fb0b0` | 200 Tests | Alle acht Kombinationen: [Lauf 36476324664](https://github.com/PhysicSimulator/physim/actions/runs/36476324664) |
| `0923b6e` | 222 Tests | Alle acht Kombinationen: [Lauf 36477404252](https://github.com/PhysicSimulator/physim/actions/runs/36477404252) |
| `8516187` | 228 Tests | Alle acht Kombinationen: [Lauf 36478529900](https://github.com/PhysicSimulator/physim/actions/runs/36478529900) |
| `2b3b6e7` | 437 Tests | Alle acht Kombinationen: [Lauf 36480859045](https://github.com/PhysicSimulator/physim/actions/runs/36480859045) |
| `b95e119` | 463 Tests | Alle acht Kombinationen: [Lauf 36482481623](https://github.com/PhysicSimulator/physim/actions/runs/36482481623) |
| `a051ea0` | 491 Tests, Build ohne Testfilter korrigiert | Alle acht Kombinationen: [Lauf 36485408557](https://github.com/PhysicSimulator/physim/actions/runs/36485408557) |

Die ursprünglichen 51 C-Tests bestanden auch lokal mit MSVC Debug; die 41 SDL-freien
Fälle zusätzlich mit Clang Release. Der vollständige Satz mit 134 Tests bestand
lokal mit MSVC Debug. Die CMake-CI bleibt bis zur vollständigen Übertragung und
Abnahme ihrer Sanitizer- und SDK-Prüfungen erhalten.

## Weitere Änderungen prüfen

Jeder Push startet die [CI](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Die jeweiligen Jobs und Artefakte zeigen den geprüften Commit. Linux-Screenshots
und das letzte CTest-Protokoll werden als `linux-ui-gcc` beziehungsweise
`linux-ui-clang` archiviert. SDK-Protokolle liegen in eigenen Artefakten.
