# ADR 014: Freie Exploreransicht und Auswahldialog besitzen getrennte Zustände

Status: angenommen, 29. Oktober 2026. Ergänzt [ADR 013](013-native-explorer-filesystem.md)
und setzt die bereits freigegebene [Explorer-A-Auswahl](../visuals/design-options/APPROVED.md)
um. Es handelt sich um keine erneute Variantenentscheidung.

## Anforderung und Alternativen

Die freie A-Referenz enthält Orte, Werkzeugleiste, Dateiliste und feste Vorschau
im Arbeitsbereich. Nur die Auswahl eines Speicherorts oder Messlaufs verwendet
einen Dialog. Beim Unteraufruf aus Projektdialog B darf der freie Explorer seinen
Ort, Suchtext, Auswahl, Vorschau und Editierzustand nicht verlieren. Auch die
offenen Dokumente samt ungespeicherten Änderungen gehören weiterhin dem Editor.

Ein einzelner zurückgesetzter Browserzustand würde beim Öffnen eines Pickers die
freie Navigation ersetzen. Ein zeitweises Umschalten eines gemeinsamen
App-Zeigers ließe Zeichnung, Fokus und asynchrone Abschlüsse implizit von der
gerade gewählten Oberfläche abhängen. Eine Kopie des gesamten UI-Codes würde
hingegen zwei auseinanderlaufende Implementierungen erzeugen.

Gewählt sind zwei ausdrücklich besitzende Kontexte und gemeinsame UI-Funktionen.
`app.browser_workspace` besitzt die freie Ansicht, `app.browser` den Auswahldialog.
Jeder Kontext besitzt seinen Arbeiter, die übertragenen Momentaufnahmen, Verlauf,
Filter, Vorschau und vier Editierzustände. Zeichen- und Aktionsfunktionen erhalten
den Kontext ausdrücklich als Parameter. Es gibt keinen vorübergehend umgebogenen
globalen Browserzeiger. Die zusätzliche Momentaufnahme bleibt durch dieselben
Pfad-, Eintrags- und Vorschaugrenzen wie im bisherigen Modell begrenzt.

## Übergänge und Fehler

„Datei → Dateien …“ öffnet die freie Ansicht über die volle Inhaltsbreite innerhalb
der bestehenden drei Arbeitsbereiche. Die globale Navigation und Menüs bleiben
bedienbar. Eine bestätigte Datei wird über den bestehenden Öffnungsweg aktiviert;
eine Auswahl oder Vorschau allein ersetzt kein Dokument. Ein Fehler lässt die
Ansicht und die übrigen Dokumente erhalten. Eine überholte Bestätigung darf nach
dem Verlassen der Ansicht keinen fremden Arbeitsbereich ersetzen.

Projekt-, Workspace- und andere Manager erhalten ihre Eingabehoheit. Nur der
eigentliche Picker sperrt die Oberfläche darunter. Der freie Kontext bleibt beim
Unteraufruf erhalten und erhält keine Ergebnisse des Pickerarbeiters. Beide
Arbeiter werden vor dem Freigeben ihrer Besitzer beendet und verbunden.

Die freie Ansicht ändert keine gespeicherte Panelanordnung. Die bestehende
Analyse-/Labor-Kontextumschaltung erhält weiter ihre eigenen Anordnungen und
stellt beim Wechsel in den Entwicklungsbereich dessen Zustand wieder her.
Ein Test muss diese abgeschlossene UI-Transition abwarten und darf nicht die
Anordnung des gerade verlassenen Arbeitsbereichs als Entwicklungsanordnung
ausgeben. Der freie Navigationszustand wird innerhalb der Sitzung gehalten;
eine Wiederherstellung dieses Browserzustands nach Neustart ist nicht zugesagt.

## Abnahme

Eigene native Abläufe prüfen volle Inhaltsbreite, responsive Liste/Vorschau,
globale Navigation, Projekt B mit unabhängigem Picker, Workspace-Manager und
Protokollfläche. Ein absichtlich ungespeichertes Dokument muss Inhalt, Cursor,
Auswahl und Undo behalten. Unabhängige UIA-/AT-SPI-Clients prüfen die Pfadwerte
sowohl im Picker als auch in der freien Ansicht und die Rückkehr über eine
normale Headeraktion. Die tatsächlichen Ergebnisse und verbleibenden Grenzen
gehören in den nativen Einbindungsnachweis; diese Entscheidung allein ist
keine bestandene Laufzeit-, Bildschirmleser- oder Plattformprüfung.

Der folgende [native Einbindungsnachweis](../visuals/unified-workspace/EXPLORER-INLINE-NATIVE.md)
dokumentiert die tatsächlich bestandenen Windows-/Linux-Gruppen, die Diagnose
der gültigen ausgeblendeten Anordnung und die noch offenen visuellen Grenzen.
