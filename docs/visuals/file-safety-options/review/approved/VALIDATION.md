# B2 als verbindliche Referenz

**29. Oktober 2026.** Der Nutzer hat B2 ausdrücklich ins fertige Design übernommen.
[Spezifikation](../../APPROVED.md), [ADR 017](../../../../decisions/017-selected-file-safety-b2.md)
und die Gesamtübergabe sind aktualisiert. Die früheren Vergleichs-/Entwurfs-
Nachweise bleiben eingefroren; deren damalige offene Auswahlstatus sind historisch.

Ausgeführt auf Windows / Headless Microsoft Edge **156.0.4314.50**:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_unified_diff.cjs
node tools/check_b2_preview.cjs
node tools/check_file_safety_options.cjs
node --check docs/visuals/file-safety-options/safety.js
node --check tools/check_b2_preview.cjs
```

Alle Prüfer **Exit 0**, keine JavaScript-Laufzeitfehler.

- **176 Layouts und vollständige bisherige Aufgaben bestehen:** vier historische
  Vergleichsrichtungen, elf Thema-/Szenariozustände, zwei Themes und 1440/500 Pixel
  Breite. Speicher-/Teilfehler-/Abbruch-, Konflikt-, Wiederherstellungs- und
  Popupabläufe bleiben erhalten. [Bericht](validation.json).
- **Diff-Prüfung:** acht explizite Beispiele, 64 Textpaare, Zeilen-/Nummernfolge,
  unabhängige Rekonstruktion beider Texte und begrenzter großer Vergleich bestehen.
  [Bericht](diff-unit.json).
- **B2-Oberfläche und Freigabemodus:** gleiche Diff-Zeilen bei Datei-/Revisionswechsel,
  B2 als Standard, Vergleichskarten in `approved=1` verborgen und widersprechendes
  `variant=a` ignoriert. Themen, Themes, Szenario und Neuladen erhalten B2. Die
  Vergleichshistorie verändert die festgelegte Referenz nicht. Kleine Fenster,
  200% UI, Systemfarben und Test-Popup weiterhin geprüft.
  [Bericht](b2-validation.json).
- 259 lokale Verweise in elf geänderten Markdown-Einstiegsdokumenten geprüft;
  alle Ziele vorhanden. Aktuelle Übernahmesperren und Entwurfskennzeichnungen
  dieser Runde durch die Freigabe ersetzt; historische Berichte unverändert.

Die [ausgewählte Referenzaufnahme](b2-approved.png) wurde visuell angesehen.
Diff, Farbrollen und Clientanordnung wurden durch die Freigabe nicht neu gestaltet.
[source-hashes.json](source-hashes.json) bindet Quellen, Prüfer und Spezifikation;
der Browserbericht nennt den Vorgänger-Commit vor dem neuen Commit.

Die Browserhülle schreibt weiterhin keine Nutzerdateien und implementiert keine
nativen OS-Aufgabenfenster. Echte Kopie, Speichern, Wiederherstellung und deren
Datei-/Fenster-/Plattformabnahme gehören gemäß Spezifikation zum umsetzenden Chat.
Die Freigabe ist verbindlich; native Fertigstellung wird hier nicht behauptet.
