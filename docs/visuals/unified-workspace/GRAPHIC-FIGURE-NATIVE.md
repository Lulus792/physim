# Grafikexport: gemeinsame Zeichenbeschreibung und SVG

Auf die [festgehaltenen Diagrammdaten](GRAPHIC-EXPORT-NATIVE.md) folgt eine
eigene unveränderliche Zeichenbeschreibung in `app/graphic_figure.c`.
Sie übernimmt Layout und Hell-/Dunkelpaletten der
[freigegebenen Vorlage](EXPORT-EXAMPLE.md). SVG verwendet bereits diese
Beschreibung. Der folgende [native Rasteradapter](GRAPHIC-RASTER-NATIVE.md)
ergänzt reale Schriftmetriken, vier PNG-Größen und eine RGB-Vorschautextur.
Sichtbare Vorschau und Dialog bleiben anzubinden; dies ist keine fertige
Export- oder Designabnahme. Die folgenden Tests behalten ihren eingefrorenen
Figurenstand; der Rasterbericht führt spätere Änderungen an Schriftmetadaten
und der dreistelligen Bereichsbeschriftung separat.

Die Basisfläche bleibt **1200 × 850**, die Diagrammfläche liegt bei
**116/230/1032/470**. Titel, Untertitel, Legende, fünf Y- und sechs X-Gitterlinien,
Einheiten, Referenz, Hintergrund und Fußzeile folgen den vorgegebenen Positionen.
Ganze und tatsächlich sichtbare X-/Y-Grenzen stammen aus der Datenkopie.
Der Export berechnet keine anderen Y-Grenzen aus einem Zeitfilter und verändert
weder Zoom noch Statistik. Jede Kurve behält ihre eigenen X-Werte.

Linien überbrücken keine Maskenlücken; einzelne gültige Punkte bleiben sichtbar.
Punktwolken und Histogramme erhalten eigene Zeichenformen. Die ausdrücklich
gewählte Referenz bleibt durchgezogen, auch an einer anderen Position als Index 0.
Vergleichslinien und Legendensymbole verwenden die Strichmuster der Vorlage.
Ihre Strichsegmente werden einmal berechnet und von allen Adaptern übernommen.
Achsenclip und Kurvenindex sind ausdrücklich enthalten; Bedienelemente und
Crosshair gehören nicht zum Bildinhalt.

Die Beschreibung besitzt auch ihre Texte. Nach dem Aufbau darf die ursprüngliche
Datenkopie freigegeben werden. Schriftmetriken werden nur während des Aufbaus
angefragt, um lange Texte einmal in die vorhandenen Breiten einzupassen.
Größe, Laufweite und gemessene Breite bleiben danach fest; SVG verwendet diese
Breite als `textLength`. Die Beschreibung behält keine Schrift-/Callbackzeiger.
Die Unit-Tests verwenden deterministische Testmetriken. Sie belegen keine reale
Schriftwahl, Glyphenrasterung, Textlesbarkeit oder Pixelgleichheit zur Vorlage.

## Ressourcen, Zahlen und Fehler

Fehlgeschlagene Konstruktionen erhalten vorherige Ausgaben und geben Kandidaten
frei. Ein abgebrochener Zeichenadapter liefert keinen Erfolg für sein halbes Bild.
Es gelten höchstens **262.144 Zeichenbefehle** und **32 MiB SVG** einschließlich
NUL-Abschluss. Ein kleineres Bytebudget ist wählbar. Sehr stark oszillierende
Vergleiche können wegen ihrer Strichsegmente das Befehlslimit erreichen und
liefern dann `PS_LIMIT` statt einer still verkürzten Grafik.

SVG wird als eigene UTF-8-Bytefolge mit XML-escaped Texten erzeugt. Skripte,
Bilder und externe Ressourcen werden nicht ausgegeben. SVG-Koordinaten haben
einen festen Dezimalpunkt ohne Änderung des Prozess-Locale. Achsentexte verwenden
Dezimalkommas, nötigenfalls wissenschaftliche Schreibweise und einen benannten
Offset für große gemeinsame Ursprünge. Finite Offscreenpunkte werden auch bei
überlaufender naiver Differenz `Wert − Minimum` korrekt geclippt. Nicht
darstellbare oder über eine Milliarde Diagrammbreiten entfernte normierte
Koordinaten liefern `PS_LIMIT`; dort wird keine verlässliche Präzision behauptet.

## Tatsächlich ausgeführte Prüfungen

Windows/MSVC/ASan und Alpine-Dev/GCC/ASan/UBSan/LSan bestehen jeweils **4/4 Gruppen**:
`graphic_figure`, `graphic_snapshot`, `report`, `series_mask`. Die neue Gruppe
prüft Maskenlücken, Referenz an Index 1, getrennte Zeitachsen, Strichlängen,
Punktwolken, Histogramme, beide Ausschnittsachsen, lange Texte, Paletten,
Extremwerte und Adapterabbruch. Alle beobachteten **3 Figuren- und 5 SVG-
Allokationsfehlerstellen** werden gezielt fehlgeschlagen ausgeführt. Byte- und
Befehlsbudgets werden mit echten dichten Kurven geprüft.

`tests/check_graphic_figure_svg.py` liest pro Plattform fünf tatsächlich vom
C-Test erzeugte SVG-Dateien als XML und prüft Referenzpositionen, Farben,
Texte, Lücken, Strichlängen, Referenz, Punkte/Balken und X-/Y-Ausschnitt.
Die fünf finalen Dateien stimmen zwischen Windows und Linux bytegenau überein.
Dies sind synthetische Berichtsdaten aus dem Kern, keine Experimentmessungen
und keine Browser- oder PNG-Rasterabnahme.

Lauf 1 scheitert beidseitig an einer ungültigen Testdatei: Der vorhandene
Berichtskern erlaubt Histogramme nur mit dimensionsloser Y-Achse. Lauf 2 besteht.
Lauf 3 erwartet bei neuen Ressourcenfällen fälschlich ein Überschreiten des
globalen SVG-Limits. Der frühe Testabbruch lässt 56.952.176 Bytes an zuvor
erfolgreichen Testobjekten offen; das ist kein fehlgeschlagener Konstruktorpfad.
Lauf 4 nutzt ein ausdrücklich kleineres Ausgabelimit, erreicht die Freigaben
und besteht auch LSan. Alle fehlgeschlagenen Belege bleiben erhalten.

Die abschließende Zahlenprüfung ergänzt in Lauf 5 eine gezielte Gegenprobe:
Benachbarte große X-Werte `1000000000000000.25` und `1000000000000000.375`
ergeben vor der Korrektur gleich gerundete Ticktexte. Beide Plattformen lehnen
diesen Stand tatsächlich mit **0/1** ab; Testobjekte werden vor der Prüfung
freigegeben. Die Schrittweite wird in Lauf 6 nach Abzug des gemeinsamen Offsets
berechnet. Alle sechs X-Beschriftungen sind danach unterscheidbar; die vier
finalen Gruppen bestehen wieder beidseitig. Die fünf SVG-Referenzausgaben
bleiben gegenüber Lauf 4 bytegleich.

[Quellen, Ausführungen und SVG-Dateien](graphic-figure-2026-10-21/validation.json).
Beide Apps lassen sich bauen, ihre Testkataloge bestehen. Die lokalen App-Builds
enthalten zehn separat erfasste Accessibility-Änderungen und sind kein Nachweis
eines sauberen Checkouts. Kein aktueller macOS-, Zwischenablage-, Dateidialog-,
Screenreader- oder vollständiger visueller Nachweis wird daraus abgeleitet.
