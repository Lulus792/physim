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
