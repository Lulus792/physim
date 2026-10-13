# F1 Handbuch und Abnahme

Entscheidung vom 13. Oktober 2026: F1 soll sowohl den ersten abgeschlossenen
Versuch ermöglichen als auch beim Arbeiten präzise Antworten liefern. Die
Abnahme betrachtet Inhalt, Bedienung, Darstellung und zugängliches Lesen.
Ein grüner Parsertest allein ist keine vollständige Abnahme.

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
bleibt offen und bestätigt keine Linux-Funktionsgleichheit.

Offen sind natives Scrollen zu verdeckten Feldzeichen, vollständige assistive
Feldbedienung und ihre native macOS-/Linux-Anbindung, eingebettete Link-/Tabellensemantik, vollständiges natives
Lesen auf macOS/Linux, praktische Screenreader-Abnahme
und die Abnahmematrix für Dichte, Themes sowie Linux und macOS. Diese Liste
beschreibt Restarbeit und erteilt keine Produktfreigabe.
