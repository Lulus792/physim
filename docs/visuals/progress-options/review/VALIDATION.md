# D06 · Browserprüfung Fortschritt und Abbruch

**30. Oktober 2026 · bestanden · Auswahl offen.** Quellbasis
`0d7732c72c0bbf6b4fa73443c0559a20db0934da` plus neue, gehashte Entwurfsdateien.
Parallele native Arbeitsänderungen waren vorhanden und gehören nicht zu dieser
Abnahme. Keine nativen Builds oder Prozesse für die Vorschau gestartet.

Windows, Edge **156.0.4314.50**, Playwright Headless. Ausgeführt:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_progress_options.cjs
node --check docs/visuals/progress-options/progress.js
node --check tools/check_progress_options.cjs
```

Exitcodes 0. Lokale Dokumentationslinks, Quellen-Hashes und begrenzten
`git diff --check` zusätzlich geprüft.

| Soll | Ist |
| --- | --- |
| Alle sechs Aufgaben, A/B/C, drei Themes, 1440/500 px | 108 laufende Ansichten ohne Seiten-/Werkbanküberlauf |
| Sieben zusätzliche Serienzustände in A/B/C, zwei Breiten | 42 Ansichten; Teilfehler enden mit allen Phasen abgeschlossen, keine scheinbar noch aktive Abschlussphase |
| Nicht unterbrechbarer Exportabschluss, drei Richtungen/zwei Breiten | 6 Ansichten; Abbruch deaktiviert und erklärt |
| Bekannter Fortschritt versus unbekannter Umfang | Serie mit 4/12 und korrekten ARIA-Zählern; andere Aufgaben ohne `aria-valuenow` oder Prozenttext |
| Abbruch anfordern, Bestätigung abwarten | Bestanden in A/B/C, wiederholte Anforderung gesperrt; Fokus zu Details; erst Folgeschritt bestätigt Abbruch |
| Fertige Ergebnisse erhalten, neuer Versuch separat | Bestanden; Zähler des neuen Versuchs beginnt bei null, Notizen bleiben; alte Beispielantwort überschreibt neuen Zustand nicht |
| Ausblenden/Escape ist kein Abbruch | Bestanden; Status bleibt laufend, Wiederöffnen und Fokusrückgabe korrekt |
| Richtungs-/Bereichswechsel | Zustand, Versuch, Notizen und Zähler bleiben erhalten |
| Erfolg vor Abbruchbestätigung | Bestanden; bestätigter Erfolg und D05-A-Karte, kein falscher Abbruch; Hinweis schließen lässt Erfolg erhalten |
| Warteschlange entfernen | Bestanden; noch nicht begonnene Aufgabe direkt entfernt |
| Export Vorbereitung → Abschluss → Erfolg | Bestanden; kein Abbruch im Abschluss; Erfolg erst im folgenden Bestätigungsschritt |
| 200% Root-Schrift bei 500 px | Serie, langer Projektpfad und Exportabschluss inklusive Details in A/B/C ohne Seiten-/Werkbanküberlauf |
| Forced Colors/Reduced Motion und 2× Dichte | Abbruchfluss bedienbar, unbestimmte Animation abgeschaltet, 2× Browser-Pixeldichte ohne Seitenüberlauf |
| JavaScript-Laufzeitfehler | 0 |

Nach dem ersten erfolgreichen Lauf fiel bei der visuellen Prüfung auf, dass
„Mit Teilfehlern beendet“ die letzte Phase noch aktiv markierte. Korrigiert
und die passende Zustandsprüfung ergänzt. Der abschließende vollständige
156-Ansichten-Lauf ist erfolgreich.

## Gezielte visuelle Referenzen

- [A · Aufgabe am Inhalt](a-running.png)
- [B · kompakte Aufgabenleiste](b-running.png)
- [C · Begleitfenster-Clientvorschau](c-running.png)
- [Abbruch angefordert](canceling.png)
- [Mit Teilfehlern beendet, B aufgeklappt](partial.png)
- [Nicht unterbrechbarer Exportabschluss, hell](export-commit-light.png)

Alle sechs Referenzen visuell geprüft; Titel, Kontext, Zähler, Zustände und
Aktionen erkennbar. Doppelte Rohbilder unter `build` nach Übernahme dieser
Referenzen entfernt; bestehende Nachweise und fremde Daten erhalten.

[Maschinenbericht](validation.json) · [SHA-256 der Quellen](source-hashes.json),
mit Git-Zeilenenden LF normalisiert.

## Grenzen und nächste Zuständigkeit

Keine offenen Browserfehler dieses Laufs. A/B/C noch nicht ausgewählt.
Die scheinbaren Aufgaben, Schritte, Rennen, alten Antworten und Endzustände
sind ausschließlich Seitenspeicher-Fixtures. Kein Beweis für echte
Worker-/Prozess-/Generationssicherheit, sichere Dateioperationen oder vorhandene
Abbruchfähigkeit eines Adapters. C ist kein OS-Fenster. Kein Performance- oder
Screenreadernachweis, keine native Windows-/Linux-/macOS-Abnahme.

Nach Auswahl native Verträge anhand der [Übergabe](../README.md) prüfen und
implementieren; bestehende Freigaben und aktuelle native Arbeiten erhalten.
