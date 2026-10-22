# Native UI: Bruchteile bis zur Rasterung erhalten

Der gemeinsame Zeichenpfad erhält jetzt gebrochene Positionen, Größen,
Radien und Strichbreiten. Auch die von Nuklear zugewiesenen Widgetrechtecke
werden nicht mehr vorzeitig auf ganze logische Pixel gekürzt. Das betrifft
Entwickeln, Simulieren, Auswerten und F1; es ist keine vollständige Designabnahme.
Die [Nutzerbefunde](USER-REVIEW-2026-10-21.md) und der übrige
[Referenzabgleich](NATIVE-FIDELITY.md) bleiben verbindlich.

## Nachgewiesene Ursache und Änderung

Die tatsächlich kompilierte und ausgeführte Gegenprobe vor der Änderung speichert
eine gewünschte Linie von `(10,25; 20,75)` nach `(40,5; 20,75)` mit Breite
`1,5` als `(10; 20)` nach `(40; 20)` mit Breite `1`. Mehr Bildpixel oder eine
andere Schrift können diese schon verlorenen Bruchteile nicht wiederherstellen.
[Originaler negativer Lauf](float-ui-2026-10-23/before.log).

Die lokale Nuklear-Erweiterung `NK_INCLUDE_FLOAT_COMMANDS` verwendet Float-
Koordinaten und -Größen für sämtliche Zeichenkommandos und deren Punktlisten.
Die variablen Listen reservieren die tatsächliche Punktgröße. Ungültige Anzahlen
und Nullzeiger werden vor dem Kopieren abgewiesen. Die beiden produktiven
Custom-Callbacks verwenden dieselben Koordinatentypen. Ohne das optionale Flag
bleiben die bisherigen kurzen Kommandotypen und die Callback-Signatur erhalten;
eine separate Übersetzungseinheit prüft diesen unveränderten Standardmodus.

Die Prüfung verfolgt alle Kommandotypen, negative Koordinaten, Punktlisten,
Textinhalt, die Float-Glyphenposition und Custom-Callbacks durch die wirkliche
Vertexkonvertierung. Widgetmessung und zugewiesenes Rechteck stimmen überein.
Die bestehenden Prüfungen für große Vertexmengen, begrenzten Speicher,
fehlgeschlagene Allokation und Wiederverwendung bestehen ebenfalls.

Die physische Schere umschließt nun beide absoluten Clipkanten mit
`floor(min)` und `ceil(max)`. Die frühere Kombination aus abgerundetem Ursprung
und separat aufgerundeter Breite konnte die letzte Pixelspalte verlieren.
Die neue Berechnung begrenzt endliche Kanten vor der Integerkonvertierung am
Framebuffer und verwirft ungültige oder leere Rechtecke. Dies bleibt eine
ganzzahlige Hardware-Schere, keine fractional antialiaste Clipmaske.

## Unabhängige GPU- und Konturprüfung

Der GPU-Test zeichnet acht horizontale Linien in Achtelpixelschritten sowie
eine Linie mit negativem Anfang. 71 tatsächliche RGB-Proben werden mit dem
berechneten linearen AA-Profil einer 1,5- beziehungsweise 2,5-px-Linie verglichen.
Windows/NVIDIA und Linux/Mesa erreichen maximal **0,5 von 255 Kanalstufen**
Abweichung. Ein weiterer vollständiger Bildvergleich prüft die konservative
12 × 21-Pixel-Schere. Die Bilder sind direkte native RGB-PNGs.

Eine separat kompilierte Gegenprobe setzt ausschließlich die Linienemission
wieder auf `short`/`unsigned short`; Kommandostrukturen und Test bleiben Float.
Derselbe GPU-Test verwirft diese Variante.
[Mutation](float-ui-2026-10-23/negative-emission.patch) ·
[tatsächliches Ergebnis](float-ui-2026-10-23/negative.json).
Der erste Hilfsaufruf scheiterte vor dem Linken am Bibliotheksnamen und zählt
ausdrücklich nicht als GPU-Gegenbeleg.

Das erste native Dialogbild mit Float-Kommandos zeigte einen Folgefehler:
Die einpixelige Feldkontur verteilte sich auf zwei Zeilen. Beispielsweise lag
das Layoutfeld bei `y=395,679`; der alte Prüfer erwartete seine volle Farbe an
`floor(y)`. Die direkte Referenz zeigt dagegen eine scharfe Kontur an Zeile 476.
[Aufgezeichnete Pixel vor der Korrektur](float-ui-2026-10-23/counter-field-border.json).

Nur die gezeichneten Feld- und Formatkartenkonturen werden deshalb am physischen
Pixelraster ihres tatsächlichen Fensters ausgerichtet. Layout, Auswahlrechtecke,
Text, Radiosymbole und Figuren behalten Float-Koordinaten. Beim Review wurde die
vorübergehende gemeinsame Verwendung des ausgerichteten Kartenrechtecks für
Zeichnung und Auswahl wieder getrennt. Die finale Prüfung verlangt unvermischt
die sechs originalen Referenzfarben bei `(1100; 476/554/632)` in Hell und Dunkel;
sie senkt die Farbanforderung nicht auf eine Toleranz ab.

## Prüfstände und Grenzen

Basis ist `bcb95d10e8d6f986cbef8a8fec19bd1b3aa445e6`. Die internen Stände 1–4
sind keine Produktversionen. Quellhashes, Befehle, Ergebnisdateien und tatsächliche
Bildquellen werden im [Prüfmanifest](float-ui-2026-10-23/validation.json) getrennt.
Die Builds enthalten die dort byteweise abgegrenzte vorgefundene Accessibility-
Arbeit; dies ist kein isolierter Clean-Checkout-Nachweis.

Windows/MSVC/ASan, Windows Release sowie Alpine-Dev/GCC/ASan/UBSan mit X11 und
Mesa wurden tatsächlich ausgeführt. Die Prüfung des ursprünglichen Float-Pfads
umfasst jeweils zehn ausgewählte Geometrie-/Schriftgruppen unter Windows und
Linux. Der ergänzte Kommandotest sowie die neuen GPU-Proben wurden danach erneut
ausgeführt. Gemeinsamer Rahmen und Werkbanktrenner stimmen in den begrenzten
direkten PNG-Prüfungen für Hell und Dunkel mit der Originalreferenz überein.
Die vollständigen einzelnen Ergebnisse und deren Quellstände stehen im Manifest.

| Finaler Stand 4 | Tatsächlich ausgeführte Abschlussgruppen |
| --- | --- |
| Windows / MSVC / ASan | **1/1**: nativer Grafikdialog mit Eingaben, Vorschau, Kopie, Dateiausgabe, Größen und sechs Referenz-Konturproben. |
| Windows / Release | **2/2**: derselbe Dialog und großer schmaler Mehrlaufvergleich. |
| Alpine-Dev / GCC / ASan / UBSan | **2/2**: derselbe Dialog und großer schmaler Mehrlaufvergleich unter X11/Mesa. |

Die bereits geprüften gemeinsamen Float- und Schriftdateien blieben zwischen
den Ständen 2 und 4 unverändert. Stand 2 besteht unter Windows Release außerdem
**5/5** Gruppen für GPU-Geometrie, Materialien, Workspace-Baum, Szenenhierarchie
und Werkbank. Unter Linux bestehen in Stand 2 Werkbank, Messlabor, F1-Kontextcode
und dunkler Mehrlaufvergleich; nur der anschließend korrigierte Dialog scheitert.
Stand 3 besteht unter Linux **3/3** für Materialien, Dialog und dunklen Vergleich.
Diese getrennten Läufe werden nicht als ein erfundener gemeinsamer Gesamtlauf gezählt.

Alle **24** paarweise geprüften PNG-/SVG-Ausgaben der drei finalen Konfigurationen
sind bytegleich zu ihrem jeweiligen Plattform-Vorstand. Der erste Sammelaufruf
verglich irrtümlich auch Linux mit dem Windows-Schriftstand und brach ab; die
endgültige Gleichheitsprüfung benutzt ausdrücklich den passenden Plattformbeleg.

Native Prüfaufnahmen mit tatsächlichen Testdaten:

| Aufnahme | Dunkel | Hell |
| --- | --- | --- |
| Windows Release / Exportdialog, Stand 4 | [Bild](float-ui-2026-10-23/windows-release-export-dark-v4.png) | [Bild](float-ui-2026-10-23/windows-release-export-light-v4.png) |
| Linux / Exportdialog, Stand 4 | [Bild](float-ui-2026-10-23/linux-debug-export-dark-v4.png) | [Bild](float-ui-2026-10-23/linux-debug-export-light-v4.png) |
| Linux / Werkbank, Stand 2 | [Bild](float-ui-2026-10-23/linux-develop-dark.png) | [Bild](float-ui-2026-10-23/linux-develop-light.png) |

[Großer schmaler Vergleich unter Windows](float-ui-2026-10-23/windows-release-comparison-eight-v4.png) ·
[F1-Kontextcode unter Windows](float-ui-2026-10-23/windows-f1-code.png).

Ein früherer Windows-Debug-Werkbanklauf blieb ohne Hosttastaturfokus stehen und
zählt als fehlgeschlagen. Die Release-Wiederholung besteht unveränderte
Werkbankbedingungen. Die anfänglichen Dialogläufe beider Plattformen scheitern
am aufgezeichneten Konturfehler; auch sie bleiben als negative Ergebnisse erhalten.
Ein CR am Ende der ersten neuen Linux-Hilfsskripte verhinderte außerdem die
Auswahl der letzten Filtergruppe. Gezählt werden ausschließlich die tatsächlich
ausgeführten Gruppen; der abschließende Linux-Materiallauf verwendet ein LF-Skript.

Nicht belegt sind vollständige Browser-Pixelgleichheit, alle Fonts und Gewichte,
sämtliche Hover-/Fokus-/Scrollzustände, externe Screenreader, physische DPI- und
Monitorwechsel oder ein aktueller macOS-Build. Die mathematischen Scherenfälle
bei 1,25-/1,5-Skalierung ersetzen keinen physischen Monitorversuch. Die sichtbaren
Text- und Komponentenunterschiede der übrigen Oberfläche bleiben echte Arbeit;
sie werden nicht pauschal als native Unmöglichkeit eingeordnet.
