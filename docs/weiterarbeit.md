# Wiedereinstieg in die native UI-Arbeit

Stand: 30. Oktober 2026. Diese kurze Übergabe vor neuen Untersuchungen lesen und
mit `git status` sowie den relevanten Diffs abgleichen. Designverträge stehen in
[AGENTS.md](../AGENTS.md) und der [Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Bei Widersprüchen zuerst Quellstand, Datum und betroffenen Umfang abgleichen.

## Nächster konkreter Schritt

**macOS-Systemmenüleiste nach [ADR 024](decisions/024-native-macos-menubar.md)
umsetzen.** A2 im Workspace- und Szenenbaum ist als Teilumfang abgeschlossen
und unten belegt. Diese Baumprüfungen ohne neue relevante Änderung nicht erneut
beginnen. Der neue Mac-Adapter soll die vorhandenen `toolbar_entries` und
`toolbar_dispatch` in `app/toolbar_ui.inc` verwenden und die bisherigen
Datei-/Bearbeiten-/Ansicht-/Build-/Hilfe-Menüs dort an AppKit anbinden. Native
Verfügbarkeit, Kürzel, einzelne Befehlsausführung, Fensterwechsel und Shutdown
prüfen; die übrige App-Gestaltung erhalten. Code und echte Mac-Abnahme sind offen.

Weitere verbindliche Arbeit umfasst A2 in der Szene, Mehrfachauswahl,
Kontextmenüs A, Completion D mit Parameterhilfe und die übrige native Oberfläche.
Freigabe und Implementierungs-/Plattformnachweis getrennt halten.

Physim verlinkt AppKit bereits; die bestehende Accessibility-Brücke nutzt
typisierte Objective-C-Runtime-Aufrufe aus C17. Offizielle Apple-Quellen und der
enge Nutzerumfang sind in ADR 024 festgehalten. Vollbild-/Menüleistenverhalten
macOS verwalten lassen. Eine native Hauptmenüanbindung existiert noch nicht.

## Abgeschlossen – nicht aus Kontextverlust wiederholen

| Teil | Ergebnis und Nachweis | Verbleibende Grenze |
| --- | --- | --- |
| Trennlinie oberhalb des Protokolls | **Behoben seit `ef1f3321`.** Dauerhaft 1 px, gemeinsame Konturfarbe; Akzent bei Hover/Fokus. [Ursprüngliche Korrektur und negative Kontrolle](verification/protocol-separator-2026-10-29.json). [Nachlauf 30.10.](verification/protocol-boundary-workspaces-2026-10-30.json): Windows Release und Linux Debug ASan/UBSan, je 12/12 Zustände in Entwickeln, Simulieren, Auswerten. | Große Schrift im Nachlauf: 22 px; keine neue macOS-/DPI-/Screenreader-Abnahme. |
| A2 für Textauswahl | Gemeinsamer Textfeldrenderer, Commit `c12e2a10`. [Umfang und native Belege](visuals/unified-workspace/TEXT-SELECTION-A2-NATIVE.md). | Belegt keine vollständige A2-Umsetzung aller Objektarten. |
| A2 für Protokoll- und Explorerlisten/-kacheln | Commit `d228b16e`. [Native Umsetzung](visuals/unified-workspace/LIST-SELECTION-A2-NATIVE.md), [Windows-/Linux-Prüfungen und sechs native Gegenproben](verification/list-selection-a2-2026-10-30.json). | Mehrfachauswahl bleibt offen. |
| A2 für Workspace- und Szenenbaum | [Umsetzung](visuals/unified-workspace/TREE-SELECTION-A2-NATIVE.md), [Nachweis 30.10.](verification/tree-selection-a2-2026-10-30.json): je zehn tatsächliche Zustände Windows/Linux, sechs native Gegenproben, zusätzlich 72 Linux-Pendellaufansichten. | Einzelauswahl; 32 px sind eine gezielte Rendererbelastung, keine neue Einstellungsoption oder reale 200%-DPI-Abnahme. |
| F1: ungültige synthetische Umschalttastenereignisse unter Linux | Commit `8840b9c9`: SDL-Ereignisse ausdrücklich initialisiert; sieben Linux-Kontextfälle bestanden. | Eigenständige Windows-Fokusbefunde bleiben offen; nicht als gleicher Fehler behandeln. |

Der Trennlinien-Nachlauf war hier eine unnötige Wiederholung eines bereits
behobenen Befunds. Er brachte zusätzliche Bereichsabdeckung, aber keine neue
Darstellungskorrektur. **Nicht noch einmal als neue Reparatur beginnen.**

## Wann erneut prüfen?

- Relevante Änderungen am Renderer, an Palette/Layout oder am betroffenen
  Eingabeweg: passende Regression ausführen und den konkreten Anlass nennen.
- Neuer konkreter Fehlerbeleg: zuerst betroffene Version und Zustand mit dem
  vorhandenen Nachweis vergleichen; nur die offene Abweichung reproduzieren.
- Eine bisher unbelegte Plattform/Eingabe-/Darstellungsdimension: gezielt diese
  Dimension ergänzen und bestehende Ergebnisse ihrer Revision zugeordnet lassen.

Ein neuer Chat oder eine Kontextkompaktierung genügt als Anlass nicht. Vor jedem
neuen Lauf kurz festhalten: **Was hat sich geändert, welche Frage ist offen und
welcher kleinste Test beantwortet sie?**

## Lokaler technischer Anschluss

Der letzte isolierte Stand heißt `tree-a2-final`. Alle zugehörigen Builds,
nativen Tests, Gegenproben und Archivprüfungen sind beendet. Die abschließenden
Baumtests `tree-a2-fifth` bestehen auf Windows/Linux, die sechs Gegenproben in
`tree-a2-counter-r3` ebenfalls. Die finalen Quellen unterscheiden sich davon nur
durch die geprüfte Python-Gegenproben-Harnesskorrektur, nicht durch App-Code.

Generierte Manifeste, Archive und Kommandoprotokolle:
`build/validation/explorer-inline-2026-10-29/tree-a2-final-*` und
`build/validation/tree-selection-a2-2026-10-30/`. Der gemeinsame isolierte
Compilerbaum liegt unter `build/validation/protocol-wrap-2026-10-26/3-isolated/`.
Für dessen nächste Änderung das neue Manifest verwenden, nicht den älteren
`list-a2-final`- oder `protocol-boundary-final`-Lauf. Der letzte Eingabebestand
umfasst 101 Dateien; neue Adapter-/Testdateien ausdrücklich mit erfassen.

Parallele lokale AT-SPI-/Orca-, Workflow- und Statusänderungen getrennt erhalten;
nur eigene geprüfte Dateien committen. Alles bleibt auf D:, F: nicht berühren.

Für spätere Mac-Tests verweist der Nutzer ausdrücklich auf NexaCloud und NexaUI:
`D:/NexaCloud/docs/MACOS_TESTING.md` sowie die aktuellen Mac-Hinweise in
`D:/NexaUI/AGENTS.md` zuerst lesen. Eigene Remote-Reste nach gesicherter,
hashgeprüfter Belegkopie gezielt aufräumen; gemeinsame Werkzeuge erhalten.
Die neueste gelesene Verfügbarkeitsnotiz (26.10.) meldet den Nutzer-Mac als aus.
Die Nachricht vom 30.10. enthält einen Anleitungshinweis, keine neue
Verfügbarkeitsmeldung. Bisher wurde deshalb keine neue SSH-/Mac-Probe gestartet.
