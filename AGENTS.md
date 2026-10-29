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

## Verbindliche Designfreigabe: Grafikexport

- Der Nutzer hat den Grafikexport am 20. Oktober 2026 als Teil des fertigen Designs
  freigegeben. Bei Arbeiten an Auswerten, Berichtsdiagrammen oder Grafikexporten
  [ADR 010](docs/decisions/010-analysis-graphic-export.md) und die
  [freigegebene Spezifikation](docs/visuals/unified-workspace/EXPORT-EXAMPLE.md) lesen
  und nativ mit umsetzen. Es handelt sich nicht mehr um einen optionalen Vorschlag.
- Der [Gesamtauftrag für den umsetzenden Chat](docs/visuals/unified-workspace/NEXT-CHAT.md)
  enthält diese Freigabe. Vorhandene Browsernachweise sind keine native Abnahme.

## Verbindliche Designauswahl vom 25. Oktober 2026

- Der Nutzer hat ausdrücklich ausgewählt: **Leeransicht bestätigt; Protokoll C;
  Szene & Werte A; Auswertung A; Läufe/Serien A; Neues Projekt B; Datei-Explorer A.**
  Die bisherige Auswahlphase ist abgeschlossen. Diese Teile im bestehenden
  nativen Auftrag mit umsetzen; keine erneute Variantenentscheidung vorschalten.
- [ADR 011](docs/decisions/011-selected-workspace-additions.md),
  [verbindliche Spezifikation](docs/visuals/design-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) lesen.
  Die gemischte Auswahl gilt auch im Unteraufruf: Projektdialog B verwendet
  Explorer A, auch wenn er aus Protokoll C geöffnet wird.
- Die Grafikexport-Freigabe bleibt erhalten. Die Designfreigabe bestätigt keine
  fertige native Umsetzung oder Plattformabnahme; bestehende Funktionen erhalten.

## Verbindliche Designfreigabe: Markierung A2

- Der Nutzer hat am 29. Oktober 2026 **A2 · Dezenter Schleier ausdrücklich als
  finales Markierungsdesign freigegeben**. A2 ersetzt die frühere Grundlage A.
  [ADR 016](docs/decisions/016-selected-marking-a2.md),
  [verbindliche Spezifikation](docs/visuals/selection-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten.
  A2 nativ mit umsetzen; keine erneute Variantenentscheidung vorschalten.
  Linksauswahl zeigt keine automatische Aktionsleiste; Aktionen über Rechtsklick,
  Menütaste oder Umschalt+F10. Bestehende Designfreigaben bleiben erhalten.

## Speicherpflege

- Der Nutzer hat die F:-Nutzung am 25. Oktober 2026 widerrufen: Physim bleibt
  vollständig auf D:. Die Rückführung ist abgeschlossen. F: weder für Builds,
  Tests, Archive, Caches noch temporäre Daten verwenden oder darauf zugreifen.
  Generierte Daten unter `D:\Physim\build` ablegen; Quellen, Testprogramme
  und kompakte Nachweise bleiben versioniert im Repository.
  `tools/with-artifacts.py -- ...` verwendet den lokalen Buildordner automatisch;
  unter WSL entspricht er `/mnt/d/Physim/build`. Explizite Ausgabeoptionen
  ebenfalls auf D: setzen. Alte eingefrorene Nachweise mit F:-Pfaden sind
  historische Daten und keine aktuellen Ausführungsanweisungen. Die Anleitung steht in
  [docs/development-storage.md](docs/development-storage.md).
- Prüfnachweise zuerst als kompakten Text-/JSON-Bericht mit Quellstand,
  Kommandos, Soll/Ist-Ergebnis und offenen Befunden dokumentieren. Bilder
  gezielt für visuelle Eigenschaften und Fehlerbelege aufnehmen; gleiche
  Zustände nicht bei jedem Eingabeschritt erneut als Vollbild speichern.
  Bestehende eingefrorene Nachweise nicht nachträglich verändern.

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
