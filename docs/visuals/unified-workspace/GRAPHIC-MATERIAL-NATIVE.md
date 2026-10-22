# Exportdialog: Hintergrundmaterial und skalierte Felder

Der native Berichtsdialog besitzt jetzt den Hintergrund-Blur der freigegebenen
Vorlage. Der vorhandene Suchfilter bleibt ein eigenes Glasmaterial mit Tönung;
der neue Modus filtert ausschließlich die bereits gezeichnete Oberfläche.
Die Abdunklung bleibt die vorgegebene Farbe `#080c147d`. Der Dialog, seine Schrift
und die eigentliche Exportgrafik liegen darüber und werden nicht weichgezeichnet.

Die Referenz verlangt `blur(3px)`. Nach dem gelesenen
[Filter-Effects-Vertrag](https://drafts.csswg.org/filter-effects-1/#funcdef-filter-blur)
ist das eine Gauß-Standardabweichung. Der native Filter verwendet eine normierte,
separable Gaußverteilung mit Sigma 3 in logischen Pixeln und endlicher Stütze
von ±9. Er kopiert den Hintergrund ausschließlich auf der GPU und liest und
schreibt in getrennte Attachments. Ein anschließender gewöhnlicher UI-Batch
verwendet wieder das normale Programm. Pro aktivem Blur fallen zwei Filterpässe
an; geschlossen oder bei hohem Kontrast/reduzierter Transparenz keine.

Für den Hintergrundmodus entfällt die bisherige Rundung des Suchmaterials.
Ist Filtern nicht verfügbar oder ausdrücklich reduziert, bleibt die tatsächliche
Oberfläche erhalten und wird nur abgedunkelt. Es wird keine einfarbige Ersatzfläche
über das ganze Fenster gezeichnet. Der Quellrenderer verliert seinen Materialzeiger
vor dem Freigeben des Dialogs.

Die drei Auswahlfelder besitzen außerdem einen explizit skalierten Zeichenpfad:
Chevron, Linienstärke, Textinsets und Menüabstand folgen der Dialogschriftgröße.
Der bisherige unskalierte benannte Auswahlpfad behält seine Geometrie für andere
Aufrufer. Zwei bestehende Glasinitialisierungen nennen den neuen Modus explizit,
damit dafür keine zusätzlichen Compilerwarnungen entstehen.

## Tatsächliche Prüfung

[Prüfmanifest](graphic-material-2026-10-22/validation.json), native Hell-/Dunkel-/
Kontrast-/Schriftaufnahmen und GPU-Testbilder halten den jeweiligen Quellstand fest.
Der GPU-Prüfer verwendet eine unabhängige Gaußberechnung aus der Exponentialfunktion,
nicht die Shaderkoeffizienten: jeweils 21 Positionen an einer roten und grünen
Stufenkante, eine unveränderte blaue Komponente, die Fensterecke, der ungefilterte
Fallback und nachfolgende normale rote Geometrie. Die bestehenden Glas-/Fallback-/
Schließen-/Wiederöffnen- und 2×/3×-Exportprüfungen bleiben erhalten.

Die Dialogfolge prüft zusätzlich tatsächlich zwei Hintergrundpässe und deren
Abschalten im Kontrastmodus. PNGs, Ausschnitte und unabhängige OS-Bildkopie laufen
weiter durch dieselbe native Prüfung. Die Ausgabe wird gegen den vorherigen
eingefrorenen Berichtsdialogstand verglichen; ein UI-Blur darf die Exportpixel
nicht verändern.

Finaler Quellstand **export-material-source2**, Basis `e6483dc`:

| Ausführung | Ergebnis |
| --- | --- |
| Windows / MSVC / ASan | `backdrop_ui`, `graphic_dialog` 2/2; Dialog 31,266 s |
| Windows Release | Dieselben Gruppen 2/2; Dialog 6,641 s |
| Alpine-Dev / GCC / ASan+UBSan / X11+Mesa | Dieselben Gruppen 2/2; Dialog 56,294 s |

Alle 24 geprüften PNG-/SVG-/Clipboard-Referenzdateien aus diesen drei Läufen
sind bytegleich mit den entsprechenden eingefrorenen Ausgaben des vorherigen
Dialogs. Der neue Hintergrundfilter beeinflusst damit diese tatsächlichen
Exportfälle nicht. Vierzehn Font-/Lizenzdateien besitzen unveränderte Hashes.
Die zwölf separat bearbeiteten lokalen Dateien bleiben erhalten und gehören
nicht zu diesem Patch. Die App-Builds enthalten ihre lokalen Accessibility-Teile;
ein isolierter sauberer Checkout wird nicht behauptet.

[Vorher ohne Blur](graphic-dialog-2026-10-22/windows-debug-plot-export-dialog.png) ·
[Aktueller nativer Dialog](graphic-material-2026-10-22/windows-debug-plot-export-dialog.png) ·
[Helle Ansicht](graphic-material-2026-10-22/linux-debug-graphic-dialog-light.png) ·
[Doppelte Dialoggeometrie mit skalierten Feldern](graphic-material-2026-10-22/linux-debug-graphic-dialog-200pct-footer.png).

## Noch keine vollständige Designabnahme

Die browserseitige und native endliche Filterapproximation können einzelne Pixel
unterschiedlich rastern. Der geprüfte Sigma-/Farbvertrag ist kein Nachweis einer
vollständigen Bitmapgleichheit mit einem Browser. Insbesondere bleiben der weiche
Dialogschatten (`0 28px 100px`) und der Papierschatten der Vorlage weiter anzugleichen.
Schriftgewicht, Hoverdetails und die übrigen gemeinsamen Komponenten benötigen
weiter ihren vollständigen Referenzabgleich.

Der sichtbare Mehrlaufcache mit gemeinsamer Kurve, Referenz, Legende, Statistik
und Export ist weiterhin beauftragt. Auch externe Screenreader, native
Dateiauswahlbedienung, physische DPI-/Monitorwechsel, ausgehängter Quellbereich
und macOS bleiben eigene offene Abnahmen. Die 2×/3×-GPU-Exporte des Glasmaterials
und doppelte Dialoggeometrie ersetzen keinen physischen Monitorfall. Testlaufzeit
ist ebenfalls kein gemessenes interaktives Framezeitbudget.
