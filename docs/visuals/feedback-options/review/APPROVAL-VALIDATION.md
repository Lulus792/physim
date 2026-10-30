# Freigabeprüfung: Rückmeldungen A

**30. Oktober 2026 · bestanden.** Nutzerfreigabe für A · Leiser Hinweis im
fertigen Design. Die [Entwurfsprüfung](VALIDATION.md) bleibt unverändert
historisch; ihre damalige offene Auswahl ist durch
[ADR 019](../../../decisions/019-selected-feedback-a.md) und
[APPROVED.md](../APPROVED.md) abgeschlossen.

Quellbasis `4a017c98d4155d2ab48edcbceaeace3d755ceed9` plus gehashte
Freigabeänderungen. Parallel vorhandene native Änderungen gehören nicht zu
dieser Browserprüfung und wurden nicht als abgeschlossen abgenommen.

Ausgeführt unter Windows, Edge **156.0.4314.50**, Playwright Headless:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_feedback_options.cjs
node --check docs/visuals/feedback-options/feedback.js
node --check tools/check_feedback_options.cjs
```

Exitcodes 0. Vollständige erneute Prüfung der 126 Layoutfälle mit A/B/C,
drei Themes, 1440/500 px und sieben Szenarien. Bestehende Bedienprüfungen
bestanden: lokale Fehler ohne zusätzlichen Toast, Wiederholen/Beheben,
Eingaben erhalten, Veraltet-Zustand, Gruppierung, Karten/Verlauf, Fokus/Escape,
optionaler Timer, 200% Root-Schrift, Forced Colors/Reduced Motion und 2× Dichte.
Keine JavaScript-Laufzeitfehler.

Zusätzlich Standardaufruf sowie `approved=1&variant=b/c` geprüft: fest A,
Variantenwahl verborgen, Freigabe sichtbar, URL mit `approved=1`, lokaler
Kopierfehler/Mehrfachbeispiel weiterhin verfügbar; Verlauf öffnen und mit
Escape schließen funktioniert. Historischer Vergleich explizit `approved=0`.

[Ausgewählte A-Referenz](approved-a.png) visuell geprüft: zwei kurze Karten
rechts unten, lokaler Veraltet-Hinweis, dezente Statuszeile und Verlaufszugang
wie ausgewählt. Keine Neugestaltung der Karten. Frühere gezielte helle,
schmale/Kontrast- und Fehlerreferenzen bleiben erhalten.

[Kompakter Maschinenbericht](approval-validation.json) ·
[SHA-256 der geprüften Quellen](approval-source-hashes.json), mit Git-Zeilenenden
LF normalisiert. Dokumentationslinks und begrenzter Git-Diff geprüft.
Eigene doppelte Rohbilder unter `build/feedback-options-review` nach Sicherung
der ausgewählten Referenz entfernt; frühere Belege und fremde Daten erhalten.

Keine offenen Fehler dieser Browserprüfung. Echte Ereignis-/Protokollanbindung,
Ziel-/Revisionsprüfung, Clipboard-/Dateioperationen, mehrere native Fenster,
Screenreader und Windows-/Linux-/macOS-Abnahme bleiben Aufgabe der nativen
Umsetzung. Fokus-Pause getestet; vollständige native Timer-/Hover-/Fenster-
Pausierung gesondert abnehmen. Designfreigabe bestätigt diese Umsetzung nicht.
