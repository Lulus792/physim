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

## Weitere Änderungen prüfen

Jeder Push startet die [CI](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Die jeweiligen Jobs und Artefakte zeigen den geprüften Commit. Linux-Screenshots
und das letzte CTest-Protokoll werden als `linux-ui-gcc` beziehungsweise
`linux-ui-clang` archiviert. SDK-Protokolle liegen in eigenen Artefakten.
