# Native Oberfläche: Abgleich mit den festgelegten Referenzen

Auftrag vom 20. Oktober 2026: Zuerst die gesamte Oberfläche einschließlich
Entwickeln, Simulieren, Auswerten und F1 möglichst identisch zur gewählten
Gestaltung umsetzen. Jede verbleibende Abweichung braucht eine konkrete
Begründung. Die bisherigen Funktionsprüfungen oder ein Vergleich zweier nativer
Versionen sind keine vollständige Designabnahme.

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
| Entwickeln | [Standardmaße, scharfe Grenzen, begrenzter Suchschatten und freie API-Aktion](WORKBENCH-GEOMETRY.md) sowie [gefüllte API-Karte, dokumentierte Beispiele und Protokollkopf](WORKBENCH-CONTEXT.md) sind weiter angeglichen. Der [Folgeabgleich des Kontexts](WORKBENCH-CONTEXT-FLOW.md) entfernt die unnötige Scrollbarspalte und übernimmt Kategorienlaufweite, zweizeilige Gliederung, Kartenabstände und native Seitenwechsel. Modellnotiz, Parametergruppierung, Dateitabs, Baum und weitere Protokollaktionen bleiben offen; sämtliche Schrift-/Seiten-/Plattformzustände vollständig abnehmen. |
| Simulieren | Szenenleiste, Szenenkopf, Objektfelder, Gruppen, Kurven, Legenden und komplette Zeitleiste einschließlich Bedienelementpositionen weiter angleichen; alle acht Seiten-/Kurvenzustände. |
| Auswerten | Gewählte Bereichsanordnung ist nativ aufgebaut; Seitenzustände, Teilfenster, Kurve-/Statistik-Trenner und Scrollzugang bestehen auf Windows/Linux. [Felder, Konturen und Tabgrenze](ANALYSIS-CONTROLS.md) sowie [kompakte Einheitenwahl und Textkorrekturen](ANALYSIS-UNITS.md) sind weiter angeglichen. Weitere Kontrollformen, Einzüge, Fokusfarben, große Schrift und vollständiger Referenzvergleich bleiben offen. Der neue Exportdialog ist noch umzusetzen. [Aufbau und Fachgrenzen](ANALYSIS-NATIVE.md). |
| Auswerten: Daten | Echte begrenzte Vorschauen mehrerer Läufe mit deren eigenen Zeitachsen, passenden Kanälen/Dimensionen und Statuslücken; gesamte Laufstatistik, ausdrücklicher aktiver Lauf und Referenz; Worker- und Berichtsherkunft erhalten. |
| F1 | Der [Artikelabgleich](../f1-layouts/SECONDARY-ACTIONS.md) entfernt die Zusatzleiste und gefüllte Zoomknöpfe. Der [Schriftrollen-Abgleich](../f1-layouts/TYPOGRAPHY.md) übernimmt die CSS-em-Größen mit aufbewahrten Fontmetriken und durchgehenden Codegrafiken. Linux besteht elf verschiedene Prüfgruppen über zwei Läufe mit gleicher Darstellungslogik, Windows neun; H umfasst 56 Zustände, Inline-Formeln 48 Fälle. Windows-H-/Seitenfokus und macOS bleiben offen. Zeilen-/Absatzabstände, Laufweiten, erste Absatzrolle, Navigation, Segmente, Kontext-Tabs und vollständigen Formelsatz weiter abgleichen. |
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
