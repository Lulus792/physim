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

## Native Verträge und verbleibende Plattformen

- Das gemeinsame Inventar ist mit dem tatsächlichen F1-Layout verbunden.
  Seine Veröffentlichung allein stellt noch keine Table-/Grid-/AX-/AT-SPI-
  Schnittstellen für externe Clients bereit. Windows besitzt jetzt den unten
  beschriebenen Adapter; Linux und macOS benötigen weiterhin eigene Adapter.
- Windows stellt Table/Grid sowie TableItem/GridItem mit vollständigen
  Spaltenheaderbeziehungen bereit. Die unabhängigen Clientprüfungen stehen unten.
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
gemeinsamen Unterbau. Externe native Tabelleninterfaces benötigen eigene Nachweise.

## Windows Table/Grid und eingebettete Textbereiche

Der Windows-Adapter stellt Tabellen mit `IGridProvider`/`ITableProvider` und
Zellen mit `IGridItemProvider`/`ITableItemProvider` bereit. Zellen liefern
vollständige Unicode-Namen, Koordinaten, Spannweiten, die zugehörige Tabelle
und Spaltenüberschriften. `IScrollItemProvider` verwendet den vorhandenen
Scrollauftrag; ausgeblendete oder deaktivierte Fenster weisen ihn ab.
Tabellen und Zellen beanspruchen keine der 256 interaktiven Controlplätze.
Ein schwacher Providercache erhält gehaltene COM-Identitäten und entfernt
unreferenzierte Provider beim letzten Release.

Der Textbaum folgt Dokument → Tabelle → Zellen → eingebettete Links.
`RangeFromChild` übernimmt die vollständigen Zellgrenzen. Verschiedene leere
virtuelle Zellen am selben Textoffset behalten ihre jeweilige umschließende
Identität; ein Link mit genau demselben Textbereich bleibt das innerste Objekt.
Dies folgt dem [Microsoft-Vertrag für GetChildren](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextrangeprovider-getchildren).
Rahmen folgen Viewport, Fensterposition und Skalierung. Gehaltene Provider
melden typisierte Änderungen für Rahmen, Sichtbarkeit und Aktivierung;
unveränderte Veröffentlichungen erzeugen keine solchen Ereignisse.

Unter Windows 10.0.26200 / MSVC ASan / SDL 3.2.30 bestehen **2/2** gemeinsame
Modellfälle (`run-8630dofv`) und **3/3** GUI-Fälle (`run-_6dstox4`), einschließlich
der direkten COM-Tabellenprobe. Diese prüft Unicode, 1.536 Zeichen lange
Zellnamen, mehrere virtuelle Leerzellen, Header, Textbaum, Offscreen-Scrollen,
Reflow, Suspend, deaktivierte Fenster, Providerfreigabe und alte Referenzen.
Der erweiterte unabhängige Client besteht mit Lese-, Feld- und Lebenszyklusfällen
**5/5** GUI-Fälle (`run-cg7zdj3y`). Die abschließende Clientfassung ergänzt
tatsächliche Bildschirm-Punktsuche und die Ruheprüfung der Ereignisse:
normal **1/1** (`run-ctvz3gmc`), Fokusstress mit zehn Zyklen **1/1** (`run-a5mrovfo`).
Beide tatsächlichen Fokusbilder wurden geöffnet und visuell geprüft.

Nach Dokumentwechsel liefern der alte Elementname und `GetItem(0,0)` im
externen Client `UIA_E_ELEMENTNOTAVAILABLE` ohne alte Daten. Der Windows-
Clientgetter `get_CurrentRowCount` lieferte dagegen reproduzierbar `S_OK` und 0.
Die Prüfung akzeptiert deshalb dort entweder unavailable oder exakt diesen
leeren Wert; eine alte Zeilenzahl bleibt unzulässig. Direkte Provideraufrufe
prüfen weiterhin unavailable. Diese beobachtete Clientbesonderheit ist keine
allgemeine Zusage für andere UIA-Versionen.

Die Gegenprobe verwendet den bisherigen Hauptzweig `188a411` und denselben
neuen unabhängigen Client. Nach ausdrücklich dokumentiertem Einlesen der
185 Quelldokumente erreicht sie innerhalb der unveränderten Bereitschaftsgrenze
den prüfbaren Zustand und scheitert gezielt an `missing embedded Table/Grid`.
Nachweis: `build/native/uia-table-before-counter-warm`. Der vorherige kalte
Lauf überschritt die Bereitschaftsgrenze und ist ausdrücklich kein semantischer
Gegennachweis. Beide Originalausgaben bleiben erhalten.

**Offen bleibt eine sporadische Windows-Reaktionsstörung:** `run-d2tfasmb`
scheiterte normal am 25-Sekunden-Timeout des ungültigen Linkaufrufs und im
Stressfall am nicht rechtzeitig sichtbaren, bereits fokussierten Link.
Die nachfolgenden erfolgreichen Läufe belegen keine beseitigte Ursache.
Die Diagnose speichert jetzt Teil-stdout/-stderr bei Subprozess-Timeouts und
optionale Client-Stufenmarken. Die Zeitgrenzen wurden nicht erhöht.

Der aktuelle Linux-Gegencheck besteht **11/11** native Fälle (`run-xoy6w368`)
und **3/3** GUI-Fälle (`run-l8apwl85`): Inline-Inhalte, Schriftbedarf und
unabhängige AT-SPI-Links. Dies ist noch kein AT-SPI-Table-Nachweis.
Der Testläufer-Selbsttest bestätigt erwartete Fehler, Timeouts und vollständige
Berichte unter `build/native/uia-tables-runner-proof`.

## Tatsächlich ausgeführte macOS-Modellprüfung

Der [macOS-Lauf auf d644453](https://github.com/Lulus792/physim/actions/runs/38768319618)
besteht auf ARM64 **11/11** native Fälle (`run-b5ub62vt`) und **13/13** GUI-Fälle
(`run-cutxpnap`), auf Intel ebenfalls **11/11** (`run-q_g5jt8r`) und **13/13**
(`run-81q278pc`). Beide Joblogs und beide tatsächlichen 16-Pixel-Inlinebilder
wurden geprüft: Überschriften und Hervorhebung sind lesbar, `l|r` bleibt in
einer Zelle. Die vorhandene Reflowprobe erhält die Zellidentität bei 16/32/16
Pixeln. Dieser Lauf bestätigt das gemeinsame Modell und bestehende AppKit-
Fälle; ein nativer AX-Tabellenadapter ist weiterhin offen.
