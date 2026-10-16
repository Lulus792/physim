# Visueller Abgleich mit der ausgewählten H-Vorlage

Nach dem Nutzervergleich vom 16. Oktober 2026 wurde die native Umsetzung erneut
direkt mit `h-dark.png`, `h-light.png` und den zugehörigen CSS-Regeln verglichen.
Dieser Bericht ergänzt die historische Abnahme in [NATIVE-H.md](NATIVE-H.md).

## Weitere Annäherung an die Referenz (17. Oktober 2026)

Die Paneel-Icons verwenden jetzt unmittelbar die Geometrie von `panelIcon()` aus
`refinements.js`: dieselben 24 Vektoreinheiten, auf die 20-Pixel-Iconbox skaliert,
einschließlich Kontur, Teilung und transparenter Seitenfüllung. Zuvor wurde die
Außenkontur selbst 20 Pixel breit gezeichnet und wirkte dadurch deutlich größer.
Die ganze Bedienfläche bleibt anklickbar und per Tastatur erreichbar.

Der gemeinsame C/Physim-Schalter ist jetzt 34 Pixel hoch, entsprechend den
28-Pixel-Segmenten mit Innenabstand und Rahmen der Vorlage. Die Kopfleiste erhält
den passenden Einzug. „PHYSIM“ verwendet echte Fettschrift, „Handbuch“ daneben
reguläre, gedämpfte Schrift. Die linken Gruppenbeschriftungen beginnen wie in H
mit sieben Pixeln Innenabstand. Modell/Beispiel bleiben auch unter dem Mauszeiger
ungefüllte Text-Tabs; nur das aktive Sprachsegment erhält seine dezente Fläche.
Die aktive Beschriftung verwendet die H-Textrolle `doc_text`.

Die Kopfleiste verwendet außerdem den 12-Pixel-Abstand zwischen den Elementen
aus `refinements.css`, anstelle des allgemeinen nativen 8-Pixel-Abstands.
Neue native Aufnahmen derselben geprüften Ausgabe: [Dunkel](native-h-reference-dark.png),
[Hell](native-h-reference-light.png), [Mindestfenster](native-h-reference-minimum.png)
und [UI 22 / Leseschrift 32](native-h-reference-large.png). Alle vier Bilder wurden
direkt geöffnet und mit der H-Vorlage verglichen; sie sind unveränderte
Formatkonvertierungen der nativen BMP-Aufnahmen.

Die abschließende Windows-Gruppe `run-fe9laksh` besteht 4/4: Tastatur, lange
Sidebartexte, F6-Bereichsnavigation und H. H umfasst alle 56 tatsächlichen
Fensterzustände einschließlich Hell/Dunkel/hohem Kontrast, großen Schriften,
unabhängigen Seiten, Bildpixeln und Navigationsgeometrie.
Windows-Binary-SHA256:
`ec03923d9967c8cffecdf14cd33792e44a785cc818cc3cc372ec1f5402227382`.
Der direkte Aufruf war `tools/build.py --build-dir
build/native/F1-reading-focus/Debug-sanitized --sanitizers --jobs 4 --test-display
--test-filter documentation_h --test-filter documentation_sidebar --test-filter
documentation_keyboard --test-filter documentation_panes`.

Die entsprechende Linux-Gruppe besteht ebenfalls 4/4 (`run-qgh_6l1h`, Alpine
3.24.2, GCC 15.2, ASan/UBSan, privates X11/Xvfb/DBus). Dunkel und große Schrift
wurden auch anhand ihrer tatsächlichen Linux-Bilder angesehen. Der zusätzliche
normale X11-/AT-SPI-Bedienlauf `build/native/h-reference-normal-pointer` besteht
alle vier Aufgaben: Sprach-/Kontextwechsel, unabhängige Paneele sowie lokale und
globale Tastatursuche mit F6. Er verwendet die ursprünglichen Eingabezeiten ohne
Diagnosemodus. Linux-Binary-SHA256:
`a13c23b1303a1e08fcb0db94b1820ba575cd8a375e80693ffb1229caaa321200`.
Für diesen jüngsten Detailpatch liegt noch keine neue macOS-Ausführung vor.
Die älteren revisionsbezogenen Mac-Nachweise unten bleiben davon getrennt.

Die geprüfte Windows-EXE liegt zusammen mit ihren passenden Debugsymbolen wieder
unter `build/native/Debug-sanitized/bin/physim.exe`. Vor der Übernahme wurden ein
laufendes Programm an diesem Zielpfad ausgeschlossen und die SDL-/ASan-DLLs
abgeglichen. Nach finaler Abnahme wurden 93 ersetzte BMPs des erfolgreichen
Windows-Zwischenlaufs entfernt (202.793.634 Byte); Ergebnisdateien, finale Bilder
und alle Referenzen bleiben erhalten.

Diese Details sind im vorhandenen Renderer möglich. Die am Ende dieses Berichts
beschriebenen Unterschiede durch Fensterrahmen, Textmetriken, vollständige Inhalte
und zusätzliche Handbuchfunktionen bleiben davon getrennt.

## Weiterer Detailabgleich der Navigation und Umschalter (17. Oktober 2026)

Der erneute Vergleich mit H trennt jetzt kleine Gruppenbeschriftungen von
Bedienbeschriftungen. Themen, Abschnittssprünge, C/Physim, Modell/Beispiel und
das globale Suchfeld verwenden die vorhandene reguläre UI-Schrift. Aktive Themen,
Abschnitte und ausgewählte Sprach-/Kontextreiter verwenden die echte Fettschrift
derselben Größe. Bei UI 16 sind dies 16 logische Pixel; die kleinen Beschriftungen
bleiben bei 14. Damit entsprechen Textgewicht und Größenhierarchie der Vorlage
enger. Der Leserzoom verändert weiterhin ausschließlich die Leseschrift.

Die Breite jedes Sprachsegments und Kontextreiters berücksichtigt beide
Schriftgewichte, damit ein Wechsel die Positionen nicht verschiebt. Die
Abschnittshöhe wird mit derselben Schrift und nutzbaren Breite wie der gezeichnete
Text gemessen. Lange echte Abschnittsnamen brechen um. Der Suchfeldrahmen und
der Hintergrund des rechten Paneelkopfs entsprechen jetzt ebenfalls den
semantischen H-Rollen `divider` und `header`.

Neue unveränderte native Aufnahmen, lediglich von BMP nach PNG konvertiert:
[Dunkel](native-h-controls-dark.png), [Hell](native-h-controls-light.png),
[Mindestfenster](native-h-controls-minimum.png) und
[UI 22 / Leseschrift 32](native-h-controls-large.png).
Die weiter unten aufgeführten früheren Bilder bleiben als historische Abnahme erhalten.

Der Windows-Lauf `run-56umm8n9` besteht zunächst 8/8: native Accessibility, Tastatur,
Schriftbedarf einschließlich Allokationsfehler/Dichtewechsel, Inline-Formeln,
lange Sidebartexte, Display-Formelsatz, F6-Bereichsnavigation und H.
H prüft alle 56 realen Zustände einschließlich unabhängiger Seiten, Lesezoom,
UI 22 sowie Hell/Dunkel/hohem Kontrast und echter Bildpixel. Die vier neuen
Bilder wurden zusätzlich direkt mit der Vorlage verglichen.
Die entsprechende Linux-Gruppe besteht 7/7 (`run-k9fuo9pr`, Alpine 3.24.2,
GCC 15.2, ASan/UBSan, X11/Xvfb/Openbox). Der zusätzliche native COM-Fall der
Windows-Gruppe gehört nicht zum Linux-Testkatalog.

Die tatsächliche Linux-Bildprüfung zeigt bei UI 22 noch eine abgeschnittene
Pendelbeschriftung. Die Korrektur reserviert die gemessene Captionhöhe innerhalb
des Grafikrechtecks, bricht den Text um und zentriert jede Zeile einzeln.
Die Grafikzeichen folgen jetzt der UI-Schrift statt dem Leserzoom.
Der bestehende unabhängige Metazeilenprüfer kontrolliert in H zusätzlich die
tatsächlich gezeichneten Captionbytes und Glyphen gegen Rechteck und Clip.
Windows besteht danach H und Bereichsnavigation erneut 2/2 (`run-52linhe9`).
Eine Kontrolle der Messdaten zeigt, dass der erste Caption-Prüfaufbau durch die
später gebaute Artikel-Metazeile überschrieben wurde. Dieser Lauf zählt deshalb
ausdrücklich nicht als Caption-Glyphennachweis. Die Captionmessstelle wird nun
erst nach dem vollständigen Fensteraufbau gesetzt. Der abschließende H-Lauf
`run-l3dht5dp` besteht 1/1 und prüft tatsächlich die Beschriftung in allen 28
sichtbaren Matrixzuständen. Von 143 Probeaufrufen sind 141 vollständige
Captionmessungen; zwei frühe Aufrufe vor dem Pendelaufbau besitzen noch kein
Zielrechteck und gehören nicht zur Abnahmematrix.
Die vier neuen PNGs stammen aus diesem abschließenden Lauf.
Linux bestätigt nach derselben Messstellenkorrektur ebenfalls H 1/1
(`run-johu7rnp`), einschließlich vollständiger zweizeiliger Caption bei UI 22
in Hell und Dunkel. Auch dort sind 141 Captionmessungen vollständig; die zwei
Startaufrufe ohne Modellziel zählen nicht zur Abnahmematrix.
Linux-Binary-SHA256:
`c9f0b6aed8fc78c2031cd9ecb8ede91b68d9a7b5f7d9e0adeda62e83322ccb13`.
Aktuelles Windows-Binary (MSVC/ASan), SHA256:
`1aff5f591d5ac062c94c05900c194fdb2b9210a7784ac16cfcd7daa6c6aa01fc`.
Dieses geprüfte Programm und seine passende PDB liegen auch unter
`build/native/Debug-sanitized/bin/physim.exe`; SDL und beide ASan-Runtimes wurden
vor der Übernahme per Hash verglichen. Das vorherige EXE-/PDB-Paar bleibt lokal gesichert.

Die isolierte Sidebar-Gegenprobe verwendet jetzt ebenfalls die aktuelle
Schrift-/Breitenmesszeile. Ihr Fehler bleibt die absichtlich wieder eingesetzte
einzeilige Höhe; der unabhängige Zeichen-/Clipprüfer bleibt unverändert.
Der tatsächliche Linux-Gegenlauf gegen den Typografiestand vor der
anschließenden Captionkorrektur verwirft genau den abgeschnittenen Abschnitt
(`build/native/h-detail-sidebar-counterexample/report.json`, `precise_rejection=true`).
Der native Parser bestätigt abschließend 185 Seiten und 220 Abschnittslinks
(`build/native/h-detail-anchors-native.log`).

Der inzwischen abgeschlossene [Mac-Lauf von 3d89897](https://github.com/Lulus792/physim/actions/runs/38974783249)
bestätigt diese Detailrevision auf ARM und Intel: jeweils 19/19 portable und
27/27 GUI-Fälle einschließlich H, Bereiche und Formelsatz. Beide bestehen auch
die Metadaten-/Sidebar-/Textur-Gegenproben, die 36 historischen Kontrollzustände
und die unabhängige externe AX-Prüfung. ARM: `run-lo1yu45e` / `run-07gjp7bm`;
Intel: `run-l8s__scq` / `run-lb4v2tks`. Die tatsächlichen Jobauszüge liegen unter
`build/native/mac3d-117221484706-verified.log` beziehungsweise
`mac3d-117221484344-verified.log`. Das belegt die H-Detailänderung; die danach
entstandene Buchstabenkorrektur besitzt eine getrennte Abnahme.

Die zusätzliche normale Linux-App-Prüfung
`build/native/h-detail-normal-pointer/results.json` bestätigt reale Mausklicks
auf beide Sprachen, beide Kontextreiter und beide Seitenicons einschließlich
exklusiver Zustände und unabhängiger Rechtecke. Die anschließende lokale
Texteingabe im unveränderten 40-ms-Burst scheitert: Statt `theta` steht `a` im
Suchfeld. Dieser Eingabebefund wird nicht als behoben ausgegeben. Der Prüfer
behält den bisherigen Burst als Standard und bietet für die Ursachenabgrenzung
zusätzlich `--acknowledge-text`: einzelne echte X11-Zeichen mit unabhängiger
AT-SPI-Bestätigung jedes Präfixes und ohne unnötiges Strg+A im leeren Feld.
Auch der tatsächliche abschließende Diagnose-Lauf
`build/native/h-detail-normal-pointer-paced/results.json` bestätigt die beiden
Maus-/Geometrieaufgaben, scheitert aber bei der Bestätigung der lokalen
Texteingabe. Damit ist ein bloß zu schneller Zeichenburst als alleinige
Erklärung nicht belegt; weder diese Eingabeaufgabe noch die danach liegenden
normalen F6-/Filteraufgaben werden als bestanden gewertet. Die unabhängigen
nativen Tastatur-/Bereichstests oben bleiben separat ausgewiesen.

Die anschließende [gezielte Eingabekorrektur](PLAIN-LETTER-INPUT.md) belegt
inzwischen die Ursache: normale Buchstaben konnten fälschlich Tastenkürzel
auslösen. Der ursprüngliche normale Linux-Ablauf besteht nach dieser Korrektur
ohne zusätzliche Wartezeiten. Die obigen Fehlbelege bleiben als Vorherzustand erhalten.

Die 61 doppelten BMPs des ersten erfolgreichen Windows-H-Einzellaufs wurden nach
der vollständigen Abnahme entfernt (150.258.018 Byte); finale Gruppenbilder,
Ergebnisdateien und die vier versionierten PNGs bleiben erhalten.

Die restlichen Unterschiede sind im letzten Abschnitt begründet. Insbesondere
sind ein anderer Fensterrahmen und unterschiedliche Schriftglättung konkrete
Plattformunterschiede. Die zusätzliche Artikelwerkzeugleiste und der vollständige
Inhaltsbestand sind bewusste funktionale Abweichungen; sie sind technisch
veränderbar und werden nicht als Unmöglichkeit des nativen Renderers ausgegeben.

## Übernommene Gestaltung

- Die linke Leiste besitzt einen festen, durch eine Linie getrennten Kopf und
  einen eigenen scrollbaren Inhalt. Themen stehen in Gruppen; die direkten
  Abschnittssprünge liegen eingerückt unter dem aktiven Artikel, mit senkrechter
  Führungslinie und eigener aktiver Markierung. Die vollständige Themenliste bleibt erhalten.
- C/Physim liegt in einer gemeinsamen abgerundeten Umrandung. Nur die gewählte
  Hälfte erhält die dezente Schalterfläche. Die bisherige blaue Füllung und der
  Unterstrich dieser Sprachwahl entfallen.
- Modell/Beispiel sind schmale Text-Tabs am linken Rand des Paneelkopfs. Die
  Auswahl wird durch den dünnen Akzentstrich markiert; große gefüllte Buttons entfallen.
- Die globale Suche steht wie in der Vorlage rechts. Innere Paneele besitzen
  gerade Kanten. Artikelüberschriften, Modellzeichnung, Wertezeilen und Hinweisblock
  sind an die visuelle Hierarchie der Vorlage angenähert.
- Die Pendelgrafik verwendet die logischen Vektorkoordinaten aus `model.svg`,
  einschließlich Winkelbogen sowie θ, L und m. Die anderen Modelle behalten ihren fachlichen Kontext.

Die kompakte native Bedienschrift besitzt eine eigene, nach Bedarf geladene Rolle:
14 logische Pixel bei UI-Einstellung 16, proportional bis 19,25 bei UI 22.
Der native Glyphensatz und die Browser-Vorschau verwenden unterschiedliche
Schriftmetriken; die Größe wurde deshalb anhand der tatsächlichen Bilder gewählt.
Die Leseschrift bleibt unabhängig mit 16 bis 32 Pixeln einstellbar. Auch die
Mindesthöhen der Bedienelemente folgen jetzt der UI-Einstellung, statt durch
Lesezoom oder einen vorherigen Themewechsel unbeabsichtigt zu wachsen.

## Tatsächliche native Fensterbilder

Die PNGs sind Formatkonvertierungen der nativen BMP-Aufnahmen, ohne nachträgliche
Layout- oder Inhaltsänderung. Der abschließende Lauf und Binary-Hash werden unten genannt.

| Zustand | Dunkel | Hell |
| --- | --- | --- |
| Beide geöffnet | [Bild](native-h-fidelity-dark.png) | [Bild](native-h-fidelity-light.png) |
| Links geschlossen | [Bild](native-h-fidelity-dark-left-closed.png) | [Bild](native-h-fidelity-light-left-closed.png) |
| Beide geschlossen | [Bild](native-h-fidelity-dark-both-closed.png) | [Bild](native-h-fidelity-light-both-closed.png) |
| Rechts geschlossen | [Bild](native-h-fidelity-dark-right-closed.png) | [Bild](native-h-fidelity-light-right-closed.png) |

Weitere Aufnahmen: [Mindestfenster](native-h-fidelity-minimum.png),
[UI 22 mit Leseschrift 32](native-h-fidelity-large.png),
[Physim-Beispiel](native-h-fidelity-example.png),
[hoher Kontrast](native-h-fidelity-contrast.png).

## Abnahme und Regression

`documentation_h` prüft weiterhin 56 echte Fensterzustände und die vorhandenen
Such-, Sprach-, Kopier-, Kontext- und Eingabeaufgaben. Hinzu kommen echte
Themen-/Abschnittsrechtecke: Abschnitte müssen eingerückt unter dem Artikel liegen,
und ihre Geometrie muss bei gleicher UI-Einstellung trotz Lesezoom, Themewechsel
und Umschalten der anderen Seite gleich bleiben.

Ein Bildpixelprüfer erfasst zusätzlich alle 56 Zustände in Hell, Dunkel und hohem Kontrast. Er verlangt
den gemeinsamen Segmentschalter, die richtige dezente Auswahlfläche und
ungefüllte Text-Tabs mit dünnem Akzentstrich. Bei hohem Kontrast erhält die
gewählte Sprachhälfte zusätzlich eine deutliche Akzentkontur. Er berücksichtigt die geglättete
Randkante des Strichs, verlangt aber weiterhin seinen tatsächlichen Akzentkern.
Das ältere echte native Fensterbild mit blau gefüllten Sprachbuttons wird
abgelehnt; ein absichtlich blau gefülltes Gegenbeispiel ebenfalls.

Die Schriftprüfung umfasst jetzt 42 F1-Varianten einschließlich der vier neuen
kompakten Größen. Im normalen Start werden sechs benötigte Varianten geladen.
Die vorhandenen Tests für fehlgeschlagene Allokation, unveränderte Einstellungen,
Cache-Wiederverwendung und Rasterdichte 1/2/1 bleiben erhalten.

Tatsächlich ausgeführte Abnahme des hier beschriebenen Stands:

| Umgebung | Ergebnis | Lauf |
| --- | --- | --- |
| Windows, MSVC/ASan | 8/8: Tastatur, mathematische Zeichen, Schriftbedarf, Navigation, Sidebar, Display- und Inline-Formelsatz, H | `run-wfw8hkai` |
| Windows, abschließende Kontrastkontur | H 1/1, alle 56 Zustände einschließlich Pixel- und Navigationsgeometrieprüfung | `run-5q0yq9r9` |
| Alpine Linux, GCC 15.2/ASan/UBSan, X11/Xvfb/Openbox | dieselben 8/8 einschließlich der abschließenden Kontur | `run-8ydqt2qe` |

Die zwölf Bilder stammen aus dem abschließenden Windows-Lauf `run-5q0yq9r9`.
Windows-Binary-SHA256:
`0875a28ea0370507493187bee0ec8e7cb8bc8a25fd328dde251f24f78e11f268`.
Linux-Binary-SHA256:
`d1b144df60ba55ae7bf10cc398762708f0214c83c3dc7e44285e2f9322a3ee81`.
Die vollständige Windows-Gruppe lief vor der letzten, ausschließlich hohen Kontrast
betreffenden Auswahlkontur; danach wurden H unter Windows und die gesamte genannte
Linux-Gruppe erneut erfolgreich ausgeführt. Die Bilder für Mindestfenster,
große Schrift, Beispiel und sämtliche Seitenzustände wurden tatsächlich geöffnet
und visuell mit der Vorlage verglichen.

Der native abschließende Parser-/Linkcheck bestätigt 185 registrierte Seiten und
220 Abschnittslinks (`build/native/h-fidelity-anchor-final.log`). Beide Lernwege
bestehen außerdem die vorhandene Prüfung ihrer Kapitel, vollständigen Quellen und
acht gemeinsamen Modellpaare. Der geprüfte Windows-Programmstand wurde nach
Abgleich der SDL-DLL auch nach `build/native/Debug-sanitized/bin/physim.exe`
übernommen; sein SHA256 entspricht dem oben genannten Windows-Binary.

Zusätzlich besteht `tests/check_documentation_h_pointer.py` gegen die normale
Linux-App ohne Selbsttestschalter (`build/native/h-fidelity-normal-pointer-4`).
Der unabhängige AT-SPI-Client findet die tatsächlichen Bildschirmrechtecke;
X11-Mauseingaben bedienen Suche, Pendel-Treffer, beide Sprachhälften, beide
Kontextreiter und beide Seitenicons. Die nativen Radiozustände sind exklusiv,
C/Physim erhält die Kontextauswahl und das linke Einklappen verändert das rechte
Rechteck nicht. Beim Schließen beider Seiten bleibt der Artikel über 700 Pixel breit.
Der Prüfer hält den Mausknopf 150 ms; die vorherigen kurzen X11-Klicks wurden in
dieser instrumentierten Sitzung nicht zuverlässig als gedrückter Zustand erfasst.
Das ist kein Beleg für vollständige Screenreader-Aufgaben.

Reproduktion mit frischem Ausgabeordner und installierten PyAT-SPI, python-dbus,
Openbox und xdotool:

```sh
xvfb-run -a -s '-screen 0 1280x1024x24' dbus-run-session -- \
  python3 tests/check_documentation_h_pointer.py \
  --app build/native/F1-linux/Debug-sanitized/bin/physim \
  --work build/native/h-pointer-new
```

Der Formel-Assetprüfer behält seine vollständigen Integritäts- und Inhaltsprüfungen.
Die Prüfung auf mindestens ein transparentes Byte verwendet jetzt `0 in pixels`
anstelle von `min(pixels) == 0`. Für die bereits geprüften nichtleeren Bytepuffer
ist die Aussage identisch; die Mitgliedschaftssuche vermeidet das vollständige
Durchlaufen jedes großen Rasters im Minimum-Iterator. Die unveränderten
193 Ausdrücke, 1351 Displayvarianten, 193 Inlinevarianten und 197 Displayblöcke
bestehen den vollständigen Prüfer.

## Nachgeführte Gegenproben

Der alte Metadaten-Gegenprüfer stellte nach der Umstellung auf H nur den früheren
Umbruch wieder her. Die neue Bedienschrift, Artikelbreite und kurze Formatierung
blieben aktiv. Das tatsächlich heruntergeladene ARM-Artefakt von `bda1d89`
(`38924682884`, ZIP-SHA256
`b5b34f62e70ef1573ee954a0e46181da1b826bbba6378008d0660a747db6e175`)
bestätigt: Die vermeintlich fehlerhafte Ausgabe bestand alle 36 Zustände.
Ein neuer lokaler Lauf von `77dec3a` bestätigt dasselbe. Das war ein fehlender
Fehlernachweis, kein fehlgeschlagener aktueller F1-Umbruch.

Der isolierte Prüfer rekonstruiert jetzt Leseschrift, Artikelrechteck und
Innenabstand sowie die damalige kurze/lange Metadatenformatierung anhand von
`2b31a7d^`. Der damalige Artikel besaß keinen rechten Modellbereich; dessen
H-Reiter werden deshalb in dieser Rekonstruktion ausgeblendet. Ein vorgeschalteter
Kontrolllauf hatte nachgewiesen, dass ihre sonst überlappenden Zeichenbefehle den
Probeinhalt verunreinigen. Der unabhängige Glyphen-/Clipprüfer bleibt unverändert.

Unter Alpine/GCC 15.2 ASan/UBSan verwirft die abschließende Gegenprobe
`build/native/F1-linux/detail-counterexample-final/report.json` gezielt Fall 2:
alle 64 Textbytes sind gezeichnet, ihre tatsächlichen Glyphen passen aber nicht
in das Rechteck (`complete=0`, `fits=0`). Der korrigierte Umbruch besteht unter
derselben historischen Schrift, Breite und Formatierung alle 36 Zustände
(`build/native/F1-linux/detail-bounded-control-isolated/report.json`,
`bounded_control_passed=true`). Beide Berichte nennen denselben Hash der
isolierten Fenstergeometrie. Der Kontrollmodus `--bounded-control` gehört nun
auch zur Mac-CI; beide Varianten bewahren ihre Bilder und Quellkopien getrennt.
`--proof-dir` erlaubt frische lokale Belege, ohne ältere Ergebnisse zu überschreiben.

Die alte Sidebar-Gegenprobe brach dagegen bereits vor dem Fensteraufbau ab, weil
sie die inzwischen ersetzte Messzeile suchte. Sie setzt nun im tatsächlichen
eingerückten H-Abschnittsrenderer wieder eine feste einzeilige Höhe ein. Die
native Probe verwirft diesen Mutanten gezielt: eine Textzeile bei unvollständigem
Abschnitt (`font=16 lines=1 complete=0 width=165 height=24`,
`build/native/F1-linux/sidebar-counterexample-final/report.json`). Die echten
aktuellen Metadaten- und Sidebar-Prüfungen bestehen danach unverändert beide
(`build/native/h-proof-positive-final.log`). Das erweitert die Fehlererkennung;
es ändert weder den ausgelieferten H-Renderer noch die konservative Gesamtwertung.

Der [tatsächliche Mac-Lauf von 77dec3a](https://github.com/Lulus792/physim/actions/runs/38933979388)
bestätigt auf ARM und Intel jeweils **19/19 portable und 26/26 GUI-Fälle**
einschließlich H und Metadaten, dazu jeweils die unabhängige externe AX-Prüfung.
ARM: GUI `run-6r0dk5t5`, portable `run-5wdlq4lh`; Intel: GUI `run-2xm5zkov`,
portable `run-0wgm6uxx`. Beide Gesamtjobs scheitern noch an der dort enthaltenen
alten Metadaten-Gegenprobe.

Der [nachfolgende tatsächliche Lauf von 123d8ad](https://github.com/Lulus792/physim/actions/runs/38936635632)
bestätigt auf ARM die nachgeführten Prüfer: 19/19 portable (`run-297_op67`),
26/26 GUI (`run-dc0p5xs6`), gezielte Metadaten-Ablehnung, alle 36 korrigierten
historischen Kontrollzustände, gezielte Sidebar-Ablehnung und externe AX-Prüfung.
Der Gesamtjob besteht. Intel besteht 19/19 portable (`run-ndw4h218`), 25/26 GUI
(`run-oiuzcvb_`) und externe AX. Sein Inline-Selbsttest läuft bei Stage 9 während
der Tastatursuche nach Kopierziel `30009` nach 60,064 Sekunden ab; die
anschließenden Gegenproben werden dort nicht ausgeführt. Dieser Befund bleibt
gesondert zu untersuchen, ohne die Zeitgrenze einfach anzuheben.

Die anschließende [native Lesefokuserweiterung](READING-FOCUS.md) besitzt eigene
Windows-/Linux-, Orca- und revisionsbezogene Mac-Nachweise. Die ältere
Mac-H-Abnahme allein belegt diese Erweiterung nicht.
Die danach ergänzte [F6-Bereichsnavigation](PANE-NAVIGATION.md) prüft zusätzlich
native Fokusrechtecke bei großer Schrift und tatsächliche Eingabe nach Tooltips.

## Verbleibende Unterschiede und ihre Gründe

Die genannten Schalterformen, Einrückungen und Paneelköpfe sind im bestehenden
C-/SDL-/Nuklear-Renderer möglich und wurden umgesetzt. Dafür ist kein WebView nötig.

Das Mockup zeigt sechs Beispielthemen und beim Pendel vier Abschnittssprünge.
Die echte Hilfe besitzt 185 registrierte Seiten und beim Pendel weitere
Abschnitte, Fachgrafiken und vollständige Quellen. Daher unterscheiden sich
Listenlänge, Textumbrüche und Artikelhöhe. Diese Inhalte werden für eine optisch
kürzere Vorschau nicht entfernt. Die Wertezeilen enthalten auch die tatsächlich
verwendete Schwerkraft, die in der kleinen Beispielgrafik nicht als eigene Zeile stand.

Der native Fensterrahmen stammt weiterhin vom Betriebssystem. Die im Mockup
gezeichnete Titelleiste ist ein HTML-Beispiel. Eine identische selbst gezeichnete
Titelleiste wäre ein eigener Fensterrahmen mit zusätzlicher plattformspezifischer
Bedienintegration; die aktuelle F1-Umsetzung behält die nativen Fensterfunktionen.
Auch pixelidentische Schriftglättung zwischen Browser und Nuklear kann über
verschiedene Betriebssysteme, Systemschriften und DPI-Einstellungen nicht garantiert werden.

Die native Artikelwerkzeugleiste für Verlauf, Start, Workspace und lokale Suche
bleibt erhalten. Dieser zusätzliche Funktionsumfang ist eine bewusste Abweichung
von der reduzierten Vorschau, keine technische Unmöglichkeit des Renderers.
Die neue Gestaltung behauptet deshalb keine vollständige Pixelidentität des gesamten Handbuchinhalts.

Vollständige Screenreader-Aufgaben und echte Monitor-/DPI-Wechsel bleiben
gesonderte offene Prüfpunkte. Die anschließend geprüfte Erweiterung für
Dokumentfokus und deren Mac-Abnahme sind im eigenen Bericht oben abgegrenzt.
