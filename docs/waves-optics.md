# Wellen, Saiten und geometrische Optik

Physim 0.181.0 ergänzt dieselben SI-Funktionen wie C für undämpfte Oszillatoren,
Laufwellen, diskrete Saiten und geometrische Optik. [Wellenreferenz](reference/waves.md),
[Optikreferenz](reference/optics.md), [Sprachbindungen](reference/language-library.md).

## Oszillator und Laufwelle

`harmonicStep` löst x''=-ω²x exakt für konstantes ω>0. Vec2.x ist Position in m,
y Geschwindigkeit in m/s. Die Größe v²+ω²x² bleibt bis auf Rundung erhalten.
Es gibt weder Dämpfung noch Anregung. Für eine ideale Saite gilt c=sqrt(T/μ)
aus positiver Spannungskraft T in N und linearer Dichte μ in kg/m.

`travelingWave` liefert A sin(kx-ωt+φ): Verschiebung in m, Geschwindigkeit
-ωA cos(...) in m/s und Steigung kA cos(...). Positive k,ω bewegen die Phase
nach +x. Das Modell setzt nicht selbst ω=c k; diese Dispersion wählt der Aufrufer.
Sehr große Phasen begrenzen die Genauigkeit der trigonometrischen Funktionen.

## Diskrete Ausbreitung und Stabilität

Die lineare Saite erfüllt u_tt=c²u_xx. `stringWaveStep` verwendet zentrierte
Differenzen zweiter Ordnung in Raum und Zeit:

u_j^(n+1)=2(1-λ²)u_j^n+λ²(u_(j-1)^n+u_(j+1)^n)-u_j^(n-1), λ=c dt/dx.

Die Eingaben sind zwei Zeitlagen mit demselben konstanten dt. Es gibt 3–4096
endliche Knoten in m; beide Endpunkte jeder Lage sind exakt null. c,dx,dt sind
positiv und λ≤1. Die CFL-Grenze wird vor Übernahme des Ergebnisses geprüft.
[MIT: Leapfrog und Stabilität](https://ocw.mit.edu/courses/18-086-mathematical-methods-for-engineers-ii-spring-2006/resources/am53/).

C verwendet caller-eigene Arrays und einen begrenzten 32-KiB-Stackpuffer;
Ausgabe darf Eingabespeicher überlappen und bleibt bei jedem Fehler erhalten.
Physim erzeugt ein neues besitzendes Array; Kopien bleiben unabhängig.
Der Core unterstützt 4096 Rechenknoten. Das vollständige grafische Beispiel
verwendet höchstens 65, damit alle Knoten in den bestehenden 96-Punkte-Szenenpool
passen. Diese Darstellungsgrenze verändert nicht den Core-Gitterschritt.
Die vorherige Lage wird nicht aus einem einzelnen Zustand erfunden. Für einen
anfangs ruhenden Modus u_j^0=A sin(mπj/(N-1)) wählen wir
u_j^-1=u_j^0 cos θ, θ=2 asin(λ sin(mπ/(2(N-1)))). Damit ist die zentrale
Anfangsgeschwindigkeit null. Die zeitlich versetzte Energie

E=½ μ dx Σ((u_j^n-u_j^(n-1))/dt)² + ½ T/dx Σ Δu_j^n Δu_j^(n-1)

ist die diskrete Invariante; sie ist nicht identisch mit einer beliebig
zeitlich abgetasteten Kontinuumsenergie. Keine variable Dichte, Anregung,
Dämpfung, nichtlineare Dehnung oder mehrdimensionale PDE wird vorausgesetzt.
Die Initialisierung liest das konfigurierte Hostintervall über `simulationTimeStep()`;
Änderung des Intervalls während dieses Versuchs wird abgewiesen.

## Reflexion, Brechung und Linsen

`reflectRay` und `refractRay` erwarten Unit-Richtungen und Unit-Normalen innerhalb
1e-10 und normalisieren akzeptierte Werte. Die Normale zeigt ins Einfallsmedium;
incident·normal≤1e-10. Positive Indizes n1,n2 definieren Snell n1 sin θ1=n2 sin θ2.
Totalreflexion liefert in C PS_SINGULAR ohne Änderung der Ausgabe; Physim erzeugt
eine abfangbare Diagnose. Reflexion ist ein ausdrücklich gewählter anderer Pfad.
Ein kritischer Sinus bis 32 DBL_EPSILON über eins wird auf eins begrenzt.
[MIT: Reflexion und Brechung](https://ocw.mit.edu/courses/2-71-optics-spring-2009/pages/lecture-slides/).

`thinLensImage` verwendet eine signierte nonzero Brennweite f und positive
Objektweite d in m: Bildweite b=f d/(d-f), Vergrößerung M=-f/(d-f).
Negative Bildweite bedeutet virtuell, negatives M ein invertiertes Bild.
Bei d=f liegt das Bild im Unendlichen; dieser Fall ist PS_SINGULAR bzw. eine
abfangbare Diagnose. Es gilt die paraxiale dünne Linse; keine dicken Linsen,
Aberrationen, Fresnelamplituden, Polarisation, Beugung oder automatische Strahlverfolgung.
[MIT: dünne Linsen](https://visionbook.mit.edu/lenses.html).

Alle Eingaben müssen endlich sein. PS_INVALID bezeichnet ungültige Werte,
PS_LIMIT zu viele Saitenknoten und PS_NUMERIC nicht darstellbare Zahlen oder
Phasen. Alle C-Ausgaben bleiben bei Fehlern erhalten. Signierte Größen dürfen
auf null runden; ein benötigter positiver Geschwindigkeitsergebniswert nicht.
Die Physim-Bindungen sind reine Werte und benutzen dieselben Core-Funktionen.
Nur `simulationTimeStep()` benötigt einen Experimenthost.

## Lernziel und Ablauf

Die Saite hat Länge 1 m, μ=1 kg/m und T=1 N. Daraus folgen c=1 m/s und im
Grundmodus eine Kontinuumsperiode von 2 s. 65 Knoten ergeben dx=1/64 m;
dt=0,005 s liefert λ=0,32. Die Anfangsamplitude ist 0,05 m. Die zentrale
Verschiebung wird gegen die Kontinuumslösung 0,05 cos(πt) verglichen.
Diskrete Dispersion erzeugt einen Fehler zweiter Ordnung; die diskrete Energie
bleibt dagegen erhalten. Das ist ein Gittermodell, keine reine Formelanimation.

1. Neues C- oder Physim-Projekt anlegen und die vollständige Experimentquelle
   unten einsetzen; beide Analysesprachen dürfen kombiniert werden. F5 baut.
2. F6 mit konstantem dt=0,005 s starten, zum Beispiel 200 Schritte für eine Sekunde.
   Stoppen finalisiert den Lauf. Analyse starten liefert vier Diagramme und CSV.
3. Modus, Länge, Amplitude, Spannung und lineare Dichte variieren. Die Beispiele
   unterstützen ungerade 17–65 Knoten und ganzzahlige Modi 1–4.
4. Bei Verfeinerung von dx und dt mit gleichem λ muss der Kontinuumsfehler ungefähr
   um Faktor vier sinken. Ein verletztes λ≤1 wird vor Veröffentlichung abgewiesen.
5. Mitgeliefertes Standalone-Programm `examples/language/waves_optics.phys`
   untersucht zusätzlich Oszillator, Laufwelle, besitzende Saitenkopien,
   Snell/Totalreflexion und reale/virtuelle Linsenbilder.

[Projektbedienung](workspace.md), [Fehlersuche](troubleshooting.md),
[Teil I – C](c-guide.md), [Teil II – Physim](physim-guide.md).

## Experiment in C

```c
#include "physim/experiment.h"
#include "physim/waves.h"
#include "physim/units.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NODES 65
/* Linear taut string, fixed endpoints, one standing mode initially at rest. */
typedef struct {
    unsigned count,mode;double length,amplitude,tension,density,speed,dx,dt,theta;
    double previous[MAX_NODES],current[MAX_NODES];
} string_state;
static ps_result measure(ps_context *c,const double *previous,const double *current,double time) {
    string_state *s=c->user;double energy=0;
    for(unsigned j=1;j+1<s->count;j++) {
        double velocity=(current[j]-previous[j])/s->dt;
        energy+=.5*s->density*s->dx*velocity*velocity;
    }
    for(unsigned j=0;j+1<s->count;j++)
        energy+=.5*s->tension/s->dx*(current[j+1]-current[j])*(previous[j+1]-previous[j]);
    double reference=s->amplitude*sin(s->mode*PS_PI*.5)*cos(s->speed*s->mode*PS_PI/s->length*time);
    double center=current[s->count/2];
    double values[]={center,reference,center-reference,energy,s->speed*s->dt/s->dx,(double)s->count};
    for(unsigned j=0;j<6;j++)if(!isfinite(values[j]))return PS_NUMERIC;
    for(unsigned j=0;j<6;j++)c->values[j]=values[j];return PS_OK;
}
static ps_result reset(ps_context *c) {
    string_state *s=c->user;
    for(unsigned j=0;j<s->count;j++) {
        s->current[j]=j==0 || j+1==s->count?0:s->amplitude*sin(s->mode*PS_PI*j/(s->count-1));
        s->previous[j]=s->current[j]*cos(s->theta);
    }
    return measure(c,s->previous,s->current,0);
}
static ps_result create(ps_context *c) {
    string_state *s=calloc(1,sizeof *s);if(!s)return PS_MEMORY;c->user=s;
    double count,mode;ps_unit density={{-1,1,0,0,0,0,0},1,"kg/m"};
    ps_result r=ps_parameter_define_unit(c,"nodes","Odd nodes across string (17..65)",PS_ONE,65,17,65,&count);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"mode","Standing wave mode (1..4)",PS_ONE,1,1,4,&mode);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"length","String length in m",PS_METRE,1,.1,10,&s->length);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"amplitude","Initial amplitude in m",PS_METRE,.05,0,1,&s->amplitude);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"tension","Constant tension in N",PS_NEWTON,1,.0001,100,&s->tension);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"linearDensity","Linear density in kg/m",density,1,.0001,100,&s->density);
    if(r!=PS_OK)return r;
    if(count!=floor(count) || ((unsigned)count)%2==0 || mode!=floor(mode))return PS_INVALID;
    s->count=(unsigned)count;s->mode=(unsigned)mode;s->dx=s->length/(s->count-1);s->dt=c->dt_s;
    r=ps_string_wave_speed(s->tension,s->density,&s->speed);if(r!=PS_OK)return r;
    double courant=s->speed*s->dt/s->dx;
    if(!isfinite(s->dt) || s->dt<=0 || !isfinite(courant) || courant>1)return PS_INVALID;
    s->theta=2*asin(courant*sin(s->mode*PS_PI/(2*(s->count-1))));
    const char *names[]={"displacement.center","reference.center","error.center","energy.discrete","courant","nodes"};
    ps_unit units[]={PS_METRE,PS_METRE,PS_METRE,PS_JOULE,PS_ONE,PS_ONE};
    for(unsigned j=0;j<6;j++)if(ps_channel_add(c,names[j],units[j],names[j])!=(int)j)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=linear fixed-endpoint string\nintegrator=centered second-order leapfrog\ninitial_velocity=zero; previous mode initialized with cos(theta)\nstability=c*dt/dx<=1; constant dt\nenergy=half-step kinetic plus cross-time gradient potential\nexcluded=damping,forcing,variable medium,nonlinear stretch,2D/3D PDE\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    string_state *s=c->user;
    if(!isfinite(dt) || fabs(dt-s->dt)>8*DBL_EPSILON*s->dt)return PS_INVALID;
    double next[MAX_NODES];ps_result r=ps_string_wave_step(s->previous,s->current,s->count,s->speed,s->dx,dt,next);
    if(r==PS_OK)r=measure(c,s->current,next,c->time_s+dt);
    if(r==PS_OK){memcpy(s->previous,s->current,s->count*sizeof(double));memcpy(s->current,next,s->count*sizeof(double));}
    return r;
}
static void scene(ps_context *c,ps_scene *out) {
    string_state *s=c->user;ps_vec3 points[MAX_NODES];
    for(unsigned j=0;j<s->count;j++)points[j]=ps_v3(j*s->dx-s->length*.5,s->current[j],0);
    ps_scene_polyline_id(out,1,points,s->count,.002,0x53aeefff);
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Standing wave on a fixed string",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

## Experiment in Physim

```physim
// Same linear string, fixed endpoints and leapfrog initialization as string_main.c.
let one = Unit(0,0,0,0,0,0,0,1,"1")
let metre = Unit(1,0,0,0,0,0,0,1,"m")
let newton = Unit(1,1,-2,0,0,0,0,1,"N")
let densityUnit = Unit(-1,1,0,0,0,0,0,1,"kg/m")
let joule = Unit(2,1,-2,0,0,0,0,1,"J")
let countValue = parameterWithUnit("nodes",one,65,17,65,"Odd nodes across string (17..65)")
let modeValue = parameterWithUnit("mode",one,1,1,4,"Standing wave mode (1..4)")
let length = parameterWithUnit("length",metre,1,0.1,10,"String length in m")
let amplitude = parameterWithUnit("amplitude",metre,0.05,0,1,"Initial amplitude in m")
let tension = parameterWithUnit("tension",newton,1,0.0001,100,"Constant tension in N")
let density = parameterWithUnit("linearDensity",densityUnit,1,0.0001,100,"Linear density in kg/m")
let nodes = Int64(countValue)
let mode = Int64(modeValue)
let speed = stringWaveSpeed(tension,density)
let dx = length/Float64(nodes-1)
let interval = simulationTimeStep()
let courant = speed*interval/dx
let pi = 3.14159265358979323846
let theta = 2*asin(courant*sin(Float64(mode)*pi/(2*Float64(nodes-1))))
let centerChannel = Channel("displacement.center",metre,"displacement.center")
let referenceChannel = Channel("reference.center",metre,"reference.center")
let errorChannel = Channel("error.center",metre,"error.center")
let energyChannel = Channel("energy.discrete",joule,"energy.discrete")
let courantChannel = Channel("courant",one,"courant")
let nodesChannel = Channel("nodes",one,"nodes")
var previous: [Float64] = []
var current: [Float64] = []
var elapsed = 0.0
func measure():
    var energy = 0.0
    for j in 1..<(nodes-1):
        let velocity = (current[j]-previous[j])/interval
        energy += 0.5*density*dx*velocity*velocity
    for j in 0..<(nodes-1):
        energy += 0.5*tension/dx*(current[j+1]-current[j])*(previous[j+1]-previous[j])
    let reference = amplitude*sin(Float64(mode)*pi*0.5)*cos(speed*Float64(mode)*pi/length*elapsed)
    let center = current[nodes/2]
    centerChannel.sample(center)
    referenceChannel.sample(reference)
    errorChannel.sample(center-reference)
    energyChannel.sample(energy)
    courantChannel.sample(courant)
    nodesChannel.sample(Float64(nodes))
func create():
    assert(Float64(nodes) == countValue && nodes%2 == 1 && Float64(mode) == modeValue,"Integer mode and odd node count required")
    assert(courant <= 1,"String requires c*dt/dx <= 1")
    metadata("model=linear fixed-endpoint string\nintegrator=centered second-order leapfrog\ninitial_velocity=zero; previous mode initialized with cos(theta)\nstability=c*dt/dx<=1; constant dt\nenergy=half-step kinetic plus cross-time gradient potential\nexcluded=damping,forcing,variable medium,nonlinear stretch,2D/3D PDE\n")
    reset()
func reset():
    previous = []
    current = []
    for j in 0..<nodes:
        var displacement = amplitude*sin(Float64(mode)*pi*Float64(j)/Float64(nodes-1))
        if j == 0 || j == nodes-1:
            displacement = 0
        current.append(displacement)
        previous.append(displacement*cos(theta))
    elapsed = 0
    measure()
func step(dt: Float64):
    assert(abs(dt-interval) <= 8*2.220446049250313e-16*interval,"String requires constant dt")
    let next = stringWaveStep(previous,current,speed,dx,dt)
    previous = current
    current = next
    elapsed += dt
    measure()
func scene():
    var points: [Vec3] = []
    for j in 0..<nodes:
        points.append(Vec3(Float64(j)*dx-length*0.5,current[j],0))
    polyline(points,0.002,0x53aeefff,1)
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
    const char *names[]={"time","displacement.center","reference.center","error.center","energy.discrete","courant","nodes"};
    for(unsigned i=0;r==PS_OK && i<7;i++)r=ps_dataset_series(c,dataset,names[i],&series[i]);
    if(r==PS_OK)r=ps_report_create("Standing wave on a fixed string","Leapfrog dispersion and discrete conserved energy",&report);
    ps_unit units[]={PS_METRE,PS_METRE,PS_JOULE,PS_ONE};
    const char *titles[]={"Center and continuum","Center error","Discrete energy","Courant number"};
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
    strcpy(table.column[0].label,"Samples");strcpy(table.column[1].label,"Maximum energy drift");
    if(r==PS_OK)r=ps_report_unit_from(PS_ONE,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_JOULE,&table.column[1].unit);
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"complete run");row.values[0]=(double)info.samples;
    row.values[1]=fmax(fabs(balance.min-initial),fabs(balance.max-initial));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-string.csv",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,series,7,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Standing wave on a fixed string analysis",.run=analyze};return &api;
}
```

## Analyse in Physim

```physim
func analyze():
    assert(inputCount() == 1,"Select exactly one string run")
    report("Standing wave on a fixed string")
    let run = Dataset(0)
    let time = run.series("time")
    let center = run.series("displacement.center")
    let reference = run.series("reference.center")
    let error = run.series("error.center")
    let energy = run.series("energy.discrete")
    let courant = run.series("courant")
    let nodes = run.series("nodes")
    let motion = center.plot(time,"Center and continuum","displacement.center")
    motion.curve(time,reference,"reference.center")
    error.plot(time,"Center error","error.center")
    energy.plot(time,"Discrete energy","energy.discrete")
    courant.plot(time,"Courant number","courant")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let joules = Unit(2,1,-2,0,0,0,0,1,"J")
    let initial = energy.value(0)
    let drift = max(abs(energy.minimum()-initial),abs(energy.maximum()-initial))
    let summary = Table("Conservation checks",["Samples","Maximum energy drift"],[one,joules])
    summary.row("complete run",[Quantity(Float64(time.count()),one),Quantity(drift,joules)])
    Series.exportColumns([time,center,reference,error,energy,courant,nodes],"string")
    run.close()
```

## Unabhängige Prüfung

```sh
python3 tools/build.py --config Release --test --test-filter waves_optics --test-filter wave_array_memory --test-filter 'language_*waves*' --test-filter string_tutorial --test-filter documentation_string_source
```

Der Coreprüfer kontrolliert Oszillatorinvariante, Laufphasen, diskrete Eigenmoden,
Alias-Ausgaben, CFL und Zahlenextreme sowie Snell, Totalreflexion und Linsenvorzeichen.
Allokationsfehler und verworfene Saitenergebnisse geben sämtliche Besitzer frei.
Der Tutorialprüfer kontrolliert sechs Gitterprofile, komplette Polylinien,
Kontinuumslösung, diskrete Energie und zweite Ordnung bei Verfeinerung, mit allen
vier Kombinationen der beiden Experiment-/Analysesprachen. SDKs wiederholen
alles gegen installierten und neu aufgebauten Core. Ausgeführte Systeme stehen
in der [Plattformprüfung](platform-validation.md).
