# Strömung: Rohrnetze und passiver Tracer

Physim 0.182.0 ergänzt dieselben SI-Funktionen wie C für laminare Rohre,
passive stationäre Drucknetze und periodischen Tracertransport.
[C-Referenz](reference/fluid.md), [Sprachbindungen](reference/language-library.md).
Dies sind ausdrücklich Lehrmodelle. CFD und FEM bleiben entsprechend dem
Projektplan eigene größere Vorhaben.

## Rohre, Stoffdaten und Gültigkeit

Für ein starres gerades kreiszylindrisches Rohr mit Radius r, Länge L und
dynamischer Viskosität μ gilt Hagen-Poiseuille:
G=πr⁴/(8μL), Q=G(pa-pb), P_loss=G(pa-pb)².
G ist in m³/(s Pa), Q in m³/s und Leistung in W. Drücke dürfen signierte
Relativdrücke sein; positive Q fließt von A nach B. Radius, Länge und Viskosität
sind positiv und endlich; G=0 schließt eine Kante. Das Modell setzt vollständig
entwickelte laminare inkompressible Newtonsche Strömung voraus. Eintritt,
Bögen, Turbulenz, elastische Wände und Kompressibilität fehlen.
[MIT: Rohrwiderstand und Poiseuille](https://ocw.mit.edu/courses/hst-542j-quantitative-physiology-organ-transport-systems-spring-2004/5384df8b0795c292af407d5cfb4a3106_fluid_mechanics.pdf).

Re=ρ|v|d/μ prüft Dichte in kg/m³, Geschwindigkeit in m/s, Durchmesser in m und
Viskosität in Pa s. Die Bibliothek wählt keinen universellen Übergangswert;
das Tutorial verlangt konservativ Re≤1000. Stoffwerte sind ausdrücklich
Modelldaten. Hydrostatik p=p0+ρgh verwendet positive Dichte, g≥0 und signierte
Tiefe nach unten. Das Rohrnetz berücksichtigt keine Höhenenergie automatisch.

## Passives Netz und Erhaltung

`pipeNetwork` löst bis zu 16 Knoten und 32 Kanten. Jede freie Knotengleichung
verlangt Σ Q_out=0; feste Knoten liefern Drücke. Int64-Arrays beschreiben
Start-/Endknoten, ein Float64-Array Leitwerte, ein Int64-Array 0/1-Fixflags und
ein Float64-Array Druckwerte. Rückgabe: zuerst N Knotendrücke, danach E
A→B-Flüsse in Eingabereihenfolge. Alle Rückgabearrays besitzen ihren Speicher.
Parallelkanten sind erlaubt. Jede durch positive G verbundene Komponente
benötigt einen festen Knoten. Ein unbestimmtes oder numerisch schlecht
aufgelöstes Netz meldet eine Singularität, nicht erfundene Drücke.

C verwendet caller-eigene Ausgaben und begrenzten Stack; Druck- und Flussausgabe
müssen voneinander getrennt sein, dürfen aber Eingabespeicher überlappen.
Nur Fixdrücke werden gelesen, freie Platzhalter bleiben ohne Bedeutung.
Globale Leitwertskalierung und Druckskalierung je Verbindungskomponente
begrenzen Zwischenzahlen. Wenn ein positiver Leitwert oder ein von null verschiedener
Fixdruck durch seine Skalierung verschwindet, ist das PS_NUMERIC. Getrennte
Komponenten dürfen unterschiedliche Druckgrößen besitzen. Es gibt keine Pumpe,
Trägheit, kompressible Speicher oder automatische Drucktransienten.

## Konservativer periodischer Transport

`transportStep` berechnet einen passiven Tracer c in kg/m³ auf 3–4096 gleich
breiten periodischen Zellen: c_t+v c_x=D c_xx. v ist konstant und signiert,
D≥0 in m²/s, dx>0 in m und dt≥0 in s. Upwind-Advektion erster Ordnung und
zentrierte explizite Diffusion verwenden die gemeinsame Grenze
|v|dt/dx+2Ddt/dx²≤1. Dann ist der Schritt konvex: nichtnegative Konzentrationen,
Masse und Maximumprinzip bleiben bis auf Rundung erhalten. Konstante Profile
werden exakt erhalten; dt=0 kopiert den Zustand. Numerische Upwind-Diffusion
wird nicht als physikalische Diffusivität ausgegeben.
[MITgcm: Advektionsverfahren und numerische Diffusion](https://mitgcm.readthedocs.io/en/latest/algorithm/adv-schemes.html).

C übernimmt erst ein vollständig gültiges Ergebnis aus 32-KiB-Stackscratch
und erlaubt Input-/Output-Aliasing. Physim erzeugt ein neues besitzendes Array.
Alle Fehler erhalten C-Ausgaben; Allokationsfehler oder verworfene Ergebnisse
gibt die Sprachbindung vollständig frei. Ungültige Werte/Stabilität sind
PS_INVALID, Größenlimits PS_LIMIT und numerische Bereiche PS_NUMERIC.
`attempt(...)` fängt entsprechende Quelldiagnosen ab.
Dies ist kein Solver der Fluidimpulse, keine Reaktion und kein offener Tracerrand.

## Lernziel und Ablauf

Drei gleiche Rohre verbinden 0→1, 1→2 und 1→3. p0=100 Pa, p2=p3=0 Pa sind
fest; p1 ist frei. Daher gilt p1=100/3 Pa und Q01=Q12+Q13 mit gleichen Teilströmen.
r=0,005 m, L=1 m und μ=0,01 Pa s liefern G≈2,45436926·10⁻⁸ m³/(s Pa),
Q01≈1,63624617·10⁻⁶ m³/s und mittlere Geschwindigkeit 1/48 m/s.
Mit ρ=1000 kg/m³ ist Re≈20,8333.

Diese mittlere Geschwindigkeit wird als vorgegebene Geschwindigkeit für einen
**separaten periodischen Tracer** über 1 m benutzt. Das ist kein Tracerabfluss
im verzweigten Rohrnetz. 64 Zellen beginnen bei 0,2+0,1 cos(2πx) kg/m³;
D=0,0001 m²/s. Die Kontinuumsreferenz ist
0,2+0,1 cos(2π(x-vt)) exp(-D(2π)²t).
Der Integrationswert ∫c dx ist Masse pro Querschnittsfläche in kg/m² und bleibt
0,2 kg/m². Ein unabhängiger Fourierfaktor beschreibt den diskreten Schritt;
Abweichung zur Kontinuumskurve zeigt seine numerische Diffusion.

1. Neues C- oder Physim-Projekt anlegen, die vollständige Experimentquelle
   unten einsetzen und jede der beiden Analysesprachen wählen. F5 baut.
2. F6 mit dt=0,05 s starten, 200 Schritte ergeben zehn Sekunden; Stoppen
   finalisiert den Lauf. Analyse starten liefert vier Diagramme, Massenbilanz und CSV.
3. Inletdruck umkehren: alle Kantenströme und die Tracerbewegung kehren ihre Richtung um.
   Druck null lässt nur Diffusion zurück; D=0 isoliert die Upwind-Advektion.
4. Radius oder Viskosität verändern. Das Tutorial weist Re>1000 und verletzte
   Transportstabilität ab, statt die laminare Gültigkeit zu behaupten.
5. Die Polylinie zeichnet alle 64 Zellen plus periodischen Endpunkt auf.
   Ihre Höhe ist eine Visualisierungsskala 1 m pro (kg/m³), keine zweite Raumachse.
   Netzpfeile zeigen das Flussvorzeichen; bei null erscheinen Linien.

Mitgeliefertes Standalone-Programm `examples/language/fluid_values.phys`
zeigt zusätzlich Hydrostatik, 4096 Zellen, unabhängige Besitzer und abgefangene Fehler.
[Projektbedienung](workspace.md), [Fehlersuche](troubleshooting.md),
[Teil I – C](c-guide.md), [Teil II – Physim](physim-guide.md).

## Experiment in C

```c
#include "physim/experiment.h"
#include "physim/fluid.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CELLS 64
/* Passive steady pipe split prescribes velocity for a separate periodic tracer.
 * This is not a coupled Navier-Stokes branch flow or an open-boundary tracer. */
typedef struct {
    double radius,pipe_length,viscosity,density,inlet,diffusion,conductance;
    double pressures[4],flows[3],velocity,reynolds,concentration[CELLS];
} transport;
static ps_result measure(ps_context *c,const double *profile,double time,double dt) {
    transport *s=c->user;double dx=1.0/CELLS,mass=0;
    for(unsigned j=0;j<CELLS;j++)mass+=profile[j]*dx;
    double k=2*PS_PI,reference=.2+.1*cos(k*(.5-s->velocity*time))*exp(-s->diffusion*k*k*time);
    double stability=fabs(s->velocity)*dt/dx+2*s->diffusion*dt/(dx*dx);
    double values[]={profile[CELLS/2],reference,profile[CELLS/2]-reference,mass,s->flows[0],s->pressures[1],s->reynolds,stability};
    for(unsigned j=0;j<8;j++)if(!isfinite(values[j]))return PS_NUMERIC;
    for(unsigned j=0;j<8;j++)c->values[j]=values[j];return PS_OK;
}
static ps_result reset(ps_context *c) {
    transport *s=c->user;
    for(unsigned j=0;j<CELLS;j++)s->concentration[j]=.2+.1*cos(2*PS_PI*j/CELLS);
    return measure(c,s->concentration,0,c->dt_s);
}
static ps_result create(ps_context *c) {
    transport *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    ps_unit viscosity={{-1,1,-1,0,0,0,0},1,"Pa s"},density={{-3,1,0,0,0,0,0},1,"kg/m^3"};
    ps_unit diffusivity={{2,0,-1,0,0,0,0},1,"m^2/s"};
    ps_result r=ps_parameter_define_unit(c,"radius","Identical pipe radius in m",PS_METRE,.005,.0001,.01,&s->radius);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"pipeLength","Each pipe length in m",PS_METRE,1,.1,10,&s->pipe_length);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"viscosity","Newtonian viscosity in Pa s",viscosity,.01,.001,1,&s->viscosity);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"density","Fluid density for Reynolds in kg/m^3",density,1000,1,10000,&s->density);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"inletPressure","Signed inlet gauge pressure in Pa",PS_PASCAL,100,-100,100,&s->inlet);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"diffusivity","Tracer diffusivity in m^2/s",diffusivity,.0001,0,.0005,&s->diffusion);
    if(r==PS_OK)r=ps_pipe_conductance(s->radius,s->pipe_length,s->viscosity,&s->conductance);
    uint8_t fixed[]={1,0,1,1};double boundary[]={s->inlet,0,0,0};
    ps_pipe_edge edges[]={{0,1,s->conductance},{1,2,s->conductance},{1,3,s->conductance}};
    if(r==PS_OK)r=ps_pipe_network_solve(fixed,boundary,4,edges,3,s->pressures,s->flows);
    if(r==PS_OK)s->velocity=s->flows[0]/(PS_PI*s->radius*s->radius);
    if(r==PS_OK)r=ps_reynolds_number(s->density,s->velocity,2*s->radius,s->viscosity,&s->reynolds);
    if(r==PS_OK && s->reynolds>1000)r=PS_INVALID;
    double dx=1.0/CELLS;
    if(r==PS_OK && (!isfinite(c->dt_s) || c->dt_s<=0 || fabs(s->velocity)*c->dt_s/dx+2*s->diffusion*c->dt_s/(dx*dx)>1))r=PS_INVALID;
    if(r!=PS_OK)return r;
    ps_unit mass_area={{-2,1,0,0,0,0,0},1,"kg/m^2"},flow={{3,0,-1,0,0,0,0},1,"m^3/s"};
    const char *names[]={"concentration.center","reference.center","error.center","mass.perArea","flow.inlet","pressure.junction","reynolds","stability"};
    ps_unit units[]={density,density,density,mass_area,flow,PS_PASCAL,PS_ONE,PS_ONE};
    for(unsigned j=0;j<8;j++)if(ps_channel_add(c,names[j],units[j],names[j])!=(int)j)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=passive three-pipe split plus separate periodic tracer\nintegrator=upwind advection and explicit centered diffusion\nnetwork=fixed nodes 0,2,3; free node 1; edges 0->1,1->2,1->3\ntransport=64 periodic cells over 1 m; mean velocity from inlet pipe\nvalidity=Re<=1000; abs(v)*dt/dx+2*D*dt/dx^2<=1\nexcluded=turbulence,momentum evolution,compressibility,open tracer boundaries,CFD\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    transport *s=c->user;double next[CELLS];
    ps_result r=ps_transport_periodic_step(s->concentration,CELLS,s->velocity,s->diffusion,1.0/CELLS,dt,next);
    if(r==PS_OK)r=measure(c,next,c->time_s+dt,dt);
    if(r==PS_OK)memcpy(s->concentration,next,sizeof next);return r;
}
static void scene(ps_context *c,ps_scene *out) {
    transport *s=c->user;ps_vec3 profile[CELLS+1];
    /* Visual height scale 1 m per (kg/m^3), not a second spatial dimension. */
    for(unsigned j=0;j<=CELLS;j++)profile[j]=ps_v3((double)j/CELLS-.5,s->concentration[j%CELLS],0);
    ps_scene_polyline_id(out,1,profile,CELLS+1,.002,0x53aeefff);
    ps_vec3 nodes[]={ps_v3(-.6,-.4,0),ps_v3(-.2,-.4,0),ps_v3(.3,-.2,0),ps_v3(.3,-.6,0)};
    unsigned a[]={0,1,1},b[]={1,2,3};
    for(unsigned e=0;e<3;e++) {
        ps_vec3 from=nodes[s->flows[e]>=0?a[e]:b[e]],to=nodes[s->flows[e]>=0?b[e]:a[e]];
        ps_scene_add_id(out,e+2,s->flows[e]==0?PS_LINE:PS_ARROW,from,to,.012,0xf2a052ff);
    }
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Pipe network and periodic tracer",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

## Experiment in Physim

```physim
// Same passive network and separate periodic tracer as transport_main.c.
let metre = Unit(1,0,0,0,0,0,0,1,"m")
let pascal = Unit(-1,1,-2,0,0,0,0,1,"Pa")
let viscosityUnit = Unit(-1,1,-1,0,0,0,0,1,"Pa s")
let densityUnit = Unit(-3,1,0,0,0,0,0,1,"kg/m^3")
let diffusivityUnit = Unit(2,0,-1,0,0,0,0,1,"m^2/s")
let massAreaUnit = Unit(-2,1,0,0,0,0,0,1,"kg/m^2")
let flowUnit = Unit(3,0,-1,0,0,0,0,1,"m^3/s")
let one = Unit(0,0,0,0,0,0,0,1,"1")
let radius = parameterWithUnit("radius",metre,0.005,0.0001,0.01,"Identical pipe radius in m")
let length = parameterWithUnit("pipeLength",metre,1,0.1,10,"Each pipe length in m")
let viscosity = parameterWithUnit("viscosity",viscosityUnit,0.01,0.001,1,"Newtonian viscosity in Pa s")
let density = parameterWithUnit("density",densityUnit,1000,1,10000,"Fluid density for Reynolds in kg/m^3")
let inlet = parameterWithUnit("inletPressure",pascal,100,-100,100,"Signed inlet gauge pressure in Pa")
let diffusion = parameterWithUnit("diffusivity",diffusivityUnit,0.0001,0,0.0005,"Tracer diffusivity in m^2/s")
let pi = 3.14159265358979323846
let conductance = pipeConductance(radius,length,viscosity)
let network = pipeNetwork([0,1,1],[1,2,3],[conductance,conductance,conductance],[1,0,1,1],[inlet,0,0,0])
let velocity = network[4]/(pi*radius*radius)
let reynolds = reynoldsNumber(density,velocity,2*radius,viscosity)
let initialDt = simulationTimeStep()
let dx = 1.0/64.0
let centerChannel = Channel("concentration.center",densityUnit,"concentration.center")
let referenceChannel = Channel("reference.center",densityUnit,"reference.center")
let errorChannel = Channel("error.center",densityUnit,"error.center")
let massChannel = Channel("mass.perArea",massAreaUnit,"mass.perArea")
let flowChannel = Channel("flow.inlet",flowUnit,"flow.inlet")
let pressureChannel = Channel("pressure.junction",pascal,"pressure.junction")
let reynoldsChannel = Channel("reynolds",one,"reynolds")
let stabilityChannel = Channel("stability",one,"stability")
var concentration: [Float64] = []
var elapsed = 0.0
func measure(dt: Float64):
    var mass = 0.0
    for value in concentration:
        mass += value*dx
    let k = 2*pi
    let reference = 0.2+0.1*cos(k*(0.5-velocity*elapsed))*exp(-diffusion*k*k*elapsed)
    centerChannel.sample(concentration[32])
    referenceChannel.sample(reference)
    errorChannel.sample(concentration[32]-reference)
    massChannel.sample(mass)
    flowChannel.sample(network[4])
    pressureChannel.sample(network[1])
    reynoldsChannel.sample(reynolds)
    stabilityChannel.sample(abs(velocity)*dt/dx+2*diffusion*dt/(dx*dx))
func create():
    assert(reynolds <= 1000,"Pipe tutorial requires Reynolds <= 1000")
    assert(abs(velocity)*initialDt/dx+2*diffusion*initialDt/(dx*dx) <= 1,"Transport stability bound exceeded")
    metadata("model=passive three-pipe split plus separate periodic tracer\nintegrator=upwind advection and explicit centered diffusion\nnetwork=fixed nodes 0,2,3; free node 1; edges 0->1,1->2,1->3\ntransport=64 periodic cells over 1 m; mean velocity from inlet pipe\nvalidity=Re<=1000; abs(v)*dt/dx+2*D*dt/dx^2<=1\nexcluded=turbulence,momentum evolution,compressibility,open tracer boundaries,CFD\n")
    reset()
func reset():
    concentration = []
    for j in 0..<64:
        concentration.append(0.2+0.1*cos(2*pi*Float64(j)/64))
    elapsed = 0
    measure(initialDt)
func step(dt: Float64):
    concentration = transportStep(concentration,velocity,diffusion,dx,dt)
    elapsed += dt
    measure(dt)
func scene():
    var profile: [Vec3] = []
    for j in 0..<65:
        profile.append(Vec3(Float64(j)/64-0.5,concentration[j%64],0))
    polyline(profile,0.002,0x53aeefff,1)
    let nodes = [Vec3(-0.6,-0.4,0),Vec3(-0.2,-0.4,0),Vec3(0.3,-0.2,0),Vec3(0.3,-0.6,0)]
    let a = [0,1,1]
    let b = [1,2,3]
    for e in 0..<3:
        if network[4+e] == 0:
            line(nodes[a[e]],nodes[b[e]],0.012,0xf2a052ff,e+2)
        else if network[4+e] > 0:
            arrow(nodes[a[e]],nodes[b[e]],0.012,0xf2a052ff,e+2)
        else:
            arrow(nodes[b[e]],nodes[a[e]],0.012,0xf2a052ff,e+2)
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
    ps_series series[9]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&dataset);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,dataset,&info);
    const char *names[]={"time","concentration.center","reference.center","error.center","mass.perArea","flow.inlet","pressure.junction","reynolds","stability"};
    for(unsigned i=0;r==PS_OK && i<9;i++)r=ps_dataset_series(c,dataset,names[i],&series[i]);
    if(r==PS_OK)r=ps_report_create("Pipe network and periodic tracer","Passive network and conservative tracer mass",&report);
    ps_unit concentration={{-3,1,0,0,0,0,0},1,"kg/m^3"},mass_area={{-2,1,0,0,0,0,0},1,"kg/m^2"},flow={{3,0,-1,0,0,0,0},1,"m^3/s"};
    ps_unit units[]={concentration,concentration,mass_area,flow};
    const char *titles[]={"Center and continuum","Center error","Tracer mass per area","Inlet flow"};
    unsigned indices[]={1,3,4,5},counts[]={2,1,1,1};
    for(unsigned p=0;r==PS_OK && p<4;p++) {
        ps_plot_info plot={0};snprintf(plot.title,sizeof plot.title,"%s",titles[p]);
        strcpy(plot.x_label,"Time");snprintf(plot.y_label,sizeof plot.y_label,"%s",titles[p]);
        r=ps_report_unit_from(PS_SECOND,&plot.x_unit);
        if(r==PS_OK)r=ps_report_unit_from(units[p],&plot.y_unit);
        ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&plot,&handle);
        for(unsigned j=0;r==PS_OK && j<counts[p];j++)r=ps_report_add_series(report,handle,c,series[0],series[indices[p]+j],names[indices[p]+j],PS_PLOT_LINE);
    }
    double initial=0;size_t count=0;ps_statistics balance={0};
    if(r==PS_OK)r=ps_series_read(c,series[4],0,&initial,1,&count);
    if(r==PS_OK && count!=1)r=PS_INVALID;
    if(r==PS_OK)r=ps_series_statistics(c,series[4],&balance);
    ps_table_info table={0};strcpy(table.title,"Conservation checks");table.columns=2;
    strcpy(table.column[0].label,"Samples");strcpy(table.column[1].label,"Maximum mass drift");
    if(r==PS_OK)r=ps_report_unit_from(PS_ONE,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(mass_area,&table.column[1].unit);
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"complete run");row.values[0]=(double)info.samples;
    row.values[1]=fmax(fabs(balance.min-initial),fabs(balance.max-initial));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-transport.csv",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,series,9,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Pipe network and periodic tracer analysis",.run=analyze};return &api;
}
```

## Analyse in Physim

```physim
func analyze():
    assert(inputCount() == 1,"Select exactly one transport run")
    report("Pipe network and periodic tracer")
    let run = Dataset(0)
    let time = run.series("time")
    let center = run.series("concentration.center")
    let reference = run.series("reference.center")
    let error = run.series("error.center")
    let mass = run.series("mass.perArea")
    let flow = run.series("flow.inlet")
    let pressure = run.series("pressure.junction")
    let motion = center.plot(time,"Center and continuum","concentration.center")
    motion.curve(time,reference,"reference.center")
    error.plot(time,"Center error","error.center")
    mass.plot(time,"Tracer mass per area","mass.perArea")
    flow.plot(time,"Inlet flow","flow.inlet")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let massArea = Unit(-2,1,0,0,0,0,0,1,"kg/m^2")
    let initial = mass.value(0)
    let drift = max(abs(mass.minimum()-initial),abs(mass.maximum()-initial))
    let summary = Table("Conservation checks",["Samples","Maximum mass drift"],[one,massArea])
    summary.row("complete run",[Quantity(Float64(time.count()),one),Quantity(drift,massArea)])
    let reynolds = run.series("reynolds")
    let stability = run.series("stability")
    Series.exportColumns([time,center,reference,error,mass,flow,pressure,reynolds,stability],"transport")
    run.close()
```

## Unabhängige Prüfung

```sh
python3 tools/build.py --config Release --test --test-filter fluid --test-filter fluid_array_memory --test-filter 'language_*fluid*' --test-filter transport_tutorial --test-filter documentation_transport_source
```

Der Coreprüfer kontrolliert Rohrgesetze, Knotenerhaltung, Parallelkanten,
Singularitäten, Aliasing, 4096 Zellen, Masse, Positivität und Zahlenextreme.
Ein unabhängiger Allocator verwirft jeden Ergebnis-Allokationsversuch und prüft
vollständige Freigabe sowie unveränderte Eingaben. Der Lernpfad prüft sechs
Profile gegen hydraulische Formeln, Fourier-Amplifikation und Kontinuum,
mit vollständigen Polylinien, Netzgeometrie und 24 gemischten Analysen.
SDKs wiederholen alles gegen installierten und neu gebauten Core. Tatsächlich
ausgeführte Systeme stehen in der [Plattformprüfung](platform-validation.md).
