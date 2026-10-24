# Native Inspektorbereiche – 24. Oktober 2026

Die [Umsetzung](../INSPECTOR-TAIL-NATIVE.md) betrifft Laufgrenzen, Ereignisse und
Aufzeichnung. Die vollständige Oberfläche einschließlich F1 ist damit nicht
abgenommen.

## Aktuelle Ergebnisse

`linux-native.json.gz` und `windows-native.json.gz` enthalten die ausgeführten
Kommandos, tatsächlichen Binärhashes, Ausgaben, Exitcodes und Laufzeiten.
Die zugehörigen `*-validation.json.gz` enthalten je zwei Schriftquellen und
30 Darstellungen plus zwei tatsächliche Leerzustände pro Schriftquelle.
Zusammen bestehen 128 native Aufnahmen; `capture-hashes.json.gz` hält ihre
Originalhashes fest. Die Layout-/Glyphenprüfung liest tatsächliche Rechtecke,
Zeichenkommandos und PNG-Pixel. Die Datei wird zusätzlich unabhängig auf ihre
zwei Messpunkte `(0.0, 142.0)` und `(0.25, 142.25)` sowie alle Chunk-CRCs geprüft.
Das explizite Logging-Testmodell ist kein physikalisches Modell.

Die internen SDL-Eingaben prüfen ungültige Bruchteile für MiB, negative Zeit,
Komma, blockierte Simulation/Analyse/Build, Korrektur über einen Ansichtswechsel
hinweg und das Öffnen des Protokolls per Tastatur. Lauf- und Quellbytes bleiben
unverändert. Grenzfelder sind während des tatsächlichen pausierten Prozesses
gesperrt. Die Statuszeile nennt das Warnungsniveau auch bei weggescrollter Karte.

Die ursprünglichen Gruppen `logging_workflow` und `project_settings_workflow`
bestehen unter Linux mit Quelle 2 und während des Nachlaufs unverändertem SDK
(`regression-sdk.json.gz`). Der Logging-Kerntest besteht auf beiden Plattformen
mit Quelle 3. Die Katalog-/Runnerprüfung besteht auf beiden Plattformen; die
zugehörigen Logs sind komprimiert. Die abschließende native Prüfung verwendet
Quelle 4 unter Windows/MSVC Release und Alpine-Dev/GCC Debug mit ASan/UBSan.

## Gegenfälle und Zwischenstände

- `before-*.png` / `baseline0.json.gz`: ursprüngliche Darstellung mit neuer
  Fixture und echten Daten. Kleine Innenfenster wiederholen dieselbe Warnung
  dreimal und schneiden ihren Inhalt ab. Grenzen nutzen generische Properties;
  Aufzeichnung zeigt den vollständigen Pfad als gewöhnliche Notiz.
- `counterexample.json.gz`: der aktuelle Prüfer weist diese alte Darstellung
  zurück. Die bloß gefärbte alte Statuszeile nennt das Warnungsniveau nicht.
- `setup-launch-failure.json.gz`: erster privater Aufruf mit relativem App-Pfad
  scheitert beim Prozessstart. Der Wiederholungsaufruf verwendet absolute Pfade;
  dies ist kein Produktfehler und kein bestandener Layoutlauf.
- `reader1-*.json.gz`: beide nativen Systemschrift-Abläufe einschließlich
  Bedienung durchlaufen erfolgreich. Der Bildleser verwechselt anschließend
  `status-warning=1` mit dem Rechteck `warning=…`. Quelle 2 verlangt vollständige
  Schlüsseltokens und besteht, ohne Geometrie- oder Pixelbedingungen zu lockern.
- `prior2-*`: beide Schriftquellen bestehen auf beiden Plattformen.
- Quelle 3 vereinheitlicht zusätzlich den Grenzeditor der weiteren nativen
  Inspektoransichten und prüft Rohtext/Korrektur nach Ansichtswechsel.
- Quelle 4 ergänzt beide Leerzustände und den Referenzhinweis „Optional“.

Die fehlgeschlagenen Gesamtfälle bleiben fehlgeschlagen archiviert. Erfolgreiche
interne App-Marker ersetzen keine bestandene unabhängige Bildprüfung.

## Provenienz und Grenzen

`source0` bis `4` enthalten LF-normalisierte Quellhashes sowie rohe Font- und
Fremdhashes. Die passenden `delta`-Dateien enthalten die öffentlichen
Überschreibungen gegenüber `157a1de5afde9bacfae4529cd2c86b42b0f9da01`.
Der letzte Stand umfasst 276 Dateien und 18 Fontdateien. Drei fremde Dateien
bleiben im lokalen Kompiliergraphen; alle zwölf fremden Änderungen sind
unverändert erhalten und nicht Bestandteil dieses Commits. Ein sauberer
Checkout wurde nicht nachgewiesen. `manifest.json` sichert 46 Belegdateien.

Die Nachweise gelten für die automatisierten nativen Abläufe. Praktische
Screenreader-Aufgaben, macOS, physische Monitor-/DPI-Wechsel, sämtliche übrigen
Arbeitsbereiche und vollständige Browser-Pixelgleichheit bleiben offen.
