# A als Grundlage, A2 als neutralerer Vergleich

29. Oktober 2026. Nutzerentscheidung: **A**. Zusätzlich beauftragte neue
Abwandlung: **A2 · Dezenter Schleier**, noch keine Bestätigung als Ersatz für A.
Die [Übergabe](../../README.md) und der Gesamtauftrag sind entsprechend aktualisiert.
Die bisherigen Prüfbelege im übergeordneten Ordner bleiben eingefroren und
beschreiben den Stand vor dieser Auswahl und A2-Ergänzung.

Geprüft auf Windows, Headless Microsoft Edge **156.0.4314.50**:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_selection_options.cjs
node --check docs/visuals/selection-options/selection.js
node --check tools/check_selection_options.cjs
```

Ergebnis: **Exit 0, 64 Layouts bestanden**, vier Markierungen × vier Inhaltsarten
× zwei Themes × 1440/500 logische Pixel Breite. Keine JavaScript-Laufzeitfehler.
Die ursprünglichen Maus-/Tastatur-, Kontextmenü-, Mehrfachauswahl-, Drag-Abbruch-,
Kontrast- und Accessibility-Browserproben bestehen weiterhin. Zusätzliche Prüfung:
**A ↔ A2 erhält dieselbe Textauswahl**, A bleibt farblich unverändert, A2 besitzt
in beiden Themes tatsächlich niedrigere Deckkraft und neutralere RGB-Werte.
Der niedrigste geprüfte Kontrast über alle Varianten bleibt 4,8004:1.

[Dunkle A2-Markierung](text-a2-dark.png) und [helle A2-Markierung](text-a2-light.png)
wurden visuell geprüft. Die Markierung ist deutlich weniger blau und schwächer;
ursprüngliche Syntaxfarben bleiben lesbar. Beide Bilder sind gezielte Ausschnitte
der tatsächlichen 1440-Pixel-Browseraufnahmen, mit identischer Auswahl wie A.

Beim ersten Lauf schloss ein noch ausstehendes Scrollereignis des bereits
abgeschlossenen Fokuswechsels auf einem kleinen Layout das gerade geöffnete
Menü. Der Handler ignoriert nun solche Ereignisse ohne neue Scrollposition
sowie Scrollen innerhalb des Menüs. Die vollständige Wiederholung besteht.
Eine zusätzliche echte Scrollgegenprobe in einem 1440×800-Fenster öffnet das
Menü per Umschalt+F10, scrollt die Seite um 40 Pixel und verlangt das geschlossene
Menü; [Ergebnis](scroll-check.json) erfolgreich. Die frühe Annahme dieser
Zusatzprobe, dass initialer Fokus bereits gescrollt haben müsse, war falsch:
Das initiale Beispiel setzt ausdrücklich `preventScroll`. Die korrigierte Probe
verlangt stattdessen die tatsächliche Positionsänderung und das Schließen.

[Vollständiger Bericht](validation.json) · [Quell- und Bildhashes](source-hashes.json).
Der Bericht nennt den Vorgänger-Commit; die Hashes binden die vor dem neuen
Commit geprüften Dateien. Keine native Physim- oder Plattformabnahme daraus
ableiten. Die zusätzliche Abwandlung bleibt auf die separate Vorschau begrenzt.
