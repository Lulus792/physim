# Projektarbeit

- Der lokale Projektstand ist die Grundlage für die weitere Umsetzung.
- Quellcode, Tests, Dokumentation und benötigte Assets vollständig mit Git verfolgen.
  Generierte Builds, lokale Abhängigkeiten, Testausgaben und Laufdaten bleiben ignoriert.
- Abgeschlossene, geprüfte Änderungen committen und in beide Repositories pushen:
  `origin` (Lulus792/Physim) und `upstream` (PhysicSimulator/physim).
  Diese Vorgabe ist vom Nutzer autorisiert; nicht bei jedem Push erneut fragen.
- Bestehende Remote-Historien erhalten. Keine erzwungenen Pushes ohne ausdrücklichen Auftrag.
- Linux- und macOS-Unterstützung anhand tatsächlich ausgeführter Builds und Tests bewerten.
  Eine vorhandene CI-Konfiguration allein beweist keine Funktionsgleichheit.

## Speicherpflege

- Regelmäßiges Aufräumen gehört zur Projektarbeit. Nach abgeschlossener Arbeit
  veraltete, ersetzte und nachweislich nicht mehr benötigte Build-Ausgaben,
  Compiler-/Linker-Caches, temporäre Hilfsdateien und automatisch erzeugte
  Testbilder löschen, damit sich keine unnötigen Datenbestände ansammeln.
- Vor dem Löschen Quellen, Abhängigkeiten und verbleibende Verwendung prüfen.
  Laufende Builds, Tests oder fremde aktive Arbeiten nicht durch Bereinigung stören.
- Quellen, lokale Änderungen, Nutzdaten/Experimente, Git-Historie, Referenzbilder
  für Bildvergleiche, Dokumentationsbilder, benötigte Programme/Debug-Symbole,
  Offline-Abhängigkeiten und VM-/WSL-Datenträger erhalten. Ausdrücklich eingefrorene
  Prüfnachweise und Belege offener Fehler bleiben geschützt.
- Alte Test-Rohbilder nach Abnahme entfernen, sobald sie weder für einen offenen
  Befund noch als Referenz benötigt werden. Alte erfolgreiche Testausgaben ohne
  verbleibenden Zweck nicht unbegrenzt aufbewahren.
- Build- und ignorierte Verzeichnisse nicht pauschal leeren. Größere Bereinigungen
  kurz mit Umfang, Begründung und freigegebenem Speicher dokumentieren.

## Gemeinsame lokale Linux-Entwicklungsumgebung

- Auf diesem Windows-Rechner verwenden Physim, NexaUI und NexaCloud gemeinsam
  die vorhandene WSL-2-Distribution `Alpine-Dev` (Alpine Linux 3.24).
- Start: `wsl -d Alpine-Dev`; direkte Werkzeugaufrufe:
  `wsl -d Alpine-Dev --exec <Programm> <Argumente>`.
- Der Linux-Datenträger liegt außerhalb der Projekte unter
  `D:\WSL\Alpine-Dev\ext4.vhdx`. Er enthält Linux, installierte Werkzeuge,
  Bibliotheken und Linux-Dateien; er ist kein entbehrliches Build-Artefakt.
- Seit 2026-10-16 ersetzt dies `NexaUI-Alpine-3.24` und den früheren Speicherort
  `D:\NexaUI\build-linux-wsl\instance\ext4.vhdx`. Bei Wiederholung alter
  Anleitungen nur den Distributionsnamen anpassen; historische/festgeschriebene
  Prüfnachweise nicht nachträglich umschreiben. Die alte Registrierung existiert
  nicht mehr; keine zweite projektbezogene Distribution dafür importieren.
- Windows-Projektpfade bleiben `D:\Physim`, `D:\NexaUI`, `D:\NexaCloud`,
  unter Linux entsprechend `/mnt/d/Physim`, `/mnt/d/NexaUI`, `/mnt/d/NexaCloud`.
  Gemeinsame Pakete oder WSL-Einstellungen nur mit Blick auf alle drei Projekte
  ändern; diesen Datenträger bei Projektbereinigungen erhalten.
- Lokale Betriebs-/Umzugsnotiz: `D:\WSL\README.md`. Diese Angaben beschreiben
  die lokale Entwicklungsmaschine, keine Voraussetzung für andere Rechner/CI.
