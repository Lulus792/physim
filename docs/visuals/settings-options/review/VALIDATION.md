# Browserprüfung der Einstellungsentwürfe

Stand: **29. Oktober 2026 · bestanden · keine Designauswahl.**
Quellbasis `78ea9f0b5c28289201f2d97ff34b67b28b5d6778` plus die neuen,
hier gehashten Entwurfsdateien. Parallele native Arbeitsänderungen waren
vorhanden und wurden weder gebaut noch als Teil dieser Studie abgenommen.

## Ausgeführt

Windows, Edge **156.0.4314.50**, Playwright im Headless-Modus:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_settings_options.cjs
node --check docs/visuals/settings-options/settings.js
node --check docs/visuals/settings-options/data.js
node --check tools/check_settings_options.cjs
git diff --check -- AGENTS.md docs/visuals/settings-options docs/visuals/design-options/README.md docs/visuals/unified-workspace/NEXT-CHAT.md docs/visuals/unified-workspace/UI-DESIGN-BACKLOG.md tools/check_settings_options.cjs
```

| Soll | Ist |
| --- | --- |
| A/B/C × fünf Gruppen × drei Themes × zwei Breiten | 90 Ansichten ohne horizontalen Seiten-/Inhaltsüberlauf |
| Lade-, Schreib- und Schriftfehler in drei Richtungen, zwei Themes/Breiten | 36 Fehleransichten ohne Seitenüberlauf |
| Nur Fensterinhalt, drei Richtungen/Themes, zwei Breiten | 18 Ansichten bei 850 px Höhe ohne Seitenüberlauf |
| Entwurf erst nach Übernehmen wirksam; sechs Werte vollständig vergleichen | Bestanden in allen Richtungen |
| Abbrechen erhält angewendeten Stand; Defaults zunächst nur Entwurf | Bestanden, inklusive nativer Defaultwerte und Sichtbarkeitsmaske 31 |
| Schreib-/Schriftfehler behalten Werte und Entwurf; Wiederholen nach Freigabe | Bestanden; unlesbare Originalkonfiguration erst nach bewusstem Übernehmen ersetzt |
| Ungültige Größen zurückweisen; unabhängige Größenänderung behalten | Bestanden bei Übernehmen eines Themes und Abbrechen von Defaults |
| Suche/kein Treffer/Variantenwechsel erhalten Entwurf; Fokus und Escape | Bestanden; Escape verlässt zuerst den Vergleich und verwirft dann den Entwurf |
| Einheiten unabhängig übernehmen; allgemeines Abbrechen erhält Einheit/Aufgabe | Bestanden; cm/km/h, eigener Entwurf, SI, Symbol-/Faktor-/Überlaufprüfung |
| Große Darstellung und alternative Browserdarstellung | Übernehmen bei 500 px/200% Root-Schrift in A/B/C; Forced Colors/Reduced Motion und 2× Pixeldichte geprüft |
| Keine JavaScript-Laufzeitfehler | 0 Fehler |

Der erste Fokus-Test wurde korrigiert, weil programmgesteuertes `selectOption`
keinen Fokus setzt. Anschließend wurde ein zu breiter Variantenselektor auf
Buttons eingeschränkt, damit das Body-Datenattribut keinen zweiten Treffer oder
Galerie-Klickhandler bekommt. Der abschließende vollständige Lauf ist erfolgreich.

## Sichtprüfung und Referenzen

Die Grundlayouts wurden in Dunkel geprüft; zusätzlich Kategorieansicht in Hell,
Fokusansicht in Hohem Kontrast, Arbeitsflächen-Detail und Einheitenaufgabe.
Screenshots dienen der visuellen Referenz, nicht als Beleg aller Tabellenprüfungen.

- [A dunkel](a-dark.png), [hell](a-light.png), [hoher Kontrast](a-contrast.png)
- [B dunkel](b-dark.png), [hell](b-light.png), [hoher Kontrast](b-contrast.png)
- [C dunkel](c-dark.png), [hell](c-light.png), [hoher Kontrast](c-contrast.png)
- [Editor in A](a-editor.png), [Arbeitsfläche in C](c-workspace.png),
  [separate Einheitenaufgabe](unit-task.png)

[Kompakter Maschinenbericht](validation.json) und
[SHA-256 der geprüften Quellen](source-hashes.json).
Die Rohbilder unter `build` wurden nach Übernahme dieser Referenzen entfernt;
eingefrorene frühere Nachweise und fremde Builddaten bleiben erhalten.

## Grenzen und offene Punkte

Kein offener Browserfehler aus diesem Lauf. Die Nutzerwahl A/B/C steht aus.
Keine nativen Fenster, echten Preferences-Dateien, Schriftressourcen oder
Autosave-Prozesse geprüft. Keine Linux-/macOS- oder Screenreader-Abnahme.
Forced Colors und Fokusprüfungen ersetzen keine vollständige Accessibility-Prüfung.
Die Browserhülle ist kein Beleg für ADR-015-Aufgabenfenster. Native Umsetzung
erst nach Auswahl mit den bestehenden Funktionsverträgen abgleichen.
