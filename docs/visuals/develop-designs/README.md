# Entwickeln: Werkbank und Detailstudien

Erstellt am 19. Oktober 2026 auf Grundlage des lokalen Projektstands und der
[verbindlichen Designrichtung](../../design-direction.md). **Werkbank ist vom
Nutzer ausgewählt.** Festgelegt sind **Schalter im Panelkopf, kompakte Tabs aus
Statusdock und Pfadlinien im Workspace**. Diese Kombination ist die Startansicht.
Verantwortlich: UI/UX-Design.

- [Aktuelle interaktive Vorschau](index.html): drei Werkbankvarianten, ohne zusätzliche Werkzeugleiste.
- [Schwebende Suche und Panelgrößen](SEARCH-RESIZE.md): zwei bedienbare Suchentwürfe und ziehbare Trennlinien.
- [Suchvergleich in Hell/Dunkel](search-overview.png).
- Ausgewählte Kombination: [Dunkel](selected-workbench-dark.png), [Hell](selected-workbench-light.png).
- [Detailvergleich](detail-overview.png): Randgriffe / Panelköpfe / Statusdock, jeweils dunkel links und hell rechts.
- **Randgriffe**: [Dunkel](detail-edges-dark.png), [Hell](detail-edges-light.png).
- **Panelköpfe**: [Dunkel](detail-headers-dark.png), [Hell](detail-headers-light.png).
- **Statusdock**: [Dunkel](detail-dock-dark.png), [Hell](detail-dock-light.png).
- [Detailvertrag und aktuelle Prüfung](DETAILS.md).

Oben lassen sich Schalterposition, Tabstil und Workspace unabhängig kombinieren.
**Ausgewählte Werkbank** stellt die festgelegte Kombination wieder her.
Die Unterordner bleiben in derselben Reihenfolge und Ebene. Der Code beginnt
bei der geprüften Desktopbreite rund 122 px weiter oben als im ursprünglichen Entwurf.
Suche und Speichern sitzen an den Dateitabs; Profil und Buildstatus in der
vorhandenen Statuszeile. Keine native Umsetzung erfolgt.

**Neue Suchrunde:** „Suchinsel“ oder „Suchkarte“ anklicken, um die jeweilige
Überlagerung zu öffnen. Sie verschiebt den Code auch bei aufgeklapptem Ersetzen
nicht. Workspace, Codekontext und Protokoll lassen sich an ihren Trennlinien
vergrößern/verkleinern; Tab und Pfeiltasten sind ebenfalls unterstützt.

## Erste Vergleichsrunde

- [Ursprüngliche interaktive Vorschau](original.html): als Vergleich erhalten.
- [Alle drei Varianten im Vergleich](overview.png): Dunkel links, Hell rechts.
- **Werkbank**: [Dunkel](workbench-dark.png), [Hell](workbench-light.png).
- **API-Atelier**: [Dunkel](atelier-dark.png), [Hell](atelier-light.png).
- **Fokus**: [Dunkel](focus-dark.png), [Hell](focus-light.png).
- [Compilerfehler mit Sprung zur Codestelle](compiler-error.png).
- [Designvertrag, Zustände und Abnahme](DESIGN.md).

Die ursprüngliche Galerie startet mit Werkbank und einem erfolgreichen Demo-Build. Designs,
Theme und Vorschauzustände lassen sich oben umschalten. Im App-Fenster sind
Dateien, Code, Suche/Ersetzen, Speichern, Panels und das Buildmenü bedienbar.
F5 startet einen Demo-Build; Ctrl/Command+F öffnet die Suche und Ctrl/Command+S
markiert die Demoänderungen als gespeichert. Beim Neuladen gehen sie verloren.

Die native Anwendung wurde nicht verändert. Build, Compiler, Dateisystem,
Projektmanager und Arbeitsbereichwechsel sind nicht angebunden. Der Code ist
ein gekürzter, illustrativer Ausschnitt des lokalen Pendelbeispiels. Die
API-Karte ist eine manuelle Designreferenz, keine vollständige API-Dokumentation.

## Prüfung wiederholen

Die Browserprüfer benötigen Playwright und Sharp. Die gebündelten
Node- und Modulpfade über `load_workspace_dependencies` bestimmen, `NODE_PATH`
auf den Modulordner setzen und den Prüfer aus dem Projekt aufrufen:

```text
node tools/check_develop_designs.cjs
node tools/check_develop_designs.cjs --export
node tools/check_workbench_details.cjs
node tools/check_workbench_details.cjs --export
node tools/check_workbench_search_resize.cjs --export
```

Ohne `--export` werden Ergebnisse ausschließlich unter
`build/develop-design-review/`, `build/workbench-detail-review/` beziehungsweise
`build/workbench-search-review/`
geschrieben. Mit `--export` werden zusätzlich die jeweiligen
Designreferenzen hier erneuert. Windows verwendet Edge; `PHYSIM_BROWSER` erlaubt
einen expliziten Chromium-Browserpfad. Dies belegt keine Linux-/macOS-Ausführung.
Referenzbilder bleiben versioniert; erfolgreiche temporäre Prüfdateien unter
`build/` werden nach visueller Abnahme entfernt.
