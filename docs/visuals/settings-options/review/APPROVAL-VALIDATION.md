# Freigabeprüfung: Einstellungen A

**29. Oktober 2026 · bestanden.** Nutzerfreigabe für A · Kategorien im fertigen
Design. Die [Entwurfsprüfung](VALIDATION.md) bleibt unverändert historisch;
ihre damalige offene Auswahl ist durch [ADR 018](../../../decisions/018-selected-settings-a.md)
und [APPROVED.md](../APPROVED.md) abgeschlossen.

Quellbasis `426a0fba` plus gehashte Freigabeänderungen. Native Arbeitsänderungen
waren parallel vorhanden; sie gehören nicht zu dieser Browserabnahme.

Ausgeführt unter Windows mit Edge 156.0.4314.50 und Playwright:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_settings_options.cjs
node --check docs/visuals/settings-options/settings.js
node --check tools/check_settings_options.cjs
```

Exitcodes 0. Vollständiger erneuter Lauf der 144 Layoutfälle und bestehenden
Interaktionen: Entwurf/Übernehmen/Abbrechen/Defaults, vollständiger Vergleich,
Schreib-/Lade-/Schriftfehler und Wiederholung, Suche/Fokus/Escape,
unabhängige Größenänderungen und Einheiten, 200% Darstellung, Forced Colors,
Reduced Motion und 2× Browser-Pixeldichte. Keine JavaScript-Laufzeitfehler.

Zusätzliche Soll-/Ist-Prüfung: Standardaufruf und `approved=1` erzwingen A,
auch bei `variant=b/c`; Variantenwahl verborgen, Freigabe sichtbar, Übernehmen
funktioniert und URL behält `approved=1`. Historischer Vergleich ist nur über
`approved=0` aktiv. Alle Prüfungen bestanden.

[Ausgewählte A-Referenz](approved-a.png) visuell geprüft: Kategorie links,
Optionen mittig, Miniatur rechts, Suche und Fußaktionen wie gewählt. Die
vorhandenen hellen/Kontrast- und Detailbilder aus der Entwurfsrunde gelten
weiter; keine Neugestaltung der ausgewählten Oberfläche.

[Kompakter Maschinenbericht](approval-validation.json) ·
[SHA-256 der geprüften Quellen](approval-source-hashes.json), mit Git-Zeilenenden
(LF) normalisiert; CRLF/LF ändern die Browsersemantik nicht.
Dokumentationslinks und `git diff --check` geprüft. Eigene wiederholte
Rohbilder unter `build/settings-options-review` nach Sicherung des ausgewählten
Bildes entfernt; frühere Referenzen unverändert erhalten.

Keine offenen Fehler dieser Browserprüfung. Native Einstellungen und
Einheitenfenster, sichere echte Speicherung, Schriftressourcen, Screenreader
und Windows-/Linux-/macOS-Abnahme bleiben Aufgabe des umsetzenden Chats.
Designfreigabe und bestandene Browserprüfung bestätigen diese Schritte nicht.
