# Wiedereinstieg in die native UI-Arbeit

Stand: 30. Oktober 2026. Diese kurze Übergabe vor neuen Untersuchungen lesen und
mit `git status` sowie den relevanten Diffs abgleichen. Designverträge stehen in
[AGENTS.md](../AGENTS.md) und der [Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Bei Widersprüchen zuerst Quellstand, Datum und betroffenen Umfang abgleichen.

## Nächster konkreter Schritt

**A2 im Workspace-Dateibaum und Szenenbaum fertigstellen.** Dafür liegen lokale,
noch ungeprüfte Änderungen in `app/workspace_tree_ui.inc`, `app/scene_tree_ui.inc`,
`app/selection_ui.h`, `app/list_selection_tests.inc`, `app/main.c` und
`app/workspace_tests.inc` sowie die neue Datei `app/tree_selection_tests.inc` vor.
Diese Änderungen erhalten und vor dem Weiterarbeiten am aktuellen Diff prüfen.

Als Nächstes die native Baum-Testfixture vervollständigen: übergroße Zielzeilen
mit echten Scrollereignissen sichtbar machen, den gemeinsamen A2-Pixelprüfer um
Workspace/Szene ergänzen und die Testgruppe registrieren. Danach gezielt bauen
und Auswahl, Originaltextfarben, Aus-/Einklappen, Sichtbarkeit, Menüschluss und
Fokus prüfen. Die ungeprüften Baumänderungen wurden **nicht** in den unten
genannten Trennlinien-Nachlauf übernommen.

Weitere verbindliche Arbeit umfasst A2 in der Szene, Mehrfachauswahl,
Kontextmenüs A, Completion D mit Parameterhilfe und die übrige native Oberfläche.
Freigabe und Implementierungs-/Plattformnachweis getrennt halten.

## Abgeschlossen – nicht aus Kontextverlust wiederholen

| Teil | Ergebnis und Nachweis | Verbleibende Grenze |
| --- | --- | --- |
| Trennlinie oberhalb des Protokolls | **Behoben seit `ef1f3321`.** Dauerhaft 1 px, gemeinsame Konturfarbe; Akzent bei Hover/Fokus. [Ursprüngliche Korrektur und negative Kontrolle](verification/protocol-separator-2026-10-29.json). [Nachlauf 30.10.](verification/protocol-boundary-workspaces-2026-10-30.json): Windows Release und Linux Debug ASan/UBSan, je 12/12 Zustände in Entwickeln, Simulieren, Auswerten. | Große Schrift im Nachlauf: 22 px; keine neue macOS-/DPI-/Screenreader-Abnahme. |
| A2 für Textauswahl | Gemeinsamer Textfeldrenderer, Commit `c12e2a10`. [Umfang und native Belege](visuals/unified-workspace/TEXT-SELECTION-A2-NATIVE.md). | Belegt keine vollständige A2-Umsetzung aller Objektarten. |
| A2 für Protokoll- und Explorerlisten/-kacheln | Commit `d228b16e`. [Native Umsetzung](visuals/unified-workspace/LIST-SELECTION-A2-NATIVE.md), [Windows-/Linux-Prüfungen und sechs native Gegenproben](verification/list-selection-a2-2026-10-30.json). | Workspace-/Szenenbaum und Mehrfachauswahl sind eigene offene Schritte. |
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

Der letzte isolierte Stand heißt `protocol-boundary-final`; seine beiden nativen
Testläufe und die anschließende Archivprüfung sind beendet, Exitcode 0. Er basiert
auf `d228b16e` plus den zwei Testdateien `app/protocol_tests.inc` und
`tests/check_protocol_separator.py`. Die neue Prüfung umfasst 24 native Zustände;
die vier Entwickeln-Aufnahmen sind bytegleich mit dem vorherigen Build.

Generierte Manifeste, Archive und Kommandoprotokolle:
`build/validation/explorer-inline-2026-10-29/protocol-boundary-final-*` und
`build/validation/protocol-boundary-2026-10-30/`. Der gemeinsame isolierte
Compilerbaum liegt unter `build/validation/protocol-wrap-2026-10-26/3-isolated/`.
Für dessen nächste Änderung das neue Manifest verwenden, nicht den älteren
`list-a2-final`-Lauf. Die zusätzliche Datei `app/tree_selection_tests.inc` bei
einer neuen isolierten Baum-Testkopie ausdrücklich mit erfassen.

Parallele lokale AT-SPI-/Orca-, Workflow- und Statusänderungen getrennt erhalten;
nur eigene geprüfte Dateien committen. Alles bleibt auf D:, F: nicht berühren.
