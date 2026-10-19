# Entwickeln: drei Designstudien

Erstellt am 19. Oktober 2026 auf Grundlage des lokalen Projektstands und der
[verbindlichen Designrichtung](../../design-direction.md). Es wurde noch keine
Variante zur nativen Umsetzung ausgewählt. Verantwortlich: UI/UX-Design.

- [Interaktive Vorschau](index.html): lokal ohne Server, Installation oder externe Ressourcen öffnen.
- [Alle drei Varianten im Vergleich](overview.png): Dunkel links, Hell rechts.
- **Werkbank**: [Dunkel](workbench-dark.png), [Hell](workbench-light.png).
- **API-Atelier**: [Dunkel](atelier-dark.png), [Hell](atelier-light.png).
- **Fokus**: [Dunkel](focus-dark.png), [Hell](focus-light.png).
- [Compilerfehler mit Sprung zur Codestelle](compiler-error.png).
- [Designvertrag, Zustände und Abnahme](DESIGN.md).

Die Galerie startet mit Werkbank und einem erfolgreichen Demo-Build. Designs,
Theme und Vorschauzustände lassen sich oben umschalten. Im App-Fenster sind
Dateien, Code, Suche/Ersetzen, Speichern, Panels und das Buildmenü bedienbar.
F5 startet einen Demo-Build; Ctrl/Command+F öffnet die Suche und Ctrl/Command+S
markiert die Demoänderungen als gespeichert. Beim Neuladen gehen sie verloren.

Die native Anwendung wurde nicht verändert. Build, Compiler, Dateisystem,
Projektmanager und Arbeitsbereichwechsel sind nicht angebunden. Der Code ist
ein gekürzter, illustrativer Ausschnitt des lokalen Pendelbeispiels. Die
API-Karte ist eine manuelle Designreferenz, keine vollständige API-Dokumentation.

## Prüfung wiederholen

`tools/check_develop_designs.cjs` benötigt Playwright und Sharp. Die gebündelten
Node- und Modulpfade über `load_workspace_dependencies` bestimmen, `NODE_PATH`
auf den Modulordner setzen und den Prüfer aus dem Projekt aufrufen:

```text
node tools/check_develop_designs.cjs
node tools/check_develop_designs.cjs --export
```

Ohne `--export` werden Ergebnisse ausschließlich unter
`build/develop-design-review/` geschrieben. Mit `--export` werden zusätzlich die
Designreferenzen hier erneuert. Windows verwendet Edge; `PHYSIM_BROWSER` erlaubt
einen expliziten Chromium-Browserpfad. Dies belegt keine Linux-/macOS-Ausführung.
Referenzbilder bleiben versioniert; erfolgreiche temporäre Prüfdateien unter
`build/` werden nach visueller Abnahme entfernt.
