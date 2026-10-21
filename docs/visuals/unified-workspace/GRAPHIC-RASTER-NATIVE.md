# Grafikexport: native Rasterausgabe und Vorschautextur

Der Export besitzt jetzt einen nativen Rasteradapter für die
[gemeinsame Zeichenbeschreibung](GRAPHIC-FIGURE-NATIVE.md). Er erzeugt echte
PNG-Dateien und eine eigene Vorschautextur aus denselben RGB-Pixeln. Der sichtbare
Exportdialog, sein Mehrlaufcache, Dateiauswahl und OS-Bildkopie bleiben umzusetzen.
Dieser Baustein ist keine vollständige Abnahme von [ADR 010](../../decisions/010-analysis-graphic-export.md).

## Darstellung und Besitz

`app/graphic_raster.c` übernimmt eine unveränderliche Diagrammkopie und erstellt
die Figur mit echten Schriftmetriken. Alle vier Exportmaße werden unabhängig
vom SDL-Fenster erzeugt: **1200 × 850, 2400 × 1700, 3600 × 2550 und 4800 × 3400**.
Schrift wird dafür in der jeweiligen Ausgabedichte neu gerastert. Ein vergrößertes
Fenster oder hochskaliertes Bildschirmfoto ersetzt diese Ausgabe nicht.

Ein eigener Bedarfsatlas enthält die tatsächlich benötigten Zeichen in regulärer
und hervorgehobener Schrift. CSS-em-Größe, echte Paarabstände und Laufweite
bestimmen die Figur. Geprüfte Schriftquellen sind Segoe UI unter Windows,
DejaVu Sans unter Alpine-Dev und die mitgelieferten Noto-Schriften auf beiden
Systemen. Fehlende Glyphen liefern einen Fehler. Höchstens 4096 benötigte Zeichen
und 128 MiB für den temporären Schrift-/Zeichenbereich sind erlaubt; zusätzlich
gelten die tatsächlichen GPU-Texturgrenzen und bestehenden Figurenlimits.

Nach Erzeugung sind Atlas, Schriftquellen und Messcallbacks freigegeben. Figur
und RGB-Bild besitzen ihre Inhalte. Das Bild lässt sich danach unabhängig von
den Quelldaten speichern. Seine bei Bedarf hochgeladene RGB-Textur gehört dem
Rasterobjekt und wird vor dem zugehörigen Grafikkontext freigegeben. GPU-Aufrufe
bleiben an dessen aktuellen Thread/Kontext gebunden. PNG-Encoding kann später
mit gesichertem RGB-Besitz getrennt ausgeführt werden; ein solcher Worker ist
hier noch nicht angebunden.

`app/graphics.c` stellt dafür eigene RGB-Bilder mit oberster Bildzeile zuerst
bereit. Offscreen-Framebuffer, Lese-/Zeichenbindung, Viewport, Pack-Ausrichtung
und aktive Textur werden am Übergang behandelt. Dies ist kein allgemeiner
Snapshot sämtlicher OpenGL-Zustände. Fehlgeschlagene Konstruktionen erhalten
das vorherige Ausgabeobjekt. Ein fehlgeschlagener Vorschautextur-Upload nimmt
dem bereits erzeugten RGB-Bild seine PNG-/SVG-Ausgabe nicht weg.

## Bildprüfung

Pro Plattform bestehen **24 tatsächliche PNG-/SVG-Fälle**: zwei Schriftanbieter
mit je Hell/Dunkel in allen vier Größen, X-/Y-Ausschnitt in beiden Themes sowie
Punkte, Histogramme und ein isolierter gültiger Linienpunkt. Die Daten sind echte
synthetische Kernberichte mit getrennten Zeitachsen und Statuslücken, keine
Experimentmessungen. Die native Rasterprobe verwendet zwei Vergleichskurven und
einen gesonderten Bericht mit drei Kurvenarten. Acht gleichzeitig gerasterte
Läufe und deren zweite Legendenzeile sind damit noch nicht visuell abgenommen.

Der unabhängige Python-Prüfer decodiert PNG-Chunks einschließlich CRC, Maße und
vollständiger RGB-Daten. Er vergleicht diese mit der tatsächlichen C-Ausgabe und
prüft Papierfarbe, Texttinte, Maskenlücken, Ausschnitt, Kurvenformen und Clipping.
Die tatsächlichen SDL-Fenster wechseln zwischen 760 × 540 und 1600 × 900; die
Ausgabemaße bleiben fest. Pro Schriftanbieter wird die native RGB-Textur erneut
gerendert und ihr vollständiges Bild bytegenau mit dem ursprünglichen RGB
verglichen. Das belegt **zwei Vorschautextur-Rundläufe je Plattform**, noch keinen
sichtbaren Exportdialog oder eine OS-Zwischenablage.

| Beispiel | Native Aufnahme |
| --- | --- |
| Windows, Segoe UI, hell, Standardgröße | [2400 × 1700](graphic-raster-2026-10-21/windows-native-platform-full-light-2.png) |
| Windows, Segoe UI, dunkler X-/Y-Ausschnitt | [2400 × 1700](graphic-raster-2026-10-21/windows-native-platform-crop-dark-2.png) |
| Windows, größte Ausgabe | [4800 × 3400](graphic-raster-2026-10-21/windows-native-platform-full-light-4.png) |
| Linux, Noto offline, Punkte und Häufigkeiten | [1200 × 850](graphic-raster-2026-10-21/linux-native-bundled-marks-light-1.png) |

Diese Bilder wurden zusätzlich visuell gelesen: Titel, Legende, Achsen und
Fußzeile bleiben getrennt, der Ausschnitt erhält beide erfassten Achsen, die
Maskenlücke bleibt offen. Die nativen Aufnahmen sind keine Browser-Pixelabnahme.

### Erkannter Geometriefehler und Gegenprobe

Der erste Adapter verwendete normale Nuklear-Zeichenbefehle. Deren ganzzahlige
Koordinaten und Strichbreiten verschoben die Kurven und kürzten 2,5 px auf 2 px.
Der aktuelle Adapter zeichnet über die vorhandene native Float-Zeichenliste.
Er darf bei erneutem Geometrieaufbau keine Textslots aus dem ersten Versuch
weiterzählen. Die Bibliothek selbst wurde dafür nicht verändert.

Ein neuer Prüfer misst ausgewählte beinahe horizontale blaue Kurvenabschnitte
gegen ihre SVG-Koordinaten. Die aufgezeichneten alten nativen PNGs werden bei
erneuter Auswertung tatsächlich verworfen: maximal **0,934 px** Abweichung und
etwa 2 px Deckung. Es wurde dafür kein alter Programmstand erneut gestartet.
Der finale Float-Pfad bleibt an diesen begrenzten Proben unter **0,127 px**,
mit ungefähr 2,63 px mittlerer Deckung einschließlich Kantenglättung.
[Alte Bilder und tatsächliche Gegenprüfung](graphic-raster-2026-10-21/counter-check.json).

Die Kurven bestehen weiterhin aus einzelnen Segmenten. Kantenglättung und
Überdeckung an deren Übergängen können vom SVG-Pfad der Vorlage abweichen.
Die genannte Probe belegt weder alle Steigungen noch ganze Pixelbilder.
SVG hält jetzt die tatsächliche primäre Schriftfamilie fest, bettet jedoch
keine Schriftdateien ein. Externe Betrachter mit anderer oder fehlender Schrift
können daher abweichen; PNG enthält seine gerasterten Glyphen vollständig.
Diese Unterschiede sind offene Angleichungsarbeit, keine behauptete technische
Unmöglichkeit einer genaueren Umsetzung.

## PNG-Zeit und Regressionen

Eine native Windows-Plotprüfung überschritt mit dem bisherigen DEFLATE-Level 6
ihre unveränderte 75-s-Grenze. Die gemessenen PNG-Phasen waren lang, während der
neue reine Schrift-/Rasteraufbau deutlich kürzer blieb. Der PNG-Schreiber nutzt
jetzt Level 1; Format, Filter, exklusive Neuanlage und decodierte RGB-Pixel bleiben
erhalten. Das verändert Dateigröße und Schreibzeit, nicht die Bildqualität.

`tools/profile_png.py` vergleicht den echten Produktionsencoder in einem Prozess
mit drei festen AB/BA/AB-Paaren. Beide Betriebssysteme erhalten dieselben bereits
nativ erzeugten 2400 × 1700 RGB-Eingaben. Alle zwölf Dateien pro Plattform werden
unabhängig decodiert und vollständig mit den jeweiligen Eingangspixeln verglichen.
Gemessen wird Encoding plus neue Datei schreiben/schließen; kein dauerhafter
Datenträger-Flush, keine Release- oder gesamte UI-Latenz wird daraus abgeleitet.

| Debug mit Sanitizern, Median aus drei Paaren | Level 6 | Level 1 |
| --- | ---: | ---: |
| Windows, hell | 2,570 s | 0,439 s |
| Windows, dunkel | 2,589 s | 0,531 s |
| Linux, hell | 1,464 s | 0,426 s |
| Linux, dunkel | 1,282 s | 0,371 s |

Die hellen Dateien wachsen von 245.910 auf 301.537 Bytes, die dunklen von 243.524
auf 299.799 Bytes. Dies ist eine lokale Messung mit zlib 1.3.2 und diesen Bildern.
[Windows-Rohwerte](graphic-raster-2026-10-21/windows-png-profile.json) ·
[Linux-Rohwerte](graphic-raster-2026-10-21/linux-png-profile.json).
Die gesamte finale Windows-Plotgruppe besteht in 30,063 s; ihr Timeout wurde
nicht vergrößert. Dies ersetzt keinen gepaarten Gesamt-App-Vergleich.

Weitere negative Läufe bleiben erhalten: Eine neue Nullpixel-Prüfung hatte
die bestehende leere Szenentextur-Allokation verhindert. Diese erlaubte Verwendung
ist wiederhergestellt; die echte Grafikregression besteht auf beiden Systemen.
Die Linux-Plotprobe verwendete nach einer asynchronen Testfensterverkleinerung
zunächst alte Layoutmaße. Das Testfenster wird vor dem ersten Layout synchronisiert.
Die normale App erhält dadurch keine neue Fensteranordnung. Im ersten
Rasterlauf wurde derselbe Größenübergang erst nachträglich abgefragt; auch diese
Probe wartet jetzt auf die tatsächliche Fenstergröße. Ein zunächst falscher
Library-Dateiname im Benchmarkstarter ist separat als Harnessfehler festgehalten.

## Abschließender Prüfstand und verbleibende Arbeit

Quellstand **graphic-raster-4**, Basis `0cacb1e`. Windows/MSVC/ASan und
Alpine-Dev/GCC/ASan/UBSan bestehen je **4/4 native Gruppen** (`plot_workflow`,
`graphics`, `series_mask_workflow`, `graphic_raster`) und **5/5 Kerngruppen**
(`graphic_figure`, `graphic_snapshot`, `report`, `series_mask`, `png`).
Die zuvor in Stand 2 ausgeführten Testkataloge behalten unveränderte
Katalogregistrierungen. 119 Quellen, 14 Schriftdateien und 20 Umbruchdateien sind
erfasst. Zehn fremde Accessibility-Änderungen bleiben über alle vier Stände
separat unverändert; die lokalen Apps enthalten sie. Kein sauberer Checkout nur
dieses Patches und keine aktuelle macOS-Ausführung werden behauptet.

[Ausführungen, Quellen, Fehlerläufe und Bildhashes](graphic-raster-2026-10-21/validation.json) ·
[Abschlussaudit](graphic-raster-2026-10-21/audit.json).

Weiter beauftragt bleiben die sichtbare Mehrlaufdatenbasis, der freigegebene
Dialog mit seinen Zugängen und Optionen, eine unveränderte offene Exportansicht,
nativer Speicherdialog einschließlich Abbruch/Überschreiben/Fehler und tatsächliche
OS-Bildkopie. Dazu gehören Tastatur, Fokuskehr zum auslösenden Fenster, große
Schrift, hoher Kontrast, Resize/DPI und die vollständige visuelle Abnahme.
Der aktuelle Rasteradapter kennt Hell/Dunkel; ein eigener Hochkontrastvertrag
für „Wie in der Auswertung“ ist noch nicht angebunden. Die vorhandene Oberfläche
verwendet weiterhin ihre bisherigen Exportaktionen. Diese gesamte Restliste
wird nicht durch einen bestandenen Bildbaustein erledigt.
