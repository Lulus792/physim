# Werkbank mit Suchinsel

Festgelegt: **Panelköpfe**, **kompakte Tabs**, **Workspace-Pfadlinien** und die
**schwebende Suchinsel**. Verworfenes wurde aus diesem Ordner entfernt.
Nur die Glasoberfläche steht noch zum Vergleich.

- [Interaktive Vorschau](index.html), lokal ohne Server oder externe Ressourcen.
- [Glasvergleich](glass-overview.png): Detailansichten, dunkel links / hell rechts.
- Seidenglas: [Dunkel](glass-satin-dark.png), [Hell](glass-satin-light.png).
- Klarglas: [Dunkel](glass-clear-dark.png), [Hell](glass-clear-light.png).
- Kristallkante: [Dunkel](glass-crystal-dark.png), [Hell](glass-crystal-light.png).
- [Designvertrag, Prüfung und Bereinigung](DESIGN.md).

Die Suchinsel sitzt 4 px unter der Oberkante der Codefläche; die separate
Pfadzeile entfällt. Sie verschiebt den Code auch mit geöffnetem Ersetzen nicht.
Für den Materialvergleich ist die Suche beim Start offen. Escape schließt sie;
Ctrl/Command+F oder das Suchicon öffnet sie wieder. Die Materialknöpfe öffnen
sie ebenfalls. „Transparenz reduzieren“ zeigt eine deckende Fläche.

Workspace, Kontext und Protokoll bleiben an ihren Trennlinien größenverstellbar,
auch per Tab und Pfeiltasten. Doppelklick oder Enter setzt eine Größe zurück.
Bei schmalen Fenstern verändern die gestapelten Nebenbereiche ihre Höhe.

Die native Anwendung wurde nicht verändert. Dateiinhalte, Speichern und Build
sind lokale Demoabläufe; Änderungen gehen beim Neuladen verloren.

## Prüfung

Playwright und Sharp über die gebündelten Laufzeitpfade verwenden; `NODE_PATH`
auf deren Modulordner setzen. Ein konsolidierter Prüfer ersetzt die alten Studienprüfer:

```text
node tools/check_develop_designs.cjs
node tools/check_develop_designs.cjs --export
```

Mit `--export` werden die sieben aktuellen Glasreferenzen hier erneuert.
Andere Prüfausgaben liegen unter `build/develop-design-review/` und werden nach
Abnahme entfernt. Windows verwendet Edge; `PHYSIM_BROWSER` kann einen anderen
Chromium-Browser angeben. Linux-/macOS-Ausführung wird damit nicht behauptet.
