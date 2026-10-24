# Analysecode im gemeinsamen nativen Werkbank-Aufbau

Der gemeinsame Referenzadapter in `workspace.js` / `bridge.js` öffnet Analysecode
unter „Entwickeln“. Die frühere native Route zeigte dagegen den alten Editor
unter „Auswerten“, einschließlich generischer Datei-, Szenen- und Einstellungsleisten.
Der bestehende Editor wird jetzt mit der ausgewählten Werkbank verbunden.

## Umsetzung

„Analysecode“ öffnet dieselbe Projektdatei und denselben Editorpuffer wie der
Dateibaum und die Dateitabs. Kompakte Tabs, Workspace-Pfadlinien, schwebende
Suche, Codekontext, Gliederung und Protokoll verwenden damit den gemeinsamen
Werkbank-Aufbau. Die Hauptnavigation und der Bereichspfad nennen „Entwickeln“
bzw. „Werkbank“. Dateibaumauswahl und Editorstatus zeigen die tatsächliche
Analysedatei; dort wurde zuvor bei diesem Einstieg fälschlich `main.c` genannt.

„Auswerten“ führt zu Messdaten bzw. dem weiterhin gewählten Analyseergebnis
zurück. „Entwickeln“ nimmt die zuletzt geöffnete Datei wieder auf. Die bestehende
interne Editoridentität bleibt erhalten, ebenso Such-, Undo- und Dateipuffer.
Beim Öffnen aus dem Analysekopf wird die Werkbankanordnung vor der ersten
Panelmessung wiederhergestellt. Ihre Größen werden getrennt von der Auswertung
gehalten; ein offener Ziehvorgang wird auch beim Wechsel dieser Unteransicht
abgebrochen. Die gespeicherte Hauptbereichsangabe entspricht „Entwickeln“.

Die Dateiauswahl übergibt sowohl den lokalen Editfokus als auch die Tastatur
an das Codefenster. Die Besitzinformation bleibt bis zu einer ausdrücklichen
anderen Aktion bestehen. Suche, Menüs, Klicks, modale Übergänge und native
Fokusanforderungen anderer Bedienelemente haben Vorrang. Eine native
Kopfaktion kann ihren Fokus noch nach Beginn des Frames erhalten; diese
Übergabe wird deshalb vor dem Aktivieren des Editors nochmals berücksichtigt.

## Zusätzlich gefundener Undo-Fehler

Die vorhandene C-Analysedatei enthält mehr als 32767 Zeichen. Nuklear kürzte
beim Undo eine Cursorposition auf `short`, obwohl Position und Cursor bereits
`int` sind. Dadurch wurde der Cursor negativ. Linux meldete beim anschließenden
Zeichnen eine Assertion. Ein früherer Windows-Release-Lauf blieb ohne Assertion
grün; sein Prüfumfang verlangte noch keinen gültigen Cursor nach Undo. Dieser
Zwischenstand gilt ausdrücklich nicht als abschließende Abnahme.

Die Verengung entfällt. Die unabhängige Kernprüfung testet ASCII und UTF-8 bei
32766, 32767, 32768, 34869 und 65535 Runen, ergänzt ein vier Byte langes Zeichen
und prüft Cursor, Auswahl, Inhalt sowie Undo/Redo. Die alte Fassung scheitert
auf beiden Plattformen. Die native Prüfung verlangt zusätzlich den korrekten
Cursor am Dateiende nach Unicode-Eingabe und Undo. Die
[Drittanbieter-Patchnotiz](../../../third_party/README.md) beschreibt den Eingriff.

## Ausgeführte Prüfungen

Windows/MSVC Release und Alpine-Dev/GCC mit ASan/UBSan bestehen jeweils vier
UI-Gruppen und zwei Kerntests:

- `analysis_editor`: C- und Physim-Analysecode mit System- und gebündelten
  Schriften, jeweils Dunkel/Hell sowie UI 22 im schmalen hellen und kontrastreichen
  Fenster. Zusätzlich Analyseprojekte ohne Experiment: C mit Systemschrift und
  Physim mit gebündelter Schrift. Zusammen 84 neue native Aufnahmen.
- `file_tabs_workflow`, `editor_replace_workflow`, `analysis_layout_dark`:
  vorhandene Dateitab-, Ersetzen-/Speicher- und Analyse-/Teilfensterabläufe;
  insgesamt weitere 50 Bilddateien. Die Ersetzenprüfung verwendet C und Physim
  sowie Experiment-, Analyse- und gewöhnliche Dokumentpuffer.
- `editor_typing_undo`, `utf8_string_bounds`: tatsächliche unabhängige
  Cursor-/Text- und UTF-8-Bereichsprüfungen. Der Runner-/Katalogtest besteht
  ebenfalls auf beiden Plattformen.

Die neue Fixture verwendet echte gespeicherte, ausdrücklich synthetische
Messdateien und einen mit der Report-API erzeugten Testbericht. Sie ist kein
Physik- oder Analyseprozessnachweis. Auswahlreihenfolge, betrachtete Daten,
Lauf-/Berichtsdateien und Editorinhalte nach Undo bleiben erhalten. Die
Tastaturprobe führt ohne zusätzlichen Klick vom Bereichswechsel bis zum
Schreiben, Undo und Speichern. Eine native Fokusanforderung an die Darstellung
und F10/Escape prüfen anschließend die Abgabe der Tastatur.

Die [eingefrorenen Belege](analysis-editor-2026-10-24/README.md) unterscheiden
alle Quellstände, verworfene Versuche, den Debuggerbefund, tatsächliche Befehle,
Binärhashes und die abschließende UI-Quelle 9. Die Kerntests stammen aus Quelle 8;
ihre Eingaben und vier Binärdateien sind bis Quelle 9 unverändert nachgewiesen.
Der Bildprüfer bestätigt unter anderem ausgewählten Hauptbereich, kompakte
Tabmaße, sichtbaren Dateinamen, aktive Dateibaumzeile und Position der Suchinsel.
Der alte Editor wird durch dieselbe begrenzte Navigations-Farbprüfung
zurückgewiesen. Eine weitere Gegenprobe verwirft die funktional grüne Quelle 8
wegen ihrer noch fehlenden Dateibaummarkierung. Die Pfadberechnung der letzten
UI-Fassung berücksichtigt deshalb ausdrücklich den Analyseeditor.

## Verbleibende Grenzen

Der aktive Quelltext und die Gliederung stammen aus dem wirklichen Projekt;
der gekürzte Pendelausschnitt der HTML-Vorlage wird nicht eingesetzt. Projekte
ohne Experiment erhalten keinen erfundenen Experimenttab. Modellnotizen,
weitere Werkbankzustände und der vollständige Abgleich aller Arbeitsbereiche
einschließlich F1 bleiben offen. Dieser Schritt ist keine 100-%-Abnahme.

Die Prüfungen verwenden interne SDL-Eingaben und native semantische
Fokusanforderungen. Praktische Screenreader-Aufgaben, physische Tastatur-/
Monitor-/DPI-Wechsel und macOS bleiben getrennt unbewiesen. Die Grafikläufe
deaktivieren Leakprüfung; die aufgeführten Kerntests laufen in derselben
Hilfsumgebung. Daraus wird keine eigenständige Leakabnahme abgeleitet.

Fehlgeschlagene Rohaufnahmen, der ursprüngliche Gegenfall und die aktuelle
Serie werden weiter benötigt und bleiben erhalten. Es wurden keine weiteren
SDK-Kopien angelegt oder aktive/fremde Bestände bereinigt.
