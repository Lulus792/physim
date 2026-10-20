# Gemeinsamer Resizing-Vertrag

Verbindliche Ergänzung vom 20. Oktober 2026: Auswerten erhält Größenverstellung;
Simulieren verwendet dieselbe Gestaltung und Bedienung wie Entwickeln.
Dies gilt für die gemeinsame Vorschau und die spätere native Umsetzung.

## Eine Interaktion für alle Bereiche

Die Referenz liegt in [resize.js](../shared/resize.js) und
[resize.css](../shared/resize.css). Entwickeln nutzt diesen gemeinsamen Baustein
ebenfalls. Bereichsspezifische Adapter bestimmen ausschließlich Geometrie,
Mindestgrößen, gespeicherte Wunschgrößen und die Richtung der Größenänderung.

| Eingabe | Verhalten |
| --- | --- |
| Maus/Pointer | Trennlinie ziehen, auch außerhalb des ursprünglichen Treffers; Capture bis zum Loslassen |
| Pfeile entlang der Achse | Grenze um 10 logische Pixel bewegen |
| Umschalt + Pfeil | 40 logische Pixel |
| Pos1 / Home | Kleinste zulässige Größe |
| Ende / End | Größte aktuell zulässige Größe |
| Enter oder Doppelklick | Bereichsspezifische Standardgröße, innerhalb der aktuellen Grenzen |
| Escape während Ziehen | Exakten Zustand vor dem Ziehen wiederherstellen |
| Pointercancel, verlorenes Capture, Fensterfokusverlust oder Fensterresize während Ziehen | Vorgang abbrechen und Ausgangswunschgrößen erhalten |

Bei rechten und unteren Panels bedeutet eine Bewegung nach links/oben eine
Vergrößerung. Die Kurventrennung verändert die tatsächliche Breite der ersten
Kurve um 10/40 Pixel; intern kann daraus ein Verhältnis abgeleitet werden.
Eine Tastaturänderung allein ist kein noch laufender Pointerdrag.

Die Trefffläche ist neun logische Pixel breit/hoch. Im Ruhezustand bleibt die
vorhandene ruhige Panelgrenze; bei Hover oder Fokus erscheint eine durchgehende,
ein Pixel starke Akzentlinie. Sichtbarer Fokus, passender Resize-Cursor und
Separatorrolle mit Name, Orientierung, Wert, Minimum, Maximum und Zielbereich
gehören dazu. Die früheren kurzen Griff-Pillen in Simulieren entfallen.

## Bereiche und Referenzgrößen

| Bereich | Grenze | Standard in der Browserreferenz | Grenzen/Erhalt |
| --- | --- | --- | --- |
| Entwickeln | Workspace links | 218 px, gestapelt 230 px hoch | 140–480 px; Mitte mindestens 300 px |
| Entwickeln | Codekontext rechts | 255 px, gestapelt 320 px hoch | 180–480 px; Mitte mindestens 300 px |
| Entwickeln | Protokoll unten | 86 px | 60–300 px; verbleibender Editor mindestens 280 px |
| Simulieren | Szenenbaum | 190 px | 150–420 px, abhängig von Mitte/Inspektor |
| Simulieren | Inspektor | 320 px | 210–540 px, mindestens 230 px für die Mitte beim Ziehen |
| Simulieren | Szene / Messbereich | 150 px geschlossen, 330 px mit Kurven | Messbereich mindestens 130 px, Szene mindestens 180 px; getrennte Wunschhöhen für offen/zu |
| Simulieren | Zeitleiste | 120 px | 95–280 px, übrige Arbeitsfläche bleibt nutzbar |
| Simulieren | Kurve 1 / Kurve 2 | 50 % | 25–75 %; Pixelbewegung wird in das tatsächliche Verhältnis umgerechnet |
| Auswerten | Laufliste | 232 px | 140–480 px; Mitte mindestens 300 px |
| Auswerten | Details | 242 px | 180–480 px; Mitte mindestens 300 px |
| Auswerten | Messkurve / Statistik | automatisch nach Breite | nach manueller Änderung 180–700 px, abhängig von Fensterhöhe; Enter stellt automatische Höhe wieder her |

Die Simulation besitzt außerhalb der gemeinsamen Shell zusätzlich die bisherige
verstellbare Gesamtvorschauhöhe (450–1100 px, Standard 660 px). Das ist eine
Präsentationsgrenze des einzelnen Entwurfs, kein zusätzlicher nativer App-Bereich.
In der gemeinsamen Shell bestimmt das Fenster die Gesamthöhe.

Schmale Fenster verwenden die jeweiligen bestehenden Stapel-/Einblenderegeln.
Unsichtbare oder bildschirmfüllende mobile Seiten haben keine leeren,
fokussierbaren Seitentrenner. In Entwickeln wechselt derselbe Trenner im
gestapelten Layout von Breiten- zu Höhenverstellung. Gespeicherte Wunschgrößen
werden beim bloßen Verkleinern nicht überschrieben; Wiedervergrößern und
Wiederöffnen stellen sie innerhalb der aktuellen Grenzen her.

In der nativen App gehen UI-Skalierung, tatsächliche Mindestgrößen und vorhandene
persönliche Docklayouts vor den Browser-Demomaßen. Unverbindlich sind feste
physische Pixelzahlen; verbindlich sind Verhalten, erkennbare Zuordnung,
Abbruch, Erhalt der Inhalte und erreichbare Hauptfläche.

## Fachliche Invarianten

Resizing verändert weder Quelltext, Dirty/Undo noch Laufmodell, Simulation,
betrachteten Zeitpunkt, Kamera, Kanalauswahl, Anzeigeeinheiten, Statistik oder
Berichtsherkunft. Die tatsächlich veränderten Rechtecke erreichen Szene und
Plotrenderer. Die Auswertung passt den Diagramm-ViewBox an die neue Höhe an,
ohne Schrift vertikal zu verzerren oder neue Messwerte zu berechnen.

Eigene Layoutzustände bleiben pro Arbeitsbereich erhalten. Verdeckte Seiten
behalten ihre Wunschgröße. Ein Rücksetzen betrifft nur die aktivierte Grenze;
beim Szene-/Messbereichsreset werden beide dazugehörigen offen/zu-Wunschhöhen
zurückgesetzt. Es wird kein neues Docking-Dateiformat vorausgesetzt.

## Ausgeführte Abnahme

`tools/check_workspace_resizing.cjs` prüft echte Maus-/Tastaturaktionen an allen
elf sichtbaren Trennern der gemeinsamen Vorschau, 10/40-Schritte, Grenzen,
Enter/Doppelklick und Escape-Abbruch. Zusätzlich: Erhalt von Quelltext,
betrachteter Zeit, Statistik und Wunschbreite beim Bereichswechsel und
Aus-/Einblenden sowie 24 Layout-/Themefälle ohne horizontales Überlaufen.
Der sechste Simulationstrenner der Einzelvorlage wird vom aktualisierten
`tools/check_inspector_variants.cjs` mitgeprüft. Neue native Prüfungen sind damit
nicht nachgewiesen. [Prüfbericht](resizing-validation.json).
