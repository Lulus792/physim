# Plattformprüfung

Stand: 4. Oktober 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

## Lokaler Intel-Mac am 4. Oktober 2026

macOS 14.6.1 (23G93), Intel x86_64, Apple Clang 16.0.0 und lokal aus
`release-3.2.30` gebautes SDL prüfen jetzt zusätzlich die Workspace-Dokumentansichten.
Der Debug-Modelltest besteht mit Format-1-Kompatibilität, maximalen Pfad-/Dokumentgrenzen,
abgeschnittenen und manipulierten Dateien sowie Erhaltung des bisherigen Zustands
bei Fehlern (`build/workspace-check/test-results/run-80qn96q7`). Die Fensterprüfung
startet getrennte App-Prozesse für Projekt-/Ordner-Wiederöffnung, aktive Dokumente,
Analyse-/Simulationsansicht, fehlende und extern gekürzte Unicode-Dateien sowie
vorhandene Autosaves. Dateibaum, beide Dokumentfenstergrößen, Buildinvalidierung
und Dokument-Autosaves sind zusätzlich geprüft. Diese lokalen Prüfungen sind kein
neuer Linux- oder Apple-Silicon-Nachweis. Alle sechs Fensterfälle bestehen unter
`build/workspace-check/test-results/run-sg5hhnt6`.

Der SDL-freie Gesamtlauf des Ausgangsstands `42d1f9e` besteht 480 von 482 Fällen
(`build/native/Debug/test-results/run-vsag2ez2`).
`language_function_values_runtime` wird vor der Programmausführung mit Signal 9
beendet; das macOS-Systemprotokoll nennt `AppleSystemPolicy` und
`Security policy would not allow process`. Eine separat ad hoc signierte Kopie
besitzt eine gültige Signatur, wird aber ebenfalls blockiert. Eine Ursache im
Sprachprogramm ist damit nicht nachgewiesen. `batch_reference` scheitert an der
Prüfung der abgeschlossenen Läufe bzw. der Fünf-Sekunden-Grenze; die Fehlerjournale
nennen Runner-Zeitüberschreitungen. Beide gezielten Wiederholungen scheitern erneut
(`run-6x4lxg2j`).

Beide Menüfenstergrößen scheitern in einem gemeinsamen lokalen Lauf; die jeweils
isolierten Wiederholungen bestehen (`run-t9h7c5ep`, `run-zij55d1b`). Eine separat
aus `42d1f9e` exportierte und gebaute App scheitert ebenfalls im kleinen Menütest
(`build/baseline-app/test-results/run-mozmzzn3`). Die wechselnden Fehlerstellen
und eine mögliche Beeinflussung durch native Fokus-/Mausereignisse bleiben offen.
Die bisherigen macOS-15-CI-Ergebnisse belegen keine Fehlerfreiheit auf macOS 14.6.1.

Die anschließende Diagnose zeigt beim Batchtest für alle drei Fehlerfälle
Zeitüberschreitungen nach ungefähr 0,25 Sekunden, einschließlich des normalen
Schemafixtures. Die Fehlerprüfungen lassen Absturz und Schemaprüfung nun fünf
Sekunden zum Starten, verlangen ausdrücklich `PS_IO` bzw. `PS_INVALID` und behalten
für den Hängefall 0,25 Sekunden und `PS_LIMIT`. Die bisherige Gesamtdauergrenze
von fünf Sekunden sowie die Abschlusszahlen bleiben geprüft. Alle drei Fehlerarten
bestehen (`build/native/Debug/test-results/run-bjamthli`); die parallele Batchreferenz
besteht separat (`run-mkysw5d1`).

Natürliche Maus-/Fokusereignisse sind im Menütest nach dem Hilfefenster protokolliert.
Absichtlich eingeschobene Ereignisse reproduzieren den Prüffehler unter
`build/toolbar-noise-before.log`. Die Teststeuerung isoliert jetzt ihre Mauskennung
auch in den Menüfällen. Mit zusätzlichen Maus-, Mausrad- und Fokusereignissen
bestehen die vollständigen kleinen und großen Menüabläufe, die normalen Menütests,
Plot-Eingabeisolation und Fensterstart (5/5 Fälle unter
`build/workspace-check/test-results/run-23gu5c67`). Der SDL-Eingabetest prüft weiterhin
die normale Freigabe gehaltener Tasten/Maustasten bei Fokusverlust.

Der anschließende vollständige lokale Debug-Build besteht 492 von 493 Tests ohne
Fenster (`build/workspace-check/test-results/run-7288kqxh`), einschließlich
Batchtests, nativen Projektbuilds und Benchmarks. Ausschließlich
`language_function_values_runtime` bleibt durch macOS-Systempolitik blockiert.
Dasselbe Sprachprogramm besteht lokal im Release-Build
(`build/native/Release/test-results/run-203vuguu`).

Die Linux-Paketprüfung zu `1b098d7` baut das Paket und besteht dessen Start sowie
SDK und alle neun GUI-Projektabläufe auf Debian 12 und Ubuntu 24.04. Beide Jobs
scheitern weiterhin am nativen Datei-/Ordnerdialog:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/37223668792/job/111500303931),
[Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/37223668792/job/111500303988).
Der Dialogprüfer veröffentlicht künftig die tatsächliche Fehlerphase und SDL-Ausgabe
auch als Jobannotation. Die genaue Ursache dieses Laufs ist ohne dessen
authentifizierte Protokolle noch nicht geprüft; die Dialogabnahme bleibt offen.

## Windows-ClangCL-Sanitizer

Aktuell offener Fehler: Im
[Windows-ClangCL-Debug-Job zu `0321c85`](https://github.com/PhysicSimulator/physim/actions/runs/36504185474/job/109201756506)
bestehen alle 493 normalen Tests einschließlich der neuen Projektcache-Reparatur.
Die Sanitizer-Probe besteht ebenfalls; danach endet der instrumentierte Core-Test
mit Windows-Zugriffsverletzung `0xC0000005` ohne Diagnoseausgabe. Die übrigen neun
Sanitizer-Tests bestehen. Die Ursache dieses Absturzes ist noch ungeklärt;
der gesamte Job ist damit fehlgeschlagen. Das lokale Protokoll liegt unter
`build/ci-cache-clang-debug.log`. Der parallele
[Job desselben Commits im zweiten Repository](https://github.com/Lulus792/physim/actions/runs/36504182060/job/109201744452)
besteht alle zehn Sanitizer-Tests mit derselben Windows-Image-Version
`20260920.314.1`. Der Fehler tritt damit nicht in jedem Lauf auf; die Ursache
bleibt offen. Das Vergleichsprotokoll ist `build/ci-cache-clang-debug-origin.log`.

Lokal scheitert der Core-Test mit ClangCL 19.1.5 ebenfalls mit `0xC0000005`;
hier meldet die Laufzeit zusätzlich eine unbekannte Interceptor-Instruktion.
Der mit [Microsoft ProcDump](https://learn.microsoft.com/en-us/sysinternals/downloads/procdump)
erfasste Minidump zeigt die Fehleradresse in `clang_rt.asan_dynamic-x86_64.dll`
bei Offset `0x4859b`, innerhalb von `__asan_region_is_poisoned`.
Das ist noch kein Nachweis derselben Ursache wie im CI-Lauf ohne Diagnoseausgabe.
Protokolle: `build/clang-asan-core.log`, `build/clang-asan-core-verbose.log`;
Speicherabbild: `build/clang-asan-dumps/physim-test-core.exe_260929_030111.dmp`.

Die Windows-Debug-Jobs konfigurieren nun
[WER-Minidumps pro Testprogramm](https://learn.microsoft.com/en-us/windows/win32/wer/collecting-user-mode-dumps)
für `physim-test-core.exe` und einen eigenen Absturzprüfer. Der Prüfer verlangt
eine echte Zugriffsverletzung und denselben Fehlercode im erzeugten Minidump.
Das Artefakt `windows-crash-diagnostics-ClangCL` beziehungsweise
`windows-crash-diagnostics-v143` enthält Dumps, Testprogramm, PDB und Laufzeit-DLLs.
Die Aufnahme erfolgt während des ursprünglichen Testlaufs; fehlerhafte Tests
werden nicht automatisch wiederholt. Lokal sind der Prüfer kompiliert, sein
Absturz mit ProcDump erfasst und die Dump-Auswertung geprüft
(`build/crash capture check 1tq7fixm`). Im Lauf zu `620246b` bestehen die echte
WER-Aufzeichnung und alle zehn Sanitizer-Tests sowohl mit
[ClangCL](https://github.com/PhysicSimulator/physim/actions/runs/36506432739/job/109208791289)
als auch mit [MSVC](https://github.com/PhysicSimulator/physim/actions/runs/36506432739/job/109208791471).
Die Protokolle sind `build/ci-wer-clang.log` und `build/ci-wer-msvc.log`.
Der ursprüngliche sporadische Absturz gilt damit noch nicht als behoben.
Lokal wurden keine administrativen WER-Einstellungen verändert.

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

Das Release-Paket besteht inzwischen auch die unten beschriebene Prüfung auf
frischen Debian-12- und Ubuntu-24.04-Systemen. Wayland, weitere Distributionen,
reale Linux-Grafiktreiber und die Integration in einen echten Desktop bleiben offen.
Äußere Fensterecken hängen unter Linux vom Desktop ab; die Windows-DWM-Rundung
ist kein plattformübergreifender Nachweis.

### Release-Paket auf frischen Linux-Systemen

Der zusätzliche Workflow `linux-package.yml` baut ein Release-SDK mit GCC unter
Debian 12 (`debian:bookworm`) und prüft dort alle Tests ohne Fenster. Das Archiv
`Physim-linux-x86_64.tar.gz` wird anschließend in zwei neue Container entpackt:
Debian 12 und Ubuntu 24.04. Diese Prüfungen haben keinen Repository-Checkout,
keine SDL-Entwicklungsdateien und kein CMake. Zunächst werden nur Desktopbibliotheken
installiert und die Oberfläche ohne Compiler oder Python gestartet. Anschließend
kommen ein C17-Compiler für Nutzerprojekte und Python für die Teststeuerung hinzu.

Ein separates Prüfpaket enthält nur den SDK-Prüfer, dessen Compilerhelfer und
kleine Prüfquellen. Der Prüfer verwendet die Header, Bibliotheken, Vorlagen und
Kernquellen aus dem verschobenen SDK. Er baut alle C-/Physim-Beispiele und Projekte
und führt alle acht C-Vorlagen sowie den vollständigen Physim-Sprachablauf in der
Oberfläche aus. Die Systempakete, aufgelösten App-Bibliotheken, Laufprotokolle und
Screenshots werden als `installed-linux-debian-12` und `installed-linux-ubuntu-24.04`
archiviert. Das Release-Archiv heißt als CI-Artefakt `physim-linux-release-x86_64`.

Der [Debian-12-Release-Build zu `0dbe24f`](https://github.com/PhysicSimulator/physim/actions/runs/36505336425/job/109205387725)
besteht alle 493 Tests ohne Fenster und erzeugt das Paket. Die anschließenden
beiden Installationsprüfungen stoppen vor dem App-Start, weil Openbox Python
als Abhängigkeit installiert hatte. Openbox wird jetzt erst nach dem Starttest
installiert; die Bedingung, dass Python beim ersten Start fehlt, bleibt erhalten.
Im Lauf zu `620246b` besteht der App-Start ohne Compiler, Python und CMake in
beiden frischen Systemen:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/36506432788/job/109210313653)
und [Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/36506432788/job/109210313628).
Auch die vollständige SDK-/Projektprüfung besteht auf beiden Systemen: verschobenes
SDK, unabhängige Header und Kernbibliothek, 15 Sprachprogramme, 27 Sprachmodule,
neun neu gebaute Projekte sowie alle acht C-Vorlagen und der vollständige
Physim-Sprachablauf in der Oberfläche. Die Protokolle liegen unter
`build/ci-installed-debian-620246b.log` und `build/ci-installed-ubuntu-620246b.log`.
Das heruntergeladene Paket zu `0dbe24f` hat außerdem die Prüfung der ZIP-/TAR-Struktur,
Ausführungsrechte der App und aller 224 Dateiprüfsummen bestanden.

Die Container verwenden X11, Xvfb und Mesa; sie prüfen die Paketabhängigkeiten
und Anwendungsabläufe, aber keine echte Grafikhardware oder Wayland-Sitzung.
Die native Datei-/Ordnerauswahl ist noch separat zu prüfen. Für Desktops ohne
XDG-Portal enthalten Installationsanleitung und folgende Paketläufe nun Zenity;
[SDL 3.2.30](https://github.com/libsdl-org/SDL/blob/release-3.2.30/src/dialog/unix/SDL_unixdialog.c)
verwendet Portal oder Zenity für diese Dialoge.

Auch der [Paketlauf zu `ae31aee`](https://github.com/PhysicSimulator/physim/actions/runs/36507609163)
besteht in beiden Distributionen mit Zenity als installierter Laufzeitabhängigkeit.
Der neue separate Dialogtest steuert echte Zenity-Fenster mit `xdotool`: Hauptordner
öffnen, externe Datei und zusätzlichen Ordner hinzufügen, anschließend abbrechen.
Alle ausgewählten Pfade enthalten Leerzeichen und Umlaute. Die App prüft die
übernommenen Pfade und den unveränderten Workspace nach Abbruch. Ein zweiter Lauf
erzwingt einen nicht vorhandenen Dialogtreiber und prüft die Fehlermeldung, ohne
den Workspace zu verändern. Protokolle, ausgewählte Pfade und ein abschließendes
Bild liegen im Artefakt unter `Native dialogs*`. Der Linux-Ausführungsnachweis
für diesen neuen Dialogtest steht noch aus.

Im [Dialoglauf zu `bd359c7`](https://github.com/PhysicSimulator/physim/actions/runs/36509517062)
bestehen auf beiden Systemen alle neun SDK-Oberflächenabläufe. Der Dialogprüfer
findet das erste Zenity-Fenster, bleibt aber nach der Ordnerpfadeingabe bis zum
Zeitlimit darin. Er bestätigt jetzt ausdrücklich mit Zenitys OK-Schaltfläche
(`Alt+O`), statt Enter im GTK-Pfadfeld zu verwenden, und archiviert Bilder vor
und nach der Eingabe sowie bei Fehlern. Die erneute Ausführung bleibt offen.
Protokolle: `build/native-linux-dialog-109220128459.log` und
`build/native-linux-dialog-109220128487.log`.

Der gleiche interaktive App-Test besteht lokal unter Windows mit MSVC Debug und
den tatsächlichen Systemdialogen (`build/native-dialog-windows-3.log`,
`build/native-dialog-windows-3/native-dialogs.bmp`). Der Pfadvergleich berücksichtigt
Windows-Pfadtrenner. Die macOS-Systemdialoge und Linux-Portaldialoge sind damit
noch nicht geprüft.

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
Die geprüften ZIP-Dateien stehen im damaligen Paketlauf als `physim-app-macos-15`
(Apple Silicon) und `physim-app-macos-15-intel` bereit. Neue direkte Builds verwenden
`physim-native-macos-15` beziehungsweise `physim-native-macos-15-intel`.
Die CI-Pakete sind Debug-Entwicklungsstände. Weitere macOS-Versionen, echte
Mac-Grafikhardware und Installation auf einem frischen Mac bleiben separate
Abnahmen; die gehosteten Grafiktests verwenden Apples Software Renderer.

Eine zusätzliche Paketprüfung startet jetzt die verschobene `.app` über
`/usr/bin/open -W -n -a`, also über macOS LaunchServices. Sie verwendet ein leeres
Arbeitsverzeichnis, den Systempfad `/usr/bin:/bin:/usr/sbin:/sbin` und entfernt
Entwickler-Vorgaben für Compiler, SDK und Bibliotheken aus der Startumgebung.
Die bisherigen Paketprüfungen starteten direkt `Contents/MacOS/physim`.
Der neue Prüfer verlangt für C und Physim einen vollständigen Oberflächenablauf,
einen erfolgreichen Abschluss nach dem Aufräumen der App, gebaute Module,
gespeicherte Messdaten/Berichte und Screenshots. Abschließend muss die Signatur
des Pakets weiterhin gültig sein. Protokolle und Bilder werden unter
`LaunchServices*` archiviert. Der tatsächliche macOS-Nachweis dieser neuen
Startvariante steht noch aus; native macOS-Dateidialoge bleiben separat offen.

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
Homebrew LLVM 20 auf beiden Mac-Architekturen verwendet. Normale App-Builds
verwenden weiter Apple Clang.

Im [Lauf zu `0d10940`](https://github.com/PhysicSimulator/physim/actions/runs/36493259775)
besteht die Fuzzer-Kampagne mit LLVM 20 auf macOS Intel. Auf
Apple Silicon meldet Apples Linker `invalid r_symbolnum=1` für instrumentierte
Objekte. Der direkte Fuzzer-Build verwendet nun LLVMs Mach-O-Linker `ld64.lld`;
im [Lauf zu `f6cedbf`](https://github.com/PhysicSimulator/physim/actions/runs/36494602802)
bestehen damit beide Mac-Fuzzer-Kampagnen einschließlich 10.000 Durchläufen.

Die direkte Linux-Clang-Sanitizer-Suite zu `c92dbd3` besteht 492 von 493 Tests.
UBSan findet im Clipboard-Test einen `memcmp`-Aufruf mit Nullzeiger bei Länge null.
Der Vergleich behandelt leere Texte nun vor dem Speichervergleich; lokal bestehen
die Clipboard-Prüfungen mit MSVC Debug und Clang Release. Im Lauf zu `f6cedbf`
bestehen alle 493 Sanitizer-Tests ohne Fenster unter Linux GCC und Clang.
Der Mac-ARM-Lauf zu `c92dbd3`
Stands besteht 479 von 482 Sanitizer-Tests. Beim erneuten Laden von Modulen melden
zwei Fälle doppelt registrierte ASan-Globals; `template_ids` bricht in der
ASan-Registrierung ab. Die folgende Minimalprüfung grenzt die Ursache ein.
Eine zusätzliche Minimalprüfung lädt zwei instrumentierte Module je 32-mal,
kontrolliert das Zurücksetzen veränderter Globals und verlangt anschließend
einen erkannten globalen Pufferüberlauf. Lokal besteht sie mit MSVC unter
`build/native/sanitizer probe ä edntxgm2`. Im
[Lauf zu `4339411`](https://github.com/PhysicSimulator/physim/actions/runs/36495445390)
besteht sie auch mit Apple Clang auf Intel, reproduziert aber auf Apple Silicon
die doppelte Registrierung von `probe_globals`. Die archivierte Assemblerdatei
zeigt dort `__mod_term_func` für den ASan-Destruktor. Die LLVM-Umstellung auf
[`__cxa_atexit`](https://reviews.llvm.org/D121327) vermeidet diese veraltete
Darstellung. Die macOS-Sanitizer-CI verwendet nun LLVM 20 und LLD wie der Fuzzer;
im [Lauf zu `69ddae7`](https://github.com/PhysicSimulator/physim/actions/runs/36495859541)
besteht die vollständige Minimalprüfung auf Apple Silicon und Intel, einschließlich
der 64 Modulöffnungen und des erkannten globalen Pufferüberlaufs nach erneutem
Laden. Auf beiden Architekturen bestehen außerdem alle 482 SDL-freien
Sanitizer-Tests sowie die direkten Build-, SDK- und Grafikschritte.

Die korrigierte UI-Größenprüfung besteht im direkten Grafikschritt unter Linux
GCC zu `0d10940`. Im vorherigen GCC-Lauf zu `49efcbb` besteht der UI-Benchmark,
aber der erste App-Ablauf hängt erneut vor der Meldung „window and OpenGL ready“.
Zusätzliche Tracepunkte unterscheiden nun SDL-Initialisierung, Fenstererzeugung
und GL-Kontext. Der Wartepunkt auf den Fenstermanager allein behebt diesen
sporadischen Startfehler damit nicht vollständig.
Zu `f6cedbf` bestehen unter Linux GCC und Clang zusätzlich alle 35 direkt gebauten
Fenster-/Grafiktests mit ASan und UBSan. Das belegt diesen Lauf, schließt den
zuvor beobachteten sporadischen Startfehler aber nicht aus.
Die Linux-CI ergänzt deshalb für `floating_workflow` eine strace-Aufzeichnung
einschließlich Prozess-, Socket- und Warteaufrufen. Der bestehende Timeout bleibt
aktiv; `--kill-on-exit` beendet beim Abbruch auch die verfolgten Prozesse.
Eine zusätzliche Linux-Läuferprüfung kontrolliert echte Traceausgabe und das
Ende eines gestarteten Kindprozesses. Der gemeinsame Läufertest besteht lokal
unter Windows (`build/native-trace-runner-windows.log`). Im
[Lauf zu `1790d87`](https://github.com/PhysicSimulator/physim/actions/runs/36497491668)
bestehen die neuen Linux-Zweige mit GCC und Clang sowie alle 35 normalen
Grafiktests mit aktivierter Aufzeichnung des ersten Ablaufs. Mit Clang bestehen
auch alle 35 instrumentierten Grafiktests. Diese erfolgreichen Läufe allein
belegen keine Behebung des sporadischen Startfehlers.

Die direkte SDK-Prüfung umfasst jetzt auch alle zuvor nur im CMake-SDK-Vergleich
gebauten Sprachmodule: 27 Module, neun Sprachexperimente mit beiden allgemeinen
Analysen, spezielle Sensoranalyse und sechs C-/Physim-Kombinationen.
Die installierten Core-Quellen und alle acht C-Vorlagen samt Analyse werden
unabhängig von den ausgelieferten Bibliotheken neu gebaut. Ein SDL-freies
MSVC-Release-SDK besteht die vollständige erweiterte Prüfung unter
`build/native/Native SDK ä cnycd4y3`. Das Clang-Release-SDK besteht zusätzlich
neun Projektbuilds und beide grafischen Abläufe unter
`build/native/Native SDK ä elwayubq`. Im Lauf zu `f6cedbf` besteht der erweiterte
direkte SDK-Schritt unter Linux GCC/Clang, macOS Apple Silicon/Intel und
Windows MSVC/ClangCL Release.

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
| `87c8ae8` | 493 Tests, zusätzlich 15 Sprachprogramme und 27 Module mit Prüfung inkrementeller Builds und Fehlerkorrektur | Alle acht Kombinationen: [Lauf 36499326457](https://github.com/PhysicSimulator/physim/actions/runs/36499326457) |

Die ursprünglichen 51 C-Tests bestanden auch lokal mit MSVC Debug; die 41 SDL-freien
Fälle zusätzlich mit Clang Release. Der vollständige Satz mit 134 Tests bestand
lokal mit MSVC Debug.

Die CI verwendet nach diesen Vergleichen vollständig den direkten Physim-Build.
Im [Lauf zu `558c2ee`](https://github.com/PhysicSimulator/physim/actions/runs/36500503825)
bestehen alle acht Plattformjobs, einschließlich der neuen SDK- und Paketabläufe.
Die 528 Tests, Sanitizer, SDK-Prüfung, Fuzzer und Benchmarks bleiben
enthalten. Die SDK-Grafikprüfung übernimmt zusätzlich alle acht C-Vorlagen;
Linux prüft sie auch mit der instrumentierten App. Mac-Pakete entstehen nur aus
dem nativen SDK und werden nach dem Signieren verschoben und erneut geprüft.
Neue SDKs enthalten keine CMake-Builddateien. Die alten Repository-Builddateien
und CMake-Testskripte sind entfernt; alle 528 Fälle bleiben im direkten Katalog.
Auch das bereinigte Repository besteht alle acht Plattformjobs im
[Lauf zu `7d39aa7`](https://github.com/PhysicSimulator/physim/actions/runs/36502055743).
Lokal bestehen nach dem Entfernen der 90 Physim-CMake-Dateien alle 493 Prüfungen
ohne Fenster mit MSVC Debug (`build/native/Debug/test-results/run-9wu8gqqs`) und
alle 35 Grafikprüfungen mit MSVC Release (`build/native/Release/test-results/run-hxhqdkcm`).
SDL wird in der CI weiterhin mit seinem CMake-Build gebaut.

## Weitere Änderungen prüfen

Jeder Push startet die [CI](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Die jeweiligen Jobs und Artefakte zeigen den geprüften Commit. Linux-Screenshots
und direkte Ergebnisberichte werden als `linux-ui-gcc` beziehungsweise
`linux-ui-clang` archiviert. Diese Artefakte enthalten auch SDK-Protokolle.
