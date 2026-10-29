# Markierung und Kontextmenü – A2 freigegeben

Stand: **29. Oktober 2026. A2 · Dezenter Schleier ist vom Nutzer als finales
Markierungsdesign freigegeben und ersetzt die frühere Grundlage A.**
[Verbindliche Spezifikation](APPROVED.md) · [ADR 016](../../decisions/016-selected-marking-a2.md).
Die freigegebene Kombination aus
[ADR 011](../../decisions/011-selected-workspace-additions.md) und der
[Grafikexport aus ADR 010](../../decisions/010-analysis-graphic-export.md) bleiben
erhalten. Diese Dateien verändern weder die bestehenden Vorschauen noch die
native Anwendung.

[Freigegebene A2-Referenz öffnen](index.html?approved=1&context=code) ·
[A](index.html?variant=a&context=code) · [B](index.html?variant=b&context=code) ·
[C](index.html?variant=c&context=code) · **[A2 im Vergleich](index.html?variant=a2&context=code)**

Lokal auch unter `http://127.0.0.1:8791/selection-options/` erreichbar, wenn der
Vorschau-Server läuft. Keine Installation oder externen Ressourcen nötig;
`index.html` funktioniert direkt als lokale Datei.

## Direkter Nutzerauftrag

Der Nutzer beanstandet, dass der Inhalt unter der bisherigen Textmarkierung
nicht erkennbar ist, und wünscht Beispiele für Markierungen mit passenden
Aktionen. Anschließend konkretisiert er: **Auswahloptionen nur bei Rechtsklick**.
Diese Bedienregel ist ausdrücklich vorgegeben. Anschließend wählt er **A** und
bittet um eine zusätzliche, transparentere und weniger blaue Alternative.
Anschließend gibt er **A2 ausdrücklich für das fertige Design und die Umsetzung
im anderen Chat frei**. Diese Entscheidung ersetzt A. Eine automatisch
erscheinende Aktionsleiste gehört zu keiner Variante.

## Gestaltungen mit identischer Bedienung

| Variante | Text | Zeilen | Szenenobjekte |
| --- | --- | --- | --- |
| **A · Sanfte Fläche**, historischer Vergleich | Transparente Akzentfläche hinter der Schrift; Syntaxfarben bleiben erhalten | Tönung, Akzent links, Häkchen | Dünner Rahmen, Griffe, dezente Fläche hinter Objekt und Beschriftung |
| **A2 · Dezenter Schleier**, final freigegeben | Transparenteres, nahezu neutrales Blaugrau; Syntaxfarben bleiben erhalten | Schwächere Fläche, graublauer Akzent, Häkchen | Dieselben Rahmen und Griffe wie A, neutraler und mit transparenterer Fläche |
| **B · Feine Kontur** | Sehr schwache Tönung und feine Unterstreichung; Syntax bleibt erhalten | Kontur, sehr schwache Fläche, Häkchen | Kontur und Griffe ohne zusätzliche Fläche |
| **C · Klarer Kontrast** | Eigene deckende Auswahlfläche mit ausdrücklich zugeordneter Textfarbe; Syntaxfarben werden vereinheitlicht | Kontrastfläche und passende Schrift, Häkchen | Kräftiger Rahmen und gefüllte Griffe; Beschriftungen erhalten ihre Farbe |

Die Material- und Textrollen folgen den gemeinsamen Werkbankfarben, auch für
Protokollbeispiele. Die ausgewählte Navigator-C-Anordnung wird hier nicht neu
entworfen; die Studie zeigt nur die betroffenen Auswahlkomponenten.

Die Auswahl liegt in der Zeichenreihenfolge **hinter** dem Inhalt. Die Textfarbe
wird separat bestimmt, statt ein halbtransparentes Rechteck über fertige Glyphen
zu zeichnen. A/A2/B definieren auch für Schlüsselwörter, Zahlen, Aufrufe und Kommentare
eigene Auswahl-Schriftrollen. C verwendet bewusst ein einheitliches Paar:
dunkles Theme `#b8d5ff` / `#152a45`, helles Theme `#205da8` / Weiß.

A2 verwendet im dunklen Theme `rgba(170,184,202,.15)` statt As
`rgba(95,155,237,.26)`, im hellen Theme `rgba(90,108,130,.11)` statt
`rgba(46,116,206,.18)`. Graublaue Objekt-/Zeilenkonturen ergänzen die weniger
auffällige Fläche. Die eigentlichen Textfarben und die sichtbare Tastaturfokuslinie
bleiben erhalten. Es wird nur die Auswahlfläche transparenter, nicht der Inhalt.
A bleibt unverändert daneben sichtbar. Beim Umschalten bleibt derselbe
Textbereich ausgewählt, damit der Vergleich direkt möglich ist.

## Interaktionen in der Vorschau

| Eingabe / Zustand | Ergebnis |
| --- | --- |
| Linksklick oder Ziehen über Text | Echte Browser-Textauswahl, auch über mehrere Zeilen. Kein Menü. |
| „Beispiel markieren“ / initialer Beispielzustand | Zeigt die Auswahl zur Beurteilung, ohne Aktionen einzublenden. |
| Rechtsklick im Text | Vorhandene Textauswahl erhalten; ohne Auswahl Wort unter dem Zeiger wählen, sofern vorhanden. |
| Linksklick auf Zeile / Objekt | Einzelne Auswahl, kein Menü. |
| Strg/⌘+Klick auf Zeile | Einzelne Elemente ergänzen / entfernen. |
| Umschalt+Klick auf Zeile | Zusammenhängenden Bereich ab dem Auswahlanker wählen. |
| Umschalt+Klick in der Szene | Objekt zur Auswahl hinzufügen / entfernen. |
| Ziehen auf freier Szenenfläche | Auswahlrechteck; Umschalt ergänzt. Escape, Pointerabbruch oder Fokusverlust stellen die vorige Auswahl wieder her. |
| Rechtsklick auf bereits ausgewähltes Element | Gesamte Mehrfachauswahl erhalten. |
| Rechtsklick auf anderes Element | Dieses Element auswählen, dann dessen Menü öffnen. |
| Umschalt+F10 oder Menütaste | Dasselbe Menü für den fokussierten Auswahlbereich. |
| Pfeil hoch/runter, Pos1/Ende, Enter | Menüpunkt wählen / ausführen; deaktivierte Punkte überspringen. |
| Escape im Menü | Nur das Menü schließen, Auswahl erhalten und Fokus zurückgeben. |
| Escape im Arbeitsinhalt | Auswahl aufheben; während Ziehen stattdessen den vorherigen Stand wiederherstellen. |
| Klick außerhalb, Scrollen, Resize, Fensterfokusverlust | Menü schließen, keine Aktion ausführen. |
| Strg/⌘+A im Inhalt | Gesamten jeweiligen Inhalt wählen; im Texteditor nur Quelltext, nicht die Galerie. |

Fokus und Auswahl sind verschieden: Tastaturfokus verwendet eine gestrichelte
Linie, ausgewählte Zeilen zusätzlich ein Häkchen. Hover allein wählt nichts aus.
Das Menü bleibt innerhalb des Fensters. Hoher Kontrast nutzt Systemfarben und
Konturen; keine Transparenz oder Animation ist für die Aufgabe erforderlich.

## Kontextabhängige Aktionen und Grenzen

- **Text & Code:** Kopieren, Auswahl als Suchanfrage in der Beispiel-Seitenleiste,
  alles auswählen. Ausschneiden ist in dieser Nur-Lese-Studie sichtbar deaktiviert.
  Die Datenübergabe an die Suchanzeige funktioniert; es wird keine vollständige
  native Suche behauptet. Browserkopie verwendet die echte Clipboard-API und zeigt
  bei verweigerter Erlaubnis einen Hinweis. Die Prüfung provoziert diesen Fehler;
  sie bestätigt kein Einfügen in eine externe Anwendung.
- **Protokoll / Dateien:** Meldungen bzw. Dateinamen mit Herkunft kopieren,
  Details/Vorschau für die Auswahl anzeigen, alles auswählen, Auswahl aufheben.
  Die Dateien existieren hier nur als Beispieldaten; kein Dateiöffnen, Löschen,
  Umbenennen oder Zugriff auf Nutzerdateien.
- **Szene:** Eigenschaften anzeigen, Auswahl isolieren / alle wieder anzeigen,
  Auswahl aufheben. Die Griffe zeigen die Markierung, sind in dieser Studie
  keine implementierten Transformationswerkzeuge. Gewichtsangabe und Pfeil sind
  ausdrücklich eine feste Pendelillustration, keine generische Kraftsemantik
  und keine laufende Simulation.

Der bestehende Grafikexport erhält keine neue Menügestaltung und keine geänderten
Standardeinstellungen. Ein künftiges Kontextmenü an einem Berichtsdiagramm kann
die vorhandene Aktion „Grafik exportieren…“ zusätzlich aufrufen; der freigegebene
sichtbare Zugang am Diagramm und derselbe Exportdialog bleiben bestehen.

## Übergabe für die native Umsetzung

1. Nutzerregel „Linksauswahl ohne automatische Aktionen, Kontextmenü auf
   Rechtsklick / Tastaturmenü“ erhalten. **A2 ist final freigegeben** und anhand
   von [APPROVED.md](APPROVED.md) nativ übertragen. Die frühere Grundlage A ist
   ersetzt; keine neue Bestätigung oder Variantenentscheidung vorschalten.
2. Gemeinsame Auswahl-Hintergrund- und Schriftrollen im vorhandenen Renderer
   verwenden. Glyphen nach der Auswahlfläche zeichnen und getrennt clippen;
   keine pauschale UI-Opacity, kein dunkler Schleier über Text, Icons oder Labels.
3. Ursprüngliche Zeichenfolgen, UTF-8-Grenzen, Zeilenumbrüche, Auswahlanker,
   Mehrfachauswahl und den nativen Clipboard-/Editorvertrag erhalten. Kopieren
   darf keine Zeilennummern, Häkchen oder Auszeichnung enthalten.
4. Ziel und Auswahl beim Menüöffnen festhalten. Bei Projekt-/Datei-/Laufwechsel
   oder entfernten Objekten Menü verwerfen; Aktionen nie auf neue Ziele umleiten.
   Rechtsklick auf ausgewählte Zeilen erhält die Menge; auf andere Zeilen ersetzt
   er sie vor dem Öffnen. Abbruch ist keine Aktion.
5. Reale Kontextfähigkeiten entscheiden über Befehle: Editierbarkeit,
   Dateirechte, Quellstellen und exportierbare Daten prüfen. Keine Dateiaktion
   allein aufgrund der HTML-Beispieldaten hinzufügen. Bestehende Fehler-/
   Berechtigungs- und Exportverträge erhalten.
6. Native Text-/Objektauswahl, schnelle Pointerereignisse, Menüfokus, Auswahl
   während Menübedienung, Screenreader, kleiner Bildschirm, große Schrift,
   Hell/Dunkel/hoher Kontrast und tatsächlich verfügbare Plattformen prüfen.
   Browser-Range, DOM und JavaScript sind keine native Implementierungsgrundlage.

Quellen: `index.html`, `selection.css`, `selection.js` in diesem Verzeichnis.
Prüfer: [check_selection_options.cjs](../../../tools/check_selection_options.cjs).
Ausgeführte Prüfungen und Bildreferenzen: [review/VALIDATION.md](review/VALIDATION.md).
Der zusätzliche A2-Nachweis steht in [review/a2/VALIDATION.md](review/a2/VALIDATION.md).
