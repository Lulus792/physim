# F1 Handbuch und Abnahme

Entscheidung vom 13. Oktober 2026: F1 soll sowohl den ersten abgeschlossenen
Versuch ermöglichen als auch beim Arbeiten präzise Antworten liefern. Die
Abnahme betrachtet Inhalt, Bedienung, Darstellung und zugängliches Lesen.
Ein grüner Parsertest allein ist keine vollständige Abnahme.

## Aktuelle Bewertung vom 14. Oktober 2026

Bewertungsgrundlage sind die zentralen Skills unter `D:/Skills/skills`:
`documentation-engineer`, `ui-ux-designer`, `accessibility-engineer`,
`ui-implementation` und `test-engineer`. Volle Punktzahl erfordert die acht
unten beschriebenen Abnahmekriterien; fehlende Prüfungen gelten als offen.

| Bereich | Belegter Stand | Rest zur vollständigen Abnahme |
| --- | --- | --- |
| Dokumentationszugang | 183 registrierte Seiten und 214 interne Abschnittslinks geprüft; Navigation, Verlauf und Suche mit realen F1-Abläufen geprüft | Vollständige Plattformabnahme derselben Revision |
| Inhalt und Aufgabe | Lernwege, Beispiele, Referenz und Fehlerbehebung vorhanden; ausgewählte Quellen-/CLI-Abläufe ausgeführt | Gesamtabgleich sämtlicher Anleitungen mit dem Projektstand |
| Lesbarkeit und Bedienung | 16/32-Pixel-Schrift, kleines Fenster, drei Themes, Codekopieren, Links und Tastaturführung durch Laufzeitbilder und Verhalten belegt | Tatsächliche DPI-/Monitorwechsel und vollständige Zustandsmatrix |
| Barrierefreiheit | Windows UIA, Linux AT-SPI und macOS AppKit ergänzen vollständigen Lesetext, Suchfelder und interne Linkbereiche mit den jeweils dokumentierten Laufzeit- und Bildnachweisen | Tabellensemantik, externe macOS-AX-Clientprüfung, echte Screenreader-Aufgaben und die unten beschriebene sporadische Windows-Linkprüfung |

Die Hilfe ist in den geprüften Windows- und Linux-Abläufen gut nutzbar. Eine vollständige
Bewertung mit Höchstpunktzahl ist wegen dieser offenen Abnahmefälle noch nicht
begründet. Die folgenden Nachweise nennen jeweils ihre Revision und Umgebung.

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
Der ARM64-Job ist noch nicht abgeschlossen. Aus dem Intel-Teilergebnis wird
keine plattformübergreifende Freigabe abgeleitet.

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
