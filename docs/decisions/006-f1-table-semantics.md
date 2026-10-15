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
  beschriebenen Adapter; Linux besitzt inzwischen den ebenfalls unten geprüften
  AT-SPI-Adapter. Der native macOS-Adapter bleibt offen.
- Windows stellt Table/Grid sowie TableItem/GridItem mit vollständigen
  Spaltenheaderbeziehungen bereit. Die unabhängigen Clientprüfungen stehen unten.
  Grundlage: [Microsoft Table Control Type](https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-supporttablecontroltype).
- Linux stellt AT-SPI Table und TableCell einschließlich außerhalb des
  Viewports liegender Zellen bereit. Die tatsächlich ausgeführten Nachweise
  stehen unten. Grundlage: [AT-SPI Table](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Table.html).
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

## Linux Table/TableCell mit unabhängigen Clients

Der AT-SPI-Baum enthält Tabellen als Dokumentkinder, Zellen als direkte
Tabellenkinder und eingebettete Links als Zellkinder. Die weiterhin geordneten
Hypertext-Linkbereiche beziehen sich auf denselben vollständigen Dokumenttext.
Table liefert Dimensionen, Header, Zellzugriff und die bijektive Zuordnung von
Zeile/Spalte zu Kindindex. TableCell ergänzt Position, Spannweiten, Tabelle und
Headerobjekte. Nicht unterstützte Auswahl bleibt leer beziehungsweise false.
Vollständige Zellnamen werden aus dem unveränderlichen Text gelesen und sind
nicht auf die Länge interaktiver Controlbeschriftungen begrenzt.

Die sparsamen Modell-IDs bleiben auch bei Scrollen und Aus-/Einblenden stabil.
`Component.ScrollTo` nimmt für Zellen TOP_LEFT, TOP_EDGE und ANYWHERE an;
andere Platzierungen werden mit false abgewiesen. Verborgene Fenster behalten
Namen/Identitäten, liefern leere Rahmen und weisen Scrollen ab. Bildschirm- und
Elternkoordinaten berücksichtigen Tabellen-/Dokumenteltern. Punktsuche löst
sichtbare Zellflächen und darin liegende Links auf. Nur Tabellenwurzeln und
tatsächlich abgefragte Zellen benötigen zusätzliche Ereignissnapshots;
unveränderte Zustände bleiben ruhig. Dokumentwechsel melden alte abgefragte
Objekte defunct und machen ihre RPC-Aufrufe ungültig.

Eine konkrete Protokollabweichung ist berücksichtigt: Die aktuelle
[TableCell-XML](https://raw.githubusercontent.com/GNOME/at-spi2-core/main/xml/TableCell.xml)
nennt für `GetRowColumnSpan` einen zusätzlichen booleschen Wert, der tatsächlich
verwendete [libatspi-Client](https://raw.githubusercontent.com/GNOME/at-spi2-core/main/atspi/atspi-table-cell.c)
liest vier Ganzzahlen (`iiii`). Der reale Lauf `run-yxn462pk` wies `biiii`
ausdrücklich zurück. Provider und Introspektion liefern deshalb `iiii`;
PyAT-SPI und der zusätzliche direkte D-Bus-Aufruf prüfen dieselben vier Werte.

Tatsächlich ausgeführt unter Alpine 3.24 / GCC 15.2 ASan/UBSan, SDL 3.2.30,
X11/Xvfb/Openbox und Software-GL:

- **11/11** native Accessibility-/Dokumentfälle (`run-mzw3j43r`), einschließlich
  Modell, Unicode-Index, Parser und 185 Seiten/214 Abschnittslinks.
- Der reale F1-Client besteht **1/1** (`run-klona1c6`) und erneut im Verbund
  `run-xxht8o3_`: Table/TableCell, Header, Koordinaten, vollständiger Zelltext,
  verschachtelte Links, tatsächliche Punktsuche und alte Referenzen.
- Dieser Verbund besteht **5/6** GUI-Fälle: allgemeiner AT-SPI-Fensterclient,
  Inline-Inhalte, vollständiges F1-Lesen, Suchfeldbedienung und F1-Links.
  Der neue Tabellenfixture-Fall scheiterte an seiner falschen Endgrenze hinter
  dem letzten Zeilenumbruch. Die korrigierte Grenze entspricht dem bestehenden
  Modellvertrag, nicht einer Lockerung der Produktvalidierung.
- Der korrigierte Fixture-Verbund besteht **2/2** (`run-xrhgh8lx`): bisheriger
  Fenster-/Text-/Edit-Client und neue 3×3-Tabelle mit Unicode-Header, 1.536 Zeichen
  langem Zellnamen und zwei verschiedenen virtuellen Leerzellen am selben Offset.
  Scrollen, verborgene Rahmen, erneutes Anzeigen, Header/Koordinaten und defunct
  werden unabhängig geprüft. Der vorherige `run-xesmgf_1` bestand 1/2 wegen
  eines Wrapper-Rückgabetyps im Testvergleich; die Prüfung liest jetzt ausdrücklich
  Breite/Höhe und bestätigt beide mit null.
- Die abschließende Clientfassung besteht **1/1** (`run-5b958ln5`) und ergänzt
  tatsächlichen Fensterumzug, passende Bildschirmrahmen und Ereignisruhe.

Der anfängliche `run-2j5s_g15` fand mit ASan ein zu großes temporäres
Kindarray auf dem kleinen D-Bus-Threadstack. Die Implementierung führt jetzt
die bereits geordneten Inventare ohne dieses Array zusammen; die folgenden
Client- und Sanitizerläufe erreichen die genannten Prüfungen erfolgreich.
Die tatsächlichen Linux-Inline- und Fokusbilder aus `run-xxht8o3_` wurden
geöffnet und visuell geprüft.

Die Gegenprobe baut ausschließlich den vorherigen AT-SPI-Provider aus `e6773c5`
in dieselbe aktuelle Tabellenfixture ein. Der unabhängige Client erreicht das
Dokument und scheitert mit Exit 1 gezielt an der fehlenden nativen Tabelle.
Quelle, Hash, Binärdatei, stdout/stderr und Bericht liegen unter
`build/native/Mac-fields-linux/table-before` und
`build/native/atspi-table-before-counter`. Dies ist ein abgegrenzter
Provider-Gegennachweis; praktische Orca-Aufgaben und Wayland-Abnahme fehlen weiter.

## Gemeinsame Rahmen für native Zeilen und Spalten

Als Vorbereitung der AX-Anbindung können native Adapter zusätzlich die
beschnittenen Zeilen- und Spaltenrahmen abfragen. Eine Zeile verwendet ihren
vollständigen Layoutbereich, eine Spalte die Vereinigung ihrer sichtbaren
Zellbereiche. Virtuelle Leerzellen benötigen dabei weiterhin keinen dichten
Speicher. Suspend liefert leere Rahmen; ungültige Indizes verändern die
Ausgabe des Aufrufers nicht.

Die erweiterte gemeinsame Tabellenprobe wurde direkt mit den normalen
Builder-Compilerflags aus `tests/test_accessibility_tables.c` und
`app/accessibility.c` gebaut und tatsächlich ausgeführt: Windows MSVC ASan
besteht mit Exit 0. Linux GCC 15.2 ASan/UBSan mit eingeschaltetem LeakSanitizer
besteht ebenfalls mit Exit 0.
Sie prüft Clipping, eine anfangs unsichtbare Zeile, die Spaltenvereinigung
nach einer Geometrieänderung, Indexfehler und Suspend. Originalausgaben und
Quellhashes stehen unter `build/native/table-axis-windows` beziehungsweise
`build/native/table-axis-linux`. Das ist ein gemeinsamer Modellnachweis;
die neue AX-Implementierung benötigt davon getrennte native Laufzeitnachweise.

## AppKit-Tabellenadapter und ausstehende native Abnahme

Der implementierte AppKit-Baum bildet Dokument → Tabelle → Zeilen → Zellen →
Links ab. Tabellen liefern zusätzlich Spaltenobjekte, Koordinatenzugriff und
Überschriftenbeziehungen. Grundlage sind die öffentlichen Verträge für
[NSAccessibilityTable](https://developer.apple.com/documentation/appkit/nsaccessibilitytable),
[NSAccessibilityRow](https://developer.apple.com/documentation/appkit/nsaccessibilityrow?changes=_2)
und [Zellindexbereiche](https://developer.apple.com/de/documentation/appkit/nsaccessibility-c.protocol/accessibilitycolumnindexrange).
Zellen verwenden die Modell-ID; native Zeilen/Spalten haben eine davon
unterschiedene Identität aus Tabelle und Index. Die gemeinsamen Rahmen
vermeiden eine zweite Geometrieberechnung im Adapter.

Objekte entstehen bei Abfrage und bleiben pro Inhaltsversion kanonisch,
außerhalb der interaktiven Controls. Dokumentwechsel und Schließen lösen den
Bestand und die Parent-Beziehungen auf; gehaltene alte Objekte werden ungültig.
Direkt geerbte Press-/Fokus-/Wertaktionen sind ausdrücklich gesperrt. Zellnamen
bleiben vollständige Textbereiche, auch jenseits der Controllabel-Kapazität.
Geteilte UTF-16-Bereiche beziehen sich weiterhin auf das Dokument, auch wenn
ein Link eine Zelle als Parent erhält. Die öffentliche Text-Reveal-Anforderung
verwendet den normalen versionierten Scrollauftrag. Verborgene Dokumente
bleiben lesbar, liefern aber keine sichtbare Zellgeometrie oder Bedienaktion.

Die neue native Fixture prüft eine dünn besetzte 3×3-Tabelle: Header,
Koordinaten, Unicode samt Surrogatgrenzen, 1.536 Zeichen lange Zellen,
verschiedene virtuelle Leerzellen, Parents, Textbereiche, Glyphenrahmen,
Punktsuche, Reveal, Reflow, Verbergen und gehaltene Referenzen nach Versionswechsel
und Schließen. Die zusätzliche reale F1-Probe sucht die 2×2-Tabelle über das
Cocoa-contentView und öffentliche Accessibility-Selectoren. Sie prüft den
vollständigen Zelltext `l|r in Metern; C`, Header, verschachtelten Link und
sichtbare Geometrie samt Punktsuche. Der Testläufer verlangt beide F1-Marker.

Diese Implementierung und ihre Prüfdefinitionen sind noch kein erfolgreicher
macOS-Lauf. Tatsächliche Objective-C-/AppKit-Aufrufe müssen auf Intel und ARM64
bestehen. Die F1-Probe läuft innerhalb der App; externe AX-Clients und praktische
VoiceOver-Aufgaben bleiben getrennte offene Abnahmen.

Die lokalen Nachbarn bestehen unter Windows MSVC ASan **4/4** native
Modell-/Dokumentfälle (`run-m9mar7rd`) und **3/3** GUI-Fälle (`run-9abxq7o4`):
nativer Provider, unabhängiger UIA-Linkclient und Inline-Inhalte. Linux GCC 15.2
ASan/UBSan mit SDL 3.2.30, X11/Xvfb/Openbox besteht **4/4** native
(`run-vmv_25d7`) und **2/2** GUI-Fälle (`run-m14rjkp7`): Inline-Inhalte und
unabhängiger AT-SPI-Linkclient. Der Testläufer besteht seine erwarteten
Fehler-/Timeoutfälle unter `build/native/mac-tables-runner-proof`.
Die abschließende native Linkprüfung bestätigt 185 Seiten und 214 Abschnittslinks.
Diese Nachbarn kompilieren den neuen Apple-Zweig nicht und ersetzen daher
keinen tatsächlichen macOS-Nachweis.

Der vorherige gemeinsame Rahmenstand `717fbae` besteht im
[macOS-Lauf](https://github.com/Lulus792/physim/actions/runs/38786156143)
auf ARM64 **11/11** native (`run-ui198hee`) und **13/13** GUI-Fälle
(`run-n0xtym6r`), auf Intel **11/11** (`run-sw1jbjkp`) und **13/13**
(`run-rc1tt6pk`). Beide tatsächlichen Joblogs wurden gelesen; auch dieser
Nachweis gilt ausschließlich für die genannte vorherige Revision.
