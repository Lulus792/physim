# Übergabepaket für die native Umsetzung

Stand: 20. Oktober 2026. Einstieg ist der [kopierfertige Auftrag](NEXT-CHAT.md).
Der Vorgängerstand der gemeinsamen Vorschau ist Commit `1c303c8`; die neue
Resizing-Ergänzung und diese Übergabe gehören zum nachfolgenden Commit. Den
aktuellen Stand mit `git log -1 -- docs/visuals/unified-workspace/NEXT-CHAT.md`
auflösen. Nicht auf einen historischen nativen Zwischenstand zurücksetzen.

## Verbindliche Quellen

| Thema | Maßgebliche Quelle |
| --- | --- |
| Zusammenhängende Oberfläche | [index.html](index.html), [README.md](README.md), sechs PNGs im selben Ordner |
| Gemeinsame Trenner | [RESIZING.md](RESIZING.md), [gemeinsamer Interaktionskern](../shared/resize.js), [Darstellung](../shared/resize.css) |
| Entwickeln | [Design](../develop-designs/DESIGN.md), [aktueller nativer Abgleich](../develop-designs/FIDELITY.md) |
| Simulieren | [Funktionsabgleich](../simulation-designs/IMPLEMENTATION.md), [Modellwertevertrag](../simulation-designs/MODEL-EDITING.md) |
| Auswerten | [Funktions-/Datenvertrag](../analysis-workspace/IMPLEMENTATION.md), [Design](../analysis-workspace/DESIGN.md) |
| Native Shell/Start | [Designrichtung](../../design-direction.md), [Workspace](../../workspace.md) |
| Browsernachweise | [Grundablauf](validation.json), [neues Resizing](resizing-validation.json) |

Die aktuelle gemeinsame Referenz ergänzt die Einzelvorlagen. Insbesondere
Home = Minimum, End = Maximum, Shift = 40 und Enter = Standard ersetzen die
frühere Simulation-Bedienung. Die Dokumente der einzelnen Bereiche bleiben für
ihre fachlichen Funktionen und Grenzen maßgeblich. Bestehende native Verträge
haben Vorrang vor rein illustrativen Browserabläufen.

## Bestand aufnehmen und weiterverwenden

Alle folgenden Codepfade beziehen sich auf die Repositorywurzel. Symbole sind
Suchanker, keine eingefrorenen Zeilennummern. Beim Übergabeabgleich waren die
`laboratory_*`-/`model_edit*`-Dateien teils untracked bzw. Teil separater aktiver
Arbeit. Vor Implementierung erneut prüfen; aus ihrer Existenz folgt keine Abnahme.

| Aufgabe | Nativer Einstieg / Suchanker | Arbeit für die gemeinsame Referenz |
| --- | --- | --- |
| Gemeinsame Navigation und Oberfläche | `app/toolbar_ui.inc`, `app/design_ui.inc`: `sidebar_panel`, `workspace_panel`, `inspector_panel`; `app/main.c`: `app` | Bereichszuordnung und gemeinsame Zustände; leeren Workspace-Start erhalten |
| Bestehendes Werkbank-Resizing | `app/docking_ui.inc`: `dock_resize_value`, `dock_resize_set`, `dock_resize_begin`, `dock_resize_abort`, `dock_resize_key`; `app/docking.c`, `app/docking.h`, `app/layout_ui.inc` | Gemeinsamen nativen Interaktionskern wiederverwenden; Bereichsadapter statt abweichender Eingabeverfahren |
| Simulation, falls inzwischen vorhanden | `app/laboratory_layout.inc`: `laboratory_context`, `laboratory_dimension_limits`, `laboratory_resize_key`, `laboratory_resize_input`; `app/laboratory_ui.inc`: `laboratory_workspace`, `laboratory_measurements`, `laboratory_separators` | Bestehende Messlaborarbeit übernehmen, gegen Referenz und gemeinsamen Trennervertrag prüfen |
| Modelländerungen, falls inzwischen vorhanden | `app/model_values.c/.h`, `app/model_edit.inc`: `model_edit_sync`, `model_edit_request`, `model_edit_finish`; `app/model_edit_ui.inc`: `model_edit_fields` | Typisierte Bindungen, Dirty-/Buildrevisionen, Fehler/Verwerfen und neue Laufartefakte bewahren |
| Szene, Zeit und Live-Kanäle | `app/scene_tree_ui.inc`, `app/timeline_ui.inc`, `app/plot_ui.inc`, `app/channel_units_ui.inc` | Echte neue Rechtecke übernehmen; Zeit/Kamera/Auswahl und Statusmasken unverändert |
| Laufwahl und Vergleich | `app/library_actions.inc`: `select_run`, `reference_run`; `app/library_ui.inc`; `app/main.c`: `load_dataset`, `request_dataset` | Begrenzte Daten-/Statistikbestände je gewähltem Lauf, klare Referenz und aktiver Lauf |
| Analyse, Berichte, Export | `app/main.c`: `analyze`, `request_report`; `app/report_ui.inc`, `app/report_details.inc`, `app/run_details_ui.inc`, `app/plot_view.c/.h` | Echte Prozesse und Provenienz; Rohdaten- und Ausschnittexporte erhalten |
| Darstellung und Zugänglichkeit | `app/design_tokens.h`, `app/ui_backend.c`, `app/ui_sdl.c`, `app/accessibility*.c` | Semantische Farben, UI-/Textskalierung, Fokus und native Separator-Werte/Orientierung |
| Bestehende Prüfeinstiege | `tools/build.py`, `tools/native_tests.py`, `tests/check_display_workflow.py`, `app/workspace_tests.inc`; ggf. `app/laboratory_tests.inc` | Bestehende Aufgaben ausführen und gezielte native Regressionen ergänzen |

Konkreter Abgleich am lokalen Zwischenstand: `laboratory_resize_key` enthielt
noch Shift-Schritte von 2 Pixeln, Home als Reset und keine End-Behandlung;
`dock_resize_key` der Werkbank besaß bereits 10/40, Home/End und Enter. Diese
Abweichung bei der nativen Umsetzung an der gemeinsamen Werkbanklogik beheben.
Der Browserkern ist eine Verhaltensreferenz, keine C-Bibliothek und kein Grund,
den bestehenden Docking-/Preference-Vertrag neu zu erfinden.

## Sinnvolle Umsetzungsschritte und Abnahmepunkte

1. **Bestand sichern und abgleichen.** Aktuelle Änderungen/Builds inventarisieren,
   vorhandene Werkbank-/Messlaborarbeiten prüfen, echte native Baseline aufnehmen.
2. **Gemeinsame Shell und Layoutzustände.** Vorhandene Navigation, Dokumente und
   Arbeitsbereichszustände erhalten. Wunschgrößen, Sichtbarkeiten und aktuelle
   Auswahl dürfen beim Bereichswechsel nicht ineinander überlaufen.
3. **Ein Trennerkern, Bereichsadapter.** Gemeinsame Eingabe-/Capture-/Abbruchregeln
   sowie Zeichnung und Accessibility; lediglich Grenzen, Standard und Geometrie
   sind bereichsspezifisch. Bestehende persönliche Docklayouts bewahren.
4. **Messlabor vervollständigen.** Alle fachlichen Funktionen der Einzelübergabe
   mit echten Kanälen, Zeitleiste, Aufzeichnung und Modellbindungen verbinden.
5. **Auswertung integrieren.** Reale Laufbibliothek, Mehrlaufdaten, Analyseprozesse,
   Berichte und Exporte. Diagrammhöhe bekommt einen eigenen Trenner gegenüber
   Statistik. Tabelleninhalt/Statistik ändern sich durch Resize oder Zoom nicht.
6. **Ende-zu-Ende-Aufgabe.** Echtes Projekt öffnen → Code ändern/speichern/bauen →
   Lauf aufzeichnen → Modell ändern/bauen → zweiter neuer Lauf → Vergleich und
   Bericht → vollständiger und Ausschnittsexport. Alte Lauf-/Berichtsherkunft prüfen.
7. **Visuelle und technische Abnahme.** Native Aufnahmen mit den sechs Referenzen
   vergleichen, Abweichungen begründen, Tests protokollieren und erst dann committen.

## Native Abnahmeliste

- [ ] Leerer Start, Ordner ohne Projekt, echtes Projekt, Build-/Runner-/Analysefehler
      und Abbruch bleiben erreichbar und verständlich.
- [ ] Dokumente, Undo/Dirty, Suche, Scrollposition, Auswahl, Kamera, betrachtete Zeit,
      Einheiten und Bericht bleiben bei Bereichswechsel und Resizing erhalten.
- [ ] Jeder Trenner: reale Mausbewegung, Capture außerhalb der Grenze, 10/40,
      Home/End, Enter/Doppelklick, Escape sowie Eingabe-/Fokusabbruch; nur die
      beabsichtigte Grenze verändert sich und die reale Zeichnung folgt ihr.
- [ ] Alle acht Simulation- und vier Analyse-Seitenkombinationen, kein Null-/Negativ-
      Rechteck, kleine/große Fenster, wieder öffnen und wieder vergrößern.
- [ ] Wunschgrößen werden nicht durch temporäres Klemmen bei kleinen Fenstern
      überschrieben. Bestehende gespeicherte, gruppierte und schwebende Docklayouts
      werden nicht still ersetzt.
- [ ] In Auswerten echte unterschiedliche Laufdauern/Abtastraten, fehlende/ungültige
      Kanäle, Statuslücken, reduzierte Vorschauen und defekte/fehlende Dateien prüfen.
- [ ] Berichte und asynchrone Worker bleiben an ihre tatsächlichen Eingaben gebunden.
      Analyseprojekte ohne Eingaben und reine Wiedergabe bleiben lauffähig.
- [ ] Hell/Dunkel, hoher Kontrast, reduzierte Transparenz/Bewegung, große App-Schrift
      und echte Tastatur-/Fokusbedienung; erreichbare Namen/Werte aller Separatoren.
- [ ] Windows, verfügbare Linux-Umgebung `Alpine-Dev` und ggf. macOS getrennt mit
      tatsächlich ausgeführten Kommandos, Revision/Binärhash und Ergebnis notieren.
      Fehlende Screenreader-/DPI-/Plattformnachweise offen benennen.
- [ ] Echte native Referenzbilder, Tests und verbleibende Unterschiede im gemeinsamen
      Designordner dokumentieren. Browserprüfungen nicht als native Ergebnisse zählen.

## Prüfung der übergebenen Vorlage

Der Browserprüfer `tools/check_workspace_resizing.cjs` führt echte Aktionen an elf
sichtbaren gemeinsamen Trennern aus. Die vorhandenen Prüfer für Entwickeln,
Inspektorvarianten, Auswertung und den gemeinsamen Datenablauf decken die übrige
Interaktion ab. Nachweise und Quellhashes stehen in `resizing-validation.json`.
Zum Wiederholen Node und `NODE_PATH` der gebündelten Playwright-/Sharp-Pakete
verwenden; alle Dateien/Assets sind im Repository und benötigen keine externen
Webressourcen. Die lokale Adresse auf Port 8791 ist optional und keine Voraussetzung
für den anderen Chat oder Rechner.
