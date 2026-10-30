# Kontextmenüs A · verbindliches Design

**Auswahl vom 30. Oktober 2026:** Der Nutzer hat **A · Kompakt** gewählt.
Die getrennte D07-Auswahlphase ist abgeschlossen; A nativ mit umsetzen.
B/C bleiben Vergleichshistorie. [ADR 021](../../decisions/021-selected-context-menu-a.md)
und [Gesamtauftrag](../unified-workspace/NEXT-CHAT.md) verankern die Entscheidung.

[Freigegebene Referenz](index.html?approved=1&context=text&theme=dark).
Standardaufrufe und `approved=1` verwenden immer A, auch mit altem `variant=b/c`.
Nur `approved=0` öffnet den historischen A/B/C-Vergleich. Die Galerie und ihre
Prüfschalter gehören nicht in die native Oberfläche.

## Gestaltung

Ruhige kompakte Liste mit Trennlinien zwischen Befehlsgruppen. Keine sichtbare
Zielüberschrift oder Gruppenbeschriftung aus B und keine Beschreibung je Befehl
aus C. Tastenkürzel stehen rechts, Untermenüs haben einen Richtungshinweis.
Globale Szenenraster-Befehle verwenden Checkboxen mit wirklichem Zustand.
Bekannte, situationsabhängig gesperrte Befehle bleiben sichtbar und nennen den
Grund; fehlende Produktfunktionen erhalten keinen scheinbar aktiven Eintrag.

Werte aus [menus.css](menus.css), in logischen UI-Einheiten nativ übertragen:

| Eigenschaft | Wert |
| --- | --- |
| Standardbreite | 260; höchstens Fensterbreite minus 16 |
| Abstand zum Fensterrand | mindestens 8, Position an verfügbare Fläche anpassen |
| Menü | Radius 9, Rahmen 1, Innenabstand 6, Schatten gemäß Theme |
| Befehlszeile | Mindesthöhe 34, Innenabstand 7 vertikal / 10 horizontal, Abstand 10 |
| Zeilenradius | 4 |
| Trennlinie | Höhe 1, Abstand 5 vertikal / 4 horizontal |
| Schrift | Befehl .8rem, Tastaturhinweis .68rem; nicht auf feste Pixelhöhe beschneiden |
| Höhe | höchstens Fensterhöhe minus 16; Menü selbst scrollbar |

| Farbrolle | Dunkel | Hell |
| --- | --- | --- |
| Menüfläche | `#25262b` | `#edeff3` |
| Rahmen / Trennlinie | `#3b3d45` | `#bec3ce` |
| Text | `#edeef2` | `#20242c` |
| Sekundärtext | `#a6a9b1` | `#555e6c` |
| A2-Fläche / Zeilenfeedback | `rgba(170,184,202,.15)` | `rgba(90,108,130,.11)` |
| A2-Kante | `#abb9ca` | `#65788d` |
| Fokus | `#8abaff` | `#17569e` |

Hoher Kontrast verwendet Schwarz/Weiß und `#fff44f`, bei System-Kontrastfarben
die entsprechenden Systemrollen. Zustände bleiben auch ohne Farbe erkennbar.
Die vorhandenen [A2-Verträge](../selection-options/APPROVED.md) gelten unverändert:
Text mit voller Deckkraft und ursprünglicher Farbe; Listen mit Kante/Häkchen,
Objekte mit dezenten Eckpunkten. Diese Punkte sind keine Transformationsgriffe.

## Befehle und echte Funktionen

Die [vollständige Neun-Ziele-Matrix](README.md#vollständige-befehlsmatrix-dieser-studie)
ist Teil dieser verbindlichen Spezifikation: Text/Code, Datei-Tab, Datei,
Ordner, Protokollmeldung, Diagramm, Szenenobjekt, Mehrfachauswahl und freie Fläche.
Text-/Pfadkopie kopiert Text und ist kein Dateikopier- oder Verschiebebefehl.
Keine neuen Lösch-, Umbenenn-, Transformations-, Split-Editor- oder Exportfunktionen
allein aus der Gestaltung ableiten.

Native Operationen an vorhandene Daten und Adapter binden:

- Rechts angeklickten Tab speichern/schließen, nicht versehentlich den aktiven
  anderen Tab. Ungespeicherte Inhalte gehen durch [B2](../file-safety-options/APPROVED.md).
- Echte Clipboard-/UTF-8-/Undo-Verträge verwenden. Protokoll-Originaltext
  einschließlich Zeilenenden unverändert kopieren; Quellenzugang nur mit
  eindeutiger gültiger Quellstelle. Vorhandene schreibgeschützte Zustände erhalten.
- Grafikexport erhält denselben Diagramm-/Laufstand und den
  [freigegebenen Export](../unified-workspace/EXPORT-EXAMPLE.md). Projektanlage
  verwendet B, Ordnerzugang Explorer A. Bestehende ADR-015-Aufgabenfenster nutzen.
- Verfügbarkeit für Maus, Tastatur und Accessibility gemeinsam berechnen.
  Echte Fehler über Rückmeldungen A und Protokoll C melden; Erfolg erst nach
  tatsächlichem Ergebnis. Keine Fixture-Erfolgstexte als Implementierung übernehmen.

Die nativen Fundstellen und Integrationsgrenzen stehen im
[Quellenabschnitt](README.md#quellen-und-erforderliche-native-bindung).

## Öffnen, Auswahl und Schließen

1. Linksklick wählt bzw. positioniert den Cursor; keine automatische Aktionsleiste.
   Rechtsklick, Menütaste oder Umschalt+F10 öffnen am Ziel bzw. fokussierten Inhalt.
2. Rechtsklick innerhalb einer Mehrfachauswahl erhält deren Menge. Auf ein anderes
   Element wird zuerst dieses zum Ziel. Ctrl/⌘ und Umschalt behalten ihre
   Ergänzungs-/Bereichsauswahl. Textbereich und A2 bleiben bei geöffnetem Menü sichtbar.
3. Leere Fläche hat ihre eigene Befehlsmenge: Ordnerzugang/Projektanlage/Refresh
   im Explorer, globales Raster in der Szene, Auswahlbefehle im Protokoll.
   Keine versteckte Aktion auf ein zuvor gewähltes Element.
4. Auf/Ab, Pos1/Ende und Enter bedienen das Menü; gesperrte Punkte überspringen.
   Rechts/Enter öffnet Untermenüs, Links/Escape schließt zuerst das Untermenü
   mit Fokus zum Auslöser. Escape im Hauptmenü schließt und gibt Fokus zur Quelle.
   Tab verlässt/schließt das Menü, keine Fokusfalle. Untermenü bietet auch einen
   Maus-Zurückzugang, wenn es bei wenig Platz über dem Hauptmenü liegt.
5. Außenklick, tatsächliches Inhaltsscrollen, Größenänderung und Fenster-Fokusverlust
   schließen ohne Aktion. Menüinternes Scrollen bleibt möglich. Nach Öffnen nur
   verspätet zugestellte alte Scrollereignisse ohne Positionsänderung ignorieren.
6. Rechte Kamera-Ziehbewegung unterdrückt das Kontextmenü; Kamera-/Szenenbedienung
   nativ erhalten. Browser-Gestenerkennung ist keine implementierte 3D-Kamera.

Beim Öffnen stabile Ziel-/Projekt-/Dokument-/Lauf-/Fenster-/Versionsidentitäten
sowie Auswahl und Textbereich festhalten. Bei Zielverlust/Revision/Projektwechsel
verwerfen und vor Ausführung erneut prüfen. Veraltete Aktionen nicht auf die
inzwischen aktive Datei umleiten. Menü/Untermenü gehören zum richtigen Fenster.

## Native Abnahme

Echte Menü-, Untermenü-, Checkbox-, deaktiviert- und Fokusrollen publizieren.
Popup-Verdeckung, Untermenügrenzen, Fokus-Rückkehr, große Schrift, kleine Fenster,
Monitor-/DPI-Wechsel, Systemkontrast, reduzierte Bewegung und rechte Kamerageste
auf verfügbaren Plattformen prüfen. Die Browserstudie zeichnet nur den inaktiven
Textauswahlhintergrund über einen `aria-hidden`-Spiegel; nativ den vorhandenen
Text-/Auswahlrenderer verwenden, keine WebView oder zweite lesbare Textquelle.

[Browser-Freigabenachweis](review/APPROVAL-VALIDATION.md) und historische
[Vergleichsprüfung](review/VALIDATION.md) getrennt erhalten. Sie bestätigen keine
fertige native Umsetzung und keine reale Screenreader-/Linux-/macOS-Abnahme.
Alle vorherigen Freigaben einschließlich Fortschritt B mit Icon-only-Pop-out
bleiben erhalten. D08 „Editor-Suche und Ersetzen“ folgt als neue Entwurfsrunde.
