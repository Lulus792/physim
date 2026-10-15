# F1-Tabellen: gemeinsame Zellgrenzen und native Semantik

Entscheidung vom 15. Oktober 2026. Verantwortlich für den Vertrag ist der
zentrale `accessibility-engineer`; `ui-implementation` und `test-engineer`
decken Darstellung und Nachweise ab. Der Gesamtauftrag bleibt die vollständige
F1-Abnahme und anschließend der gesamte verbindliche Projektplan.

## Problem und verbindliches Ziel

Der bisherige Renderer zerlegt bereits von Inline-Markup bereinigten Text erneut
an jedem `|` und berücksichtigt höchstens 16 Zellen. Damit wird beispielsweise
Code `l|r` zu zwei Spalten; der Strich verschwindet. Native Clients erhalten
keine verlässliche Tabelle mit Zeilen, Spalten, Zellen und Spaltenüberschriften.
Ein Link innerhalb einer Tabellenzeile allein erfüllt dieses Ziel nicht.

Das Ziel ist dieselbe Tabelle in Darstellung und nativem Lesebaum: sämtliche
Zellen einschließlich leerer und außerhalb des Ausschnitts liegender Zellen,
stabile Identitäten während Scrollen und Reflow, eindeutige Überschriften und
passende Textbereiche. Ein Dokumentwechsel macht gehaltene alte Bereiche
ungültig. Ein- und Ausblenden erhält den Text, sperrt aber Bedienaktionen und
sichtbare Geometrie entsprechend dem bestehenden Dokumentvertrag.

## Gemeinsame Darstellung und Inhaltsgrenzen

Der Parser erkennt Spaltentrenner vor dem Entfernen der Inline-Auszeichnung.
Striche in Code, Linkbeschriftungen, Hervorhebungen und maskierte Striche
gehören zum Zelltext. Das ist der Vertrag dieses begrenzten lokalen Readers,
keine Behauptung vollständiger Markdown-Konformität. Die vorhandene kontinuierliche
Lesefassung und die eingebetteten Linkbereiche bleiben erhalten.

Jede Tabellenzeile enthält Zellbereiche im unveränderlichen Klartext, den Index
ihrer ersten Tabellenzeile und ihren nullbasierten Zeilenindex. Zellbereiche
enthalten getrimmten Text und dürfen leer sein. Der Renderer verwendet dieselben
Bereiche für Messung, Zeichnung und native Glyphengeometrie. Es gibt keine
stille Begrenzung auf 16 Spalten. Die deklarierte Kapazität von 16.384 Zellen
wird atomar geprüft; bei Überschreitung bleibt das bisherige Dokument erhalten.
Eine Leerzeile oder anderer Inhalt beendet die Tabellenzugehörigkeit. Eine
Trennzeile darf nur die erste unmittelbar zugehörige Tabellenzeile als
Spaltenüberschrift kennzeichnen.

## Veröffentlichtes gemeinsames Tabelleninventar

Das Accessibility-Modell besitzt jetzt ein eigenes, bei Bedarf angelegtes
Tabelleninventar neben den 256 interaktiven Controls. Es übernimmt sämtliche
Zeilen und vorhandenen Zellbereiche eines Dokuments, auch außerhalb des
sichtbaren Ausschnitts. Seine Grenzen entsprechen dem Parser: 4.096 Tabellen/
Zeilen und 16.384 tatsächlich gespeicherte Zellen. Fehlende Zellen am Ende
kürzerer Zeilen sind leere virtuelle Zellen am Ende des Zeilentextes; sie
benötigen keinen dicht aufgefüllten Speicher. Alle Zeilen werden anhand der
maximalen Spaltenzahl ihrer Tabelle ausgerichtet.

Jede Tabelle reserviert einen eindeutigen ID-Bereich für sich und ihre Zellen.
Geometrieänderungen derselben Inhaltsversion erhalten diese IDs. Andere
Zeilen-/Spaltenzahlen, Zellbereiche oder Headerbeziehungen benötigen eine neue
Inhaltsversion. Dokumentwechsel und Entfernung lassen alte IDs auslaufen.
Zellnamen bleiben vollständige Bereiche im UTF-8-Lesetext. Die erste markierte
Zeile liefert Spaltenüberschriften; unmarkierte Tabellen besitzen keine solche
Zuordnung. Native Adapter können die Zellen nach Koordinaten oder ID abfragen.

Zell- und Tabellenrahmen werden gegen den tatsächlichen Viewport beschnitten.
Während Suspend bleiben Text und IDs lesbar, alle Rahmen sind leer und
Scrollaufträge werden abgewiesen. Eine unvollständige Tabellenregistrierung
derselben Generation oder ein ausdrücklich abgebrochener UI-Entwurf ersetzt
keine veröffentlichte Momentaufnahme. Erst die vollständige Veröffentlichung
gibt einen neuen Entwurf frei. Die bestehende Mutex-Grenze gilt auch für das
neue Inventar; die UI gibt ihre temporären Kopierbereiche nach der Übernahme frei.

## Noch umzusetzende native Verträge

- Das gemeinsame Inventar ist mit dem tatsächlichen F1-Layout verbunden.
  Seine Veröffentlichung allein stellt noch keine Table-/Grid-/AX-/AT-SPI-
  Schnittstellen für externe Clients bereit. Diese Adapter und ihre Prüfungen
  bleiben die nächsten verbindlichen Schritte.
- Windows benötigt Table/Grid sowie TableItem/GridItem mit vollständigen
  Headerbeziehungen. Nur ein neuer ControlType ist kein ausreichender Adapter.
  Grundlage: [Microsoft Table Control Type](https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-supporttablecontroltype).
- Linux benötigt AT-SPI Table und Zellbeziehungen einschließlich außerhalb des
  Viewports liegender Zellen. Grundlage: [AT-SPI Table](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Table.html).
- macOS benötigt native AX-Tabellen-, Zeilen- und Zellobjekte und die zugehörigen
  Header-/Textbeziehungen; die genaue AppKit-Anbindung und ihre externen
  Clientnachweise stehen noch aus.
- Unabhängige Clients müssen dieselbe Unicode-Tabelle mit leerer Zelle,
  eingebettetem Link und Offscreen-Zeile lesen, Koordinaten und Überschriften
  zuordnen und gehaltene Referenzen nach Dokumentwechsel abweisen. Die
  praktischen Screenreader-Aufgaben bleiben zusätzliche Abnahmen.

Der gemeinsame Parser-/Renderer-Schritt allein schließt die offene
Tabellensemantik nicht und erhöht die Gesamtwertung nicht auf 10/10.

## Tatsächlich ausgeführte Prüfungen

Windows 10.0.26200 / MSVC ASan / SDL 3.2.30 besteht **3/3** native Fälle
(`run-dvkhy_7x`: Parser, API-Referenz und Abschnitts-/Themeninventur) und
**4/4** GUI-Fälle (`run-yf2um606`: Inline-Inhalte, Zeicheninventur, Zoom und
unabhängiger Windows-Linkclient). Alpine 3.24 / GCC 15.2 ASan/UBSan mit
X11/Openbox und Software-GL besteht dieselben **3/3** nativen Fälle
(`run-o8g_bwvo`) und **4/4** GUI-Fälle (`run-rwmqtq3u`), mit unabhängigem
AT-SPI-Linkclient statt Windows UIA. Beide Themeninventuren bestätigen
**185 Seiten und 214 Abschnittslinks**. Der vorherige Linux-Lauf
`run-x768ski9` bestand 2/3: Er fand die aus ADR 004 verlinkte, aber nicht
registrierte ADR 005. ADR 005 und diese Entscheidung sind jetzt im nativen
Themenkatalog enthalten.

Die zusätzliche GUI-Probe verlangt einen tatsächlich veröffentlichten
Glyphenrahmen für den Strich in `l|r` bei 16 Pixeln und setzt anschließend
die vorhandene Tastaturnavigation zum Tabellenlink bei 32 Pixeln fort.
Ihre erste Platzierung nach dem 32-Pixel-Linkfokus scheiterte in
`run-f2wrsd0r`, weil der Strich oberhalb des sichtbaren Ausschnitts lag;
das Originalbild bestätigt diesen Prüferfehler. Die Probe liegt deshalb
in der vollständig sichtbaren Anfangsansicht. Mit der korrigierten Probe
und dem vorherigen Renderer scheitert der tatsächliche Gegenlauf
`run-76pdfdt2` an Stufe 2 mit Exit 1: Der Strich fehlt. Danach wurde die
korrigierte Quelle wiederhergestellt und der finale Verbund erneut ausgeführt.
Originale und Gegenprobenprotokoll liegen unter `build/native`.

Die Windows- und Linux-16-Pixel-Bilder mit zwei Zellen und sichtbarem `l|r`
wurden unmittelbar geprüft. Testkatalog und erwartete Läuferfehler bestehen
in `build/native/table-cells-runner-proof`. Der macOS-Workflow enthält jetzt
zusätzlich Parser, Themen-/Abschnittsinventur und den tatsächlichen Inline-Fall;
seine Definition allein ist kein macOS-Ausführungsnachweis.

## Prüfungen des veröffentlichten Tabelleninventars

Die neue gemeinsame Modell-/UI-Anbindung besteht unter Windows MSVC ASan
**10/10** native Accessibility-/Dokumentfälle (`run-phwh4rq2`) und
**7/7** GUI-Fälle (`run-ag2ji7pn`): Zeicheninventur, Inline-Inhalte, Dichte,
Navigation sowie unabhängige UIA-Lese-, Feld- und Linkclients. Linux GCC 15.2
ASan/UBSan mit X11/Openbox besteht **11/11** native Fälle einschließlich
Themen-/Abschnittsinventur (`run-eyuuwhql`) und **7/7** entsprechende GUI-Fälle
mit unabhängigen AT-SPI-Clients (`run-baz5hkit`).

Der erweiterte tatsächliche Inline-Fall liest die veröffentlichte Tabelle und
ihre Zell-/Headerbereiche. Er hält dieselbe Zelle über einen echten
16-/32-/16-Pixel-Reflow und anschließendes Scrollen; ein Dokumentwechsel
liefert neue IDs. Windows besteht die Ergänzung zusammen mit Schriftbedarf
und externem Fenster-Lebenszyklus **3/3** (`run-wzn_9h15`), Linux mit
Schriftbedarf **2/2** (`run-4sjdvprt`). Nach der ausdrücklichen Initialisierung
der kopierten UI-Ausgaben bestehen die finalen Inline-Fälle nochmals **1/1**
auf Windows (`run-b_nat9lw`) und Linux (`run-r8lfpf20`); der native Linux-
Tabellenfall besteht dabei zusätzlich **1/1** (`run-q6lui_gl`). Beide finalen
16-Pixel-Aufnahmen wurden geöffnet und geprüft.

Die separate GCC-Probe besteht mit ASan, UBSan und eingeschaltetem
LeakSanitizer. Die gezielte Gegenfassung entfernt nur die Suspend-Sperre der
Zellrahmen und wird mit Exit 1 an der weiterhin sichtbaren verborgenen Zelle
abgewiesen. Quellenkopien, Originalausgaben und Bericht liegen unter
`build/native/table-model-counter`. Der erste Aufbau dieser separaten Probe
benötigte zusätzlich den vorhandenen `src`-Include-Pfad; nach dessen Ergänzung
wurde die tatsächliche positive und negative Ausführung bestätigt.

Der Modellfall prüft außerdem vollständige Unicode-Bereiche, virtuelle leere
Zellen, Headerbeziehungen, Clipgeometrie, stale IDs, den verworfenen UI-Entwurf,
unvollständige Veröffentlichungen derselben Version, alle 16.384 gespeicherten
Zellen mit abgewiesener zusätzlicher Zelle, ID-Überlauf und die unabhängige
Nutzung aller 256 interaktiven Controlplätze. Diese Nachweise bestätigen den
gemeinsamen Unterbau. Die externen nativen Tabelleninterfaces fehlen weiterhin.
