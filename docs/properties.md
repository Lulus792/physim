# Material- und Medieneigenschaften als Daten

`physim/properties.h` beschreibt eine Eigenschaft durch Name, Quelle, SI-Einheit,
Gültigkeitsbereich und ein konstantes oder tabellarisches Modell. Es gibt keine
automatische Materialauswahl. Ein mechanisches Modell kann beispielsweise Dichte,
Viskosität, Reibung oder Restitution abfragen; ein thermisches oder elektrisches
Modell kann andere Eigenschaften aus denselben Datenstrukturen wählen.
Das konsumierende Modell prüft deren physikalische Bedeutung und Dimension.

## Bereich, Einheiten und Interpolation

Temperatur und Druck werden in Kelvin beziehungsweise Pascal angegeben und sind
nichtnegativ. `value_unit.scale` muss 1 sein: gespeicherte Werte sind bereits SI.
Der geschlossene Bereich `[Tmin,Tmax] × [Pmin,Pmax]` ist Teil der Eigenschaft.
Außerhalb folgt ein Fehler; die Bibliothek extrapoliert nicht. Celsiuswerte
müssen vor dem Aufruf ausdrücklich nach Kelvin umgerechnet werden.

Konstante Eigenschaften benötigen keine Tabelle. Tabellen besitzen je 1–64
streng steigende endliche Achsenpunkte und bis zu 4096 Werte. Die Anordnung ist
`values[temperature_index * pressure_count + pressure_index]`. Mehrpunktachsen
müssen den erklärten Gültigkeitsbereich abdecken. Eine Einpunktachse bezeichnet
Unabhängigkeit von dieser Koordinate im gesamten erklärten Bereich; ihr
Referenzpunkt muss darin liegen. Eine reine Temperaturabhängigkeit benötigt
beispielsweise nur einen Druckpunkt.

Die Auswertung ist bilinear, linear bei nur einer veränderlichen Achse und
konstant bei zwei Einpunktachsen. Eckwerte bleiben erhalten. Skalenfreie
Intervallgewichte und eine beschränkte konvexe Kombination vermeiden einen
unnötigen Überlauf bei entgegengesetzten Extremwerten. Der interpolierte Wert
liegt im Bereich der vier verwendeten Eckwerte. Das ist kein Beweis, dass ein
reales Material dort linear verläuft. Phasenwechsel, Hysterese, Korrelationen
und Unsicherheit werden nicht implizit erfunden.

Der C-Deskriptor leiht Arrays und Texte; sie müssen während aller Auswertungen
leben und unverändert bleiben. Jede Auswertung validiert die vollständige
Tabelle, ohne Heap-Allokation oder versteckten Cache. Gemeinsame Änderungen
benötigen externe Synchronisierung. Bei Fehlern bleibt die Ausgabe erhalten.
`PS_INVALID` bezeichnet ungültige Daten/Bereiche/Queries, `PS_LIMIT` die
Achsenbegrenzung und `PS_NUMERIC` nichtendliche Ergebnisarithmetik.

In Physim liefern `propertyConstant(name, source, value, domain, temperature,
pressure)` und `propertyTable(name, source, unit, domain, temperatures, pressures,
values, temperature, pressure)` eine `Quantity`. `domain` ist
`Vec4(Tmin,Tmax,Pmin,Pmax)`. Eigene Wertstrukturen können Name, Quelle, Einheit,
Bereich und Arrays gemeinsam besitzen; ihre Kopien bleiben unabhängig.
`tests/fixtures/language/property_values.phys` zeigt diese Struktur und abfangbare
Fehler mit `attempt`. Die beiden Funktionsbindungen sind in Programmen,
Experimenten und Analysen verfügbar.

## Vollständiger C-/Physim-Ablauf

Dieses Beispiel verwendet **synthetische** Eigenschaftsdaten. Die Dichte folgt
`ρ(T,P) = 1200 - 0.2 T + 10^-6 P` in kg/m³. Es sind keine Messwerte für Wasser
oder ein anderes benanntes Material. Eine kontrollierte T/P-Fahrt durchläuft
273–373 K und 0–500000 Pa in einer Simulationssekunde; danach bleiben die
Eingaben am Bereichsrand. Es wird kein zusätzliches Bewegungsmodell behauptet.

| Temperatur | 0 Pa | 500000 Pa |
| --- | --- | --- |
| 273 K | 1145.4 kg/m³ | 1145.9 kg/m³ |
| 373 K | 1125.4 kg/m³ | 1125.9 kg/m³ |

Die Metadaten speichern Quelle, SI-Einheit, Bereich, Achsen, sämtliche Eckwerte,
Interpolationsmethode und ausgeschlossene Modelle. Das Modell entscheidet
explizit, nur die Dichte auszuwerten. Eine Materialbibliothek kann weitere
Eigenschaften mit eigenen Quellen und Gültigkeiten hinzufügen. Die bisherigen
[Mechanik-/Medienmodelle](mechanics.md) verwenden weiterhin ihre ausdrücklich
übergebenen SI-Eigenschaften.

## C-Experiment

```c
#include "physim/experiment.h"
#include "physim/properties.h"
#include <math.h>
#include <stdio.h>
/* Synthetic material data for a controlled T/P sweep, not measured water. */
static const double temperatures[]={273,373},pressures[]={0,500000},density_values[]={1145.4,1145.9,1125.4,1125.9};
static const ps_unit density_unit={{-3,1,0,0,0,0,0},1,"kg/m3"};
static ps_property density_property(void) {
    ps_property p={.model=PS_PROPERTY_TABLE,.name="density",.source="synthetic affine reference",.value_unit=density_unit,
        .minimum_temperature_k=273,.maximum_temperature_k=373,.minimum_pressure_pa=0,.maximum_pressure_pa=500000,
        .temperature_k=temperatures,.pressure_pa=pressures,.values_si=density_values,.temperature_count=2,.pressure_count=2};return p;
}
static ps_result measure(ps_context *c,double time) {
    double fraction=fmin(1,time),temperature=273+100*fraction,pressure=500000*fraction;
    ps_property property=density_property();ps_quantity density;
    ps_result r=ps_property_evaluate(&property,temperature,pressure,&density);
    if(r==PS_OK){c->values[0]=temperature;c->values[1]=pressure;c->values[2]=density.value;}return r;
}
static ps_result reset(ps_context *c){return measure(c,0);}
static ps_result create(ps_context *c) {
    const char *names[]={"temperature","pressure","density"};ps_unit units[]={PS_KELVIN,PS_PASCAL,density_unit};
    for(unsigned i=0;i<3;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=synthetic material property sweep\nproperty.density.source=synthetic affine reference\nproperty.density.unit=kg/m3\nproperty.domain=273..373 K; 0..500000 Pa\nproperty.axes.temperature=273,373\nproperty.axes.pressure=0,500000\nproperty.values=1145.4,1145.9,1125.4,1125.9\nproperty.interpolation=bilinear; no extrapolation\nexcluded=measured material data,phase transitions,uncertainty\n");return reset(c);
}
static ps_result step(ps_context *c,double dt){return measure(c,c->time_s+dt);}
static void scene(ps_context *c,ps_scene *out){ps_scene_add_id(out,1,PS_SPHERE,ps_v3(c->time_s,0,0),ps_v3(0,0,0),.1,0x53aeefff);}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Synthetic material property sweep",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

## C-Analyse

```c
#include "physim/report.h"
#include <stdio.h>
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *ctx=NULL;ps_dataset dataset;ps_series time,density;
    ps_result r=ps_analysis_create(prefix,0,&ctx);
    if(r==PS_OK)r=ps_analysis_open_run(ctx,input,&dataset);
    if(r==PS_OK || r==PS_RECOVERED)r=ps_dataset_series(ctx,dataset,"time",&time);
    if(r==PS_OK)r=ps_dataset_series(ctx,dataset,"density",&density);
    ps_report *report=NULL;ps_plot_handle plot;ps_plot_info info={0};
    snprintf(info.title,sizeof info.title,"Synthetic density sweep");
    ps_unit unit={{-3,1,0,0,0,0,0},1,"kg/m3"};ps_report_unit_from(PS_SECOND,&info.x_unit);ps_report_unit_from(unit,&info.y_unit);
    if(r==PS_OK)r=ps_report_create("Material property","Synthetic affine reference; bilinear interpolation; bounded SI domain",&report);
    if(r==PS_OK)r=ps_report_add_plot(report,&info,&plot);
    if(r==PS_OK)r=ps_report_add_series(report,plot,ctx,time,density,"density",PS_PLOT_LINE);
    char path[4096];snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(ctx);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Material property analysis",.run=analyze};return &api;
}
```

## Physim-Experiment

```physim
// Synthetic material data, not measured water; same three SI channels as C.
let kelvin = Unit(0,0,0,0,1,0,0,1,"K")
let pascal = Unit(-1,1,-2,0,0,0,0,1,"Pa")
let densityUnit = Unit(-3,1,0,0,0,0,0,1,"kg/m3")
let temperature = Channel("temperature",kelvin,"temperature")
let pressure = Channel("pressure",pascal,"pressure")
let density = Channel("density",densityUnit,"density")
func measure(time: Float64):
    let fraction = min(1,time)
    let t = 273 + 100*fraction
    let p = 500000*fraction
    let value = propertyTable("density","synthetic affine reference",densityUnit,
        Vec4(273,373,0,500000),[273.0,373.0],[0.0,500000.0],
        [1145.4,1145.9,1125.4,1125.9],t,p)
    temperature.sample(t)
    pressure.sample(p)
    density.sample(value.value)
func create():
    metadata("model=synthetic material property sweep\nproperty.density.source=synthetic affine reference\nproperty.density.unit=kg/m3\nproperty.domain=273..373 K; 0..500000 Pa\nproperty.axes.temperature=273,373\nproperty.axes.pressure=0,500000\nproperty.values=1145.4,1145.9,1125.4,1125.9\nproperty.interpolation=bilinear; no extrapolation\nexcluded=measured material data,phase transitions,uncertainty\n")
    reset()
func reset():
    measure(0)
func step(dt: Float64):
    measure(simulationTime()+dt)
func scene():
    sphere(Vec3(simulationTime(),0,0),0.1,0x53aeefff,1)
```

## Physim-Analyse

```physim
func analyze():
    report("Material property")
    let run = Dataset(0)
    let time = run.series("time")
    let density = run.series("density")
    density.plot(time,"Synthetic density sweep","density")
```

## Ausführen und prüfen

Lege die jeweilige Experiment- und Analysequelle als `main.c`/`analysis.c` oder
`main.phys`/`analysis.phys` in einem verwalteten Projekt ab. Die App baut das
Projekt ohne CMake. Ein Lauf mit `dt=0.25 s` und vier Schritten liefert fünf
Messzeilen einschließlich des Anfangszustands. Beide Analysen können die Läufe
beider Experimentsprachen öffnen.

`tests/test_properties.c` prüft über 10000 T/P-Paare gegen eine unabhängige affine
Formel, einen nichtquadratischen 3×2-Aufbau mit echtem T·P-Term, Einpunktachsen,
Konstanten, maximalen Tabellenumfang, Extremwerte, UTF-8 und Fehler-Rücknahme.
`tests/test_property_workflow.py` liest die tatsächlichen Laufdateien inklusive
CRC, Schema und Metadaten; ein eigener Reportprüfer kontrolliert alle vier
C-/Physim-Auswertungskombinationen. Die vollständigen Quellen oben werden mit
dem Repository-Katalog byteweise abgeglichen.

[Funktionsreferenz](reference/properties.md) · [Einheiten](numerics.md)
