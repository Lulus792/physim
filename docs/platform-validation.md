# Plattformprüfung

Stand: 28. September 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

## Linux

**Ubuntu 24.04 mit GCC ist für die geprüften Arbeitsabläufe bestätigt.**
Der [vollständig erfolgreiche CI-Job](https://github.com/PhysicSimulator/physim/actions/runs/36453338672/job/109033291071)
prüft den Quellstand `738cac3` mit AddressSanitizer und UndefinedBehaviorSanitizer:

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

Die Clang-Freigabe ist noch offen: Ein Lauf bestand alle 311 CTest-Prüfungen und
das SDK, scheiterte aber im zusätzlichen C-App-Ablauf. Folgeläufe werden geprüft.
Wayland, weitere Distributionen, reale Linux-Grafiktreiber und ein fertiges
Installationspaket auf einem frischen Zielsystem sind noch nicht abgenommen.
Äußere Fensterecken hängen unter Linux vom Desktop ab; die Windows-DWM-Rundung
ist kein plattformübergreifender Nachweis.

## Windows

Die [CI-Matrix für `c576355`](https://github.com/PhysicSimulator/physim/actions/runs/36452292141)
bestand in allen vier Windows-Jobs: MSVC und ClangCL jeweils in Debug und Release,
einschließlich Tests und installiertem SDK. Fenster- und Grafiktests werden
zusätzlich lokal unter Windows ausgeführt; die GitHub-Windows-Worker garantieren
keinen OpenGL-3.3-Treiber.

## Weitere Änderungen prüfen

Jeder Push startet die [CI](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Die jeweiligen Jobs und Artefakte zeigen den geprüften Commit. Linux-Screenshots
und das letzte CTest-Protokoll werden als `linux-ui-gcc` beziehungsweise
`linux-ui-clang` archiviert. SDK-Protokolle liegen in eigenen Artefakten.
