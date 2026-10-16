# F1 Handbuch und Abnahme

Entscheidung vom 13. Oktober 2026: F1 soll sowohl den ersten abgeschlossenen
Versuch ermöglichen als auch beim Arbeiten präzise Antworten liefern. Die
Abnahme betrachtet Inhalt, Bedienung, Darstellung und zugängliches Lesen.
Ein grüner Parsertest allein ist keine vollständige Abnahme.

## Aktuelle Bewertung vom 16. Oktober 2026

Bewertungsgrundlage sind die zentralen Skills unter `D:/Skills/skills`:
`documentation-engineer`, `ui-ux-designer`, `accessibility-engineer`,
`ui-implementation`, `graphics-engineer` und `test-engineer`. Volle Punktzahl erfordert die acht
unten beschriebenen Abnahmekriterien; fehlende Prüfungen gelten als offen.

| Bereich | Belegter Stand | Rest zur vollständigen Abnahme |
| --- | --- | --- |
| Dokumentationszugang | 185 registrierte Seiten und 218 interne Abschnittslinks geprüft; Navigation, Verlauf und Suche mit realen F1-Abläufen geprüft | Vollständige Plattformabnahme derselben Revision |
| Inhalt und Aufgabe | Lernwege, Beispiele, Referenz und Fehlerbehebung vorhanden; ausgewählte Quellen-/CLI-Abläufe ausgeführt; vier Resamplingverfahren einschließlich PCHIP mit API, Implementierung und acht Tests je Windows/Linux abgeglichen | Gesamtabgleich sämtlicher Anleitungen mit dem Projektstand |
| Lesbarkeit und Bedienung | 16/32-Pixel-Schrift, kleines Fenster, drei Themes, Codekopieren, Links und Tastaturführung durch Laufzeitbilder und Verhalten belegt | Tatsächliche DPI-/Monitorwechsel und vollständige Zustandsmatrix |
| Formelsatz | 193 TeX-Ausdrücke auf 22 Seiten; 197 Displayblöcke und zwei Inlinepassagen unter Windows/Linux geprüft; macOS ARM besteht alle 25 GUI-Fälle samt 162 Inline-Bildprüfungen, Intel 24/25 mit Zeitlimit nach 46 Inline-Ansichten | Vollständige Intel-Abnahme nach Verringerung redundanter Testübergänge, weitere Fachseiten, echte Dichtewechsel und strukturierte mathematische Screenreader-Navigation |
| Barrierefreiheit | Windows UIA, Linux AT-SPI und macOS AppKit ergänzen vollständigen Lesetext, Suchfelder und interne Linkbereiche; alle drei Plattformen besitzen zusätzlich geprüfte native Tabellen-, Zell- und Headerbeziehungen | Externe macOS-AX-Clientprüfung, echte Screenreader-Aufgaben und die unten beschriebene sporadische Windows-Linkprüfung |

Die Hilfe ist in den geprüften Windows- und Linux-Abläufen gut nutzbar. Eine vollständige
Bewertung mit Höchstpunktzahl ist wegen dieser offenen Abnahmefälle noch nicht
begründet. Die folgenden Nachweise nennen jeweils ihre Revision und Umgebung.
Die konservative Gesamtwertung bleibt **UI 8/10, Dokumentationsaufgabe 8/10
und Barrierefreiheit 5/10**. Der Formelsatz besitzt jetzt auch Inline-Brüche,
Wurzeln, Hoch-/Tiefstellungen und Matrizen mit gemeinsamer Textgrundlinie;
seine technische Abnahme ersetzt die offenen Nutzeraufgaben nicht.

### Sidebar-Aktionen ohne Wortbruch

Die bisherigen 32-Pixel-Bilder auf macOS Intel und ARM64 zeigen „Themen“ in
zwei Zeilen. Die neue Anordnung misst beide Beschriftungen mit der tatsächlich
aktiven Schrift und berücksichtigt Innenabstand, Rand und Rundung des normalen
Button-Zeichenwegs. Reicht eine halbe Zeile nicht aus, stehen „Themen“ und
„Inhalt“ untereinander. Die Fokuskennungen und Aktivierungswege bleiben erhalten.

Der erweiterte echte Sidebar-Fall prüft beide Beschriftungen bei 16 und 32 Pixeln
in Dunkel, Hell und hohem Kontrast. Ein unabhängiger Blick auf die gezeichneten
Textkommandos verlangt jeweils eine vollständige, ungeclippte Textzeile. Enter
wechselt zu den Themen, ein tatsächlicher Pointerklick zum Inhalt; Editorquellen
und Simulationszeit bleiben unverändert. Die bisherigen Prüfungen mehrzeiliger
Kapitelüberschriften und sichtbarer Abschnittssprünge bleiben zusätzlich erhalten.
Die Windows-MSVC-ASan-Gruppe besteht **3/3** (`run-nqf4wnzx`): Sidebar,
Navigation und Inline-Inhalte. Nach der abschließenden Anordnung ohne leere
Zwischenzeile besteht dieselbe Windows-Gruppe erneut **3/3** (`run-yexqjinq`).
Linux GCC 15.2 ASan/UBSan mit X11/Xvfb/Openbox besteht **2/2** native Fälle
(`run-l41lxcbd`) und **3/3** GUI-Fälle (`run-jf46d3n3`). Dort wechseln die
32-Pixel-Labels tatsächlich in die einspaltige Anordnung und bleiben in allen
drei Themes vollständig. Alle drei 32-Pixel-Themebilder der Linux-Probe und der neuen
Sidebar-Aktionsprobe wurden geöffnet und visuell geprüft.

Die vorhandene Windows-Anordnung bestand die neuen Draw-Prüfungen schon mit
gebündelter Schrift (`run-qf65m_hk`); das ist kein negativer Gegennachweis für
den auf macOS beobachteten Fehler. Die neue macOS-Gegenprobe baut deshalb nur
die alte Zweispalten-Anordnung in denselben aktuellen Test ein und verlangt
die konkrete Ablehnung eines mehrzeiligen 32-Pixel-Labels. Quellen, Hashes,
Originalausgaben und Bilder werden dafür gesichert. Die tatsächliche
macOS-Ausführung dieser Korrektur und Gegenprobe steht noch aus.

### Native macOS-Tabellenabnahme auf Revision 474ccd8

Die [Tabellenentscheidung](006-f1-table-semantics.md) enthält jetzt tatsächliche
AppKit-Nachweise für Tabellen, Zeilen, Spalten, vollständige Zelltexte, Header
und eingebettete Links. Intel und ARM64 bestehen jeweils **11/11** native und
**13/13** GUI-Fälle im [macOS-Lauf](https://github.com/Lulus792/physim/actions/runs/38789867159).
Beide Originalarchive und Joblogs sowie die tatsächlichen Fokus-/Tabellenbilder
wurden geprüft. Die F1-Probe findet die native Tabelle über das Cocoa-contentView;
sie ist eine In-Prozess-Prüfung und ersetzt keinen externen AX-Client oder VoiceOver.
Windows besteht lokal vier native und drei GUI-Nachbarn; Linux-CI besteht mit
GCC und Clang jeweils 26 native und 34 GUI-Fälle. Die genauen Laufkennungen
stehen in ADR 006.

Die 32-Pixel-Bilder zeigen einen konkreten UI-Restbefund: Die Sidebar-Aktion
„Themen“ bricht bei geringer Breite mitten im Wort um. Die Layoutabnahme bleibt
deshalb neben den bereits offenen Monitor-/DPI- und assistiven Aufgaben offen.
Der aktuelle zusätzliche Windows-Inhaltsabgleich besteht 26/26
Dokumentations-/Referenzfälle (`run-usbfqz70`). Die gesonderten tatsächlich
ausgeführten Tutorial-/Batch-Fälle bestehen **12/12** (`run-z84kqlpm`),
einschließlich C-/Physim-Modellen, gemischten Auswertungen, analytischen
Ergebnisprüfungen, 1.536 Monte-Carlo-Läufen und 1.024 Batch-Archiven. Die Quellen-
und Ausführungsprüfungen ergänzen den Inhaltsnachweis; sie ersetzen keinen
vollständigen fachlichen Textabgleich aller 185 Seiten.

### Fortschritt der Tabellenabnahme

Die [Tabellenentscheidung](006-f1-table-semantics.md) dokumentiert jetzt den
Windows-Table/Grid-Adapter mit vollständigen Zellnamen, Spaltenüberschriften,
Koordinaten und verschachtelten Textbereichen. Der unabhängige Client besteht
den normalen Lauf und zehn Fokuszyklen einschließlich Bildschirm-Punktsuche,
Fensterbewegung, Ereignisruhe und abgewiesenen alten Referenzen. Ein separater
Gegenlauf mit dem bisherigen Hauptzweig erkennt gezielt die fehlende Table/Grid-
Schnittstelle. Originalausgaben und die Einschränkung des vorherigen kalten
Gegenlaufs sind in ADR 006 festgehalten.

Die sporadische Windows-Fokus-/Timeoutstörung bleibt ausdrücklich offen:
`run-d2tfasmb` scheiterte, die abschließenden Läufe `run-ctvz3gmc` und
`run-a5mrovfo` bestanden. Die Diagnose bewahrt nun auch Teilsausgaben bei
Timeouts; eine beseitigte Ursache ist damit nicht nachgewiesen.

Das gemeinsame Tabellenmodell auf `d644453` besteht unter macOS ARM64 und
Intel jeweils **11/11** native und **13/13** GUI-Fälle. Beide Joblogs und
tatsächlichen Inlinebilder wurden geprüft. Die Windows-Erweiterung hat einen
zusätzlichen Linux-Gegencheck mit **11/11** nativen und **3/3** GUI-Fällen.
Die jeweiligen Laufkennungen und Plattformgrenzen stehen in ADR 006.
Linux besitzt inzwischen ebenfalls den in ADR 006 beschriebenen AT-SPI-
Table/TableCell-Adapter. Der aktuelle Sanitizerstand besteht 11/11 native Fälle;
die fünf bisherigen GUI-Nachbarn sowie der korrigierte spezielle Tabellenclient
bestehen mit getrennt dokumentierten Laufkennungen. Der Client prüft Unicode,
vollständige lange Namen, verschiedene virtuelle Leerzellen, Header, Scrollen,
Fensterumzug, verborgene Rahmen, Ereignisruhe und alte Referenzen. Die Gegenprobe
weist gezielt den vorherigen Provider ohne native Tabelle ab. Beide tatsächlichen
Linux-F1-Bilder wurden geprüft. Die nachfolgend dokumentierte native AX-
Tabellenabnahme ergänzt diesen Stand; praktische Screenreader-Aufgaben fehlen weiter.
Die konservative Gesamtwertung **UI 8/10, Dokumentationsaufgabe 8/10,
Barrierefreiheit 5/10** bleibt deshalb bestehen; historische Reviews behalten
ihre unten genannte Revision.

Der [nachfolgende macOS-Lauf auf e6773c5](https://github.com/Lulus792/physim/actions/runs/38776320863)
besteht auf ARM64 **11/11** native (`run-_nnhtdgr`) und **13/13** GUI-Fälle
(`run-rykxdim5`). Intel besteht **11/11** native (`run-gy8w2pkb`) und **12/13**
GUI-Fälle (`run-z8vwe4sf`): `documentation_math` überschreitet die unveränderte
45-Sekunden-Grenze beim ersten Appstart. Die Teilspur erreicht erst gegen Ende
dieser Zeit das Linkinventar; eine abgeschlossene Math-Abnahme dieses Laufs
liegt nicht vor. Die übrigen Intel-GUI-/AppKit-Fälle bestehen. Beide Joblogs
wurden gelesen. Der Fehler bleibt offen; der erfolgreiche frühere Lauf auf
`d644453` ersetzt diesen fehlgeschlagenen aktuellen Nachweis nicht.

Der [macOS-Lauf auf e8042e4](https://github.com/Lulus792/physim/actions/runs/38779190950)
besteht danach auf Intel **11/11** native (`run-swyuqlz1`) und **13/13** GUI-Fälle
(`run-7l_i0mga`), auf ARM64 **11/11** (`run-1x3k2uz_`) und **13/13**
(`run-sn100w33`). Der vorherige Timeout gilt damit weiterhin als sporadischer
Befund mit offener Ursache. Der [Linux-CI-Lauf derselben Revision](https://github.com/Lulus792/physim/actions/runs/38779190162)
besteht mit GCC **26/26** native (`run-rpwdeeai`) und **34/34** GUI-Fälle
(`run-i3kxedvu`), mit Clang **26/26** (`run-bs28t7f7`) und **34/34**
(`run-yuqdth8h`). Beide enthalten den unabhängigen TableCell-Client.
Alle vier Joblogs wurden eingesehen. Der geprüfte Tabellenstand ist in beide
Hauptzweige übernommen; eine danach bearbeitete Schriftoptimierung benötigt
eigene Nachweise.

Die Schriftoptimierung auf `14cfe8d` besitzt inzwischen diese tatsächlichen
[macOS-Nachweise](005-f1-font-demand.md): auf Intel und ARM64 jeweils 11/11
native und 13/13 GUI-/AppKit-Fälle sowie bytegleiche vollständige Atlanten im
anschließenden Paarvergleich. Die frühere sporadische Bereitschaftsstörung wird
dadurch weiterhin nicht als ursächlich behoben eingeordnet.
Der [Linux-Lauf von 14cfe8d](https://github.com/Lulus792/physim/actions/runs/38782921283)
besteht mit GCC 26/26 native (`run-fq4gfdbb`) und 34/34 GUI-Fälle
(`run-hraws8y2`), mit Clang 26/26 (`run-8e5gtzid`) und 34/34
(`run-s7rvzh1o`). Beide vollständigen Joblogs wurden gelesen. Die geprüfte
Schriftoptimierung ist damit ebenfalls in beide Hauptzweige übernommen.

### Zentraler Skill-Review auf Revision 56a59ed

Nachtrag: Die [gemeinsamen Tabellen-Zellgrenzen](006-f1-table-semantics.md)
und die Registrierung der ADRs 005/006 bestehen unter Windows MSVC ASan sowie
Linux GCC ASan/UBSan jeweils **3/3** native und **4/4** GUI-Fälle. Der native
Themen-/Abschnittstest bestätigt jetzt **185 Seiten und 214 Abschnittslinks**.
Die Gegenprobe weist den alten Renderer am fehlenden `|` in Inline-Code ab.
Native Tabellenrollen, Zell-/Headerbeziehungen und Screenreader-Aufgaben
sind damit weiterhin nicht abgenommen. Die folgende Gesamtwertung bleibt bestehen.

Die erneute Auswahl mit `D:/Skills/skills/scripts/skill_system.py select` liefert
`documentation-engineer`, `ui-ux-designer` und `accessibility-engineer`.
Angewendet wurden deren Einstiegsdateien, Fachabläufe und relevante Wissensdateien,
insbesondere die Desktop-Abnahmematrix. Die Wertung bleibt ein konservatives
fachliches Urteil über die Gesamtaufgabe, keine normierte Messung.

| Gegenstand | Wertung | Befund und verbleibende Abnahme |
| --- | --- | --- |
| UI und Nutzerführung | 8/10 | Überschriften, Hervorhebungen und umbrochene Links sind in den aktuellen Linux-Dichtebildern lesbar. Der Bedarfstest und der Dichterundlauf erhalten Inhalt und logische Schriftgröße. Die Navigation beansprucht bei kleinen Fenstern weiterhin viel Lesefläche; tatsächliche Monitorwechsel und die vollständige Plattform-Zustandsmatrix sind offen. |
| Aufgabe als Dokumentation | 8/10 | Der User Guide nennt Voraussetzungen, einen abgeschlossenen Pendelversuch und prüfbare Ergebnisse. Lernwege, Referenz und symptomorientierte Fehlerhilfe unterstützen unterschiedliche Aufgaben. Die aktuellen Schrift-/GUI-Tests ersetzen den weiterhin offenen vollständigen Abgleich aller Anleitungen mit der Implementierung nicht. |
| Barrierefreiheit | 5/10 | Native Lese-, Feld-, Link- und Tabellenbeziehungen sind für die jeweils unten genannten Revisionen geprüft. Der unabhängige Linux-Linkclient besteht erneut. Externe macOS-AX-Clientprüfung, praktische Screenreader-Aufgaben und die zuvor beobachtete Windows-Fokussporadik bleiben offen. |

Nachweise der Schriftschnittkorrektur: MSVC ASan besteht **6/6** native Schriftfälle
(`run-mcsyv9gx`), die finale Collection-Probe **1/1** (`run-ecnofoaj`) und
**5/5** benachbarte GUI-Fälle (`run-ihvg_vqi`): Atlasbesitz, zehnfacher
Windows-Linkstress, Zeicheninventur, Schriftbedarf und Dichte.
Der abschließende Stand `56a59ed` unter Alpine 3.24 / GCC 15.2 mit
ASan/UBSan, X11/Openbox und Mesa 26.1.6 llvmpipe
besteht **6/6** native Schriftfälle (`run-1btunwdm`) und **6/6** GUI-Fälle
(`run-1fm1nn8e`): Atlasbesitz, Grafik, Zeicheninventur, Schriftbedarf, Dichte
und unabhängige AT-SPI-Linkbedienung. Beide 1520 × 1080-Pixel-Dichtebilder
dieses Linux-Laufs wurden geöffnet und visuell geprüft.
Die Ergebnisse liegen unter `build/native/Debug-sanitized/test-results`
beziehungsweise `build/native/Mac-fields-linux/test-results`.

Der zunächst wartende macOS-Lauf für `56a59ed` wurde nach der geprüften
Tabellenkorrektur durch die umfassendere Revision `188a411` ersetzt. Deren
[tatsächlich abgeschlossener macOS-Lauf](https://github.com/Lulus792/physim/actions/runs/38656494042)
besteht mit LLVM 20 ASan/UBSan und SDL 3.2.30 auf Darwin 24.6.0:
Intel **10/10 native und 13/13 GUI-/AppKit-Fälle** (`run-cnzci5pl`,
`run-31ypm8tz`), ARM64 ebenfalls **10/10 und 13/13** (`run-140_le02`,
`run-cpzf2jd5`). Beide Originalarchive und Joblogs wurden gelesen.
Die tatsächlichen System-Collections liefern auf beiden Architekturen
Helvetica normal **Index 0**, fett **Index 1**, Menlo **Index 0**. Zeicheninventur,
Bedarf, Dichte, Inline-Inhalte und native Lese-/Feld-/Linkfälle bestehen.
Je Architektur wurden die tatsächlichen Inline-, Mathematik- und Dichtebilder
geöffnet und geprüft. Originale und Bildkonvertierungen liegen unter
`build/native/F1-review/mac-font-faces-{arm,intel}.zip` und
`build/native/F1-review/mac-font-faces-images` im Hauptcheckout.
Dieser Nachweis bestätigt die Schriftschnitt- und Bedarfs-/Dichtekorrektur auf
`188a411`; das danach ergänzte gemeinsame Tabelleninventar benötigt seine
eigene macOS-Prüfung. Externe AX-, Screenreader- und vollständige
Monitor-/DPI-Abnahmen bleiben ebenfalls offen.
Die folgenden älteren Reviews und Zwischenstände behalten ihre jeweilige Revision.

### Aktueller zentraler Skill-Review auf Revision 2ccd520

Am 14. Oktober 2026 wurden die Skills erneut unter `D:/Skills/skills`
ausgewählt und angewendet: `documentation-engineer`, `ui-ux-designer`,
`accessibility-engineer` und `test-engineer`. Grundlage sind deren Fachabläufe
und die verbindliche Desktop-Abnahmematrix. Dieser Review trennt die Qualität
der Anleitung von der Bedienoberfläche und der assistiven Bedienbarkeit.

| Gegenstand | Fachliche Wertung | Aktueller Befund |
| --- | --- | --- |
| UI und Nutzerführung | 8/10 | Lernwege, Inhaltsnavigation, Verlauf, Suche und reine Leseansicht sind nachvollziehbar. Die aktuelle Windows-Prüfung bestätigt Zoom, Navigation und umbrochene Inhaltsaktionen. Bei 760 × 540 und 32-Pixel-Text nimmt die Navigation viel Lesefläche ein; echte DPI-/Monitorwechsel und die vollständige Plattform-Zustandsmatrix bleiben offen. |
| Aufgabe als Dokumentation | 8/10 | Der User Guide führt zu einem gespeicherten Pendellauf mit Ergebnisprüfung und kontrollierter Änderung. Beide Sprachwege bieten sieben Kapitel, vollständige Beispielquellen und Referenz-/Fehlerhilfe. Der aktuelle Test bestätigt 183 erreichbare Seiten, 214 Abschnittslinks und 1.264 aufgelöste API-Symbole. Ein vollständiger inhaltlicher Abgleich sämtlicher Anleitungen mit der Umsetzung ist weiterhin nicht nachgewiesen. |
| Barrierefreiheit | 5/10 | Windows und Linux besitzen geprüfte native Lese-, Feld- und Linkzugänge; macOS besitzt jetzt auch geprüftes vollständiges AppKit-Lesen. Native macOS-Feld-/Linktextbereiche, Tabellensemantik, externe macOS-AX-Clientprüfung und praktische Screenreader-Aufgaben bleiben offen. Deshalb wird noch keine vollständige assistive Abnahme erteilt. |

Die Zahlen bleiben ein konservatives fachliches Urteil über die gesamte
Abnahme, keine normierte Messung. Die früher festgestellten Font- und
Inhaltsaktionsfehler sowie der fehlende macOS-Lesetext sind inzwischen durch
die unten dokumentierten Änderungen und ihre begrenzten Prüfungen bearbeitet.
Die historischen Tabellen beschreiben jeweils ihren damaligen Stand.

Für diesen erneuten Review tatsächlich ausgeführt, jeweils Exitcode 0:

- Windows 10.0.26200, MSVC ASan, SDL 3.2.30:
  `run-3k0yru_1` besteht **26/26** ausgewählte Dokumentations-/API-Fälle.
  Enthalten sind Parser, veröffentlichte C-/Physim-Quellen, ausgeführte
  Beispielmodule, Lernwege, Referenz und native Abschnittsprüfung.
- Derselbe aktuelle Windows-Build: `run-2t_vy0bx` besteht **3/3** GUI-Fälle
  `documentation_zoom`, `documentation_navigation` und `documentation_sidebar`.
  Die neuen Bilder der Inhaltsnavigation bei 16 Pixeln, des hohen Kontrasts
  bei 32 Pixeln und der reinen Leseansicht wurden visuell geprüft. Der Fokus
  und die mehrzeilige aktive Inhaltsaktion sind vollständig sichtbar.
- Der [ausgeführte macOS-Hauptzweiglauf](https://github.com/Lulus792/physim/actions/runs/38620637921)
  auf derselben Quellrevision besteht auf Intel und ARM64 jeweils **3/3**
  Modell- und **5/5** GUI-/AppKit-Fälle. Die vollständigen Joblogs wurden
  eingesehen: Intel `run-iqr2n3zc` / `run-j22xsx17`, ARM64
  `run-1oxnxay5` / `run-nz38ynye`. Dies bestätigt die ausgewählten Fälle auf
  Darwin 24.6.0 und ersetzt keine externe AX- oder VoiceOver-Abnahme.

Die lokalen Ergebnisdateien und neuen Bilder liegen im ignorierten Verzeichnis
`build/native/F1-uia-sanitized/test-results`; die Bildkonvertierungen für den
Review unter `build/native/F1-review/current-review-images`. Geprüft wurde die
Quellrevision vor dieser reinen Berichtergänzung. Priorität für die weitere
Umsetzung haben native macOS-Suchfelder und Linktextbereiche, Tabellensemantik,
praktische Screenreader-Aufgaben, echte DPI-/Monitorwechsel und der verbleibende
Inhaltsabgleich. Die Gesamtaufgabe erhält noch keine 10/10.

### Erneuter Skill-Review des lokalen Stands

Geprüft am 14. Oktober 2026 auf Windows 10.0.26200 mit dem Debug-Build unter
`build/native/F1-review`, Ausgangsrevision `38efa9d`. Die Auswahl erfolgte über
`D:/Skills/skills/scripts/skill_system.py select`; gelesen und angewendet wurden
`documentation-engineer`, `ui-ux-designer`, `accessibility-engineer` und
`test-engineer` aus diesem zentralen Bestand einschließlich ihrer Einstiegsdateien
und der Desktop-Abnahmematrix. Die Wertung ist ein fachliches Urteil über die
vorliegenden Belege, keine normierte Messung oder Produktzertifizierung.
10/10 setzt vollständige Erfüllung und Prüfung der relevanten Abnahmekriterien
voraus; fehlende Umgebungen und Prüfungen erhalten keine Erfolgspunkte.

| Gegenstand | Bewertung | Begründung und Grenze |
| --- | --- | --- |
| UI und Nutzerführung | 8/10 | Getrennte Lernwege, lokale/globale Suche, Verlauf, Codekopieren und Tastaturführung funktionieren in den geprüften Windows-Aufgaben. 32-Pixel-Text bleibt bei 760 × 540 bedienbar; Dunkel, Hell und hoher Kontrast besitzen geprüfte Bilder. Die kompakte Ansicht lässt allerdings wenig Lesefläche. Vollständige Schriftabdeckung, tatsächliche DPI-/Monitorwechsel und sämtliche Plattformzustände fehlen. |
| Aufgabe als Dokumentation | 8/10 | Einstieg führt zu einem gespeicherten Versuch und einer prüfbaren Erwartung. Sieben Kapitel je Lernweg, acht gemeinsame Modellaufgaben, vollständige Beispielquellen, API-Verträge und symptomorientierte Fehlerhilfe unterstützen Lernen und Nachschlagen. 183 Seiten und 214 Abschnittslinks sind erreichbar. Der Gesamtabgleich aller Texte mit der Umsetzung ist offen; der Review findet konkrete veraltete Aussagen. |
| Barrierefreiheit | 5/10 | Externe Windows-UIA-Clients lesen Dokumenttext, bedienen Suchfelder und Links und prüfen den Fensterlebenszyklus. Das ist ein belastbarer Teilnachweis. Echte Narrator-/NVDA-/VoiceOver-/Orca-Aufgaben, native Tabellensemantik und vollständige Text-/Feld-/Linkanbindung auf macOS/Linux bleiben offen. |

Für die Inhaltsaufgabe wurden User Guide, beide Lernwege, C-Experimenttutorial,
API-Einstieg und Fehlerbehebung gegen ihre Quellen und vorhandenen Prüfungen
betrachtet. Der User Guide nennt Voraussetzungen, konkrete Bedienaktionen,
erwartete Dateien/Kurven und eine anschließende kontrollierte Änderung. Die
Referenz ergänzt Signaturen, Einheiten, Lebensdauer und Fehlerverträge.
Planung und implementierter Stand werden im Handbuchstart ausdrücklich getrennt.
Der Quellenvergleich bestätigt identische veröffentlichte Beispiele; er bedeutet
nicht, dass jedes Modell in diesem Review vollständig ausgeführt wurde.

Aktuell tatsächlich ausgeführte Belege, jeweils Exitcode 0:

- `run-o7656q5r`: 27/27 native Inhalts-/Referenzfälle. Darunter 1.264 aufgelöste
  API-Symbole, 31 Referenzdokumente, beide Lernwege, Beispielquellenabgleich,
  ausgeführte C-/Physim-Bewegungsbeispiele sowie 183 Seiten und 214 Abschnittslinks.
- `run-8eh1g5p7`: 5/5 GUI-Fälle für Tastatur, Verlauf, Zoom, Inline-Inhalte und
  den echten Windows-F1-Fensterlebenszyklus. Die neuen Bilder des kleinen
  32-Pixel-Lesers, der hellen Suche und der Wiederöffnung wurden visuell geprüft.
- `run-u5xoqydr`: 3/3 externe Windows-UIA-Appfälle für Suchfelder,
  interne Links und vollständigen Dokumenttext.

Die erweiterte Zeichen-Gegenprobe `run-m3m9o5rx` scheitert mit Exitcode 1.
Sie findet U+1D40, neun Rahmenzeichen, U+2603 sowie U+1F30D und U+1F600 in
allen 38 Fontvarianten ohne verwendbare Glyphen. Die geladenen Bereiche
schließen diese Codepunkte aus; das Wiederöffnungsbild zeigt `θ?` statt `θ🌍`.
Die experimentelle Probe meldet außerdem U+0020, weil sie für jedes druckbare
Zeichen sichtbare Tinte verlangt. Dieser Leerzeichenbefund ist ein Prüferfehler
und zählt nicht als fehlende Schriftglyph. Die bestehende mathematische
43-Zeichen-Prüfung belegt folglich keine vollständige Zeichendarstellung.

Zwei konkrete Inhaltskorrekturen folgen aus dem Review: `docs/accessibility.md`
behauptete noch, Windows/UIA sei nicht angebunden, und führte es nochmals pauschal
als offene Aufgabe. Der Text beschreibt jetzt den tatsächlich geprüften Umfang
und dessen Grenzen. Der Einstieg von `docs/physim-guide.md` nennt nun den
Entwicklungsstand `0.1.0-dev`, passend zu README und Sprach-SDK, statt einer
historischen Versionsangabe. Weitere historische Nachweise bleiben als solche
erhalten. Priorität haben anschließend Schriftfallback, praktische assistive
Nutzeraufgaben, Tabellensemantik und die native Desktop-Abnahmematrix.
Nach den Inhaltskorrekturen bestehen Parser, Lernwege, Offline-Abschnittslinks
und API-Referenz erneut 4/4 mit Exitcode 0 in `run-9e2tghw0`.

### Bewertung nach Schrift- und Rasterkorrekturen

Der erneute Review betrachtet den lokalen Stand `efe152c` mit denselben vier
zentralen Fachskills. Die frühere Wertung bleibt nachvollziehbar bei **8/10 für
UI/Nutzerführung, 8/10 für die Dokumentationsaufgabe und 5/10 für
Barrierefreiheit**. Das ist ein fachliches Urteil, keine standardisierte
Punkteskala. Die damalige fehlende Zeichenabdeckung ist inzwischen für alle
167 druckbaren Zeichen der registrierten Seiten behoben; sie zählt nicht mehr
als aktueller Mangel. Weitere Schriftsysteme bleiben außerhalb dieses Nachweises.

Direkt betrachtet wurden die tatsächliche 2×-Aufnahme aus `run-4qsmr6yd` sowie
die kleine 32-Pixel-Ansicht, die helle Suche und das offline gezeichnete
Unicode-/Rahmenzeichenbild aus `run-vylai6qc`. Überschriften, starke Hervorhebung,
Code, Links und Suchfokus sind unterscheidbar. Bei 760 × 540 und doppelter
Textgröße bleibt die Oberfläche bedienbar, doch Navigation und Werkzeugzeilen
lassen wenig Lesefläche. Eine kompaktere Navigation ist deshalb weiter ein
konkreter Gestaltungsbedarf. Die 2×-Aufnahme bestätigt den getesteten
Rasterpfad; sie ersetzt keine tatsächliche Windows-Inhaltsskalierung.

Die vorhandenen Ergebnisdateien wurden erneut geprüft: `run-au995z_a` enthält
7/7 bestandene native MSVC-ASan-Fälle, `run-vylai6qc` 6/6 bestandene reale
F1-/UIA-Fälle und `run-2qbh1dw6` 4/4 bestandene Dokumentations-/API-/Fontfälle.
Das verschobene Release-Paket bestätigt 538 Dateien einschließlich zwölf
Font-/Lizenzdateien und den tatsächlichen Dichte-Rundlauf. Diese Kontrolle
ist eine Sichtung vorhandener Ausführungsbelege, kein neuer Testlauf.

Die Inhaltsstichprobe am User Guide und an der Fehlerhilfe zeigt weiterhin
Voraussetzungen, konkrete Aktionen, überprüfbare Ergebnisse und nächste Schritte.
Offen bleiben der Gesamtabgleich aller Anleitungen, echte DPI-/Monitorwechsel,
native Text-/Feld-/Linkbereiche und Tabellen auf allen Zielplattformen sowie
praktische Screenreader-Aufgaben. Der Linux-Lauf für `efe152c` wurde unter
Workflow `38511888866` gestartet und war bei dieser Bewertung noch in Arbeit;
für diese Revision wird hier kein Linux-Erfolg behauptet. macOS bleibt offen.

### Linux-Suchfelder mit Unicode-Auswahl und Zwischenablage

Der nächste geprüfte Stand ergänzt auf Grundlage von `004c408` die beiden
F1-Suchfelder als native AT-SPI-Entries. Die zentralen Skills
`linux-specialist`, `accessibility-engineer` und `test-engineer` bestimmen
Protokollumfang, Gegenproben und Plattformgrenzen; der Dokumentationsreview
verwendet zusätzlich `documentation-engineer` und `ui-ux-designer`.

Der bestehende gemeinsame Feldweg übernimmt native Werte, Auswahl, Caret,
Fokus und horizontales Sichtbarmachen auch unter Linux. `Text` verwendet
Unicode-Skalarpositionen. `EditableText.InsertText` verwendet eine Skalarposition
und eine UTF-8-Bytelänge; ein Präfix innerhalb eines Mehrbyte-Codepunkts wird
abgewiesen. Die sechs Änderungs- und Zwischenablageoperationen folgen dem
[offiziellen EditableText-Vertrag](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.EditableText.html).
Versionierte Aufträge werden beim Besuch des weiterhin gültigen Felds
übernommen. SDL-Zwischenablageaufrufe erfolgen im UI-Thread ohne gehaltene
Provider-/Modellmutexe; anschließend wird das Ziel erneut geprüft.

Die unabhängige Zwei-Fenster-Probe prüft `A θ🌍 B`, Skalaranzahl, Auswahl,
Caret, Einfügen/Löschen, Kopieren/Ausschneiden/Einfügen, einen degenerierten
Caret-Bereich beim Sichtbarmachen, Kapazitätsgrenzen sowie gesperrte und
entfernte Ziele. Die tatsächliche F1-Probe setzt beide Suchwerte nativ,
markiert `θ🌍`, ersetzt diese Auswahl durch echte Tastatureingabe und verlässt
das Feld per Tab. Editorinhalt und Simulationszeit bleiben erhalten.

Die erste Testfassung setzte die zusätzliche Feldprobe zu früh vor einer
bestehenden Cache-Gegenprobe ein; außerdem wählte der F1-Client bei einem
gleichlautenden Namen die statische Beschriftung statt des Eingabefelds.
Die Prüfungen berücksichtigen jetzt Phase sowie Namen und Rolle. Eine feste
Zwischenablage-Wartezeit wurde durch Warten auf die beobachtete Annahme des
nächsten Auftrags ersetzt. Diese Prüferkorrekturen sind keine Produktfehler.

Tatsächlich ausgeführte abschließende Belege, jeweils Exitcode 0:

- Alpine 3.24.2/WSL2, GCC 15.2.0, SDL 3.2.30, X11/Xvfb/Openbox,
  ASan/UBSan: **4/4** in `run-t6_3pm5r` unter
  `build/native/F1-linux/Debug-sanitized/test-results`. Fälle:
  `accessibility_atspi`, `documentation_navigation`,
  `accessibility_document_atspi`, `accessibility_edit_atspi`.
- Windows 10.0.26200, MSVC ASan: **5/5** in `run-634jnjyo` unter
  `build/native/F1-uia-sanitized/test-results`. Geprüft wurden F1-Tastatur,
  native Suchfelder, Fensterlebenszyklus, vollständiger Lesetext und Navigation.
- Testkatalog, erwartete Gegenfehler und Timeout-/Reportverhalten bestehen
  unter `build/native/F1-review/atspi-edit-runner-proof`.

Die drei tatsächlichen Linux-F1-Bilder aus dem vorherigen ebenfalls bestandenen
Lauf `run-h8wgw4_k` wurden visuell betrachtet: gesetzter Dokument-Suchwert,
markierte Unicode-Auswahl und fokussierte globale Suche sind sichtbar.
Die zusätzliche Fensterfixture bestätigt Textsemantik, keine vollständige
Schriftabdeckung. Der Ubuntu-Lauf für den vorherigen Lesetext-Commit `004c408`
ist ebenfalls abgeschlossen: Workflow `38521829659`, Clang und GCC jeweils
**24/24 native und 28/28 GUI-Fälle**. Das belegt diese vorherige Revision;
für die neue Feldanbindung wird daraus kein Ubuntu-Erfolg abgeleitet.

Der anschließende Ubuntu-Lauf für den Suchfeld-Commit `e4a7bbf` ist inzwischen
ebenfalls tatsächlich abgeschlossen: Workflow `38596181683`, GCC-Job
`115963388120` mit **24/24 native und 29/29 GUI-Fällen** (`run-coinkji_`,
`run-yprdpcua`), Clang-Job `115963388839` mit ebenfalls **24/24 und 29/29**
(`run-ykddijmi`, `run-wulbufgf`). Die Originalprotokolle beider abgeschlossenen
Jobs wurden eingesehen; `accessibility_edit_atspi` besteht in beiden.

Die bisherigen fachlichen Wertungen **8/10 UI, 8/10 Dokumentationsaufgabe und
5/10 Barrierefreiheit** bleiben eine konservative Bewertung der Gesamtaufgabe.
Linux-Suchfelder und vollständiges Linux-Lesen zählen jetzt als geprüfter
Fortschritt. Native macOS-Text-/Feldanbindung, Linux-/macOS-Linktextbereiche,
Tabellensemantik, praktische Screenreader-Aufgaben, tatsächliche
DPI-/Monitorwechsel und der vollständige Inhaltsabgleich bleiben offen.

### Linux-Links im vollständigen Lesetext

Die nächste Erweiterung verbindet interne F1-Links mit dem vollständigen
AT-SPI-Lesetext. `Hypertext` liefert Linkanzahl, Links in Lesereihenfolge und
den Linkindex an einer Unicode-Position. Ein separates `Hyperlink` liefert
Zieladresse, Start-/Endposition, Gültigkeit und das bedienbare Link-Control.
Offscreen-Links bleiben im Inventar; Fokus macht ihren tatsächlichen Textbereich
sichtbar. Link-Controls nutzen die bereits geprüften gemeinsamen Fokus- und
Aktivierungsaufträge. Die URI wird ohne Kürzung in den kopierten Snapshot
übernommen. Ungültige UTF-8-Ziele und übergroße Ziele werden abgewiesen.

Grundlage sind die offiziellen Verträge für
[Hypertext](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Hypertext.html)
und [Hyperlink](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Hyperlink.html).
Die native Gegenprobe findet zwei Integrationsfehler: Ein gemeinsamer Objektpfad
für Accessible und Hyperlink liefert aus dem libatspi-Cache den falschen Objekttyp
(`run-bf3e4jsb`). Separate Hyperlink-Objektpfade beheben dies. Die XML-Dokumentation
nennt für `NAnchors` den Typ `n`; libatspi fordert tatsächlich `i`
(`run-rhp252mc`). Die Anbindung veröffentlicht deshalb den wirksamen 32-Bit-Typ,
auch in ihrer eigenen Introspektion. Dieser Unterschied wurde am
[offiziellen libatspi-Quelltext](https://raw.githubusercontent.com/GNOME/at-spi2-core/main/atspi/atspi-hyperlink.c)
und am tatsächlich ausgeführten Client überprüft. Das zwischenzeitlich versuchte
`HyperlinkImpl` ist kein von diesem Client unterstütztes AT-SPI-Interface und
wurde entfernt.

Die unabhängige F1-Probe bestätigt zwei interne Links einschließlich eines
Tabellenlinks nach Unicode-Text. Sie prüft exakte Textbereiche und URIs,
ungültige Link-/Ankerindizes, Offscreen-Fokus und Glyphengeometrie,
fehlgeschlagene Navigation mit erhaltenem Quelltext, erfolgreiche Navigation
und danach ungültige alte Hyperlinkobjekte. Quellenadressen bleiben die
dokumentierte Kopieraktion. Der negative Lauf mit dem vorherigen App-Build
scheitert bereits an der fehlenden Hypertext-Schnittstelle
(`build/native/F1-linux/link-before.log`).

Die negativen D-Bus-Anfragen werden ausdrücklich direkt geprüft, weil der
Python-Wrapper bei einzelnen nativen Fehlern einen leeren Rückgabewert liefert.
`run-k7h37ltc` findet diese falsche Prüferannahme bei ungültigen Indizes;
`run-ajd6pxw7` erreicht alle positiven Linkaufgaben, scheitert aber noch an
derselben Annahme bei einem alten Hyperlink. Der abschließende unabhängige
Client verwendet für beide Gegenproben native D-Bus-Fehlernamen. Er besteht
mit demselben App-Build und Exitcode 0 unter
`build/native/F1-linux/links-final-client`; `results.json` und das echte
App-Endsignal bestätigen erfolgreiche Navigation und unveränderten Editor/
Simulationszustand. Die gehaltene Hyperlinkreferenz liefert nach Navigation
`org.a11y.atspi.Error.Defunct`.

Die finale Produktion besteht zusätzlich vier benachbarte GCC-ASan/UBSan-
GUI-Fälle in `run-ajd6pxw7`: allgemeines AT-SPI, F1-Navigation, vollständiger
Lesetext und native Suchfelder. Der historische Gesamtlauf bleibt korrekt
als 4/5 erhalten; der anschließend korrigierte Linkclient ist ein eigener
erfolgreicher Ausführungsbeleg. Die drei Windows-GUI-Nachbarn für Links,
Lebenszyklus und Suchfelder bestehen mit MSVC ASan in `run-br9upxjo`.
Die drei Modellfälle für Dokument, Fensterpause und Editieren bestehen
dort in `run-2kyziyfi`; der ergänzte Linkziel-/Besitzfall besteht auch direkt
mit GCC ASan/UBSan einschließlich Leakprüfung
(`build/native/F1-linux/atspi-links-model.log`). Der Provider besteht GCC
`-Wall -Wextra -Werror -fsyntax-only`; Testläufer und Kataloggegenproben
bestehen in `build/native/F1-review/atspi-links-runner-proof`.

Die drei finalen Linux-Aufnahmen wurden unmittelbar geprüft: Der zuvor
verborgene Tabellenlink trägt sichtbaren Fokus, die ungültige Navigation
zeigt eine lesbare Fehlermeldung im Handbuch und die erfolgreiche Navigation
öffnet den tatsächlichen C-Lernweg. Diese Erweiterung ersetzt keine praktische
Orca-Abnahme oder native Tabellensemantik. macOS-Text-/Feld-/Linkbereiche,
Screenreader-Aufgaben, echte DPI-/Monitorwechsel und vollständiger
Inhaltsabgleich bleiben offen; eine Gesamtwertung mit 10/10 wird nicht erteilt.

### Vollständige Kapitelüberschriften im Inhaltsverzeichnis

Die Inhaltsnavigation verwendete noch feste 44-/36-Pixel-Zeilen und einzeilige
Buttons. Die zuletzt geprüfte F1-Aufnahme des C-Lernwegs zeigt abgeschnittene
Kapitelüberschriften. Die tatsächliche Gegenprobe am unveränderten Zeichenweg
bestätigt bei 760 × 540 und 16-Pixel-Schrift: ein Textkommando, unvollständiger
Titel, 185 Pixel Breite und 36 Pixel Höhe. Dieser Lauf endet bewusst mit
Exitcode 1 (`build/native/F1-review/sidebar-before.log`).

Der Titel des Inhaltsverzeichnisses erhält jetzt seine gemessene Umbruchhöhe.
Abschnittsaktionen verwenden denselben mehrzeiligen Zeichenweg wie die bereits
umgebrochene Themenliste. Ihre Höhe folgt Text, Schrift und verfügbarer Breite.
Die Schaltfläche behält ihren vollständigen nativen Namen und die vorhandenen
Tastatur-/Mausaktionen. Der tatsächliche erste Kapiteltitel wird bei 16 Pixeln
in zwei Zeilen auf 60 Pixel Höhe, bei 32 Pixeln auf 92 Pixel Höhe gezeichnet.

Der Prüfer liest vor der Rendererfreigabe die tatsächlichen Textkommandos und
ihre Cliprechtecke. Er vergleicht den vollständigen Titel und prüft die
gezeichneten Zeilen unabhängig von der Höhenberechnung. Durch Umbruch entfallene
Leerzeichen werden für diesen Vergleich ausgelassen; alle übrigen UTF-8-Bytes
bleiben verbindlich. Die reale App-Probe erreicht den Abschnitt mit Tab/Enter
und später mit einem Pointerklick, erhält Editor und Simulationszeit und prüft
16/32 Pixel, Dunkel, Hell und hohen Kontrast. Die anschließende strengere Probe
fordert zusätzlich sichtbare Glyphen des ersten und letzten Titelzeichens im
Zielabschnitt statt lediglich einer veränderten Scrollposition.

Tatsächlich ausgeführte Belege mit Exitcode 0:

- Windows 10.0.26200, MSVC ASan: **4/4** in `run-jrk9l21l` für F1-Tastatur,
  native Links, Lesemodus-Navigation und die neue Inhaltsnavigation.
- Alpine 3.24.2/WSL2, GCC 15.2.0, SDL 3.2.30, X11/Xvfb/Openbox,
  ASan/UBSan: **4/4** in `run-__qmt9_z` für Lesemodus-Navigation, native
  Suchfelder, native Links und die neue Inhaltsnavigation.
- Die strengere Abschnittsprobe besteht unter Windows mit MSVC ASan
  **1/1** in `run-mble_6g9` und unter Linux mit GCC ASan/UBSan
  **1/1** in `run-ncohyi1h`.
- Testkatalog und erwartete Gegenfehler bestehen unter
  `build/native/F1-review/sidebar-runner-proof`.

Die vier Windows-Aufnahmen und die Linux-Aufnahmen bei 16 Pixeln sowie
32 Pixeln in Hell/hohem Kontrast wurden unmittelbar betrachtet. Der fokussierte
Kapiteltitel ist vollständig lesbar; seine Aktion bleibt innerhalb der
scrollbaren Navigation erreichbar. Diese Prüfung bestätigt keine echte
DPI-/Monitorwechselmatrix oder praktische Screenreader-Bedienung.
Die zusätzlichen Abschnittsbilder beider Plattformen zeigen außerdem die
vollständige Zielüberschrift nach Enter bei 16 Pixeln und nach dem Pointerklick
bei 32 Pixeln; sie wurden ebenfalls unmittelbar geprüft.

Auch die vorherige Linux-Linkrevision `fd38710` besitzt inzwischen tatsächlich
abgeschlossene Ubuntu-Belege: Workflow `38599913386`, GCC-Job `115976191559`
mit **24/24 nativen und 30/30 GUI-Fällen** (`run-5p6vnm6e`, `run-jqijs7j2`),
Clang-Job `115976192069` mit ebenfalls **24/24 und 30/30** (`run-3nfsp5ui`,
`run-ijkr4dx2`). Die Originalprotokolle beider Jobs wurden eingesehen;
`accessibility_links_atspi` besteht in beiden. Das ist ein Nachweis dieser
vorherigen Revision, keine vorweggenommene Ubuntu-Abnahme der neuen Sidebar.

### Fokus nach Theme-Reflow mit breiterer Systemschrift

Die Ubuntu-Gegenprobe zu `36a12ef` besteht zwar alle 24 nativen Fälle,
scheitert aber mit GCC und Clang im letzten High-Contrast-Schritt der neuen
Sidebar-Probe: **30/31 GUI-Fälle**, Workflow `38602604785`, Jobs `115985346834`
und `115985346665` (`run-e6a1jatk`, `run-5b4b1fnf`). Die Originalprotokolle und
das GCC-Artefakt wurden eingesehen. Mit der dort verwendeten breiteren
DejaVu-Schrift besitzt der Kapiteltitel drei Zeilen auf 130 Pixel Höhe.
Beim Kontrastwechsel bricht zusätzlich der Button `Themen` um; dadurch wandert
die bereits fokussierte Kapitelaktion unter die Clipgrenze. Die Aufnahme
bestätigt tatsächlich abgeschnittenen Text, keinen bloßen Prüferfehler.

Eine lokale Gegenprobe installiert DejaVu 2.37-r6 unter Alpine und macht die
unveränderten Paketfonts an der bereits vorgesehenen Systemfont-Adresse
verfügbar. Der vorhandene App-Build reproduziert denselben Fehler im letzten
Theme-Schritt (`build/native/F1-linux/sidebar-dejavu-before.log`). Das erklärt,
warum die vorherige lokale Probe mit Noto-Fallback bestanden hatte; ihre Belege
bleiben auf diese Umgebung begrenzt.

Nach Anwenden neuer Darstellungseinstellungen wird das Sichtbarmachen des
aktiven F1-Tastaturziels erneut angefordert. Die vorhandene Fokusprüfung
berechnet den nötigen Scrollversatz erst am neuen Layout. Sie erhält das Ziel
und senkt keine Clip-/Vollständigkeitsanforderung. Der erste Schritt mit dem
breiteren Font bleibt jetzt auch nach dem Themewechsel vollständig sichtbar.

Abschließend tatsächlich ausgeführt, jeweils Exitcode 0:

- Alpine/DejaVu, GCC ASan/UBSan: **3/3** in `run-jebn_meu` für Zoom,
  Lesemodus-Navigation und die strengere Sidebar-/Abschnittsprobe.
- Windows, MSVC ASan: **3/3** in `run-u9dmvnrf` für dieselben Fälle.

Die finale Linux-Kontrastaufnahme wurde unmittelbar geprüft; alle drei
Titelzeilen liegen im sichtbaren Fokusrahmen. Die Ubuntu-Nachprüfung dieser
Korrektur steht noch aus. Praktische Screenreader-, macOS- und echte
DPI-/Monitorwechsel-Abnahme werden durch diese Reflow-Korrektur nicht behauptet.

### Explizite Schriftschnittwahl in macOS-Collections, 15. Oktober 2026

Die Ursache der auf Intel beobachteten identischen normalen und fetten Schrift
ist eingegrenzt: Beide Rollen laden `Helvetica.ttc`; der bisherige Bake wählt
stets Collection-Index 0. Die Konfiguration trägt jetzt einen expliziten
Collection-Index, den Bake und Quell-Cmap-Prüfung verwenden. Der macOS-Loader
wählt einen nicht kursiven Schnitt anhand des OpenType-`head`-Bold-Bits;
Microsoft dokumentiert die [Bold-/Italic-Bits](https://learn.microsoft.com/en-us/typography/opentype/spec/head).
Fehlt ein passender System-Schnitt, bleibt der bisherige Offline-Fallback
verfügbar. Windows und Linux behalten ihre vorhandenen primären Fontdateien.

Der neue native Fall `font_faces` erzeugt eine Collection aus bereits verfolgten
Schriftdateien. Index 0 enthält keine lateinischen Buchstaben, Index 1 ist als
fett/kursiv markiert, Index 2 enthält den echten fetten Schnitt. Die Auswahl
muss Index 2 liefern, dessen Glyphen, Metriken und Rasterdeckung der separat
geladenen Bold-Datei entsprechen. Die Fallback-Prüfung muss dabei die Cmap des
ausgewählten Schnitts verwenden. Ungültige Indizes, drei Allokationsfehler,
fehlende Stiltreffer und vollständige Freigabe werden zusätzlich geprüft.

MSVC ASan besteht `run-mcsyv9gx` **6/6** native Schriftfälle und anschließend
`run-ecnofoaj` **1/1** für die finale Collection-Probe. Der absichtliche Modus
`--wrong-face` erzwingt Index 0 und wird mit Exit 1 an der tatsächlichen
Glyphenprüfung abgewiesen; Originale in `build/native/font-faces-wrong`.
Unter macOS prüft derselbe Fall zusätzlich die tatsächlichen Helvetica- und
Menlo-Collections, verschiedene Helvetica-Indizes und unterscheidbare
Latein-Rasterdeckung. Die oben dokumentierte Ausführung auf `188a411` besteht
inzwischen auf Intel und ARM64 mit den tatsächlichen Helvetica-/Menlo-Collections.

Die finale Linux-Ausführung besteht unter GCC 15.2 ASan/UBSan **6/6** native
Schriftfälle (`run-1btunwdm`) und **6/6** GUI-Fälle (`run-1fm1nn8e`), einschließlich
vollständiger Zeicheninventur, Dichte, Bedarf und unabhängigem AT-SPI-Linkclient.
Die benachbarte Windows-GUI-Prüfung besteht **5/5** (`run-ihvg_vqi`). Der
Testläufer mit Katalog- und erwarteten Fehlergegenproben besteht in
`build/native/font-faces-runner-proof`.

### F1-Schriften nach Bedarf, 15. Oktober 2026

Die [Architekturentscheidung 005](005-f1-font-demand.md) ist jetzt mit der
Größenwahl verbunden. F1 berechnet zunächst fünf benötigte Varianten und
ergänzt neue Größen beim Zoom oder bei getrennten UI-/Code-Einstellungen.
Bestehende Atlanten bleiben dabei gültig. Ein Dichtewechsel ersetzt den Cache
erst nach vollständigem Erfolg. Alle bisherigen Zeichen und 38 möglichen
Varianten bleiben erhalten; die vollständige Inventur baut sie weiterhin mit
System- und mitgelieferten Schriften auf.

Der App-Test prüft abgebrochene Zoom-/Einstellungsänderungen, erhaltene Handles,
GL-Kontext, unveränderte Einstellungsdatei, erfolgreichen neuen Versuch,
Cachewiederverwendung und den Dichtewechsel eines bereits gefüllten Caches.
Fehler-, Einstellungs- und Dichteaufnahmen wurden tatsächlich geprüft.
Windows MSVC ASan besteht **29/29** F1-/Einstellungs-/native GUI-Fälle in
`run-17g6yjzd`; die Erweiterung des gefüllten Dichtefalls besteht danach
**1/1** in `run-scefp_ru`. Beide Fälle mit gemeinsamer PNG-/Textpixelprüfung
bestehen **2/2** in `run-trrn7fj7`. Linux GCC 15.2 ASan/UBSan mit X11/Openbox
und Software-GL besteht **10/10** in `run-zaz1o0wk`, einschließlich unabhängiger
AT-SPI-Lese-, Feld- und Linkbedienung.

Drei folgende vollständige unabhängige Windows-Linkläufe erreichen F1 nach
**5,093 / 7,266 / 5,609 Sekunden**. Der F1-Bake liegt bei
**0,555714 / 0,708286 / 0,572180 Sekunden**. Die bisherigen Zeitgrenzen und
Zeichenprüfungen bleiben erhalten. Originalberichte und Fehlergegenproben
sind in 005 aufgeführt. Die neue macOS-Ausführung sowie die unterscheidbare
Fettschrift auf macOS bleiben offen; erfolgreiche Windows-Wiederholungen
beweisen keine Behebung der historischen Fokussporadik. Die vollständige
F1-Abnahme und die Gesamtaufgabe bleiben unvollständig.

### Windows-Bereitschaft und kompakter Schriftatlas, 15. Oktober 2026

Die Untersuchung verwendet die zentralen Debugging-, Windows-, Performance-,
Grafik- und Test-Skills unter `D:/Skills/skills`. Die Linkprobe schreibt jetzt
auch im Fehlerfall `diagnostics.json`: letzter Abschnitt, beobachtete Marker,
Laufzeit, ursprünglicher Prozessstatus und eine etwaige Beendigung durch den
Prüfer. Die vorhandene App-Spur enthält zusätzlich Schriftquellen-, Atlas- und
Linkinventur-Zeitpunkte. Die Bereitschaftsgrenze bleibt 20 Sekunden, die Grenze
jedes externen UIA-Aufrufs bleibt 25 Sekunden.

`build/native/windows-link-diagnosis-07` reproduziert einen Bereitschaftsfehler:
Die Hauptschriften benötigen 8,274 Sekunden; beim Ablauf der Grenze ist der
F1-Schriftaufbau noch nicht abgeschlossen. Linkinventur und externer UIA-Client
sind noch nicht gestartet. Dieser konkrete Fehler liegt damit vor der nativen
Linkprüfung. Er erklärt nicht den früheren separaten UIA-Abbruch mit `E_FAIL`.
Ein kontrollierter früher Prozessabbruch bestätigt die neue Fehlerdiagnostik
in `build/native/windows-link-diagnostics-negative`.

Auf dem bisherigen RGBA-Pfad misst `run-8_f1g2at` 0,237/0,798 Sekunden für
Haupt-/F1-Schriftquellen und anschließend 4,472/6,857 Sekunden für Atlasaufbau,
Prüfung, Upload und Freigaben. Die größte gemessene Phase folgt dem Laden.
Revision `5c361d5` verwendet deshalb Nuklears vorhandenen Alpha8-Atlas direkt
als `GL_R8`-Textur. Der Textur-Swizzle liefert weißes RGB und die gespeicherte
Deckung als Alpha. Dies entspricht der bisherigen Nuklear-RGBA-Konvertierung;
Khronos dokumentiert genau diese
[Abbildung für Deckungstexturen](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_texture_swizzle.txt).
Zeichenvorrat, Größen, Oversampling und atomarer Atlaswechsel bleiben erhalten.
Ein Atlas mit 1024 × 32768 Pixeln benötigt dadurch 32 statt 128 MiB Pixeldaten;
die zusätzliche RGBA-Konvertierung entfällt.

Der GPU-Gegenvergleich rendert beide Formate mit demselben UI-Shader und
vergleicht alle Bildpunkte. Ungerade Zeilenbreite, Teildeckung und lineare
Filterung sind enthalten; unabhängige Sollwerte an Texelzentren verhindern,
dass zwei leere Ausgaben als Erfolg gelten. MSVC ASan besteht Grafik,
unabhängige UIA-Links und vollständige Zeicheninventur mit System- und
mitgelieferten Schriften in `run-ngr6so76` (**3/4**). Alle 167 benötigten Zeichen
in 38 F1-Varianten sind abgedeckt; die Unicode-/Rahmenaufnahme wurde geprüft.
Fünf native Schriftfälle bestehen separat in `run-duj0esy6` (**5/5**).

Die Linkbereitschaft liegt in diesem R8-Lauf bei 12,704 Sekunden gegenüber
14,984 Sekunden im unmittelbar vorherigen RGBA-Lauf. Wegen der gemessenen
Schwankungen ist dies kein allgemeiner Beschleunigungsnachweis. Der vierte
GUI-Fall, Dichtewechsel, überschreitet während eines parallelen Linux-Builds
sein internes Zeitlimit. Die isolierte Wiederholung `run-yq6xxrs6` besteht
**1/1**, einschließlich 1,25/1,5/2/1-Rundlauf und fehlerhaftem Atlaswechsel. Der
Plattform-Katalog samt absichtlichen Fehlergegenproben besteht in
`build/native/alpha-atlas-runner-proof`. Weitere Plattform- und
Wiederholungsnachweise werden erst nach tatsächlichem Abschluss ergänzt.
Alpine 3.24.2 / GCC 15.2 / ASan/UBSan besteht die fünf nativen Schriftfälle
in `run-s9jgztzu` (**5/5**) und unter X11/Openbox mit Software-GL Grafik,
Zeicheninventur, Dichtewechsel und unabhängige AT-SPI-Links in `run-6idpji8p`
(**4/4**). Das tatsächliche 1520 × 1080-Bild bei Dichte 2 wurde direkt geprüft.
Die entsprechende Windows-Aufnahme aus `run-yq6xxrs6` wurde ebenfalls geprüft.

Die isolierte Dreierserie in `build/native/alpha-atlas-link-series.json` erreicht
F1 nach 13,609/11,688/11,969 Sekunden. Zwei vollständige Linkläufe bestehen;
der dritte scheitert mit `Link lost focus after window movement` im Abschnitt
`window movement and quiet events`. Dabei sind Disable-/Enable-Ereignisse
bereits vorhanden. Dieser Befund ist genauer als der frühere ungeklärte
`E_FAIL`, beweist aber noch keine gemeinsame Ursache. Zusätzliche bedingte
Spuren erfassen gewährten Linkfokus und Blur-Eingaben. Damit bestehen
`run-ld8tgt_b` sowie drei Läufe in `build/native/focus-diagnosis-01` bis `-03`.
Das ist keine bestätigte Behebung des Fokusfehlers. Die tatsächliche macOS-
Prüfung auf Intel/ARM64 ist als
[Workflow 38645225826](https://github.com/Lulus792/physim/actions/runs/38645225826)
beauftragt, zum Zeitpunkt dieses Eintrags aber noch in der Runner-Warteschlange.
Die Windows-Sporadik und die vollständige F1-Abnahme bleiben offen.

#### Engere Atlas- und Fokusdiagnose

Die folgende Untersuchung erweitert ausschließlich die bedingte Diagnose und
den unabhängigen Prüfer. `run-pk_uwcx8` trennt jetzt die Atlasberechnung vom
anschließenden Prüfen und GPU-Upload: Hauptfenster **2,665310 / 0,013465**
Sekunden, F1 **6,324942 / 0,012661** Sekunden. Der dominante Rest liegt in der
CPU-Atlasberechnung. Ein weiterer R8-Lauf
`build/native/focus-state-03` scheitert nach 20,141 Sekunden erneut an der
Bereitschaft; die Schriften sind dort fertig, die Linkinventur endet erst
unmittelbar vor Ablauf der Grenze. R8 allein behebt die Startzeit-Sporadik
folglich nicht. Schriftumfang oder Prüffristen werden dafür nicht vermindert.

Die Fokusspur unterscheidet native Tastaturfokuswechsel, gewährte Linkaufträge,
beim Registrieren deaktivierte Links und bei der Publikation entfernte Ziele.
Ein erneuter Fokusfehler meldet zusätzlich, ob das Hostfenster tatsächlich im
Vordergrund, aktiviert und sichtbar ist. Die bisherigen neuen Spuren zeigen
noch keinen weiteren Fokusfehler; erfolgreiche Läufe allein bestätigen keine
Ursache des historischen Abbruchs.

`--stress-focus` lässt denselben unabhängigen UIA-Client zehn vollständige
Disable-/Enable-, Fokus-, Verschiebe- und Ruhezyklen im geöffneten F1-Fenster
ausführen. Jeder Zyklus verlangt neue Zustandsereignisse, verweigert Invoke
während der Deaktivierung, prüft die genaue 13/7-Pixel-Verschiebung und
abschließend weiterhin gesetzten Fokus. Der Wrapper verlangt alle zehn
erfolgreichen Zyklen und führt anschließend die ungültige und gültige
Navigation mit ihren bisherigen Kriterien aus. Die 20-/25-Sekunden-Grenzen
bleiben gleich. Der neue Windows-Katalogfall heißt
`accessibility_links_stress_windows`.

Tatsächlich ausgeführt:

- MSVC ASan: ursprüngliche Linkprobe `run-mn8mh97w`, `run-pk_uwcx8` und
  `run-yxfklw6y` jeweils **1/1**; zwei direkte vollständige Zehnfachläufe in
  `build/native/focus-stress-01` und `-02` bestanden.
- Gezeichnete Accessibility plus Katalog-Stresstest: `run-bheksqbs` **2/2**.
  Nach Ergänzung der Zykluszahl im Ergebnisbericht bestehen ursprüngliche
  Linkprobe und Stresstest erneut in `run-5on64pga` **2/2**.
- Plattform-Katalog und absichtliche Fehlergegenproben bestehen in
  `build/native/focus-stress-runner-proof`.
- Aktueller Linux-Nachbar mit GCC 15.2 ASan/UBSan, X11/Openbox und Software-GL:
  Grafik und unabhängige AT-SPI-Links in `run-xqlmqxlt` **2/2** bestanden.
- Zusätzlich wurden die beiden Joblogs des breiteren
  [Linux-CI-Laufs auf 5c361d5](https://github.com/Lulus792/physim/actions/runs/38645225859)
  ausgewertet: GCC **24/24** native und **31/31** GUI-Fälle
  (`run-_76dwsvg`, `run-0ftqfj6b`); Clang ebenfalls **24/24** und **31/31**
  (`run-lmmy1zch`, `run-rjy6pmkp`). Enthalten sind die F1-Zeichen-, Dichte-,
  Navigations-, Seitenleisten- und externen AT-SPI-Lese-/Feld-/Linkprüfungen.

Nächster belegbarer Leistungsansatz ist eine Änderung des CPU-Schriftaufbaus.
Die [Architekturentscheidung 005](005-f1-font-demand.md) vergleicht zwei
Besitzmodelle und legt die Anforderungen an den Aufbau nach Bedarf und dessen
Fehlerfälle fest. Der zusätzliche Atlasbesitz ist im Backend vorbereitet;
F1s Größenwahl wird erst im nächsten Schritt damit verbunden.
Sie muss alle Zeichen und Schriftvarianten, echte Rasterdichte, stabile
logische Metriken und den atomaren Fehlerfall erhalten. Weder diese Diagnose
noch die bestandenen Stresszyklen schließen die offene Windows-Sporadik oder
die übrigen F1-Abnahmen.

#### Zusätzliche macOS-Intel-Abnahme des R8-Pfads

Job `116130296964` aus dem oben verlinkten Workflow auf `5c361d5` ist inzwischen
tatsächlich abgeschlossen: LLVM 20 ASan/UBSan, SDL 3.2.30, OpenGL 4.1
APPLE-21.1.1 mit Apple Software Renderer. Der Modellverbund `run-c2latp21`
besteht **4/4**; der GUI-Verbund `run-65o9coyv` besteht **8/10**.
RGBA/R8-Bildpunkte sind auch auf diesem Renderer identisch. Gezeichnete
Accessibility, Navigation, Seitenleiste und native AppKit-Lese-/Feld-/Linkfälle
bestehen. Die tatsächlichen Linkfokus- und 32-Pixel-Lesemodusbilder wurden
geprüft. Das Originalarchiv liegt unter
`build/native/F1-review/alpha-atlas-mac-intel.zip`.

Die neu mitgeprüfte Zeicheninventur scheitert an nicht unterscheidbarer normaler
und fetter Schrift bei sämtlichen Größen. Der aktuelle macOS-Pfad wählt für
beide Rollen `Helvetica.ttc`; Nuklears aktueller Loader verwendet jeweils
Collection-Index 0. Die neu ausgeführte Prüfung bestätigt damit keine echte
fette Schriftschnittwahl auf macOS. Dieser eigenständige Fehler bleibt zu beheben.
Der Dichtetest überschreitet nach mehreren vollständigen Atlasaufbauten sein
internes Zeitlimit nach 61,240 Sekunden. Auch dieser Fall bleibt offen.
Der ARM64-Job `116130296671` ist inzwischen ebenfalls abgeschlossen und sein
Originalprotokoll wurde geprüft: **4/4** native Fälle (`run-ivgiwdal`) und
**8/10** GUI-Fälle (`run-o6kl4hf9`). Auch dort scheitert die Zeicheninventur an
normaler/fetter Schrift; der Dichtefall überschreitet sein internes Zeitlimit
nach **64,053 Sekunden**. Die übrigen acht Fälle bestehen. Diese Ergebnisse
gelten für `5c361d5`, vor Schriftbedarf und expliziter Collection-Auswahl.
Sie bestätigen keine Abnahme der neueren Korrekturen.

### Native macOS-Linkattribute, Ziele und Aktivierung

Die Erweiterung auf Quellrevision `589d7d4` verbindet interne F1-Linkbereiche
mit nativen AppKit-Linkobjekten. Verwendet wurden die zentralen macOS-,
Accessibility- und Test-Skills unter `D:/Skills/skills`. Der native
`NSAccessibilityLinkTextAttribute` verweist innerhalb des attributierten
Lesetextes auf denselben Link, der als Kind des Dokuments erreichbar ist.
Grundlage ist Apples [Linktextattribut](https://developer.apple.com/documentation/appkit/nsaccessibilitylinktextattribute).
Die Nativelemente liefern den vollständigen Beschriftungstext, relative URLs,
geteilte UTF-16-Textbereiche und sichtbare Glyphenrahmen. Die Kinder folgen
der Textreihenfolge; Dokumentlinks erscheinen nicht doppelt als Wurzelelemente.

Die tatsächliche AppKit-Gegenprobe prüft `θ🌍` als drei UTF-16-Einheiten,
Teilbereiche, unverlinkten Text, Surrogat-Innenpositionen, umgekehrte
Registrierungsreihenfolge und eine Beschriftung mit 1.536 Zeichen außerhalb
des begrenzten Controllabels. Offscreen-Fokus, sichtbare Zeichenrahmen,
Verbergen, Deaktivierung, Versionswechsel und gehaltene Referenzen nach
Zerstörung werden mit echten AppKit-Objekten geprüft.

Die reale F1-Probe liest Linkattribute, URL und Dokumentzugehörigkeit, fokussiert
einen Tabellenlink, aktiviert ein absichtlich ungültiges Ziel und navigiert
anschließend erfolgreich zum C-Lernweg. Die normale UI-Navigation bestätigt
nur die abgeschlossene gültige Aktion; nach dem Wechsel bleibt die gehaltene
alte Linkreferenz ungültig. Editorinhalt und Simulationszeit bleiben unverändert.
Die Linkinventur prüft **183 Seiten, 1.060 interne Links und höchstens 119 Links
pro Seite** mit Platz für die weiteren Controls.

Der [tatsächlich abgeschlossene macOS-Lauf](https://github.com/Lulus792/physim/actions/runs/38637540938)
auf `589d7d4` besteht mit LLVM 20 ASan/UBSan und SDL 3.2.30 unter Darwin 24.6.0:

- Intel: **4/4** Modellfälle in `run-hg_7_ct1`, **7/7** GUI-/AppKit-Fälle
  in `run-_f0uc4wt`; Job `116104427088`.
- ARM64: **4/4** Modellfälle in `run-wwabg475`, **7/7** GUI-/AppKit-Fälle
  in `run-ei3673d3`; Job `116104427496`.
- Beide vollständigen Joblogs und Originalarchive wurden gelesen. Auf beiden
  Architekturen wurden alle drei tatsächlichen Bilder für Fokus, fehlerhaftes
  Ziel und geöffnete Seite unmittelbar geprüft. Archive und Bildkonvertierungen
  liegen unter `build/native/F1-review/mac-links-intel.zip`, `mac-links-arm.zip`
  und den entsprechenden `-images`-Verzeichnissen.

Lokale Nachbarn, jeweils mit den ursprünglichen Prüfkriterien:

- MSVC ASan besteht die drei Modellfälle in `run-qmpteyi7`. Der GUI-Lauf
  `run-gvp32iph` besteht **2/3** Fälle: gezeichnete Accessibility und Navigation;
  der Windows-Linkclient erreicht seinen Bereitschaftsmarker innerhalb seiner
  20 Sekunden nicht. Seine leeren App-Ausgänge bleiben als Originalbeleg erhalten.
- Derselbe unveränderte Windows-Build besteht den vollständigen unabhängigen
  Linkclient separat in `build/native/mac-links-windows-isolated`. Der nächste
  Kataloglauf `run-w03vm1w3` scheitert jedoch innerhalb der Linkprüfung mit
  `E_FAIL`. Die beobachteten Ereignisse lauten zwei Geometrieänderungen und eine
  Sichtbarkeitsänderung; vor diesem Fehler fehlen Disable-/Enable-Ereignisse.
  Aus diesen Daten allein lässt sich der genaue fehlgeschlagene Schritt nicht
  bestimmen. Eine Ursache oder Fehlerbehebung wird nicht behauptet.
- Der Windows-Prüfer nennt jetzt den konkreten Prüfabschnitt und bei einem
  Fokus-Timeout die beobachteten Fokus-/Sichtbarkeitswerte. Werte, Zeitgrenzen
  und sämtliche Erfolgskriterien sind erhalten. Danach bestehen die vollständigen
  Linkläufe `run-7vf1hwvt` und `run-j8cygacu` jeweils **1/1**. Diese erfolgreichen
  Wiederholungen beseitigen die offene Sporadik nicht rückwirkend.
- Alpine 3.24.2 / GCC 15.2 / X11 mit Openbox und ASan/UBSan besteht **3/3**
  Modellfälle in `run-b8gex1n2` sowie **3/3** GUI-Fälle in `run-jsqoxafn`:
  Navigation und die unabhängigen AT-SPI-Feld- und Linkclients.
- Die Plattform-Katalog- und Fehlergegenproben bestehen unter Windows und in
  beiden ausgeführten macOS-Jobs. Spätere Windows-Diagnose- und
  Berichtergänzungen ändern die geprüfte macOS-Implementierung nicht.

Diese Nachweise schließen den bisher offenen nativen macOS-Linktextzugang.
Sie ersetzen keine externe AX- oder praktische VoiceOver-Abnahme. Semantische
Tabellen, praktische Screenreader-Aufgaben, vollständige Display-/Theme-Matrix,
Inhaltsabgleich und die Untersuchung der sporadischen Windows-Linkprobe bleiben
offen. Die Gesamtaufgabe erhält weiterhin keine 10/10.

### Native macOS-Suchfelder mit Auswahl und Caret

Die Erweiterung auf dem Zweig `codex/mac-f1-fields` verbindet beide benannten
F1-Suchfelder mit AppKit. Die zentralen Skills `macos-specialist`,
`accessibility-engineer` und `test-engineer` wurden dafür aus `D:/Skills/skills`
angewendet. Der Feldknoten liefert Wert, Zeichenanzahl, Auswahltext, einfache
und als `NSValue` verpackte UTF-16-Auswahlbereiche, Einzelzeilenbereiche,
sichtbaren Text und Bildschirmrahmen einschließlich des Carets. Grundlage sind
Apples Verträge für [Auswahlbereiche](https://developer.apple.com/documentation/appkit/nsaccessibilityprotocol/setaccessibilityselectedtextranges%28_%3A%29)
und [Zeilenbereiche](https://developer.apple.com/documentation/appkit/nsaccessibilityprotocol/accessibilityrange%28forline%3A%29).

Die Schreibmethoden prüfen den lebenden Feldknoten, aktivierte
Darstellung, Kapazität und gültige UTF-16-Grenzen. Eingebettete Nullzeichen,
falsche Objekttypen, Surrogat-Innenpositionen und mehrzeilige Eingaben gelangen
nicht in das UI-Modell. Das normale Setzen des Gesamtwerts und das Ersetzen einer
Auswahl besitzen unterschiedliche Caret-Verträge: Ersetzen erhält den Text vor
und nach der Auswahl und liefert den Caret direkt hinter der Einfügung gemeinsam
mit dem neuen Wert. Das gilt auch bei einer identischen Ersetzung. Eine inzwischen
geänderte Quelle verwirft die Ersetzung. Die Bildprobe verlangt nach einem nativen
Sprung zum Textanfang zunächst unsichtbare Endglyphen und macht erst dann das
Ende mit einer nativen Bereichsanforderung wieder sichtbar.

Tatsächlich ausgeführte lokale Prüfungen des gemeinsamen Modells und der
Plattformnachbarn, jeweils Exitcode 0:

- MSVC ASan: **4/4** Modellfälle in `run-r2nh0hrv` für UTF-16, Lesedokument,
  Suspension und editierbare Felder; **3/3** GUI-Fälle in `run-gkqf444w` für
  gezeichnete Accessibility, den externen Windows-Feldclient und Navigation.
- Alpine 3.24.2 / GCC 15.2 / X11 mit Openbox und ASan/UBSan: **4/4** Modellfälle
  in `run-o7jzt6f4`; **3/3** GUI-Fälle in `run-20y28hl_` für Navigation,
  Inhaltsaktionen und den unabhängigen AT-SPI-Feldclient.
- Eine gezielt defekte Modellkopie ohne Übernahme des Einfüge-Carets scheitert
  an der neuen Wert-/Auswahl-/Caret-Prüfung mit Exitcode 1. Die unveränderte
  finale Modellquelle besteht separat mit GCC ASan/UBSan und Leakprüfung.
  Originalprotokolle: `build/native/F1-review/mac-fields-caret-negative.log`
  und `mac-fields-caret-final.log`.
- Der Testläufer erkennt die nur auf Darwin ausführbaren AppKit-Fälle und die
  separaten Linux-AT-SPI-Clients korrekt. Die Fehler-, Timeout- und
  Kataloggegenproben bestehen unter Windows und Linux; ihre absichtlich
  fehlschlagenden simulierten Fälle zählen nicht als Produktfehler.

Der erste [tatsächlich ausgeführte macOS-Lauf](https://github.com/Lulus792/physim/actions/runs/38627140037)
für `8be7ca0` besteht auf Intel und ARM64 jeweils **4/4** Modell- und **6/6**
GUI-/AppKit-Fälle, einschließlich echter F1-Wertänderung, Unicode-Auswahl,
Ersetzung, Caret, verdecktem Textende, Verbergen und Editorisolation. Die
ursprüngliche Probe nahm wegen Verbergen vor dem Rendern kein Endbild auf;
die finale Probe verlangt diese Bilddatei ausdrücklich.

Der [finale ausgeführte macOS-Lauf](https://github.com/Lulus792/physim/actions/runs/38630367316)
auf Quellrevision `197f112` besteht auf beiden Architekturen mit LLVM 20
ASan/UBSan und SDL 3.2.30 unter Darwin 24.6.0:

- Intel: **4/4** Modellfälle in `run-wa1mmz6l`, **6/6** GUI-/AppKit-Fälle
  in `run-li1fhofh`; Job `116081789003`.
- ARM64: **4/4** Modellfälle in `run-z0_cr1nm`, **6/6** GUI-/AppKit-Fälle
  in `run-wb7u02mc`; Job `116081789315`.
- Die Plattform-Katalog- und Fehlergegenproben bestehen zusätzlich auf beiden
  Hosts. Beide vollständigen Joblogs und Originalartefakte wurden gelesen.
  Die neue reale Endaufnahme wurde auf beiden Architekturen unmittelbar
  geprüft: Im fokussierten Suchfeld bleibt das native Ziel `θ🌍` sichtbar.
  Die originale F1-Probe bestätigt auch den Caret direkt hinter der Ersetzung
  und den unveränderten Editorinhalt.

Die Originalarchive liegen unter `build/native/F1-review/mac-fields-final-intel.zip`
und `mac-fields-final-arm.zip`, die Bildkonvertierungen in den gleichnamigen
`-images`-Verzeichnissen. Diese Belege gelten für den getesteten Quellstand;
spätere Berichtergänzungen ändern die geprüfte Implementierung nicht.

Native macOS-Linktextbereiche, Tabellensemantik, externe AX-Clientprüfung und
praktische Screenreader-Aufgaben bleiben separate Abnahmepunkte. Diese
In-Prozess-AppKit-Prüfungen erteilen keine VoiceOver-Freigabe und keine 10/10
für die Gesamtaufgabe.

### Vollständiger AppKit-Lesetext auf macOS

Die zentralen Skills `macos-specialist`, `accessibility-engineer` und
`test-engineer` bestimmen den nächsten Plattformschritt. Ein eigener
`NSAccessibilityNavigableStaticText`-Knoten ergänzt F1 außerhalb des begrenzten
Control-Katalogs. Seine Wert- und Bereichsabfragen liefern den vollständigen
UTF-8-Lesetext über NSString; Zeichenpositionen zählen UTF-16-Einheiten gemäß
[Apples NSString-Vertrag](https://developer.apple.com/documentation/foundation/nsstring).
Der portable Index besitzt eigene UTF-8-/Bytekopien, behandelt zusätzliche
Surrogateinheiten und verwirft Teilbereiche innerhalb eines Surrogatpaars sowie
überlaufende Bereiche. Fehlgeschlagener Ersatz erhält den alten Index.

Der Knoten liefert Zeichenanzahl, reine und attributierte Teilstrings,
logische Newline-Bereiche, sichtbaren Textbereich und sichtbare Zeichenrahmen
in Bildschirmkoordinaten. Die Geometrie stammt aus dem tatsächlichen Layout;
unsichtbare Bereiche liefern keinen erfundenen Rahmen. Die Getter und die
native Anforderung zum Sichtbarmachen folgen
[NSAccessibilityProtocol](https://developer.apple.com/documentation/appkit/nsaccessibilityprotocol).
Dies ergänzt vollständiges Lesen, keine editierbare Auswahl, semantische
Tabellen, sprachabhängige Textformung oder native macOS-Linktextbereiche.

Ein eigenes Objective-C-Element besitzt eine Dokumentgeneration unabhängig
von numerischen Control-IDs. Der Gegenfall verwendet absichtlich dieselbe
Nummer für Dokument und Button. Direkte Press-/Fokus-/Wertaktionen des
Dokuments werden ausdrücklich abgefangen; sie können kein Control adressieren.
Zurückbehaltene Referenzen bleiben nach Entfernen und Wiedererscheinen ungültig.
Verbergen erhält den Lesetext, sperrt Geometrie und Scrollen. Native
Suspensionspublikationen werden bei unveränderter Verfügbarkeit ausgelassen.

Die tatsächliche F1-Probe fragt AppKit-Wert, Gesamtbereich und Zeichenanzahl
am laufenden Dokument ab, bestätigt initiale Glyphengeometrie und fordert
das Dokumentende über den nativen Bereichssetter an. Die vorhandene F1-Prüfung
fordert danach das sichtbare letzte Inhaltszeichen und eine echte Aufnahme.
Der erste Intel-Artefaktbeleg enthält **486.670 Bytes und 481.208 UTF-16-Einheiten**,
übereinstimmenden Gesamtwert/-bereich sowie Geometrie; die Endaufnahme zeigt
den tatsächlichen JSON-Abschluss. Die abweichende Größe gegenüber Windows-
Quellbelegen erklärt sich durch LF statt CRLF im Checkout, nicht durch Kürzung.

Die erste tatsächliche Ausführung, Workflow `38610393267` für `864ffa3`,
besteht unter macOS 15 auf beiden Architekturen mit ASan/UBSan:

- Intel, Job `116011918835`: **3/3 native** (`run-kvrblgrr`) und
  **5/5 GUI** (`run-qrkzfl6m`). Das vollständige Originalartefakt wurde gelesen;
  AppKit-Primitive und F1-Wert/Bereich/Geometrie bestätigen ihre Ausführung.
- ARM64, Job `116011919525`: **3/3 native** (`run-xvkmuf0n`) und
  **5/5 GUI** (`run-12sfaluo`). Die Originalprotokolle wurden eingesehen.

Die lokale UTF-16-/Dokumentgruppe besteht mit MSVC ASan **2/2** in
`run-3as_tnuh`; drei Windows-UI-Nachbarn bestehen dort in `run-bq9hqzu4`.
Der portable UTF-16-Grenzprüfer besteht zusätzlich unter GCC ASan/UBSan mit
Leakprüfung. Die macOS-Prüfung nutzt tatsächliche AppKit-Objekte im Prozess
und beide Plattform-ABIs; sie ist keine externe AXUIElement-Clientprüfung
und keine praktische VoiceOver-Abnahme. Diese Grenzen bleiben ausdrücklich offen.

Der abschließende Implementierungsstand `23c0495` besteht ebenfalls tatsächlich
auf beiden Architekturen, Workflow `38616701014`:

- Intel, Job `116034845271`: **3/3 native** (`run-_3ukzd28`) und
  **5/5 GUI** (`run-p2m61wr6`).
- ARM64, Job `116034845684`: **3/3 native** (`run-_o0hryh0`) und
  **5/5 GUI** (`run-u0c14epg`).

Die Originalprotokolle und das vollständige finale ARM64-Artefakt wurden
eingesehen. Es bestätigt den absichtlich kollidierenden Button-/Dokumentfall,
die gehaltenen Referenzen, vollständige native F1-Wert-/Bereichsabfragen mit
486.670 Bytes/481.208 UTF-16-Einheiten und reale Glyphengeometrie. Die finale
ARM64-Endaufnahme zeigt ebenfalls die schließende JSON-Klammer und wurde
unmittelbar betrachtet. Das Prüfsystem erfasst beide tatsächlichen Images
`macos-15`/`macos-15-arm64` und Darwin 24.6.0.

Die vorherige Theme-Reflow-Korrektur `17367e6` besteht inzwischen auch unter
Ubuntu: Workflow `38613571494`, Clang-Job `116022808125` und GCC-Job
`116022808828`, jeweils **24/24 native und 31/31 GUI-Fälle**. Die eingesehenen
Originalprotokolle enthalten die erfolgreiche strengere Sidebar-Probe;
Clang: `run-ds9pr0wu`/`run-7u8vx_5z`, GCC:
`run-_cuszih9`/`run-gkgpojxg`. Die vorherigen fehlgeschlagenen Theme-Läufe
bleiben als begrenzte Gegenbelege erhalten.

### Offline-Schriftquellen und vollständige Zeicheninventur

Die nächste Korrektur erweitert die Gegenprobe auf alle **167 druckbaren Zeichen
der 183 registrierten Originalseiten**. Die vorherige Beschränkung auf 43
mathematische Zeichen genügte nicht. Leerzeichen benötigen einen vorhandenen
Quellglyph und positiven Vorschub, aber keine sichtbare Tinte. U+0378 bleibt
ein negativer Fall: Die rohen Atlas-Metadaten kennen seinen Codepunkt, der
Quellfont besitzt jedoch keinen Glyphenindex dafür.

Fünf unveränderte Noto-Schriften werden offline mit ihren SIL-OFL-1.1-Lizenzen
ausgeliefert. Quelle ist Google Fonts, Revision
`66928797f271fed5ee428544e7229685e2d90127`; Herkunft, Bytegrößen und SHA-256
stehen in `third_party/fonts/manifest.json`. Noto Sans, Sans Mono, Sans Math,
Sans Symbols 2 und Emoji ergänzen nur Zeichen, die in vorherigen deklarierten
Quellbereichen fehlen. Die gebackenen Glyphen speichern ihre tatsächliche
Quellpräsenz; der Leser überspringt `.notdef` auch bei überlappenden Bereichen.
Diese Information bleibt nach Freigabe der TTF-Quelldaten gültig. Eigene
Fallback-Bereiche bleiben bis zur Atlasfreigabe erhalten.

Die neue native Gegenprobe entdeckt zunächst einen Nuklear-Fehler:
Zusatzschriften wurden an den ersten statt den zuletzt angelegten Handle
gehängt (`run-yzktc5r5`). Nach dieser Korrektur findet sie den weiterhin
verwendeten fehlenden Originalglyph (`run-wx8p5re3`). Die Quellpräsenz und
Lookup-Korrektur beheben beide Ursachen. Fehlgeschlagene Adds verändern keine
Listen oder Besitzverträge; die Tests prüfen Allokationsausfälle, Retry,
Rastertinte und vollständige Freigabe. Der erfolgreiche Merge liefert den
tatsächlich erweiterten letzten Handle zurück.

Die erste komplette GUI-Gegenprobe findet noch die neun Rahmenzeichen
(`run-plv4gkwn`); die Monospace-Quelle ergänzt sie. Die Bildprüfung entdeckt
anschließend verschiedene Vorschübe für Rahmen und Leerzeichen. Rahmen gehören
jetzt auch zum deklarierten Bereich der primären Codeschrift; alle neun
Codevarianten müssen gleiche Zellbreiten liefern. Ist die Systemcodeschrift
nicht verfügbar, wird Noto Sans Mono als primäre Codeschrift geladen.
Die zusätzlichen Bilder prüfen Globus, Gesicht, Schneemann, Modifikator,
tiefgestellten Buchstaben und Rahmencode bei 16/32 Pixeln.

Ein zweiter echter App-Lauf setzt `PHYSIM_TEST_BUNDLED_FONTS=1` und umgeht alle
Systemschriften. Er bestätigt 50 tatsächlich geladene primäre Handles im
Hauptfenster und in F1 sowie erneut alle 167 Zeichen in 38 F1-Varianten.
Diese strengere Gegenprobe findet das fehlende Integralzeichen U+222B
(`run-6am85uut`); Noto Sans Math behebt es. Der finale ASan-Lauf besteht
beide Modi. Dies beweist den geprüften Zeichenbestand ohne Systemschrift;
weitere Schriftsysteme oder komplexe Textformung folgen daraus nicht.

Abschließende tatsächlich ausgeführte Windows-Belege, jeweils Exitcode 0:

- `run-efjeijrr`: 8/8 native Font-, Parser-, Lernweg-, API- und Abschnittsfälle.
- `run-28qo4u86` unter MSVC ASan: 4/4 Fontfälle mit Quellfonts, Rastertinte,
  Fehlereigentum und Cleanup. Die Quelle ohne Integral wurde zusätzlich durch
  die reale App-Gegenprobe erkannt.
- `run-2ciy1267` unter MSVC ASan: 5/5 GUI-/UIA-Fälle für Zeichen einschließlich
  rein gebündelter primärer Fonts, Zoom, Suchfelder, Links und Lebenszyklus.
  Die neuen 32-Pixel-Bilder mit Integral und Rahmen wurden visuell geprüft.
- `build/native/F1-review/font-installed-proof`: der echte Release-SDK wurde
  nach `build/native/font installed ä/Physim` verschoben und von einem anderen
  Arbeitsverzeichnis ausgeführt. Beide primären Fontmodi bestehen dieselbe
  App-Zeichenprobe. Alle 536 Paketdateien und zehn Font-/Lizenzdateien stimmen
  mit ihren Manifest-Hashes überein; die primären Fontpfade liegen im SDK.

Damit sind die belegten fehlenden Quellzeichen und falschen Merge-/Besitzpfade
behoben. Eine universelle Schriftabdeckung oder vollständige UI-Abnahme wird
nicht erteilt. Bei rein gebündelten primären Fonts bleiben Hervorhebungsgewichte
und die vertikalen Abstände einzelner Rahmendarstellungen optischer Feinschliff.
Die tatsächliche DPI-/Monitor-, assistive und native Plattformabnahme bleibt
ebenfalls offen. Die Linux-Workflowauswahl enthält jetzt beide neuen nativen
Fontfälle und die erweiterte Zwei-Modi-Zeichenprobe.

Der vorherige Linux-Stand `38efa9d` ist inzwischen tatsächlich erfolgreich:
Clang besteht 20/20 native und 25/25 GUI-Fälle (`run-ri29r7wd`, `run-uyccp8ik`),
GCC ebenso (`run-1r6uzz3_`, `run-16qahise`), Workflow `38497490053`.
Dies ist ein Nachweis für den vorherigen Stand und ersetzt keine Ausführung
der neuen Schriftkorrektur unter Linux oder macOS.
Nach den Dokumentationskorrekturen bestehen API, Parser, Lernwege und
Abschnittslinks erneut 4/4 in `run-2b5685yy`. Der Testläufer erkennt seine
gezielten Fehlermuster in `build/native/F1-review/font-runner-proof`;
die Gesamtprüfung dieses Läufers endet mit Exitcode 0. `.gitattributes` erhält
die gepinnten Font-/Lizenzbytes auch bei Windows-Checkouts unverändert.

### Hervorhebung, Rahmenzellen und tatsächlich sichtbarer Leserabschluss

Die Gegenprobe `run-d8kr410o` bestätigt identische normale und starke Schrift
in allen neun rein gebündelten Leseschriftgrößen. Die normale Standardachse
eines variablen Fonts ersetzt keinen Bold-Schnitt. Eine unveränderte statische
Noto-Sans-Bold-Datei aus dem offiziellen archivierten Noto-Repository,
Revision `ffebf8c1ee449e544955a7e813c54f9b73848eac`, wird jetzt für starke Rollen
geladen. Der Font und seine SIL-OFL-1.1-Lizenz besitzen eigene gepinnte Quelle
und Hashes. Der Fontbestand umfasst damit sechs Dateien plus sechs Lizenzen.

Die native Rasterprüfung misst für A–Z bei 32 Pixeln eine Alpha-Summe von
526290 für Regular und 821164 für Bold. Die Strichfläche ist tatsächlich
verschieden. Die reale App fordert außerdem unterscheidbare AaBb-Metriken
in allen neun Lesegrößen und 24 tatsächlich geladene Bold-Handles innerhalb
ihrer 50 primären Hauptfenster-/F1-Handles. Normal und stark stehen im neuen
Prüftext nebeneinander; beide Schriftgrößen und Fontmodi werden gezeichnet.

`run-94p1ztw2` erkennt eine Codezeilenhöhe von 38 gegenüber einer vertikalen
Quellzelle von 39. Für Codeblöcke mit Rahmenzeichen wird die natürliche Zellhöhe
einmal pro Layout berechnet und gecacht. Messung, Zeichnung und native
Zeichenrahmen verwenden dieselbe Zellverschiebung. Gewöhnlicher Code und
Absätze behalten ihre bisherige Zeilenhöhe; Fontatlas-Glyphen werden dafür
nicht global verändert.

Die strengere Schlusslinienprobe findet zusätzlich abgeschnittene Abschlüsse.
Ein direkt gesetzter Fokuswert und der echte Tab-/Ende-Ablauf beseitigen die
Ursache nicht (`run-ltd3cj00`, `run-8jhuq64b`). Die Diagnose `run-l3uvjvhd`
zeigt, dass ein großer Scrollauftrag vom Scrollbalken anschließend auf 78
zurückgesetzt wird. Eine ungenutzte Layoutzeile zwischen Messung und Anlage
des Lesergruppenrahmens verschob dessen Ursprung, während seine Höhe noch
von der früheren Position aus berechnet wurde. Ihr Entfernen richtet
Gruppenrahmen und tatsächliche Clipfläche aus. Der echte Tab-/Ende-Ablauf
zeigt danach den vollständigen Diagrammabschluss.

Die unabhängige BMP-Prüfung liest echte Pixel aller drei vertikalen Linien
und der Schlusslinie. Sie berücksichtigt das Verhältnis zwischen 760 × 540
logischen Koordinaten und den Bildpixeln, verlangt durchgehende Striche und
lehnt eine injizierte Lücke ab. Sie besteht mit Systemfonts und rein gebündelten
Fonts. Die neuen Bilder mit sichtbarer Hervorhebung und geschlossenem Rahmen
wurden visuell geprüft.

Abschließende Windows-Belege mit Exitcode 0:

- `run-hxqhkbmu`: 9/9 GUI-/UIA-Fälle für Tastatur, Verlauf, Inline-Inhalt,
  Schrift/Pixel, Zoom, Suchfelder, vollständiges Lesen, Links und Lebenszyklus.
- `run-kbrw86jp` unter MSVC ASan: 6/6 native Font-, Parser- und Abschnittsfälle.
- `run-bdgenm68` unter MSVC ASan: 4/4 GUI-/UIA-Fälle einschließlich vollständigem
  Lesen, Links, Zoom und beiden Fontmodi mit Rastergegenprobe.
- `build/native/F1-review/typography-installed-proof`: beide echten App-Fontmodi
  bestehen aus dem verschobenen Release-SDK unter
  `build/native/typography installed ä/Physim`, von einem anderen Arbeitsordner.
  Alle 538 Paketdateien und zwölf Font-/Lizenzdateien entsprechen ihren Hashes.
  Die tatsächlichen Bold-Handles und geschlossenen Rahmen werden mitgeprüft.

Der vorige Stand `2d88eb3` ist auch unter Linux tatsächlich erfolgreich:
Workflow `38504376167`, Clang 22/22 native (`run-w9fa1arb`) und 25/25 GUI
(`run-zg7hwyie`), GCC 22/22 native (`run-mjoimclz`) und 25/25 GUI
(`run-ium9fqh7`). Die neuen Typografie-/Leserhöhenänderungen benötigen eigene
Linux- und macOS-Nachweise; die vorherigen Läufe ersetzen sie nicht.

Die beiden optischen Font-Restpunkte sind damit in den geprüften Windows-Modi
behoben. Tatsächliche DPI-/Monitorwechsel, praktische Screenreader-Aufgaben,
native Tabellen- und vollständige Text-/Feld-/Linkanbindung auf macOS/Linux
und weitere F1-Abnahmekriterien bleiben offen. Eine vollständige Produktfreigabe
wird weiterhin nicht behauptet.
Nach der Dokumentation dieser Korrektur bestehen Parser, API-Referenz, Lernwege
und Abschnittslinks erneut 4/4 mit Exitcode 0 in `run-rt8xha9i`.

### Pixeldichte, logische Fontgrößen und atomarer Atlaswechsel

Der nächste Schritt trennt die Rasterauflösung eines Fonts von seiner logischen
Höhe. Nach dem Bake werden die Handles und Query-Metriken auf die gewünschte
logische Größe abgebildet. Die native Probe bestätigt die Rasterdichten
1/1,25/1,5/2 mit unveränderten Textbreiten; erneute Abbildung ist idempotent,
ungültige Werte verändern den vorhandenen Stand nicht. Rahmenzellen lesen
ebenfalls die logischen Query-Metriken statt unskalierter Atlasrechtecke.

F1 fragt bei Erstellung und vor dem Zeichnen `SDL_GetWindowPixelDensity` ab.
Seine 38 Größen-/Stilvarianten werden mit entsprechend höherer Rasterauflösung
angelegt. Ein Ersatzatlas wird getrennt aufgebaut, vollständig gebacken und
hochgeladen. Erst bei Erfolg werden Textur, Nulltextur und Font-Handles
übernommen; der neue Defaultfont wird vor Freigabe des alten Atlasses installiert.
Die Dokumentposition wird anhand der veröffentlichten Leseposition erhalten
und der Layoutcache invalidiert. Fehlgeschlagene Versuche behalten die lebenden
Fonts; dieselbe fehlgeschlagene Dichte wird nicht in jedem Frame neu gebacken.

Die wirkliche App-Probe `run-bhggy_1m` wechselt die Rasterdichte
1→1,25→1,5→2→1. Bei 16 logischen Pixeln liegen 20/24/32/16 Rasterpixel vor;
die gemessene AaBb-Textbreite bleibt 57,081. Textversion und Editorinhalt bleiben
gleich. Die anschließende Fehler-Gegenprobe `run-5610x6n2` deckt einen Debug-Assert
bei absichtlich fehlender Bake-Allokation auf. Nur der betreffende Prüfprozess
wurde beendet. Temporäre, Glyph-, Pixel- und RGBA-Allokationsfehler kehren jetzt
auch im Debug-Build durch die bereits vorhandenen Freigabepfade zurück. Die
reale Probe bestätigt den erhaltenen Defaultfont und vollständige Freigabe des
fehlgeschlagenen Kandidaten.

Eine weitere Gegenprobe erkennt eine zu frühe Umschaltung im Prüfer
(`run-unzsj6g6`): Der Zahlenbeleg stammte noch von Dichte 2, die PNG-Aufnahme
entstand nach dem Wechsel auf Dichte 1. Der folgende Wechsel erfolgt jetzt
erst im nächsten Frame. Der finale Beleg `run-4qsmr6yd` zeichnet den echten
1520 × 1080-Framebuffer aus 760 × 540 logischen Koordinaten mit dem tatsächlich
32 Pixel hoch gerasterten 16-Pixel-Font. PNG-CRC, Dimensionen, Scanlines und
lesbare Textpixel werden unabhängig geprüft; das Bild wurde visuell angesehen.

Abschließende Windows-Belege, jeweils Exitcode 0:

- `run-au995z_a`: 7/7 native Font-, Parser- und Abschnittsfälle mit MSVC ASan.
- `run-vylai6qc`: 6/6 reale F1-/UIA-Fälle unter MSVC ASan für Rasterwechsel,
  Zeichen, Zoom, vollständiges Lesen, Links und Fensterlebenszyklus.
- `build/native/F1-review/density-installed-proof`: der verschobene Release-SDK
  unter `build/native/density installed ä/Physim` besteht Raster-Rundlauf,
  Fehlererhalt und doppelte Framebuffer-Ausgabe von einem anderen Arbeitsordner.
  Alle 538 Paketdateien und zwölf Font-/Lizenzdateien entsprechen ihren Hashes.

Diese Rasterprüfungen sind keine echte Windows-DPI- oder Monitorwechselabnahme.
Der verfügbare Windows-Monitor liefert 96 DPI. SDL unterscheidet Pixeldichte
und Inhaltsskalierung: Windows/X11 verwenden typischerweise physische
Fensterkoordinaten mit Inhaltsskalierung, macOS/Wayland zusätzlich Fenster- zu
Pixelverhältnisse. Grundlage sind die offiziellen
[SDL-HiDPI-Verträge](https://wiki.libsdl.org/SDL3/README-highdpi) und
[DisplayScale](https://wiki.libsdl.org/SDL3/SDL_GetWindowDisplayScale).
Der nun geprüfte Font-Rasterpfad behandelt die Pixeldichte; Windows-Inhaltsskalierung,
die Hauptfenster-DPI-Integration, reale Monitorwechsel und native Plattformbilder
bleiben weitere Aufgaben. Die Diagnose überschreibt in der Probe die Rasterdichte
und erzeugt tatsächliche Renderziele; sie täuscht kein anderes Betriebssystem-DPI vor.

Der vorherige Typografie-Stand `9e172e4` ist unter Linux inzwischen ausgeführt:
Workflow `38508599005`, Clang 22/22 native (`run-ewmhvafp`) und 25/25 GUI
(`run-srggasan`), GCC 22/22 native (`run-1qg4r9pk`) und 25/25 GUI
(`run-olowylup`). Der neue Rasterwechsel benötigt einen eigenen Linux-/macOS-Nachweis.
Der Rasterstand `efe152c` besteht inzwischen ebenfalls unter Ubuntu 24.04/X11/Xvfb
mit ASan/UBSan: Clang 23/23 native (`run-g6ir94u1`) und 26/26 GUI
(`run-27_up4e4`), GCC 23/23 native (`run-rptj2xn6`) und 26/26 GUI
(`run-57dgly_y`). Beide prüfen den tatsächlichen Raster-Rundlauf.
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38511888866/job/115682024030),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38511888866/job/115682024274).
Dies ersetzt weiterhin keine echte Windows-DPI- oder macOS-Abnahme.

Die abschließenden Parser-/API-/Abschnitts-/Dichtefälle bestehen nach der
Dokumentation erneut 4/4 mit Exitcode 0 in `run-2qbh1dw6`. Der Testläufer
bestätigt seine gezielten Fehlermuster in `build/native/F1-review/density-runner-proof`.

### Lesemodus mit erreichbarer Navigation

Die Navigation lässt sich jetzt über einen benannten Schalter sowie
Ctrl+Shift+B bzw. Cmd+Shift+B ein- und ausblenden. Der Artikel erhält dadurch
die volle Fensterbreite. Bei mindestens 24-Pixel-Leseschrift und ausreichender
Breite teilen sich Navigation, Verlauf, Start, Arbeitsbereich und Schriftgröße
eine Werkzeugzeile. Die Dokumentensuche bleibt direkt sichtbar.

Der Wechsel erhält Inhalt und Inhaltsversion, Suchbegriffe und eine Textstelle
aus dem veröffentlichten sichtbaren Bereich. Wartende Texteingaben werden vor
dem Verlassen ihres Felds verarbeitet; der alte Editierfokus wird zurückgesetzt.
Ausgeblendete Seiten- und Suchcontrols entfallen aus der Tab-Reihenfolge und dem
neuen nativen Frame-Snapshot. Der Navigationsschalter bleibt im Leser erreichbar.
Die Auswahl bleibt über Schließen/Wiederöffnen innerhalb der Sitzung erhalten.

Die echte Windows-Probe misst bei 760 × 540 und 32-Pixel-Schrift eine
Lesefläche von **404 × 288** mit Navigation und **716 × 344** im Lesemodus.
Eine gescrollte Textstelle (Byte 212 der Prüffassung) bleibt nach dem Umbruch
sichtbar. Die Probe durchläuft die tatsächliche Tab-Reihenfolge, verarbeitet
Unicode-Eingabe unmittelbar vor dem Umschalten, ignoriert Tastenwiederholung,
bedient den Schalter über Windows UIA und Maus und prüft Wiederöffnen,
erhaltene Editorinhalte und unveränderte Simulationszeit. Dunkle, helle und
Kontrastbilder stammen aus abgeschlossenen Frames; kleine Ansichten und die
helle Suche wurden direkt betrachtet.

Ausgeführte Prüfungen unter Windows 10.0.26200/MSVC, jeweils Exitcode 0:

- `run-j9za539b`: 9/9 reale Fälle für Tastatur, Verlauf, Zoom, Inline-Inhalte,
  Lesemodus sowie externe UIA-Felder, Lesetext, Links und Fensterlebenszyklus.
- `run-hzn8c7b7` unter MSVC ASan: 4/4 Fälle für Lesemodus, Zoom,
  Inline-Inhalte und extern geprüften Fensterlebenszyklus.
- `run-hyy3ijtt` im Release-Build: 1/1 vollständiger Lesemodus-Ablauf.
- Der native Abschnitts-/Seitenprüfer bestätigt 214 Abschnittslinks und
  183 parsbare, intern erreichbare Seiten nach der Anleitungsergänzung.
  Testkatalog und erwartete Fehlerfälle bestehen in
  `build/native/F1-review/navigation-runner-proof`.

Die Linux-Regression enthält den neuen Lesemodus, wurde aber an diesem
Zwischenschritt noch nicht ausgeführt. macOS-UI, echte DPI-/Monitorwechsel,
native Text-/Feld-/Linkbereiche und Tabellensemantik auf allen Plattformen sowie
praktische Screenreader-Aufgaben bleiben Teil der vollständigen Abnahme.
Der frühere Mangel der engen Lesefläche besitzt nun einen geprüften Bedienweg;
das allein begründet weiterhin keine volle Gesamtpunktzahl.

### Vollständiger nativer Lesetext unter Linux

Die Linux-Anbindung ergänzt F1 um einen eigenen `document text`-Knoten mit
AT-SPI-Text-Interface. Er gehört zum richtigen Fenster, benötigt keinen Platz
im begrenzten Control-Katalog und besitzt eine eigene Objektadresse. Solange
das Dokument vorhanden ist, bleiben Objektkennung und Lesetext beim Verbergen
erhalten. Nach Entfernen und erneutem Publizieren wird die Kennung nicht
wiederverwendet. Alte Referenzen können das neue Dokument nicht lesen oder
bewegen.

Der separate Index hält eine eigene UTF-8-Kopie sowie Zeichen- und Bytepositionen.
Teilbereiche und Zeichenanzahl zählen Unicode-Skalarwerte, einschließlich
mehrbyteiger Zeichen und Zeichen außerhalb der BMP. Vor dem Wechsel werden
neue Speicherblöcke vollständig angelegt; ungültige Eingaben erhalten den
vorherigen Index. Native Geometrie stammt aus den tatsächlichen veröffentlichten
Glyphenrahmen. Verborgene Ansichten liefern leere Geometrie und weisen
Scrollanforderungen ab, behalten aber den lesbaren Gesamttext.

Inhaltsmitteilungen verwenden eine separate Kopie des zuletzt angekündigten
Texts. Eine Clientabfrage zwischen Modellpublikation und nativer Mitteilung
darf den Index bereits auf den neuen Inhalt umstellen; die Delete-Mitteilung
muss weiterhin den alten Text und dessen Zeichenanzahl beschreiben. Der
unabhängige Client bestätigt Delete-/Insert-Inhalt und Zeichenanzahl sowie
Namenswechsel und das Auslaufen alter Objektkennungen.

Die tatsächliche F1-App liefert dem unabhängigen AT-SPI-Client alle **498.605
Bytes / 493.143 Zeichen** der Quellseite einschließlich ihrer Abschlusszeilen.
Der Client prüft einen sichtbaren Anfangsglyphen und löst das native Scrollen
bis zum Ende aus. Der App-Prüfer bestätigt den sichtbaren letzten Inhaltsglyphen
und erzeugt ein Endbild. Dieses Linux-Bild wurde direkt betrachtet.
Grundlage sind die offiziellen [Text-Verträge](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Text.html)
und die [Dokumentrolle](https://docs.gtk.org/atspi2/enum.Role.html).

Die lokale Umgebung wurde tatsächlich neu erhoben: Alpine **3.24.2 unter WSL2**,
GCC **15.2.0**, X11/Xvfb/Openbox, Mesa **26.1.6/llvmpipe**, D-Bus **1.16.2**,
AT-SPI **2.60.5** mit PyAT-SPI **2.58.2**. SDL **3.2.30** wurde aus dem
festgelegten Release-Quellstand `f5e5f6588921eed3d7d048ce43d9eb1ff0da0ffc`
gebaut und installiert. Der erste GUI-Anlauf scheitert vor den neuen
Anwendungstests an fehlenden GSettings-Schemata und GLX-Laufzeitpaketen
(`run-97uenk_1`); diese Voraussetzungen sind anschließend eingerichtet.
Die Tests besitzen einen eigenen Accessibility-Bus und XDG-Laufzeitordner,
statt den anders besessenen WSLg-Laufzeitordner zu verwenden.

Der folgende normale Lauf `run-a9_rvrvd` besteht beide nativen Textfälle,
scheitert aber einmal am Lesemodus. Die Sanitizergegenprobe `run-1nuffmei`
und die Ereignisdiagnose `run-bw68fx59` belegen einen ungültigen Bool-Wert
im künstlichen Tastaturereignis bereits vor den App-Callbacks. Der geprüfte
GCC-Code des gemeinsamen Testhelfers initialisiert mit `SDL_Event e = {0}`
nur die ersten vier Bytes; `repeat` liegt an Byte 37. Der Helfer verwendet
jetzt `SDL_zero(e)` für die vollständige Ereignisstruktur. Die gleiche
Korrektur gilt für den künstlichen Tastaturimpuls der AT-SPI-Fensterprobe.
Der erfolgreiche Ablauf `run-840du_wy` folgt unmittelbar auf diese Korrektur.

Abschließende ausgeführte Nachweise, jeweils Exitcode 0:

- `run-x05sqnwd` unter lokalem GCC ASan/UBSan: **3/3** für den unabhängigen
  AT-SPI-Fenster-/Dokumentclient, den vollständigen realen F1-Leser und den
  Lesemodus; Diagnoseausgaben wurden vorher wieder entfernt.
- `run-a0kkfmxc`: **3/3** derselben normalen Linux-Fälle mit exakt geprüften
  Textänderungsmitteilungen. Dieser Lauf liegt noch vor der expliziten
  vollständigen Tastaturereignisinitialisierung.
- `run-nxl6ilvu` unter MSVC ASan: **4/4** für Windows-Felder, vollständigen
  Lesetext, Lebenszyklus und Lesemodus nach der gemeinsamen Helferkorrektur.
- Unicode-Index nativ unter GCC sowie GCC ASan/UBSan bestanden;
  unter MSVC **1/1** in `run-b1slqtp3`, unter MSVC ASan **1/1** in
  `run-6m10t0br`. Testkatalog und erwartete Fehlerfälle bestehen in
  `build/native/F1-review/atspi-runner-proof`.

Der vorherige Lesemodus-Stand `64b9bd5` besteht inzwischen auch unter Ubuntu
24.04/X11/Xvfb: Clang 23/23 native (`run-wocb32z4`) und 27/27 GUI
(`run-7cl66m41`), GCC 23/23 native (`run-l6d6kqvr`) und 27/27 GUI
(`run-zu2laq9b`).
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38514071351/job/115688803175),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38514071351/job/115688802981).
Dies ist ein Nachweis dieser früheren Revision; der neue Textpfad besitzt
hier zunächst die ausgeführten lokalen Alpine-Nachweise.

Offen bleiben native Linux-Felder/Editoren, Linktextbereiche und Tabellen,
visuelle Zeilengrenzen und sprachabhängige Segmentierung, begrenzte
Bereichsabfragen und Scrollen zu einer bestimmten Bildschirmposition,
macOS-Textzugang, reale DPI-/Monitorwechsel und praktische Screenreader-Aufgaben.
Die vollständige F1- und Projektabnahme ist damit weiterhin nicht abgeschlossen.

## Nutzeraufgaben

- Eine Vorlage anlegen, bauen, aufzeichnen und auswerten, mit sichtbaren
  Zwischenergebnissen und einer Prüfung des gespeicherten Ergebnisses.
- Ein Beispiel nach einer physikalischen Frage auswählen und verändern.
- C oder Physim lernen, ohne unbeabsichtigt den Sprachweg zu wechseln.
- Eine Funktion einschließlich Signatur, Einheiten, Lebensdauer und Fehlern finden.
- Ein beobachtetes Problem anhand seiner Symptome beheben.

## Navigation und Darstellung

Der Einstieg priorisiert Bedienung, Beispiele, Lernwege, Referenz und
Fehlerbehebung. Projekttechnik und historische Nachweise bleiben erreichbar.
Seitennavigation erhält Verlauf und Leseposition. Eine Suche zeigt ihren Umfang,
Treffer und den aktuellen Treffer eindeutig. Fehlende Dokumente und fehlende
Treffer geben eine verständliche Rückmeldung und einen erreichbaren Rückweg.

Der Text bleibt die Hauptfläche. Überschriften zeigen Hierarchie; Hervorhebung,
Inline-Code, vollständige kopierbare Codeblöcke, Tabellen und mathematische
Zeichen müssen ihren Inhalt erhalten. Links stehen erkennbar beim zugehörigen
Text. Lange Texte und deutsche Beschriftungen dürfen keine Aktion verdecken.
Die räumlichen Erklärungen besitzen Zahlen, Einheiten und Textalternativen.

## Abnahmekriterien

1. Alle registrierten Seiten und internen Abschnittslinks sind offline erreichbar.
   Historische Texte werden ohne Inhaltsverlust auf lesbare Archivbände verteilt.
2. Codekopieren, Suche, Abschnittssprünge und vollständiger Navigationsverlauf
   funktionieren mit Maus und Tastatur. F1-Eingaben verändern keinen Editor.
3. Beispiele entsprechen den veröffentlichten Quellen und der tatsächlichen CLI.
   Implementierung, ausgeführte Prüfung und Planung bleiben unterscheidbar.
4. Standardtext und mindestens doppelte Textgröße bleiben bei Mindestgröße und
   gewöhnlicher Fenstergröße lesbar und bedienbar. Die Schrift enthält alle
   in den gebündelten Seiten verwendeten mathematischen Zeichen.
5. Dunkel, Hell und hoher Kontrast besitzen geprüfte Zustände und sichtbaren Fokus.
   Text-, Such- und Fehlerzustände sind auch ohne alleinige Farbkodierung erkennbar.
6. Native Accessibility veröffentlicht Inhalt und Bedienelemente mit sinnvoller
   Lesefolge, Namen, Rollen, Werten und Zuständen. Echte Screenreader-Aufgaben
   prüfen Suche, Lesen, Linkaktivierung und Rückkehr zum Arbeitsbereich.
7. Fenstergrößen, tatsächliche Pixeldichten, Theme-Wechsel und Eingabeisolation
   werden mit Laufzeitbildern und Verhalten geprüft. Simulation und Erklärung
   bleiben getrennt; reduzierte Effekte erhalten sämtliche Funktionen.
8. Windows, Linux und macOS erhalten eigene Build-, Funktions- und UI-Nachweise
   für die geprüfte Revision. Nicht ausgeführte Fälle bleiben offen.

## Prüfstand

Die Umsetzung dieser Entscheidung ist in Arbeit. Nach der ersten Reparaturgruppe
bestehen unter Windows 11 Pro 10.0.26200 mit MSVC und SDL 3.2.30 die 27 ausgewählten
Dokumentations-/API-Tests (`run-tyh8qq7j`) und elf GUI-Fälle (`run-comr4nko`).
Die übergroße Plattformseite wurde ohne Textverlust aufgeteilt; alle 183
registrierten Seiten und 214 Abschnittslinks sind erreichbar. Der Fontatlas
enthält jetzt mathematische Zeichen, Pfeile und tiefgestellte Ziffern.

Der zunächst fehlgeschlagene Kopiertest war ein Fehler des Prüfers: SDL ergänzt
unter Windows CR vor LF für CF_UNICODETEXT. Die echte Aktion liefert den gesamten
Code. Der Vergleich akzeptiert ausschließlich diese Zeilenendendarstellung;
Gegenbeispiele mit fehlenden Zeilen, verändertem Code und Einrückung bleiben Fehler.
Ergebnisse liegen unter `build/native/F1-review/test-results` im ignorierten
Buildverzeichnis. Dies ist eine lokale Prüfung, keine vollständige Produktabnahme.

Der Verlauf enthält jetzt bis zu 64 Seiten und Abschnittsziele mit Leseposition,
Suchbegriff, ausgewähltem Suchtreffer, Inhaltsansicht und Sprachweg. Die
globale Suche berücksichtigt registrierte Quellseiten, ordnet Titeltreffer zuerst
und verwendet dieselbe Reihenfolge für Anzeige und Tastatur. Bedienung und
Beispiele stehen am Anfang der Themenliste.

Die Hilfe besitzt eine eigene Leseschrift bis 32 Pixel. Bei doppelter
Standardschrift werden Navigation und Suche kompakter; mehrzeilige Themen
bleiben lesbar. Die 13 GUI-Fälle dieser Erweiterung bestehen in `run-9w1tddq_`;
die abschließende Gegenprobe für Verlauf, Header-Suche und Schrift besteht
3/3 (`run-lf_ojqxq`). Die 30 ausgewählten nativen Fälle bestehen in `run-1ay2gb5q`;
nach Erhaltung des ausgewählten Suchtreffers bestehen die sechs betroffenen
Modell-/Parser-/Referenz-/Geometrie-/Einstellungs-/Linkfälle erneut in `run-lrv_uo0z`.
Die Bilder des kleinen 32-Pixel-Lesers und des wiederhergestellten Verlaufs
wurden visuell geprüft. Das bestätigt noch keine HiDPI- oder Screenreader-Abnahme.

Die abschließende 32-Pixel-Prüfung besteht zusätzlich unter Dunkel, Hell und
hohem Kontrast, bei 760 × 540 und 1080 × 780 sowie nach Schließen und
Wiederöffnung (`run-fohiafzg`). Die unabhängige Prüfung der tatsächlichen
Bitmapfarben besteht in `build/native/F1-review/theme-proof`. Sie erkennt
das ursprüngliche Gegenbeispiel, bei dem der Themenwechsel vor der Aufnahme
stattfand und die Bildnamen nicht zur dargestellten Palette passten. Die
korrigierten Theme-Bilder wurden visuell geprüft.

Inline-Texte behalten jetzt Hervorhebungen, Codezeichen und Links in ihrem
Lesefluss. Überschriften besitzen drei sichtbare Größen; Tabellen behalten die
Formatierung ihrer Zellen. Links lassen sich direkt anklicken oder mit Tab und
Enter/Leertaste aktivieren. Mehrzeilige Links erhalten auch dann stabilen Fokus,
wenn der Link höher als der kleine Lesebereich ist. Suchtreffer zählen keine
zusätzlichen, unsichtbaren Link-Aktionsblöcke.

Die Bildprüfung entdeckte einen Fehler der erweiterten Schriftpalette:
Nuklear meldete einen nur teilweise gepackten Atlas als erfolgreich. Bei
1024 × 32768 fehlten unter anderem die ASCII-Glyphen einer 30-Pixel-Schrift.
Die F1-Schriften verwenden jetzt zwei horizontale und einen vertikalen
Rasterungsschritt; das Backend prüft sichtbare ASCII-Glyphen aller geladenen
Schriften vor der Verwendung. Die ursprüngliche leere Codedarstellung
(`run-duh14r10`) wird durch die unabhängige Pixelprüfung abgelehnt.

Die abschließenden 14 Windows-GUI-Fälle bestehen in `run-1cpt5r4u`, einschließlich
F1-Eingabeisolation, 16/22/32-Pixel-Schrift, Links in Absätzen und Tabellen,
Verlauf, Codekopieren und drei Themes. Die gezielten Laufzeitbilder wurden
visuell geprüft; die Codefläche enthält tatsächlich lesbare Glyphen.
Die 29 ausgewählten nativen Dokumentations-, Referenz-, Beispiel- und
Accessibility-/Geometriefälle bestehen in `run-mji_rnhn`.

Windows besitzt jetzt einen UI-Automation-Provider für die veröffentlichten
Bedienelemente. Die C17-Brücke veröffentlicht Namen, Rollen, Fokus und
Button-/Checkbox-/Auswahl-/Slider-Aktionen über den nativen Fensteranschluss.
Ein separater UIA-Clientprozess bedient die tatsächlichen Einstellungen;
die drei Checkbox-, Auswahlgruppen- und Fokusfälle bestehen in `run-k8519wd_`.
Der Client identifiziert gleichnamige Gruppen und Textüberschriften anhand
ihrer Rolle. Entfernte oder zerstörte Elemente können keine Aktion mehr auslösen.

Mit aktiver Windows-Anbindung bestehen 16 Accessibility- und F1-GUI-Fälle
(`run-sezoviel`). Die beiden nativen Provider-/Rendererfälle bestehen zusätzlich
mit MSVC AddressSanitizer (`build/native/F1-uia-sanitized/test-results/run-ycfu0o49`).
Die Windows-Brücke und der unabhängige Client sind reguläre Testziele; ihre
Laufprotokolle verbleiben im ignorierten Buildverzeichnis.
Nach dem abschließenden Review bestehen alle fünf Windows-Provider- und
Clientfälle erneut (`run-nh1gv2ti`); die fünf Modell-/Parser-/Abschnittsfälle
bestehen in `run-shcvhfxl`. Der sichtbare Fokus des extern bedienten Buttons
wurde im Laufzeitbild geprüft. Die native ID beschreibt die aktuelle
Laufzeitidentität eines Elements; eine dauerhaft sprachunabhängige Automation-ID
wird nicht vorgetäuscht.

Die F1-Ansicht veröffentlicht jetzt zusätzlich den vollständigen Text in
Lesereihenfolge und die tatsächlich gezeichneten Zeichenpositionen. Der
portable Entwurf und der veröffentlichte Stand bleiben getrennt; Seitenwechsel
verwerfen alte Textbereiche und Scrollanfragen. Code behält seine Originalbytes;
zusätzliche Absatztrennungen liegen außerhalb der kopierbaren Codeblöcke.

Windows stellt diese Daten über den nativen TextPattern bereit. Ein separater
UIA-Client liest die echte Quellseite `docs/project-audit.json` einschließlich
aller **498605 UTF-8-Bytes** des Lesetexts. Der Vergleich mit der Originaldatei
plus den beiden Absatztrennungen ist bytegenau. Derselbe Client klont und
vergleicht Textbereiche, prüft sichtbare Geometrie und fordert einen Sprung zum
Dokumentende an. Unicode-Suche, Surrogatpaare, begrenzte Zeichenbewegung und
ungültige alte Textbereiche sind zusätzlich am nativen Provider geprüft.

Die Bildabnahme erkannte zunächst einen leeren Zielbereich trotz großer
Scrollposition. Ursache war eine abweichende Umbruchbreite zwischen Messung
und Zeichnung. Beide verwenden jetzt dieselbe Breite; die Prüfung verlangt,
dass das letzte inhaltliche Zeichen tatsächlich in den veröffentlichten
Glyphenpositionen sichtbar ist. Windows-CRLF wird als eine visuelle Zeilengrenze
behandelt und bleibt im gelesenen Quelltext erhalten. Die korrigierte Aufnahme
zeigt den echten Dokumentabschluss (`run-f6yat_2_`).

Die 19 betroffenen GUI-Fälle bestehen in `run-23izk2ps`; nach der visuellen
Korrektur bestehen Dokumentende, Absatz-/Codeformatierung und Abschnittssprung
erneut 3/3 (`run-f6yat_2_`). Die abschließenden zwei Provider-/Clientfälle bestehen
in `run-3hc3s5t2`. Zwei Provider-/App-Lesefälle bestehen mit MSVC AddressSanitizer
in `build/native/F1-uia-sanitized/test-results/run-89tndz7k`. Die portable
Dokument-/Modell-/Parsergruppe besteht 5/5 (`run-hcb0ypz3`); auch der Testkatalog
und dessen erwartete Fehlerfälle bestehen. Diese Nachweise bestätigen den
beschriebenen Leseweg, keine vollständige Screenreader-Abnahme.
Der abschließende Stand besteht zusätzlich 4/4 in `run-oxz08pfb` (Provider,
externer Leser, 22-Pixel-Tastatur und Verlauf) sowie 6/6 Modell-/Parser-/Linkfälle
in `run-u3ns2d1p`. Obere und untere Scrollausrichtung werden als unterschiedliche
native Anfragen übertragen; der gezeigte externe Leseweg prüft den Dokumentabschluss.

Die beiden F1-Suchfelder besitzen unter Windows jetzt eine Edit-Rolle, feste
Namen, ihren aktuellen UTF-8-Wert und den nativen ValuePattern. Schreibanfragen
werden ohne Kürzung validiert und genau einmal im passenden UI-Frame übernommen.
Ungültiges Unicode, Zeilenumbrüche, zu lange Werte und entfernte oder deaktivierte
Felder verändern den bisherigen Text nicht. Die native Schnittstelle meldet
Wertänderungen; Feldnamen und Laufzeit-IDs bleiben dabei stabil.

Ein unabhängiger Client bedient **Aktuelles Dokument durchsuchen** und
**Alle Inhalte durchsuchen**. Er setzt `ps_rng_seed` und den Fokus. Die App prüft
den tatsächlichen Suchzustand, zusätzliche Tastatureingabe und den Rückweg per
Tab. Haupteditoren und Simulationszeit bleiben dabei unverändert. Die Bilder
der beiden Suchansichten wurden geprüft. Eine Gegenprobe zeigte zunächst ein
fälschlich deaktiviertes Feld im inaktiven Nachbarpanel; die native Berechtigung
berücksichtigt jetzt sichtbare, nicht überdeckte Panels.

Die 20 betroffenen GUI-Fälle bestehen in `run-l9swswr9`; die externe Feldprüfung
besteht zusätzlich in `run-0cs6jmql`. Die sieben Modell-/Parser-/Linkfälle bestehen
in `run-oqc3g_rw`, einschließlich Unicode, Leerwert, verspäteter Schreibanfrage,
Kapazitätswechsel und veralteter Ziele. Provider und echter Feldablauf bestehen
mit MSVC AddressSanitizer in
`build/native/F1-uia-sanitized/test-results/run-nqs4wt1q`. Auch die Erweiterung
des Testkatalogs besteht mit den erwarteten Fehlerfällen.

Die Windows-Suchfelder veröffentlichen jetzt zusätzlich TextPattern und
TextPattern2 mit Unicode-Textbereichen, aktueller Auswahl und Cursorposition.
Die Geometrie verwendet die Zeichenbreiten, den Textursprung und den horizontalen
Versatz des echten Felds. Nur tatsächlich sichtbare Zeichen werden gemeldet.
Die Cursorposition bleibt auch bei rückwärts gerichteter Auswahl getrennt von
den sortierten Auswahlgrenzen. Text- und Auswahländerungen lösen native Ereignisse aus.

Auswahlanfragen binden sich an Feld-ID und Inhaltsversion, übernehmen UTF-8-
Skalargrenzen einmalig im UI-Frame und verfallen bei geändertem Inhalt. Der
Windows-Textcache unterscheidet Eigentümer und Version; ein nativer Gegenfall
prüft gleichzeitig Feld und Dokument mit identischer Versionsnummer.
Ein externer Client wählt `θ🌍` im echten Suchfeld aus, liest die Auswahl zurück,
prüft die aktive Cursorposition und ihre sichtbare Geometrie. Anschließende
Tastatureingabe ersetzt genau diesen Bereich. Die Laufzeitaufnahme zeigt die
Auswahl; ein nicht vorhandenes Emoji wird vom bestehenden Font als Ersatzglyph
gezeichnet, während der native Text unverändert bleibt.

Die 15 GUI-Regressionen bestehen in `run-l15ud31b`; weitere fünf Feld-,
Fokus-, Zoom- und Absatzfälle bestehen in `run-ys7t4zmb`. Die vier portablen
Dokument-/Edit-/Fokus-/Auswahlmodelle bestehen in `run-bvhbk87r`; Grundmodell
und Editor-Clipboard bestehen in `run-7fwpnj0c`. Provider und echter Unicode-
Feldablauf bestehen zusätzlich mit MSVC AddressSanitizer in
`build/native/F1-uia-sanitized/test-results/run-b3n46l3_`.
GPU-/SDL-Eingabeprüfung und externer Feldablauf bestehen abschließend 2/2 in
`run-evxuvinw`. Die Eingabeprüfung bestätigt, dass mehrzeiliges oder tabhaltiges
Einfügen eine bestehende einzeilige Auswahl unverändert lässt; der mehrzeilige
Editor behält seine Navigation sowie Undo/Redo.

Der Linux-Sanitizerlauf für `7ea0c66` ist tatsächlich fehlgeschlagen:
[Clang-GUI-Lauf](https://github.com/Lulus792/physim/actions/runs/38474528096/job/115561838320).
Die Protokolle melden `Font initialization: Invalid texture size`; 20 von 21
GUI-Fällen scheitern, darunter F1-Medien- und Abschnittsfälle. Dieser Befund
bestätigt keine Linux-Funktionsgleichheit.

Die Atlas-Packung berücksichtigt jetzt die tatsächliche GPU-Texturgrenze beider
Achsen. Bei Platzmangel wird mit breiterer Fläche erneut gepackt; jeder Glyphen-
und Cursorbereich muss Platz finden. Rasterisierungsfehler dürfen keinen
unvollständigen Atlas liefern. Die Schriftauswahl und ihre Abtastung bleiben erhalten.
Eine eingebettete Schriftprobe erzeugt einen vollständigen 2048 × 2048-Atlas,
prüft Pixel sämtlicher sichtbarer ASCII-Glyphen, kleine und nichtpotente Grenzen,
acht Allokationsfehler sowie RGBA-Konvertierungsfehler, Freigabe und erneutes Backen.

Die beiden Fontfälle bestehen unter Windows in `run-i9w2bshn` und mit MSVC ASan
in `build/native/F1-uia-sanitized/test-results/run-353uk564`. Zwölf tatsächliche
GPU-/F1-Fälle bestehen in `run-aahao1yx`, darunter 16/22/32-Pixel-Schrift,
Absätze/Code, Themes, Verlauf, native Feldauswahl und vollständiges natives Lesen.
Die Codeaufnahme wurde visuell geprüft. Testkatalog und erwartete Fehlerfälle
bestehen in `build/native/F1-review/font-limit-runner-proof`.
Der korrigierte Stand `3ab6a1b` besteht unter Ubuntu 24.04, X11/Xvfb mit GCC
und AddressSanitizer/UBSan: **14/14 Modell-/Editor-/Fontfälle** und **21/21
GUI-Fälle**, einschließlich F1-Medien/Abschnitten und der bestehenden AT-SPI-
Bedienprobe. [Ausgeführter GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38478702052/job/115575701645).
Clang besteht ebenfalls 14/14 native Fälle und die vier F1-Medien-/Abschnittsfälle.
Seine GUI-Gruppe besteht 18/21; Completion und beide Kamera-Ebenenfälle melden
denselben globalen Pufferüberlauf in der Editor-Cursorbreitenmessung.
[Ausgeführter Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38478702052/job/115575701905).
Dies ist ein begrenzter X11-Nachweis, keine Wayland- oder macOS-Abnahme.

Die Cursorursache ist ein leerer Editor mit behaltenem Pufferspeicher:
`nk_str_get_const` liefert null und wird durch `""` ersetzt, während die
Cursorabfrage noch auf den ursprünglichen Puffer zeigt. Die Subtraktion von
Zeigern unterschiedlicher Objekte erzeugt eine falsche Messlänge. Ein leerer
Editor verwendet jetzt durchgehend denselben leeren String als Text und
Cursorbasis. Unter Windows bestehen Completion, beide Kamera-Ebenenfälle und
das native F1-Feld mit MSVC ASan 4/4 in `run-9o8cfvas`. Der Stand `fce0249`
besteht anschließend unter Ubuntu 24.04/X11/Xvfb mit beiden Compilern:
Clang **14/14 native und 21/21 GUI-Fälle** (`run-4hjvqzva`, `run-cas15ah_`),
GCC ebenfalls **14/14 und 21/21** (`run-tt2lzkxk`, `run-geqdyjui`).
[Clang-Nachweis](https://github.com/Lulus792/physim/actions/runs/38479929845/job/115579741517),
[GCC-Nachweis](https://github.com/Lulus792/physim/actions/runs/38479929845/job/115579741720).
Die Linux-Artefakte des vorherigen Fontstands zeigen tatsächlich Mesa 25.2.8/
llvmpipe mit 16384 Pixel GPU-Grenze, einen 2048 × 16384-Hauptatlas und einen
4096 × 16384-F1-Atlas. Die Bilder der räumlichen F1-Erklärung wurden geprüft.

Eine Zeicheninventur der 183 registrierten Seiten findet 43 mathematische,
griechische oder hoch-/tiefgestellte Zeichen. U+1D63 (`ᵣ`) im Pendel-Tutorial
fehlte im geladenen Bereich. Die Windows-Fonts Segoe UI, Segoe UI Semibold und
Consolas enthalten laut direkter GDI-Glyphenprüfung sämtliche 43 Zeichen;
das ersetzt keine Prüfung ihrer tatsächlich gebackenen und gezeichneten Glyphen.
U+1D63 wird jetzt für Text und Code geladen. Der neue Test `documentation_math`
liest die registrierten Originalseiten erneut und prüft sämtliche so gefundenen
Zeichen in allen **38 F1-Schriftvarianten**. Er verlangt einen tatsächlichen
Codepunkt im gebackenen Atlas, einen echten Glyphenindex im Quellfont und positive
Glyphengeometrie. Die Negativprobe U+0378 besitzt zwar einen Eintrag im deklarierten
griechischen Bereich, wird aber als fehlender Quellglyph erkannt. Der vorherige
Stand scheitert für U+1D63 in allen 38 Varianten (`run-pvxb2xzn`).

Der Test öffnet das echte Pendel-Tutorial, sucht `eᵣ` und prüft seine sichtbare
Glyphenposition bei 16 und 32 Pixeln. Die Bildprüfung entdeckte zunächst einen
falschen 32-Pixel-Ausschnitt: Der Test las noch die 16-Pixel-Geometrie, während
die Ansicht nach neuem Umbruch den Treffer verlor. Die strengere Gegenprobe
prüft nach abgeschlossenen Layout-Frames und scheitert am vorherigen Zoom
(`run-sq0lwnpr`). Zoom richtet einen vorhandenen Suchtreffer jetzt nach dem
neuen Umbruch erneut aus. Beide korrigierten Laufzeitbilder zeigen `eᵣ` in der
Formel, einschließlich 32-Pixel-Leseschrift im kleinen Fenster.

Die Mathematik-, Verlauf- und Zoomfälle bestehen 3/3 in `run-edtojs15`; die
abschließenden fünf Mathematik-/Inline-/Tastatur-/nativen Feld-/Lesefälle bestehen
in `run-bqpq0966`. Mathematik und Zoom bestehen 2/2 mit MSVC ASan in
`build/native/F1-uia-sanitized/test-results/run-ob6vmb6j`. Testkatalog und erwartete
Fehlerfälle bestehen in `build/native/F1-review/math-runner-proof`.
Die abschließende Mathematikprobe einschließlich U+0378 besteht zusätzlich mit
MSVC ASan in `build/native/F1-uia-sanitized/test-results/run-7pbhh6m_`.
Der Stand `72ff73f` besteht unter Ubuntu 24.04/X11/Xvfb mit ASan/UBSan:
Clang **14/14 native und 22/22 GUI-Fälle** (`run-1im2mdks`, `run-l1tjzep9`),
GCC ebenfalls **14/14 und 22/22** (`run-aoj8jg81`, `run-jd0b_5m4`).
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38482055800/job/115586716595),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38482055800/job/115586716800).
Beide bestehen die neue Zeichendeckung. Die Linux-Artefakte bestätigen 43
Zeichen in 38 Fontvarianten; die 16-/32-Pixel-Bilder der Formel wurden visuell
geprüft. Die mathematische Zeichendeckung unter macOS bleibt offen.

Windows kann jetzt auch verdeckte Textbereiche eines F1-Suchfelds per
`ScrollIntoView` sichtbar machen. Die Anfrage bindet sich an Feld-ID und
Inhaltsversion, validiert UTF-8-Grenzen und verfällt bei geänderten, entfernten
oder deaktivierten Zielen. Sie verändert weder Text noch Auswahl oder Fokus.
Der horizontale Versatz gehört zum jeweiligen Feld und bleibt beim Zeichnen
eines unfokussierten Felds erhalten. Tastatureingabe kann den Cursor weiterhin
normal sichtbar halten. Verborgene oder minimierte Fenster erhalten keine
Scrollaktion über den Windows-Provider.

Ein unabhängiger UIA-Prozess setzt einen langen Unicode-Suchbegriff und verlässt
das Feld per Tab. Er bestätigt zunächst, dass der angeforderte Bereich außerhalb
der sichtbaren Textbereiche liegt, fordert das Ende `Ziel ᵣ` und danach den Anfang
`Anfang θ` an und verlangt deren vollständige Zugehörigkeit zu sichtbaren
Textbereichen. Wert, beide Auswahlendpunkte, Cursorposition, aktiver Caret-Zustand
und Tastaturfokus bleiben gleich. Die App bestätigt zusätzlich den erhaltenen
Fokus auf „Vorheriger“. Beide Laufzeitbilder wurden geprüft.

Die vier GPU-/nativen Feld-/Lesefälle bestehen in `run-zetj65o2`; die neun
betroffenen Provider-/UI-/F1-Fälle bestehen in `run-zpr20ya4`. Fünf Modell-/Editorfälle
bestehen in `run-1xc1a_c6`, einschließlich Unicode-Grenzen, Inhaltswechsel,
einmaliger Auslieferung und dem tatsächlichen Zeichenversatz eines unfokussierten
Felds bei unverändertem Fokus/Auswahl des anderen Felds. Provider und echter
Feldscroll bestehen mit MSVC ASan 2/2 in `run-li5put2z`; Modell und Editorprimitive
bestehen dort ebenfalls 2/2 in `run-mptcb7p_`. Beide ASan-Läufe liegen unter
`build/native/F1-uia-sanitized/test-results`. Testkatalog und erwartete Fehlerfälle
bestehen in `build/native/F1-review/edit-scroll-runner-proof`.
Die abschließende Modellprobe für deaktivierte, entfernte und nach dem Zeichnen
queued Ziele besteht mit MSVC ASan in `run-t96ly68b`.

Der Stand `5ba0638` besteht inzwischen auch unter Ubuntu 24.04/X11/Xvfb mit
ASan/UBSan: Clang **15/15 native und 22/22 GUI-Fälle** (`run-349e8nv_`,
`run-otthh0xf`), GCC ebenfalls **15/15 und 22/22** (`run-o1y6jb2l`,
`run-wgjwn5yp`).
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38485919616/job/115599421224),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38485919616/job/115599421568).
Der neue Editor-Scrolltest gehört zu den nativen Fällen. Diese Läufe prüfen
keinen Windows-UIA-Provider unter Linux.

Zoom ohne Suchbegriff bewahrt jetzt eine Textstelle aus dem veröffentlichten
sichtbaren Lesebereich. Vor dem Umbruch wird deren UTF-8-Position gebunden an die
Inhaltsversion vorgemerkt; das neue Layout richtet diese Textstelle aus.
Mehrere Zoomtasten vor dem nächsten Bild ersetzen den ursprünglichen Anker
nicht. Eine bereits vorgemerkte native Scrollaktion behält Vorrang. Ohne
sichtbare Textgeometrie bleibt die bisherige Pixel-Skalierung als Fallback.
Dies verspricht keine identische Pixelposition oder Anker für rein grafische
Inhalte.

Die Gegenprobe `run-vpdclvrf` verliert ohne Suchbegriff die Formel nach dem
Zoom auf 32 Pixel. Die Korrektur besteht die erweiterte Mathematikprobe in
`run-t_vcy24o`: Nach abgeschlossenen Layout-Frames ist die Formel mit und ohne
Suche sichtbar, einschließlich Rückkehr zu 16 Pixeln. Alle drei zusätzlichen
Lesebilder wurden visuell geprüft. Der Modellfall prüft Versionsbindung,
fehlende/verborgene Geometrie, Unicode-Grenzen, unabhängige Boxreihenfolge und
den Vorrang bestehender Scrollanfragen (`run-z87vr1aw`). Fünf betroffene
Tastatur-/Verlauf-/Zoom-/native Feld-/Lesefälle bestehen in `run-kww751ua`.
Die MSVC-ASan-Läufe bestehen **1/1 Modellfall** (`run-5xw41dxo`) und **2/2
Mathematik-/Zoomfälle** (`run-1_js51y5`) unter
`build/native/F1-uia-sanitized/test-results`. Die Prüfung des Testläufers mit
erwarteten Gegenfehlern besteht in
`build/native/F1-review/reading-anchor-runner-proof`.
Der Stand `e676b92` besteht inzwischen unter Ubuntu 24.04/X11/Xvfb mit
ASan/UBSan: Clang **15/15 native und 22/22 GUI-Fälle** (`run-sygycws1`,
`run-h3lzuif4`), GCC ebenfalls **15/15 und 22/22** (`run-m5mr0erm`,
`run-yhfoibhp`).
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38487336229/job/115604045409),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38487336229/job/115604045648).
Die neue Lesepositionsprobe gehört zu beiden GUI-Läufen. macOS bleibt offen.

Interne F1-Links besitzen jetzt eine eigene native Rolle und einen versionierten
Bereich im vollständigen Lesetext. Ihr Name entspricht dem Linktext. Die
Publikation enthält auch Links außerhalb des sichtbaren Ausschnitts; ihre
Identität bleibt beim Scrollen erhalten. Native Fokusanforderungen machen die
zugehörige Textstelle sichtbar. Ein anderes Dokument erhält neue Linkidentitäten.
Verdeckte oder deaktivierte Ziele nehmen keine native Fokus-/Aktivierungsaktion
an. Die Aktion „Quellenadresse kopieren“ bleibt als Button veröffentlicht.

Windows `TextPattern` liefert die eingebetteten Links über `GetChildren` in
Lesereihenfolge, auch bei teilweise überlappenden Bereichen. `RangeFromChild`
liefert ihre exakten UTF-16-Grenzen; `GetEnclosingElement` erkennt den Link auch
innerhalb seines Textbereichs. Ein Linkbereich enthält keine weiteren Kinder.
Fremde Controls, nicht zugeordnete Elemente und veraltete Bereiche werden
abgewiesen. Diese Zuordnung folgt den Microsoft-Verträgen für
[Hyperlink](https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-supporthyperlinkcontroltype),
[RangeFromChild](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextprovider-rangefromchild)
und [GetChildren](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextrangeprovider-getchildren).

Die Inventur parst alle **183 registrierten Originalseiten** und veröffentlicht
ihre **1.058 internen Links**, höchstens **119 pro Seite**, in einem separaten
Modell. Linkname und Linkbereich entsprechen exakt dem Text der Leserfassung; das Modell
behält mindestens 64 Plätze für andere Bedienelemente. Die Inventur läuft auch
im plattformübergreifenden Mathematikfall. AXLink und AT-SPI-Linkrolle sind
zugeordnet; native Textbereichszuordnung auf diesen Plattformen bleibt offen.

Der unabhängige Windows-UIA-Prozess prüft im echten F1-Fenster zwei Links in
Absatz und Tabelle, ihre Rolle, Reihenfolge, exakten Textbereiche und
umschließenden Elemente. Er bestätigt zunächst den verdeckten Tabellenlink,
fokussiert ihn, verlangt sichtbare Glyphen und öffnet darüber den C-Lernweg.
Danach ist der alte Textbereich ungültig. Die App bestätigt den Zielartikel,
Rückkehr zum Arbeitsbereich sowie unveränderte Editorinhalte und Simulationszeit.
Die Bildprüfung korrigierte einen zunächst zu engen Fokusrahmen um „C“; das
abschließende Bild zeigt das Zeichen lesbar im eigenen Rahmen.

Modell und Parser bestehen 2/2 in `run-horbig68`; die fünf Provider-/externen
Link-/nativen Lese-/Mathematik-/Inline-Fälle bestehen in `run-guf9qwrb`.
Vier betroffene Modellfälle bestehen nach der abschließenden Identitätsprüfung
in `run-3mgmcufa`. MSVC ASan besteht **2/2 Modell-/Parserfälle**
(`run-6fjr53xr`), **3/3 Provider-/Render-/echte Linkfälle** (`run-e54z9n9s`),
die zusätzliche reale Renderprüfung verdeckter offscreen Links
(`run-x44glzgj`) und die abschließende vollständige Inventur mit Mathematik/Zoom
(`run-xe4xlwwk`). Diese ASan-Läufe liegen unter
`build/native/F1-uia-sanitized/test-results`. Testkatalog und erwartete
Fehlerfälle bestehen in `build/native/F1-review/link-runner-proof`.
Die abschließende Provider-/Clientprüfung einschließlich verschiedener nichtleerer
Automation-IDs besteht unter MSVC ASan zusätzlich 2/2 (`run-5zghefgs`).
Die strengere Inventur einschließlich vollständiger Linknamen besteht unter
MSVC ASan in `run-uvy0j5b0`.
Die erweiterte Linux-Prüfung enthält die neuen Modelle sowie Inline-Links,
Verlauf und Zoom; eine Workflowdefinition bestätigt deren Ausführung noch nicht.

Der Stand `38dde0b` wurde unter Ubuntu 24.04/X11/Xvfb tatsächlich mit ASan/UBSan
ausgeführt: GCC besteht **19/19 native und 24/25 GUI-Fälle** (`run-63n96l08`,
`run-vvzfn_n2`); Clang ebenfalls **19/19 und 24/25** (`run-hcesvfom`,
`run-k0fed1p5`).
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38489455686/job/115610946220),
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38489455686/job/115610946434).
Der zusätzliche Zoomfall prüfte unmittelbar nach der asynchronen Größenänderung
noch die Leseflächenhöhe des vorigen Bildes (250 Pixel). Das archivierte
1080 × 780-Pixel-Bild zeigt bereits das korrekt vergrößerte Layout. Die kleine
Zoomansicht und die Formel ohne Suchbegriff wurden ebenfalls visuell geprüft.
Die Korrektur synchronisiert beide Größenanforderungen mit `SDL_SyncWindow`
und wartet abgeschlossene Layout-Frames ab. Sie senkt keinen Geometrie-Grenzwert.
Jedes Zoom-Bild wird jetzt gegen die protokollierte tatsächliche logische Größe,
Drawable-Pixelgröße, Schriftgröße und Leseflächenhöhe geprüft. Das entspricht
dem [SDL-Vertrag für Größenänderungen](https://wiki.libsdl.org/SDL3/SDL_SetWindowSize).
Die korrigierte Linux-Ausführung stand an diesem Zwischenschritt noch aus.

Der korrigierte Stand `fa3e8ac` besteht inzwischen unter Ubuntu 24.04/X11/Xvfb
mit ASan/UBSan: GCC **19/19 native und 25/25 GUI-Fälle** (`run-ex_l712d`,
`run-r0gu5ywa`), Clang ebenfalls **19/19 und 25/25** (`run-0bqee4bo`,
`run-hrzfzoz5`).
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38493455260/job/115623771433),
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38493455260/job/115623771688).
Beide bestehen den strengeren Zoomfall einschließlich tatsächlicher Bildgrößen.

Windows meldet jetzt Änderungen der Linkgeometrie, Sichtbarkeit und Bedienbarkeit
mit typisierten UIA-Property-Ereignissen. Die Geometrie berücksichtigt auch eine
Verschiebung des nativen Fensters bei unverändertem Dokumentlayout. Ein nativ
deaktiviertes Fenster sperrt Fokus, Aktivierung und die Übernahme von
Widget-Aufträgen. Die Ereigniserzeugung hält Providerreferenzen, gibt den
Modellmutex vor nativen Aufrufen frei und meldet keine unveränderten Werte.

Maus, Tastatur und native Linkaktivierung verwenden denselben Abschlussweg.
`Invoked` wird nach erfolgreicher Navigation gesendet, während der veröffentlichte
Quelllink noch verfügbar ist. Annahme eines Auftrags, deaktivierte Ziele und
fehlgeschlagene Navigation erzeugen keine erfolgreiche Aktivierungsmeldung.
Der Client registriert seine Ereignisse mit gecachter Automation-ID, sodass er
den nach der Navigation verschwindenden Quelllink zuordnen kann. Dieser Ablauf
folgt den [Microsoft-Invoke-Konventionen](https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-implementinginvoke).

Die unabhängige Ereignisprobe scheitert am vorherigen Stand (`run-r9fl1j38`):
Der Link wird sichtbar, aber Geometrie- und Sichtbarkeitsereignisse fehlen.
Die Korrektur besteht zunächst in `run-pyylvgh5`. Die erweiterte Probe deaktiviert
und reaktiviert das tatsächliche Hostfenster, verlangt beide Zustandsereignisse,
weist eine Aktivierung während der Deaktivierung ab und verschiebt das Fenster
um 13/7 Pixel. Native Geometrie und Ereignis bestätigen die Verschiebung;
unveränderte Folgeframes erzeugen keine zusätzlichen Meldungen (`run-8xmxztfz`).

Ein zusätzlicher ungültiger Link zeigt den Fehler jetzt im F1-Fenster selbst.
Der Client wartet auf diesen nativen Fehlertext und bestätigt einen weiterhin
gültigen Quellbereich ohne Invoked-Ereignis; die App bestätigt unveränderte
Seite und Inhaltsversion. Die anschließende erfolgreiche Navigation meldet
genau eine Aktivierung und invalidiert den alten Bereich. Die Fehleransicht
wurde visuell geprüft. Ereignisse, strenger Zoom und Inline-Links bestehen
3/3 in `run-59gvjw_h`; die fünf betroffenen nativen Feld-/Scroll-/Lese-/Verlauf-/
Mathematikfälle bestehen in `run-7eu3zujr`. Die vier Provider-/Render-/externen
Ereignis-/Zoomfälle bestehen mit MSVC ASan in
`build/native/F1-uia-sanitized/test-results/run-1b5w42va`.
Der Testläufer samt erwarteten Gegenfehlern besteht in
`build/native/F1-review/link-events-runner-proof`.

Beim Verbergen oder Minimieren bleibt der veröffentlichte Lesetext erhalten,
aber das Modell setzt native Aufträge und Fokus aus. Wartende Text-, Wert-,
Auswahl-, Scroll-, Fokus- und Aktivierungsaufträge werden verworfen. Erst eine
vollständig gezeichnete neue Publikation gibt die Aktionen wieder frei.
Inhalte, Inhaltsversionen und Elementidentitäten bleiben über diese Pause
erhalten. Das Fenster benötigt für diese Zustandsmeldung keinen neuen GPU-Frame.

Windows liefert in diesem Zustand keine sichtbare Glyphengeometrie und nur
einen leeren, degenerierten sichtbaren Textbereich. Fokus und aktiver Caret
sind aus; Fokus- und Aktivierungsanforderungen werden abgewiesen. Dies folgt
den Verträgen für [GetVisibleRanges](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextprovider-getvisibleranges)
und [GetBoundingRectangles](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextrangeprovider-getboundingrectangles).
Das Lesen des erhaltenen Gesamttexts ist weiterhin möglich.

Der unabhängige Client hält dieselben Dokument-, Feld-, Link- und Textbereichs-
objekte über Escape/Schließen, Wiederöffnen, echtes Win32-Minimieren und
Wiederherstellen. Die Gegenprobe `run-v0zb0uxy` findet nach Schließen noch
aktiven Feldfokus; `run-hh37coiv` zeigt fehlende Sichtbarkeitsereignisse.
Die erste Korrektur besteht in `run-gztif0t0`. Die strengere Prüfung der
Wiederöffnung entdeckt anschließend einen alten Nuklear-Editierzustand
(`run-60i5pyfg`). Dessen ausdrückliche Rücksetzung verhindert erneuten Fokus
aus dem vorherigen Fensterzustand.

Die finale Probe bestätigt zwei Hidden- und zwei Shown-Ereignisse, keinen
Invoked-Event, unveränderten Gesamttext, Unicode-Feldwert und Inhaltsversion,
leere verborgene Textgeometrie, abgewiesene verborgene Aktionen und erhaltene
Elementidentitäten. Editor und Simulation bleiben gleich. Die Bildprüfung
entdeckte zunächst eine zu frühe Minimierung vor der Wiederöffnungsaufnahme;
der Client wartet nun auf die bestätigte Aufnahme. Beide finalen Bilder wurden
geprüft. Der ergänzte Modellfall prüft rückgenommene Aufträge, Aktionen während
der Pause, erhaltene Werte/IDs und Wiederfreigabe erst nach der Publikation.

Lebenszyklus und Linkereignisse bestehen 2/2 in `run-ysg64uez`; fünf betroffene
Modellfälle bestehen in `run-cgltr8ud`. Die drei Modell-/Feld-/Lesefälle bestehen
mit MSVC ASan in `run-v_fnovl6`; die abschließenden vier Provider-/Render-/
Lebenszyklus-/Linkfälle bestehen dort in `run-h32gapm8`. Die ASan-Ergebnisse
liegen unter `build/native/F1-uia-sanitized/test-results`. Vier weitere
Tastatur-/native Feld-/Lese-/Zoomfälle bestehen in `run-b824u53x`. Testkatalog
und erwartete Gegenfehler bestehen in `build/native/F1-review/lifecycle-runner-proof`.
Ein vorheriger ASan-Lauf verlor nach dem externen Fensterverschieben den
Linkfokus (`run-ke7exiiw`); der Client bestätigt den Fokus jetzt auch am Ende
seiner Bewegungs-/Idle-Prüfung. Der abschließende vollständige Fallverbund
und das Paar `run-tlbbydis` bestehen. Dies ersetzt keine vollständige
Fenster-/Monitor-/DPI-Matrix oder echte Screenreader-Abnahme.

Die Lebenszyklusbilder zeigen zusätzlich ein Ersatzzeichen für `🌍` im
Suchfeld, obwohl der native UTF-8-/UTF-16-Wert erhalten bleibt. Der Codepunkt
U+1F30D gehört nicht zu den aktuell geladenen Fontbereichen. Die mathematische
Inventur bleibt gültig, bestätigt aber keine solche Zeichenabdeckung.
Schriftfallback und eine umfassendere visuelle Zeichenprüfung bleiben offen.

Offen sind vollständige assistive Feldbedienung und ihre native
macOS-/Linux-Anbindung, native Linktextbereiche auf macOS/Linux, native
vollständige Fenster-/Monitor-Ereignismatrix auf allen Zielplattformen,
Schriftfallback für weitere Unicode-Zeichen, Tabellensemantik, vollständiges natives
Lesen auf macOS/Linux, praktische Screenreader-Abnahme
und die Abnahmematrix für Dichte, Themes sowie Linux und macOS. Diese Liste
beschreibt Restarbeit und erteilt keine Produktfreigabe.

## Ergänzter Auftrag zum Formelsatz

Am 15. Oktober 2026 verlangt der Nutzer für F1 einen sauberen Formelsatz wie
bei LaTeX. Brüche, Wurzeln, Hoch-/Tiefstellungen, Integrale und mathematische
Symbole sollen typografisch gesetzt werden. Die bisherige Unicode-/Codezeile
ist dafür kein ausreichender Nachweis. Zum Auftragszeitpunkt besaß der App-Renderer noch
keinen TeX-Formelsatz. Die folgende erste Umsetzung ergänzt ihn; die vollständige
Seitenübertragung sowie Dichte- und assistive Abnahme gehören zur weiteren Arbeit.

## Bestätigte macOS-Seitenleistenkorrektur vom 15. Oktober 2026

Revision `64f302cecb3446fb66850231fe396df6e04d5e2a` wurde im
[macOS-Lauf 38794037819](https://github.com/Lulus792/physim/actions/runs/38794037819)
auf Intel und ARM tatsächlich gebaut und geprüft (Darwin 24.6.0,
LLVM 20, ASan/UBSan, SDL 3.2.30). Intel besteht 11/11 native Prüfungen
(`run-4bdc4ueu`) und 13/13 GUI-Fälle (`run-be2p4_p1`); ARM besteht
11/11 (`run-nfsi1se7`) und 13/13 (`run-a3rd00dk`). Die vollständigen Joblogs
und beide Archive wurden gelesen; die positiven 32-px-Aufnahmen visuell geprüft.
Die beiden Navigationsschalter stehen dort vollständig auf eigenen Zeilen.
Alle zwölf Kombinationen aus Schalter, Schriftgröße und Theme bestehen.

Die isolierte Gegenprobe setzt nur die alte zweispaltige Schalteranordnung
wieder ein und behält den neuen Prüfer bei. Auf beiden echten Mac-Architekturen
scheitert sie mit `font=32 label=Themen lines=2 complete=0`, Exit 1.
Die Archive bestätigen `precise_rejection=true`. Dies ist ein Gegenbeispiel
für das alte Layout, kein vollständiger historischer Programm-Build.
Die Archiv-SHA256 sind ARM
`5db028705576161f9de0557ca3127c7fa0b47549b2dd3d673f86fcda0c1648b4` und Intel
`266bd2680acaba718aa15f56c4b1aafe30cc16d19abd038f9cda1abe9caa63d3`.
Dies bestätigt diesen Navigationsfehler; externe AX-Clients, VoiceOver,
Monitorwechsel und die vollständige Dokumentationsabnahme bleiben gesondert offen.

## Erste native Umsetzung des LaTeX-artigen Formelsatzes

Sechs unterschiedliche Pendelformeln stehen jetzt als echte Displayformeln im
User Guide und Pendel-Lernpfad (sieben veröffentlichte Blöcke). MathJax 4.1.3
setzt TeX mit NewCM-Zeichendaten für sieben Breiten. SVG-Originale und begrenzte
komprimierte Alpharaster sind versioniert; die installierte C17-App braucht kein
Node, TeX, JavaScript oder Netzwerk. Das Maintainer-Verfahren steht in
`tools/math/README.md`, Herkunft und Apache-2.0-Lizenz in `docs/formulas`.

Die native Darstellung verwendet den F1-Grafikkontext, einen Cache von maximal
16 Texturen, flächentreue Verkleinerung und die Textfarbe des aktuellen Themes.
Die Dateilänge, Entpackgrenzen, vollständiger Streamverbrauch und CRC werden vor
GPU-Upload geprüft. Der native Dokumenttext enthält eine lineare Unicode-
Alternative in Lesereihenfolge. Das ist keine strukturierte MathML-Navigation
und keine praktische Screenreader-Abnahme. Unbekanntes TeX bleibt als Quelltext
lesbar; Codeblöcke bleiben unverändert und ungeschlossene Displays sind ungültig.

Tatsächlich ausgeführte Prüfungen am Windows-MSVC-ASan-Build: Parser, vollständige
Dokumentationslinks und API-Referenz 3/3 in `run-wyk_5ozh`; GUI-Mathematik,
Zoom und neuer Formelsatz 3/3 in `run-4pv1cqnn`. Die neue GUI-Probe verlangt
36 vollständig sichtbare echte Formelbilder: sechs Formeln, 16/32 px und drei
Themes, passende native Textbereiche sowie unveränderten Editor/Simulation.
Die Pixelauswertung verlangt tatsächliche sichtbare Tinte; Korruptions- und
Größengegenproben sowie eine unabhängige Flächenmittelung sind enthalten.

Linux wurde lokal mit GCC, ASan/UBSan, SDL 3.2.30, Mesa-Software-GL, X11/Xvfb
und Openbox tatsächlich ausgeführt: Parser/Links 2/2 in `run-dawvrpn8`, GUI
3/3 in `run-uilfjh8t`. Reale Windows-Bilder aller sechs Formeln in allen drei
Themes und Linux-Bilder bei 32 px wurden visuell geprüft. Die erste Linux-
Kompilation erkannte einen überzähligen zlib-Funktionsparameter, den MSVC
akzeptiert hatte; der korrigierte Aufruf besteht auf beiden Plattformen.
Die anfängliche Zoom-GUI-Probe prüfte noch die vor dem Umbruch gespeicherte
Leseposition. Sie wartet nun auf abgeschlossene Positionserhaltung und fordert
erst danach das jeweilige Formelziel an.

`tools/math/check.py` prüft alle 42 Varianten, die Quell-/SVG-/Asset-Hashes,
zlib-Abschluss, Abmessungen, CRC, den nativen Katalog und sämtliche publizierten
Displayblöcke ohne Node oder Netzwerk. Checkout-Attribute erhalten die gehashten
Bytes. Paketierung enthält die Assets, Lizenzen und den benötigten nativen
Katalog; eine tatsächliche neue Paketabnahme steht noch aus. macOS, echte
Dichte-/Monitorwechsel, Inline-TeX und die übrigen mathematischen Seiten bleiben
für diesen neuen Formelsatz ausdrücklich offen. Die CI nimmt die neue GUI-Probe
in ihre echten Linux- und macOS-Läufe auf.

## Erweiterter Formelsatz: sieben Seiten und 67 Displayblöcke

Der Katalog enthält jetzt 66 unterschiedliche mathematische Ausdrücke mit 462
Breitenvarianten. Die sieben Seiten sind User Guide, Pendel, Vakuumwurf,
Wurf mit Widerstand, Material/Medium, Feder–Masse–Dämpfer und zentraler Stoß.
Die 28 nichtmathematischen Codeblöcke der sechs überarbeiteten Lernseiten wurden
mit Revision `cd8825081d5c6e956f089e4b4ffd8b4bd38956ed` verglichen und sind
unverändert. Mathematische Symbole und Modellwerte bleiben inhaltlich gleich;
kompakte Kraft-/Energiesymbole im Materialmodell werden ausdrücklich erklärt.

`documentation_formula_catalog` öffnet jede dieser sieben Seiten im echten
F1-Fenster bei 1100 × 800 und 16 px, steuert jeden Formelblock an und prüft
vollständig sichtbare Formelbilder und native Lesebereiche, die echte Reihenfolge der 67 linearen
Lesealternativen, sichtbare Formelzeichen in allen 67 Aufnahmen sowie erhaltenen
Editor-/Simulationszustand. Die getrennte 36-Bilder-Matrix bleibt bei 760 × 540,
16/32 px und allen drei Themes erhalten. Der Pixelprüfer akzeptiert das reale
Integralbild und verwirft eine separat erzeugte gleichförmige Gegenaufnahme.
Alle sieben Windows-Formelübersichten wurden visuell geprüft.

MSVC mit ASan besteht beide neuen GUI-Fälle in `run-e9hupjak`, Parser/Links/
API-Referenz 3/3 in `run-tki6yg1_`. Linux GCC mit ASan/UBSan besteht Parser/Links
2/2 in `run-56alvua1` und Mathematik/Zoom/Formelsatz/Katalog 4/4 in
`run-crc_7ul9` (SDL 3.2.30, Mesa-Software-GL, X11/Xvfb, Openbox). Ein echtes
Linux-Bild mit Stokes-Referenz und Energiegleichungen wurde zusätzlich geprüft.
Die Prüferausführung bleibt innerhalb ihrer vorhandenen 90/120-Sekunden-Grenzen.
Testkatalog und erwartete Läuferfehler bestehen in
`build/native/formula-catalog-runner-proof2`.

Die erste Renderer-Revision `cd88250` wurde außerdem als Windows-Release-Paket
gebaut, in `F1 Formula Paket ä` verschoben und erneut gestartet. Alle 601
Paketprüfsummen und die 42 Formelvarianten stimmen; die native 36-Bilder-Matrix
besteht aus diesem verschobenen Paket. Die neue 66-Ausdruck-Erweiterung ist in
jenem historischen Paket noch nicht enthalten und benötigt eine eigene neue
Paketabnahme.

Auf macOS ARM wurde dieselbe erste Renderer-Revision im
[Lauf 38798322026](https://github.com/Lulus792/physim/actions/runs/38798322026)
tatsächlich mit LLVM 20, ASan/UBSan und SDL 3.2.30 geprüft: native Verträge
11/11 (`run-lz1zfjd3`), GUI 14/14 (`run-kbmbjvhh`), darunter alle 36 echten
Formelansichten. Joblog und Archiv wurden gelesen, Integral- und Energiebild
bei 32 px visuell geprüft. Archiv-SHA256:
`6f4d0e58ee25ee4b6df236bd42ea846967f1e47881ae4c8e835a48f443aad057`.
Intel und die vollständige neue 67-Blöcke-Matrix werden hiermit noch nicht
als bestanden behauptet. Die übrigen Fachseiten, Inline-Formeln, reale
Dichte-/Monitorwechsel und praktische mathematische Screenreader-Aufgaben
bleiben offen; die konservative Gesamtbewertung bleibt bestehen.

## Fachseiten und schmale Gitterformeln vom 15. Oktober 2026

Thermodynamik, Elektromagnetismus, Wellen/Optik und Strömung verwenden ebenfalls
registrierte TeX-Displays. Insgesamt sind 118 Ausdrücke mit 826 Breitenvarianten
in 119 Blöcken auf elf Seiten enthalten. Verwendet wurden zusätzlich die
Abgleichregeln des zentralen `physics-engineer`: tatsächliche C-Implementierung,
Einheiten, Grenzen und Referenztests statt einer Genauigkeitsbehauptung aus dem
Schriftbild. Alle 20 Codeblöcke der vier Fachseiten sind gegenüber `05b1d79`
unverändert. Die numerische Tracerreferenz bezeichnet ausdrücklich SI-Zahlenwerte.

Die diskrete Saitenenergie wurde direkt gegen `examples/documentation/string_main.c`
geprüft: Geschwindigkeiten für j = 1 bis N−2, Kantenterme für j = 0 bis N−2.
Die Formel erklärt jetzt beide Summengrenzen, die Kantendifferenz und die
Geschwindigkeit zwischen den Zeitlagen n−1 und n. Die lange Gesamtsumme hätte
in schmalen Varianten bei 32 px bis zu 386 Pixel Höhe beansprucht. Ihre
algebraisch gleichwertige Aufteilung in E_K und E_G erhält das Modell und passt
in die kleine Ansicht. Diese Terme sind ausdrücklich eine diskrete Invariante,
keine beliebig abgetastete kontinuierliche Energie.

Die native Zusatzprobe `documentation_formula_physics` öffnet die vier neuen
Seiten bei 1100 × 800 und prüft alle 52 Formelblöcke samt echten Bildern und
exakter linearer Lesereihenfolge. `documentation_formula_narrow` prüft den
Gitterzeitschritt, beide Energieterme und die Zwischengeschwindigkeit bei
760 × 540 und 32 px. Alle vier Bilder müssen vollständig im realen Clip liegen
und sichtbare Formelzeichen enthalten. Die bisherigen 67-Blöcke- und
36-Zoom-/Theme-Prüfungen bleiben erhalten. Die Grenzwerte bleiben 90 Sekunden
für die App und 120 Sekunden für den umgebenden Fall.

MSVC mit ASan besteht Thermodynamik, Elektromagnetismus, Wellen/Optik, Strömung,
alle vier vollständigen Tutorials sowie Parser/Links 10/10 in `run-adnoq2hf`.
Nach der Energieaufteilung bestehen alle vier GUI-Fälle in `run-oc5xp64i`:
36 + 67 + 52 + 4 tatsächliche Aufnahmen. Linux GCC mit ASan/UBSan besteht
Parser/Links und die vier Core-Domänen 6/6 in `run-lb0rqxqw`; alle vier GUI-Fälle
bestehen in `run-vv3tcbgp` (SDL 3.2.30, Mesa-Software-GL, X11/Xvfb, Openbox).
Alle vier Windows-Formelübersichten vor der Aufteilung, die vier finalen
Windows-Bilder der kleinen Ansicht und die finalen Linux-Bilder für Gitter-
und Gradiententerm wurden visuell geprüft. Der Testläufer mit seinen erwarteten
Fehlerfällen besteht in `build/native/formula-narrow-runner-proof`.

Die erste Renderer-Revision `cd88250` ist nun auch auf macOS Intel tatsächlich
bestätigt: `run-tbxaqu2c` besteht 11/11 native Verträge, `run-51nffbp4` besteht
14/14 GUI-Fälle einschließlich aller 36 Formelansichten. Joblog und vollständiges
Archiv aus Lauf 38798322026 wurden gelesen; Integral- und Energiebild bei 32 px
wurden geprüft. Archiv-SHA256:
`275d73f7a1f53f4f83e99384fc9e9540b15200d13ec6853e54524e9610748184`.
Damit ist der erste Renderer auf Intel und ARM belegt. Die aktuelle Erweiterung
auf 119 Blöcke und die neue kleine Gitterprobe benötigen weiterhin ihre eigenen
macOS-Läufe; der historische Nachweis wird dafür nicht umgedeutet.

Die Mac-Aufnahmen der ersten Renderer-Revision zeigen zudem eine teilweise
abgeschnittene Metazeile im Kopf des Dokumentbereichs. Ein eigener Abgleich
von Messbreite, Padding und tatsächlichen Textzeichenbefehlen steht dafür aus.
Die neue Formelprobe bestätigt die Formelbilder, nicht die gesamte Kopfzeile.
Weitere mathematische Fachseiten, Inline-TeX, echte Monitor-/Dichtewechsel,
praktische Screenreader-Aufgaben und die vollständige Inhaltsprüfung bleiben
Bestandteil der F1-Abnahme. Die Gesamtbewertung wird deshalb nicht aufgewertet.

## Vollständige Kopfzeile und Schriftzeichen vom 15. Oktober 2026

Eine unabhängige Zeichen-/Clipprobe bestätigt den Kopfzeilenfehler unter Linux:
`run-7upviqou` verwirft Fall 2 bei 760 × 540 und 16 px. Alle 64 Textbytes der
Linkfehlermeldung werden ausgegeben, aber das letzte Zeichen liegt außerhalb
des tatsächlichen Clips. Die Windows-Ausgangsfassung besteht dieselben 36
Zustände (`run-giwviyp4`). Ein separater nativer Nuklear-Reproducer mit 10 Pixel
Zeichenbreite und 15 Pixel Platz liefert für „px“ zwei Zeichen mit tatsächlichen
20 Pixeln, aber gemeldeten 10 Pixeln. Das erklärt die bisher unzureichende
Breitenprüfung.

Die Metazeile verwendet jetzt dieselbe begrenzte UTF-8-Zeilenaufteilung für
Messung und Ausgabe auf der tatsächlichen Widgetbreite. Zahl und Einheit
bleiben durch ein geschütztes Leerzeichen zusammen. Eine eigene API erhält
die semantische Nur-Lese-Beschriftung auch bei manuellem Zeichnen. Die Probe
kontrolliert tatsächliche Glyphenpositionen, vollständige Textbytes und die
zusammenhängende Einheit. Sie prüft Normaltext, Suche und echten ungültigen
internen Link bei zwei Fenstergrößen, zwei Schriftgrößen und drei Themes:
insgesamt 36 Zustände.

Die isolierte Gegenprobe ersetzt ausschließlich die neue Metazeilenausgabe
durch die vorige Ausgabe und erhält den unabhängigen Prüfer. Unter Linux
wird erneut Fall 2 präzise verworfen; ein beliebiger Programmfehler genügt
dem Gegenprüfer nicht. Bericht: `build/native/detail-counterexample/report.json`,
Quell-SHA256 `6be9c1e2fa5b247aad15d762d9ce2d560366cc60ea91b58aeb2ddd03c8d0c5eb`,
Binär-SHA256 `ceb79720114518c4762674df251b96914e7e2cecd374ce04fad05ce8d0ff58b5`.

Der tatsächliche macOS-Lauf von `b23aff1`
([38804963034](https://github.com/Lulus792/physim/actions/runs/38804963034))
hat zusätzlich U+1D65 „ᵥ“ im neuen Thermodynamik-Fließtext als fehlend auf
beiden Architekturen erkannt. Native Verträge bestehen jeweils 11/11;
GUI besteht ARM 16/17 und Intel 15/17. Auf Intel erreicht der 67-Formel-Test
52 vollständige Zustände vor der unveränderten internen 60-Sekunden-Grenze.
Das ist kein Beleg für einen Hänger. Das Intel-Archiv wurde gelesen und mit
SHA256 `9c41f65886faf824c2828fd56a9295dfa964e5a3b023a4baead8e40f099f162f`
abgeglichen.

U+1D65 ist jetzt ausdrücklich in allen drei relevanten Schriftbereichen
enthalten. Die Quellinventur verlangt weiterhin beide tiefgestellten Zeichen
ᵣ und ᵥ; Fontquellen-Test und sichtbare Zeichenprobe decken ᵥ zusätzlich ab.
Der Formelkatalog wartet auf eine abgeschlossene Veröffentlichung und einen
verbrauchten Sprung statt auf drei zusätzliche identische Frames. Alle
67 + 52 + 4 Formelzustände, Zeichen-/Clipprüfungen und Zeitgrenzen bleiben
erhalten. Die Dauer wird im Abschlussmarker aufgezeichnet.

Mit finalem Quellstand bestehen unter MSVC/ASan Fontquellen/-faces 2/2
(`run-3yhog087`) sowie Mathematik, Katalog, Fachformeln, kleine Gitteransicht
und Metazeile 5/5 (`run-otmpw978`). Unter Linux GCC/ASan/UBSan bestehen
dieselben nativen Fälle 2/2 (`run-1sftfrx9`) und GUI-Fälle 5/5
(`run-2vskm3as`). Die Mathematikinventur prüft 166 druckbare Zeichen in
38 Schriftvarianten mit normaler und gebündelter Primärschrift. Die
Windows-Katalogprüfung benötigt 14,118 Sekunden. Navigation, Seitenleiste,
Metazeile, schmale Formeln und externe native Links bestehen zusätzlich
5/5 unter Windows (`run-q4l1zcte`) und Linux (`run-4buziqln`);
Windows-Accessibility besteht 2/2 (`run-ci4s8r5q`). Finale Kopfzeilen- und
Zeichenbilder beider Plattformen wurden visuell geprüft.

Die macOS-CI enthält jetzt die positive 36-Zustände-Probe und die isolierte
negative Gegenprobe. Für diese Revision steht die reale macOS-Ausführung noch
aus. Lokale Ergebnisse gelten nicht als bestandene aktuelle Mac-Abnahme.
Die konservative Gesamtbewertung und die übrigen offenen Abnahmepunkte bleiben.

## Bestätigte Mac-Revision und verschobenes Windows-Paket

Revision `2b31a7d` besteht den tatsächlichen
[macOS-Lauf 38812185951](https://github.com/Lulus792/physim/actions/runs/38812185951)
auf beiden Architekturen mit LLVM 20, ASan/UBSan und SDL 3.2.30:

- ARM: native Verträge 11/11 (`run-67d__ns_`), GUI 18/18 (`run-fo9irl60`).
- Intel: native Verträge 11/11 (`run-u3hj8q3x`), GUI 18/18 (`run-8sgxcx2v`).

Jeweils enthalten sind alle 36 Kopfzeilenansichten, 36 Theme-/Zoom-Formelbilder,
67 Lernformeln, 52 Fachformeln, vier schmale Gitterformeln und die Inventur von
166 Zeichen in 38 Schriftvarianten mit normaler und gebündelter Primärschrift.
Der Lernformelkatalog benötigt 35,309 Sekunden auf ARM und 34,984 Sekunden auf
Intel; die unveränderte interne Grenze von 60 Sekunden wird eingehalten.
Beide isolierten Gegenproben verwerfen die alte Metazeile präzise in Fall 9
bei 32 px. Auch die vorige Sidebar-Anordnung wird auf beiden Macs verworfen.

Beide vollständigen Joblogs und Archive wurden gelesen. Archiv-SHA256:
ARM `8b7f008f0a13947552e062a07fd66ba0e58ef2d638000dbf12dd487dc2f1a8df`,
Intel `aa1e57f5b14a441c8c9c033d0493a36e1a40e0c0cb340c7e0367626f7c301f8c`.
Die neuen Kopfzeilenbilder beider Macs, die ARM-Zeichenprobe mit ᵥ und die
schmale Intel-Formelansicht wurden visuell geprüft. Dieser Nachweis betrifft
AppKit und gerenderte Oberflächen, keine praktische Screenreader-Aufgabe.

Ein neuer Windows-Releasebuild erstellt außerdem alle 82 Sprachbeispiele und
das portable SDK mit allen 119 veröffentlichten Formelblöcken. Nach Verschieben
in `F1 Formeln 119 Paket ä` stimmen alle 1.578 Paketprüfsummen; die vier
Formelgruppen bestehen. Der zusätzliche Metazeilentest erreicht 30 vollständige
Zustände, bevor er nach 61,281 Sekunden an der internen Grenze abbricht.
Die Zeichen-/Clipprüfung selbst meldet bis dahin keinen Fehler. Dieser Lauf
wird als fehlgeschlagen erhalten und nicht durch die Formelprüfungen verdeckt.

Der Metazeilentest wartet anschließend wie der Katalog auf eine abgeschlossene
Veröffentlichung ohne ausstehenden Scrollauftrag, statt zusätzliche identische
Frames abzuwarten. Alle 36 Zustände, tatsächliche Glyphen-/Clipprüfungen,
Aufnahmen und Zeitgrenzen bleiben erhalten; der Abschluss nennt die Dauer.
Diese Änderung betrifft nur den Testablauf. Die oben genannten Mac-Ergebnisse
beziehen sich weiterhin ausdrücklich auf `2b31a7d` mit dem vorherigen Warteweg.

Die App wird erneut als Release gebaut und mit dem offiziellen Installer unter
Wiederverwendung der unveränderten eigenständigen Beispielprogramme installiert.
Im erneut verschobenen Paket `F1 Formeln 119 Bereitschaft ä` stimmen alle
1.578 Prüfsummen; alle fünf Gruppen bestehen einschließlich der 36 Metazeilen
(3,470 Sekunden). Binär-SHA256:
`49c3a19ac0f6e6271b36eb32bdd0b99cd5b084a790ed915abd07993fe26e9dbd`.
Bericht: `build/native/formula119-package-ready-proof/report.json`.
Die vollständigen 119 Formelblöcke und ihre 826 Varianten werden zusätzlich
direkt aus dem Paket durch `tools/math/check.py --root ...` bestätigt.
Die genaue Ursache der höheren Laufzeit des ersten Paketlaufs ist damit nicht
isoliert bewiesen; eine generelle Start- oder Laufzeitgarantie wird nicht behauptet.

Der neue Metazeilen-Warteweg besteht unter Linux GCC/ASan/UBSan zusätzlich
1/1 in `run-9lmie5el`, weiterhin mit allen 36 tatsächlichen Ansichten.
Die abschließende isolierte Gegenprobe mit diesem Warteweg verwirft erneut
präzise Fall 2 bei 16 px; Bericht unter `build/native/detail-counterexample`,
Binär-SHA256 `10f7bddce3aaed74420030677cdc3dc2fa85ca304a060382c7bab4f3a5981751`.
Der frühere Bericht bleibt unter `detail-counterexample-2b31a7d` erhalten.

## Reale Gase, Messunsicherheit und Monte Carlo vom 16. Oktober 2026

Weitere 19 Ausdrücke werden als Displayformeln gesetzt: vier im begrenzten
Van-der-Waals-Modell, sechs bei Messmodell und Standardunsicherheit einschließlich
Student-t-Intervall sowie neun im Monte-Carlo-Lernweg. Insgesamt sind 137
Ausdrücke, 959 Breitenvarianten und 138 veröffentlichte Blöcke auf 14 Seiten
vorhanden. Alle 16 ausführbaren Codeblöcke dieser drei Seiten sind gegenüber
`cc125f0` unverändert; beide vollständigen Tutorialquellen stimmen zusätzlich
mit den tatsächlich verwendeten Beispieldateien überein.

Die Gleichungen wurden gegen `src/thermodynamics.c`, `src/measurement.c`,
`src/student_interval.h` und `examples/documentation/monte_carlo_main.c`
abgeglichen. Die Druckableitung hält Stoffmenge und Temperatur fest. Die
RSS-Unsicherheit ist algebraisch in relative und Quantisierungsbeiträge
aufgeteilt, mit ausdrücklich nichtnegativem Ergebnis. Gezogener Rauschwert und
Standardabweichung der Rauschverteilung bleiben verschieden. Das t-Quantil
bezeichnet die kumulierte Wahrscheinlichkeit (1+C)/2 bei n-1 Freiheitsgraden.
Die Monte-Carlo-Energie nennt die feste Masse von 1 kg explizit; Erwartungswerte
und Populationsstreuungen bleiben von einer einzelnen Zufallsbahn unterschieden.

Die neue GUI-Gruppe `documentation_formula_models` prüft alle 19 Formeln bei
1100 x 800 und 16 px, `documentation_formula_models_narrow` dieselben 19 bei
760 x 540 und 32 px. Beide verlangen wirkliche Formelbilder im Clip, sichtbare
native Lesebereiche, genaue Lesereihenfolge und unveränderten Editor-/Simulations-
zustand. Die bisherigen 67 Lernformeln und vier schmalen Gitterformeln bleiben
in der gemeinsamen Katalogprobe erhalten. Zeitgrenzen wurden nicht erhöht.

Dabei wurde eine Lücke der bisherigen Schriftinventur nachgewiesen: TeX wird
beim Parsen durch den nativen Lesetext des Katalogs ersetzt. Allein das Alphabet
der Markdown-Quelldateien erfasst diesen Text nicht. Die erweiterte Gegenprobe
`build/native/formula-alt-before.log` verwirft tatsächlich U+2C7C, den lateinischen
tiefgestellten Buchstaben j, in allen 38 Schriftvarianten. UI-, Code- und
Fallbackbereiche enthalten ihn jetzt; Fontquellentest und sichtbare 16-/32-Pixel-
Zeichenprobe decken ihn zusätzlich ab.

Ein ähnliches Zeichen für y war tatsächlich U+1D67, ein griechisches
tiefgestelltes Gamma. Acht lineare Lesealternativen verwenden deshalb jetzt
eindeutig `_y`. Der sichtbare TeX-Formelsatz behält den korrekten y-Index.
Die finale Inventur umfasst Quelldokumente und sämtliche Katalog-Lesealternativen:
180 druckbare Zeichen in 38 Schriftvarianten, jeweils mit normaler und
gebündelter Primärschrift. Eine vollständige Unicode-Abdeckung folgt daraus nicht.

MSVC/ASan besteht sechs native Fälle in `run-u4o8sjvm`: reale Gase, Messmodell,
Dokumentparser, Fontquellen und die beiden Tutorialquellen. Alle fünf GUI-Fälle
bestehen in `run-36figaca`. Linux GCC/ASan/UBSan besteht dieselben sechs Fälle
in `run-zf_5y8vq` und alle fünf GUI-Fälle in `run-19tclqo1`, mit SDL 3.2.30,
X11/Xvfb, Openbox und Mesa-Software-GL. Alle 19 kleinen Formelbilder beider
Plattformen wurden als beschriftete Übersichten visuell geprüft; zusätzlich
wurden die ganze Windows-Ableitungsansicht und Linux-Unsicherheitsansicht geöffnet.
Der Testläufer samt erwarteten Fehlerfällen besteht unter
`build/native/formula-models-runner-proof`; Asset- und nativer Katalogabgleich
bestätigen alle 137 Ausdrücke.

Die ersten lokalen Läufe werden als Vorbefunde erhalten. Windows
`run-nis4yh39` besteht die vier Formelgruppen, verwirft die Zeicheninventur
wegen U+1D67. Im ersten Linux-Lauf `run-vj6vyaad` wurden die Lesetextquellen
noch während des laufenden Tests geändert; seine Vergleichsdaten sind deshalb
nicht für eine Abnahme geeignet. Erst der vollständig abgeschlossene finale
Quellstand dient als positiver Nachweis.

Die Mac-CI enthält beide neuen 19-Formel-Gruppen und die erweiterte Inventur.
Für diese Erweiterung steht die tatsächliche Mac-Ausführung noch aus. Die
vorherigen 119 Blöcke und früheren 166-Zeichen-Proben werden dafür nicht als
vollständiger Nachweis ausgegeben. Weitere mathematische Seiten, Inline-TeX,
reale Dichte-/Monitorwechsel, externe Mac-AX-Prüfung und praktische Screenreader-
Aufgaben bleiben offen; die Gesamtbewertung bleibt konservativ.

## Bestätigte 138-Blöcke-Revision auf beiden Macs

Revision `82e6a23` besteht den tatsächlichen
[Mac-Lauf 38817543681](https://github.com/Lulus792/physim/actions/runs/38817543681):
ARM native Verträge 11/11 (`run-6kywvxf8`), GUI 20/20 (`run-oy6qqvi1`);
Intel native Verträge 11/11 (`run-ut4_ebfz`), GUI 20/20 (`run-no0l8fk5`).
Beide prüfen die 19 zusätzlichen Modelle bei 16 und 32 px sowie 180 Zeichen
in 38 Varianten mit normaler und gebündelter Primärschrift. Die neuen Gruppen
benötigen auf ARM 8,832 und 11,240 Sekunden, auf Intel 14,047 und 14,664 Sekunden.
Beide bisherigen Layout-Gegenproben werden weiterhin präzise verworfen.

Vollständige Logs und Archive wurden gelesen; Archiv-SHA256:
ARM `befd31c92f27ec8d1a84cbafcfb39ebe1921e92be38f6fc75d7ac254f7ae36ed`,
Intel `8784dd36d50d31709df1e2a388cfcd8e59af15da61d0d6c08fff803a99d1f77c`.
Gasableitungs- und Zeichenbilder auf ARM sowie Gasableitungs- und
Unsicherheitsbilder auf Intel wurden visuell geprüft. Dies bestätigt die
138-Block-Erweiterung einschließlich lateinischem j-Index und klaren y-Lesetexten;
es ersetzt weder praktische Screenreader-Aufgaben noch die folgende neue Abnahme.

## Mechanik, Auftrieb und RK45 vom 16. Oktober 2026

34 zusätzliche Ausdrücke ergeben mit Wiederverwendung vorhandener Formeln 39
neue Displayblöcke: 24 in Mechanik, zwölf im Auftriebsmodell und drei in Numerik.
Insgesamt sind 171 Ausdrücke, 1.197 Breitenvarianten und 177 veröffentlichte
Blöcke auf 17 Seiten vorhanden. Alle sieben ausführbaren Codeblöcke dieser
drei Seiten sind gegenüber `82e6a23` unverändert. Öffentliche API und
numerische Modelle werden durch diese Dokumentationsarbeit nicht geändert.

Die Gleichungen sind gegen `src/mechanics.c`, `src/numerics.c` und
`examples/buoyancy/main.c` abgeglichen. Der Drehimpuls trennt Bahn- und Spinanteil
mit Körper- und Welttensor. Die axiale Federkraft wirkt auf A entlang A nach B;
die Gegenseite erhält die entgegengesetzte Kraft. Die Auftriebs-API arbeitet
vektoriell entgegen dem Gravitationsvektor. Das vertikale Beispiel verwendet
dagegen die positive y-Richtung nach oben und den positiven Betrag 9,81 m/s².
Kappenhöhe, eingetauchtes Volumen und Fluidschwerpunkt stimmen mit der
Kugelgeometrie überein; trocken gelten ausdrücklich Nullausgaben. Dissipation
und effektives Potential beschreiben Körper plus ideales hydrostatisches Reservoir.

Die RK45-Seite erklärt komponentenweise positive Skalierung, dimensionslose
Fehlernorm und die lokale eingebettete Schätzung. Absolute Toleranzen sind
strikt positiv, die relative Toleranz liegt zwischen null einschließlich und
eins ausschließlich. Nicht darstellbare Skalen oder Normen bleiben Fehler;
die Formel ist keine globale Genauigkeitsgarantie.

Der Generator benötigt für die fette griechische Winkelgeschwindigkeit das
bereits mitgelieferte TeX-Paket `boldsymbol`. Die erste Ausführung verwirft den
noch unbekannten Befehl ausdrücklich; nach Laden der Erweiterung tritt ein
zweiter, eigenständiger Fehler beim legalen Vergleich im Normindex auf: Der
HTML-Serializer belässt das Kleinerzeichen in einem SVG-Metadatenattribut.
Sharp/libvips verwirft das als ungültiges XML. Die Quelltexte werden deshalb
nicht auf andere Mathematik eingeschränkt. Stattdessen verwendet der Generator
den vorhandenen `liteAdaptor.serializeXML`-Pfad von MathJax 4.1.3, dessen
`protectAttribute` im XML-Modus die erforderlichen Zeichen maskiert.
`tools/math/check.py` prüft nun zusätzlich die XML-Struktur jedes SVGs.

Alle 959 zuvor bestehenden Rastervarianten bleiben in Dimensionen, Bytezahl,
CRC, Assetpfad und SHA256 identisch. Ihre SVG-Originale werden XML-konform neu
serialisiert; diese veränderten SVG-Hashes sind im Manifest enthalten. Die
171-Ausdruck-Prüfung bestätigt Quellkatalog, alle 1.197 SVG-/Rastervarianten,
native Deklarationen und alle veröffentlichten Blöcke. Fehlerausgaben bleiben
unter `build/native/formula-mechanics-render.log` und `formula-mechanics-render2.log`
erhalten, erfolgreiche Generierung unter `formula-mechanics-render3.log`.

Die neue Mechanikgruppe prüft alle 39 Blöcke in der großen Ansicht bei 16 px
und dieselben 39 bei 760 x 540 und 32 px. Tatsächlicher Clip, Formelbild,
native Lesebereiche, exakte Lesereihenfolge sowie Editor- und Simulationszustand
bleiben verpflichtend. Die Gruppenauswahl verwendet benannte Enumwerte und
eine Pfadtabelle; alle bisherigen Gruppen werden als Regression mit ausgeführt.
Die vorhandenen Zeitgrenzen werden nicht erhöht.

MSVC/ASan besteht Mechanik, Numerik, Dokumentparser und die tatsächliche
Auftriebsreferenz 4/4 in `run-_545aqz6`, die acht GUI-Gruppen 8/8 in
`run-w82iz0p9`. Linux GCC/ASan/UBSan besteht dieselben nativen Fälle 4/4 in
`run-fgi5ds6k` und GUI 8/8 in `run-kdr97w6_` mit SDL 3.2.30, X11/Xvfb,
Openbox und Mesa-Software-GL. Die Referenz prüft reale Runnerwerte, Energiebilanz
und Konvergenz der Kugel. Die erweiterte Zeicheninventur bestätigt auf beiden
Plattformen jeweils 182 Zeichen in 38 Varianten mit normaler und gebündelter
Primärschrift. Alle 39 kleinen Formelbilder beider Plattformen wurden anhand
von je zwei beschrifteten Übersichten visuell geprüft; die vollständige
Windows-Spinansicht wurde zusätzlich geöffnet.

Die Mac-CI enthält beide neuen 39-Formel-Gruppen. Für diese Erweiterung steht
der tatsächliche Mac-Lauf noch aus; der vorherige 138-Blöcke-Nachweis wird dafür
nicht umgedeutet. Weitere Fachseiten, Inline-TeX, reale Dichte-/Monitorwechsel,
externe Mac-AX-Prüfung und praktische Screenreader-Aufgaben bleiben offen.
Die Gesamtbewertung wird dadurch nicht als vollständig ausgegeben.

## Bestätigte Mechanikrevision auf beiden Macs

Revision `6cf85ac` besteht den tatsächlichen
[Mac-Lauf 38868123489](https://github.com/Lulus792/physim/actions/runs/38868123489):
ARM native Verträge 11/11 (`run-5dlvknb2`), GUI 22/22 (`run-pygyrlx0`);
Intel native Verträge 11/11 (`run-64she4j_`), GUI 22/22 (`run-s42h39n0`).
Die vollständigen Joblogs bestätigen beide 39-Formel-Gruppen, 182 Zeichen in
38 Schriftvarianten mit normaler und gebündelter Primärschrift sowie die
präzise Zurückweisung beider alten Layouts. Dies bestätigt 177 veröffentlichte
Blöcke; die folgende Erweiterung benötigt eine eigene Mac-Abnahme.

Die vollständigen Archive sind anhand der veröffentlichten SHA256 geprüft:
ARM `26bc4fa4641fb37822c1995823cce92b5a9e73fd270838c6e5a690d10631507b`,
Intel `f5ed811e1ee07354b74b7b40cb7ef7b830a5437adf9a92bbc42cc1ffc6344d68`.
Alle 11 nativen und 22 GUI-Ergebnisse beider Archive melden Erfolg. Die
Spinformel bei 32 px auf beiden Macs und die ARM-Zeichenprobe wurden aus den
Archivbildern visuell geprüft. Die ersten unvollständigen Übertragungen bleiben
als Vorbefund erhalten; erst die vollständig zusammengesetzten, geprüften
Archive dienen als Nachweis.

## Grundlagenformeln vom 16. Oktober 2026

22 zusätzliche Ausdrücke erweitern den Katalog auf 193 Ausdrücke, 1.351
Breitenvarianten und 199 veröffentlichte Blöcke auf 22 Seiten. Die fünf Seiten
Mathematik, Einheiten, Eigenschaftstabellen, räumliche Erklärungen und C-Einstieg
behalten alle zwölf ausführbaren Codeblöcke gegenüber `6cf85ac` unverändert.
Die Gleichungen wurden gegen die tatsächlichen Mathematik-, Einheiten- und
Eigenschaftsquellen sowie `app/documentation_media.inc` abgeglichen.

Die symmetrische Vergleichsbedingung bezieht sich weiterhin auf exakte binäre
Eingaben. Kurventangente und physikalische Geschwindigkeit unterscheiden sich
durch die positive Durchlaufdauer; die Teilkurvenableitungen berücksichtigen
die Kettenregel. Die Interpolationsgewichte setzen zwei mehrpunktige Achsen
voraus; Einpunktachsen behalten ihre gesonderten Regeln. Die synthetische
Dichteformel nennt Kelvin, Pascal und kg/m³ ausdrücklich. Beim räumlichen Pendel
ist die positive Stangenkraft auf den tatsächlich einstellbaren Winkelbereich
von -1,2 bis +1,2 rad bezogen. Die Lernaufgabe erhöht den Betrag des Winkeltempos
bei festem Winkel und fester Länge, damit die Aussage zur Energie auch bei
negativem Winkeltempo stimmt.

Die neue GUI-Gruppe `documentation_formula_basics` prüft alle 22 Blöcke bei
1100 x 800 und 16 px, `documentation_formula_basics_narrow` dieselben Blöcke
bei 760 x 540 und 32 px. Beide verlangen wirkliche Formelbilder im Clip,
sichtbare native Lesebereiche, genaue Lesereihenfolge und unveränderten
Editor-/Simulationszustand. Die bestehenden Zeitgrenzen bleiben erhalten.

MSVC/ASan besteht zehn native Fälle in `run-g77b4azi`, Linux GCC/ASan/UBSan
dieselben zehn in `run-jj404zi8`: Kern, Mathematik, Eigenschaften, Dokumentparser,
unabhängige Vergleichs- und Kurvenreferenzen, beide Tutorialquellen sowie
ausgeführte C-/Physim-Eigenschaftsbeispiele. Die ersten sechs GUI-Fälle bestehen
unter Windows in `run-bvvcdkmz` und unter Linux in `run-yvyqujwt`, einschließlich
beider räumlicher Medienproben. Die beschrifteten Formelübersichten zeigen bei
den ersten Umbrüchen eine schwerer erkennbare implizite Multiplikation.

Drei Produkte verwenden deshalb jetzt einen ausdrücklichen Multiplikationspunkt:
Vergleichstoleranz, rechte Teilkurvenableitung und die bisherige RK45-Skala.
Nach dieser reinen Darstellungsänderung bestehen Zeicheninventur, beide neuen
Formelgruppen und die 39 schmalen Mechanikformeln unter Windows 4/4 in
`run-dv5ym1le`, unter Linux 4/4 in `run-chzzfvmk`. Alle 22 finalen kleinen
Formelbilder beider Systeme wurden als beschriftete Übersichten visuell geprüft;
die ganze Windows-Ansicht der rechten Teilkurvenableitung wurde zusätzlich
geöffnet. Die Zeicheninventur umfasst 183 Zeichen in 38 Schriftvarianten mit
normaler und gebündelter Primärschrift. Von den bisherigen Ausdrücken bleiben
170 mit allen 1.190 SVG-/Rastervarianten unverändert; die sieben Varianten
der RK45-Skala enthalten den bewusst ergänzten Multiplikationspunkt.

Die Assetprüfung bestätigt alle 193 Ausdrücke einschließlich XML, Prüfsummen,
nativer Deklarationen und veröffentlichter Blöcke. Beide neuen Gruppen sind in
Linux- und Mac-CI aufgenommen. Für den aktuellen Stand mit 199 Blöcken steht
die tatsächliche Mac-Ausführung noch aus. Inline-TeX, weitere Fachseiten,
reale Monitor-/Dichtewechsel, externe Mac-AX-Prüfung und praktische Screenreader-
Aufgaben bleiben offen. Die Gesamtbewertung bleibt bei UI 8/10, Dokumentation
8/10 und Barrierefreiheit 5/10; vollständige Abnahme wird nicht behauptet.

## Bestätigte Grundlagenrevision auf beiden Macs

Revision `bff571c` besteht den tatsächlichen
[Mac-Lauf 38874970904](https://github.com/Lulus792/physim/actions/runs/38874970904):
ARM native Verträge 11/11 (`run-9_ugygqm`), GUI 24/24 (`run-rco5uta5`);
Intel native Verträge 11/11 (`run-gdd8l64h`), GUI 24/24 (`run-b1emn24i`).
Beide vollständigen Joblogs und Archive sind geprüft. Archiv-SHA256:
ARM `1889674d1900f76b1288417eadde6c92d1869cc76ed35cc574356ba88d0606c8`,
Intel `6ef252d784733fc5c70b7e3dc701d423f9390b8535f2cac9b665754a1078db8c`.
Die Ergebnisdateien bestätigen alle Fälle und die Inventur mit 185 Seiten und
183 Zeichen. Beide vorherigen Layouts werden weiterhin präzise verworfen.
Die 32-Pixel-Spaltenvektoren beider Macs wurden aus den Archivbildern visuell
geprüft. Das bestätigt die bisherigen 199 Displayblöcke einschließlich RK45-Punkt;
die folgende Inline-Erweiterung besitzt einen eigenen Abnahmeumfang.

## Inline-Formelsatz vom 16. Oktober 2026

Registrierte Ausdrücke zwischen einzelnen Dollarzeichen werden im laufenden
Text gesetzt. Für alle 193 Katalogeinträge gibt es eine zusätzliche MathJax-
Inlinevariante. Die Kleinwinkelperiode im User Guide und die kinetische Energie
im C-Einstieg nutzen diese Darstellung. Damit sind 197 Displayblöcke und zwei
Inlinepassagen auf weiterhin 22 Seiten veröffentlicht. Alle 1.351 bisherigen
Displayvarianten bleiben als SVG und Raster unverändert; Manifestformat 2
ergänzt die Grundlinie und 193 Inlinevarianten. Ausführbare Codeblöcke bleiben
unverändert.

MathJax 4.1.3 zerlegt Inline-Ausgaben standardmäßig in mehrere SVGs. Ein Zugriff
auf das erste Kind allein würde den Ausdruck abschneiden. Die tatsächliche
Gegenprobe zur Kleinwinkelperiode erzeugt zwei SVGs mit nur 16,3296 Pixeln im
ersten Fragment; mit `linebreaks.inline=false` entsteht ein vollständiges SVG
mit 85,8496 Pixeln. Der Generator verlangt genau ein SVG; der native Test
verlangt für diesen Ausdruck zusätzlich mehr als 50 Pixel Breite. Bericht:
`build/native/inline-fragment-counterexample.json`. Die XML-Prüfung bestätigt
auch die aus dem ViewBox abgeleitete Grundlinie sämtlicher Varianten.

Inline-Mathematik bleibt beim Umbruch ein zusammenhängender Ausdruck. Ist die
Inlinefassung zu breit, wird eine passende umbrochene Displayfassung verwendet;
passt auch diese nicht, bleibt die lineare Alternative im normalen Textumbruch.
Formel und umgebende Schrift teilen sich eine Grundlinie. Nur Zeilen mit
Mathematik erhalten angepasste Höhen; gewöhnlicher Text und Rahmencode behalten
ihre bisherigen Maße. Code, maskierte Dollarzeichen und unbekanntes TeX bleiben
wörtlich. Der Parser begrenzt auch expandierte Lesealternativen auf den
verfügbaren Textpuffer. Suche, native Lesereihenfolge und verlinkte Formeln
verwenden dieselben vollständigen Lesealternativen.

Die neue GUI-Gruppe `documentation_inline_math` prüft 48 tatsächliche Ansichten
bei 16/32 px und drei Themes: sechs repräsentative Ausdrücke, Unicode im
umgebenden Text, einen Absatz mit 20 verschiedenen Formeln und eine tatsächlich
angeklickte Formelverknüpfung. 162 Formelbilder müssen vollständig im Clip
liegen und sichtbare native Lesebereiche besitzen. Kurze Formeln werden gegen
die Grundlinie der tatsächlichen gezeichneten Textkommandos geprüft. Für alle
Bilder prüft der Python-Client sichtbare Pixel. Editor und Simulation bleiben
unverändert. Die bestehenden Zeitgrenzen bleiben erhalten.

Der Grafikcache schützt in diesem Frame verwendete Texturen vor Verdrängung
und wächst bei Bedarf begrenzt. Der Gegenlauf
`tools/check_inline_math_counterexample.py` stellt die frühere sofortige
Verdrängung isoliert wieder her. Unter Linux werden tatsächliche Verdrängungen
bereits referenzierter Texturhandles und der folgende OpenGL-Fehler 0x502
erkannt; dies geschieht schon beim ersten Zeichnen der vollständigen Fixture.
Der Bericht liegt unter `build/native/inline-math-counterexample/report.json`.
Die alte Variante erreicht deshalb nicht erst den späteren Einzelbildtest.

MSVC/ASan besteht Parser, natives Lesemodell und alle Abschnittslinks 3/3 in
`run-uqqeldyh`; Linux GCC/ASan/UBSan dieselben drei in `run-j8ooeft0`.
Windows besteht beide gezielten GUI-Fälle 2/2 in `run-m2jxpqxl` und die fünf
übrigen Regressionen 5/5 in `run-h8ge23et`. Linux besteht alle sieben GUI-Fälle
7/7 in `run-izccrh9z`, mit SDL 3.2.30, X11/Xvfb/Openbox und Mesa-Software-GL.
Die Zeicheninventur bleibt bei 183 Zeichen in 38 Varianten mit normaler und
gebündelter Primärschrift. Beschriftete Übersichten für 16 px dunkel und 32 px
hell wurden auf beiden Systemen visuell geprüft, zusätzlich 32 px mit hohem
Kontrast unter Windows sowie zwei ganze Windows-Ansichten. Der Testläufer
besteht seine unabhängigen Fehler-/Timeoutfälle unter `build/native/inline-runner-proof`.

Die Vorläufe bleiben erhalten: Windows `run-x4xe81oh` besteht 5/7, Linux
`run-11uj1u_9` 6/7. Windows zeigt einen durch neue Bruchteil-Zeilenmaße
unvollständig abgeschlossenen Fokus-Reveal. Die Korrektur beschränkt die neue
Grundliniengeometrie auf Mathematikzeilen. Die erste Inline-Bildprobe unterscheidet
zudem die 16-/32-Pixel-Cacheeinträge noch nicht und verwirft sichtbare Bilder;
die finale Probe prüft Variante und tatsächliche Rastergröße gemeinsam.
Die ersten isolierten Gegenläufe sind wegen fehlendem Arbeitsordner bzw.
unverändertem Binärfingerprint bei geändertem lokalen Include keine gültige
Gegenabnahme. Der endgültige Hauptquell-Fingerprint umfasst die isolierte
Cachequelle, und der abschließende Bericht bestätigt den tatsächlichen Fehler.

Die Mac-CI enthält den neuen Inline-Fall und die isolierte Cachegegenprobe.
Der anschließende Lauf wird im folgenden Abschnitt getrennt nach Plattform
bewertet.
Strukturierte mathematische Screenreader-Navigation, praktische Screenreader-
Aufgaben, reale Monitor-/Dichtewechsel und der vollständige inhaltliche Abgleich
aller Hilfeseiten bleiben offen. Die Gesamtwertung bleibt konservativ.

## Inline-Abnahme auf Mac und begrenzte Testübergänge

Workflow `38879251192` für `f79ab793` besteht auf macOS ARM die elf nativen
Fälle in `run-qyl8ohzb` und alle 25 GUI-Fälle in `run-se6rcoff`, einschließlich
der Inline-Formeln. Alle drei Layout-/Cachegegenproben werden zurückgewiesen.
Intel besteht elf native Fälle in `run-4jjmwi0y`, aber nur 24/25 GUI-Fälle in
`run-np1_rtmr`: `documentation_inline_math` endet nach 60,068 Sekunden am
unveränderten inneren Zeitlimit. Das Archiv enthält 46 Ansichten; die letzten
vollständigen Formelprüfungen betreffen Fall 45. Das ist keine vollständige
Intel-Abnahme. Die Ansichten 34 und 40 wurden zusätzlich visuell geprüft:
Matrix, Brüche, Fließtext und hoher Kontrast sind darin korrekt gesetzt.
Beide Originalarchive sind vollständig heruntergeladen und gegen ihre
GitHub-SHA-256-Digests geprüft. Die Auswertung unter
`build/native/review-inline-mac.py` liest alle nativen und GUI-Ergebnislisten:
ARM enthält 162 vollständige Formelbilder bis Fall 47, Intel 141 bis Fall 45.
Auch die ARM-Ansichten 34 und 47 wurden geöffnet und visuell geprüft,
einschließlich des verlinkten mathematischen Ausdrucks im hohen Kontrast.

Der Test wiederholte bisher Größen- und Themewechsel auch bei unverändertem
Zustand. Er führt nun genau zwei Fenstergrößenwechsel und sechs Themewechsel
aus. Jeder der weiterhin 48 Fälle kontrolliert die tatsächliche Fenstergröße
und das aktive Theme; alle 162 Bildprüfungen, Grundlinien-, Cache-, Link- und
Isolationsprüfungen bleiben erhalten. Das Abschlussprotokoll nennt die Anzahl
der Übergänge und die Laufzeit; der unabhängige Testclient verlangt diese
Anzahlen. Weder die innere noch die äußere Zeitgrenze wurde erhöht.

Diese Änderung besteht unter Windows MSVC/ASan die beiden betroffenen
GUI-Fälle in `run-aeu0a4fq` und unter Linux GCC/ASan/UBSan alle sieben
Formel-/Darstellungsfälle in `run-vhn6ttu4`. Formelassets und alle 31 erzeugten
Referenzdokumente sind erneut statisch geprüft. Ob die verringerten Übergänge
das Intel-Zeitlimit beheben, bleibt bis zu einer tatsächlichen Mac-Ausführung
offen; aus den lokalen Erfolgen wird das nicht abgeleitet.

## Unabhängiger externer macOS-AX-Prüfweg

Der zusätzliche Client `tests/accessibility_macos_probe.c` verwendet ausschließlich
öffentliche ApplicationServices-/CoreFoundation-Schnittstellen. Er linkt keinen
Physim-Provider und spricht nur den vom Prüfwerkzeug gestarteten App-Prozess an.
Über AXWindows, den F1-Fenstertitel und die Dokumentkennung sucht er den echten
Lesebereich. Geprüft werden vollständiger UTF-8-Inhalt, UTF-16-Anzahl, eine genaue
Teilzeichenfolge, echte Startgeometrie, Readonly-Status und die native Anforderung
des letzten Inhaltszeichens. Die App muss daraufhin sichtbare Schlussglyphen
bestätigen und ihr tatsächliches Endbild sichern.

`tools/check_macos_ax.py` prüft zuerst die vorhandene Clientberechtigung über
[AXIsProcessTrusted](https://developer.apple.com/documentation/applicationservices/1460720-axisprocesstrusted).
Es verändert keine Systemeinstellung und öffnet keinen Berechtigungsdialog.
Ohne vorhandene Berechtigung steht im Bericht ausdrücklich `unavailable`,
niemals `passed`. Ohne `--allow-unavailable` liefert dieser Fall Exit 77;
die CI verwendet die Option ausschließlich zum getrennten Erfassen der
Prüfumgebung. Andere Fehler bleiben Fehler. Dieses Ergebnis zählt nicht zu den
bisherigen In-Prozess-GUI-Fällen und hebt deren Abnahme nicht auf eine externe
AX- oder VoiceOver-Abnahme an.

Für den neuen Client steht die tatsächliche Mac-Ausführung noch aus. Felder,
Linkaktionen, Tabellen, Ereignisse und gehaltene alte Referenzen benötigen
zusätzliche externe Fälle; der vollständige Readonly-Quelltext ist der erste
Ausführungsschritt dieses Prüfwegs. Die Gesamtwertung bleibt unverändert.

Lokal sind vorab Python-Syntax, der Nicht-Mac-Plattformschutz des Clients
mit GCC C17 und strengen Warnungen sowie der unabhängige Testläufer unter
`build/native/mac-ax-runner-proof` geprüft. Windows meldet im Bericht korrekt
`unavailable` und Exit 77. Diese Prüfungen ersetzen weder das Übersetzen der
ApplicationServices-Aufrufe mit dem Mac-SDK noch ihren tatsächlichen AX-Aufruf.

## Inhaltlicher Abgleich der Datenreihen-Hilfe

`docs/series.md` nannte weiterhin drei Resamplingverfahren, obwohl die C-API und
die Physim-Bindung PCHIP als viertes Verfahren anbieten. Die Übersicht enthält
jetzt `PS_RESAMPLE_PCHIP`, erklärt stückweise Formerhaltung, stetige erste und
möglicherweise unstetige zweite Ableitung, innere Extrema, Randsteigungen sowie
die Ein-/Zweipunktfälle. Das Zahlenbeispiel mit X = 0, 1, 2 und Y = 0, 1, 4
stimmt mit dem ausgeführten PCHIP-Referenztest überein: bei 0,5 entsteht 0,3125
statt des linearen Werts 0,5. Die fünf bisherigen Codeblöcke bleiben unverändert.

Geprüfte Quellen sind `include/physim/series.h`, `src/series.c`, `src/pchip.h`,
`tests/test_pchip.c`, `tests/test_resample.c` und die veröffentlichte Physim-
Bindung `Series.resampledPchip`. MSVC/ASan besteht acht Fälle in `run-2glhqoc1`,
Linux GCC/ASan/UBSan dieselben acht in `run-eh96o1wv`: Series, Resampling,
SI-Konvertierung, numerische Extremwerte, Masken, PCHIP, Dokumentparser und
alle registrierten Abschnittslinks. `tools/generate-reference.py --check`
bestätigt zusätzlich alle 31 erzeugten Referenzdokumente, eingebettete
Tutorialquellen, Seitenlimits und ihre Offline-Erreichbarkeit.
Das belegt diesen Inhaltsabgleich, weiterhin keinen semantischen Gesamtabgleich
aller 185 registrierten Dateien.

## Numerik-Handbuch: ein akzeptierter Schritt

`docs/numerics.md` erklärt jetzt den Unterschied zwischen vollständiger
Integration und `ps_ode_step_diagnosed`: genau ein akzeptierter Schritt,
mögliche Verwerfungen, tatsächlicher Endzeitpunkt, vorzeichenbehafteter
Folgevorschlag und transaktionaler Zustand bei Fehlern. Der Physim-Weg
`rk45StepReported` und der adaptive Modellcallback sind direkt verlinkt.
Der einleitende Überblick nennt auch den gebräuchlichen Suchbegriff Numerik.
Die Verträge wurden gegen `src/numerics.c`, `include/physim/numerics.h`,
`include/physim/language_sdk.h` und `tests/test_adaptive.c` abgeglichen.
Windows MSVC/ASan besteht Adaptive, Numerik, Dokumentparser und Abschnittslinks
4/4 in `run-xe4ri0zs`, Linux GCC/ASan/UBSan dieselben vier in `run-0zvemu7n`.
Alle 31 erzeugten Referenzdokumente bestehen zusätzlich die Aktualitätsprüfung.
Das belegt diesen Teil des Inhaltsabgleichs, keinen vollständigen Review.

## Zusammenhängende Listenabsätze

Die direkte Windows-Bedienprüfung mit `computer-use` öffnet F1 aus einem leeren
Workspace, verwendet die globale Suche und folgt dem Link von der C-Referenz
zur Numerik-Anleitung. Dort zerlegte der Parser einen im Markdown umgebrochenen
Listenpunkt in mehrere Textblöcke. Die Fortsetzung erschien mit zusätzlichem
Absatzabstand außerhalb des zugehörigen Punktes. Ursache war die sofortige
Ausgabe jeder Zeile mit Listenmarker; die Fortsetzung wurde erst danach gesammelt.

Der Parser sammelt nun den ganzen Listenabsatz, bevor er Inline-Auszeichnung,
Links und native Lesebereiche erzeugt. Ein neuer Marker oder eine Blockgrenze
schließt ihn ab. Einrückung der Fortsetzungszeile erzeugt keinen zusätzlichen
Abstand. Marker benötigen abschließenden Leerraum; Dezimalzahlen werden dadurch
nicht mehr als nummerierte Listen zerlegt. Die Probe deckt CRLF/LF, mehrere
Marker, eingerückte Unterpunkte, Links sowie Grenzen zu Code, Formeln, Tabellen,
Überschriften und mit Leerraum gefüllten Leerzeilen ab. Das ergänzt keine
vollständige verschachtelte Markdown-Listen- oder native Listensemantik.

Der neue Regressionstest verwirft den bisherigen Parser in `run-fi2eux05`
gezielt an der zusammenhängenden ersten Listenzeile. Nach der Korrektur
bestehen Dokumentparser, natives Lesemodell und Abschnittslinks unter Windows
MSVC/ASan 3/3 in `run-oxw0rlni`, unter Linux GCC/ASan/UBSan 3/3 in `run-1qoiobhm`.
Die fünf GUI-Fälle Tastatur, Navigation, Inline-Inhalte, Zeicheninventur und
Inline-Formeln bestehen auf Windows in `run-5zxizwdg` und auf Linux in
`run-v7g6isqx` jeweils vollständig. Formelassets und alle 31 erzeugten
Referenzdokumente bestehen die statischen Prüfungen.

Die anschließende manuelle Kontrolle des korrigierten Builds wurde vor dem
Start durch die physische Escape-Taste beendet. Weitere Desktop-Eingaben
bleiben gestoppt. Die reale visuelle Nachkontrolle der ursprünglichen Ansicht
und die Mac-Abnahme dieser Parseränderung bleiben deshalb ausdrücklich offen.

## UI-Handbuch mit dem vorhandenen Verhalten abgeglichen

`docs/ui-states.md` behauptete noch, dass nur die dunkle Palette und eine
schreibgeschützte Dateivorschau existierten. Die Anleitung beschreibt jetzt
die drei vorhandenen Paletten sowie Dateibaum und editierbare Textdokumente.
`docs/settings.md` erklärt die tatsächlich implementierte Priorität des eigenen
F1-Lesezooms: Solange er aktiv ist, ersetzt er die getrennten UI-/Codegrößen.
Die mittlere Größenanzeige setzt ihn zurück. Der frühere offene Status des
vollständigen nativen F1-Lesetextes wurde durch den aktuellen Schnittstellenstand
mit Verweis auf die weiterhin offene praktische Screenreader-Abnahme ersetzt.
Quellen sind `app/design_tokens.h`, `app/preferences.h`, `app/settings_ui.inc`,
`app/document_actions.inc` und die Größenfunktionen in `app/documentation_ui.inc`.

Die ausgeführten Gruppen `run-o51av9di` unter Windows und `run-kqq42p3i` unter
Linux bestehen jeweils 2/4. Preferences einschließlich Palettenkontrasten
besteht auf beiden Systemen. Der zusätzliche Windows-Baumtest überschreitet im
langen Worktree-Testpfad die erreichbare Pfadlänge; derselbe unveränderte Test
besteht mit seinem vollständigen 64-Ebenen-Fall unter
`D:/Physim/build/native/ui-tree-review-54344aca`. Unter Linux scheitert der
Dateimodustest auf dem eingebundenen Windows-Dateisystem; derselbe unveränderte
Test besteht auf dem nativen Linux-Dateisystem unter `/tmp`. Diese Nachläufe
belegen keine Windows-Langpfad- oder POSIX-Rechtegarantie für DrvFS.

Die neue Abschnittsverlinkung verwendete zunächst umschriebene statt der
vorhandenen Unicode-Anker. Nach der Korrektur prüfen die beiden nativen
Ankerclients alle 218 Abschnittslinks und 185 Seiten erfolgreich. Die
Originalausgaben stehen unter `build/native/ui-doc-*`; alle 31 erzeugten
Referenzdokumente bestehen ebenfalls die Aktualitätsprüfung.

## Erster externer Mac-AX-Lauf: falsche Schreibbarkeitsangabe

Workflow `38880365090`, Intel-Job `116905360795` für `4fd04ba`, übersetzt den
unabhängigen C17-Client mit dem tatsächlichen Mac-SDK. Der Client besitzt bereits
AX-Zugriff; es wird keine Berechtigung geändert. Elf native Fälle bestehen in
`run-jvmbc1dw`, alle 25 GUI-Fälle in `run-08d1d7fd`, und die drei Gegenproben
erkennen ihre früheren Fehler. Der externe Fall scheitert danach an der
Schreibbarkeitsangabe des Dokumentwerts: `AXUIElementIsAttributeSettable` liefert
Erfolg und meldet `AXValue` als schreibbar. Die bisherige In-Prozess-Prüfung der
Selector-Allowlist hatte diese falsche externe Angabe nicht erkannt.

Das Originalarchiv `12038414515` ist vollständig gegen SHA-256
`8c96b7d9c9de426ac87162477948db24d16783b63aaa705115820084449965c3`
geprüft. Seine externe Abschrift stimmt bytegenau mit dem Git-Quellstand
`4fd04ba:docs/project-audit.json` plus zwei abschließenden LF überein:
486.670 UTF-8-Bytes und 481.208 UTF-16-Einheiten. Die lokalen CRLF-Dateibytes
sind dafür kein geeigneter Vergleich. Vor der fehlgeschlagenen
Readonly-Prüfung bestätigt der Client Teilzeichenfolge und positive
Zeichenrahmen. Der native Ende-Reveal wird noch nicht angefordert und ist
somit in diesem externen Lauf nicht abgenommen.

Die Korrektur entfernt den ausdrücklich registrierten wirkungslosen Wertsetter
der Dokumentklasse. Nach Apples [AppKit-Vertrag](https://developer.apple.com/documentation/appkit/nsaccessibilityprotocol)
kennzeichnet ein allein überschriebener Getter die Eigenschaft als readonly;
ein überschriebener Setter veröffentlicht Schreibzugriff. Der geerbte
Control-Setter weist Dokumentobjekte nun vor der Auflösung numerischer IDs ab.
Die In-Prozess-Probe läuft mit einem gleich nummerierten Button und zusätzlich
mit einem Slider; ein direkter numerischer Setteraufruf darf weder einen
Wertauftrag erzeugen noch den Modellwert ändern. Die native Readonly-Angabe
wird zusätzlich abgefragt. Der unabhängige AX-Client bleibt unverändert streng.

Diese Korrektur benötigt noch ihre tatsächliche Mac-Ausführung. Der erfolgreiche
GUI-Vorlauf ist weiterhin ein Nachweis der früheren Revision und keine Abnahme
des neuen Setterschutzes. Der erste externe Lauf bleibt als Fehler erhalten;
eine praktische VoiceOver-Abnahme wird daraus nicht abgeleitet.

## Vollständiger Inhaltsabgleich: Arrays, String-Views und Hashmaps

Die drei Anleitungen `docs/array.md`, `docs/string-view.md` und `docs/hashmap.md`
sind gegen ihre öffentlichen Header und Implementierungen abgeglichen:
Speicherbesitz, Grenzen, Allokationen, Selbstkopien beziehungsweise Aliasing,
Fehlerausgaben, Traversierung und Zeigerlebensdauer. Der Array-Text nennt nun
ausdrücklich die nötige externe Synchronisation. String-Views unterscheiden
überlappende Daten von einem überschriebenen, weiter benötigten Descriptor.
Die Hashmap-Anleitung erklärt den Besitz einer zuvor über den Ausgabezeiger
referenzierten Map beim erneuten Aufruf ihres Beispielhelpers.

Die drei veröffentlichten C-Beispiele werden jetzt unmittelbar aus ihrem
jeweiligen Markdown-Codeblock übernommen und mit einer separaten Ergebnisprobe
übersetzt. Es gibt keine gepflegte zweite Kopie des Beispielcodes. Der native
Testkatalog führt `documentation_array_example`,
`documentation_string_view_example` und `documentation_hashmap_example` aus.
Die Proben prüfen 100 Positionen und Koordinaten, geliehene getrimmte Textbereiche,
unveränderte Fehlerausgaben sowie den gespeicherten und wiedergefundenen
Map-Parameter. Codepfade verweisen mittels `#line` auf die veröffentlichte Seite.
Der Bericht hält die Hashes der mit LF normalisierten Seite, des tatsächlich
verwendeten Codeblocks und der erzeugten C-Quelle fest. Fehlende oder zusätzliche
C-Blöcke werden abgewiesen, damit neuer Beispielcode nicht unbemerkt ungeprüft bleibt.

MSVC/ASan besteht alle drei Beispiele, die drei bestehenden Bibliothekstests und
die Abschnittslinks 7/7 in `run-dbg30_ct`; Linux GCC/ASan/UBSan dieselben sieben
in `run-8j9zvq0_`. Die abschließende Hashprüfung vergleicht beide Berichte mit
den aktuellen veröffentlichten Codeblöcken. Die Vorläufe mit jeweils sechs
Fällen bestehen ebenfalls: `run-phrramcn` und `run-_j2g5uir`.
Eine isolierte, tatsächlich übersetzte Variante mit nur 99 Positionen statt 100
wird von derselben Ergebnisprobe abgewiesen. Auch ein zusätzlicher ungetesteter
Codeblock wird erkannt; `build/native/foundation-counterexample/report.json`
enthält die Originalergebnisse. Der unabhängige Testläufer besteht zusätzlich
seine Fehler-, Timeout- und Berichtsfälle unter
`build/native/foundation-final-runner-proof`.

Der fokussierte Mac-Workflow nimmt die drei Beispielprüfungen in seine native
Gruppe auf; ihre tatsächliche Mac-Ausführung steht noch aus. Dieser Abgleich
schließt diese drei Fachanleitungen ein, keinen vollständigen semantischen
Review aller 185 registrierten Dateien.

## Speicher- und Logging-Anleitungen mit ausgeführten Fragmenten

`docs/memory.md` und `docs/logging.md` nennen jetzt ausdrücklich den Kontext
ihrer C-Ausschnitte: Funktionsrumpf mit öffentlichem Report-Header beziehungsweise
Experimentcallback mit gültigem `ps_context` und Experiment-Header. Der
Sprachausschnitt zum Logging trägt die Kennzeichnung `physim`.
Speicherbesitz, Arena-Rücksetzung, Freigabereihenfolge, Fehlererhaltung und die
8-MiB-Serialisierung wurden gegen `include/physim/memory.h`, `src/memory.c`,
`src/report.c` und `src/series.c` abgeglichen. Logger-Lebensdauer, UTF-8-Grenzen,
Zeitbezug, Rückgabewerte, Wire-Version und Archivierung stimmen mit
`include/physim/log.h`, `include/physim/experiment.h`, `src/core.c`,
`src/protocol.h` und `runners/logging.inc` überein.

Der native Beispielprüfer kann nun einen ausdrücklich gespeicherten
Funktionskontext vor den unveränderten veröffentlichten Codeblock setzen.
Die beiden neuen Fälle verwenden ausschließlich die genannten öffentlichen
Header. Der Speicherausschnitt erstellt und zerstört acht Arena-Berichte.
Der Logging-Ausschnitt prüft Nachricht, Level, tatsächliche Context-Zeit,
unveränderten Context, Sinkfehler, ungültige Zeit und alte Context-Größe.
Die drei bisherigen vollständigen C-Beispiele bleiben aktiv.

Windows MSVC/ASan besteht die fünf Beispiele, Memory, Memory Owners, Logging
und Abschnittslinks 9/9 in `run-e3q_lsc1`; Linux GCC/ASan/UBSan dieselben neun
in `run-mklb7n94`. Echte C- und Physim-Runnerläufe bestehen zusätzlich 2/2
unter Windows in `run-u8redolz` und unter Linux in `run-q6e50xt_`. Sie prüfen
unter anderem identische JSONL-/Archivmeldungen, CRC-Blöcke, Zeitpunkte,
exklusive Begleitdateien, Volumenlimit und den optierten Wire-Datenstrom.

Für das alleinstehende Speicherbeispiel aktiviert der native Testläufer in
Linux-ASan-Builds ausdrücklich LeakSanitizer. Vorhandene ASan-Optionen bleiben
erhalten; die Option wird nur in die Umgebung dieses Testprozesses übernommen.
Der Bericht nennt, ob diese Prüfung aktiv war. Die finale Beispielgruppe
besteht 2/2 unter Windows in `run-2vyj9lan` und 2/2 unter Linux mit aktivem
Speicher-Leakcheck in `run-eek52xlw`. Die aktuellen Markdown- und Codeblock-Hashes
beider Gruppen wurden abgeglichen. Daraus wird keine allgemeine Leakfreiheit
der App oder der übrigen Plattformen abgeleitet.

Eine isolierte, tatsächlich übersetzte Variante ohne Pufferfreigabe erzeugt
8.388.608 verlorene Bytes in acht Allokationen. Der native Testläufer selbst
weist diese Variante mit Exit 23 und LeakSanitizer-Bericht ab, obwohl seine
übergeordnete Umgebung `detect_leaks=0` enthält; die übergeordnete Option bleibt
unverändert. `build/native/memory-runner-leak-counterexample/report.json`
bewahrt den erwarteten fehlgeschlagenen Fall `run-425ry99g`. Der separate
Vorläufer unter `build/native/memory-leak-counterexample/report.json` bestätigt
denselben tatsächlichen Leak und das saubere Original. Der allgemeine
Testläufer besteht seine Fehler-/Timeout-/Berichtsfälle unter
`build/native/memory-log-final-runner-proof`.

Die Mac-Gruppe nimmt beide Beispiele und ihre Kontextdateien auf; die
Workflow-Pfade erfassen auch spätere Änderungen der fünf Beispielseiten und
Prüfdateien. Die tatsächliche Mac-Ausführung bleibt offen. Der Inhaltsnachweis
dieses Abschnitts bezieht sich auf Speicherverwaltung und Logging.

## Eigenständig ausführbares Beispiel zur Laufidentität

Der Identitätsausschnitt in `docs/run-streams.md` verwendete den zuvor
konfigurierten Store aus dem Schreibabschnitt. Die nötigen Voraussetzungen
blieben beim isolierten Kopieren des Ausschnitts unsichtbar. Ein tatsächlicher
Linux-Vorlauf mit einem frisch erzeugten Store weist die Writer-Erstellung
entsprechend dem API-Vertrag mit Exit 70 ab: Konfiguration und Kanalplan fehlen.
`build/native/identity-doc-before/report.json` hält diesen Kontextfall fest;
das ist kein Fehler des Writers und keine Aussage über den vollständig
vorbereiteten Store im vorherigen Abschnitt.

Das Beispiel konfiguriert jetzt selbst Zeitschritt, Seed, Metadaten und einen
SI-Kanal, schreibt eine Messzeile und schließt den Writer ausdrücklich vor dem
Lesen. Es prüft beide IDs, die gelesene Zeile und den finalisierten Abschluss.
Die kopierte Identität wird auch nach Reader-/Store-Close weiterverwendet.
`experiment.psrun` muss neu angelegt werden können; diese Voraussetzung steht
direkt über dem Beispiel. Die identische Quelle liegt unter
`examples/documentation/run_identity.phys`.

Der neue native Integrationsfall `documentation_run_identity_example` prüft
zuerst die Gleichheit des veröffentlichten Codeblocks mit dieser Quelle und
führt dann das tatsächlich kompilierte Programm aus. Beide ID-Zeilen müssen
exakt erscheinen; der zusätzliche Runtime-Speicherprüfer verlangt am Ende null
lebende Sprachallokationen. Zusammen mit Run-Stream-, Generationsstilllegungs-
und Abschnittslinktests bestehen MSVC/ASan unter Windows 4/4 in
`run-vwltniet` und GCC/ASan/UBSan unter Linux 4/4 in `run-yrr9hj6j`.
Die Mac-Gruppe erhält denselben Fall; ihre tatsächliche Ausführung steht aus.
Dieser Nachweis betrifft das Identitätsbeispiel, keinen vollständigen Review
aller weiteren Run-Stream-Ausschnitte.
