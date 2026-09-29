# Ein eigenes Experiment in der Physim-Sprache

Ziel: Eine Kugel bewegt sich ohne Kräfte und Kollisionen mit 1,5 m/s entlang
der X-Achse. Nach einer Sekunde erwarten wir 1,5 m, nach zwei Sekunden 3 m.
Aus den gespeicherten Positionen berechnen wir anschließend die Geschwindigkeit
und zeigen sie als Diagramm. Das [C-Tutorial](experiment-tutorial.md) verwendet
dasselbe Modell und dieselben Referenzwerte.

## Projekt vorbereiten

Lege in der App ein **Pendel · Physim-Sprache**-Projekt in einem neuen Ordner an
und wähle für die Auswertung ebenfalls **Physim-Sprache**. Ersetze den gesamten
Inhalt von `main.phys` und `analysis.phys` durch die folgenden beiden Beispiele.
Die Originalquellen liegen unter `examples/documentation/language_main.phys`
und `examples/documentation/language_analysis.phys`. Physim pflegt die
`physim.project` und baut beide Dateien mit F5 nativ; du musst keinen C-Code schreiben.

## Experimentcode

```physim
// Uniform motion: 1.5 m/s along X, without forces or collisions.
let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
let position = Channel("position.x", metres, "Position along X")
var x: Float64 = 0

func create():
    metadata("model=uniform motion\nvelocity_m_s=1.5\nintegrator=exact")

func reset():
    x = 0
    position.sample(x)

func step(dt: Float64):
    x += 1.5 * dt
    position.sample(x)

func scene():
    sphere(Vec3(x, 0, 0), 0.15, 1688924159, 1)
```

`let` bindet unveränderliche Werte, `var` einen veränderlichen Zustand.
`Unit` beschreibt die Längeneinheit; `Channel` legt den Messkanal mit seinem
Namen und seiner Einheit an. `create` schreibt Modellmetadaten. `reset` setzt
den Zustand auf null und erfasst den Anfangswert. Der Runner ruft `step` pro
Zeitschritt mit `dt` in Sekunden auf; `x += 1.5 * dt` ist für konstante
Geschwindigkeit exakt. Danach schreibt `position.sample(x)` den Messwert.
`scene` zeichnet eine Kugel mit stabiler Objekt-ID `1` an der aktuellen
Position. Blöcke beginnen mit `:` und verwenden Einrückung statt Klammern.

Der Beispielcode hält nur skalaren Zustand. Für Arrays und Strukturen gelten
Wertkopien: Eine Änderung an einer Kopie verändert das Original nicht.
[Typen und Speicherlebensdauer](language-values.md) erklärt die Regeln.

## Auswertungscode

```physim
func analyze():
    report("Uniform motion")
    let run: Dataset = Dataset(0)
    let time: Series = run.series("time")
    let position: Series = run.series("position.x")
    let velocity: Series = position.derivative(time)
    let plot: Plot = velocity.plot(time, "Velocity", "dx/dt")
    velocity.export(time, "velocity")
    run.close()
```

`Dataset(0)` öffnet den ersten ausgewählten Lauf. `run.series` liest Zeit und
Position. `derivative(time)` bildet daraus eine Geschwindigkeitsreihe;
`plot` fügt sie dem Bericht hinzu und `export` schreibt die zugehörigen
vollständigen Daten. `run.close()` gibt den Dataset-Handle frei. Die
[Sprachbibliotheksreferenz](reference/language-library.md) listet die
verfügbaren Dataset-, Series- und Plot-Funktionen.

## Bauen, laufen lassen und prüfen

1. Drücke **F5**. Der Build prüft zuerst Syntax und Typen der beiden `.phys`-Dateien und übersetzt sie anschließend in native Module.
2. Wechsle zu **Simulieren** und starte mit **F6**. Stoppe nach mindestens zwei simulierten Sekunden.
3. Öffne **Auswerten**, wähle den neuen Lauf und starte die Analyse. Im Diagramm liegt die abgeleitete Geschwindigkeit bei 1,5 m/s.
4. Vergleiche die Positionswerte bei einer und zwei Sekunden mit 1,5 m und 3 m. Der Lauf bleibt im Projektordner unter `runs/` erhalten.

Im Quellbaum baut `python3 tools/build.py --no-app --test --test-filter
documentation_language_example` dieselben Module und führt den vollständigen
Referenztest aus (Windows: `python`): 200 Schritte
mit `dt = 0.01` und Seed 42, 201 gespeicherte Positionen, Reset, Szene und
Bericht. Der Test prüft die analytische Position und die abgeleitete
Geschwindigkeit gegen 1,5 m/s. Die C-Fassung durchläuft denselben Test.

## Fehler finden und erweitern

Ändere testweise `x += 1.5 * dt` zu `x += "schnell" * dt` und drücke **F5**.
Der Compiler meldet den Typfehler an der `.phys`-Quellposition; ein fehlerhaftes
Modul wird nicht ausgeführt. Stelle danach die ursprüngliche Zeile wieder her.
Fehler zur Laufzeit enthalten ebenfalls Datei, Zeile und Spalte. In der App
zeigt das Build-/Laufprotokoll die Diagnose; [Fehler finden](troubleshooting.md)
beschreibt die weiteren Schritte.

Für konstante Beschleunigung ergänze einen Geschwindigkeitszustand `v` und
verwende im Schritt `x += v * dt + 0.5 * a * dt * dt` sowie `v += a * dt`.
Bei variablen Kräften wähle einen passenden Integrator und prüfe das Ergebnis
mit halbiertem Zeitschritt. [Numerische Verfahren](numerics.md) erklärt die
Fehlerabschätzung; [Mechanik und Kontakte](mechanics.md) zeigt Kräfte,
Reibung und Gelenke. Für eigene Messkanäle und Sensoren helfen
[Einheiten und Messungen](measurement.md).

Als nächster Schritt zeigt die [Mehrlaufanalyse](language-analysis-tutorial.md),
wie du gespeicherte C- und Physim-Läufe mit einem einzigen Sprachmodul
vergleichst und die Ergebnisse exportierst.
