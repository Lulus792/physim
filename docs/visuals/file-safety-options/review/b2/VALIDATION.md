# B2 – gemeinsamer Dateivergleich als Entwurf

29. Oktober 2026. Nutzerpräferenz: B. Gewünschte neue Variante **ausdrücklich
als Entwurf**: eine gemeinsame Änderungsansicht wie ein Git-Diff. Noch keine
Freigabe für die native Übernahme. Die früheren A/B/C-Nachweise bleiben erhalten.

Ausgeführt auf Windows mit Headless Microsoft Edge **156.0.4314.50**:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_unified_diff.cjs
node tools/check_b2_preview.cjs
node tools/check_file_safety_options.cjs
node --check docs/visuals/file-safety-options/unified-diff.js
node --check docs/visuals/file-safety-options/safety.js
```

Alle drei Prüfer **Exit 0**, keine JavaScript-Laufzeitfehler.

- **176 Layouts:** A/B/B2/C × elf Thema-/Szenariozustände × zwei Themes ×
  1440/500 Pixel Breite. Kein horizontaler Seiten- oder Dialogüberlauf. Alle
  bisherigen Speicher-, Konflikt-, Wiederherstellungs- und Popupaufgaben
  bestehen einschließlich B2. [Vollständiger Bericht](validation.json).
- **Zeilenvergleich:** acht explizite Beispiele und 64 Textpaare einschließlich
  leerer Texte, Einfügen/Entfernen, identischer Fassungen, doppelter Zeilen,
  Unicode und abschließendem Zeilenumbruch. Alte und neue Texte werden aus den
  jeweiligen Ergebniszeilen unabhängig rekonstruiert; eine explizite Erwartung
  prüft Reihenfolge und alte/neue Nummern. Der begrenzte große Vergleich erhält
  beide Texte bei bewusst gröberer Darstellung. [Einzelbericht](diff-unit.json).
- **Tatsächliche B2-Oberfläche:** genau eine gemeinsame Ansicht in allen drei
  Themen und beiden Themes, keine nebeneinander dargestellten Versionen. Die
  DOM-Zeilen ergeben exakt die gespeicherten und ungespeicherten Fixturetexte.
  Wechsel zwischen C, Physim und Markdown aktualisiert Inhalt und Zahlen.
  B bleibt separat nebeneinander; Wechsel B↔B2 erhält die gewählte Datei.
- **Sonderfälle:** gelöschtes Original hat ausschließlich Pluszeilen; erneute
  externe Änderung aktualisiert die Minusfassung, erhält den Entwurf und
  verhindert Abschluss. Ohne lesbare Sicherung kein erfundener Vergleich.
- **Zusätzlich:** tatsächliches B2-Popup, 500 Pixel mit 200% UI-Schrift und
  erzwungene Systemfarben. Plus/Minus bleibt als eigenes Zeichen sichtbar.
  [Browserbericht](b2-validation.json).

Die gezielten Aufnahmen wurden visuell geprüft; besonders Speichern/Dunkel,
Konflikt/Hell und Wiederherstellung/Dunkel zeigen die neutrale Grundfläche und
die klar unterscheidbaren roten/grünen Änderungszeilen. Quelle und Text bleiben
lesbar. Gemeinsame Ansicht bedeutet keine zusammengeführte Datei.

| Thema | Dunkel | Hell |
| --- | --- | --- |
| Speichern | [B2](b2-save-dark.png) | [B2](b2-save-light.png) |
| Dateikonflikt | [B2](b2-conflict-dark.png) | [B2](b2-conflict-light.png) |
| Wiederherstellung | [B2](b2-recovery-dark.png) | [B2](b2-recovery-light.png) |

Aufnahmen: vollständige Dialog-Clientfläche, 1440×1100 Browserfenster, 1×.
[source-hashes.json](source-hashes.json) bindet Quellen, Prüfer und Bilder;
der allgemeine Browserbericht nennt den Vorgänger-Commit vor dem neuen Commit.

Kein Git-Prozess, Patch-Export, Merge oder echte Dateioperation. Der gemeinsame
Vergleich ist eine neue Browserdarstellung für Beispieldaten. Native große
Dateien, Accessibility-/Screenreader-, OS-Fenster- und Plattformabnahme bleiben
separate Aufgaben nach Auswahl. Bestehende A2-/Export-/Bereichsfreigaben erhalten.
