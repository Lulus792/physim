# F1: ausgewähltes finales Design H

Der Nutzer hat H am 16. Oktober 2026 als Zielbild ausgewählt. Die helle und dunkle
Variante sind fertig, beide Seitenleisten schalten unabhängig. Die native App
wird in einem späteren Chat umgestellt.

- [Interaktive Vorlage](index.html): standardmäßig Dunkel, Hell umschaltbar.
- [Hell](h-light.png) · [Dunkel](h-dark.png) · [Vergleich und Seitenzustände](h-final-overview.png).
- [Verbindliche Designspezifikation](DESIGN-H.md).
- **[Kopierfertiger Auftrag für den anderen Chat](NEXT-CHAT.md)**.
- [Projektentscheidung ADR 007](../../decisions/007-f1-layout-h.md).

Die Vorlage nutzt lokale Dateien und funktioniert ohne Installation oder Server.
Ein bereits laufender lokaler Vorschau-Server kann ebenfalls verwendet werden.
Über die Größenwahl lassen sich 1080/760 px und 16/32 px Lesetext ausprobieren.
Die äußere Galerie gehört nicht zur vorgeschlagenen F1-Appoberfläche.

## Fehlerkorrektur

Die gemeldete Kopplung beim Schließen links war reproduzierbar: Durch automatische
Rasterplatzierung bekam der Artikel null Breite; der rechte Bereich rückte in
die falsche Spalte. Navigation, Artikel und Kontext sind jetzt ausdrücklich den
Spalten 1, 2 und 3 zugeordnet. Die linke Aktion ändert nur die linke Sichtbarkeit,
die rechte nur die rechte. Diese geometrische Eigenschaft wird geprüft.

Der rechte Modell-/Beispiel-Tab, globale C-/Physim-Wahl, Suchumfang und Lesestelle
bleiben erhalten. Themewechsel ändern Farben, keine Seitenzustände. Die Farben
leiten sich aus den vorhandenen Rollen in `app/design_tokens.h` ab.

## Prüfer erneut ausführen

`tools/check_f1_design_h.cjs` verwendet Playwright und Sharp. Über das Desktoptool
`load_workspace_dependencies` die gebündelten Node-/Modulpfade ermitteln, den
Modulpfad als `NODE_PATH` setzen und mit dem gebündelten Node ausführen:

```text
node tools/check_f1_design_h.cjs
node tools/check_f1_design_h.cjs --export
```

Ohne `--export` bleiben Ausgaben unter `build/f1-h-review/`. Mit `--export` werden
zusätzlich die acht gelieferten Hell-/Dunkel-Zustandsbilder und die Übersicht im
Designordner erneuert. Unter Windows wird der installierte Edge genutzt; optional
wählt `PHYSIM_BROWSER` einen konkreten Browserpfad. Unter anderen Betriebssystemen
muss ein Playwright-Chromium verfügbar sein. Diese Anleitung ist keine Behauptung,
dass die Prüfung dort bereits ausgeführt wurde.

Ausgeführt unter Windows mit Edge 155.0.4283.45: 32 Layoutzustände sowie
Geometrie-/Sprach-/Theme-/Tastatur-/Suchfälle bestanden; keine JavaScriptfehler.
Zehn berechnete Text-/Flächenkontraste: 6,30:1 bis 13,55:1. Bilder visuell geprüft.
Es gibt noch keine native Implementierungs-, Screenreader-, Monitorwechsel- oder
neue Linux-/macOS-Abnahme. Details und die spätere Abnahme stehen in DESIGN-H.md.

## Quellen und Archive

Aktiv: `index.html`, `final-h.css`, `final-h.js`, `refinements.css`,
`refinements.js`, `layouts.css`, `content.js`, `model.svg`.

[Frühere Runde 3](round3.html) mit H/I/J und [Runde 2](round2.html) mit E/F/G/A/C/D
bleiben als Archiv erhalten. Maßgeblich für die neue Umsetzung sind ausschließlich
DESIGN-H.md, die finalen H-Bilder und ADR 007. Die alten Beispielgalerien sind
keine zusätzlichen Produktanforderungen.
