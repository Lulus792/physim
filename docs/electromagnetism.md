# Elektromagnetismus: Ladungen, Felder und RC-Schaltungen

Physim 0.180.0 bindet dieselben allokationsfreien SI-Funktionen wie C.
[C-Referenz](reference/electromagnetism.md), [Sprachbindungen](reference/language-library.md).
Permittivität ist Modelldatum, das jede Punktladungsfunktion ausdrücklich erhält.
`PS_VACUUM_PERMITTIVITY` und `vacuumPermittivity()` liefern den empfohlenen
CODATA-2022-Wert 8,8541878188·10⁻¹² F/m mit Standardunsicherheit 1,4·10⁻²¹ F/m;
dies ist eine gemessene Größe, keine exakte Konstante.
[NIST: Vakuumpermittivität](https://www.physics.nist.gov/cgi-bin/cuu/Value?eqep0=).

## SI-Vertrag und Grenzen

Punktladung q in C, Position in m und positive Permittivität ε in F/m ergeben
E=q(r-r0)/(4π ε |r-r0|³) in V/m und Potential φ=q/(4π ε |r-r0|) in V, mit
Nullpunkt im Unendlichen. Der Quellpunkt ist singulär, auch bei q=0. Es gibt
kein verborgenes Softening. Mehrere Ladungen werden ausdrücklich superponiert.
Gültig ist ein homogenes, isotropes, unendliches Medium im elektrostatischen
Modell. Grenzflächen, Abschirmung, retardierte Felder und räumliche Maxwell-
Integration sind keine Eigenschaften dieses Modells.

Die Lorentzkraft F=q(E+v×B) verwendet vorgegebene elektrische Felder in V/m,
magnetische Felder in T und nichtrelativistische Geschwindigkeit in m/s.
Das magnetische Glied verrichtet an der idealen Punktladung keine Arbeit:
v·(v×B)=0. Die Funktion erzeugt keine Felder und integriert keine Bewegung.
Strahlung, Selbstkraft und relativistische Dynamik benötigen eigene Modelle.

Ideale lineare Widerstände verwenden I=V/R, V=IR und P=V²/R. R in ohm ist
strikt positiv. Zwei Widerstände lassen sich in Serie oder parallel kombinieren.
Es gibt keinen impliziten Solver beliebiger Netze. Ein idealer Kondensator
mit positiver Kapazität C in F speichert U=½CV² in J. Spannung und Strom sind
signiert. Für eine konstante Quelle Vs im Serienkreis gilt
Vc(t+dt)=Vs+(Vc(t)-Vs)exp(-dt/(RC)). Dieser exakte Schritt hat keine
Stabilitätsgrenze; dt≥0 und dt=0 erhält den Zustand. Induktivität, parasitäre
Elemente, nichtlineare Bauteile und Quellenwechsel innerhalb eines Schritts
sind ausgeschlossen. Der Offline-Runner begrenzt dt gesondert auf 1 s.

Alle Argumente müssen endlich sein. Ungültige Eingaben geben PS_INVALID,
Singularität PS_SINGULAR und nicht darstellbare Ergebnisse PS_NUMERIC;
C-Ausgaben bleiben bei jedem Fehler erhalten. Signierte Größen dürfen auf
null runden. Normalisierte Produkte vermeiden Zwischenüberlauf. Physim
liefert Quelldiagnosen, die `attempt(...)` innerhalb eines Wertausdrucks abfängt.
Die Bindungen sind in Standalone-, Experiment- und Analysecode verfügbar.

## Lernziel und Ablauf

Der Serienkreis verwendet R=1000 ohm, C=0,002 F, Vs=12 V und V0=0 V.
Die Zeitkonstante ist 2 s. Nach einer Zeitkonstante erwarten wir
Vc=7,585446705942692 V. Bei vollständigem Laden enthält der Kondensator
0,144 J; weitere 0,144 J werden im Widerstand dissipiert. Die Quelle liefert
insgesamt 0,288 J. W_source=Vs*C*(Vc-V0) ist positiv für Energie in den Kreis.
U+Q-W_source bleibt gleich der Anfangsenergie. Eine absorbierende Quelle kann
negative Arbeit liefern; Widerstandsverluste bleiben physikalisch nichtnegativ.
Die Wärme folgt dem exakten Integral von I²R; expm1 erhält dabei kleine
Zeitintervalle ohne Auslöschung. Ein unabhängiger Prüfer kontrolliert sämtliche
Werte mit einer hochpräzisen Lösung.

1. Neues C- oder Physim-Projekt anlegen und die vollständige Experimentquelle
   unten einsetzen. Für die Analyse kann jede der beiden Sprachen gewählt werden.
2. F5 speichert und baut. Widerstand, Kapazität und beide Spannungen sind SI-Parameter.
3. Mit F6 zehn Sekunden simulieren, zum Beispiel 200 Schritte zu 0,05 s; Stoppen
   finalisiert den Lauf. Analyse starten liefert vier Diagramme, Bilanz und CSV.
4. Vs=0 V und V0=12 V entlädt den Kondensator. V0=Vs erzeugt weder Strom noch Wärme.
   Vs=-12 V bei V0=12 V zeigt den Wechsel der Polarität und signierte Quellenarbeit.
5. Zehn Schritte zu 1 s müssen dieselben Endwerte wie 200 Schritte zu 0,05 s liefern,
   bis auf Rundung. Verkleinern von RC ändert die Zeitkonstante, nicht die Stabilität.

Die Höhe der linken Kugel codiert Spannung und stellt keine Teilchenbahn dar.
Punktladungsfeld und Lorentzkraft werden getrennt im mitgelieferten Standalone-
Programm `examples/language/electromagnetism.phys` gezeigt; das RC-Modell behauptet
kein elektrostatisches Feld eines realen Kondensators aus einer einzelnen Ladung.
[Projektbedienung](workspace.md), [Fehlersuche](troubleshooting.md),
[Teil I – C](c-guide.md), [Teil II – Physim](physim-guide.md).

## Experiment in C

```c
#include "physim/experiment.h"
#include "physim/electromagnetism.h"
#include "physim/units.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
/* Constant-source series RC, ideal linear components; all values in SI. */
typedef struct { double resistance,capacitance,source,initial,voltage,initial_energy; } circuit;
static ps_result measure(ps_context *c,double voltage,double elapsed) {
    circuit *s=c->user;double current=0,power=0,energy=0;
    ps_result r=ps_resistor_current(s->source-voltage,s->resistance,&current);
    if(r==PS_OK)r=ps_resistor_power(s->source-voltage,s->resistance,&power);
    if(r==PS_OK)r=ps_capacitor_energy(s->capacitance,voltage,&energy);
    double rate=elapsed/(s->resistance*s->capacitance);
    double difference=s->initial-s->source;
    double work=s->source*s->capacitance*(-difference)*(-expm1(-rate));
    double heat=.5*s->capacitance*difference*difference*(-expm1(-2*rate));
    double values[]={voltage,current,power,energy,heat,work,energy+heat-work};
    if(r==PS_OK)for(unsigned i=0;i<7;i++)c->values[i]=values[i];
    return r;
}
static ps_result reset(ps_context *c) {
    circuit *s=c->user;ps_result r=measure(c,s->initial,0);if(r==PS_OK)s->voltage=s->initial;return r;
}
static ps_result create(ps_context *c) {
    circuit *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    ps_unit ohm={{2,1,-3,-2,0,0,0},1,"ohm"},farad={{-2,-1,4,2,0,0,0},1,"F"};
    ps_unit volt={{2,1,-3,-1,0,0,0},1,"V"},watt={{2,1,-3,0,0,0,0},1,"W"};
    ps_result r=ps_parameter_define_unit(c,"resistance","Series resistance in ohm",ohm,1000,1,1e6,&s->resistance);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"capacitance","Capacitance in farad",farad,.002,1e-6,1,&s->capacitance);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"sourceVoltage","Constant supply in V",volt,12,-1000,1000,&s->source);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"initialVoltage","Initial capacitor voltage in V",volt,0,-1000,1000,&s->initial);
    if(r==PS_OK)r=ps_capacitor_energy(s->capacitance,s->initial,&s->initial_energy);
    const char *names[]={"voltage","current","power.resistor","energy.capacitor","energy.dissipated","energy.source","energy.balance"};
    ps_unit units[]={volt,PS_AMPERE,watt,PS_JOULE,PS_JOULE,PS_JOULE,PS_JOULE};
    for(unsigned i=0;r==PS_OK && i<7;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)r=PS_LIMIT;
    if(r!=PS_OK)return r;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=constant-source series RC\nunits=SI\nintegrator=exact exponential\nenergy_reference=zero capacitor energy at zero voltage\nsource_work=Vs*C*(V-V0); positive into circuit\nheat=exact integral of I^2*R; stable expm1\nexcluded=inductance,parasitics,nonlinear components,source switching\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    circuit *s=c->user;double next;
    ps_result r=ps_rc_voltage_step(s->resistance,s->capacitance,s->voltage,s->source,dt,&next);
    if(r==PS_OK)r=measure(c,next,c->time_s+dt);if(r==PS_OK)s->voltage=next;return r;
}
static void scene(ps_context *c,ps_scene *out) {
    circuit *s=c->user;ps_vec3 a=ps_v3(-.6,s->voltage*.03,0),b=ps_v3(.6,0,0);
    ps_scene_add_id(out,1,PS_SPHERE,a,a,.2,0xf2a052ff);
    ps_scene_add_id(out,2,PS_SPHERE,b,b,.2,0x53aeefff);
    ps_scene_add_id(out,3,PS_LINE,a,b,.015,0xc8d7eaff);
}
static void destroy(ps_context *c) { free(c->user);c->user=NULL; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Series RC: voltage and energy",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

## Experiment in Physim

```physim
// Same constant-source RC model and seven SI channels as rc_main.c.
let ohm = Unit(2,1,-3,-2,0,0,0,1,"ohm")
let farad = Unit(-2,-1,4,2,0,0,0,1,"F")
let volt = Unit(2,1,-3,-1,0,0,0,1,"V")
let ampere = Unit(0,0,0,1,0,0,0,1,"A")
let watt = Unit(2,1,-3,0,0,0,0,1,"W")
let joule = Unit(2,1,-2,0,0,0,0,1,"J")
let resistance = parameterWithUnit("resistance",ohm,1000,1,1e6,"Series resistance in ohm")
let capacitance = parameterWithUnit("capacitance",farad,0.002,1e-6,1,"Capacitance in farad")
let source = parameterWithUnit("sourceVoltage",volt,12,-1000,1000,"Constant supply in V")
let initial = parameterWithUnit("initialVoltage",volt,0,-1000,1000,"Initial capacitor voltage in V")
let initialEnergy = capacitorEnergy(capacitance,initial)
let voltageChannel = Channel("voltage",volt,"voltage")
let currentChannel = Channel("current",ampere,"current")
let powerChannel = Channel("power.resistor",watt,"power.resistor")
let energyChannel = Channel("energy.capacitor",joule,"energy.capacitor")
let heatChannel = Channel("energy.dissipated",joule,"energy.dissipated")
let sourceChannel = Channel("energy.source",joule,"energy.source")
let balanceChannel = Channel("energy.balance",joule,"energy.balance")
var voltage = 0.0
var elapsed = 0.0
func measure():
    let energy = capacitorEnergy(capacitance,voltage)
    let rate = elapsed/(resistance*capacitance)
    let difference = initial-source
    let work = source*capacitance*(-difference)*(-expm1(-rate))
    let heat = 0.5*capacitance*difference*difference*(-expm1(-2*rate))
    voltageChannel.sample(voltage)
    currentChannel.sample(resistorCurrent(source-voltage,resistance))
    powerChannel.sample(resistorPower(source-voltage,resistance))
    energyChannel.sample(energy)
    heatChannel.sample(heat)
    sourceChannel.sample(work)
    balanceChannel.sample(energy+heat-work)
func create():
    metadata("model=constant-source series RC\nunits=SI\nintegrator=exact exponential\nenergy_reference=zero capacitor energy at zero voltage\nsource_work=Vs*C*(V-V0); positive into circuit\nheat=exact integral of I^2*R; stable expm1\nexcluded=inductance,parasitics,nonlinear components,source switching\n")
    reset()
func reset():
    voltage = initial
    elapsed = 0
    measure()
func step(dt: Float64):
    voltage = rcVoltageStep(resistance,capacitance,voltage,source,dt)
    elapsed += dt
    measure()
func scene():
    let a = Vec3(-0.6,voltage*0.03,0)
    let b = Vec3(0.6,0,0)
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
    ps_series series[8]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&dataset);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,dataset,&info);
    const char *names[]={"time","voltage","current","power.resistor","energy.capacitor","energy.dissipated","energy.source","energy.balance"};
    for(unsigned i=0;r==PS_OK && i<8;i++)r=ps_dataset_series(c,dataset,names[i],&series[i]);
    if(r==PS_OK)r=ps_report_create("Series RC: voltage and energy","Constant-source circuit; capacitor and Joule heat",&report);
    ps_unit watt={{2,1,-3,0,0,0,0},1,"W"};
    ps_unit volt={{2,1,-3,-1,0,0,0},1,"V"};
    ps_unit units[]={volt,PS_AMPERE,watt,PS_JOULE};
    const char *titles[]={"Capacitor voltage","Circuit current","Resistor power","Energy accounting"};
    unsigned indices[]={1,2,3,4},counts[]={1,1,1,4};
    for(unsigned p=0;r==PS_OK && p<4;p++) {
        ps_plot_info plot={0};snprintf(plot.title,sizeof plot.title,"%s",titles[p]);
        strcpy(plot.x_label,"Time");snprintf(plot.y_label,sizeof plot.y_label,"%s",titles[p]);
        r=ps_report_unit_from(PS_SECOND,&plot.x_unit);
        if(r==PS_OK)r=ps_report_unit_from(units[p],&plot.y_unit);
        ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&plot,&handle);
        for(unsigned j=0;r==PS_OK && j<counts[p];j++)r=ps_report_add_series(report,handle,c,series[0],series[indices[p]+j],names[indices[p]+j],PS_PLOT_LINE);
    }
    double initial=0;size_t count=0;ps_statistics balance={0};
    if(r==PS_OK)r=ps_series_read(c,series[7],0,&initial,1,&count);
    if(r==PS_OK && count!=1)r=PS_INVALID;
    if(r==PS_OK)r=ps_series_statistics(c,series[7],&balance);
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
    n=snprintf(path,sizeof path,"%s-rc.csv",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,series,8,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Series RC: voltage and energy analysis",.run=analyze};return &api;
}
```

## Analyse in Physim

```physim
func analyze():
    assert(inputCount() == 1,"Select exactly one RC run")
    report("Series RC: voltage and energy")
    let run = Dataset(0)
    let time = run.series("time")
    let voltage = run.series("voltage")
    let current = run.series("current")
    let power = run.series("power.resistor")
    let energy = run.series("energy.capacitor")
    let heat = run.series("energy.dissipated")
    let work = run.series("energy.source")
    let balance = run.series("energy.balance")
    voltage.plot(time,"Capacitor voltage","voltage")
    current.plot(time,"Circuit current","current")
    power.plot(time,"Resistor power","power.resistor")
    let energies = energy.plot(time,"Energy accounting","energy.capacitor")
    energies.curve(time,heat,"energy.dissipated")
    energies.curve(time,work,"energy.source")
    energies.curve(time,balance,"energy.balance")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let joules = Unit(2,1,-2,0,0,0,0,1,"J")
    let initial = balance.value(0)
    let drift = max(abs(balance.minimum()-initial),abs(balance.maximum()-initial))
    let summary = Table("Conservation checks",["Samples","Maximum energy drift"],[one,joules])
    summary.row("complete run",[Quantity(Float64(time.count()),one),Quantity(drift,joules)])
    Series.exportColumns([time,voltage,current,power,energy,heat,work,balance],"rc")
    run.close()
```

## Unabhängige Prüfungen

```sh
python3 tools/build.py --config Release --test --test-filter electromagnetism --test-filter 'language_*electromagnetism*' --test-filter rc_tutorial --test-filter documentation_rc_source
```

Der Coreprüfer kontrolliert Feld/Potentialgradient, Lorentzvorzeichen und Arbeit,
Schaltungsgesetze, exakte Zeitkomposition, numerische Extreme und erhaltene
Ausgaben. Der Tutorialprüfer verwendet 65-stellige Decimal-Lösungen für sieben
Szenarien und kontrolliert alle Messwerte, Einheiten, Szenen, CSV und 28 gemischte
C-/Physim-Analysen. SDK-Prüfungen wiederholen die Modelle gegen installierten
und neu gebauten Core. Tatsächlich ausgeführte Systeme stehen in der
[Plattformprüfung](platform-validation.md).
