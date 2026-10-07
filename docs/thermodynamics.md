# Thermodynamik: ideales Gas und Wärmefluss

Sprachvertrag 0.179.0 ergänzt dieselben allokationsfreien SI-Funktionen für C und
Physim. [C-Referenz](reference/thermodynamics.md), [Sprachbindungen](reference/language-library.md).
Die Gasgleichung verwendet R=8,31446261815324 J/(mol K), die double-Darstellung
des exakten Produkts der definierten SI-Konstanten k und N_A.
[NIST-Konstanten](https://physics.nist.gov/cgi-bin/cuu/Value?r).

## Vertrag und Modellgrenzen

Absolute Temperatur wird in Kelvin eingegeben, nicht in Celsius. Stoffmenge n
ist in mol, Volumen V in m³ und Druck p in Pa. Diese Größen, Masse, spezifische
und gesamte Wärmekapazität sind endlich und strikt positiv. Leitwert G in W/K
und Zeitintervall dt in s sind endlich und nichtnegativ. SI-Einheiten werden
an Kanälen und Parametern gespeichert; nackte Zahlen tragen keine Dimension.

Für ein ideales Gas gilt pV=nRT. Bei konstantem molarem cv gilt U=n cv T mit
gewählter Referenz U=0 bei T=0. Zwischen zwei Gleichgewichtszuständen derselben
Stoffmenge ist ΔS=n[cv ln(T1/T0)+R ln(V1/V0)] in J/K. Dies ist eine Zustandsdifferenz,
keine Schätzung irreversibler Entropieproduktion und kein Wärmepfadintegral.
Gültig ist das verdünnte ideale Gas mit konstantem cv, ohne Phasenübergang.
Reale Gase, temperaturabhängige Stofftabellen und Strahlung sind noch offen.

Ein homogener Körper mit konstanter spezifischer Wärmekapazität c hat C=mc und
benötigt Q=C(T1-T0). Für lineare Wärmeleitung von A nach B gilt P=G(Ta-Tb).
Bei einer homogenen Platte setzt das Modell ausdrücklich G=kA/L aus seinen
Materialdaten. Die Bibliothek erfindet keinen Stoff und keinen Leitwert.

Ein gut durchmischter Körper an einem Reservoir fester Temperatur Tr folgt
T(t+dt)=Tr+(T(t)-Tr)exp(-Gdt/C). Zwei isolierte Körper mit konstanten Kapazitäten
nähern sich Teq=(Ca Ta0+Cb Tb0)/(Ca+Cb) mit Rate G(1/Ca+1/Cb).
`thermalPairStep` liefert beide Endtemperaturen als Vec2; x ist A, y ist B.
Die exakte Exponentiallösung hat keine Zeitschritt-Stabilitätsgrenze.
Räumliche Temperaturgradienten benötigen ein anderes Modell; CFD/FEM bleiben
eigenständige Projekte. Absolute Energie hier bezeichnet C*T mit gewähltem
Nullpunkt und darf nicht als kalorimetrisch gemessene Gesamtenergie verstanden werden.

Alle C-Funktionen geben bei ungültigen Eingaben PS_INVALID und bei nicht
repräsentierbaren Ergebnissen PS_NUMERIC zurück; Ausgaben bleiben erhalten.
Ein erforderlicher positiver Wert, der zu null unterläuft, ist PS_NUMERIC.
Signierte Wärme, Leistung und Entropiedifferenzen dürfen auf null runden.
Physim liefert bei denselben Fehlern eine Quelldiagnose; `attempt(...)` fängt sie ab.
Die Bindungen funktionieren in Standalone-, Experiment- und Analysecode.

## Lernziel und Ablauf

Zwei isolierte Körper beginnen bei 400 K und 300 K. Ca=100 J/K, Cb=300 J/K und
G=5 W/K liefern Teq=325 K und Rate 1/15 s⁻¹. Nach 15 s erwarten wir
Ta=352,59095808785815 K und Tb=315,80301397071394 K. Die Bilanz bleibt
130000 J. Körper A enthält n=Ca/cv=8 mol ideales Gas bei cv=12,5 J/(mol K)
und festem V=0,1 m³. Druck und Gasenergie sinken mit seiner Temperatur.
Die Höhe der zwei dargestellten Kugeln codiert Temperatur, nicht Bewegung.

1. Neues C- oder Physim-Projekt anlegen und die vollständige Experimentquelle
   unten einsetzen. Die Analysequelle derselben oder der anderen Sprache wählen.
2. F5 speichert und baut. Ca, Cb, Anfangstemperaturen und G sind SI-Parameter.
3. Mit F6 30 s simulieren, zum Beispiel 200 Schritte zu 0,15 s. Stoppen finalisiert
   den Lauf. Analyse starten erzeugt vier Diagramme, eine Bilanzprüfung und CSV.
4. G=0 bewahrt die Anfangstemperaturen; vertauschte Anfangstemperaturen kehren
   die Flussrichtung um. Gleiche Anfangstemperaturen erzeugen keine Leistung.
5. 30 Schritte zu 1 s müssen dieselben Endwerte wie 200 Schritte zu 0,15 s
   liefern, bis auf Rundung. Die reine API erlaubt auch einen 30-s-Schritt;
   der Offline-Runner begrenzt dt auf 1 s. Vergleiche die Bilanz, nicht nur die Kurvenform.

[Projektbedienung](workspace.md), [Fehlersuche](troubleshooting.md),
[Teil I – C](c-guide.md), [Teil II – Physim](physim-guide.md).
Das mitgelieferte Standalone-Programm `examples/language/thermodynamics.phys`
zeigt zusätzlich Gasinversion, Energie, Entropie und abgefangene Fehler.

## Experiment in C

```c
#include "physim/experiment.h"
#include "physim/thermodynamics.h"
#include "physim/units.h"
#include <stdio.h>
#include <stdlib.h>
/* Isolated well-mixed bodies, constant capacities; gas A at fixed volume. */
typedef struct { double ca,cb,g,ta0,tb0,n,cv,volume;ps_vec2 temperatures; } thermal;
static ps_result measure(ps_context *c,ps_vec2 temperatures) {
    thermal *s=c->user;double power=0,pressure=0,energy=0;
    ps_result r=ps_heat_flow(s->g,temperatures.x,temperatures.y,&power);
    if(r==PS_OK)r=ps_ideal_gas_pressure(s->n,temperatures.x,s->volume,&pressure);
    if(r==PS_OK)r=ps_ideal_gas_energy(s->n,s->cv,temperatures.x,&energy);
    double values[]={temperatures.x,temperatures.y,power,pressure,energy,
                    s->ca*temperatures.x+s->cb*temperatures.y};
    if(r==PS_OK)for(unsigned i=0;i<6;i++)c->values[i]=values[i];
    return r;
}
static ps_result reset(ps_context *c) {
    thermal *s=c->user;ps_vec2 initial={s->ta0,s->tb0};
    ps_result r=measure(c,initial);if(r==PS_OK)s->temperatures=initial;return r;
}
static ps_result create(ps_context *c) {
    thermal *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    ps_unit capacity={{2,1,-2,0,-1,0,0},1,"J/K"};
    ps_unit conductance={{2,1,-3,0,-1,0,0},1,"W/K"};
    ps_unit watt={{2,1,-3,0,0,0,0},1,"W"};
    ps_result r=ps_parameter_define_unit(c,"capacityA","Gas A heat capacity in J/K",capacity,100,1,10000,&s->ca);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"capacityB","Body B heat capacity in J/K",capacity,300,1,10000,&s->cb);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"conductance","Constant coupling in W/K",conductance,5,0,10000,&s->g);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"temperatureA","Initial A temperature in K",PS_KELVIN,400,1,10000,&s->ta0);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"temperatureB","Initial B temperature in K",PS_KELVIN,300,1,10000,&s->tb0);
    s->cv=12.5;s->n=s->ca/s->cv;s->volume=.1;
    const char *names[]={"temperature.a","temperature.b","heat.power","gas.pressure","gas.energy","energy.balance"};
    ps_unit units[]={PS_KELVIN,PS_KELVIN,watt,PS_PASCAL,PS_JOULE,PS_JOULE};
    for(unsigned i=0;r==PS_OK && i<6;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)r=PS_LIMIT;
    if(r!=PS_OK)return r;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=isolated constant-capacity thermal pair\nunits=Kelvin,SI\nintegrator=exact exponential\ngas=ideal; fixed volume 0.1 m^3; cv=12.5 J/(mol K); n=capacityA/cv\nexcluded=spatial gradients,radiation,latent heat,real gas\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    thermal *s=c->user;ps_vec2 next;
    ps_result r=ps_thermal_pair_step(s->ca,s->temperatures.x,s->cb,s->temperatures.y,s->g,dt,&next);
    if(r==PS_OK)r=measure(c,next);if(r==PS_OK)s->temperatures=next;return r;
}
static void scene(ps_context *c,ps_scene *out) {
    thermal *s=c->user;
    ps_vec3 a=ps_v3(-.6,(s->temperatures.x-300)*.005,0),b=ps_v3(.6,(s->temperatures.y-300)*.005,0);
    ps_scene_add_id(out,1,PS_SPHERE,a,a,.2,0xf2a052ff);
    ps_scene_add_id(out,2,PS_SPHERE,b,b,.2,0x53aeefff);
    ps_scene_add_id(out,3,PS_LINE,a,b,.015,0xc8d7eaff);
}
static void destroy(ps_context *c) { free(c->user);c->user=NULL; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Ideal gas and isolated thermal exchange",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

## Experiment in Physim

```physim
// Same model and six SI channels as thermal_main.c.
let kelvin = Unit(0,0,0,0,1,0,0,1,"K")
let capacityUnit = Unit(2,1,-2,0,-1,0,0,1,"J/K")
let conductanceUnit = Unit(2,1,-3,0,-1,0,0,1,"W/K")
let watt = Unit(2,1,-3,0,0,0,0,1,"W")
let pascal = Unit(-1,1,-2,0,0,0,0,1,"Pa")
let joule = Unit(2,1,-2,0,0,0,0,1,"J")
let ca = parameterWithUnit("capacityA",capacityUnit,100,1,10000,"Gas A heat capacity in J/K")
let cb = parameterWithUnit("capacityB",capacityUnit,300,1,10000,"Body B heat capacity in J/K")
let conductance = parameterWithUnit("conductance",conductanceUnit,5,0,10000,"Constant coupling in W/K")
let ta = parameterWithUnit("temperatureA",kelvin,400,1,10000,"Initial A temperature in K")
let tb = parameterWithUnit("temperatureB",kelvin,300,1,10000,"Initial B temperature in K")
let temperatureA = Channel("temperature.a",kelvin,"temperature.a")
let temperatureB = Channel("temperature.b",kelvin,"temperature.b")
let power = Channel("heat.power",watt,"heat.power")
let pressure = Channel("gas.pressure",pascal,"gas.pressure")
let energy = Channel("gas.energy",joule,"gas.energy")
let balance = Channel("energy.balance",joule,"energy.balance")
var temperatures = Vec2(400,300)
func measure():
    temperatureA.sample(temperatures.x)
    temperatureB.sample(temperatures.y)
    power.sample(heatFlow(conductance,temperatures.x,temperatures.y))
    pressure.sample(idealGasPressure(ca/12.5,temperatures.x,0.1))
    energy.sample(idealGasEnergy(ca/12.5,12.5,temperatures.x))
    balance.sample(ca*temperatures.x+cb*temperatures.y)
func create():
    metadata("model=isolated constant-capacity thermal pair\nunits=Kelvin,SI\nintegrator=exact exponential\ngas=ideal; fixed volume 0.1 m^3; cv=12.5 J/(mol K); n=capacityA/cv\nexcluded=spatial gradients,radiation,latent heat,real gas\n")
    reset()
func reset():
    temperatures = Vec2(ta,tb)
    measure()
func step(dt: Float64):
    temperatures = thermalPairStep(ca,temperatures.x,cb,temperatures.y,conductance,dt)
    measure()
func scene():
    let a = Vec3(-0.6,(temperatures.x-300)*0.005,0)
    let b = Vec3(0.6,(temperatures.y-300)*0.005,0)
    sphere(a,0.2,0xf2a052ff,1)
    sphere(b,0.2,0x53aeefff,2)
    line(a,b,0.015,0xc8d7eaff,3)
```

## Analyse in C

```c
#include "physim/report.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_dataset dataset={0};ps_dataset_info info={0};
    ps_series series[7]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&dataset);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,dataset,&info);
    const char *names[]={"time","temperature.a","temperature.b","heat.power","gas.pressure","gas.energy","energy.balance"};
    for(unsigned i=0;r==PS_OK && i<7;i++)r=ps_dataset_series(c,dataset,names[i],&series[i]);
    if(r==PS_OK)r=ps_report_create("Ideal gas and thermal exchange","Constant capacities; isolated energy balance",&report);
    ps_unit watt={{2,1,-3,0,0,0,0},1,"W"};
    ps_unit units[]={PS_KELVIN,watt,PS_PASCAL,PS_JOULE};
    const char *titles[]={"Temperatures","Heat flow","Gas pressure","Energy accounting"};
    unsigned indices[]={1,3,4,5},counts[]={2,1,1,2};
    for(unsigned p=0;r==PS_OK && p<4;p++) {
        ps_plot_info plot={0};snprintf(plot.title,sizeof plot.title,"%s",titles[p]);
        strcpy(plot.x_label,"Time");snprintf(plot.y_label,sizeof plot.y_label,"%s",titles[p]);
        r=ps_report_unit_from(PS_SECOND,&plot.x_unit);
        if(r==PS_OK)r=ps_report_unit_from(units[p],&plot.y_unit);
        ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&plot,&handle);
        for(unsigned j=0;r==PS_OK && j<counts[p];j++)r=ps_report_add_series(report,handle,c,series[0],series[indices[p]+j],names[indices[p]+j],PS_PLOT_LINE);
    }
    double initial=0;size_t count=0;ps_statistics balance={0};
    if(r==PS_OK)r=ps_series_read(c,series[6],0,&initial,1,&count);
    if(r==PS_OK && count!=1)r=PS_INVALID;
    if(r==PS_OK)r=ps_series_statistics(c,series[6],&balance);
    ps_table_info table={0};strcpy(table.title,"Conservation checks");table.columns=2;
    strcpy(table.column[0].label,"Samples");strcpy(table.column[1].label,"Maximum energy drift");
    if(r==PS_OK)r=ps_report_unit_from(PS_ONE,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_JOULE,&table.column[1].unit);
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"complete run");row.values[0]=(double)info.samples;
    row.values[1]=fmax(fabs(balance.min-initial),fabs(balance.max-initial));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-thermal.csv",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,series,7,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Ideal gas and thermal exchange analysis",.run=analyze};return &api;
}
```

## Analyse in Physim

```physim
func analyze():
    assert(inputCount() == 1,"Select exactly one thermal run")
    report("Ideal gas and thermal exchange")
    let run = Dataset(0)
    let time = run.series("time")
    let a = run.series("temperature.a")
    let b = run.series("temperature.b")
    let power = run.series("heat.power")
    let pressure = run.series("gas.pressure")
    let energy = run.series("gas.energy")
    let balance = run.series("energy.balance")
    let temperatures = a.plot(time,"Temperatures","temperature.a")
    temperatures.curve(time,b,"temperature.b")
    power.plot(time,"Heat flow","heat.power")
    pressure.plot(time,"Gas pressure","gas.pressure")
    let energies = energy.plot(time,"Energy accounting","gas.energy")
    energies.curve(time,balance,"energy.balance")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let joules = Unit(2,1,-2,0,0,0,0,1,"J")
    let initial = balance.value(0)
    let drift = max(abs(balance.minimum()-initial),abs(balance.maximum()-initial))
    let summary = Table("Conservation checks",["Samples","Maximum energy drift"],[one,joules])
    summary.row("complete run",[Quantity(Float64(time.count()),one),Quantity(drift,joules)])
    Series.exportColumns([time,a,b,power,pressure,energy,balance],"thermal")
    run.close()
```

## Unabhängige Prüfung

```sh
python3 tools/build.py --config Release --test --test-filter thermodynamics --test-filter 'language_*thermodynamics*' --test-filter thermal_tutorial --test-filter documentation_thermal_source
```

Der Tutorialprüfer verwendet eine 65-stellige Decimal-Exponentiallösung,
sechs Szenarien, sämtliche Messwerte, Szenengeometrie und 24 gemischte Analysen.
Er prüft CRC, Footer und SI-Dimensionen unabhängig vom Core, vergleicht CSV mit
Rohdaten und lädt jeden Bericht erneut. SDK-Prüfungen wiederholen diese Abläufe
gegen installierten und neu gebauten Core. Tatsächlich ausgeführte Systeme
stehen in der [Plattformprüfung](platform-validation.md).
