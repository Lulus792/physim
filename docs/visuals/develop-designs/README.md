# Werkbank mit Suchinsel

Festgelegt: **Panelköpfe**, **kompakte Tabs**, **Workspace-Pfadlinien** und die
**schwebende Suchinsel**. Verworfenes wurde aus diesem Ordner entfernt.
**Klarglas mit der 16-px-Rundung von Kristallkante ist gewählt.** Zum Vergleich
stehen nur noch drei dezente Farbabstimmungen von Glas, Iconflächen und Eingabefeld.

- [Interaktive Vorschau](index.html), lokal ohne Server oder externe Ressourcen.
- [Glasvergleich](glass-overview.png): Detailansichten, dunkel links / hell rechts.
- Neutral: [Dunkel](glass-neutral-dark.png), [Hell](glass-neutral-light.png).
- Kühl: [Dunkel](glass-cool-dark.png), [Hell](glass-cool-light.png).
- Warm: [Dunkel](glass-warm-dark.png), [Hell](glass-warm-light.png).
- [Designvertrag, Prüfung und Bereinigung](DESIGN.md).

Die Suchinsel sitzt 4 px unter der Oberkante der Codefläche; die separate
Pfadzeile entfällt. Sie verschiebt den Code auch mit geöffnetem Ersetzen nicht.
Für den Materialvergleich ist die Suche beim Start offen. Escape schließt sie;
Ctrl/Command+F oder das Suchicon öffnet sie wieder. Die Materialknöpfe öffnen
sie ebenfalls. „Transparenz reduzieren“ zeigt eine deckende Fläche.

Neue SVG-Suchicons ersetzen die Schriftzeichen. Die Treffernavigation teilt
eine gemeinsame, ruhige Iconfläche. Der rechte Kontextschalter bleibt beim
Schließen und Öffnen ganz rechts an derselben Position, stets rechts von Suche
und Speichern. Die Farbauswahl verändert diese Anordnung nicht.

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
