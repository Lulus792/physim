# D07 · Browserprüfung Kontextmenüs

**30. Oktober 2026 · bestanden · Auswahl offen.** Quellbasis
`bd17f8425b7809b62ad61ba64beb6e374290f8e6` im abschließenden HTTP-Lauf
plus neue, gehashte Entwurfsdateien.
Parallele native Änderungen waren vorhanden und wurden nicht durch diese
Studie gebaut, geprüft oder als abgeschlossen abgenommen.

Windows, Edge **156.0.4314.50**, Playwright Headless. Ausgeführt:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_context_menu_options.cjs
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/context-menu-options/index.html'
node tools/check_context_menu_options.cjs
node --check docs/visuals/context-menu-options/menus.js
node --check tools/check_context_menu_options.cjs
```

Exitcodes 0. Nach Korrekturen vollständige Prüfung bestanden, zuletzt über
denselben HTTP-Pfad wie die Vorschau. Lokale Dokumentationslinks, Quellen-
Hashes und begrenzter `git diff --check` zusätzlich geprüft.

| Soll | Ist |
| --- | --- |
| Neun Ziele × A/B/C × drei Themes × 1440/500 px | 162 geöffnete Menüs ohne horizontalen Seitenüberlauf, am Viewport begrenzt |
| Nur lesen, leerer Puffer, keine Quelle, ungespeicherter Tab | Weitere 24 geöffnete Menüs; korrekte gesperrte Befehle und Gründe |
| Linksauswahl ohne Menü, Rechtsklick/Tastaturmenü | Bestanden; Auswahl-/Textbereich beim Öffnen und Escape erhalten |
| Exakte Textauswahl kopieren, Cut/Paste/Undo im Seitenspeicher | Bestanden; keine Systemzwischenablage benötigt |
| Pfeile, Pos1/Ende, Enter, Tab, Escape | Bestanden, deaktivierte Punkte übersprungen, Fokus zur Quelle |
| Untermenü Rechts/Links/Enter und Maus-Zurück | Bestanden; Menü und Untermenü auch bei 360 px / 200% Schrift erreichbar |
| Mehrfachauswahl und Rechtsklick auf anderes Ziel | Bestehende Menge bleibt, anderes Element wird einzeln gewählt; gemeinsamer Text-/Pfadbezug |
| Freie Fläche in der Dateiliste | Passendes Hintergrundmenü, kein Öffnen des zuvor gewählten Elements; Markierung erhalten |
| Protokoll-Kopie | Vollständiger Fixture-Originaltext einschließlich abschließendem Zeilenumbruch, keine zusätzlichen Auswahlzeichen |
| Ungespeicherter Tab | B2-Zugang statt stillen Verwerfens; Speichern/Schließen eines anderen Tabs erhält die ungespeicherte Quelle |
| Diagramm-/Szenenbefehle | Bestehender Exportzugang, idempotenter Zoomreset; Checkbox für globales Raster; A2-Objektkontur mit vier Griffen |
| Rechte Ziehbewegung | Kein Szenenmenü, Geste ohne echte Kameraberechnung erkannt |
| Entferntes/revidiertes Ziel | Menü verworfen, alter Befehl verändert keinen Puffer/Inhalt |
| Resize/Scrollen außerhalb | Menü ohne Aktion geschlossen; eigenes Scrollen bleibt möglich |
| Forced Colors / Reduced Motion / 2× Dichte | Tastatur-Untermenü, Kopie und Seitenbreite geprüft |
| JavaScript-Laufzeitfehler | 0 |

Die Sichtprüfung ergänzte einen nicht lesbaren Textspiegel nur für den
Auswahlhintergrund, weil Chromium die native Textarea-Auswahl nach Fokuswechsel
ins Menü nicht mehr zeichnete. Der echte Text und Auswahlbereich bleiben erhalten.
Ein verspätetes altes Document-Scrollereignis konnte ein gerade geöffnetes Menü
schließen; jetzt wird tatsächliche Positionsänderung nach dem Öffnen geprüft.
Die Resize-Prüfung wartet auf das tatsächlich asynchron zugestellte Resize-
Ereignis statt unmittelbar nach der Viewport-Anforderung einen Endzustand
anzunehmen. Abschließende vollständige Läufe bestanden.

## Gezielte visuelle Referenzen

- [A · kompaktes Textmenü](a-text.png)
- [B · Zielkopf und Gruppen](b-text.png)
- [C · Befehle mit Erklärung](c-text.png)
- [Protokollmeldung in B](b-message.png)
- [Untermenü und Mehrfachauswahl, hell](c-submenu-light.png)
- [Kleines Fenster, große Schrift, hoher Kontrast](c-narrow-contrast.png)
- [Szenenobjekt mit A2-Markierung](a-scene.png)

Alle Referenzen visuell geprüft: Hierarchie, lesbarer Auswahltext, Zielbezug,
Untermenü und Statusfarben. Wiederholte Rohbilder unter `build` nach Übernahme
dieser Referenzen entfernt; frühere Nachweise und fremde Daten erhalten.
[Maschinenbericht](validation.json) · [SHA-256 der Quellen](source-hashes.json),
mit Git-Zeilenenden LF normalisiert.

## Grenzen

Keine offenen Browserfehler dieses Laufs. Nutzerwahl A/B/C steht aus.
Keine echte Datei-/Clipboard-/Projekt-/Kameraaktion, kein nativer Menüadapter
oder Screenreader geprüft. Semantische Rollen und Tastaturprüfungen ersetzen
keine UIA-/AX-/AT-SPI- oder reale Assistive-Abnahme. Keine vollständige
Windows-/Linux-/macOS-Abnahme. Die [Übergabe](../README.md) verlangt nach Wahl
echte Befehlsverfügbarkeit, Ziel-/Fensterbindung und native Gegenprüfungen.
