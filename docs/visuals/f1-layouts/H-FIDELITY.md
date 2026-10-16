# Visueller Abgleich mit der ausgewählten H-Vorlage

Nach dem Nutzervergleich vom 16. Oktober 2026 wurde die native Umsetzung erneut
direkt mit `h-dark.png`, `h-light.png` und den zugehörigen CSS-Regeln verglichen.
Dieser Bericht ergänzt die historische Abnahme in [NATIVE-H.md](NATIVE-H.md).

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
bestätigt auf ARM **19/19 portable und 26/26 GUI-Fälle** einschließlich H und
Metadaten, dazu die unabhängige externe AX-Prüfung. Die GUI-Gruppe heißt
`run-6r0dk5t5`, die portable Gruppe `run-5wdlq4lh`. Der Gesamtjob scheitert noch
an der dort enthaltenen alten Metadaten-Gegenprobe. Die nachgeführten Prüfer
benötigen einen eigenen tatsächlichen Mac-Lauf; die ARM-GUI-Ergebnisse ersetzen
ihn nicht.

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

Vollständige Screenreader-Aufgaben, echte Monitor-/DPI-Wechsel und die gesamte
aktuelle macOS-Abnahme bleiben gesonderte offene Prüfpunkte. Die bereits belegte
Orca-Korrektur für aktive Fenster bleibt erhalten; die noch nicht fertig geprüfte
Erweiterung für Dokumentfokus gehört nicht zu diesem visuellen Stand.
