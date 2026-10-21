# F1 H: Paarabstände des Titels

Folgearbeit vom 21. Oktober 2026 auf Grundlage von `d8048a8` und dem
[Artikel-/Zeilenabgleich](ARTICLE-FLOW.md). Die unveränderte H-Referenz hatte
im gleichen Windows-Titel eine vier Pixel schmalere rechte Kante gezeigt.
Der [Gesamtauftrag](../unified-workspace/NATIVE-FIDELITY.md) bleibt unverändert.

## Belegte Ursache

Der bisherige Nuklear-Fonthandle summiert Glyphenbreiten, ohne Paarabstände
zu berücksichtigen. Zusätzlich überspringt der eingebettete stb-Leser
OpenType-Lookups vom Typ 9. Die tatsächlich geladene Windows-Schrift
`C:/Windows/Fonts/seguisb.ttf` legt ihre Paarabstände gerade in solchen
ExtensionPos-Tabellen ab, die PairPos vom Typ 2 umschließen.

Eine direkte Probe des bisherigen Lesers lieferte für
„Pendel und Vergleich von Integratoren“ deshalb **0 Pixel** Kerning.
Der isoliert um die Erweiterung korrigierte Leser findet sieben Paare:

| Paar | Abstand in Font-Einheiten |
| --- | ---: |
| Pe | −69 |
| Ve | −131 |
| rg | −18 |
| vo | −16 |
| te | −12 |
| to | −12 |
| re | −18 |

Das ergibt −276 Einheiten bei 2048 Einheiten pro em, entsprechend
**−3,5578125 logischen Pixeln bei 26,4 px em**. Eine unabhängige Probe mit der
bereits installierten HarfBuzz-Version 13.2.1 liefert denselben Wert.
Sie vergleicht `kern=0` und `kern=1` bei deaktivierten Ligaturen und gleicher
Datei, Textfolge, Glyphenanzahl und Schriftgröße. Sie verwendet den Ablauf
des [offiziellen HarfBuzz-Beispiels](https://harfbuzz.github.io/a-simple-shaping-example.html)
und liest die [tatsächlichen Glyphenpositionen](https://harfbuzz.github.io/harfbuzz-hb-buffer.html#hb-glyph-position-t).
Diese Probe ist keine neue Browseraufnahme und kein Beleg für allgemeine
OpenType-Gleichwertigkeit.

## Native Umsetzung

Der eingebettete Leser folgt jetzt einer ExtensionPos-Tabelle im Format 1,
wenn diese eine PairPos-Tabelle umschließt. Andere Erweiterungstypen bleiben
unberührt. Die lokale Änderung ist in
[third_party/README.md](../../../third_party/README.md) dokumentiert.

Die Überschriftenadapter für H1 und H2 verwenden die tatsächlichen Paarabstände
neben ihrer unveränderten CSS-Laufweite. Dieselben Werte gehen in volle
Textmessung, Zeilenumbruch, Glyphenabstände und native Textrechtecke ein.
Eine Paarverbindung über eine Stilspanne wird einmal berücksichtigt; vor einer
neuen Zeile und nach einer Formel wird keine Verbindung zur vorigen Zeile
beziehungsweise zur Formelquelle übernommen.

Die Primärschrift wird vor der Freigabe ihrer TTF-Daten in eine unabhängig
besessene Ressource kopiert. Es gibt je eine Ressource für die beiden
Überschriftenfamilien, unabhängig von der Anzahl ihrer gebackenen Größen.
Beim Anhängen weiterer Größen bleiben sie erhalten. Beim Ersetzen der Bank
werden neue Ressourcen erst nach erfolgreichem Atlasaufbau übernommen und die
alten anschließend freigegeben. Ein Fehler verwirft nur die neuen Ressourcen.
Schließen des Dokumentationsfensters gibt beide frei.

Die erste Umsetzung fragte dieselben Tabellen bei jedem Zeichen erneut ab.
Die finale Ressource hält dafür einen begrenzten Cache mit 512 Paaren, auch
für den Wert null. Er speichert ausschließlich Font-Einheiten, keine
größenabhängigen Ergebnisse. Schlüsselvergleich und Verdrängung erhalten
andere Paare; die Skalierung behält die bisherige Rechenreihenfolge. Die
zusätzliche Prüfung verdrängt Einträge mit fast 2000 weiteren Zeichen und
prüft anschließend unterschiedliche Paarwerte und eine andere Größe erneut.

Eine vorhandene direkte SDL-Fokusabfrage im allgemeinen Kombinationsfeld
verhinderte außerdem das Linken der isolierten Fonttests. Der gemeinsame
Accessibility-Hook zeichnet bereits den nativen Fokus. Die zusätzliche
Abfrage entfällt; der Backend-Code lässt sich wieder ohne SDL-Fokusverwaltung
testen. Tastaturmenü und die beiden echten Auswahlfeldabläufe prüfen diesen
Weg zusätzlich. Es wurden keine Test-Stubs eingeführt.

Die vorhandenen allgemeinen Nuklear-Fonthandles, Fließtext-, Code- und
Formelrenderer werden dadurch nicht auf einen neuen Shaping-Renderer umgestellt.
HarfBuzz ist ausschließlich ein unabhängiges Messwerkzeug in der vorhandenen
Linux-Umgebung; Physim erhält keine neue HarfBuzz-Laufzeitabhängigkeit.
Die [Probe](../../../tests/probe_font_shaping.py) ist im Repository enthalten.

## Prüfungen und Grenzen

Die zusätzliche `font_faces`-Prüfung erzeugt aus einem verfolgten Font eine
direkte und eine entsprechend umschlossene Klassentabelle mit derselben
vorgegebenen Paarverschiebung. Sie prüft gleiche Ergebnisse, doppelte
Skalierung, Nutzung nach Freigabe der Eingabedaten, fehlende Glyphen,
ungültige Größen/Indizes, Allokationsfehler und vollständige Freigabe.
Diese Fixture benötigt keine proprietäre Windows-Schrift.

Die H-Prüfung liest zusätzlich pro Aufnahme sowohl die volle Textbreite als
auch die Summe der tatsächlichen Glyphenabstände aus. Alle 56 Zustände müssen
übereinstimmen und die gespeicherte Fontressource verwenden. Vollständige
Titelbytes, Zeilenboxen, Seitengeometrie und die bisherigen Bedienprüfungen
bleiben erhalten. Der unabhängige Titelkanten-Prüfer behält seine Toleranz von
höchstens einem Pixel je Kante.

[Prüfdaten](kerning-2026-10-21/validation.json) halten die eingefrorenen
Quellstände 72/73, alle Ergebnisse, Programmhashes, Befehle und Bildherkunft
fest. Unverändert vorgefundene lokale Accessibility-Dateien sind separat
gehasht und gehören nicht zu dieser Änderung; die lokalen Builds sind deshalb
kein Nachweis eines sauberen Checkouts allein dieses Commits.

| Stand / Plattform | Tatsächlich ausgeführtes Ergebnis |
| --- | --- |
| 72 / Windows | 14/14 Fenster-/Grafikgruppen bestanden. Die separaten Fonttests bestehen 1/3; zwei Targets scheitern an der beschriebenen Link-Abhängigkeit, bevor ihre Prüfung beginnt. |
| 72 / Linux | 12/14 Fenster-/Grafikgruppen bestanden. Seiten und H erreichen ihre Bildmatrizen, laufen später jedoch in die innere 60-Sekunden-Grenze. |
| 73 / Windows | 3/3 Fonttests bestanden. Der breite UI-Lauf besteht 16/17; der dunkle Auswahlfeldtest wartet bereits in Stufe 0 auf Windows-Hostfokus. Die gezielte unveränderte Wiederholung besteht 1/1. |
| 73 / Linux | 3/3 Fonttests bestanden. Der breite UI-Lauf besteht 16/17; H erreicht Stufe 47 und läuft dort in dieselbe Zeitgrenze. Die anschließende isolierte unveränderte H-Prüfung besteht 1/1 vollständig. |

Damit sind auf beiden Plattformen **20 verschiedene Gruppen** über die
73er-Läufe positiv geprüft: drei Fonttests und 17 UI-/Grafikgruppen. Die
beiden breiten 17er-Läufe enthalten weiterhin jeweils einen dokumentierten
Fehler. Aus erfolgreichen Einzelwiederholungen wird keine allgemeine
Fokus- oder Timingstabilität abgeleitet. Keine Zeitgrenze wurde angehoben.
Der anfänglich wegen CRLF gescheiterte Linux-Hilfsskriptstart ist ebenfalls
getrennt erfasst; dabei war noch keine Anwendung gestartet.

Die [negative Gegenprobe](kerning-2026-10-21/counterexample-legacy-reader.json)
führt die neue Fontprüfung mit dem alten Tabellenleser aus und scheitert
genau am Unterschied zwischen direkter und erweiterter Paarverschiebung.
Der [direkte Sanitizer-Lauf](kerning-2026-10-21/direct-font-faces-72.json)
prüft zusätzlich ASan/UBSan/Leakprüfung; er ersetzt nicht die oben getrennt
nachgewiesene reguläre Target-Verlinkung.

## Bildvergleich und gemessene Abfragekosten

Die Windows-Titelkantenprüfung besteht jetzt in Hell und Dunkel:
`[281,144,717,169]` gegenüber `[281,143,717,168]` in Clientkoordinaten.
Die rechte Kante und Breite stimmen überein; die vertikalen Kanten unterscheiden
sich um einen Pixel. Die Toleranz bleibt unverändert. Das ist weder ein
Nachweis identischer Glyphenrasterung noch eine volle H-Abnahme.

Alle **112 vollständigen H-Bildpaare** aus 72 und 73 sind innerhalb derselben
Plattform pixelgleich: 56 Zustandspaare unter Windows und 56 unter Linux. Der Cache ändert
damit in dieser Matrix keine gezeichneten Pixel. Die Prüfdaten enthalten je
Aufnahme beide Ursprünge, Maße und vollständige RGB-Hashes; es wurden keine
Masken, Ausschnitte, Umfärbungen oder Skalierungen verwendet.

Die [feste AB/BA/AB-Abfragemessung](kerning-2026-10-21/paired-profile.json)
führt dieselbe Produktionsfunktion vor und nach dem Cache aus. Je Variante
entstehen drei Prozesse mit je sieben Batches zu 144.000 Abfragen. In dieser
Linux-Debug-/ASan-/UBSan-Probe sinkt der Median von 1,380639 auf 0,053955 Sekunden
je Batch. Die 144 gesondert gespeicherten Float-Ergebnisse sind in allen sechs
Prozessen vollständig bytegleich. [Profilierquelle](../../../tools/profile_font_pairs.c).
Das belegt nur diese wiederholte Abfragearbeit, keine allgemeine Beschleunigung
der Anwendung, des GPU-Renderings oder neuer, ständig wechselnder Texte.

Aktuelle native Beispiele:

- [Windows dunkel](kerning-2026-10-21/windows-h-dark.png),
  [hell](kerning-2026-10-21/windows-h-light.png),
  [große UI und Leseschrift](kerning-2026-10-21/windows-h-ui-22-reader-32.png).
- [Linux dunkel](kerning-2026-10-21/linux-h-dark.png),
  [Mindestfenster](kerning-2026-10-21/linux-h-minimum.png),
  [große UI und Leseschrift](kerning-2026-10-21/linux-h-ui-22-reader-32.png).
- [Windows-Formelsatz](kerning-2026-10-21/windows-math-32.png),
  [Linux-Formelsatz](kerning-2026-10-21/linux-math-32.png).

Die zwölf PNGs sind verlustfreie RGB-Konvertierungen der tatsächlichen
73er-BMPs. Die bisherigen Formel-/Codegrafik-, Glyphen-, Bytekopie-, Dichte- und
Fonttransaktionsprüfungen bleiben in den erfolgreichen Gruppen enthalten.

## Speicherpflege

Nach der aktuellen Gegenprüfung wurden 230 abgelöste automatische Bilder
aus acht erfolgreichen 72er-Gruppen entfernt: 553.837.008 Bytes, rund 554 MB.
[Dateiliste und Hashes](kerning-2026-10-21/cleanup.json). Dieselben Gruppen
sind im aktuellen 73er-Stand auf beiden Plattformen positiv geprüft.
Alle 112 vollständigen H-Vergleichspaare, Mathematik-/Inline-Inhalt-/Formelsatzbilder,
Fehlerbelege, aktuellen 73er-Ausgaben, Quellen, Programme, Debug-Symbole,
Nutzdaten, Abhängigkeiten und der gemeinsame WSL-Datenträger bleiben erhalten.
Es wurden nur erzeugte `docs-*.bmp`-/`docs-*.png`-Dateien direkt in den ersetzten
erfolgreichen Abläufen gelöscht.

## Linux-Schriftgrundlage

Die tatsächliche Linux-Primärschrift ist weiterhin
`/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf`. Dieselbe unabhängige
Messung liefert für den Titel mit Laufweite 557,55 px statt 440,95 px mit Segoe
UI Semibold. Der Titel muss daher im vorhandenen Artikelraum auf zwei Zeilen
umbrechen. Kerning allein beseitigt diesen Unterschied nicht.

Die derzeit gebündelte Noto Sans Bold ergibt 491,39 px und besitzt ebenfalls
andere Glyphenformen. Ein bloßer Wechsel zwischen diesen vorhandenen Fonts
wäre kein Nachweis identischer Typografie. Dafür brauchen Vorlage und Laufzeit
dieselbe verfügbare Schriftgrundlage. Die aktuelle Linux-Schriftwahl ist eine
konkrete offene Abweichung; sie erklärt keine anderen korrigierbaren
Layoutunterschiede.

Vollständige H-Abnahme, weitere Navigations-/Kontextdetails, dauerhafte Fokus-/Timingstabilität,
macOS, reale Monitor-/DPI-Wechsel und externe Assistive-Nachweise bleiben offen.
Auch die Artikelstruktur ist weiter anzugleichen: In `index.html` liegt die
Herkunftszeile innerhalb von `article-scroll`; nativ steht sie noch vor der
scrollenden Artikelgruppe. Seitliche Innenabstände und Scrollleisten sowie
diese gemeinsame Scrollbewegung benötigen einen eigenen nativen Abgleich.
Es ist keine technische Unmöglichkeit dieser Anpassungen belegt.
Entwickeln, Simulieren, Auswerten und der freigegebene Grafikexport aus ADR 010
bleiben Teil des priorisierten Designauftrags.
