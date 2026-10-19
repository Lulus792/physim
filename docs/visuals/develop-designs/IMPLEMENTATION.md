# Technische Übergabe: Entwickeln / Klarglas Kühl

Stand: 19. Oktober 2026. Zuständigkeit des nächsten Chats: native UI-Umsetzung
mit passenden UI-, Grafik-, Plattform- und Accessibility-Skills aus dem zentralen
Bestand. [NEXT-CHAT.md](NEXT-CHAT.md) enthält den vollständigen Auftrag.

Die folgende Übergabe beschreibt den Ausgangsstand. Die danach ausgeführte
native Umsetzung, Bilder und Abnahmegrenzen stehen in
[NATIVE-STATUS.md](NATIVE-STATUS.md).

## Ausgangspunkt und Umfang

Die Gestaltung ist final gewählt. [DESIGN.md](DESIGN.md) und die beiden
[dunklen](glass-cool-dark.png)/[hellen](glass-cool-light.png) Referenzen gelten
gemeinsam mit der interaktiven [Vorschau](index.html). Frühere Varianten werden
nicht erneut angeboten. Die native App ist durch diese Übergabe noch nicht geändert.

Geprüfte native Ausgangsbasis: `89d59409493dfa6249ff56c3b97454029eab9231`;
die Übergabedateien werden danach ergänzt. Beim Umsetzen den dann aktuellen
lokalen Stand verwenden. Gleichzeitig vorhandene Änderungen außerhalb dieses
Designordners und seines Browserprüfers gehören anderen Arbeiten.

Auftrag ist die Entwickeln-Oberfläche und ihre wiederverwendbare Dokumentsuche.
Die Gestaltung anderer Arbeitsbereiche wird damit nicht neu entschieden.
Gemeinsame Shell-/Docking-Änderungen dürfen bestehende Funktionen, Layouts und
die separat dokumentierte Auswertungsoberfläche nicht verdrängen.

## Zuordnung zum vorhandenen Code

| Teil | Ausgangspunkt | Umsetzung / zu erhaltender Vertrag |
| --- | --- | --- |
| Gesamtaufbau | [design_ui.inc](../../../app/design_ui.inc): `draw_ui`, `sidebar_panel`, `workspace_panel`, `inspector_panel`, `log_panel` | Kompakter Aufbau, Panelköpfe, Aktionen an Dateitabs, Profil/Buildstatus unten; andere Workspace-Ansichten erreichbar halten. |
| Dateitabs | [file_tabs_ui.inc](../../../app/file_tabs_ui.inc): `file_tabs_view`, `file_tab_select`, `file_tabs_keyboard` | Pill-Gestaltung; reale Dateinamen, Dirty, Schließen, Umordnen, Überlauf und Tastaturwechsel beibehalten. |
| Workspace-Baum | [workspace_tree_ui.inc](../../../app/workspace_tree_ui.inc), [workspace_catalog_ui.inc](../../../app/workspace_catalog_ui.inc) | Pfadlinien, selektierte Datei, korrekte Hierarchie und vorhandene Dateiaktionen. Keine synthetischen Ordner erzeugen. |
| Editor/Projekt-Suche | [main.c](../../../app/main.c): `editor`, `find_text`, `show_search`; [design_ui.inc](../../../app/design_ui.inc): `workspace_panel` | Vorhandenen Editor wiederverwenden; Overlay statt Layoutzeile; Vor-/Rückwärtssuche und Trefferstatus ergänzen. |
| Weitere Textdateien | [document_ui.inc](../../../app/document_ui.inc): `document_view` | Gemeinsame Suchinsel für `doc->edit`; Recovery, Fehlerhinweise und ausstehende Dateiaktionen erhalten. |
| Docking/Größen | [docking_ui.inc](../../../app/docking_ui.inc): `dock_prepare`, `dock_arrange`, `dock_relayout`, `dock_show`; [docking.c](../../../app/docking.c), [docking.h](../../../app/docking.h), [preferences.c](../../../app/preferences.c) | Bestehende Panel-IDs, Layoutserialisierung und gespeicherte Maße nutzen; kein paralleles Layoutmodell. |
| Kontext/Completion | [api_reference_ui.inc](../../../app/api_reference_ui.inc), [completion_ui.inc](../../../app/completion_ui.inc), [completion.c](../../../app/completion.c) | Echte API-Referenz/Completion und vorhandene Daten erhalten. Demo-Gliederung und Modellnotiz sind keine vorhandene Symbolanalyse. |
| Menü/Build | [toolbar_ui.inc](../../../app/toolbar_ui.inc), [design_ui.inc](../../../app/design_ui.inc), [main.c](../../../app/main.c) | Vorhandene Aktionen, Buildprofile, Worker, Abbruch, Save-Prüfungen und Diagnosesprung anbinden. |
| Farben/Skalierung | [design_tokens.h](../../../app/design_tokens.h), [ui_backend.c](../../../app/ui_backend.c), [ui_sdl.c](../../../app/ui_sdl.c) | Semantische App-Farben, Schrift- und UI-Skalierung erhalten; nur Suchinsel bekommt das Glasmaterial. |
| Glasrenderer | [graphics.c](../../../app/graphics.c): `ui_target`, `ps_graphics_ui`; [graphics.h](../../../app/graphics.h); [ui_sdl.c](../../../app/ui_sdl.c): `nk_sdl_render` | OpenGL-Komposition untersuchen und begrenzt erweitern; derzeit kein Backdrop-Blur-API vorhanden. |
| Accessibility | [ui_backend.c](../../../app/ui_backend.c), [ui_sdl.c](../../../app/ui_sdl.c), [accessibility.h](../../../app/accessibility.h), native Adapter unter `app/accessibility_*` | Echte Namen, Rolle, Fokus, Zustands-/Größenwerte, Overlay-Hitbereiche und Plattformzugang ergänzen. |

Funktionsnamen dienen als Suchanker, da Zeilennummern durch laufende Arbeit wandern.

## Konkrete Unterschiede zur Browserreferenz

**Suchlayout:** `workspace_panel` erzeugt derzeit bei `show_search` eine vierteilige
Suchzeile und zieht zusätzlich 38 px von der Editorhöhe ab. `document_view` addiert
für dieselbe Suche 34 px zu `used`. Beide Pfade müssen dieselbe Overlay-Komponente
verwenden und ohne Suchhöhenabzug arbeiten. Die Overlay-Rechtecke aus dem aktuellen
Editorrechteck ableiten, ohne dessen Scrollcontainer umzubauen. Echte Recovery- und
Fehlerhinweise sind keine überflüssige Suchleiste und bleiben funktionsfähig.

**Suchmodell:** `find_text` sucht derzeit vorwärts, setzt Selection/Cursor und
Editor-Scroll und ersetzt einen Treffer mit Nuklear-Undo-Unterstützung. UTF-8-Byte-
und Rune-Indizes werden umgerechnet. Rückwärtssuche und Trefferzahl nicht nur als
UI vortäuschen; Grenzfälle leere Suche, kein Treffer, Start/Ende, Umlaute, mehrzeilige
Auswahl, leere Ersetzung und Undo prüfen. Initiales Öffnen darf `find_text` nicht
aufrufen, weil dies die geforderte Scrollinvariante verletzen würde.

**Panelgrößen:** Die Demo speichert nur bis zum Neuladen. Nativ existieren bereits
`preferences.dock`, `sidebar_width`, `inspector_width` und `log_height` sowie
schwebende Panels und Tabgruppen. Größen/Versteckzustand über diese Mechanik
erhalten. Ein-/Ausklappen darf nicht gespeicherte Größen löschen. Insbesondere
`dock_show` und dessen Wiedereinordnung des Inspektors auf das gewählte Verhalten
prüfen. Nicht bei jedem Frame oder Tabwechsel das Standardlayout erzwingen.

Native aktuelle Breitenklemmen sind für Sidebar 208–360 und Inspektor 208–480;
die Protokollgröße wird auf 150–340 begrenzt. Das unterscheidet sich von den
Browsermaßen in DESIGN.md. Letztere sind Gestaltungsreferenzen bei 100 % Skalierung,
keine direkten Ersatzkonstanten: native Kopf-/Statushöhen, Inhaltsminima und große
Schrift berücksichtigen. Das CSS-Stapeln unter 951/621 px ist eine Referenz für
Erreichbarkeit bei Platzmangel, kein vorhandener nativer Dockingzustand. Native
Mindestfenster und wieder erreichbare Panels prüfen, ohne Floating-Layouts zu zerstören.

**Fester rechter Schalter:** In der Demo bleibt `#toggle-api` im selben DOM-Elternteil,
absolut 8 px rechts und 6 px unter dem oberen Rand der gesamten `.workspace`.
Er belegt auch geschlossen eine reservierte rechte Aktionsfläche. Nativ ein
gemeinsames stabiles Kopf-Rechteck verwenden: eine Aktion, eine ID, gleiche
Position und Fokus nach jedem Zustandswechsel. Bei unverändertem Fenster und
Layout nur Panel-Sichtbarkeit ändern und die Schalterrechtecke vergleichen.
Bei bewusst verändertem Dock-/Floating-Layout eine nachvollziehbare erreichbare
Verankerung beibehalten; die Browserdemo spezifiziert dafür keine zweite Fenster-Shell.

**Glas:** Die Demo verwendet `backdrop-filter`; C/Nuklear kann CSS nicht ausführen.
Ein plausibler nativer Ansatz ist, den bereits gezeichneten Inhalt hinter der
Suchinsel in einer begrenzten Textur zu filtern und vor ihren Bedienelementen
zu komponieren. Erst die tatsächliche Renderreihenfolge im aktuellen Code prüfen;
dies ist ein Arbeitsvorschlag, keine fertige API. Nur den Hintergrund filtern,
nicht Suchtext oder Icons. Rechteck/Radius scissoren, Raster-/DPI-Skalierung und
Fensterwechsel beachten, Ressourcen bei Resize/Schließen freigeben und Fehler auf
den deckenden Fallback führen. Kein CPU-Readback pro Frame und kein Blur des
gesamten Betriebssystemfensters als Ersatz für den Codehintergrund.

**Echte Funktionen:** `develop.js` enthält absichtlich gekürzten Pendelcode und
simuliertes Speichern/Build. `workbench.js` ordnet die Demo um;
`search-resize.js` enthält Suchicons und Referenzinteraktionen. Daraus Gestalt und
Verhalten übernehmen, nicht deren Beispielwerte oder simulierte Implementierung.
Ein Outline-Parser oder neue Modellnotizen sind kein zusätzlicher Auftrag; Kontext
aus existierenden Daten speisen bzw. einen ehrlichen Leerzustand zeigen.

## Abnahme der nativen Umsetzung

- [ ] Beide Referenzerscheinungen erkennbar umgesetzt; kompakte Dateileiste,
  Pfadlinien und kurze Distanz zum ersten Code, ohne zusätzliche Such-/Pfadzeile.
- [ ] Suche geschlossen/offen und Ersetzung zu/auf: identische Editorposition,
  -größe, Selection und Scrollposition vor einer bewussten Trefferaktion. Mit
  langem Dokument und horizontalem sowie vertikalem Scroll testen.
- [ ] Kontext offen/geschlossen: identische Schalterposition bei unverändertem
  Fenster; immer rechts von Suche/Speichern. Linke/rechte Seite in allen vier
  Kombinationen bedienbar, Fokus bleibt beim Schalter.
- [ ] Suche/Ersetzen funktionieren für C-/Physim-Experiment, Analyse und Textdatei;
  Vor-/Zurück mit Umbruch, Trefferzahl, UTF-8, Undo/Redo, Save und Dirty korrekt.
  Escape/Fokus, Ctrl/Command+F, Enter/Umschalt+Enter und Tabfolge getestet.
- [ ] Overlay nimmt nur innerhalb seiner Grenzen Eingaben ab; Code außerhalb
  lässt sich bearbeiten/scrollen. Menüs, Completion, Dialoge und Recovery besitzen
  korrekte Eingabepriorität und überdecken sich nicht mit aktiven unsichtbaren Zielen.
- [ ] Glas zeigt tatsächlich veränderten Code darunter; 16/9 px Rundungen, 2 px
  Blur, kühle Flächen und vereinheitlichte Icons. Deckender Fallback bleibt bei
  gleichem Layout lesbar. Kein permanenter kostspieliger Effekt bei geschlossener Suche.
- [ ] Alle relevanten Panels per Maus und Tastatur resizbar; Minimum, Maximum,
  Abbruch, Reset, Scrollbereiche, verstecken/wiederöffnen und Neustart geprüft.
  Bestehende gespeicherte, gruppierte und schwebende Layouts funktionieren weiter.
- [ ] Profilwechsel, echter erfolgreicher/fehlerhafter Build, Abbruch und
  Diagnosesprung getestet. Datei schließen/umordnen, mehrere Dateien und
  Leerzustand funktionieren; übrige Arbeitsbereiche bleiben zugänglich.
- [ ] Kleine und große native Fenster, große UI-/Codeschrift, DPI-/Monitorwechsel,
  Hell/Dunkel, hoher Kontrast und Transparenzfallback geprüft. Native Accessibility
  meldet Namen/Zustände/Größen und erreichbare Bedienelemente korrekt.
- [ ] Echte native Screenshots und Testkommandos mit Umgebung, Ergebnis und
  verbleibenden Plattformlücken dokumentiert. Browserbilder als Referenzen erhalten.

## Prüfung und vorhandene Tests

Die Übergabe selbst wurde nur als Browserartefakt geprüft, siehe DESIGN.md.
Am 19. Oktober wurde zusätzlich `python tools/build.py --help` mit Exitcode 0
ausgeführt, um die unten genannten Optionen abzugleichen. Kein nativer Build
oder neuer nativer UI-Test wurde für diese Dokumentationsänderung ausgeführt.

Der nächste Chat führt nach seiner Implementierung passende Prüfungen aus.
Die tatsächlichen Testnamen sind in [native_tests.py](../../../tools/native_tests.py)
und den dort verwendeten Workflow-Katalogen registriert. Relevante Ausgangspunkte:
[editor_replace_tests.inc](../../../app/editor_replace_tests.inc),
[file_tabs_tests.inc](../../../app/file_tabs_tests.inc),
[docking_tests.inc](../../../app/docking_tests.inc),
[diagnostic_tests.inc](../../../app/diagnostic_tests.inc),
[document_recovery_tests.inc](../../../app/document_recovery_tests.inc),
[ui_size_tests.inc](../../../app/ui_size_tests.inc),
[test_ui_geometry.c](../../../tests/test_ui_geometry.c) und
[test_accessibility_ui.c](../../../tests/test_accessibility_ui.c).
Gezielte Regressionen für die neue Overlay-Geometrie und die feste Schalterposition
ergänzen; bestehende Tests nicht allein für neue Koordinaten abschwächen.

```text
python tools/build.py --config Debug --test
python tools/build.py --config Debug --test-display
```

`--test` und `--test-display` separat ausführen. Für gezielte Läufe ist wiederholtes
`--test-filter` mit registrierten Namen/Globs möglich. SDL-/Compilerpfade aus
dem aktuellen lokalen Setup und [build.md](../../build.md) verwenden, keine
vorhandenen Buildverzeichnisse pauschal leeren. Linux bei lokaler Verfügbarkeit
über die gemeinsame Distribution `Alpine-Dev` prüfen. macOS und Assistive-Technik
nur nach dort ausgeführter Prüfung als bestanden dokumentieren.
