# D05 · Browserprüfung der Rückmeldungsentwürfe

**30. Oktober 2026 · bestanden · Auswahl offen.**
Quellbasis `fd015a3048fb24b5070044242ab5db1b901e41af` plus neue, gehashte
Entwurfsdateien. Parallel vorhandene native Änderungen waren nicht Teil dieser
Prüfung und wurden nicht als fertig abgenommen.

Windows, Edge **156.0.4314.50**, Playwright Headless. Ausgeführt:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_feedback_options.cjs
node --check docs/visuals/feedback-options/feedback.js
node --check tools/check_feedback_options.cjs
```

Exitcodes 0. B mit aufgeklapptem Verlauf zusätzlich mit Playwright geöffnet,
Sichtbarkeit geprüft und als gezielte Referenz aufgenommen.

| Soll | Ist |
| --- | --- |
| 3 Richtungen × 3 Themes × 2 Breiten × 7 Szenarien | 126 Ansichten bei 1440/500 px ohne horizontalen Seiten-/Werkbanküberlauf |
| Kopierfehler lokal, Wiederholung ohne Behebung erfolglos | Bestanden, ein Eintrag mit Zähler; Eingaben/Export bleiben erhalten; Erfolg erst nach expliziter Fixture-Behebung |
| Pfad prüfen ohne Eingabeverlust; Einsetzen allein behebt nichts | Bestanden, Fokus und `aria-invalid` korrekt; kein Projekt angelegt |
| Veraltet bleibt nach Schließen des Verlaufs | Bestanden; nur explizite Beispiel-Neuauswertung erledigt den Zustand |
| Ereignisse und Beispieltext bleiben bei Varianten-/Bereichswechsel erhalten | Bestanden; Speichern ändert nur den Seitenspeicher und Dirty-Anzeige |
| Vier Ereignisse zu drei Ursachen gruppieren, keine zusätzliche lokale Fehlerkarte | Bestanden; A höchstens zwei Erfolgskarten; ausgeblendete Meldungen bleiben im Verlauf |
| Verlauf/Leerzustand/Escape/Fokusrückgabe | Bestanden für A/B/C; betroffene Stelle über Ereignis erreichbar |
| Optionales Ausblenden nach sechs aktiven Sekunden | Bestanden; Verlauf erhalten, Tastaturfokus pausiert Ablauf, lokale Fehler laufen nicht ab |
| 200% Root-Schrift bei 500 px | Pfad/Kopierfehler/Veraltet/Mehrfachmeldungen in allen Richtungen ohne Seitenüberlauf; Pfadaktionen bedienbar |
| Forced Colors/Reduced Motion, 2× Browser-Pixeldichte | Neuauswertung bedienbar, Dichte/Seitenbreite geprüft |
| JavaScript-Laufzeitfehler | 0 |

Der erste Timer-Test verwendete Playwright `fastForward`, das zwischenliegende
Intervalle überspringt. Für den tatsächlichen Sechs-Sekunden-Ablauf auf
`runFor` umgestellt. Anschließend wurde ein Überlauf langer Wörter in den
Variantenkarten bei 200% Text durch Wortumbruch behoben. Der vollständige
abschließende Lauf ist erfolgreich. Syntax und lokale Dokumentationslinks
sowie der begrenzte Git-Diff wurden geprüft.

## Gezielte visuelle Referenzen

- [A · Karten und Veraltet-Hinweis](a-burst.png)
- [B · kompaktes Statusband](b-burst.png) und [aufgeklappter Verlauf](b-expanded.png)
- [C · Aktivitäten rechts](c-burst.png)
- [Kopierfehler hell](copy-light.png)
- [Pfadfehler schmal und kontrastreich](path-narrow-contrast.png)

Alle sechs Zustände visuell geprüft: Hierarchie, lokale Erklärung, lesbare
Auswahl, Verlauf und kleine Ansicht. Wiederholte Rohbilder unter `build` nach
Sicherung dieser Referenzen entfernt; fremde Daten und alte Nachweise erhalten.

[Kompakter Maschinenbericht](validation.json) ·
[SHA-256 der Quellen](source-hashes.json), mit Git-Zeilenenden LF normalisiert.

## Grenzen

Keine offenen Browserfehler aus diesem Lauf. A/B/C noch nicht gewählt.
Keine echten Dateien, Zwischenablage, Projekte, Worker oder Physik berechnet.
Hover-/Tabwechsel-Pausierung implementiert; der automatisierte Timerfall prüft
Fokus und Zeitablauf. Keine vollständige Screenreader- oder native
Windows-/Linux-/macOS-Abnahme; Forced Colors ersetzt diese nicht. Die
Werkbank/Exportausschnitte sind Prüfhülle, keine neue native Gestaltung.
Die [Übergabe](../README.md) verlangt später echte Ereignisidentität und
Einbindung in Protokoll C sowie die bestehenden OS-Aufgabenfenster.
