# Prüfung der getrennten Markierungsstudie

Ausgeführt am **29. Oktober 2026**, Windows, Headless Microsoft Edge
156.0.4314.42 über Playwright. Kein nativer Physim-Build für diese reine
HTML/CSS/JavaScript-Designstudie geändert oder als abgenommen erklärt.

Quellstand: Vorgänger-Commit in [validation.json](validation.json), die tatsächlich
geprüften geänderten Quellen separat mit SHA-256 in
[source-hashes.json](source-hashes.json). Der Prüflauf erfolgt vor dem Commit
dieses Pakets; der Vorgänger-Hash allein beschreibt die Änderungen daher nicht.

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_selection_options.cjs
node --check docs/visuals/selection-options/selection.js
git diff --check -- docs/visuals/selection-options docs/visuals/design-options/README.md docs/visuals/unified-workspace/NEXT-CHAT.md tools/check_selection_options.cjs
```

## Soll und tatsächlich geprüftes Ergebnis

- **48 Layouts bestehen, Exit 0:** drei Markierungen × vier Inhaltsarten × zwei
  Themes × 1440/500 logische Pixel Breite, jeweils 1100 Pixel Fensterhöhe.
  Kein horizontaler Seitenüberlauf. Beispielauswahl öffnet kein Menü.
- **Echte Mausaufgaben bestehen:** mehrzeilige Textauswahl durch Ziehen,
  unveränderte Textauswahl nach Rechtsklick, Strg-/Umschalt-Mehrfachauswahl,
  Rechtsklick auf ausgewählte bzw. andere Zeilen, Szenen-Auswahlrechteck und
  Escape-Rollback während einer Zeigerbewegung. Menüaktionen übernehmen die
  ausgewählten Inhalte; Details und Isolierung funktionieren in der Vorschau.
- **Tastaturaufgaben bestehen:** Umschalt+F10 in jedem Layout, Menüposition
  innerhalb des Fensters, Escape mit Fokus-Rückgabe, Pfeile/Pos1/Ende/Enter,
  native Menütaste und Strg+A nur im Quelltextbereich.
- **Kontrast der ausgewählten Texte:** Auswahl-Schrift und komponierter
  Auswahlhintergrund aus tatsächlichen CSS-Highlightrollen für normalen Text,
  Schlüsselwörter, Zahlen, Aufrufe und Kommentare geprüft. Alle drei Varianten
  in beiden Themes erreichen mindestens **4,8004:1**. Keine globale Aussage
  über sämtliche sonstigen Oberflächenfarben daraus ableiten.
- **Zusätzliche Browserzustände bestehen:** 200% Wurzel-Schriftgröße,
  erzwungene Systemfarben, reduzierte Bewegung und 2× Browser-Pixeldichte.
  Das sind keine physischen Monitor-/DPI-Wechselprüfungen.
- **Clipboard-Fehler:** bewusst verweigerte Schreibberechtigung führt zur
  verständlichen Fehlermeldung. Keine erfolgreiche externe Zwischenablage-
  oder Betriebssystemintegration damit behauptet.
- Keine JavaScript-Laufzeitfehler im finalen Prüflauf.

Die visuellen Ausschnitte wurden zusätzlich angesehen: A erhält die Syntaxfarben,
B unterstreicht die Auswahl, C verwendet klar zugeordnete Schrift. Szenentext
bleibt in den Konturen sichtbar. Der Kraft-Auswahlrahmen wurde bei der Sichtprüfung
verbreitert, damit er die vollständige Beschriftung umfasst; danach die relevanten
Maus-/Layoutprüfungen erneut ausgeführt.

Bei den frühen Prüferläufen wurden zwei Testannahmen korrigiert: `innerText`
normalisiert die Darstellung mehrzeiliger Suchtexte, daher wird die unveränderte
Datenübergabe mit `textContent` geprüft. Der CSS-Systemfarbenprobe fehlte ein
expliziter erster Treffer. Offene Menüs werden als Viewportbild aufgenommen,
weil eine Vollseitenaufnahme eine Scrolländerung und damit das absichtlich
vorgesehene Schließen des Menüs auslösen kann. Der finale Bericht enthält nur
die anschließend erfolgreich ausgeführte Prüfung.

## Kompakte visuelle Referenzen

| Markierung | Dunkel | Hell |
| --- | --- | --- |
| A | [Text](text-a-dark.png) | [Text](text-a-light.png) |
| B | [Text](text-b-dark.png) | [Text](text-b-light.png) |
| C | [Text](text-c-dark.png) | [Text](text-c-light.png) |

[Auswahlkonturen in der Szene](scene-contours.png) ·
[Protokollmarkierung mit offenem Rechtsklick-Menü](protocol-menu.png)

Text- und Szenenbilder sind gezielte Ausschnitte der Browseraufnahmen; Maße
und Ausschnitte sind im Hashbericht festgehalten. Sechs Textausschnitte dienen
dem Vergleich der tatsächlich unterschiedlichen Markierungen/Themes. Weitere
temporäre Erfolgsaufnahmen gehören in den ignorierten Buildordner.

## Offene Abnahme und Zuständigkeit

Die Auswahl zwischen A/B/C bleibt beim Nutzer. Linux, macOS, echte native
Menüs/Renderer, Screenreader, Editier-/Dateirechte, reale Dateisystemaktionen,
IME/Bidi, große Listen und praktische DPI-Wechsel sind durch diese Studie nicht
abgenommen. Nach Auswahl übernimmt die native Arbeit die Zustandsregeln aus
[README.md](../README.md) und prüft sie am dann aktuellen Physim-Stand.
