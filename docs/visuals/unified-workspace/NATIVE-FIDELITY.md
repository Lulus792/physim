# Native Oberfläche: Abgleich mit den festgelegten Referenzen

Auftrag vom 20. Oktober 2026: Zuerst die gesamte Oberfläche einschließlich
Entwickeln, Simulieren, Auswerten und F1 möglichst identisch zur gewählten
Gestaltung umsetzen. Jede verbleibende Abweichung braucht eine konkrete
Begründung. Die bisherigen Funktionsprüfungen oder ein Vergleich zweier nativer
Versionen sind keine vollständige Designabnahme.

Die [sieben tatsächlichen Nutzeraufnahmen vom 21. Oktober](USER-REVIEW-2026-10-21.md)
zeigen weitere sichtbare Mängel, insbesondere die alte Berichtsbedienung,
Auswahlpfeile, Szenenzeilen, Inspektorabschnitte und Workspace-Symbole. Sie haben
Vorrang beim nächsten Komponentenabgleich. Die Gesamtabnahme wird dadurch nicht
auf diese sieben Ausschnitte begrenzt.

Der anschließende [native Komponentenabgleich](COMPONENTS-NATIVE.md) korrigiert
Auswahlpfeile, Workspace-Chevrons, umbrechende Szenenzeilen und Inspektorregeln
sowie dessen Modellfehler-Scrollblockade. Sechs Windows- und sieben Linux-
Gruppen bestehen. Das ersetzt keine vollständige visuelle, Assistive- oder
Plattformabnahme. Der anschließende [native Berichts-Exportdialog](GRAPHIC-DIALOG-NATIVE.md)
ersetzt die alte breite Exportleiste durch den freigegebenen Zugang mit Vorschau,
Optionen, PNG/SVG und nativer Bildkopie. Die dort aufgeführten Abweichungen
und die übrige Oberfläche bleiben offen. Der folgende
[sichtbare Messlaufvergleich](ANALYSIS-COMPARISON-NATIVE.md) ergänzt jetzt
echte Vergleichskurven, Statistik und den gemeinsamen Exportzugang.
Der [gemeinsame Float-Zeichenpfad](FLOAT-GEOMETRY-NATIVE.md) erhält nun gebrochene
Widget- und Zeichenmaße bis zur Rasterung. Die Exportkonturen werden gezielt
am tatsächlichen Pixelraster ausgerichtet. Die GPU-Gegenproben und die
begrenzten Referenzprüfungen belegen weiterhin keine vollständige Designgleichheit.

Der folgende [Abgleich der Diagrammwerkzeuge und Workspace-Pfade](CHART-TOOLS-PATHS-NATIVE.md)
vereinheitlicht Zoom-/Exportkontrollen nach den ursprünglichen CSS-/SVG-Maßen.
Vollständige Pfade umbrechen innerhalb des Fensters; außergewöhnlich lange
Tooltip-Inhalte lassen sich ohne Bewegung des Editors scrollen. Die dort
getrennten nativen Verhaltens-, Farb- und Bildbelege sind ebenfalls Teilnachweise.

Der folgende [Abgleich von Dateitabs und Simulations-Mindesthöhen](WORKBENCH-TABS-NATIVE.md)
übernimmt natürliche Tabbreiten, kräftige C-/P-Zeichen, Projektraute und
Verzeichnisabstände. Ein geöffneter Protokollbereich verdrängt die Messfläche
nicht mehr; dessen letzte Diagnose ist zusätzlich mit einer negativen
Bildgegenprobe geprüft. Die große Linux-Schrift zeigt noch konkrete Fehler an
Tempoauswahl und Laufsteuerung. Der folgende
[Transportabgleich](SIMULATION-TRANSPORT-NATIVE.md) korrigiert diese beiden
Befunde und den dabei belegten nativen Menü-Fokusfehler. Die gesamte Oberfläche
bleibt weiterhin offen.

## Verbindlicher Umfang

Für die drei Hauptbereiche gelten die sechs Bilder und die interaktive Vorlage
in diesem Verzeichnis. Die Einzelverträge ergänzen Zustände und Fachfunktionen:

- [Entwickeln / Werkbank Klarglas Kühl](../develop-designs/DESIGN.md).
- [Simulieren / Messlabor, Wertefelder B](../simulation-designs/DESIGN.md).
- [Auswerten / Messdaten und Analyseergebnis](../analysis-workspace/DESIGN.md).
- [F1 / Design H](../f1-layouts/DESIGN-H.md).
- [Gemeinsame Trenner](RESIZING.md).
- [Vom Nutzer freigegebener Grafikexport](EXPORT-EXAMPLE.md), verbindlich nach
  [ADR 010](../../decisions/010-analysis-graphic-export.md). Der Exportdialog mit
  Vorschau, PNG/SVG, Gesamt-/Ausschnittswahl und nativer Bildkopie gehört zum
  aktuellen Folgeauftrag und muss eine eigene native Abnahme erhalten.

Die jüngere Nutzerentscheidung zu festen Panelplätzen gilt weiterhin:
Herausziehen an vorhandener Überschrift beziehungsweise vorhandenem Tab,
echtes externes Fenster, Rückkehr über dessen Schließen. Keine zusätzlichen
Auslagerungsleisten, Platzhalterkarten oder Popout-Schaltflächen.

## Erster Umsetzungsschritt: gemeinsamer Rahmen

Der native Rahmen verwendet jetzt 38 px Menüzeile, 40 px Tabzeile mit
abschließender 1-px-Linie, die 39 px hohe Workspace-/Bereichszeile und die
28 px hohe Statuszeile. Bei 1440 px stimmen die aktiven Tabflächen mit den
PNG-Referenzen überein. Die Bereichszeile zeigt tatsächlichen Workspace,
Arbeitsbereich und Verarbeitungs-/Dirty-Status; die Folgeaktion wechselt den
vorhandenen Arbeitsbereich.

Die gemeinsamen semantischen Farben folgen `workspace.css` und dem Palettenblock
aus `bridge.js`. Die hellen Szenen verwenden deshalb jetzt `#e6e9ee`.
Paneelgrenzen belegen keine zusätzliche leere Layoutspalte. Sichtbare Regeln
werden als ein Pixel breite Flächen gezeichnet; die bisherigen breiteren,
teilweise weich gerasterten Linien entsprechen nicht der Vorlage.

Der Simulationskopf erhält die vorhandene Modellbezeichnung mit Kennzeichnung,
kompaktere Laufaktionen, getrennte Tempo-Beschriftung und Statuskapsel. Die
Messwerte erhalten größere Zahlen und ihre farbigen Linien. Die Kurvenhöhe,
Auswahl und Datenquelle bleiben fachlich gebunden. Der nachfolgende
[Werkbank-Kontextabgleich](WORKBENCH-CONTEXT-FLOW.md) erhält die Kartenhöhe auch
am kurzen Entwickeln-Fenster und macht die Buildinformationen durch Scrollen
erreichbar. Er ersetzt damit die frühere Höhenverkürzung.

Am Mindestfenster bleiben mindestens 180 logische Pixel für die Szene erhalten.
Der Messbereich besitzt ab 130 px einen eigenen Scrollbereich; Messkarten und
Diagrammachsen werden bei großer Schrift nicht in unbrauchbare Rechtecke
gequetscht. Sein Kopf mit dem Kurvenschalter bleibt erreichbar. Bei fokussiertem
„Kurven anzeigen“ scrollen Bild-ab und Bild-auf den Inhalt. Auch die sichtbare
Trefffläche des Kurventrenners ist auf diesen Bereich begrenzt. Herausgelöste
Messfenster benutzen ihre eigene tatsächliche Inhaltsbreite für 10-/40-px-Schritte.

`tests/check_unified_chrome.py` vergleicht echte Aufnahmen direkt mit den
festgelegten PNGs. Sein begrenzter Umfang ist ausdrücklich Clientgröße,
ausgewählte Flächen, Tababdeckung und zwei durchgehende horizontale Grenzen.
Die unveränderte Referenz besteht die Selbstprobe. Der frühere native
Zwischenstand verfehlt die beiden Linienprüfungen; er wird nicht nachträglich
als bestanden ausgegeben. Windows und Linux bestehen diesen Vergleich für
Entwickeln in Hell und Dunkel. **Das ist keine 100-%-Abnahme der Oberfläche.**

## Noch erforderlicher vollständiger Abgleich

| Bereich | Noch abzugleichen oder umzusetzen |
| --- | --- |
| Gemeinsamer Rahmen | [Fünf Menüs, direkter Theme-Wechsel und Akzentfokus](SHELL-NATIVE.md) sind nativ umgesetzt und auf Windows/Linux geprüft. Fensteraktionen, Statusinhalte und sämtliche Hover-/Fokus-/Auswahlzustände vollständig abgleichen. |
| Entwickeln | [Standardmaße, scharfe Grenzen, begrenzter Suchschatten und freie API-Aktion](WORKBENCH-GEOMETRY.md) sowie [gefüllte API-Karte, dokumentierte Beispiele und Protokollkopf](WORKBENCH-CONTEXT.md) sind weiter angeglichen. Der [Folgeabgleich des Kontexts](WORKBENCH-CONTEXT-FLOW.md) entfernt die unnötige Scrollbarspalte und übernimmt Kategorienlaufweite, zweizeilige Gliederung, Kartenabstände und native Seitenwechsel. Der [Dateitab-/Dateizeichen-Abgleich](WORKBENCH-TABS-NATIVE.md) übernimmt natürliche Breiten, Monospace-Zeichen, Projektraute und Chevron-/Pfadlinienabstände. Modellnotiz, Parametergruppierung, der alte Analysecode-Editorweg und weitere Baum-/Protokollzustände bleiben offen; sämtliche Schrift-/Seiten-/Plattformzustände vollständig abnehmen. |
| Simulieren | Der [Mindesthöhen-/Diagnoseabgleich](WORKBENCH-TABS-NATIVE.md) erhält die Messfläche auch bei geöffnetem Protokoll und prüft deren Herausziehen. Der [Transportabgleich](SIMULATION-TRANSPORT-NATIVE.md) übernimmt kompakte Tempo-Chevrons, natürliche einzeilige Aktionen, Originalabstände und sichtbare Glyphen; beide Schriftpfade und native Menü-/Fokusaufgaben bestehen auf Windows/Linux. Szenenleiste, Szenenkopf, Objektfelder, Gruppen, Kurven, Legenden und komplette Zeitleiste einschließlich Bedienelementpositionen weiter angleichen; alle acht Seiten-/Kurvenzustände. |
| Auswerten | Gewählte Bereichsanordnung ist nativ aufgebaut; Seitenzustände, Teilfenster, Kurve-/Statistik-Trenner und Scrollzugang bestehen auf Windows/Linux. [Felder, Konturen und Tabgrenze](ANALYSIS-CONTROLS.md) sowie [kompakte Einheitenwahl und Textkorrekturen](ANALYSIS-UNITS.md) sind weiter angeglichen. Weitere Kontrollformen, Einzüge, Fokusfarben, große Schrift und vollständiger Referenzvergleich bleiben offen. Die [gemeinsame Export-Datenbasis](GRAPHIC-EXPORT-NATIVE.md) kopiert Berichtsdiagramme und bis zu acht Messläufe mit eigenen Zeitachsen, Masken, Einheiten und vollständiger Statistik. [Zeichenbeschreibung und SVG](GRAPHIC-FIGURE-NATIVE.md) sowie [nativer Rasteradapter mit echten Schriften, vier PNG-Größen und RGB-Vorschautextur](GRAPHIC-RASTER-NATIVE.md) sind geprüft; der [sichtbare Mehrlaufvergleich](ANALYSIS-COMPARISON-NATIVE.md) verbindet inzwischen den Cache mit Kurven, Referenz, Statistik und freigegebenem Dialog. PNG/SVG und native Bildkopie sind auf Windows/Linux geprüft. Der [Float-Abgleich](FLOAT-GEOMETRY-NATIVE.md) entfernt frühe Koordinatenkürzung und erhält scharfe Exportfeldkonturen. Vollständige visuelle und externe Plattform-/Assistive-Abnahmen bleiben offen. [Aufbau und Fachgrenzen](ANALYSIS-NATIVE.md). |
| Auswerten: Daten | Bis zu acht echte Läufe mit eigenen Zeitachsen, passenden Kanälen/Dimensionen, Statuslücken, vollständiger Statistik und ausdrücklicher Referenz sind [nativ angebunden](ANALYSIS-COMPARISON-NATIVE.md). Noch offen bleiben sämtliche visuellen Zustände, externe/ausgehängte Quellen und die dort genannten Fehler-/Wiederholungsgrenzen. Worker- und Berichtsherkunft weiterhin erhalten. |
| F1 | Der aktuelle [Kontextdetail-Abgleich](../f1-layouts/CONTEXT-DETAILS.md) übernimmt die dreiseitige Codekopfkontur und den SVG-Maßstab der einzeiligen Bildunterschrift. Sechs native Gruppen je Windows/Linux bestehen, einschließlich Modellrastern mit System-/Offline-Schriften bei 1×/2×. Linux verwendet ausdrücklich temporäre ext4-Arbeitsdaten; Hardware-DPI-/Monitorwechsel sind damit nicht nachgewiesen. Die vorhergehende [Codevorschau](../f1-layouts/CODE-PREVIEW.md) erhält Tokens/Quellbytes und ergänzt horizontalen Tastatur-, Rad- und Ziehzugriff; native Eingabefälle, Unicode-Konformität, Windows-Hostfokus und Paket-Lizenzkopien sind separat geprüft. 112 vollständige native H-Bildpaare bleiben unverändert. Der vorhergehende [SVG-Schriftabgleich](../f1-layouts/SVG-LABELS.md) ergänzt echte kursive Serifen im Kontextmodell samt Offline-Font und geprüfter Lebensdauer. Der vorhergehende [Navigations-/Kontextabgleich](../f1-layouts/NAV-CONTEXT.md) übernimmt Seitenleistenmaße, Panelkante, Modellwerte und Beispielkopf. Sprachsegmente und Kontextreiter messen ihre wirklichen Schriftbreiten; die Unterstreichung ist zwei Pixel hoch. [Scrollabgleich](../f1-layouts/SCROLL.md), [Paarabstände](../f1-layouts/KERNING.md), [Artikelzeilen](../f1-layouts/ARTICLE-FLOW.md), [Schriftrollen](../f1-layouts/TYPOGRAPHY.md) und [Artikelaktionen](../f1-layouts/SECONDARY-ACTIONS.md) bleiben historische Teilnachweise. Die aktuellen nativen Belege trennen Quellstände, Plattformen, Sanitizer-Gegenfälle und Zeitfehler. Weiter offen: Schriftfamilien, Glyphen-/Kantenrasterung, weitere Kontext-/Fokuszustände und vollständige H-Abnahme. macOS und vollständige externe Assistive-Nachweise bleiben unbewiesen. |
| Gesamtabnahme | Native Aufgaben, Hell/Dunkel, Kontrast, reduzierte Transparenz, Textgrößen, Minimum/Normal/Maximiert, verfügbare DPI-/Monitorfälle, Tastatur, Fokuskehr und tatsächliche Assistive-Nachweise. |

Eine erfolgreiche Teilprüfung entfernt keine andere Zeile dieser Liste.
Die weitere Projektumsetzung folgt erst nach dem priorisierten Designauftrag.

## Inhalt und technische Grenzen unterscheiden

Quelltext, Dateinamen, Messkanäle, Modellobjekte, Zeiten, Einheiten und Berichte
kommen aus dem echten Projekt. Die Browserbeispiele liefern dafür keine
Produktdaten. Ein Modell mit zwei Messkanälen bekommt keinen erfundenen dritten
Kanal; unbekannte Modellangaben bleiben als unbekannt gekennzeichnet. Das erklärt
abweichenden Inhalt, aber keine anderen Farben, Abstände oder Kontrollformen.

Die Szene muss echte Modellgeometrie und vorhandene Kamera-/Darstellungsfunktionen
zeigen. Eine illustrative Browserkugel darf nicht das tatsächliche Modell
ersetzen. Abweichungen von Material oder Darstellung werden einzeln geprüft;
sie gelten nicht pauschal als unvermeidlich. Ebenso sind native Textrasterung
und Plattformschrift getrennt von korrigierbaren Schriftgrößen und Einzügen zu
beurteilen.

Ein nicht ausgeführter macOS-, Screenreader- oder DPI-Lauf bleibt ein fehlender
Nachweis. Weder ein CI-Eintrag noch ein passendes Bildschirmbild ersetzt ihn.
Die verbindlichen Originalreferenzen werden für die Anpassung nicht verändert.

## Ausgeführte native Teilprüfung am 20. Oktober 2026

Die [Prüfdaten](native-2026-10-20/validation.json) unterscheiden die breite
Version 10 von der abschließenden Version 13. Es sind interne Prüfstände dieses
Umsetzungsschritts, keine Produktversionen. Der Basiscommit, Quellhashes,
Programmhashes, ausgeführten Befehle und Bildquellen sind dort festgehalten.
Die Builds enthalten unverändert vorgefundene lokale Accessibility-Arbeit;
deren separat erfasste Dateien gehören nicht zu diesem Änderungsumfang.

| Ausführung | Tatsächlicher Umfang |
| --- | --- |
| Windows und Alpine-Dev / Version 10 | Je acht bestandene Gruppen: gemeinsamer Rahmen, Werkbank, Messlabor, native Teilfenster, großer/schmaler Inspektor, Toolbar, Tastaturmenüs und F1 H. |
| Windows und Alpine-Dev / Version 13 | Je drei erneut ausgeführte Gruppen für den abschließend geänderten Messbereich: Messlabor, native Teilfenster und großer/schmaler Inspektor. |
| Originalreferenzen gegen native Aufnahmen | Acht Bildfälle: Entwickeln und Simulieren × Hell/Dunkel × Windows/Linux; je elf begrenzte Prüfungen des gemeinsamen Rahmens. Entwickeln stammt aus Version 10, Simulieren aus Version 13. |
| Unabhängiger Windows-Fokusbeobachter / Version 10 | Kurzes Werkbankfenster und tatsächlicher 1440×1000-Client; Vordergrund, aktives Fenster und Hostfokus beobachtet, ohne Eingaben des Beobachters. |

Die Messlabor-Gruppe umfasst sechs Theme-/Größenvarianten und alle acht
Kombinationen von Szenenbaum, Inspektor und Kurven. Im 1080×740-Client mit
UI-Größe 22 scrollt Bild-ab den Messinhalt tatsächlich; Bild-auf kehrt zurück,
während Szene, Kamera und Zeit unverändert bleiben. Die Szene behält mindestens
180 logische Pixel. Die volle Fensterfläche bleibt dieselbe; eine andere
Clientgröße wird nicht nachträglich als Mindestfenster ausgegeben.

Der neue externe Kurventrenner-Test verwendet ein echtes 720×440-Fenster,
abweichend von der Breite seines früheren Hauptfensterplatzes. Er prüft sowohl
den gespeicherten Teilungswert als auch die gezeichnete Diagrammbreite: rechts
10 px, anschließend Umschalt+links 40 px und Enter auf die Mitte. Die übrige
Teilfenster-Prüfung öffnet alle sechs nativen Bereiche, erhält Eingaben und
Zustände, öffnet F1 und prüft Rückkehr sowie Wiedereingliedern.

Ein entdeckter Fehler wird als [negatives Bild](native-2026-10-20/counterexample-hidden-tooltip.png)
erhalten: Der Tooltip einer bereits weggescrollten Messkarte überdeckte den
festen Kopf. Die Hoverprüfung verwendet nun das sichtbare Schnittrechteck.
Die finale Aufnahme zeigt den freien Kopf unter
[Windows](native-2026-10-20/windows-minimum-scrolled-v13.png) und
[Linux](native-2026-10-20/linux-minimum-scrolled-v13.png).
Frühere fehlgeschlagene Versuche des neuen externen Tastaturtests sind keine
bestandenen Nachweise; der finale Test wartet auf die tatsächliche gezeichnete
Größenänderung und prüft weiterhin den vollen 10-/40-px-Vertrag.

| Native Aufnahme, 1440×1000 | Dunkel | Hell |
| --- | --- | --- |
| Windows / Entwickeln | [Bild](native-2026-10-20/windows-develop-dark.png) | [Bild](native-2026-10-20/windows-develop-light.png) |
| Linux / Entwickeln | [Bild](native-2026-10-20/linux-develop-dark.png) | [Bild](native-2026-10-20/linux-develop-light.png) |
| Windows / Simulieren | [Bild](native-2026-10-20/windows-simulate-dark-v13.png) | [Bild](native-2026-10-20/windows-simulate-light-v13.png) |
| Linux / Simulieren | [Bild](native-2026-10-20/linux-simulate-dark-v13.png) | [Bild](native-2026-10-20/linux-simulate-light-v13.png) |

Die versionierten PNGs wurden verlustfrei aus echten nativen BMP-Aufnahmen
konvertiert. Die dekodierten RGB-Pixel sind jeweils identisch; weder Referenzen
noch Nachweisbilder wurden retuschiert, skaliert oder für den Vergleich
umgefärbt. Die interaktive lokale HTML-Vorlage konnte das Browserwerkzeug wegen
seiner `file:`-Sperre in diesem Schritt nicht öffnen; daraus wird keine neue
Browserprüfung abgeleitet. PNGs sowie HTML-/CSS-Quellen waren lokal lesbar.

Die oben aufgelisteten offenen Bereiche bleiben offen. Insbesondere ist die
erfolgreiche F1-Funktionsgruppe noch kein neuer vollständiger Bildabgleich mit H,
Die danach nativ aufgebaute Auswertung ist getrennt in [ANALYSIS-NATIVE.md](ANALYSIS-NATIVE.md) dokumentiert; sie ist weiterhin keine vollständige Designabnahme.

Der folgende [native Zeitleistenabgleich](SIMULATION-TIMELINE-NATIVE.md) übernimmt
die ursprüngliche Kopf-/Zeit-/Spuraufteilung und ergänzt tatsächliche Zeitnavigation
bei adaptiven Schritten. Er behandelt außerdem Abbruch, Projektwechsel und
denselben Inhalt im herausgezogenen nativen Fenster. Die dort begrenzten
Nachweise ersetzen weiterhin keine Gesamtabnahme.

Der [Inspektor-Teilabgleich](INSPECTOR-FIELDS-NATIVE.md) übernimmt die endgültigen
B-Abschnittsköpfe, Modellwertkarten, Inhaltsbreite und innere Fußzeile. Er hält
auch die trotz bestandener Funktionssuiten gefundenen nativen Überdeckungs- und
Wortumbruchfehler fest. Vollständiger Inspektorinhalt, Szenenbaum, F1 und die
übrigen offenen Bereiche bleiben weiter abzugleichen.

Der folgende [Objektabgleich](OBJECT-PROPERTIES-NATIVE.md) ergänzt den fehlenden
Auswahlkopf und die B-Eigenschaften mit tatsächlichen Radius-/Positionswerten,
getrennten Koordinaten und den beiden vorgegebenen Aktionsformen. Die native
Zeiger-/Tastaturprobe erhält Quelle und Lauf und prüft auch schmale lange Namen.
Fehlende allgemeine Modellnamen-/Massenbindungen und die verbleibende native
Windows-/Assistive-/DPI-Abnahme sind dort ausdrücklich begrenzt; die übrige
Oberfläche und der Szenenbaum bleiben offen.

Der folgende [Szenenbaumabgleich](SCENE-TREE-NATIVE.md) entfernt die vorgeschalteten
Altabschnitte links und übernimmt Baumzeilen, leise Sichtbarkeit, unteren
Darstellungsbereich und den linken Resizebereich 150–420 px. Reale Hierarchie,
Tastaturauswahl und gemeinsam gebundene linke/rechte Kamera bleiben erhalten.
Der dabei gefundene Fokusfehler überkreuzender Separatoren und die falsche
gespeicherte Startgröße sind mit eigenen Vorher-/Nachher-Belegen erfasst.
Die Kamera-Presetgestaltung und weitere Gesamtzustände bleiben offen; dies
ist kein vollständiger Szenen-, Inspektor- oder Drei-Plattform-Abschluss.

Der folgende [Kamera-Abgleich](CAMERA-CONTROLS-NATIVE.md) übernimmt die flachen
B-Auswahlfelder, Zahlenwerte, Regler und Speicheraktion. Er korrigiert echten
Tastatureingang für Pos1/Ende, Spurklick und abgeschnittene Fokuslinien. Bei
schmaler Spalte und großer Schrift bleibt die Höhe beim Wechsel zur freien
Ansicht stabil, damit der gezogene Regler nicht verrutscht. Die native Probe
prüft beide Schriftquellen und erhält den realen pausierten Versuch. Die
zugeordneten acht Produktgruppen und der Windows-Build schließen die übrige
Szenengestaltung, den gesamten Inspektor, F1 oder andere Plattformen nicht ab.

Der folgende [Szenenrahmen-Abgleich](SCENE-FRAME-NATIVE.md) ergänzt den oberen
Kameraüberzug und die untere Ansichtsleiste. Reale Kameraeingabe, Tabfolge,
Beschriftungsabstand und unveränderte Versuchsdaten werden nativ geprüft.
Herausgezogene Bereiche behalten bei gleicher Inhaltsbreite ihre Designvariante.
Der Gesamtauftrag und praktische Windows-/Assistive-/Monitorprüfungen bleiben offen.

Der folgende [Messbereich-Abgleich](MEASUREMENTS-NATIVE.md) bearbeitet die
Zahlen-/Einheitenhierarchie, Karte und Schalter sowie die flache Kurvenzeichnung.
Die native Probe erhält reale Daten und prüft Kanalwahl, Anzeigeumrechnung und
Rückkehr. Ein sichtbarer Unterstrichfehler und falsche Aufnahmezeitpunkte sind
mit Gegenbelegen erfasst. Weitere Kanalkopf-/Status-/Langtextzustände und die
praktische Plattformabnahme bleiben offen.
