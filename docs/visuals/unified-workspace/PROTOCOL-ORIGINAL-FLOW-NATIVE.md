# Protokoll C: Originaltext mit natürlicher Höhe

Der Originaltext übernimmt jetzt die natürliche Höhe des freigegebenen
CSS-`pre`: 11,2 logische Pixel Schrift, 20,72 Pixel Zeilenhöhe und jeweils
11,2 Pixel Blockabstand bei Skalierung 1. Der bisherige eigene, auf
110–200 Pixel begrenzte Editor entfällt. Lange Texte scrollen im umgebenden
Detailbereich. Der [vorherige Umbruchnachweis](PROTOCOL-ORIGINAL-NATIVE.md)
bleibt als datierter Zwischenstand erhalten.

Zeichen, Cursor, Auswahl und native Zeichenrechtecke verwenden weiterhin
denselben Zeilenplan bei unveränderten Originalbytes. Strg+Ende macht das Ende
über den Detailbereich sichtbar. Eine nötige Scrolländerung wird beim nächsten
Gruppenbeginn angewandt, damit Text und nachfolgende Aktionen nicht innerhalb
eines Frames unterschiedliche Scrollstände verwenden. Das Feld fügt weder
Rahmen noch Hintergrund, Innenabstand oder eigene Scrollleiste hinzu.

## Belegte Fehler und Prüfungen

| Soll | Tatsächliches Ergebnis |
| --- | --- |
| Höhe entspricht Zeilenanzahl × Zeilenhöhe | Fünf native Originaltextfälle je Plattform bestehen: Dunkel, Hell, hoher Kontrast, große Schrift und schmales Fenster. Innerer Offset bleibt 0; der Detailbereich erreicht das letzte Zeichen. |
| Original und Quelle erhalten | Je 12.288 Byte, exakte SDL-Zwischenablage, Auswahl nach tatsächlichem Resize und gesperrtes Löschen bestehen auf Windows/Linux. Die Endzeichenprobe verlangt sichtbare Pixeltinte. |
| Unsichtbare Feldteile übernehmen keine Klicks | Neue Unit-Gegenprobe scheitert am vorherigen Nuklear-Stand mit `!edit.active`. Nach Begrenzung der Aktivierung auf Feld **und** Elternclip bestehen unsichtbarer und sichtbarer Zeigerfall. |
| Zeichenrechtecke für eine vollständig sichtbare Seite | Der alte Vorrat von 2.048 Rechtecken scheitert im gezielten nativen 12-KiB-Fall. 16.384 Rechtecke bestehen; beide Varianten wurden mit ASan/UBSan ausgeführt. |
| Prüfer erkennt falsche Belege | Echter Windows-Lauf wird akzeptiert; fehlender Kopierschritt, geänderte Originalbytes, falsches Kopierergebnis und leere Endzeichenregion werden jeweils abgewiesen. Dafür entstehen keine neuen Bilder oder App-Läufe. |
| Relevante Regressionen | Windows: sieben Unit-Gruppen; Linux: neun Unit-Gruppen und vier Displaygruppen. Linux verwendet ASan/UBSan. Diagnoseabläufe und unabhängige F1-Editclients bestehen zusätzlich unter Windows. |

Die [kompakte Ergebnisliste](../../verification/protocol-original-flow-2026-10-26.json)
verknüpft genaue Befehle, Exitcodes, Quell-/Binärhashes, Gegenproben und lokale
Archive unter `D:/Physim/build/validation/protocol-wrap-2026-10-26/8-original-flow`.
Die zwölf eigenen Dateien stimmen bytegleich mit der isolierten Testkopie
überein. Andere lokale Änderungen sind dort ausgeschlossen. Bestehende
Laufzeitdateien werden anhand des bisherigen Manifests wiederverwendet;
geänderte Programme und Symbole sind nach dem Packen vollständig nachgelesen
und hashgeprüft. Frühere Fehlerbelege werden nicht überschrieben.

Unter Linux tatsächlich gemessen: 14.640 Byte je Accessibility-Knoten und
8.545.544 Byte je Modell. Die größere Rechteckkapazität erhöht das Modell
gegenüber dem vorigen Stand um 917.504 Byte. Zeichenrechtecke werden nur für
den sichtbaren Zeilenbereich erzeugt. Dies ist keine allgemeine Performance-
oder Speicherabnahme.

## Windows-Fokus und Grenzen

Die ersten Windows-Navigatorläufe scheiterten vor der Separatorprobe an
fehlendem Host-Tastaturfokus: `host=0`, obwohl das Suchfeld intern aktiv war.
Ein neuer isolierter Lauf besteht unverändert alle acht Fälle mit `host=1`.
Nach den übrigen Windows-Displaytests wurde ausschließlich die Testausgabe um
diese Fokuskennungen ergänzt; der Bericht bindet beide Binärstände getrennt.
Eine Computer-Use-Fensteraktivierung fand dabei nicht statt. Die zunächst
unzutreffende Helfer-Metadatenangabe wurde in einer separaten, verknüpften
Korrektur richtiggestellt; die ursprüngliche Rohdatei bleibt erhalten.
Dies schließt den [direkten physischen Separatorfokus](PROTOCOL-FOCUS-NATIVE.md)
weiterhin nicht ab.

Folgende Abweichungen bleiben konkrete Umsetzungsaufgaben:

- Originaltext ist weiterhin in Abschnitte bis 12 KiB aufgeteilt. Die Vorlage
  zeigt ein durchgehendes `pre`; die Begrenzung folgt dem bestehenden nativen
  Edit-Snapshot und ist keine unvermeidbare Plattformgrenze.
- Der Detailinnenabstand beträgt weiterhin 18 statt der im Katalog-CSS
  vorgesehenen 20 Pixel. Quellsprung und Kopieraktionen verwenden noch breite
  Zeilen statt der inhaltsbreiten Flex-Aktionen der Vorlage.
- Der native Tastaturcursor reserviert 1 Pixel Breite. Dadurch kann ein
  Grenzumbruch vom CSS-Text abweichen. Der feste native Dockbereich scrollt
  seine Details, während die HTML-Katalogseite mit dem Inhalt wachsen kann.
- Die unabhängigen UIA-/AT-SPI-Clients prüfen F1-Eingabefelder; der lange
  Originaltextpfad mit nativem `ScrollIntoView` ist noch separat zu prüfen.
  Tatsächliches Screenreader-Lesen, macOS sowie vollständige C-/H- und
  Gesamtgestaltung sind weiterhin offen.

Der Browser hat die lokale interaktive Vorlage in diesem Nachlauf nicht
geöffnet. Grundlage sind die vorhandenen HTML-/CSS-Quellen und echte native
Aufnahmen; daraus folgt keine neue Browser- oder vollständige Pixelabnahme.
Die Linux-GUI-Prüfung läuft unter Xvfb/Openbox mit Software-OpenGL und
`detect_leaks=0`; sie belegt keine Leak-Freiheit.

## Bild- und Speicheraufbewahrung

Für die Dokumentation genügen zwei unveränderte Endzustände:
[Windows dunkel](protocol-original-flow-2026-10-26/windows-dark.png) und
[Linux hell](protocol-original-flow-2026-10-26/linux-light.png).
Die fünf Originaltextmodi behalten jeweils nur ihre gezielte Endaufnahme;
Tastatur-, Kopier- und Resizeaktionen stehen als Text-/JSON-Zustände im Bericht.
Modellgrößenmessung und Prüfergegenproben erzeugen keine weiteren Bilder.
Bereits eingefrorene Nachweise und offene Fehlerbilder bleiben erhalten.
Alle neuen Dateien liegen auf D:; F: wurde nicht verwendet.
