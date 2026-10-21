# Werkbank: Kontextmaße, Schriftlaufweite und Scrollzugang

Folgeabgleich vom 21. Oktober 2026 auf Grundlage von `6f527f6`.
Die unveränderten Original-PNGs und `develop-designs/workbench.css` bleiben die
Referenz. Der [vorherige Bericht](WORKBENCH-CONTEXT.md) beschreibt Stand 47.

Nachfolgende F1-Arbeiten und aktuelle Plattformgrenzen stehen im
[Abgleich der Artikelansicht und Aktionen](../f1-layouts/SECONDARY-ACTIONS.md).
Die unten enthaltene F1-Prüfung beschreibt den früheren Stand 52.

## Native Umsetzung

Die Kontextspalte berechnet ihren vollständigen Platzbedarf vor dem Öffnen des
Nuklear-Panels. Dieselben Fontmetriken, Umbrüche, Kartenmaße und Abstände werden
anschließend gezeichnet. Passt der Inhalt bei voller Breite, reserviert sie
keine Scrollbarspalte. Andernfalls erhält sie den vorhandenen nativen Scrollweg.
Diese Entscheidung benötigt keinen Vorframe-Cache und keine Fensterhöhen-Heuristik.
Beim Zurückvergrößern verschwindet die Leiste wieder; ein dann überflüssiger
Scrollversatz wird auf null begrenzt.

Der Codekontext nutzt die Zeilenboxen, Abstände und Schriftrollen der Vorlage.
Kategorien haben 1,6 px Laufweite, die kleinere API-Kategorie 0,8 px. Die
Gliederung verwendet monospaced Namen, darunter die tatsächliche Symbolart
und Quelltextzeile. Sie behält ihren eigenen Scrollbereich und den bisherigen
Sprung mit Auswahl, Fokus und Undo. Der zuvor fehlende Buchstabe `ƒ` gehört
jetzt ausdrücklich zu den geladenen Fontbereichen; sein tatsächlicher Glyphenbesitz
wird geprüft. Namen und Herkunft werden aus der aktiven Quelle gelesen.

Die API-Karte berücksichtigt den ein Pixel breiten Rahmen zusätzlich zu den
13 px CSS-Innenabstand. Beschreibung und Code verwenden volle Zeilenboxen;
die Argumentliste hat 13 px Abstand vor dem ersten Hinweis und 6 px zwischen
Hinweisen. Der Rahmen wird aus zwei gerundeten nativen Flächen gezeichnet;
seine geraden Ein-Pixel-Stücke werden anschließend deckend gefüllt.
Der bisherige ganzzahlige Nuklear-Konturbefehl verliert einen halben Pixelversatz
und erzeugt an den geprüften geraden Kanten weichere Farben als die Vorlage.
Auch die innere gerundete Fläche kann eine gerade Kante um einen Farbwert
aufhellen. Die zusätzlichen geraden Füllflächen verhindern diese Mischung,
ohne die Rundungen durch rechteckige Ecken zu ersetzen.

Die Buildumgebung führt Compiler, tatsächliche API-Version und das aktuelle
Modulziel. Die Profilwahl bleibt in der vorhandenen Fußzeile erreichbar.
Lange Werte umbrechen; die Compilerbeschriftung reserviert die aufgerundete
Glyphenbreite. Eine unsichtbare horizontale Scrollbar belegt keinen zusätzlichen
Streifen unter der Gliederung oder dem Kontext.

## Bedienung bei wenig Platz

Ein kurzes Fenster behält den Kartenaufbau der Vorlage. Es verkleinert die
Karte nicht mehr allein deshalb, um gleichzeitig alle Buildangaben zu zeigen.
Stattdessen lassen sich diese tatsächlich im äußeren Kontext nach unten scrollen.
Das ersetzt die frühere Anpassung aus Stand 40/47 ausdrücklich im Sinne des
später verlangten direkten Vorlagenabgleichs.

Hat der Kontextschalter oder ein Bedienelement im Kontext den Tastaturfokus,
scrollen **Bild-ab/Bild-auf** den äußeren Bereich. Der sichtbare Kontextschalter
behält dabei den Fokus. Das separate Kontextfenster unterstützt denselben
Seitenwechsel. Die Tasten werden vor der Editorroute verarbeitet; Quelltext und
Analyse werden dadurch nicht bearbeitet. Es werden keine sichtbaren zusätzlichen
Buttons oder Auslagerungsleisten eingeführt.

Die äußere Scrollprüfung setzt das Mausrad ausdrücklich über den äußeren
Randbereich. Über der Gliederung gehört das Rad ihrem eigenen inneren
Scrollbereich. Eine vollständige Gleichheit aller verschachtelten
Browser-Scrollketten ist damit noch nicht nachgewiesen.

## Ausgeführte Prüfung

Die [Prüfdaten vom 21. Oktober](workbench-context-flow-2026-10-21/validation.json)
binden Quellstand 52 an Programmhashes, tatsächliche Befehle, Beobachtungen und
17 unveränderte Werkbank-RGB-Aufnahmen sowie sechs zusätzliche F1-Aufnahmen.
Die Nummern 50 bis 53 bezeichnen interne
Prüfstände, keine Produktversionen. Alle Builds enthalten die separat erfassten,
unverändert vorgefundenen Accessibility-Dateien.

| Ausführung | Tatsächliches Ergebnis |
| --- | --- |
| Windows / MSVC ASan, `run-gd_xcb6d` | Gliederung 3/3 Varianten, Werkbank 9/9 und API-Referenz 2/2 bestanden. API-Karte: C dunkel, Physim hell und C mit UI 22 im schmalen Fenster bestanden. Die vierte Variante scheiterte vor ihrer ersten Aktion am fehlenden Betriebssystemfokus. Zusammen 17 von 18 Varianten bestanden; die API-Gruppe ist ausdrücklich fehlgeschlagen. |
| Alpine-Dev / ASan/UBSan, `run-aw1jw1fr` | Dieselben ersten drei Gruppen mit 14 Varianten bestanden. In der API-Gruppe bestanden drei Varianten; die vierte scheiterte nach dem Schließen des externen Fensters bei der Fokusrückkehr in der Xvfb-Anzeige ohne Fenstermanager. |
| Alpine-Dev / eigene Xvfb-Anzeige mit Openbox 3.6.1, `run-637lhdpn` | Die vollständige API-Gruppe erneut 4/4 bestanden, einschließlich externem Fenster, Bild-ab/Bild-auf, Schließen, Rückkehr in denselben Platz, Undo/Redo, tatsächlichem Build und Messlauf. Zusammen mit den unveränderten übrigen Gruppen sind alle 18 Linux-Varianten auf demselben Programmhash bestanden. |
| Direkter Referenzvergleich | Windows/Linux × Hell/Dunkel: je sechs begrenzte Kartenrahmenprüfungen und 13 Prüfungen der gemeinsamen Werkbankgrenzen bestanden. Beide Originalreferenzen bestehen die Selbstprüfung; die echte helle Aufnahme aus Stand 51 wird wegen ihrer abweichenden oberen/unteren Randfarbe abgewiesen. |

Die normale C-Karte liegt auf beiden Plattformen bei `1202,417,222,236`;
die Aktion bei `1216,609,194,30`. Der gesamte Kontext misst in diesem Zustand
697,5 logische Pixel, sowohl vorausberechnet als auch tatsächlich gezeichnet.
Die längere echte Physim-Referenz benötigt 270 statt 236 Pixel Kartenhöhe.
Bei wenig Breite oder großer Schrift wächst der notwendige Text-/Scrollraum;
es wird keine Beschreibung zum Erreichen der Beispielhöhe gekürzt.

Die Seitentasten verändern den tatsächlichen äußeren Scrollversatz und lassen
den Quelltext unverändert. Bild-auf bringt ihn auf null zurück. Nach der Rückkehr
auf 1440 × 1000 sind Karte und Kontext wieder ohne Scrollbarspalte, mit Breite
222 und Versatz null sichtbar; Quelltext und Analyse bleiben erhalten.

Die Windows-Ausnahme bleibt offen: `SDL_GetKeyboardFocus()==0` über den ganzen
Startwartezeitraum, Timeout nach 120 Sekunden in Stufe 0. Warum Windows die
Aktivierung in diesem Lauf verweigert hat, ist noch nicht abschließend belegt.
Der Linux-Negativlauf erreicht dagegen Stufe 8 nach erfolgreichen Seitentasten
im externen Fenster. Die lokale SDL-X11-Implementierung sendet beim Anheben
`_NET_ACTIVE_WINDOW` an den Fenstermanager; die Wiederholung mit vorhandenem
Openbox besteht ohne Änderung der Anwendung oder Abschwächung der Fokusprüfung.
Die vorherigen Fehler werden durch den erfolgreichen Wiederholungslauf nicht
nachträglich in bestandene Ergebnisse umgeschrieben.

Stand 50 bestand bereits alle vier Gruppen auf beiden Plattformen. Die
Quellfingerprints zeigen bis Stand 52 ausschließlich Änderungen an
`workbench_api_ui.inc` (Rahmenzeichnung) und `api_card_tests.inc`
(zusätzliche Diagnoseausgaben). Dieser historische Nachweis ersetzt die fehlende
aktuelle Windows-Ausführung des externen Falls nicht.

| Aktuelle native Ansicht | Windows | Linux |
| --- | --- | --- |
| C-Referenz dunkel | [Aufnahme](workbench-context-flow-2026-10-21/windows-api-c-dark.png) | [Aufnahme](workbench-context-flow-2026-10-21/linux-api-c-dark.png) |
| Physim-Referenz hell | [Aufnahme](workbench-context-flow-2026-10-21/windows-api-phys-light.png) | [Aufnahme](workbench-context-flow-2026-10-21/linux-api-phys-light.png) |
| Große Schrift / schmales Fenster | [Aufnahme](workbench-context-flow-2026-10-21/windows-api-large-narrow.png) | [Aufnahme](workbench-context-flow-2026-10-21/linux-api-large-narrow.png) |
| Bild-ab / kurzes Fenster | [Aufnahme](workbench-context-flow-2026-10-21/windows-context-keyboard.png) | [Aufnahme](workbench-context-flow-2026-10-21/linux-context-keyboard.png) |
| Wieder vergrößert | [Aufnahme](workbench-context-flow-2026-10-21/windows-context-restored.png) | [Aufnahme](workbench-context-flow-2026-10-21/linux-context-restored.png) |

[Aktuelles externes Linux-Fenster](workbench-context-flow-2026-10-21/linux-api-detached.png)
und [Rückkehr in den festen Platz](workbench-context-flow-2026-10-21/linux-api-returned.png).
Das [negative Rahmenbild aus Stand 51](workbench-context-flow-2026-10-21/counterexample-light-frame-51.png)
bleibt mit seinem tatsächlichen abgewiesenen Vergleich erhalten.

Die Codeprüfung verfolgt die schreibgeschützte Fokusabfrage unter dem bestehenden
Accessibility-Mutex, die Eingaberoute vor dem Editor und die Lebensdauer der
gespeicherten Schrift-Handles. Messung und Zeichnung teilen Umbrüche und
Zeilenboxen. Diese Prüfung wurde vom umsetzenden Agenten vorgenommen; eine
unabhängige Zweitprüfung wird damit nicht behauptet.

### Ergänzende aktuelle F1-/Formelprüfung

Der unveränderte Laufzeitstand besteht unter Linux mit Openbox zusätzlich
`documentation_h` und `documentation_inline_math` in `run-po1bhx2a`:
56 tatsächliche H-Zustände mit unabhängigen Seiten und Kontrollpixeln sowie
48 Inline-Formelfälle über sechs Themewechsel und zwei Fenstergrößenwechsel.
Der direkte Bildvergleich mit `h-dark.png` bleibt eine gesonderte Sichtprüfung;
die Zustandsmatrix allein ist keine vollständige Referenzabnahme.

`documentation_math` meldet in diesem ersten Lauf beide nativen Teilprüfungen
als bestanden, scheitert jedoch anschließend im Python-Prüfer an dessen
veralteter Erwartung von 54 primären Schrift-Handles. Tatsächlich besitzen die
Hauptfensterbanken vier reguläre, vier Titel- und sechs Codeschriften; F1 besitzt
neun reguläre, neun Titel-, elf starke, neun Code- und vier kleine UI-Schriften.
Zusammen sind das genau 56. Stand 53 ändert ausschließlich diesen Prüfer und
fordert nun zusätzlich die vollständige Verteilung: 17 Noto Sans, 24 Noto Sans
Bold und 15 Noto Sans Mono. Laufzeitquellen und Programmhash bleiben unverändert.

Der erneute Linux-Lauf `run-8u7jrgs2` besteht die vollständige Mathematikgruppe
einschließlich der gebündelten Schriftvariante: 183 Zeichen in allen 42
F1-Schriftvarianten und 185 echte Seiten mit 1082 Links. Die frühere abgewiesene
Prüfung bleibt in den Daten erfasst.

Aktuelle native Bilder: [H dunkel](workbench-context-flow-2026-10-21/linux-f1-h-dark.png),
[H hell](workbench-context-flow-2026-10-21/linux-f1-h-light.png),
[Minimum](workbench-context-flow-2026-10-21/linux-f1-h-minimum.png),
[UI 22 / Leser 32](workbench-context-flow-2026-10-21/linux-f1-h-large.png),
[Formelsatz bei 32](workbench-context-flow-2026-10-21/linux-f1-math-32.png)
und [gebündelte Glyphen](workbench-context-flow-2026-10-21/linux-f1-bundled-glyphs-32.png).
Die zusätzliche native Artikelwerkzeugzeile und die Zoomaktionen in der Fußzeile
sind weiterhin sichtbare Unterschiede zur H-Vorlage. Ihre Gestaltung ist im
priorisierten Gesamtauftrag weiter anzugleichen; sie gelten nicht als technisch
unvermeidliche oder bereits abgenommene Abweichungen.

## Weiter offen

Dieser Abgleich ist keine vollständige Abnahme aller Oberflächen. Die native
Gliederung zeigt tatsächliche Symbole und Herkunft statt der drei illustrativen
Pendel-Einträge der Vorlage. Parametergruppierung, Dokument-/Modellnotizen,
weitere Baum-/Tab-/Protokollzustände sowie sämtliche Zustände von Simulieren,
Auswerten und F1 bleiben im [Gesamtauftrag](NATIVE-FIDELITY.md).

Die Begrenzung auf gerade Rahmenstücke beweist keine vollständige Gleichheit
von Eckenglättung, Glyphenrasterung und allen Hover-/Fokuszuständen. Native
macOS-Prüfung, physische Maus-/Monitor-DPI-Aufgaben, neue externe Screenreader
und ein sauberer Checkout bleiben gesondert offen. Die Builds enthalten die
separat erfasste lokale Accessibility-Arbeit.
