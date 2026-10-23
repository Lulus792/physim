# Zeitleiste: Originalaufteilung und echte Zeitnavigation

Stand: 23. Oktober 2026, Grundlage `ca16fc4a95fb1854d315df90f6ae37c0106f918f`.
Die gesamte Oberfläche bleibt in Arbeit. Die zugehörigen Quellgraphen,
Vorstufen und Laufspuren stehen unter [timeline-2026-10-23](timeline-2026-10-23/README.md).

Die native Zeitleiste übernimmt die Aufteilung aus `simulation.css` und den
unveränderten gemeinsamen Referenzbildern: Kopf mit Zustandsbeschreibung,
rechts Abspielen und Live, darunter die große Zeitangabe, die Spur und ihre
Beschriftung. Bei Standardschrift gelten 18 px Außenabstand, 24,5 px hohe
Aktionen mit 6 px Abstand, 120 px Mindestbreite für die Zeitanzeige,
25 px Abstand zur Spur und 17 px Eingabehöhe. Die Schriftrollen folgen den
originalen 11/10/18/11/9 CSS-px. Es kommen keine Auslagerungsbuttons hinzu.

Die Skala verwendet lesbare 1-/2-/5-Schritte pro Größenordnung und höchstens
sieben Marken. Zeiten verwenden das deutsche Komma. Bei extremen Intervallen
bilden eindeutig beschriftete Endpunkte den Rückfall. Die vollständige präzise
Zeit, der erhaltene Zustandsindex und der Hinweis auf ausgedünnte Vorschauen
stehen auch im nativen zugänglichen Text der Zeitangabe. Ein großer Hovertext
über der Skala wurde entfernt.

Die Maus wählt anhand des tatsächlichen Zeitpunkts den nächsten gespeicherten
Zustand. Das ist bei adaptiven Zeitschritten etwas anderes als eine lineare
Umrechnung auf den Arrayindex. Links/Rechts bewegen genau einen erhaltenen
Zustand; Pos1/Ende springen zum Anfang/Ende, Leertaste startet oder stoppt die
Wiedergabe. Der laufende Versuch und seine Messdatei werden dabei nicht
weitergerechnet oder verändert. Abspielen/Anhalten behalten bei der Änderung
ihrer Beschriftung den tatsächlichen Tastaturfokus.

Ein einzelner SDL-Ereignisblock mit Drücken, Bewegen und Loslassen wird in
seiner Reihenfolge verarbeitet. Escape, Größenänderung und echter Fokusverlust
brechen eine aktive Ziehbewegung ab und stellen den vorherigen Vorschauzustand
wieder her. Beim Öffnen eines Projekts wird diese Wiederherstellung verworfen:
Ein späteres Escape darf keine Szene des alten Projekts zurückholen. Auch die
alte Leerlaufzeit und Messwerte werden beim erfolgreichen Projektwechsel
zurückgesetzt. Ungültige Projekte werden weiterhin vor diesem Austausch geprüft.

Hauptfenster und herausgezogene Zeitleiste verwenden denselben Zeichenpfad.
Die bestehende Überschrift bleibt der Ziehbereich; Schließen des nativen
Zusatzfensters setzt den Inhalt an seine vorherige Stelle zurück. Der
Mindestfall des Zusatzfensters ist 480×160; dort bricht die Zustandsbeschreibung
um. Im Hauptfenster werden bei 95 px Mindesthöhe und UI-Größe 22 die senkrechten
Zwischenräume komprimiert. Die Schrift und die Bedienflächen bleiben groß.
Eine identische vertikale Luftigkeit passt dort geometrisch nicht in die
vorgesehene Mindesthöhe; bei normaler Höhe gelten die Originalabstände.

Der gemeinsame Trenner meldet weiterhin seinen nativen einstellbaren Wert.
Sein Fokus wird als vorhandene ein Pixel breite Linie gezeichnet. Der vorher
zusätzlich gezeichnete Rahmen um die neun Pixel große Trefferfläche entfällt.
Die unabhängige Pixelprüfung verlangt die einzelne Linie ausdrücklich.

## Nachweise und gefundene Ursachen

Die [Validierung](timeline-2026-10-23/validation.json) ordnet jeden Lauf seinem
Quellenmanifest und tatsächlichen Binärhash zu. Die ursprünglichen App-Grenzen
bleiben 180 Sekunden für den neuen vollständigen Designablauf, 120 Sekunden
für den bestehenden Zeitleistenablauf und 45 Sekunden Außengrenze für die
kurze Kontextprobe. Es wurden keine bestehenden Zeitgrenzen erhöht.

Der Designablauf startet einen echten adaptiven Pendellauf, pausiert ihn und
vergleicht vor/nach den UI-Aufgaben sowohl Quelltext als auch die Bytes der
weiter geöffneten Messdatei. Ein unabhängig ausgewerteter Zeitvektor belegt,
dass die Maus den zeitlich nächsten Zustand wählt; die alte proportionale
Indexrechnung ergibt im gewählten Gegenfall einen anderen Index.
Die separate Kontextprobe ist ausdrücklich synthetisch und belegt keine Physik.

| Tatsächliche Ausführung | Umfang |
| --- | --- |
| Windows Release, Quelle 6 | 5/6 Gruppen: Inspektor, Grafikdialog, Transport, Kontext und Zeitleistendesign bestanden. Der bestehende Zeitleistentest überschritt nach spätem Hostfokus seine unveränderte Wiedergabegrenze. |
| Alpine-Dev Debug ASan/UBSan, Quelle 6 | 6/6 derselben Gruppen bestanden. |
| Windows Release, Quelle 7 | 4/4 UI-Gruppen: bestehende Zeitleiste, alle Teilfenster, Kontext und Design bestanden; separater Textfluss 1/1. |
| Linux, Quelle 7 | 4/4 derselben UI-Gruppen bestanden; separater Textfluss mit ASan/UBSan 1/1. |

Beide Schriftpfade sind eigene native App-Prozesse. Der Designablauf prüft
Dunkel/16, Hell/16, Dunkel/18, Hell/20, Hell/22 und Kontrast/22, einschließlich
1080×740, 95 px Hauptfensterzeitleiste, echtem Herausziehen, 480×160 und
900×250 sowie Rückkehr an den vorherigen Platz. Die Windows-Originalkonturen
für Abspielen, Live und die Spur bestehen mit unveränderter Ein-Pixel-Toleranz.
Das ist ein begrenzter Maßvergleich, keine vollständige Bildgleichheit.

In den Vorstufen wurden sowohl Produktfehler als auch Fehler der neuen
Testvorrichtung gefunden. Die [eingefrorenen Gegenproben](timeline-2026-10-23/verify-negative.py)
erhalten den verworfenen alten Snapshot, die alte Leerlaufzeit, den überdeckenden
Tooltip, zusätzliche Fokusrahmen und die vorher um zwei Pixel versetzte Spur.
Der SDL-Testfilter ließ zuerst den absichtlich erzeugten realen Fokusverlust
nicht durch; nur dieser eigene Hilfsfensterfall ist im Selftest jetzt freigegeben.
Der normale Produktpfad besaß diesen Testfilter nicht.

Unter Windows lehnte SDL das Lesen der noch schreibbar geöffneten Messdatei ab.
Die Vorrichtung liest dort über denselben C-stdio-Sharingvertrag wie der Runner;
Writerflags und Produkt-I/O wurden dafür nicht geändert. Ein weiterer früher
Fehler war die zu frühe Prüfung einer erst im nächsten UI-Frame wirksamen
Range-Aktion. Der Test wartet jetzt auf den beobachteten Zustandswechsel.
Eine Parserkorrektur trennt abschließend die Erfolgsmeldung von Geometriedatensätzen.

Der bestehende Zeitleistentest startet den Runner nun erst nach tatsächlichem
Hostfokus; sein eigener Prozess verwendet den dokumentierten SDL-Raise-Hinweis.
Er gibt weder Fokus vor noch verkürzt er die geprüfte Wiedergabe. Die vorher
während der Fokuswartezeit aufgezeichneten 62 Sekunden sind als fehlgeschlagener
Lauf erhalten. Die Quelle 7 korrigiert außerdem nur die zusätzliche generische
Range-Fokuszeichnung; andere Range-Aufrufer behalten den bisherigen Pfad.

Die [16 kanonischen Exporte](timeline-2026-10-23/exports.json) der Quelle 6 sind
gegenüber den vorherigen plattformspezifischen Nachweisen bytegleich. F1- und
Export-Schriftrollen sowie alle 18 Font-/Lizenzressourcen bleiben unverändert.
Die zwölf fremden lokalen Dateien sind separat mit Rohhash erfasst und nicht
Teil dieses Commits. Die Tests belegen den dokumentierten lokalen Gesamtstand;
die öffentliche Quellliste grenzt die 240 zugehörigen Dateien getrennt ab.

| Native Aufnahmen | Beispiel |
| --- | --- |
| Vorher | [Native Ausgangsaufnahme](timeline-2026-10-23/before-timeline.png) |
| Windows, dunkel | [Originalaufteilung](timeline-2026-10-23/windows7-dark.png) |
| Windows, große Schrift und Mindesthöhe | [Native Aufnahme](timeline-2026-10-23/windows7-minimum.png) |
| Windows, herausgezogen | [480×160](timeline-2026-10-23/windows7-detached-browse.png) |
| Linux, dunkel | [Native Aufnahme](timeline-2026-10-23/linux7-dark.png) |
| Linux, herausgezogen | [480×160](timeline-2026-10-23/linux7-detached-browse.png) |
| Linux, Kontrast/22 | [Native Aufnahme](timeline-2026-10-23/linux7-contrast.png) |

Die Ausgangsaufnahme ist verlustfrei vom nativen BMP in PNG konvertiert; die
dekodierten RGB-Pixel sind gleich. Die übrigen PNGs sind unveränderte native
Ausgaben. Es wurden keine Nachweisbilder retuschiert oder skaliert.

## Grenzen

Die native Plattformschrift und die Rasterung unterscheiden sich zwischen
Segoe UI, Linux-Systemschrift und mitgelieferten Noto-Schriften. Ein begrenzter
Maßvergleich beweist keine vollständige Pixelgleichheit der gesamten Anwendung.
macOS, ein physischer Monitorwechsel mit anderer DPI und praktische
Screenreader-Aufgaben sind durch diese Teilprüfung nicht nachgewiesen.
Die offenen Befunde an Szenenbaum, Inspektor und weiteren Oberflächen sowie
der ältere F1-Lifecycle-Prüfweg bleiben separat zu bearbeiten.
