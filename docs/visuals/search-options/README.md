# D08: Editor-Suche und Ersetzen · ergänzende Studie

Stand: **30. Oktober 2026 · bestehende Suchinsel bleibt, keine erneute Wahl nötig.**
Der Nutzer hat darauf hingewiesen, dass die Suche bereits gestaltet wurde.
Der D08-Eintrag in der [UI-Liste](../unified-workspace/UI-DESIGN-BACKLOG.md)
war als offene Detailausarbeitung formuliert; daraus wurde hier unnötig eine neue
Anordnungsrunde abgeleitet. Diese A/B/C-Zusatzstudie bleibt getrennt und **nicht
freigegeben**. Sie ersetzt die vorhandene Suchinsel nicht und ist keine offene
Pflichtentscheidung. D09 ist der nächste tatsächlich offene Designpunkt.

[Vorschau öffnen](index.html?variant=a&scenario=matches&theme=dark&replace=1) ·
[A · Suchinsel](index.html?variant=a&replace=1) ·
[B · Dokumentleiste](index.html?variant=b&replace=1) ·
[C · Trefferbereich](index.html?variant=c&replace=1).
Lokaler Server: `http://127.0.0.1:8791/search-options/`.
Ohne Netzwerk auch als lokale Datei verwendbar; keine externen Assets/Fonts.

## Drei Anordnungen, dieselbe Aufgabe

| Variante | Gestaltung | Unterschied |
| --- | --- | --- |
| A · Suchinsel | Kleine kühle Suchfläche rechts über dem Code, aufklappbare Ersetzung | Greift die vorhandene Suchinsel auf. Auf kleinen Breiten in den Dokumentfluss versetzt; Code und Aktionen bleiben erreichbar. |
| B · Dokumentleiste | Fest unter den Datei-Tabs, vor dem Code | Suchfelder und Aktionen haben einen festen Platz. Keine Codeüberdeckung; Ersetzen erweitert die Leiste. |
| C · Trefferbereich | Rechts neben dem Code, Fundstellen mit Zeilenkontext | Einzelne Fundstellen direkt anspringen. Bei wenig Breite oberhalb des Codes; Ergebnisliste bleibt scrollbar. |

Die vorhandene Suchinsel ist eine bestehende Referenz. B/C sind getrennte
Vergleichsvorschläge für diese Detailrunde und heben keinen bestehenden Vertrag
auf. A2, Kontextmenüs A, B2, alle gewählten Bereiche, Export und Fortschritt B
mit Icon-only-Pop-out bleiben verbindlich. Diese Studie integriert kein neues
Menü in die Browser-Textfläche; die native Menüfreigabe gilt weiterhin.

Alle Varianten haben dieselbe Such-/Ersetzungslogik. Varianten- und Themewechsel
erhalten Beispieltext, ungespeicherte Änderungen, Suchtext, Ersetzung und Auswahl.
Szenariowechsel/Zurücksetzen lädt ausdrücklich neue Fixtures.

## Am nativen Bestand geprüft

- [editor_search.h](../../../app/editor_search.h): wörtliche, groß-/kleinsensitive,
  nicht überlappende Suche. UTF-8-Bytepositionen, Vorwärts/Rückwärts und Wrap.
  Suchpuffer-Kapazität 128 einschließlich Abschlussbyte.
- [editor_search_ui.inc](../../../app/editor_search_ui.inc): aktuelle Datei,
  Strg/⌘+F, Enter/Umschalt+Enter, Escape, aufklappbare Ersetzung, Zähler,
  Vorher/Nachher und kühle Suchinsel mit Material-Fallback.
- [main.c](../../../app/main.c), `editor_search_target`, `editor_search_open`,
  `find_text_direction`: Experiment-/Analyseeditor und bearbeitbare Dokumente;
  Wiederherstellungsdokumente sind kein Suchziel. Ersetzen prüft zuerst die exakt
  passende Auswahl, sucht sonst ab Cursor und ersetzt einen Treffer. Änderungen
  markieren den richtigen Editor als geändert und invalidieren nötigenfalls Builds.
- [editor_replace_tests.inc](../../../app/editor_replace_tests.inc): bestehende
  Unicode-Ersetzung, leere Ersetzung und Undo für verschiedene native Editorziele.
- [Suchinsel-Referenz](../develop-designs/search-resize.js) und
  [A2](../selection-options/APPROVED.md) für bestehende Gestaltung/Bedienung.

Diese Fundstellen belegen die genannten bestehenden Funktionen. Die Browserstudie
ist kein Nachweis, dass die neuen Anordnungen bereits nativ implementiert sind.

## Umfang und Erweiterungen

| Funktion | In der Studie | Native Einordnung |
| --- | --- | --- |
| Aktuelle Datei durchsuchen, vor/zurück, Wrap, Zähler | Echt am Beispieltext | Vorhandene Grundlage erhalten. |
| Einzelnen Treffer ersetzen / durch leeren Text löschen | Echt am Beispieltext, rückgängig | Vorhandene Funktion; UTF-8, Dirty-/Undo-/Build-Regeln erhalten. |
| Drei Datei-Tabs mit getrennten Entwürfen | Jede Datei separat, Wechsel durchsucht nur diese | Bestehende Zielermittlung verwenden. Keine globale Änderung anderer Dateien. |
| Alle ersetzen | **Als Entwurfserweiterung gekennzeichnet**, Prüfung mit Datei/Anzahl, ein Undo-Schritt | In den gelesenen Suchfunktionen nicht vorhanden. Erst nach Designwahl gezielt nativ ergänzen und testen; kein vorgetäuschter bestehender Befehl. |
| Trefferhintergründe / aktive Kontur / C-Liste | Aus Originaltext und dessen Bereichen berechnet | Zusätzliche Darstellung; denselben Suchbestand/Originaltext verwenden. |
| Regex, Groß/Klein-Umschalter, Ganzwort, Auswahl-Scope | Nicht angeboten | Keine unbelegten Suchmodi einführen. |
| Projektweite Suche / Ersetzung | Nicht angeboten | Bleibt eigene mögliche Erweiterung I03. Kein Dateisystemscan durch diese Studie. |

Suche findet auch Teilzeichenfolgen in Bezeichnern: `damping` kommt im normalen
Beispiel **fünfmal** vor, einschließlich `pendulum_set_damping`. Kein impliziter
Ganzwortfilter. Leere Suche hat null Treffer und verändert nichts.
Suchtext/Ersetzung sind auf 127 UTF-8-Bytes validiert; Überlänge wird sichtbar
gemeldet, niemals still abgeschnitten. JavaScript verwendet UTF-16-Bereiche
der Textarea. Nativ Byte-/Rune-/Glyphpositionen ausdrücklich konvertieren.

## Interaktive Abläufe

1. Suchtext ändern, vorwärts/rückwärts navigieren; Enter/Umschalt+Enter in den
   beiden Feldern navigiert. Wrap wird benannt. Die Suche bleibt offen und
   verlagert den Fokus nicht bei jedem Treffer in den Code.
2. Ersetzen aufklappen. Einzelersetzung verwendet die exakt passende Auswahl,
   andernfalls den nächsten Treffer ab Cursor. Leere Ersetzung löscht den Treffer.
   Rückgängig stellt Text und Dirty-Stand wieder her; kein automatisches Speichern.
3. „Alle ersetzen…“ zeigt Datei, Anzahl, Suchtext/Ersetzung und Löschwirkung bei
   leerem Ersatz. Abbrechen/Escape ändern nichts. Bestätigung ändert nur diese
   Datei und lässt sich gemeinsam rückgängig machen. Ersetzung mit gleichem
   Suchtext läuft einmal über den vorherigen Stand, keine Endlosschleife.
4. Suchtext, Ersetzung, Dokumenttext oder Datei ändern: alte Prüfung verwerfen.
   Vor Ausführung Datei/Revision/Abfragen/Rechte erneut prüfen. Eine alte Aktion
   darf nicht in die inzwischen aktive Datei schreiben.
5. Tabwechsel bewahrt andere ungespeicherte Beispielentwürfe. Tabpfeile/Pos1/Ende
   bedienen die Tablist; keine projektweite Suche. Keine Treffer oder leere Suche
   ersetzt/selektiert nichts. Nur lesen erlaubt Navigation, sperrt Änderungen.
6. Escape schließt zuerst die Ersetzungsprüfung, dann die Suche mit Fokus zum
   Code. Auswahl bleibt erhalten. Strg/⌘+F oder Suchsymbol öffnet und fokussiert
   den Suchtext. Tab/Shift+Tab verlassen die Suche ohne Fokusfalle.
7. C-Fundstellen wählen den exakten Bereich und geben Fokus zum Code. Ausschnitte
   können gekürzt sein; Quelle, Bereich und Ersetzungen verwenden den Originaltext.
8. Lade-/Quellenfehler sind ausdrücklich simulierte Zustände. Navigation und
   Ersetzen gesperrt, vorhandener Text bleibt. Retry bzw. Ladeabschluss ist eine
   Fixture-Aktion; Abbruch schließt ohne Datenverlust. Keine echte Datei-I/O.

Es gibt weder Systemclipboard-Zugriff noch Persistenz/Local Storage, native
Speicherung oder einen Worker. Neuladen setzt zurück. Der Rückgängig-Button
arbeitet auf der Fixture-Historie; Strg/⌘+Z im Code verwendet dieselbe Historie.
Die Suchfelder behalten ihre browserübliche eigene Texteingabe-Historie.

## Darstellung und Accessibility

Werkbankfarben und A2 kommen aus dem bestehenden Design. Kein dritter Auswahlstil:
Dunkel `rgba(170,184,202,.15)`, Hell `rgba(90,108,130,.11)`, Text volle Deckkraft.
Suchtreffer: Bernsteinfläche `.14` dunkel / `.12` hell; aktiver Treffer zusätzlich
Kontur `#d6b577` / `#805509`. Treffer sind nicht automatisch alle Textauswahl.
Die Legende benennt den Unterschied, Zähler/aktive Listenzeile ergänzen Farbe.

Ein `aria-hidden`-Textspiegel zeichnet nur Hintergründe/Konturen hinter den
originalen Textarea-Glyphen. Bei Suchfeldfokus erhält er den A2-Hintergrund des
wirklich ausgewählten Bereichs. Keine doppelte lesbare Textquelle. Nativ den
vorhandenen Editorrenderer nutzen; kein WebView-/Textarea-Vertrag.

Felder/Icons haben Namen, Disclosure seinen Zustand, Tabs Auswahl/Fokusfolge,
C-Fundstellen Zeile/Position, Status eine höfliche Live-Region. Suche ist kein
modaler Dialog. Große Schrift, schmale Fenster, Systemkontrast, reduzierte
Bewegung und deckender Transparenz-Fallback werden geprüft. CSS-Geometrie und
vollständige Tokens stehen in [search.css](search.css).

## Bestandsabgleich und verbleibende Detailarbeit

**Vorhandene Suchinsel weiterführen, keine erneute Variantenwahl vorschalten.**
Die Zusatzanordnungen und Erweiterungen nicht als neue Freigabe behandeln.
Vorhandene native Suche, separate Aufgabenfenster und alle Freigaben gemäß
[Gesamtauftrag](../unified-workspace/NEXT-CHAT.md) erhalten. Diese Studie kann
später ausdrücklich beauftragte Detailarbeit illustrieren; sie autorisiert keine
neue Ersetzungsfunktion oder andere Anordnung.

Für echte Integration: Dokument-/Projekt-/Fensteridentitäten, Revisionen und
Rechte gemeinsam mit Trefferbereichen festhalten. Edit-/Query-/Tabwechsel
invalidiert Treffer/Prüfung. Undo-/Redo-/Dirty-/Build-Verträge, leere und längere
Unicode-Ersetzungen, Quellenfehler und B2 beim späteren Speichern/Schließen mit
echten Daten prüfen. „Alle ersetzen“ benötigt eine gemeinsame Undo-Transaktion;
keine Schleife mit wechselndem Quellstand oder teilweisem stillen Speichern.

D09 „Vervollständigung und Hilfe direkt am Code“ ist die nächste neue Runde.

## Prüfung

[Prüfbericht](review/VALIDATION.md), [Maschinenbericht](review/validation.json),
[Quellhashes](review/source-hashes.json).
`node tools/check_search_options.cjs` mit Playwright/Edge; alternativ
`PHYSIM_BROWSER`. Rohdaten unter `D:/Physim/build/search-options-review`.
Browsernachweise sind keine native Windows-/Linux-/macOS- oder reale
Screenreaderabnahme. Die native Integration bleibt bis zur Auswahl getrennt.
