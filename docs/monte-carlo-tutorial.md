# Monte Carlo: unsichere Anfangswerte

Lernziel: Übertrage eine bekannte Unsicherheit der Anfangsgeschwindigkeit auf die
Endposition. Wiederhole dasselbe Experiment mit expliziten Seeds, untersuche die
archivierten Verteilungen und vergleiche Stichprobenwerte mit der Modellreferenz.
Experimente und Auswertungen liegen vollständig in C und Physim vor.

## Modell und Erwartung

Eine Punktmasse von 1 kg startet bei (−2, 0) m im Vakuum. Die konstante
Fallbeschleunigung beträgt g=9,80665 m/s². Es gibt keinen Boden, Luftwiderstand,
Sensorfehler oder Parameterdrift. Zu Beginn jedes Laufs werden vx und vy einmal
unabhängig normalverteilt gezogen. Danach wird die Bewegung exakt berechnet:

- x(t)=−2+vx·t; y(t)=vy·t−½g·t².
- vx(t)=vx; vy(t)=vy−g·t.
- E=½(vx²+vy(t)²)+g·y(t) bleibt erhalten.
- E[x(t)]=−2+μx·t, E[y(t)]=μy·t−½g·t².
- σx(t)=σvx·t und σy(t)=σvy·t für t≥0.

Dadurch überlagert kein Integrationsfehler die Anfangswertunsicherheit.
Die Zufallszahlen sind deterministische Pseudozufallszahlen aus dem protokollierten
64-Bit-Seed. Die statistische Interpretation setzt unabhängige Normalziehungen
voraus; aufeinanderfolgende Seeds sind eine reproduzierbare praktische Serie und
kein mathematischer Beweis ihrer Unabhängigkeit. Bei σ=0 ist die Ziehung konstant
und verbraucht keine Zufallszahl. Reset spielt denselben Seed erneut ab.

| Parameter | Standard | Bereich |
| --- | --- | --- |
| `meanVx` | 3 m/s | −20..20 m/s |
| `sigmaVx` | 0,15 m/s | 0..5 m/s |
| `meanVy` | 5 m/s | −20..20 m/s |
| `sigmaVy` | 0,25 m/s | 0..5 m/s |

Neun Kanäle speichern tatsächliche Position und Geschwindigkeit, Sollposition,
Populationsstandardabweichungen und Energie. `uncertainty.x/y` sind weder
Messfehler eines Sensors noch Konfidenzgrenzen eines Mittelwerts. Die Szene zeigt
einen Einzellauf, seine Sollbahn und ein Kreuz von ±einer Standardabweichung je
Achse. Dieses Kreuz ist kein gemeinsames 68%-Gebiet in zwei Dimensionen.

C verwendet `experiment` für Parameter, Kanäle und Szenen, `measurement` für
Normalziehungen sowie `run_index`, `series` und `report` für das Lesen und
Auswerten der Archive. Physim nutzt die entsprechenden Parameter-, Kanal-,
`randomNormal`-, `RunIndex`-, `Series`- und `Table`-Bindungen. Einheiten und
Dimensionen begleiten Parameter, Kanäle, Plotachsen und Tabellen.

## In der App durchführen

1. Lege ein Projekt mit **Wurf mit Unsicherheit** in C oder Physim an. Ersetze
   Experiment und Analyse durch die vier passenden vollständigen Quellen unten.
   Wähle Release, h=0,03125 s und baue.
2. Starte einen Einzellauf, speichere mindestens 1 s und betrachte die Sollbahn.
   Die Analysequelle unten benötigt anschließend eine vollständige Serie.
3. Öffne **Monte Carlo**. Setze 256 Läufe, 32 Schritte, dt=0,03125 s,
   Startseed 42, Kanal `position.x` und vier gleichzeitige Läufe. Aktiviere
   **Gemeinsame Endzeit** mit 1 s. Lasse Parameterstudie und adaptive Schritte aus.
4. Starte die Serie. Der allgemeine Serienbericht erscheint nach dem Abschluss.
5. Kehre zu **Monte Carlo** zurück und wähle **Ersten Serienlauf auswerten**.
   Die gebaute Analyse bekommt den ursprünglichen Pfad von `run-0001.psrun` und
   liest alle 256 Nachbardateien. Die Laufarchive werden nicht verschoben.
6. Vergleiche horizontale und vertikale Endwerte, Histogramme, Quantile und
   die beiden Tabellen unter **Auswerten**. Breite Tabellen lassen sich
   unten horizontal scrollen. Wiederhole in einem neuen Serienordner
   mit anderem Seed oder setze beide σ-Parameter auf null.

Die Schaltfläche ist nur nach einer vollständigen erfolgreichen Serie und mit
gebauter, gespeicherter Analyse verfügbar. Bei gewöhnlichen Analysequellen
wertet sie nur den ersten Lauf aus. Ein später gestartetes Analyseprojekt kann
von der Kommandozeile ebenfalls eine Originaldatei aus dem Serienordner verwenden.
Ein importierter einzelner Lauf wird dagegen kopiert und enthält seine
Nachbardateien nicht; verwende dafür den ursprünglichen Serienpfad.

## Bericht und statistische Grenzen

Diese Beispielanalyse erwartet genau die benannten Dateien `run-0001.psrun` bis
`run-0256.psrun`, jeweils vollständig, mit neun passenden Kanälen und derselben
positiven Endzeit, Sollposition und Populationsstreuung. Fehlende, beschädigte
oder unpassende Läufe werden abgewiesen. Sie prüft keine Seed-Unabhängigkeit und
ersetzt keine fehlenden Werte durch neue Zufallsziehungen. Für eine andere
Seriengröße ändere `TUTORIAL_RUNS` in C und `runs` in Physim gemeinsam und baue
neu. Der allgemeine Serienbericht unterstützt unabhängig davon 1..1000 Läufe.

Vier Plots zeigen Endwerte nach Laufindex und Histogramme mit 16 Klassen je
Achse; eine konstante Population ergibt eine Klasse. Die erste Tabelle enthält
Mittelwert, Stichprobenstandardabweichung (Nenner n−1), Quantile 2,5%, 50% und
97,5% sowie Modellmittelwert und Modellstandardabweichung. Die Quantile benutzen
lineare Interpolation zwischen sortierten Werten (Typ 7). Die zweite Tabelle
zeigt das 95%-Intervall des Mittelwerts für **bekannte Modellstreuung**:

`Mittelwert ± 1,959963984540054 · Modellstandardabweichung / √256`.

Unter den genannten unabhängigen Normalannahmen überdecken so berechnete
Intervalle in wiederholten Serien den festen Populationsmittelwert zu 95%.
Dies ist kein 95%-Vorhersageintervall eines einzelnen Wurfs und kein Intervall
für unbekannte Streuung; dafür wäre eine eigene Schätzung mit anderen Annahmen
nötig. Die empirischen Quantile beschreiben hier einzelne Endwerte.

Bei t=1 s gelten μx=1 m, μy=0,096675 m, σx=0,15 m und σy=0,25 m.
Die Intervallradien des Mittelwerts betragen ungefähr 0,01837 m und 0,03062 m.
Stichprobenmittelwerte und Quantile müssen nicht exakt auf der Referenz liegen.
σ=0 führt zu identischen Endwerten und einem Intervall ohne Breite.

`<prefix>-endpoints.csv` enthält alle 256 Endwertpaare mit Laufindex; die Rohdateien
enthalten weiterhin sämtliche 33 Messpunkte. Große dt-Werte verändern die
Abtastung der Bahn, nicht die hier exakt berechnete Endposition.

## Aus einem SDK bauen und auswerten

Die Befehle gelten in einem SDK-Verzeichnis unter macOS/Linux. Verwende neue
Ausgabepfade; der Serienordner darf vorher nicht existieren. Windows benötigt
angepasste Shellbefehle und Dateiendungen.

```sh
mkdir monte-carlo-c
cp examples/documentation/monte_carlo_main.c monte-carlo-c/main.c
cp examples/documentation/monte_carlo_analysis.c monte-carlo-c/analysis.c
printf '%s\n' physim_project=1 > monte-carlo-c/physim.project
bin/physim-build --project monte-carlo-c --sdk . --output monte-carlo-c/build/Release --physimc bin/physimc --profile Release
bin/physim-batch "$PWD/bin/physim-runner" "$PWD/monte-carlo-c/build/Release/experiment.so" "$PWD/monte-carlo-series" position.x 256 32 0.03125 42 --workers 4 --until 1
bin/physim-analysis-runner monte-carlo-c/build/Release/analysis.so monte-carlo-series/run-0001.psrun monte-carlo-result
```

Die beiden Physim-Module liegen bereits als `bin/language-monte_carlo_main.so`
und `bin/language-monte_carlo_analysis.so` im SDK. Ersetze damit die Module und
wähle einen neuen Serienordner. Beide Analysesprachen lesen beide Laufsprachen.

## Vollständige Quellen

### C: Experiment

```c
#include "physim/experiment.h"
#include "physim/measurement.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { double mean_x,sigma_x,mean_y,sigma_y,vx,vy,time; } projectile;
static const double gravity=9.80665;
static ps_result measure(ps_context *c,const projectile *p,double time) {
    double x=-2+p->vx*time,y=p->vy*time-.5*gravity*time*time,vy=p->vy-gravity*time;
    double values[]={x,y,p->vx,vy,-2+p->mean_x*time,p->mean_y*time-.5*gravity*time*time,
        p->sigma_x*time,p->sigma_y*time,.5*(p->vx*p->vx+vy*vy)+gravity*y};
    for(unsigned i=0;i<9;i++)if(!isfinite(values[i]))return PS_NUMERIC;
    for(unsigned i=0;i<9;i++)c->values[i]=values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    projectile *p=c->user,next=*p;ps_rng rng;ps_rng_seed(&rng,c->seed);
    ps_result r=ps_distribution_sample((ps_distribution){PS_DIST_NORMAL,p->mean_x,p->sigma_x},&rng,&next.vx);
    if(r==PS_OK)r=ps_distribution_sample((ps_distribution){PS_DIST_NORMAL,p->mean_y,p->sigma_y},&rng,&next.vy);
    if(r==PS_OK)r=measure(c,&next,0);
    if(r==PS_OK){next.time=0;*p=next;c->rng=rng;}return r;
}
static ps_result create(ps_context *c) {
    projectile *p=calloc(1,sizeof *p);if(!p)return PS_MEMORY;c->user=p;
    ps_result r=ps_parameter_define_unit(c,"meanVx","Mean initial horizontal velocity",PS_VELOCITY,3,-20,20,&p->mean_x);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"sigmaVx","Initial horizontal standard deviation",PS_VELOCITY,.15,0,5,&p->sigma_x);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"meanVy","Mean initial vertical velocity",PS_VELOCITY,5,-20,20,&p->mean_y);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"sigmaVy","Initial vertical standard deviation",PS_VELOCITY,.25,0,5,&p->sigma_y);
    if(r!=PS_OK)return r;
    const char *names[]={"position.x","position.y","velocity.x","velocity.y","nominal.x","nominal.y","uncertainty.x","uncertainty.y","energy"};
    for(unsigned i=0;i<9;i++)if(ps_channel_add(c,names[i],i==2 || i==3?PS_VELOCITY:i==8?PS_JOULE:PS_METRE,names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,
        "model=uncertain initial velocities, exact vacuum projectile\nmass_kg=1\ngravity_m_s2=9.80665\ninitial_position_m=-2,0\nvx_distribution=normal\nvy_distribution=normal\nvelocity_dependence=independent model draws\nintegrator=exact ballistic propagation\nuncertainty_channels=population standard deviations, not sensor errors\nexcluded=ground, drag, sensor noise, parameter drift\n");
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    if(!isfinite(dt) || dt<=0 || !isfinite(c->time_s+dt) || c->time_s+dt==c->time_s)return PS_INVALID;
    projectile *p=c->user;double time=c->time_s+dt;ps_result r=measure(c,p,time);
    if(r==PS_OK)p->time=time;
    return r;
}
static void scene(ps_context *c,ps_scene *s) {
    projectile *p=c->user;ps_vec3 actual=ps_v3(c->values[0],c->values[1],0),nominal=ps_v3(c->values[4],c->values[5],0);
    ps_scene_add_id(s,1,PS_SPHERE,actual,actual,.09,0x53dec2ff);
    ps_scene_add_id(s,2,PS_POINT,nominal,nominal,.05,0x779bccff);
    ps_scene_add_id(s,3,PS_ARROW,actual,ps_vadd(actual,ps_v3(.12*c->values[2],.12*c->values[3],0)),0,0x53dec2ff);
    (void)ps_scene_label_id(s,4,actual,"Einzellauf",0x53dec2ff);
    (void)ps_scene_label_id(s,5,nominal,"Erwartungswert",0x779bccff);
    ps_scene_add_id(s,6,PS_LINE,ps_vadd(nominal,ps_v3(-c->values[6],0,0)),ps_vadd(nominal,ps_v3(c->values[6],0,0)),.006,0xf2c572ff);
    ps_scene_add_id(s,7,PS_LINE,ps_vadd(nominal,ps_v3(0,-c->values[7],0)),ps_vadd(nominal,ps_v3(0,c->values[7],0)),.006,0xf2c572ff);
    (void)ps_scene_label_id(s,8,ps_v3(-.5,1.6,0),"Kreuz: eine Populations-Standardabweichung",0xf2c572ff);
    if(p->time>0) {
        ps_vec3 a[32],b[32];for(unsigned i=0;i<32;i++) {
            double t=p->time*i/31;a[i]=ps_v3(-2+p->vx*t,p->vy*t-.5*gravity*t*t,0);
            b[i]=ps_v3(-2+p->mean_x*t,p->mean_y*t-.5*gravity*t*t,0);
        }
        (void)ps_scene_polyline_id(s,9,a,32,.008,0x53dec2ff);(void)ps_scene_polyline_id(s,10,b,32,.004,0x779bccff);
    }
    (void)ps_scene_group(s,100,0,"Unsicherer Vakuumwurf");
    for(unsigned i=1;i<=8;i++)(void)ps_scene_set_parent(s,i,100);
    if(p->time>0){(void)ps_scene_set_parent(s,9,100);(void)ps_scene_set_parent(s,10,100);}
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.capabilities=PS_EXPERIMENT_SCENE_HIERARCHY,
        .name="Monte Carlo initial velocities",.create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

### C: Analyse

```c
#include "physim/report.h"
#include "physim/run_index.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
/* Match the tutorial's 256-run series. Change this and the Physim constant
 * together when deliberately analyzing a different-size archived series. */
#define TUTORIAL_RUNS 256u
static ps_result analyze(const char *input,const char *prefix) {
    char directory[4096];size_t size=strlen(input);if(size>=sizeof directory)return PS_LIMIT;
    memcpy(directory,input,size+1);char *slash=strrchr(directory,'/'),*backslash=strrchr(directory,'\\');
    if(backslash && (!slash || backslash>slash))slash=backslash;
    if(!slash)return PS_INVALID;
    *slash=0;
    double indices[TUTORIAL_RUNS],x[TUTORIAL_RUNS],y[TUTORIAL_RUNS],expected[4]={0},endpoint=0;
    const char *names[]={"position.x","position.y","velocity.x","velocity.y","nominal.x","nominal.y","uncertainty.x","uncertainty.y","energy"};
    ps_result r=PS_OK;
    for(unsigned i=0;r==PS_OK && i<TUTORIAL_RUNS;i++) {
        char path[4096];int n=snprintf(path,sizeof path,"%s/run-%04u.psrun",directory,i+1);
        if(n<0 || (size_t)n>=sizeof path)return PS_LIMIT;
        ps_run_index *run=NULL;r=ps_run_index_open(path,ps_allocator_default(),4096,&run);
        if(r==PS_RECOVERED)r=PS_CORRUPT;
        ps_run_index_info info={.struct_size=sizeof info,.version=PS_RUN_INDEX_VERSION};
        if(r==PS_OK)r=ps_run_index_get_info(run,&info);
        if(r==PS_OK && (!info.complete || !info.samples || info.channels!=9))r=PS_INVALID;
        for(unsigned k=0;r==PS_OK && k<9;k++) {
            ps_unit unit=k==2 || k==3?PS_VELOCITY:k==8?PS_JOULE:PS_METRE;
            if(strcmp(info.schema[k].name,names[k]) || memcmp(info.schema[k].dimension,unit.dimension,7))r=PS_INVALID;
        }
        double time=0,values[PS_MAX_CHANNELS];if(r==PS_OK)r=ps_run_index_read(run,info.samples-1,1,&time,values);
        if(r==PS_OK) {
            if(!i){endpoint=time;for(unsigned k=0;k<4;k++)expected[k]=values[4+k];}
            if(time!=endpoint || endpoint<=0 || values[6]<0 || values[7]<0)r=PS_INVALID;
            for(unsigned k=0;r==PS_OK && k<4;k++)if(values[4+k]!=expected[k])r=PS_INVALID;
            indices[i]=i+1;x[i]=values[0];y[i]=values[1];
        }
        ps_run_index_destroy(run);
    }
    if(r!=PS_OK)return r;
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_series ordinal={0},series[2]={0};
    r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK)r=ps_series_from_values(c,indices,TUTORIAL_RUNS,PS_ONE,"run",&ordinal);
    if(r==PS_OK)r=ps_series_aligned_values(c,ordinal,x,TUTORIAL_RUNS,PS_METRE,"position.x endpoint",&series[0]);
    if(r==PS_OK)r=ps_series_aligned_values(c,ordinal,y,TUTORIAL_RUNS,PS_METRE,"position.y endpoint",&series[1]);
    if(r==PS_OK)r=ps_report_create("Monte Carlo initial velocities","256 archived endpoints at a common positive time",&report);
    ps_table_info info={0};strcpy(info.title,"Endpoint statistics");info.columns=7;
    const char *labels[]={"Mean","Sample standard deviation","Q2.5%","Median","Q97.5%","Model mean","Model standard deviation"};
    for(unsigned k=0;r==PS_OK && k<7;k++){strcpy(info.column[k].label,labels[k]);r=ps_report_unit_from(PS_METRE,&info.column[k].unit);}
    ps_table_handle summary={0},confidence={0};if(r==PS_OK)r=ps_report_add_table(report,&info,&summary);
    memset(&info,0,sizeof info);strcpy(info.title,"95% mean interval, known model sigma");info.columns=2;
    strcpy(info.column[0].label,"Lower");strcpy(info.column[1].label,"Upper");
    if(r==PS_OK)r=ps_report_unit_from(PS_METRE,&info.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_METRE,&info.column[1].unit);
    if(r==PS_OK)r=ps_report_add_table(report,&info,&confidence);
    for(unsigned axis=0;r==PS_OK && axis<2;axis++) {
        const char *title=axis?"Vertical endpoints":"Horizontal endpoints";ps_plot_info plot={0};
        strcpy(plot.title,title);strcpy(plot.x_label,"Run");strcpy(plot.y_label,"Position");
        r=ps_report_unit_from(PS_ONE,&plot.x_unit);if(r==PS_OK)r=ps_report_unit_from(PS_METRE,&plot.y_unit);
        ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&plot,&handle);
        if(r==PS_OK)r=ps_report_add_series(report,handle,c,ordinal,series[axis],axis?"y":"x",PS_PLOT_LINE);
        if(r==PS_OK)r=ps_report_add_histogram(report,c,series[axis],axis?"Vertical distribution":"Horizontal distribution","Position",16,&handle);
        ps_statistics stats={0};if(r==PS_OK)r=ps_series_statistics(c,series[axis],&stats);
        ps_table_row row={0};strcpy(row.label,axis?"y":"x");row.values[0]=stats.mean;row.values[1]=ps_statistics_stddev(&stats);
        const double probabilities[]={.025,.5,.975};
        for(unsigned k=0;r==PS_OK && k<3;k++)r=ps_series_quantile(c,series[axis],probabilities[k],&row.values[2+k]);
        row.values[5]=expected[axis];row.values[6]=expected[2+axis];if(r==PS_OK)r=ps_report_add_row(report,summary,&row);
        double radius=1.959963984540054*expected[2+axis]/sqrt((double)TUTORIAL_RUNS);
        row.values[0]=stats.mean-radius;row.values[1]=stats.mean+radius;if(r==PS_OK)r=ps_report_add_row(report,confidence,&row);
    }
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-endpoints.csv",prefix);ps_series columns[]={ordinal,series[0],series[1]};
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,columns,3,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Monte Carlo archived series analysis",.run=analyze};return &api;
}
```

### Physim: Experiment

```physim
let metres = Unit(1,0,0,0,0,0,0,1,"m")
let speed = Unit(1,0,-1,0,0,0,0,1,"m/s")
let joules = Unit(2,1,-2,0,0,0,0,1,"J")
let meanVx = parameterWithUnit("meanVx",speed,3,-20,20,"Mean initial horizontal velocity")
let sigmaVx = parameterWithUnit("sigmaVx",speed,0.15,0,5,"Initial horizontal standard deviation")
let meanVy = parameterWithUnit("meanVy",speed,5,-20,20,"Mean initial vertical velocity")
let sigmaVy = parameterWithUnit("sigmaVy",speed,0.25,0,5,"Initial vertical standard deviation")
let gravity = 9.80665
let cx = Channel("position.x",metres,"position.x")
let cy = Channel("position.y",metres,"position.y")
let cvx = Channel("velocity.x",speed,"velocity.x")
let cvy = Channel("velocity.y",speed,"velocity.y")
let cnx = Channel("nominal.x",metres,"nominal.x")
let cny = Channel("nominal.y",metres,"nominal.y")
let csx = Channel("uncertainty.x",metres,"uncertainty.x")
let csy = Channel("uncertainty.y",metres,"uncertainty.y")
let ce = Channel("energy",joules,"energy")
var vx = meanVx
var vy = meanVy
var elapsed = 0.0
func measure(time: Float64, initialVx: Float64, initialVy: Float64):
    let x = -2 + initialVx * time
    let y = initialVy * time - 0.5 * gravity * time * time
    let velocityY = initialVy - gravity * time
    let energy = 0.5 * (initialVx * initialVx + velocityY * velocityY) + gravity * y
    let nx = -2 + meanVx * time
    let ny = meanVy * time - 0.5 * gravity * time * time
    let sx = sigmaVx * time
    let sy = sigmaVy * time
    cx.sample(x)
    cy.sample(y)
    cvx.sample(initialVx)
    cvy.sample(velocityY)
    cnx.sample(nx)
    cny.sample(ny)
    csx.sample(sx)
    csy.sample(sy)
    ce.sample(energy)
func create():
    metadata("model=uncertain initial velocities, exact vacuum projectile\nmass_kg=1\ngravity_m_s2=9.80665\ninitial_position_m=-2,0\nvx_distribution=normal\nvy_distribution=normal\nvelocity_dependence=independent model draws\nintegrator=exact ballistic propagation\nuncertainty_channels=population standard deviations, not sensor errors\nexcluded=ground, drag, sensor noise, parameter drift\n")
func reset():
    let nextVx = randomNormal(meanVx,sigmaVx)
    let nextVy = randomNormal(meanVy,sigmaVy)
    measure(0,nextVx,nextVy)
    vx = nextVx
    vy = nextVy
    elapsed = 0
func step(dt: Float64):
    assert(dt > 0 && simulationTime() + dt > simulationTime(),"Positive representable step required")
    let time = simulationTime() + dt
    measure(time,vx,vy)
    elapsed = time
func scene():
    let actual = Vec3(-2 + vx * elapsed,vy * elapsed - 0.5 * gravity * elapsed * elapsed,0)
    let nominal = Vec3(-2 + meanVx * elapsed,meanVy * elapsed - 0.5 * gravity * elapsed * elapsed,0)
    sphere(actual,0.09,0x53DEC2FF,1)
    point(nominal,0.05,0x779BCCFF,2)
    arrow(actual,actual + Vec3(0.12 * vx,0.12 * (vy - gravity * elapsed),0),0,0x53DEC2FF,3)
    label(actual,"Einzellauf",0x53DEC2FF,4)
    label(nominal,"Erwartungswert",0x779BCCFF,5)
    let horizontal = Vec3(sigmaVx * elapsed,0,0)
    let vertical = Vec3(0,sigmaVy * elapsed,0)
    line(nominal - horizontal,nominal + horizontal,0.006,0xF2C572FF,6)
    line(nominal - vertical,nominal + vertical,0.006,0xF2C572FF,7)
    label(Vec3(-0.5,1.6,0),"Kreuz: eine Populations-Standardabweichung",0xF2C572FF,8)
    if elapsed > 0:
        var actualPath: [Vec3] = []
        var nominalPath: [Vec3] = []
        for i in 0..<32:
            let time = elapsed * Float64(i) / 31
            actualPath.append(Vec3(-2 + vx * time,vy * time - 0.5 * gravity * time * time,0))
            nominalPath.append(Vec3(-2 + meanVx * time,meanVy * time - 0.5 * gravity * time * time,0))
        polyline(actualPath,0.008,0x53DEC2FF,9)
        polyline(nominalPath,0.004,0x779BCCFF,10)
    group("Unsicherer Vakuumwurf",100,0)
    for id in 1..<9:
        sceneParent(id,100)
    if elapsed > 0:
        sceneParent(9,100)
        sceneParent(10,100)
```

### Physim: Analyse

```physim
// Match the number of archived runs configured in the tutorial's series.
let runs: Int64 = 256
func analyze():
    assert(inputCount() == 1,"Select a raw run inside the 256-run series")
    let path = inputPath(0).replacingOccurrences(of: "\\",with: "/")
    var directory = ""
    if let separator = path.lastIndex(of: "/"):
        directory = path.prefix(separator)
    assert(directory.count > 0,"Series directory required")
    var indices: [Float64] = []
    var xs: [Float64] = []
    var ys: [Float64] = []
    var expected = [0.0,0.0,0.0,0.0]
    var endpoint = 0.0
    let names = ["position.x","position.y","velocity.x","velocity.y","nominal.x","nominal.y","uncertainty.x","uncertainty.y","energy"]
    for index in 0..<runs:
        var digits = String(index + 1)
        while digits.count < 4:
            digits = "0" + digits
        var run = RunIndex(directory + "/run-" + digits + ".psrun",4096)
        assert(run.isComplete() && run.sampleCount() > 0 && run.channelCount() == 9,"Complete tutorial run required")
        for channel in 0..<9:
            assert(run.channelName(channel) == names[channel],"Tutorial channel schema required")
            var dimension = [1,0,0,0,0,0,0]
            if channel == 2 || channel == 3:
                dimension = [1,0,-1,0,0,0,0]
            else if channel == 8:
                dimension = [2,1,-2,0,0,0,0]
            for axis in 0..<7:
                assert(run.channelDimension(channel,axis) == dimension[axis],"Tutorial SI dimensions required")
        let block = run.read(run.sampleCount() - 1,1)
        let time = block.times()[0]
        if index == 0:
            endpoint = time
            for channel in 0..<4:
                expected[channel] = block.column(channel + 4)[0]
        assert(time == endpoint && endpoint > 0,"Common positive endpoint required")
        assert(block.column(6)[0] >= 0 && block.column(7)[0] >= 0,"Nonnegative population sigma required")
        for channel in 0..<4:
            assert(block.column(channel + 4)[0] == expected[channel],"Identical model parameters required")
        indices.append(Float64(index + 1))
        xs.append(block.column(0)[0])
        ys.append(block.column(1)[0])
        run.close()
    report("Monte Carlo initial velocities")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let metres = Unit(1,0,0,0,0,0,0,1,"m")
    let ordinal = Series.fromValues(indices,one,"run")
    let x = ordinal.alignedValues(xs,metres,"position.x endpoint")
    let y = ordinal.alignedValues(ys,metres,"position.y endpoint")
    let summary = Table("Endpoint statistics",["Mean","Sample standard deviation","Q2.5%","Median","Q97.5%","Model mean","Model standard deviation"],[metres,metres,metres,metres,metres,metres,metres])
    let confidence = Table("95% mean interval, known model sigma",["Lower","Upper"],[metres,metres])
    let series = [x,y]
    let titles = ["Horizontal endpoints","Vertical endpoints"]
    let distributions = ["Horizontal distribution","Vertical distribution"]
    let labels = ["x","y"]
    for axis in 0..<2:
        let values = series[axis]
        values.plot(ordinal,titles[axis],labels[axis])
        values.histogram(distributions[axis],16)
        summary.row(labels[axis],[Quantity(values.mean(),metres),Quantity(values.stddev(),metres),
            Quantity(values.quantile(0.025),metres),Quantity(values.quantile(0.5),metres),Quantity(values.quantile(0.975),metres),
            Quantity(expected[axis],metres),Quantity(expected[axis + 2],metres)])
        let radius = 1.959963984540054 * expected[axis + 2] / sqrt(Float64(runs))
        confidence.row(labels[axis],[Quantity(values.mean() - radius,metres),Quantity(values.mean() + radius,metres)])
    Series.exportColumns([ordinal,x,y],"endpoints")
```
