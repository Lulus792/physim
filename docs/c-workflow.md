# C-Einstieg für den Physim-Workflow

[Teil I – C](c-guide.md) · [Nächster Schritt: Experiment und Analyse](experiment-tutorial.md)

Dieser Einstieg benötigt nur C17. Er berechnet E = m v² / 2 in SI und zeigt die
Konzepte, die du danach in einem Physim-Modul verwendest. Für m=2 kg und v=3 m/s
sind es 9 J; für die beiden Geschwindigkeiten 3 und 4 m/s liegt der Mittelwert
bei 12,5 J. Es gibt hier keine Simulation, Sensoren oder Rundungsannahmen über
unbekannte Messdaten.

## Vollständiges Programm

Die Quelle liegt unter `examples/documentation/c_workflow.c`.

```c
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
typedef struct {double mass_kg,speed_m_s;} motion;
static double kinetic_energy(motion value){return .5*value.mass_kg*value.speed_m_s*value.speed_m_s;}
/* Failure preserves caller output, like checked Physim operations. */
static bool positive(double value,double *out){if(!out || !isfinite(value) || value<=0)return false;*out=value;return true;}
int main(void){
    motion state={2,3};double energy=kinetic_energy(state);
    double speeds[]={3,4},total=0;
    for(unsigned i=0;i<2;i++){motion sample=state;sample.speed_m_s=speeds[i];total+=kinetic_energy(sample);}
    motion copy=state;copy.speed_m_s=5;
    double selected=7;bool found=positive(-1,&selected);
    if(energy!=9 || total/2!=12.5 || state.speed_m_s!=3 || copy.speed_m_s!=5 || found || selected!=7)return 1;
    printf("Energy: %.1f J\nMean: %.1f J\n",energy,total/2);return 0;
}
```

## Was die C-Konstrukte bedeuten

`double` beschreibt die hier verwendeten Gleitkommazahlen, `bool` ein logisches
Ergebnis. `motion` gruppiert Masse und Geschwindigkeit in einer Struktur;
`state` besitzt ihren Speicher auf dem Stack. `kinetic_energy` erhält eine
Wertkopie. Eine Änderung an `copy` verändert deshalb `state` nicht. Ein Array
hält mehrere Werte desselben Typs; die Schleife verwendet ausschließlich seine
beiden gültigen Indizes. C prüft diese Grenzen nicht automatisch.

`positive` erhält einen Pointer auf Speicher des Aufrufers. `*out=value`
schreibt dorthin; vorher werden Pointer und Wert geprüft. Bei einem ungültigen
Wert bleibt `selected` unverändert. Physim verwendet bei geprüften Operationen
statt Bool einen [ps_result](reference/core.md), damit du den Fehler unterscheiden
kannst. Prüfe den Rückgabecode, bevor du einen Ausgabeparameter verwendest.

## Lebensdauer und Ressourcen

Diese lokalen Strukturen und Arrays brauchen kein `malloc` und kein `free`.
Ein Experimentzustand muss dagegen mehrere Runner-Aufrufe überleben: Das
[Experimenttutorial](experiment-tutorial.md) allokiert ihn in create, setzt ihn
in reset zurück und gibt ihn in destroy frei. Es gibt nie eine Garantie, dass
ein lokaler Pointer nach Rückkehr aus seiner Funktion noch gültig ist.
Dataset, Series und Plot haben zusätzliche Owner-/Generationsregeln;
[Speicher und Allocator](memory.md), [Datenreihen](series.md) und
[Berichte](reports.md) erklären sie getrennt.

Zahlen sind hier durch Feldnamen ausdrücklich SI. Für eine Messung mit
Anzeigeeinheiten benutzt du [Unit und Quantity](reference/units.md). Eine Skala
wie cm ist keine Änderung der Dimension. Eine erfolgreiche Zahlenoperation
beweist kein korrektes physikalisches Modell; prüfe Annahmen und Referenzwerte.

## Ausführen und Fehler finden

Im Repository baut und prüft derselbe automatisierte Fall das Programm:

```sh
python3 tools/build.py --no-app --test --test-filter c_workflow_intro
```

Unter Windows `python` verwenden. Erwartete Ausgabe:

```text
Energy: 9.0 J
Mean: 12.5 J
```

Aus einem installierten SDK lässt sich dieses kleine, von der Bibliothek
unabhängige Programm unter Linux/macOS direkt prüfen:

```sh
cc -std=c17 "$PWD/build/SDK/examples/documentation/c_workflow.c" -lm -o intro
./intro
```

Unter Windows mit den eingerichteten C++ Build Tools erzeugt
`cl /std:c17 /TC c_workflow.c /Fe:intro.exe` das Programm. Der Projektbuilder
übernimmt bei den folgenden Physim-Modulen zusätzlich SDK-Includes und Core.

Zum Üben ändere eine Geschwindigkeit und berechne E vorher auf Papier. Ein
fehlendes Semikolon ist ein Compilerfehler; ein ungültiger Arrayindex ist in C
undefiniertes Verhalten und braucht einen passenden Test/Sanitizer. Einen
fehlgeschlagenen Physim-Aufruf behandelst du über seinen Rückgabecode.
[Fehlersuche](troubleshooting.md), [Sanitizer-Befehle](build.md).

[Weiter: das vollständige C-Experiment](experiment-tutorial.md) · [Teil I – C](c-guide.md)
