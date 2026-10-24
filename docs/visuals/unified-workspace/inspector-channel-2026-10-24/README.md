# Native Messkanalansicht – 24. Oktober 2026

Die [Umsetzung](../INSPECTOR-CHANNEL-NATIVE.md) betrifft die B-Karten für
Messkanal/Einheit und den Messwert zur betrachteten Zeit. Gesamtoberfläche,
macOS, praktische Screenreader-Aufgaben und physische DPI-Wechsel bleiben offen.

## Abschließende Ausführungen

| Plattform | Quelle 3 | Ergebnis |
| --- | --- | --- |
| Windows/MSVC Release | `run-0wg0igey` | `inspector_channel` und `inspector_design` bestanden |
| Alpine-Dev/GCC, ASan/UBSan | `run-__v806lk` | dieselben beiden Gruppen bestanden |

`*-native.json.gz` enthalten die tatsächlich ausgeführten Befehle, Binärhashes,
Ausgaben und Exitcodes. `*-validation.json.gz` halten die Rechteck-, Text- und
Pixelprüfung für beide Schriftquellen fest. `capture-hashes.json.gz` erfasst
60 Messkanalbilder; `inspector-capture-hashes.json.gz` weitere 205 Aufnahmen des
vorhandenen Inspektorfalls (102 Windows, 103 Linux einschließlich eines
zusätzlichen Scrollnachweises). Ausgewählte originale PNGs sind direkt beigefügt.
Es wurden keine Bilder umgefärbt, retuschiert oder skaliert.

Die C-/Physim-Einheitenabläufe (`regression2-*.json.gz`) verwenden Quelle 2:
Windows `run-z_mj1ug2`, Linux `run-qo0t68zu`. Sie prüfen reale Persistenz,
beschädigte Ablage, ausdrückliches Zurücksetzen und unveränderte Rohdateien.
Die komprimierten Runnerlogs belegen die zusätzliche Katalog-/Runnerprüfung.
Linux-UI-Läufe verwenden Xvfb/Openbox und Software-OpenGL; Leakprüfung ist wegen
dieser Grafiklaufzeit deaktiviert. ASan und UBSan sind davon getrennt aktiv.

## Gegenfälle und verworfene Vorbereitung

- `baseline.json.gz` / `before-windows.png`: die ursprüngliche native Ansicht
  mit kleinem Kanalwähler, zusätzlichem „Anzeigeeinheit …“-Knopf und fehlendem
  großen Inspektorwert; tatsächlicher laufender Pendelversuch.
- Quelle 1 lädt in der neuen Fixture die isolierte Einheitenablage noch nicht.
  Der identifizierte Windows-Testprozess wurde deshalb in Stufe 9 beendet.
  `aborted-fixture1-windows.json.gz` bleibt fehlgeschlagen; daraus wird keine
  Produkt- oder Layoutabnahme abgeleitet.
- Quelle 2 initialisiert diese Ablage und besteht die eigentlichen nativen
  Bedienabläufe. Der Bildleser weist beide Plattformläufe dennoch zurück:
  Die Fixture wendet vor dem Speichern eines Bilds bereits das nächste Theme
  und die nächste Größe an. `rejected-capture2-light-value.png` zeigt den
  Widerspruch zur protokollierten hellen 1440-px-Ansicht. Die Serien bleiben
  fehlgeschlagen archiviert.
- Quelle 3 schaltet erst im folgenden Testschritt um. Die bestehenden
  Pixelbedingungen werden nicht gelockert; die Konturprüfung verwendet
  zusätzlich die tatsächlichen Palettenfarben mit engerer Toleranz.
  Die Abschnittsüberschrift übernimmt außerdem das originale „&“.

## Provenienz

Basis: `67dcdb14fe5a4b4b673557b2153374f38e99e433`. `source1..3.json.gz`
erfassen jeweils 279 Quell-/Fixture-/Prüfdateien, 18 Schriftressourcen und
die Rohhashes von zwölf unverändert vorgefundenen fremden Dateien.
`delta1..3.json.gz` rekonstruieren die eigenen Änderungen jedes Prüfstands
gegen diesen Basiscommit. Die rekonstruierten Textbytes wurden gegen die
jeweiligen Quellhashes geprüft. Drei fremde Accessibility-Dateien gehören zum
lokalen Buildgraphen; dies ist ausdrücklich kein Clean-Checkout-Nachweis.

`references.json.gz` hält die unveränderten Originalreferenzen fest.
`manifest.json` schützt sämtliche eingefrorenen Belegdateien außer dieser
README und dem Manifest selbst. Die weiterhin aktiven Build-/Testverzeichnisse
enthalten vollständige Rohdaten und werden nicht pauschal bereinigt.
