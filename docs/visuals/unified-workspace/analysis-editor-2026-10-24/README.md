# Analysecode in der Werkbank – native Belege vom 24. Oktober 2026

Die [Umsetzung](../ANALYSIS-EDITOR-NATIVE.md) betrifft den gemeinsamen
Werkbank-Einstieg für Analysecode und dabei gefundene Eingabe-/Undo-Fehler.
Die gesamte Oberfläche einschließlich F1 ist damit nicht abgenommen.

## Abschließende UI-Quelle 9, unveränderte Kerntests aus Quelle 8

| Plattform | UI-Gruppen | Kerntests |
| --- | --- | --- |
| Windows/MSVC Release | `run-75iw1xfc`, 4/4 | `run-qt2olw5a`, 2/2 |
| Alpine-Dev/GCC, ASan/UBSan | `run-3u1ied8j`, 4/4 | `run-wv9m55ps`, 2/2 |

`final9-windows-native.json.gz` / `final9-linux-native.json.gz` enthalten die
tatsächlichen Kommandos, Binärhashes, Ausgaben und Exitcodes der neuen Gruppe.
Die sechs Varianten pro Plattform in `final9-*-validation.json.gz` unterscheiden
ausdrücklich vier normale Sprach-/Schriftkombinationen und zwei Analyseprojekte
ohne Experiment. `final9-capture-hashes.json.gz` schützt ihre 84 nativen
PNG-Aufnahmen. Die ausgewählten aktuellen Bilder tragen ebenfalls `final9-`.

Die `final9-`-Ergebnisdateien der drei bestehenden UI-Gruppen verwenden Quelle 9.
`final9-regression-image-hashes.json.gz` erfasst weitere 50 originale BMP-/PNG-Dateien.
Die Kerntestergebnisse ohne diesen Präfix verwenden Quelle 8.
`final9-core-provenance.json.gz` prüft deren unveränderte Header-/Testeingaben
und vier Binärdateien; die zusätzliche UTF-8-Testdatei bleibt gegenüber dem
Basiscommit unverändert. Die Rendererbilder wurden nicht
skaliert, retuschiert oder umgefärbt. Beide Runner-/Katalogprüfungen sind als
komprimierte Logs enthalten. Xvfb/Openbox und Software-OpenGL bilden die
Linux-Grafikumgebung. ASan/UBSan sind aktiv; die Hilfsumgebung setzt
`ASAN_OPTIONS=detect_leaks=0`, auch für den abschließenden Kernnachlauf.

## Ursprünglicher Gegenfall und verworfene Stände

- `before-windows.png` / `baseline.json.gz`: vorhandener Windows-Release-SDK,
  bestehende Ersetzen-Fixture, altes Analyseeditorlayout. Der Client ist
  1440 × 940 statt der 1440 × 1000 der großen neuen Variante. Der begrenzte
  Gegenvergleich verwendet ausschließlich dieselbe obere Navigationsfläche bei
  identischer Breite. Der eigene Binärhash des Vorheraufrufs wurde nicht erneut
  aufgezeichnet; dieser Lauf ersetzt keine aktuelle Quellprovenienz.
- Der Vorher-PNG wurde verlustfrei aus dem nativen BMP erzeugt und enthält
  dessen Framebuffer-Alpha. Für den RGB-Gegenvergleich wird Alpha im Speicher
  ignoriert. Die RGB-Bytes stimmen mit dem Original-BMP überein; die gespeicherte
  PNG-Datei wird nicht verändert. `counterexample.json.gz` nennt den beschränkten
  Farbtest und seine tatsächlichen Treffer.
- Quelle 1 setzt in der neuen Testvorbereitung einen Dokumentzeiger fälschlich
  auf `true`. Windows stürzt beim ersten Kontextwechsel ab, GCC weist die
  Zuweisung zurück. Ergebnis und vollständiges Linux-Buildlog bleiben als Fehler
  erhalten; sie sind kein Produktnachweis.
- Quelle 2 entfernt diese Testzuweisung. Der neue Eintrittstest scheitert auf
  beiden Plattformen am noch aktiven alten Analysekontext. Quelle 3 bindet den
  Kontextwechsel unmittelbar an die Dateiauswahl.
- Quellen 3 bis 5 zeichnen die Werkbank, scheitern aber an der Schreibprobe nach
  Rückkehr: Die Kopfzeile übernimmt die Tastatur wieder. Einmaliges Setzen des
  Fensters und danach die zusätzliche semantische Übergabe reichen noch nicht.
- Der Debuggerlauf mit Quelle 5 (`analysis-editor-input.*.gz`) beobachtet echte
  Textankunft, `window=header` und `ROM=1/1` im Editor. Der Ablauf endet weiterhin
  mit Fehler; die Diagnose ist kein bestandener UI-Lauf und keine Leakprüfung.
- Quelle 6 behält den gezielten Editorbesitz über Frames. Der Windows-Fall
  besteht, prüft aber noch keinen gültigen großen Cursor nach Undo. Linux bricht
  beim Undo am Dateiende mit `cursor_ptr` ab. `prior6-windows.json.gz` bleibt als
  historisch grün, aber unzureichend gekennzeichnet.
- Quelle 7 ergänzt die unabhängige Grenzprüfung. Beide Plattformen verwerfen
  den alten `short`-Cursor. Der absichtlich früh abbrechende C-Test lässt dabei
  seine Testpuffer zurück; die LSan-Ausgabe des negativen Linux-Kernlaufs ist kein
  eigenständiger Produkt-Leaknachweis.
- Quelle 8 entfernt die Verengung, prüft Cursor/Auswahl nach dem nativen Undo
  und ergänzt die ausdrückliche native Fokus-/Menüübergabe. Die finale
  Schreibprobe verwendet einen Unicode-Codepoint und genau eine Undo-Einheit;
  die Daten-/Textschutzbedingungen bleiben erhalten.
- Quelle 8 besteht alle funktionalen Abläufe, berücksichtigt aber in der
  Dateibaum-Pfadberechnung den Analyseeditor noch nicht. Quelle 9 ergänzt diesen
  Fall und verlangt die tatsächlich gefüllte Auswahlzeile in allen Bildvarianten.
  `counter8-tree.json.gz` weist das entsprechende Bild aus Quelle 8 mit demselben
  Pixelkriterium zurück. Die unpräfigierten Ergebnis-/Bilddateien bleiben als
  Vorprüfung aus Quelle 8 erhalten; `manifest8.json` hält deren ursprüngliche
  68 Belegdatei-Hashes fest. Die alte Sammlung wurde um Quelle 9 erweitert.

## Provenienz und Grenzen

Basiscommit: `7faf5e26bb919ad480bc5fbdbddf54d3a40928c7`.
`source1..9.json.gz` erfassen die jeweiligen Quell-/Fixture-/Prüfstände;
die abschließende UI-Quelle umfasst 282 Dateien sowie 18 Schriftressourcen und
die Rohhashes zwölf unverändert vorgefundener fremder Dateien.
`delta1..9.json.gz` halten die eigenen vollständigen Überschreibungen gegen den
Basiscommit fest. Drei fremde Accessibility-Dateien gehören zum lokalen
Buildgraphen; es wird kein sauberer Checkout behauptet.

`references.json.gz` schützt die tatsächlich verwendeten Originalvorlagen und
die weiterhin verbindliche Exportentscheidung. `manifest.json` schützt alle
eingefrorenen Belegdateien außer dieser README und dem Manifest selbst.
Praktische Screenreader, physische Eingabe-/DPI-/Monitorprüfungen, macOS und
vollständige Pixelgleichheit der Gesamtoberfläche bleiben offen.
