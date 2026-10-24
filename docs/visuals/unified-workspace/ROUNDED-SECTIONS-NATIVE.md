# Gemeinsame Rundungen und vollständige Inspektortitel

Fortsetzung von `a46f301fe8b45a06b427741fda02870c000457dc` nach den tatsächlichen
[großen Aufnahmen](chart-captions-2026-10-24/README.md#ergebnis-und-weiterer-sichtbefund).
Der gesamte Oberflächenauftrag bleibt offen.

## Korrektur

Der gemeinsame Nuklear-Zeichenpfad begrenzte Rundungen auf die volle Kantenlänge.
Bei großen Kapselradien kreuzten sich die Eckbereiche; Füllung und Kontur konnten
Zacken bilden. Auch gültige Radien verwendeten nur drei Segmente pro Viertelkreis.
Der Pfad begrenzt jetzt auf die halbe kürzere Kante und normalisiert negative
Radien auf null. Die Unterteilung wächst begrenzt mit dem Radius. Exakte
Kardinalpunkte und entfernte doppelte Übergangspunkte verhindern kleine rückwärts
laufende Kanten an zusammenfallenden Ecken. Zeichenkommandos, Schriftmetriken,
Clipping und Glyphenrenderer erhalten ihre bisherigen Verträge.

Das betrifft gemeinsame gerundete Flächen und Konturen auch außerhalb des
Messlabors. Es handelt sich um eine Korrektur des vorhandenen nativen Renderers;
ein WebView oder neue Gestaltung wurde dafür nicht eingeführt. Die Referenz
bleiben die vorhandenen Radius-/Konturrollen aus den HTML-/CSS-Entwürfen.

Im Inspektor verdrängte die rechte Zusatzangabe Teile von Wörtern in der linken
Überschrift. Passt ein ganzes Wort nicht mehr neben die Zusatzangabe, steht
letztere jetzt rechts unter dem Titel. Die Mindestbreite berücksichtigt die
längste tatsächliche Titelvokabel, die aktuelle Schrift und die vorhandenen
Einzüge. Die gespeicherte Wunschbreite bleibt erhalten. Bei Schriftgröße 22
werden in den geprüften Linux-Schriften statt des Wunsches 208 px mindestens
245 px mit Systemschrift beziehungsweise 226 px mit Noto benötigt. Bei normaler
Schrift bleiben Mindestbreite 208 px und die übliche Breite 320 px bestehen.
Die native Trennersemantik veröffentlicht dasselbe tatsächliche Minimum.

Diese Anpassung folgt dem Vorrang tatsächlicher Schrift-/Mindestmaße aus
[RESIZING.md](RESIZING.md). Vollständige Wörter und erreichbare Abschnitte gehen
bei großer Schrift vor dem festen Demomaß. Dies ist keine technische
Unmöglichkeit, sondern eine notwendige Größenanpassung für unverkleinerte Schrift.

## Prüfung

Die [eingefrorenen Belege](rounding-headings-2026-10-24/README.md) unterscheiden
originale Gegenfälle, einen verworfenen Zwischenstand und finale Ergebnisse.
Der alte Pfad besteht nur 5/30 unabhängige Geometriefälle; die native GPU-Aufnahme
zeigt dieselben falschen Formen. Der erste Reparaturstand besteht 20/30: Seine
separat angenäherten Bogenenden erzeugen noch rückwärts laufende Kanten. Der
finale Stand besteht 30/30 unter Windows und Linux. Die unabhängige Distanzprüfung
verlangt höchstens 0,13 logische Pixel Fehler an Punkten und Segmentmitten.
Tatsächliche GPU-Innen-/Außenpixel bestehen bei 1× und 2× unter beiden Systemen.
Das ist keine vollständige Browser-AA- oder physische DPI-Abnahme.

Die Titelprüfung verwirft im unveränderten Produkt schon „Modellwer“ / „te“ bei
Schriftgröße 18. Der finale native Ablauf erreicht durch echte Scrollereignisse
alle neun Überschriften in sechs Darstellungen und beiden Schriftquellen. Er
prüft tatsächliche Zeichenkommandos, vollständige Wörter, sichtbare Zeilen,
Tastatur-/Mauseingaben, Modellwertkarten und unveränderte reale Versuchsdaten.
Kamera, Messbereich, Zeitleiste, Docking und externe Teilfenster bestehen ebenfalls.

Insgesamt bestehen 13 unterschiedliche Linux-Gruppen mit ASan/UBSan sowie vier
Windows-Gruppen: Geometrie, gerundete Pfade, Subpixel und tatsächlicher GPU-Readback.
Die zusätzliche Katalog-/Runnerprüfung besteht unter Windows und Linux. Sie
fand zunächst veraltete Zuordnungen bereits vorhandener spezialisierter
Bildprüfer; deren explizite Liste und die GPU-Kategorie sind jetzt vollständig.
Unbekannte Fälle bleiben Fehler. Diese Testdatei gehört nicht zum SDK-Kompiliergraphen.

Der erste breite F1-Lauf auf `/mnt/d` erreicht alle 56 Layoutzustände, überschreitet
danach jedoch die vorhandene 60-s-Appgrenze. Der unveränderte ursprüngliche
Prüfer besteht anschließend vollständig mit ext4-Arbeitsdaten und demselben
SDK. Zeitgrenzen und Erwartungen bleiben unverändert. Der erste Lauf wird
weiter als fehlgeschlagen geführt; eine allgemeine Lösung seiner Zeitüberschreitung
oder ein neuer Windows-F1-Bediennachweis wird daraus nicht abgeleitet.
Werkbank und Grafikdialog bestehen bereits im ersten breiten Nachlauf.

Die finale SDK-Quelle ist Stand 4; schreibgeschützte Nachläufe prüfen unveränderte
Quell-, Font- und Binärhashes. Drei fremde AT-SPI-Dateien bleiben im lokalen
Kompiliergraphen enthalten. Alle zwölf fremden Änderungen bleiben erhalten und
werden nicht mit eingecheckt; dies ist kein Clean-Checkout-Nachweis.
macOS, praktische Screenreader-Aufgaben, physische DPI-/Monitorwechsel und die
vollständige Gestaltung einschließlich aller F1-Inhalte bleiben offen.

## Speicherpflege

Nach vollständigem Byte-/Hashvergleich wurden 72 Dateien der doppelten privaten
ext4-Arbeitskopie entfernt: 176.448.473 Byte (rund 168 MiB). Das vollständige
identische Archiv bleibt unter `build/native/rounded-sections-h-ext4` erhalten,
ebenso alle ausgewählten und eingefrorenen Nachweise. Der WSL-Datenträger bleibt
erhalten; er wurde nicht kompaktiert. Die Zahl beschreibt entfernte
Dateibytes im Linux-Dateisystem, keine behauptete Verkleinerung der VHDX unter Windows.
