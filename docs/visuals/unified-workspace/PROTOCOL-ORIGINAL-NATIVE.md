# Protokoll C: Originaltext mit weichem Umbruch

Der Originaltext verwendet jetzt einen nativen visuellen Zeilenplan mit
bevorzugten Wortumbrüchen und zusätzlichem Umbruch langer Bezeichner. Grundlage
ist das freigegebene `pre-wrap`/`overflow-wrap:anywhere` aus
[options.css](../design-options/options.css): 11,2 logische Pixel Schrift und
20,72 Pixel Zeilenhöhe bei Skalierung 1. Die Originalbytes werden durch den
Umbruch nicht verändert. Die bisherigen
[Diagnosenachweise](PROTOCOL-DIAGNOSTICS-NATIVE.md) behalten ihren datierten
Geltungsbereich; der dort offene lange Originaltext ist hier weiterbearbeitet.

## Umsetzung und Grenzen

Zeichnen, Trefferprüfung, Cursor, Auswahl und native Zeichenrechtecke verwenden
denselben unveränderlichen Zeilenplan. Notumbrüche und Pfeilbewegungen erhalten
erweiterte Graphemcluster. Home/End beziehen sich auf die sichtbare Zeile;
Strg+Ende erreicht das Originalende. Kopieren erhält UTF-8, Tabulatoren und CRLF.
Eine neue Breite berechnet nur die Darstellung neu und erhält die Auswahl.
Schreibzugriffe bleiben im Editor und in den nativen Accessibility-Adaptern
gesperrt. Ein einzelner Cluster, der breiter als der verfügbare Platz ist,
bleibt vollständig und kann deshalb überbreit sein.

Der vorhandene libunibreak-Stand 8.0 wurde um die zugehörigen unveränderten
Graphemdateien und den offiziellen Testkorpus ergänzt. Die
[Herkunftsliste](../../../third_party/libunibreak-8.0/manifest.json) enthält
Commit, Git-Blob- und SHA-256-Werte. Dies führt keine neue Text-Shaping-Engine
ein: Die vorhandenen Fonts und der Renderer begrenzen weiterhin komplexe
Schriftformen und Emoji. Clustergrenzen allein beweisen keine vollständige
Glyphendarstellung.

Die Anzeige behält vorerst den bestehenden eigenen Scrollbereich mit
110–200 logischen Pixeln Höhe und Textabschnitten von höchstens 12 KiB.
Das CSS-`pre` hat dagegen natürliche Höhe im scrollenden Detailbereich.
Diese Layoutabweichung ist **noch offene Umsetzung**, keine unvermeidbare
Plattformgrenze. Sie und die übrigen Abstände brauchen einen direkten
Referenzabgleich. Aus dem Umbruchnachweis folgt keine vollständige Designgleichheit.

Ein früherer Windows-Endzustand zeigt außerdem einen Tooltip „Funktion · Zeile
59“ aus dem Codekontext über dem Protokoll. Die spätere Messung bewegt nur den
Prüfzeiger zur neutralen Kopfzeile; das verhindert eine verdeckte Endzeichenprobe,
schließt aber diesen Hover-/Überlagerungsbefund nicht. Nächster Schritt ist die
Treffer-/Clippingprüfung von `app/source_outline_ui.inc` bei offenem Protokoll.

## Gefundene Ursachen und Korrekturen

| Befund | Belegte Ursache und Korrektur |
| --- | --- |
| Lange Zeile abgeschnitten | Nuklear bricht den bisherigen Editor nur an echten Zeilenumbrüchen um. Ein optionaler visueller Pfad ergänzt weiche Zeilen ohne zusätzliche Originalbytes. |
| Falsche Bewegung am weichen Zeilenende | Dieselbe Byteposition kann Zeilenende oder Anfang der Folgezeile sein. Die Cursor-Affinität hält beide Zustände auseinander; vertikale Treffer verwenden die Zeilenmitte statt gerundeter Grenzwerte. |
| Windows-Absturz bei langer Originalmeldung | ASan meldet einen Überlauf beim Kopieren von 6.050 Byte in den alten 1.024-Byte-Providercache. Dessen Größe folgt jetzt der Modellkapazität; der volle 12-KiB-Fall besteht anschließend unter Windows-ASan. |
| Linux-Absturz vor der Darstellung | Gemessener SDL-Standardstack etwa 134 KiB, ASan-instrumentierter D-Bus-Handler etwa 156 KiB. Nur der AT-SPI-Dispatcher erhält explizit 1 MiB Stack. |
| Leer werdendes Protokoll nach Verbreitern | Der alte äußere Scrolloffset blieb erhalten, obwohl das Layout ohne äußere Scrollleiste vollständig passte. Der passende Zustand setzt nur diesen äußeren Offset zurück; innere Text-/Listenpositionen bleiben unabhängig. |
| Bedingter Trennstrich außerhalb der Zielbreite | Ein bevorzugter Soft-Hyphen-Umbruch wurde trotz zusätzlicher Strichbreite gewählt. Der Anywhere-Pfad prüft nun auch diese Breite; der bisherige F1-Pfad bleibt erhalten. |

Das native Accessibility-Modell enthält nun bis zu 12 KiB Originaltext je
Edit-Snapshot. Unter Linux gemessen: 14.640 Byte je Knoten, 7.628.040 Byte je Modell.
Das ist ein zusätzlicher begrenzter RAM-Bedarf; eine allgemeine Performance-
oder Speicherabnahme wird damit nicht behauptet.

## Prüfweg und Aufbewahrung

Die [kompakte Ergebnisliste](../../verification/protocol-original-2026-10-26.json)
verknüpft die tatsächlich ausgeführten Kommandos, Quell-/Programmhashes,
Exitcodes, Zustandsfolgen und Gegenproben. Eigene Änderungen wurden auf eine
isolierte Kopie des Git-Ausgangsstands gelegt. Andere lokale Änderungen bleiben
ausgeschlossen; im AT-SPI-Hauptadapter sind nur die beiden eigenen Änderungen
an Textzustand und Threadstack enthalten.

Die zwölf gezielten Unit-Gruppen enthalten alle 766 offiziellen Graphemfälle
und alle 19.338 Zeilenumbruchfälle. Der native Originaltextfall prüft Dunkel,
Hell, hohen Kontrast, große Schrift und das schmale Fenster mit 12.288 Byte
Originaltext: Ende erreichbar, letztes Zeichen im Viewport mit sichtbarer
Pixeltinte, exakte SDL-Zwischenablage, Auswahl nach Resize, blockiertes Löschen
und unveränderte Quelldatei. Navigatorprüfungen decken acht Zustände ab.
Gegenbeispiele müssen sowohl defekte Umbrüche als auch unvollständige oder
veränderte Prüfausgaben und eine leere Endzeichenregion abweisen.

Die vier Gegenbelege sind zusätzlich als wiederholbarer
`tests/check_protocol_original_counterexamples.py` versioniert. Er wird mit
`--root D:/Physim --work <abgeschlossener dunkler Originaltext-Prüfordner>`
aufgerufen, akzeptiert zuerst den echten Lauf und verändert anschließend nur
dessen eingelesene Zustandskopien. Seine JSON-Ausgabe liegt im Prüfordner;
zusätzliche Bilder oder Programmläufe entstehen dadurch nicht.

Die zusätzlich ausgeführte unabhängige F1-Editprüfung ist **fehlgeschlagen**:
Windows-UIA findet in beiden Fällen das aktuelle Dokument-Suchfeld nicht,
Linux-AT-SPI findet es ebenfalls nicht. Derselbe Zugriff scheitert auch mit dem
vorherigen isolierten Vergleichsbuild. Dessen 390 App-/SDK-/Nuklear-Quelldateien
stimmen nach reiner CRLF-Normalisierung mit dem bisherigen Git-Stand überein;
die Programme wurden aus den hashgeprüften bestehenden Archiven kurzzeitig
wiederhergestellt. Das grenzt den Befund als bereits bestehenden Prüf-/UI-Fehler
ein, schließt ihn aber nicht. Aktuelles Programm und Vergleichsprogramm bleiben
an ihre tatsächlichen unterschiedlichen Hashes gebunden. Ein vollständig
bestandener nativer Edit-/Accessibility-Regressionslauf wird nicht behauptet.
Die vier temporären Programm-/SDL-Kopien mit 57.764.224 Byte wurden anschließend
hashgeprüft entfernt; ihre ursprünglichen Archive bleiben erhalten. Das ist
keine zusätzliche Nettofreigabe aus historischen Datenbeständen.

Pro Originaltextmodus entsteht nur ein Endzustandsbild. Die elf Eingabeschritte
je Modus sind JSON. Nur eine ausgewählte Aufnahme wird zusätzlich versioniert;
der [Windows-Endzustand](protocol-original-native-2026-10-26/original-end-windows.png)
zeigt den vollständigen Schluss der umbrochenen Nachricht ohne Tooltip darüber.
Die übrigen unterschiedlichen Themes und Fehlerbelege bleiben lokal unter
`D:/Physim/build/validation/protocol-wrap-2026-10-26`. Quell-/Laufzeitarchive
werden vor dem Ersetzen hashgeprüft gesichert. Ersetzte Objekt-Caches sind im
[Speicherbericht](../../storage-maintenance-2026-10-26.md) quittiert.

Windows nutzt MSVC Release; Linux `Alpine-Dev`, GCC Debug mit ASan/UBSan,
Xvfb/Openbox und Software-OpenGL. Die GUI-Prüfung deaktiviert Leak-Erkennung.
macOS wurde hier nicht gebaut oder bedient. Tatsächliches Screenreader-Lesen,
physische Maus-/Tastaturfokusführung, mehrere DPI-Stufen und die vollständige
Oberfläche bleiben offen. Die
[bisherige physische Fokuslücke](PROTOCOL-FOCUS-NATIVE.md) ist durch synthetische
SDL-/Modellaktionen nicht geschlossen.

Die äußere lokale Prüfhülle meldete beim letzten Windows- und Linux-Aufruf
jeweils Exitcode 1: Sie erwartete ein unverändertes, bereits gebautes Programm,
der Aufruf baute jedoch die zuvor geänderte Aufnahmevorbereitung neu. Der
Builder und beide nativen Prüfer endeten jeweils mit 0. Ihre pro Lauf erfassten
Binärhashes entsprechen den abschließend nachgelesenen Laufzeitarchiven.
Die Ergebnisliste trennt diese Hüllenassertion vom tatsächlichen Testergebnis;
ein grüner Gesamtstatus der äußeren Hülle wird nicht behauptet. Künftig zuerst
bauen und danach die Hülle mit unverändertem Programm ausführen.

Auch der ältere erfolgreiche Windows-ASan-Nachlauf hat eine Provenienzgrenze:
Sein roher `source_manifest`-Verweis zeigt noch auf einen früheren Stand. Das
positive Programm und seine Logs bleiben erhalten, sind darüber aber keine
vollständig eingefrorene Quellbaumzuordnung. Die neuen isolierten Archive
belegen den aktuellen Quell-/Laufzeitstand getrennt davon.
