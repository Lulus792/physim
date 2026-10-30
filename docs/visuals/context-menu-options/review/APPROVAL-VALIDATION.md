# Freigabeprüfung: Kontextmenüs A

Datum: **30. Oktober 2026**. Nutzerentscheidung: **A · Kompakt**.
Basis vor dieser Änderung: `9798acb285158b3b88acda3469f9d436aee2b69e`.
Der genaue geprüfte Quellinhalt ist in
[approval-source-hashes.json](approval-source-hashes.json) mit SHA-256 nach
CRLF→LF-Normalisierung festgehalten. Fremde native Änderungen im gemeinsamen
Arbeitsverzeichnis wurden nicht durch diese Designänderung bearbeitet.

## Ausgeführt

Windows, Edge **156.0.4314.50**, headless über Playwright. Lokaler HTTP-Server
`http://127.0.0.1:8791`, Wurzel `D:/Physim/docs/visuals`.

```powershell
node --check docs/visuals/context-menu-options/menus.js
node --check tools/check_context_menu_options.cjs
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/context-menu-options/index.html'
node tools/check_context_menu_options.cjs
git diff --check
```

Ergebnis: **bestanden**, keine JavaScript-Seitenfehler.
[Maschinenbericht](approval-validation.json).

- 186 geöffnete Layoutfälle: neun Ziele × drei Varianten × drei Themes × zwei
  Breiten; zusätzlich Nur lesen, leerer Puffer, fehlende Quelle und ungespeicherter
  Tab. Menü im Viewport, keine horizontale Seitenüberbreite.
- Bestehende Funktionsprüfung aller Varianten erhalten: Auswahl ohne automatisches
  Menü; exakt Copy/Cut/Paste/Undo im Beispielpuffer; deaktivierte Befehle,
  Tastatur/Fokus, Untermenüs, Mehrfachauswahl, freie Fläche, Protokoll-Originaltext,
  Tab-Zielbindung/B2-Referenz, Exportreferenz/Zoom, Raster und Kamerageste,
  veraltete Ziele/Aktionen werden verworfen.
- 360 Pixel und 200 % Wurzelschrift, Menü-/Untermenü-Scroll, Resize und
  Außen-Scroll, Forced Colors/Reduced Motion und zweifache Browser-Pixeldichte.
- Neu: Standardaufruf und freigegebene B-/C-Links erzwingen A, Variantenwahl
  und Menü-Zielkopf bleiben unsichtbar; Menü startet geschlossen. Tastaturöffnung,
  Escape und `approved=1` bleiben funktionsfähig. `approved=0` erhält den Vergleich.

Die [freigegebene Referenzaufnahme](approved-a.png) wurde visuell geprüft:
Freigabestatus, kompakte Liste, Trennlinien, Tastaturhinweise, Fokus, gesperrter
Undo-Befehl mit Grund und erhaltene dezente Textauswahl. Die ursprünglichen
[Vergleichsnachweise](VALIDATION.md) bleiben unverändert.

## Grenzen und nächste Arbeit

Die Browser-Fixture verändert nur ihren Beispieltext/-puffer. Keine echten
Datei-, Clipboard-, Speicher-, Physik- oder Kameraoperationen wurden geprüft.
DOM-Rollen und Tastatureingaben sind keine reale Screenreaderabnahme. Es wurde
kein nativer Windows-/Linux-/macOS-Build für diese reine Designfreigabe ausgeführt.

Native Integration und Plattformabnahme gemäß [APPROVED.md](../APPROVED.md)
bleiben offen. A ist verbindlich und braucht keine erneute Designentscheidung.
D08 „Editor-Suche und Ersetzen“ ist die nächste neue Entwurfsrunde.
