# A2: native Textauswahl

Stand: 30. Oktober 2026. Dieser Teil übernimmt die Textfläche aus
[ADR 016](../../decisions/016-selected-marking-a2.md) und der
[verbindlichen A2-Spezifikation](../selection-options/APPROVED.md). Die gesamte
Listen-/Szenenmarkierung und die [Kontextmenüs A](../context-menu-options/APPROVED.md)
bleiben weitere Teile desselben Auftrags.

## Farb- und Rendervertrag

`text_selection` ist eine eigene semantische Rolle. Dunkel verwendet sie
RGB 170/184/202 mit 15 Prozent Deckkraft, Hell RGB 90/108/130 mit 11 Prozent.
Nuklear speichert Farbkanäle einschließlich Alpha als acht Bit; die nächsten
Alpha-Werte sind daher 38/255 und 28/255. Der Pixelvergleich erlaubt dafür
höchstens einen Kanalwert Abweichung vom exakt aus der Freigabe berechneten
Ergebnis. Die gemeinsame Hochkontrast-Markierung bleibt deckend und verwendet
ihre vorhandenen Kontrastrollen.

Die Rolle wird ausschließlich hinter den Zeichen gezeichnet. Sie ersetzt
keine Text-, Syntax- oder Fehlerfarbe und setzt keine Deckkraft auf das Feld
oder dessen Elternbereich. Normale und visuell umbrochene schreibgeschützte
Felder können im nativen Renderer die jeweils tatsächliche Schriftfarbe
beibehalten. Die Eigenschaft ist standardmäßig aus; Physims gemeinsames Styling
aktiviert sie sowohl für normale als auch für Property-Edit-Felder. Damit
wirken auch nachträglich gesetzte Fehlerfarben korrekt. Der Codeeditor behält
seine transparente native Textebene und zeichnet die unveränderte Syntax
über der Auswahl. Navigations-/Schaltermarkierungen besitzen weiterhin ihre
eigenen Rollen; sie sind kein Ersatz für die noch ausstehende A2-Listenmarkierung.

## Prüfung und Reproduktion

Der [kompakte Nachweis](../../verification/text-selection-a2-2026-10-30.json)
ordnet Quellen, Kommandos, native Resultate und Gegenproben zu. `text_selection`
prüft im Renderer normale, mehrzeilige und schreibgeschützte Felder mit
wechselnden Schriftfarben; außerdem bleibt das bisherige Verhalten ohne Opt-in
erhalten. `text_cursor` und `editor_readonly` prüfen die benachbarten Verträge.

`text_selection_a2` benutzt die wirklichen Code-, Such- und Vorschau-Widgets.
Eingereihte SDL-Ereignisse fokussieren und wählen den Text; die wirkliche
Zwischenablage liefert die Originalbytes einschließlich Unicode und zulässiger
Windows-CRLF-Umsetzung. Bilder vor und nach der Auswahl prüfen getrennt die
Hintergrundkomposition und die ursprünglichen Schriftpixel. Der Codefall enthält
mehrere Syntaxfarben. Die Beobachtung verwendet bei Nuklear ausdrücklich die
Pufferlänge; `nk_str_get_const` verspricht keine C-String-Terminierung.

Mit eingerichteter Windows-C17-Toolchain und lokaler SDL-Installation:

```powershell
python -X utf8 -B tools/with-artifacts.py -- python -X utf8 -B tools/build.py --config Release --build-dir build/text-selection-release --sdl third_party/SDL3-3.2.30 --test --test-filter text_selection --test-filter text_cursor --test-filter editor_readonly
python -X utf8 -B tools/with-artifacts.py -- python -X utf8 -B tools/build.py --config Release --build-dir build/text-selection-release --sdl third_party/SDL3-3.2.30 --test-display --test-filter text_selection_a2
```

Die isolierten tatsächlich ausgeführten Windows-/Linux-Kommandos stehen im
Nachweis. Die absichtlich deckende Auswahlfläche und eine absichtlich geänderte
Schriftfarbe müssen an den tatsächlichen nativen Rasterbildern scheitern;
der reguläre unveränderte Build muss dieselben Prüfungen bestehen.

## Weiterer Auftrag

Die zusätzlich ausgeführte F1-Kontextcodeprüfung ist weiterhin rot. Windows
scheitert im ersten Fall am veröffentlichten Fokuszustand, Linux meldet beim
Shift-Ereignis einen ungültigen Bool-Wert in `SDL_KeyboardEvent.repeat`.
Die unveränderte Prüfung reproduziert beide Befunde mit den archivierten
Vor-A2-Programmen. Der Nachweis enthält auch den zunächst ungültigen
Baseline-Aufruf ohne Ausgabeordner und die danach tatsächlich ausgeführten
Vergleiche. Diese F1-Befunde bleiben offen; die A2-Farbprüfung ersetzt sie nicht.

Dies ist keine vollständige A2- oder Oberflächenabnahme. Listen benötigen noch
Kante/Häkchen, Mehrfachauswahl und die gemeinsame Kontextbedienung. Für die
Szene bleiben Kontur, Griffe, Auswahlrahmen und die Abgrenzung vom Kameradrag.
Die neunteilige native Menübindung aus ADR 021 sowie sichtbare Auswahl beim
geöffneten Menü sind gesondert umzusetzen und zu prüfen. Ebenso offen bleiben
die von der Auswahl getrennte gestrichelte Fokusmarkierung,
physische Eingabe, echte Screenreader, weitere Monitor-/DPI-Zustände und eine
aktuelle macOS-Ausführung. Die bestehenden Aufgabenfenster, B2, Einstellungen,
Rückmeldungen und Fortschritt bleiben Teil des Gesamtauftrags.
