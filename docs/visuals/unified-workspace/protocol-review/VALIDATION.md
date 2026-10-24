# Browserprüfung: Protokollentwurf und Inhaltsinventar

24. Oktober 2026, Windows, Playwright mit installiertem Microsoft Edge
**156.0.4314.42**, headless. Referenzbasis `7faf5e2` plus die Änderungen des
Protokollentwurfs; die dokumentierte laufende native Arbeit war nicht Testgegenstand.

| Ausgeführtes Kommando | Exitcode | Ergebnis |
| --- | --- | --- |
| `node tools/check_protocol_design.cjs` | 0 | 18 Kombinationen aus drei Bereichen, zwei Themes und 1440/760/500 px; Details/Filter/Suche, Quellenwechsel, Unicode/Originaltext, echter gefilterter TXT-Download, Quellsprung ohne Textänderung, Resizing und Escape-Rollback, Sitzungs-Buildfehler mit Wiederherstellung und echte Vorschau-Laufübernahme |
| `node tools/check_unified_workspace.cjs` | 0 | Gemeinsamer Modell-/Build-/Simulations-/Auswertungsfluss, 24 Layouts, keine Browserfehler |
| `node tools/check_workspace_resizing.cjs` | 0 | 11 bestehende Trenner, 24 Layouts; Quell-/Datenzustand erhalten |
| `node tools/check_graphic_export.cjs` | 0 | Freigegebener SVG-/PNG-Export mit echter Downloadprüfung, vier Dialogbreiten, Snapshot-/Zoom-/Einheitenfälle |

Lokal erforderliches Paketverzeichnis:
`NODE_PATH=C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules`.
Die Tests öffnen lokale Dateien; der Vorschau-Server ist für die Tests nicht nötig.
Syntaxprüfung von `protocol.js`, `workspace.js` und `bridge.js` mit `node --check`
und die Git-Whitespace-Prüfung bestanden ebenfalls.

Maschinenlesbar: [Protokoll](validation.json),
[gemeinsame Vorschau](unified-regression.json),
[bestehendes Resizing](resizing-regression.json),
[Grafikexport](export-regression.json).

Zusatzfälle des Protokolltests: 2× Browser-Pixeldichte, Komponententext auf 24 px
vergrößert (größer als das Doppelte der meisten Standardlabels), kurzes Fenster
760×740, reduzierte Bewegung und fokussierbarer Trenner unter Forced Colors.
Die Textprüfung stellt sicher, dass Meldungsdetails und Downloadaktion nach
Scrollen erreichbar sind. Kopiererfolg und -verweigerung verwenden ausschließlich
einen lokalen Clipboard-Stub; die Betriebssystem-Zwischenablage wird nicht verändert.

## Sichtbare Referenzen

- [Compilerfehler, dunkel, 1440×1000](develop-error-dark.png): Protokoll auf
  500 px vergrößert, Originalmeldung aufgeklappt, Quellsprung und Kopieren sichtbar.
- [Modellwarnung, hell, 1440×1000](simulate-warning-light.png): Simulieren mit
  derselben Protokollliste, Modellzeit und Auswirkung; Protokollhöhe 500 px.
- [Fehlerdetail bei 500×1000](compact-error-light.png): eigene kompakte
  Detailansicht mit „Alle Meldungen“ und fehlender Quellposition.
- [Große Schrift bei 760×740](large-text.png): gesamter Protokollbereich ist
  scrollbar, ausgewählter Text bleibt erreichbar. Kopf und Fuß dürfen dabei
  aus dem Ausschnitt scrollen.

Die vier Bilder wurden visuell geprüft. Ein früherer zu kleiner Inhaltsbereich
bei großer Schrift wurde durch den vollständigen Scrollfluss korrigiert.
Abbruch beim Ziehen stellt auch die ARIA-Höhenangabe wieder her. Tests warten
bei bereichsübergreifenden Meldungen auf die asynchrone Iframe-Zustellung.

Das [Inhaltsinventar](../CONTENT-INVENTORY.md) wurde mit den dort benannten
lokalen API-/Beispiel-/UI-Quellen abgeglichen. Keine zusätzliche Kraftsemantik
aus Pfeilgeometrie behauptet; neue Metadaten-/Legendenvorschläge ausdrücklich
getrennt. Keine native Implementierung durch diese Arbeit.

Grenzen: Windows-Browserprüfung, keine ausgeführte native Plattformprüfung,
kein physischer Monitor-/DPI-Wechsel, kein tatsächlicher Screenreaderlauf und
keine Performance-Abnahme bei laufendem nativem Meldungsstrom. Große Schrift
wurde am neuen Protokoll geprüft, nicht als erneute Gesamt-App-Abnahme.
Die bisherigen nativen Befunde bleiben unabhängig davon offen.

Speicherpflege: Nach der Prüfung 41 eigene erfolgreiche rohe Browserbilder und
temporäre Aufnahmehilfen entfernt (4.973.506 Byte). Die vier versionierten
Referenzbilder, JSON-Prüfberichte und sämtliche fremden/nativen Belege bleiben
erhalten. Keine Buildverzeichnisse pauschal geleert.
