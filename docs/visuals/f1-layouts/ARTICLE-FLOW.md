# F1 H: Zeilenboxen, Artikelabstände und Textrollen

Folgearbeit vom 21. Oktober 2026 auf Grundlage von `4ca5018` und dem
[Schriftrollen-Abgleich](TYPOGRAPHY.md). Die unveränderten H-Bilder sowie
`layouts.css`, `refinements.css` und `final-h.css` bleiben die Referenz.
Dieser Schritt ist keine vollständige H- oder Gesamtoberflächenabnahme.
Der [gesamte Designauftrag](../unified-workspace/NATIVE-FIDELITY.md) bleibt offen.

## Native Änderung

Der Artikel verwendet die Zeilenboxen der Vorlage anstelle der bisherigen
Schrifthöhe plus sechs Pixel. Die erste Textpassage nach der Hauptüberschrift
bekommt die eigene Einleitungsrolle; der tatsächliche Handbuchtext bleibt erhalten.
Tabellen haben eigene normale, fette und Code-Schriftrollen. Die Übersicht nennt
logische Pixel bei Leseschrift 16; die Schriftanteile folgen dem Lesezoom.

| Artikelteil | Übertragener Vertrag |
| --- | --- |
| Fließtext | 16 px em, Zeilenhöhe 1,65 |
| Einleitung | 0,94 em, Zeilenhöhe 1,7; unterer Absatzabstand 27 px |
| Hauptüberschrift | 1,65 em, Zeilenhöhe 1,2; bei Leseschrift 32 die große H-Rolle mit 1,3 em; Laufweite −0,55 px; danach 20 px |
| Zweite Überschrift | 1,25 em, Zeilenhöhe 1,4; Laufweite −0,2 px; Abstände 27 / 13 px |
| Dritte Überschrift | 1,05 em, Zeilenhöhe 1,65; Abstände 24 / 12 px |
| Tabellen | 0,8125 em, Zeilenhöhe 1,5; echte fette Kopfzeile, Zellabstände 8 px horizontal und 12 px vertikal |
| Codeblöcke | 0,8 der Codegröße, Zeilenhöhe 1,7; gerader Rahmen, Innenabstände entsprechend 14 / 17 px plus Rahmen |
| Artikelherkunft | 0,7 em, Zeilenhöhe 1,65; gleiche linke Textkante wie der Titel |

Benachbarte obere und untere Blockabstände werden wie zusammenfallende
CSS-Ränder über ihr Maximum bestimmt. Die bisherige pauschale Blockpolsterung
entfällt. Codekopf und Kopierknopf haben natürliche, getrennte Breiten; dadurch
bleibt die Beschriftung bei großer Schrift zugänglich. Zusätzliche
Inline-Code-Kapseln entfallen entsprechend der Referenz.

Die Laufweite wird an persistenten nativen Fonthandles sowohl für Messung als
auch Zeichnung angewendet. Die vollständigen UTF-8-Zeichenbefehle bleiben
erhalten; Zeichen mit nachfolgender Nullbreitenmarkierung werden beim Umbruch
zusammengehalten. Paar-Kerning ist damit noch nicht umgesetzt.

Formeln verwenden weiterhin ausdrücklich die gewählte Leseschriftgröße und
Pixeldichte. Eine kleinere Einleitungs- oder Tabellenrolle verkleinert keine
TeX-Textur versehentlich. Ihre tatsächliche Ober-/Unterlänge darf die Zeilenbox
vergrößern. Die vorhandenen Formelquellen und Assets sind unverändert.
Unicode-Codegrafiken behalten den zuvor geprüften ganzzahligen Glyphenabstand
mit einer logischen Randzeile Überlappung.

Die schmale H-Geometrie beginnt jetzt entsprechend der Vorlage bei **900 px
einschließlich**, statt zuvor unter 1000 px. Seitenbreiten sind 220 px bis 900,
danach 250 px. Der rechte Innenabstand und die Modellabbildung wechseln an
derselben Grenze. Der neue native 900-/901-Test erhält Lesestelle, Suche,
Sprache, Kontexttab, Quellbytes und Simulationszeit.

## Fehlerursache und Korrekturen

Der erste neue Artikelstand 69 ließ die Inline-Linkprüfung nicht abschließen.
Bei großer Schrift lagen Fokus- und Clipgrenzen auf Bruchteilen eines Pixels,
der Scrollwert ist aber ganzzahlig. Beispielsweise blieb die Windows-Prüfung
mit Scrollwert 379 und Fokusoberkante 190,008 in der Sichtbarmachung hängen.
Dieselbe Ursache trat unter Linux auf. Die historischen Fehler bleiben in den
[Prüfdaten](article-flow-2026-10-21/validation.json) erhalten.

Stand 70 verwendete für die Scrollentscheidung bereits die tatsächlichen
ganzzahligen Zeichenkoordinaten. Der Link wurde erreicht; die anschließende
unveränderte Begrenzungsprüfung verglich jedoch noch ungerundete gespeicherte
Rechtecke. Stand 71 speichert auch Fokus und Artikelviewport in den tatsächlich
gezeichneten Koordinaten. Die ursprünglichen Anforderungen bleiben erhalten:
der Fokus ist nicht höher als der Viewport, beginnt innerhalb dessen Oberkante,
und der volle Link ist nachweislich höher als der Viewport.

Der finale Windows-Nachweis meldet Fokus `y=189, h=311`, Viewport `y=189, h=318`
und eine volle Linkhöhe von 613,632 px. Mausaktionen auf Codeköpfen und Fokuszielen
werden außerdem auf den sichtbaren Inhalt und aktive Fenster beschränkt.
Ein nicht sichtbarer Codekopf veröffentlicht keine sichtbare Kopieraktion.

Unter Linux lief Stand 69 bei Formel 33 von 36 in die innere 60-Sekunden-Grenze,
während alle bis dahin ausgewerteten Formelbilder vollständig waren. Die innere
Grenze der umfangreichen Formelsatzgruppe beträgt jetzt wie deren bereits
bestehende äußere Grenze 90 Sekunden. Die 36 Fälle und ihre Bildanforderungen
bleiben erhalten. Daraus wird keine gemessene Leistungsverbesserung abgeleitet.

## Tatsächlich ausgeführte Prüfungen

[Quellstände, Binärhashes, Kommandos, Einzelgruppen und Bildherkunft](article-flow-2026-10-21/validation.json)
trennen alle Versuche. Die Builds enthalten unverändert vorgefundene lokale
Accessibility-Arbeit mit separat erfassten Hashes; sie sind kein Nachweis eines
sauberen Checkouts nur dieses Commits.

| Stand / Plattform | Ergebnis |
| --- | --- |
| 69 / Windows | 12/13 bestanden, einschließlich vollständigem H und Seitenwechseln. Inline-Linkfokus scheitert an der beschriebenen Bruchpixel-Ursache. |
| 69 / Linux | 11/13 bestanden, einschließlich vollständigem H. Inline-Linkfokus und das Zeitbudget der Formelsatzgruppe scheitern. |
| 70 / Windows | 10/13 bestanden. Link wird erreicht, Begrenzungsprüfung scheitert noch. Seiten und H haben keinen Windows-Hostfokus. |
| 70 / Linux | 12/13 bestanden, einschließlich H und Formelsatz. Dieselbe Inline-Begrenzungsprüfung scheitert noch. |
| 71 / Windows | 11/13 bestanden: Fenster, Tastatur, Verlauf, Mathematik, Zoom, Fonttransaktionen, Inline-Inhalt, Pixeldichte, Formelsatz, Detailtext und Inline-Formeln. Seiten und H scheitern weiterhin am fehlenden Windows-Hostfokus. |
| 71 / Linux | Alle 13/13 Gruppen bestanden, einschließlich vollständigem H, Seitenwechseln, 48 Inline-Formelfällen und 36 Formelsatzfällen. |

Die begrenzte unabhängige Artikelprüfung liest pro Plattform und Stand alle
56 tatsächlich eingereihten H-Aufnahmen aus: vollständige Titelbytes,
Zeilenhöhen, Titelzeilenabstände, 20-px-Abstand zur Einleitung, gemeinsame
linke Textkante und Schriftrollen. Ein Erfolg dieser Teilprüfung macht einen
fehlgeschlagenen Fokusablauf nicht erfolgreich.

Die Mathematikgruppe erhält den vollständigen Katalog-/Glyphen- und
Codegrafiknachweis einschließlich erzwungener gebündelter Schriften. Die
Inline-Formelgruppe prüft 48 Fälle; die Formelsatzgruppe prüft 36 Fälle mit
tatsächlichen Formelbildern. Weitere vorhandene Gruppen prüfen Codekopien,
Tabellen, externe Links, Zoom, Dichte und Fonttransaktionen.

## Direkter Vergleich und offene Abweichungen

Der neue [Titelkanten-Prüfer](../../../tests/check_f1_article_title.py) vergleicht
den gleichen Titel anhand einer festen Region in unveränderten Vollbildern.
Die Vorlage enthält einen ein Pixel breiten Rahmen und eine 34-px-Mock-Titelleiste;
die nativen Aufnahmen enthalten Clientpixel. Ursprünge und Farbschwellen sind
offengelegt. Jede der vier Kanten hat unverändert höchstens einen Pixel Toleranz.
Die Selbstprüfung benutzt ausschließlich die tatsächliche Originalvorlage.
Dieser Prüfer ist kein Ähnlichkeitsscore für ganze Fenster oder andere DPI-Fälle.

Im Windows-Bild liegen linke Titelkante und vertikale Lage jetzt höchstens
einen Pixel neben der Vorlage. Die rechte Kante liegt noch vier Pixel weiter
außen: `[281,144,721,169]` gegenüber `[281,143,717,168]` in Clientkoordinaten.
Der Prüfer meldet deshalb ausdrücklich **nicht bestanden**. Vor diesem Schritt
lag die rechte Kante bei 741 und der Titel je nach Theme 15–19 px weiter unten.
Die noch fehlende Paar-Kerning-Unterstützung und Unterschiede der Rasterung
werden weiter untersucht; ihre jeweiligen Anteile an den vier Pixeln sind
noch nicht nachgewiesen. Eine technische Unmöglichkeit ist nicht belegt.

Linux zeigt unter seiner tatsächlich geladenen anderen Schrift einen
zweizeiligen Titel und deutlich andere Glyphenformen. Die feste Titelregion
erfasst ihn nicht vollständig und wird ausdrücklich als unvollständig und
nicht bestanden markiert; ihre rechte Kante ist kein Maß der gesamten
Titelbreite. Linux besteht damit die Funktions- und Zeilenboxprüfungen, aber
keine plattformübergreifende Pixelgleichheit. Eine gemeinsame, verfügbare
Schriftgrundlage für Vorlage und Laufzeit ist noch herzustellen.

Die native Artikellänge und Themenliste stammen weiterhin aus dem echten
Handbuch. Die kürzere Demo-Einleitung, Demo-Abschnitte oder ein statisches
Pendel werden nicht als Ersatz für diese Inhalte eingefügt. Das erklärt
abweichenden Inhalt, aber keine abweichende Gestaltung.

Aktuelle unveränderte Aufnahmen:

- [Windows dunkel](article-flow-2026-10-21/windows-h-dark.png),
  [hell](article-flow-2026-10-21/windows-h-light.png),
  [große UI und Leseschrift](article-flow-2026-10-21/windows-h-ui-22-reader-32.png).
- [Linux dunkel](article-flow-2026-10-21/linux-h-dark.png),
  [Mindestfenster](article-flow-2026-10-21/linux-h-minimum.png),
  [große UI und Leseschrift](article-flow-2026-10-21/linux-h-ui-22-reader-32.png).
- [Windows, Codekopf](article-flow-2026-10-21/windows-code.png),
  [lange Linkfokussierung](article-flow-2026-10-21/windows-wrapped-link-focus.png),
  [Formelsatz](article-flow-2026-10-21/windows-math-32.png).
- [Linux, Tabellen-/Codebereich](article-flow-2026-10-21/linux-table.png),
  [Formelsatz](article-flow-2026-10-21/linux-math-32.png),
  [Codegrafik](article-flow-2026-10-21/linux-grid-32.png).

Die 24 PNGs sind verlustfreie RGB-Konvertierungen tatsächlicher nativer BMPs.
Die Windows-Bilder an 900/901 px stammen ausdrücklich aus dem erfolgreichen
Stand 69; der aktuelle Windows-H-Ablauf erreicht diese späten Schritte nicht.
Die anderen aktuellen Bilder stammen aus Stand 71. Quelldatei, Version,
Bildmaße und Pixelhash sind je Bild erfasst. Nichts wurde retuschiert oder skaliert.

## Speicherpflege

Nach der erfolgreichen aktuellen Gegenprüfung wurden 460 abgelöste automatische
Aufnahmen aus erfolgreichen Gruppen der Stände 69/70 entfernt: 1.107.673.980 Bytes,
rund 1,11 GB. [Dateiliste und Hashes](article-flow-2026-10-21/cleanup.json).
Die betroffenen acht Gruppen sind im aktuellen Stand auf beiden Plattformen
positiv geprüft. Sämtliche H-/Mathematik-/Inline-Inhalt-/Formelsatzversuche,
Fehlerbelege, aktuelle 71er-Ausgaben, Referenzen, Protokolle, Quellen, Nutzdaten,
Abhängigkeiten und der gemeinsame WSL-Datenträger bleiben erhalten. Es wurden
nur erzeugte `docs-*.bmp`-/`docs-*.png`-Dateien direkt in den abgelösten Abläufen gelöscht.

## Nächste Abnahme

Weiter offen bleiben Titelbreite/Kerning/Rasterung, Navigationsabstände und
Einzüge, Kategorien, Segmente, Kontext-Tabs sowie die vollständige visuelle
H-Abnahme. Der aktuelle Windows-Fokus, macOS, reale Monitor-/DPI-Wechsel und
externe Screenreader brauchen weiterhin eigene Nachweise. Die übrigen
Arbeitsbereiche und der verbindliche Grafikexport aus ADR 010 bleiben Teil
des unveränderten Gesamtauftrags.
