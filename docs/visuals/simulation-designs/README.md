# Simulationsbereich: drei Designstudien

Erstellt am 16. Oktober 2026 auf Grundlage des lokalen Physim-Stands und der
[bestehenden Designrichtung](../../design-direction.md). Am 19. Oktober wurde
**Messlabor** ausgewählt: Szenenbaum zu, Inspektor offen, Kurven zu. Die native Anwendung
wurde durch diese Studien nicht verändert.

- [Interaktive Vorschau](index.html) lokal im Browser öffnen, ohne Installation,
  Server oder externe Ressourcen. Start: Messlabor, dunkel, pausiert.
- [Vergleich: Dunkel links, Hell rechts; Studio / Messlabor / Fokus von oben nach unten](overview.png).
- Studio: [Dunkel](studio-dark.png), [Hell](studio-light.png).
- Messlabor: [Dunkel](labor-dark.png), [Hell](labor-light.png).
- Fokus: [Dunkel](focus-dark.png), [Hell](focus-light.png).
- [Designvertrag und Prüfbericht](DESIGN.md).

Die Galerie außerhalb des App-Fensters gehört nur zum Variantenvergleich.
Die Auswahl oben schaltet das Layout, daneben lassen sich Theme und Laufzustand
wechseln. Innerhalb der Vorschau sind Seitenbereiche, Objektauswahl, Sichtbarkeit,
Laufsteuerung, Tempo, Kameraansichten und Zeitleiste bedienbar.

**Aktualisiert am 19. Oktober:** Über den Schalter **Kurven anzeigen** in der
Messwertleiste sind die beiden Signalverläufe ein- und ausblendbar. Studio und
Messlabor starten mit zugeklappten Kurven. Beim Zuklappen wächst die Szene; die Messwerte
und Zeitleiste bleiben sichtbar. Der Zustand wird pro Variante für diese Sitzung
behalten. Vergleichszustände: Studio aufgeklappt
([Dunkel](studio-dark-curves-open.png), [Hell](studio-light-curves-open.png)) und
Messlabor aufgeklappt ([Dunkel](labor-dark-curves-open.png),
[Hell](labor-light-curves-open.png)). Die bisherigen Dateinamen
`labor-*-curves-closed.png` zeigen ebenfalls den aktuellen Startzustand.

Die Pendelbewegung und Kurven sind illustrative Beispieldaten aus einer
Kleinwinkelnäherung mit l = 1,50 m, m = 1,00 kg und θ₀ = 0,50 rad. Die Zeit läuft
in der Vorschau von 0 bis 6 s. Der native Runner, echte Laufdateien, Kamerainteraktion
und Build sind nicht angebunden. „Offline“ illustriert nur die Auswahl und bewegt
die Demo mit 16×. Die globale Menüzeile und die drei Arbeitsbereich-Tabs sind
statische Bestandteile des Designentwurfs.

## Prüfung wiederholen

`tools/check_simulation_designs.cjs` verwendet Playwright und Sharp. Die gebündelten
Laufzeitpfade über `load_workspace_dependencies` bestimmen, den Modulordner als
`NODE_PATH` setzen und den Prüfer mit Node ausführen:

```text
node tools/check_simulation_designs.cjs
node tools/check_simulation_designs.cjs --export
```

Ohne `--export` liegen Prüfausgaben nur unter `build/simulation-design-review/`.
Mit `--export` werden zusätzlich die sechs Designbilder und die Übersicht hier
erneuert. Unter Windows verwendet der Prüfer Edge; `PHYSIM_BROWSER` kann einen
anderen Browserpfad angeben. Andere Systeme benötigen ein verfügbares Chromium.
Das ist keine Aussage über bereits ausgeführte Linux-/macOS-Prüfungen.

Die gelieferten Screenshots und die Übersicht bleiben als Designreferenzen
versioniert. Temporäre erfolgreiche Browserbilder unter `build/` werden nach
visueller Abnahme entfernt; offene Fehlerbelege würden erhalten bleiben.
