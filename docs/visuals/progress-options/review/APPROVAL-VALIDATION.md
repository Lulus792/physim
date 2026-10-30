# Auswahlprüfung: B · Aufgabenleiste mit Pop-out

**30. Oktober 2026 · bestanden.** Der Nutzer hat B mit eigenem Pop-out-Fenster
gewählt. Die [historische offene Auswahl](VALIDATION.md) ist durch
[ADR 020](../../../decisions/020-selected-progress-b-popout.md) und
[APPROVED.md](../APPROVED.md) abgeschlossen.

Quellbasis `31cb9cf0e8f2f9d68ea8194bccd6740ceca07129` plus gehashte
Auswahländerungen. Parallele native Änderungen sind nicht Teil dieser Prüfung.
Windows, Edge **156.0.4314.50**, Playwright Headless. Ausgeführt:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_progress_options.cjs
node tools/check_progress_popout.cjs
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/progress-options/index.html'
node tools/check_progress_popout.cjs
node --check docs/visuals/progress-options/progress.js
node --check tools/check_progress_options.cjs
node --check tools/check_progress_popout.cjs
```

Exitcodes 0. Die Pop-out-Prüfung wurde sowohl lokal als Datei als auch über
den gleichen HTTP-Vorschaupfad ausgeführt, den die App öffnet.

| Soll | Ist |
| --- | --- |
| Bestehende sechs Aufgaben/Zustände/Themes/Breiten und Bedienung | Vollständiger erneuter 156-Ansichten-Lauf bestanden |
| Standardaufruf/alte A/C-Links im gewählten Modus | Fest B, keine Variantenwahl; Pop-out-Zugang sichtbar |
| Echte separate Browser-Popup-Page | Geöffnet, gleiche Versuchsnummer/Zähler/Details; Hauptnotizen weiter bearbeitbar |
| Wiederholtes Öffnen | Bestehendes Fenster aktiviert, keine zusätzliche Page/Worker-Kopie |
| Abbruch/Bestätigung/neuer Versuch aus beiden Ansichten | Synchron, Zähler und Eingaben erhalten, alter Versuch nicht wieder aktiviert |
| Zur Leiste, Browserfenster schließen und Escape | Anzeige zurückgeholt, Zustand und Versuch unverändert; sinnvolle Fokusrückgabe |
| Nicht unterbrechbarer Exportabschluss | Abbruch auch im Pop-out gesperrt; Erklärung sichtbar |
| Erfolg vor Abbruchbestätigung | Gemeinsamer Erfolg; genau eine D05-A-Karte im Hauptbereich, Ergebniszugang im Pop-out |
| Fenster Hell/Dunkel/Hoher Kontrast, 360 px / 200% Schrift | Kein horizontaler Seitenüberlauf; Aktionen bedienbar |
| Quellseite neu laden | Altes Pop-out zeigt getrennte Verbindung ohne aktive Befehle |
| Falsche Sitzung / Popupblockade | Unbekannte Nachricht ignoriert; Blockade lässt Aufgabe in B erhalten |
| Schmale Leiste/große Schrift, Forced Colors/Reduced Motion | Pop-out- und Abbruchbedienung bestanden |
| Anschließender Icon-only-Wunsch | Leere sichtbare Buttonbeschriftung, SVG, Tooltip und zugänglicher Name für Pop-out/erneutes Anzeigen geprüft; 34-px-Button |
| JavaScript-Laufzeitfehler | 0 |

Bei der ersten Integration kollidierte ein Body-Datenattribut mit dem neuen
Aktionsselektor. Ansichtsattribut und Buttonselektoren getrennt; die vollständige
Regression danach bestanden. Escape schließt das Fenster bereits beim Keydown;
die Prüfung behandelt die erwartete Page-Schließung vor Keyup korrekt.
Diagnose des Reload-Falls zeigte: Chromium liefert die `pagehide`-Nachricht
mit `event.source === null`. Nur der verbindungstrennende Hinweis darf mit
passender Sitzungskennung so ankommen; Zustands-/Befehlsnachrichten behalten
die strenge Quellfensterzuordnung. Beide Transportwege bestanden. Die anschließende Icon-only-Nachführung wurde
zusätzlich mit der vollständigen HTTP-Pop-out-Prüfung einschließlich großer
Schrift und schmaler Leiste abgenommen. Die 156-Ansichten-Regression prüft die
Pop-out-Grundfassung vor dieser reinen Buttondarstellung.

## Ausgewählte Referenzen

- [Kompakte B-Leiste mit Pop-out-Zugang](approved-b-shelf.png)
- [Leiste während der Auslagerung](approved-b-detached.png)
- [Eigenes Browserfenster, Clientfläche](approved-b-window.png)
- [Nicht unterbrechbarer Export im Pop-out](export-popout.png)

Alle vier Referenzen visuell geprüft. [Maschinenberichte](approval-validation.json)
und [SHA-256 der Quellen](approval-source-hashes.json), mit Git-Zeilenenden LF
normalisiert. Dokumentationslinks und begrenzter Git-Diff geprüft. Doppelte
Rohbilder unter `build` nach Sicherung entfernt; frühere Belege erhalten.

Keine offenen Browserfehler dieser Prüfung. Die Pop-up-Page ist kein natives
Physim-OS-Fenster und beweist keine echte Worker-/Datei-/Threadfunktion.
Quellseite besitzt nur Fixture-Zustand. Eigene OS-Fenster, Job-Ownership,
mehrere Aufträge, echte Quell-/Shutdown-Rennen, Monitor-/DPI-Wechsel,
Screenreader und Windows-/Linux-/macOS-Abnahme bleiben native Arbeit.
