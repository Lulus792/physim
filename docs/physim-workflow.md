# Physim-Einstieg: Syntax, Typen und Werte

[Teil II – Physim](physim-guide.md) · [Nächster Schritt: Experiment und Analyse](language-tutorial.md)

Die Physim-Sprache prüft statische Typen vor nativer Ausführung. Dieser Einstieg
berechnet dieselben 9 J und 12,5 J wie der [C-Einstieg](c-workflow.md). Er benötigt
keinen Experimenthost; die Zahlen sind ausdrücklich SI. Die vollständige Quelle
liegt unter `examples/documentation/physim_workflow.phys`.

## Vollständiges Programm

```physim
struct Motion:
    let mass: Float64
    var speed: Float64

func kineticEnergy(mass: Float64, speed: Float64) -> Float64:
    return 0.5 * mass * speed * speed

func positive(value: Float64) -> Float64?:
    if value > 0:
        return Optional.some(value)
    return nil

let mass: Float64 = 2.0
var state = Motion(mass: mass, speed: 3.0)
let original = state
state.speed = 5.0
let speeds = [3.0,4.0]
var total = 0.0
for speed in speeds:
    total += kineticEnergy(mass: mass, speed: speed)
let energy = kineticEnergy(mass: original.mass, speed: original.speed)
assert(energy == 9.0 && total / 2 == 12.5)
assert(original.speed == 3.0 && state.speed == 5.0)
let selected: Float64? = positive(-1)
assert(selected == nil && positive(4).unwrap() == 4)
print(energy)
print(total / 2)
```

## Inferenz und explizite Typen

`let mass: Float64=2.0` benennt den Typ ausdrücklich. Bei `let speeds=[3.0,4.0]`
folgt `[Float64]` aus den Elementen. `let` verhindert Änderungen an der Bindung;
`var` erlaubt sie. Das gilt auch für Felder: `Motion.mass` ist unveränderlich,
`Motion.speed` veränderlich. Der Funktionskopf benennt die Parameter- und
Rückgabetypen. Argumente dürfen positional oder mit den gezeigten Labels stehen.
Ein Bool als Geschwindigkeit ist ein Typfehler und wird vor Ausführung abgewiesen.

Blöcke beginnen mit `:` und sind eingerückt. `for speed in speeds` bindet in jeder
Iteration ein Float64-Element; `total += ...` liest, berechnet und ersetzt den
alten Wert einmal. Die Sprache verwendet Bool-Bedingungen, `&&`/`||`, `if` und
`else`. Der [Sprachvertrag](language.md) beschreibt die implementierten Operatoren,
Schleifen, Module, Methoden und Generics einschließlich ihrer Grenzen.

## Wertkopien und optionale Ergebnisse

`original=state` bewahrt einen unabhängigen Strukturwert. Die spätere Änderung
von `state.speed` ändert die gespeicherte Kopie nicht. Arrays besitzen ebenfalls
Wertsemantik, auch bei Strukturfeldern und Closures. [Werte und Lebensdauer](language-values.md)
erklärt die automatische Verwaltung und die begrenzten Speicherbudgets.

`Float64?` ist ein optionaler Float64-Wert. `positive(-1)` liefert nil;
`positive(4)` einen Wert. `unwrap()` verlangt einen vorhandenen Wert und erzeugt
sonst eine Quellfehlermeldung. Prüfe das Ergebnis, nutze optionales Binding oder
`??`, wenn ein fachlich sinnvolles Ersatzverhalten möglich ist. Fehlende Messungen
haben in Physim ausdrücklich ihren Messstatus; ein Ersatzwert ist keine neue Messung.

## Bauen und ausführen

Im Quellrepository prüft dieser Befehl das vollständige Programm:

```sh
python3 tools/build.py --no-app --test --test-filter language_native_physim_workflow_intro
```

Unter Windows `python` verwenden. Erwartete Ausgabe:

```text
9
12.5
```

Mit einem installierten SDK kannst du unter Linux/macOS denselben Quelltext
ohne Umschreiben in C übersetzen und ausführen:

```sh
sdk="$PWD/build/SDK"
"$sdk/bin/physimc" --emit-c "$sdk/examples/documentation/physim_workflow.phys" > intro.c
cc -std=c17 -I "$sdk/include" intro.c "$sdk/lib/libphysim-core.a" -lm -o intro
./intro
```

`intro.c` ist generiert; bearbeite ausschließlich die `.phys`-Quelle.
Windows-Compiler und nativer Modulworkflow stehen in der
[Buildanleitung](build.md). Das SDK-Prüfkit führt diesen Einstieg separat gegen
die ausgelieferten Header und Bibliothek aus.

`physimc --check DATEI.phys` prüft eine eigene Datei ohne Ausführung.
`--emit-c` erzeugt C17 für ein eigenständiges Programm, `--emit-experiment` und
`--emit-analysis` erzeugen die jeweiligen Module. Der normale Projektbuilder
übernimmt diese Aufrufe; C-Ausgabe und Objektdateien bleiben unter `build/`.
[Einrichtung und vollständige Compilerbefehle](build.md).

## Fehler, Hostbindung und nächster Schritt

Ein Typfehler hält den Build vor Ausführung an. Ein ungültiger Index, ein
Zahlenüberlauf oder `unwrap()` auf nil liefert eine Laufzeitdiagnose mit
Quellposition. `attempt(expression)` kann solche Wertausdrücke abfangen und
liefert dann nil; es nimmt bereits sichtbare Seiteneffekte nicht zurück.
[Diagnosen](diagnostics.md), [Fehlersuche](troubleshooting.md).

Dieses Programm enthält keine Pointer und keine manuellen Freigaben. Die
späteren Analysehandles gehören aber ihrem Context, und explizites `close` oder
`release` kann sie ungültig machen. Die [Bibliotheksreferenz](reference/language-library.md)
beschreibt den jeweiligen Besitzer und welche Funktionen einen Host benötigen.
`Channel`, Szenen- und Messaufrufe gehören zum Experiment, `Dataset` und `report`
zur Analyse. Noch geplante Syntax darf nicht als bereits verfügbare Funktion
benutzt werden; maßgeblich sind Sprachvertrag und Compilerprüfung.

[Weiter: das vollständige Physim-Experiment](language-tutorial.md) · [Teil II – Physim](physim-guide.md)
