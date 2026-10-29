# Explorer A: C-API-Kontext

Stand: 29. Oktober 2026. Ergänzt den [Vorschauabgleich](EXPLORER-PREVIEW-NATIVE.md)
für die [freigegebene Variante A](../design-options/APPROVED.md).
Der [kompakte Nachweis](../../verification/explorer-api-context-2026-10-29.json)
enthält Quellen, Laufzeiten, Kommandos und tatsächliche Ergebnisse. Dieser Text
behauptet keine vollständige Designabnahme. Der neu beauftragte
[Vertrag für eigenständige Aufgabenfenster](../../decisions/015-independent-task-windows.md)
ist eine anschließende Änderung mit eigener Abnahme und gehört nicht zu diesen
Explorer-Laufzeiten.

## Inhalt und Eigentum

Die Vorschau sucht im tatsächlich eingelesenen C-Quelltext nach einem dokumentierten
C-Symbol. Bevorzugt wird die erste bekannte Funktion mit folgendem `(`, sonst der
erste bekannte Bezeichner. Kommentare, Zeichenketten, Zeichenliterale und
Membernamen liefern keinen erfundenen API-Bezug. Die Auswahl ist lexikalisch:
Präprozessorbedingungen, Makroexpansion und die tatsächliche Ausführung werden
nicht ausgewertet. Der Bezug gilt für den eingelesenen Vorschaupräfix von höchstens
64 KiB; spätere Dateiteile werden nicht als untersucht dargestellt.

Der vorhandene Dateiarbeiter lädt das tatsächliche Modul unter
`docs/reference/<Modul>.md`, prüft UTF-8 und parst den passenden Abschnitt.
Die Handbuchdatei ist auf 256 KiB begrenzt; eine gekürzte Referenz wird abgewiesen.
Datei und Handbuchwurzel gehören demselben versionierten Auftrag. Nach schneller
Neuauswahl darf ein alter Auftrag weder Text noch Dokument austauschen. Neue
Aufträge behalten bereits übertragene Dokumente unverändert; Abbruch und Shutdown
geben nicht veröffentlichte Dokumente frei. Weder Dateizugriff noch Markdownparser
laufen während der UI-Zeichnung.

Fehlende oder beschädigte Referenzen beeinträchtigen die Quelltextvorschau nicht.
Ein gefundenes Symbol erhält dann einen ausdrücklichen Ladefehler. Ohne ein
dokumentiertes C-Symbol erscheint kein fiktiver Kontext. Physim- und andere
Textdateien aktivieren diese C-Auswahl nicht.

## Native Oberfläche

Die aufklappbare Zeile folgt unter dem Quelltext. Darunter stehen wirklicher
Symbolname, unveränderte Handbuchsignatur, Beschreibung, vorhandene Parameter und
der Sprung in den entsprechenden Abschnitt des F1-Fensters. Die Signatur hat eine
eigene schreibgeschützte native Textauswahl mit gemeinsamem Umbruchmodell.
Kopieren und Resize behalten die Originalbytes und Auswahl; die Referenz ersetzt
keine offene API-Karte in der Werkbank. Picker und freie Exploreransicht behalten
eigene Dokumente, Aufklappzustände, Umbruchkarten und Textfeldzustände.

Die Schriftgrößen folgen dem wirksamen CSS: Kontext 10,92 px, Überschrift 11,9 px,
Signatur und Tabelle 10,08 px; Signaturzeilenhöhe 1,85. Die Tabelle berücksichtigt
Inhaltsbreiten, Umbruch und 8/5 px Zellabstand. Kopfzeile und Trennlinien verwenden
die ausgewählten semantischen Farben. Der native Fontabgleich für den speziellen
CSS-Kopfzeilenwert `font-weight:500` bleibt gesondert offen.

Die Browserdemo zeigt fest `ps_scene_add_id` und den Zusatz „Beispiel“. Die native
Ansicht entfernt den reinen Demohinweis und verwendet das tatsächlich passende
Symbol und dessen Dokumentation. Unterschiedliche Signaturlängen, Beschreibungen
und Parameter sind dadurch echte Datenunterschiede; sie sind keine technische
Begründung für eine abweichende Anordnung.

## Nachweise und verbleibender Umfang

Windows Release und Linux Debug mit ASan/UBSan einschließlich Leakprüfung bestehen
die Symbol-, API-Parser- und tatsächlichen Dateiarbeiterprüfungen. Eine Gegenvariante
ohne API-Laden scheitert an der erforderlichen Referenz. Geprüft sind unter anderem
überholte bereits geparste Ergebnisse, übertragene Eigentümerschaft, fehlende oder
falsche Module, UTF-8/BOM und die genaue Dokumentgrößengrenze.

Der abschließende native Nachlauf besteht auf Windows Release und Linux Debug
mit ASan/UBSan jeweils acht Gruppen. Darin sind die vorhandenen unabhängigen
UIA-/AT-SPI-Prüfungen für Explorer und Textvorschau sowie der Projektdialog B
enthalten. Die GUI-Läufe verwenden unter Linux `detect_leaks=0`; die separaten
Symbol-/Parser-/Workerprüfungen verwenden `detect_leaks=1`. Die neue Gruppe
prüft Hell, Dunkel, hohen Kontrast und große Schrift,
Auf-/Zuklappen, unverändertes Kopieren, Resize, den tatsächlichen F1-Sprung und
Neuauswahl einer Physim-Datei. Je ein gezieltes Windows-/Linux-Bild bestätigt
Aufbau, Signatur, Tabelle und sichtbaren Fokus; ein Bild beweist keine Assistive-
Bedienung. Die erste Variante des UI-Tests klickte nach dem Scrollen auf eine nicht
sichtbare Dateizeile; die korrigierte Aufgabe verwendet Suche und Tastatur.
Die Tabellenprüfung des ersten Worker-Tests hatte den Markdown-Trenner fälschlich
als Datenzeile gezählt. Die Fehlversuche bleiben mit Quellen/Laufzeit erhalten.

Offen bleiben eigenständige native Reader-/Screenreader-Aufgaben für den neuen Kontext,
zusätzliche DPI-/Monitorzustände, 200 % Text, macOS, weitere lange Tabellen und die
vollständige visuelle Abnahme. Die hier eingeführten Textbeschriftungen für
Tabellenzeilen beweisen keine native Tabellen-Collection-Semantik. Messlauf- und
Bildvorschau sowie weitere Punkte der [Gesamtmatrix](NATIVE-FIDELITY.md) sind
ebenfalls noch offen.

Die am selben Tag zusätzlich freigegebene [Markierung A2](../selection-options/APPROVED.md)
hat einen eigenen nativen Umsetzungsauftrag. Die hier geprüfte ursprüngliche
Textauswahl ist kein Nachweis ihrer fertigen Übernahme.
