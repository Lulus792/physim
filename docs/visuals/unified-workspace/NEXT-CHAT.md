# Kopierfertiger Auftrag: die gemeinsame Physim-Oberfläche nativ umsetzen

Aktueller nativer Folgeauftrag vom 20. Oktober: zuerst die gesamte Gestaltung
einschließlich F1 direkt gegen die gewählten Referenzen angleichen. Der
[native Abgleich](NATIVE-FIDELITY.md) nennt den aktuellen Zwischenstand und die
noch erforderlichen Schritte. Bestehende Umsetzung übernehmen; die folgende
ursprüngliche Übergabe bleibt Anforderungsgrundlage, nicht Fertigstellungsnachweis.

Stand: 20. Oktober 2026. Die Browserreferenz verbindet die drei gewählten Designs
einschließlich gemeinsamem Resizing. Dieser Text ist zur Übergabe in einen
anderen Chat bestimmt. Die Vorschauarbeit selbst ist keine native Umsetzung.

---

Arbeite im bestehenden Projekt `D:\Physim` die native Physim-Oberfläche zu der
zusammenhängenden Oberfläche aus
`docs/visuals/unified-workspace/index.html` um. Implementiere die drei Bereiche
**Entwickeln**, **Simulieren** und **Auswerten** im vorhandenen C-/SDL-/Nuklear-/
OpenGL-Aufbau bis zur geprüften Fertigstellung. Die Browserreferenz ist festgelegt;
erstelle keine weiteren Designvarianten und keinen WebView-Ersatz.

Lies zuerst `AGENTS.md` und `D:/Skills/skills/AGENTS.md`. Prüfe `git status`, den
aktuellen lokalen Code und die vorhandenen Builds. Der lokale Projektstand ist
die Grundlage. Bei Erstellung der Übergabe liefen separate native Arbeiten;
Dateien wie `app/laboratory_*.inc`, `app/model_edit*.inc` und `app/model_values.*`
waren lokal vorhanden und teilweise noch uncommittet. Übernimm brauchbare bereits
vorhandene Umsetzung; überschreibe oder verwerfe keine fremden Änderungen. Die
Browserprüfung ist keine Abnahme dieser nativen Zwischenstände.

Lies anschließend:

- `docs/visuals/unified-workspace/README.md`
- `docs/visuals/unified-workspace/RESIZING.md`
- `docs/visuals/unified-workspace/IMPLEMENTATION.md`
- `docs/design-direction.md` und `docs/workspace.md`
- `docs/visuals/develop-designs/DESIGN.md` und `FIDELITY.md`
- `docs/visuals/simulation-designs/IMPLEMENTATION.md` und `MODEL-EDITING.md`
- `docs/visuals/analysis-workspace/IMPLEMENTATION.md` und `docs/reports.md`

Öffne die gemeinsame HTML-Datei lokal im Browser oder stelle `docs/visuals` mit
einem lokalen HTTP-Server bereit und öffne `/unified-workspace/`. Die sechs
`develop-*`, `simulate-*` und `analyze-*`-PNG-Dateien in diesem Ordner sind echte
Referenzaufnahmen. Die älteren Einzelvorlagen enthalten zusätzliche Zustände.
Die verbindliche aktuelle Ergänzung zu Resizing steht ausschließlich in
`unified-workspace/RESIZING.md`; ältere abweichende Home-/Shift-Regeln sind ersetzt.

Erhalte die kompakte globale Menüzeile und drei gleich breite Arbeitsbereich-Tabs.
Ein Projekt, Dokumente, Buildstatus, Laufdaten und Berichtsherkunft verbinden die
Bereiche. Der native Start bleibt die bestehende leere Arbeitsumgebung mit
„Ordner öffnen“; das automatisch offene Pendelbeispiel gehört zur Browserdemo.

**Entwickeln:** Die gewählte Werkbank Klarglas Kühl mit Panelköpfen, kompakten
Dateitabs, Workspace-Pfadlinien und schwebender Suchinsel erhalten bzw. angleichen.
Suche/Ersetzen dürfen den Code nicht verschieben. Der Kontextschalter bleibt
äußerst rechts. Echte Dokumente, Undo/Redo, Dirty, Save/Recovery, Build, Diagnosen,
Completion, API und F1 bleiben funktionsfähig.

**Simulieren:** Messlabor mit Wertefeldern B. Ohne persönliche abweichende
Dockanordnung initial Szenenbaum zu, Inspektor offen, Kurven zu. Große echte Szene,
Laufsteuerung und Tempo oben, Messwerte und optionale Kurven darunter, Zeitleiste
unten. Szenenbaum, Inspektor und Kurven sind unabhängig; Auswahl, Kamera und
betrachtete Zeit bleiben erhalten. Alle bestehenden Inspektorfunktionen,
Laufparameter/Validierung, Limits, Ereignisse und Aufzeichnung zugänglich halten.
Modellwerte verwenden einen expliziten typisierten Bindungsvertrag. Vormerken,
Verwerfen und ggf. Stoppen & neu kompilieren führen über den echten Buildweg;
erst ein neuer Lauf verwendet neue Artefakte. Keine pauschalen Regex-Ersetzungen
aus der Browserdemo in native C-/Physim-Quellen übernehmen.

**Auswerten:** Links durchsuchbare echte Messläufe und Mehrfachauswahl mit
eindeutiger Referenz, Mitte Messdaten/Analyseergebnis, rechts Kanal/Einheiten,
ausdrücklich benannter aktiver Lauf und Herkunft. Analysecode öffnet den vorhandenen
Editor. Analyse starten und Export bleiben echte vorhandene Prozesse. Implementiere
einen begrenzten echten Mehrlaufvergleich mit unabhängigen Zeitvektoren, passenden
Kanälen/Dimensionen, Statusmasken, Lücken und vollständiger Statistik je Lauf.
Bewahre die Auswahlreihenfolge und acht Analyseeingaben. Kein Paaren nach Sampleindex.
Berichte behalten ihre ursprüngliche Auswahl, Einheiten und Herkunft auch nach
neuer Laufwahl; alte Workerabschlüsse dürfen aktuelle Auswahl nicht überschreiben.
Rohdaten-/Ausschnittexporte, CSV/SVG/PNG und PNG-Größenwahl bleiben erhalten.

**Ein gemeinsames Resizing:** Verwende für alle drei Bereiche dieselbe native
Trennerinteraktion auf Grundlage der bestehenden Werkbank/Docking-Implementierung.
Ruhige Grenze, bei Hover/Fokus eine durchgehende dünne Akzentlinie, großzügige
Trefffläche, korrekter Cursor, Maus-Capture, sichtbarer Fokus und native Separator-
Semantik. Pfeile bewegen die Grenze um 10 logische Pixel, Umschalt um 40.
Home/End setzen Minimum/Maximum, Enter/Doppelklick die jeweilige Standardgröße.
Escape, Eingabeabbruch oder Fokusverlust während Ziehen stellen den Ausgangszustand
wieder her. Rechts/unten wird der Bereich bei Bewegung nach links/oben größer.

Entwickeln: Workspace, Codekontext, Protokoll. Simulieren: Szenenbaum, Inspektor,
Szene/Messbereich, Zeitleiste und Verhältnis der beiden Kurven. Auswerten: Laufliste,
Details und Diagrammhöhe gegenüber Statistik. Die Gesamtvorschauhöhe der einzelnen
Simulation ist Präsentationsrahmen und kein zusätzlicher nativer Trenner.
Speichere Wunschgrößen und Grenzen passend zum bestehenden Docking, ohne sie bei
Fensterresize oder Seitenwechsel still zu überschreiben. Die Größen müssen echte
Renderrechtecke verändern. Kein Code-, Modell-, Zeit-, Kamera-, Daten- oder
Statistikwechsel durch Resizing. Beachte UI-/Textskalierung und kleine Fenster.

Die Vorschau führt weder C noch Analysecode aus, verwendet eine Kleinwinkelnäherung
und hält höchstens acht Demoläufe nur in der Sitzung. Diese Grenzen und das
automatische Pausieren beim Bereichswechsel sind kein neuer Vertrag für den
nativen Runner, die gespeicherte Laufbibliothek oder echte Analysen. Erhalte
bestehende native Abläufe, Abbruch, Initialisierung, HELLO/Timeouts, Reset in neue
Laufdateien, Analyseprojekte ohne Eingaben und historische Wiedergabe.

Prüfe die fertige native Umsetzung anhand von `IMPLEMENTATION.md`: echte
Windows-Aufgaben und Screenshots, tatsächliche Datenherkunft, alle acht
Simulation-/vier Analyse-Seitenkombinationen, Maus und Tastatur an jedem Trenner,
Abbruch/Reset, Größenwechsel, Hell/Dunkel, hoher Kontrast, große Schrift und
Accessibility. Führe Linux-Builds/Tests bei verfügbarer Umgebung in der gemeinsamen
Distribution `Alpine-Dev` aus. Linux/macOS nur nach tatsächlich ausgeführten
Prüfungen als geprüft melden; Browserergebnisse oder vorhandene CI-YAML genügen
nicht. Dokumentiere offene Plattform-/Screenreader-/DPI-Nachweise ehrlich.

Halte native Änderungen, ausgeführte Tests, echte Aufnahmen und begründete
Abweichungen im gemeinsamen Designordner fest. Committe abgeschlossene geprüfte
Änderungen und pushe nach `origin` und `upstream` ohne Force-Push entsprechend
AGENTS.md. Räume nur eigene nachweislich entbehrliche Ausgaben auf; Quellen,
Nutzdaten, Referenzen, aktive Arbeiten und den gemeinsamen WSL-Datenträger erhalten.
