# Mehrere Läufe in der Physim-Sprache auswerten

Dieses Beispiel fasst die Endpositionen mehrerer gespeicherter Läufe zusammen.
Es verarbeitet C- und Physim-Experimente gemeinsam, solange beide einen
Messkanal `position.x` in Metern enthalten. Du erhältst ein Diagramm pro Lauf,
ein Histogramm, eine Tabelle und eine vollständige CSV-Datei. Beginne bei Bedarf
mit dem [ersten Sprach-Experiment](language-tutorial.md).

## Läufe vorbereiten

Erzeuge zwei Läufe und bewahre ihre `.psrun`-Dateien auf. Für einen Vergleich
wie im automatischen Test kannst du den [C-Wurf](projectile-tutorial.md) und
den [Physim-Wurf](projectile-tutorial.md) verwenden. Beide schreiben
`position.x`; das Beispiel setzt keinen gemeinsamen Zeitraster und keine
gleiche Laufdauer voraus. Die Reihenfolge der ausgewählten Dateien legt den
nullbasierten Index im Diagramm fest.

In der App kannst du die `.psrun`-Dateien in `runs/` deines Analyseprojekts
legen und **Läufe & Berichte** aktualisieren. Wähle unter **Messläufe** zwei bis
acht Dateien und dann **Auswahl vergleichen**. Baue zuvor `analysis.phys` mit
**F5**. [Läufe & Berichte](runs.md) erklärt Auswahl und gespeicherte Ergebnisse.

## Analysecode

Ersetze `analysis.phys` durch den folgenden Inhalt. Die getestete Quelldatei
liegt unter `examples/language/analysis_batch_endpoints.phys`.

```physim
// Combine C and Physim run results with the same channel and unit.
func analyze():
    report("Horizontal endpoints across runs")
    let one = Unit(0, 0, 0, 0, 0, 0, 0, 1, "1")
    let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
    var indices: [Float64] = []
    var endpoints: [Float64] = []
    for index in 0..<inputCount():
        let run = Dataset(index)
        let position = run.series("position.x")
        if position.count() > 0:
            indices.append(Float64(index))
            endpoints.append(position.value(position.count() - 1))
        run.close()
    assert(endpoints.count > 0, message: "No usable position.x endpoints")
    let x = Series.fromValues(indices, one, "run")
    let y = x.alignedValues(endpoints, metres, "position.x endpoint")
    let plot = y.plot(x, "Endpoint comparison", "position.x")
    plot.export("endpoints")
    y.export(x, "endpoints")
    let distribution = y.histogram("Endpoint distribution", 8)
    distribution.export("distribution")
    let summary = Table("Batch summary", ["Count", "Mean endpoint"], [one, metres])
    summary.row("selected runs", [Quantity(Float64(y.count()), one),
        Quantity(y.mean(), metres)])
    summary.export("summary")
```

`inputCount()` zählt die ausgewählten Dateien. `Dataset(index)` öffnet jeweils
einen Lauf; `run.close()` schließt ihn nach dem Lesen. Leere Positionreihen
werden ausgelassen. Wenn keine Reihe einen Endwert liefert, meldet `assert`
die Quellposition und den angegebenen Text. Ein fehlender Kanal oder eine
beschädigte Laufdatei erzeugt ebenfalls einen Quellfehler. Die Analyse wird
dann ohne fertigen Bericht beendet.

`Series.fromValues` legt eine neue Indexachse an. `x.alignedValues` bindet die
Endwerte an genau diese Achse; so können Plot und CSV beide Reihen gemeinsam
verwenden. Die originale Zeitachse jedes Laufs wird hier bewusst nicht
interpoliert. Die Tabelle zählt nur verwendete Endwerte und berechnet ihren
Mittelwert in Metern. Histogramm und Tabelle sind beschreibende Statistik;
ein Konfidenzintervall liefert dieses Beispiel nicht.

## Ergebnis prüfen

Mit dem C-Referenzlauf aus dem Sprach-Analysetest und dem Physim-Wurf liefert
die Auswahl in genau dieser Reihenfolge Endwerte `1 m` und `0 m`. Das
Indexdiagramm enthält also `(0, 1)` und `(1, 0)`, die Tabelle Anzahl `2` und
Mittelwert `0,5 m`. Der automatisierte Test prüft diese Werte sowie zwei
Diagramme, eine Tabelle und die erzeugte CSV.

Auf der Kommandozeile verwendet derselbe Test die Form:

```text
physim-analysis-runner ANALYSEMODUL --runs AUSGABEPRAEFIX C_LAUF.psrun SPRACH_LAUF.psrun
```

`AUSGABEPRAEFIX` bezeichnet den Anfang der Ergebnisdateien, etwa
`vergleich.psreport` und `vergleich-endpoints.csv`. Die Elternverzeichnisse
müssen existieren; die Ausgabedateien dürfen noch nicht vorhanden sein.
`python3 tools/build.py --no-app --test --test-filter language_analysis` baut das
Modul im Quellbaum und prüft den gemischten C-/Sprachlauf samt Bericht
(Windows: `python`). Das Modul heißt im Buildordner
`bin/integration-physim-language-analysis-batch-endpoints.so` (Windows: `.dll`).
Für die allgemeine Form von
`Dataset`, `Series`, `Plot` und `Table` siehe die
[Sprachbibliotheksreferenz](reference/language-library.md).
