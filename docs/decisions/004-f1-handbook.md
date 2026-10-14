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
| Barrierefreiheit | Windows UIA liest vollständigen Text, bedient Suchfelder und veröffentlicht interne Links mit Textbereichen, Fokus und Aktivierung; unabhängige Clientprüfungen bestehen | Native Text-/Feld-/Linkbereiche auf macOS/Linux, Tabellensemantik und echte Screenreader-Aufgaben |

Die Hilfe ist in den geprüften Windows-Abläufen gut nutzbar. Eine vollständige
Bewertung mit Höchstpunktzahl ist wegen dieser offenen Abnahmefälle noch nicht
begründet. Die folgenden Nachweise nennen jeweils ihre Revision und Umgebung.

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

Offen sind vollständige assistive Feldbedienung und ihre native
macOS-/Linux-Anbindung, native Linktextbereiche auf macOS/Linux, native
Änderungs-/Aktivierungsereignisse der Links, Tabellensemantik, vollständiges natives
Lesen auf macOS/Linux, praktische Screenreader-Abnahme
und die Abnahmematrix für Dichte, Themes sowie Linux und macOS. Diese Liste
beschreibt Restarbeit und erteilt keine Produktfreigabe.
