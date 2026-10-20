# Finale Werkbank: Klarglas Kühl

Die [gemeinsame Designvorschau](../unified-workspace/index.html) verbindet diese
Werkbank mit Simulieren und Auswerten zu einem durchgängigen Beispielablauf.

**Kühl ist gewählt.** Panelköpfe, kompakte Tabs, Workspace-Pfadlinien,
schwebende Suchinsel und verstellbare Panels sind festgelegt. Veraltete
Farbvarianten und ihre Auswahl wurden entfernt.

Aktueller nativer Designabgleich, echte Windows-/Linux-Bilder und verbleibende
Unterschiede: [FIDELITY.md](FIDELITY.md).

Für die Umsetzung in einem anderen Chat:

1. [NEXT-CHAT.md](NEXT-CHAT.md): kopierfertiger Umsetzungsauftrag.
2. [IMPLEMENTATION.md](IMPLEMENTATION.md): Zuordnung zum nativen Code,
   Unterschiede zur Demo und Abnahmeliste.
3. [DESIGN.md](DESIGN.md): verbindliche Gestaltung, Materialwerte und Bedienung.

Visuelle Referenzen:

- [Interaktive Vorschau](index.html), lokal ohne Server oder externe Ressourcen.
- [Kühl dunkel](glass-cool-dark.png) und [Kühl hell](glass-cool-light.png).
- [Suchdetail dunkel/hell](glass-overview.png).

Die Suchinsel sitzt 4 px unter der Oberkante der Codefläche. Sie verschiebt
den Text auch mit geöffneter Ersetzung nicht. Die Vorschau startet zur Ansicht
des Materials mit offener Suche; Escape schließt sie, Ctrl/Command+F oder das
Suchicon öffnet sie. „Transparenz reduzieren“ zeigt den deckenden Fallback.

Der rechte Kontextschalter bleibt beim Schließen/Öffnen ganz rechts an derselben
Position, rechts von Suche und Speichern. Workspace, Kontext und Protokoll sind
an ihren Trennlinien per Maus und Tastatur größenverstellbar. Doppelklick oder
Enter setzt eine Größe zurück. Bei schmalen Fenstern verändern gestapelte
Nebenbereiche ihre Höhe.

Die native Umsetzung und ihre tatsächlichen Prüfbelege stehen in
[NATIVE-STATUS.md](NATIVE-STATUS.md). Speichern/Build und Dateiinhalte in der
HTML-Vorschau bleiben Demoabläufe; Änderungen gehen dort beim Neuladen verloren.

## Referenzprüfung

Playwright und Sharp über die verfügbaren Node-Laufzeitpfade verwenden;
`NODE_PATH` gegebenenfalls auf deren Modulordner setzen.

```text
node tools/check_develop_designs.cjs
node tools/check_develop_designs.cjs --export
```

`--export` erneuert die drei finalen Referenzbilder. Weitere Ausgaben landen
ignoriert unter `build/develop-design-review/` und werden nach Abnahme entfernt.
Windows verwendet Edge; `PHYSIM_BROWSER` kann einen anderen Chromium-Browser
angeben. Die tatsächlich ausgeführte Browserprüfung steht in DESIGN.md;
native und andere Plattformprüfungen sind separat erforderlich.
