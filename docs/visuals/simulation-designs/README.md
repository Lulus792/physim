# Simulationsbereich: drei Designstudien

Erstellt am 16. Oktober 2026 auf Grundlage des lokalen Physim-Stands und der
[bestehenden Designrichtung](../../design-direction.md). Am 19. Oktober wurde
**Messlabor** ausgewählt: Szenenbaum zu, Inspektor offen, Kurven zu. Die native Anwendung
wurde durch diese Studien nicht verändert.

**Native Umarbeitung vorbereitet:** [Kopierfertiger Auftrag](NEXT-CHAT.md) ·
[Funktionsabgleich, vollständiger Inspektor und Abnahme](IMPLEMENTATION.md).

**Neue Runde:** Drei Darstellungen innerhalb derselben Abschnitte:
**A · Ruhige Zeilen**, **B · Wertefelder**, **C · Kompakt**.
Oben in der Vorschau umschalten. [Dunkler Vergleich](inspector-styles-dark.png) ·
[Heller Vergleich](inspector-styles-light.png). A ist der vorläufige Startwert,
eine Auswahl durch den Nutzer steht noch aus.

Die Desktop-Bereichsgrenzen sind ziehbar und per Pfeiltasten bedienbar:
Seitenbreiten, Messbereich, Zeitleiste, Kurvenverhältnis und Arbeitsflächenhöhe.
**Modellwerte** wie Fadenlänge sind editierbar und bieten anschließend
**Neu kompilieren** bzw. **Stoppen & neu kompilieren** an. Die Browserdemo führt
keinen Compiler aus; sie demonstriert Vormerken, Verwerfen und Übernahme erst
im neuen Beispiellauf. [Bindungs- und Buildvertrag](MODEL-EDITING.md).

- [Interaktive Vorschau](index.html) lokal im Browser öffnen, ohne Installation,
  Server oder externe Ressourcen. Start: Messlabor, dunkel, pausiert.
- [Vergleich: Dunkel links, Hell rechts; Studio / Messlabor / Fokus von oben nach unten](overview.png).
- Studio: [Dunkel](studio-dark.png), [Hell](studio-light.png).
- Messlabor: [Dunkel](labor-dark.png), [Hell](labor-light.png).
- Fokus: [Dunkel](focus-dark.png), [Hell](focus-light.png).
- [Designvertrag und Prüfbericht](DESIGN.md).
- Vollständiger Inspektor: [Dunkel](inspector-dark-overview.png),
  [Hell](inspector-light-overview.png). Die neun Gruppen sind aufklappbar und
  unabhängig von der Szene scrollbar.

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
node tools/check_inspector_variants.cjs --export
```

Ohne `--export` liegen Prüfausgaben nur unter `build/simulation-design-review/`.
Mit `--export` werden zusätzlich die sechs Designbilder und die Übersicht hier
erneuert. Unter Windows verwendet der Prüfer Edge; `PHYSIM_BROWSER` kann einen
anderen Browserpfad angeben. Andere Systeme benötigen ein verfügbares Chromium.
Das ist keine Aussage über bereits ausgeführte Linux-/macOS-Prüfungen.

Die gelieferten Screenshots und die Übersicht bleiben als Designreferenzen
versioniert. Temporäre erfolgreiche Browserbilder unter `build/` werden nach
visueller Abnahme entfernt; offene Fehlerbelege würden erhalten bleiben.
