# Pendel und Vergleich von Integratoren

Lernziel: Berechne dasselbe nichtlineare Pendel mit fünf numerischen Verfahren,
vergleiche Winkel und Energiefehler und bestimme seine Schwingungsdauer aus den
Messwerten. Experiment und Mehrlaufanalyse liegen vollständig in C und Physim vor.

## Modell und Gleichungen

Eine Punktmasse von 1 kg hängt an einer masselosen starren Stange. Die Länge ist
standardmäßig 1,5 m, die Anfangsauslenkung 0,45 rad und die Winkelgeschwindigkeit
anfangs null. Die Schwerkraft beträgt 9,80665 m/s². Der Winkel wird gegenüber der
Senkrechten gemessen; die positive y-Achse zeigt nach oben. Es gilt

- `θ′ = ω`, `ω′ = −(g/L) sin θ`.
- `x = L sin θ`, `y = −L cos θ`.
- `E = ½mL²ω² + mgL(1 − cos θ)`; im Vakuum bleibt E konstant.

Luftwiderstand, Antrieb, Reibung, Stangenmasse, Kontakte und Messrauschen sind
hier ausgeschlossen. `sensor.angle` ist deshalb gleich dem tatsächlichen Winkel.
Ein Energieanstieg entsteht bei diesen Beispielen durch das numerische Verfahren.
Dieser Lernpfad untersucht keine gedämpfte Bewegung; dafür enthält das allgemeine
C-Pendelbeispiel `examples/pendulum/main.c` eine gesonderte Widerstandskraft.

Die nichtlineare Periode ist `T = 4√(L/g) K(sin(|θ₀|/2))`, wobei
`K(k) = ∫₀^(π/2) (1−k² sin²φ)^(-½) dφ`. Die Kleinwinkelnäherung
`T₀ = 2π√(L/g)` unterschätzt die Periode bei endlicher Auslenkung. Die
[analytische Pendelreferenz von John Burkardt](https://people.sc.fsu.edu/~jburkardt/m_src/pendulum_nonlinear_exact/pendulum_nonlinear_exact.html)
erklärt die elliptischen Funktionen; [Reinberger et al.](https://arxiv.org/abs/2108.09395)
behandeln exakte Reihen für Bewegung und Periode. Der Test dieses Lernpfads
berechnet K selbst durch Simpsonquadratur und verwendet eine unabhängig
rekursive Taylorreihe für den Zustand bei t=0,5 s.

## Verfahren auswählen

Alle Modellinstanzen besitzen die typisierten Parameter `length` (m, 0,1..10),
`initialAngle` (rad, −1,5..1,5) und `integrator` (dimensionslos, ganze Zahl 0..4).
Die Auswahl wird im Laufmanifest mit Einheiten und Modellannahmen gespeichert.
Unganzzahlige Verfahren werden abgewiesen. Reset erhält die Instanzparameter.

| integrator | Verfahren | Verhalten im Vergleich |
| --- | --- | --- |
| 0 | explizites Euler | erste Ordnung; Energie wächst bei üblichen festen Schritten |
| 1 | symplektisches Euler | zuerst Geschwindigkeit, dann Position; erste Ordnung, oszillierender Energiefehler |
| 2 | klassisches RK4 | vierte Ordnung; kleine Fehler bei hinreichend kleinen Schritten |
| 3 | Velocity Verlet | zweite Ordnung; geeignet für die hier geschwindigkeitsunabhängige Beschleunigung |
| 4 | Dormand–Prince 5(4) | lokale Fehlerschätzung; intern angepasste Schritte oder tatsächlich adaptives Messraster |

C verwendet `experiment`, `numerics`, `units` und die Szene-/Parameterfunktionen.
Physim verwendet die Bindungen `eulerStep`, `rk4Step`, `verletStep` und RK45;
symplektisches Euler zeigt die beiden Aktualisierungen ausdrücklich. Die Analysen
verwenden `series` und `report`. Öffentliche Core-API und Sprachsyntax werden
für diesen Lernpfad nicht erweitert.

Die Szene zeigt die Gewichtskraft rot und die Stangenkraft grün. Beide Pfeile
beginnen an der Masse und verwenden denselben beschrifteten Maßstab von
0,05 m pro Newton. Der gelbe Geschwindigkeitspfeil behält seinen eigenen
Maßstab von 0,3 s. Mit `eᵣ = (sin θ, −cos θ)` gilt
`F_G = (0, −mg)` und `F_Stange = −m(g cos θ + Lω²)eᵣ`. Zusammen erzeugen
sie die tangentiale Pendelbeschleunigung und die radiale Zentripetalbeschleunigung.
Das Modell besitzt eine starre Stange; die Stangenkraft ist kein Modell eines
schlaffen Seils. Die Pfeile sind Darstellungen der Modellkräfte und ändern
weder Integrator noch Messdaten.

## In der App ausprobieren

1. Lege ein Pendelprojekt in der gewünschten Sprache an.
2. Ersetze die Experimentquelle durch die vollständige entsprechende Quelle
   unten und die Analysequelle durch deren Analysegegenstück. Beide Quellen
   einer Projektsprache lassen sich direkt bearbeiten; gemischte Analysesprachen
   sind ebenfalls möglich.
3. Wähle Release, setze den Zeitschritt auf `0.005` s und baue das Projekt.
4. Wähle im Parameterbereich `integrator=0`, starte und speichere etwa 20 s
   simulierte Zeit. Wiederhole mit 1, 2, 3 und 4; verwende dieselbe Länge und
   Anfangsauslenkung. Bei fester Schrittweite wird exakt ein Messwert je
   Ausgabeschritt gespeichert, auch wenn RK45 intern mehrere Schritte benötigt.
5. Wähle die fünf gespeicherten Läufe in der gewünschten Reihenfolge und starte
   die Analyse. Die Legende verbindet Eingabereihenfolge und gespeicherten
   Verfahrensnamen. Vergleichbar sind nur Läufe desselben physikalischen Modells;
   die Auswertung vereinheitlicht deren Einstellungen nicht automatisch.
6. Für ein wirklich adaptives Raster wähle ausdrücklich `integrator=4`, aktiviere
   adaptive Schritte und verwende z. B. Start-/Maximumschritt 0,2 s und Minimum
   `1e-8` s. Die lokalen Toleranzen sind `1e-10` absolut und `1e-8` relativ.
   Andere Verfahren werden in diesem Modus mit einer verständlichen Meldung
   abgewiesen. Siehe [adaptive Schritte](workspace.md#adaptive-simulationsschritte) und [Numerik](numerics.md).

Die Analyse erzeugt zwei Plots mit gemeinsam dargestellten Winkel- beziehungsweise
Energieabweichungskurven und zwei Tabellen: Stichprobenzahl, maximale absolute
Energieabweichung, Dauer und Zahl der Periodenintervalle; daneben die gemessenen
Perioden, soweit vorhanden. Ein positiver Nulldurchgang wird linear zwischen
benachbarten Punkten interpoliert. Erst zwei solche Durchgänge liefern ein
Periodenintervall. Zu kurze oder ruhende Läufe erhalten keinen erfundenen
Periodenwert. Eine fehlende Zeile in der Periodentabelle bedeutet nicht genug
Durchgänge; die Vergleichstabelle führt jeden Lauf weiter auf.

Jede Kurve behält ihr gespeichertes Zeitraster. Es erfolgt keine implizite
Interpolation zwischen Läufen. Berichtsplots dürfen große Datenmengen gemäß
Report-Vertrag reduzieren; `source_count` und vollständiges CSV bleiben verfügbar.
`<prefix>-pendulum_1.csv` bis `_8.csv` enthalten jeweils alle Werte für Zeit,
Winkel, Energie und `E−E(0)` mit Einheiten. Auch gespeicherte C-Läufe sind in der
Physim-Analyse und gespeicherte Sprachläufe in der C-Analyse verwendbar.

## Über Konsole bauen und vergleichen

Erstelle zunächst einen Release-SDK mit allen Beispielen. Linux/macOS benutzen
`.so`, Windows `.dll` und `.exe`; passe die Dateiendungen entsprechend an.
Die folgenden Befehle laufen unter Linux/macOS aus dem SDK-Verzeichnis:

```sh
mkdir pendulum-c
cp examples/documentation/pendulum_main.c pendulum-c/main.c
cp examples/documentation/pendulum_analysis.c pendulum-c/analysis.c
printf '%s\n' physim_project=1 > pendulum-c/physim.project
bin/physim-build --project pendulum-c --sdk . --output pendulum-c/build/Release --physimc bin/physimc --profile Release
```

Der native Builder erzeugt `pendulum-c/build/Release/experiment.so` und
`analysis.so`. Er wählt die passenden Linkeroptionen auf macOS und Linux.
Beide Physim-Module sind mit `--examples` schon im SDK enthalten:

```sh
bin/physim-runner bin/language-pendulum_main.so euler.psrun --steps 4000 --dt 0.005 --param integrator=0
bin/physim-runner bin/language-pendulum_main.so symplectic.psrun --steps 4000 --dt 0.005 --param integrator=1
bin/physim-runner bin/language-pendulum_main.so rk4.psrun --steps 4000 --dt 0.005 --param integrator=2
bin/physim-runner bin/language-pendulum_main.so verlet.psrun --steps 4000 --dt 0.005 --param integrator=3
bin/physim-runner bin/language-pendulum_main.so rk45.psrun --steps 4000 --dt 0.005 --param integrator=4
bin/physim-analysis-runner bin/language-pendulum_analysis.so --runs comparison euler.psrun symplectic.psrun rk4.psrun verlet.psrun rk45.psrun
bin/physim-runner bin/language-pendulum_main.so adaptive.psrun --steps 100000 --dt 0.2 --adaptive --min-dt 1e-8 --max-dt 0.2 --until 20 --param integrator=4
```

Für C ersetze das Experimentmodul durch das gebaute `experiment.so`  und das Analysemodul durch `analysis.so` . Der Analyse-Runner akzeptiert auch nur einen Lauf:
`bin/physim-analysis-runner <modul> <lauf.psrun> <prefix>`.

## Erwartete Ergebnisse und Grenzen

Bei den Standardwerten beträgt die anfängliche Energie etwa 1,464420636 J und die
nichtlineare Periode etwa 2,488805872 s. Die tatsächlichen Referenzwerte
werden unabhängig im Test berechnet. Bei 4000 Schritten zu 0,005 s verlangt der
Test für RK4 und RK45 maximale Energiefehler unter `1e-8` J und Periodenfehler
unter `2e-6` s. Verlet wird mit `1e-4` J und `5e-5` s geprüft; symplektisches
Euler mit `0,02` J. Euler zeigt den deutlichen Energieanstieg im konservativen
Modell. Eine Halbierung von h nähert den Zustand bei t=0,5 s mit den erwarteten
Fehlerfaktoren 2 (Euler/symplektisch), 4 (Verlet) und 16 (RK4) an.

Die gemessene Periodengenauigkeit hängt auch vom Messraster und der linearen
Nulldurchgangsinterpolation ab. Für das gröbere adaptive Raster ist deshalb eine
andere Prüfschranke sinnvoll. RK45 kontrolliert lokale Fehler, garantiert aber
keinen globalen Fehler. Sehr große Schritte können alle Verfahren unbrauchbar
machen; kleines E allein beweist keine korrekte Phase. Bei Winkel null bleibt
das Modell in Ruhe und besitzt keinen messbaren Nulldurchgang. Die Parameter-
begrenzung vermeidet hier Separatrix und Rotationen; daraus entsteht keine
Zusage für andere Pendelmodelle.

Die Quellen berechnen neue Messwerte vor der Übernahme des Modellzustands.
Ungültige Schritte oder überlaufende Messungen veröffentlichen keine Teilwerte.
Der Host beendet einen fehlerhaften Lauf. Der Physim-Adapter markiert nach einem
abgefangenen Laufzeitfehler nur diese Instanz als fehlgeschlagen; erst Reset oder
Neuanlegen hebt das auf. Andere Instanzen und deren Parameter bleiben unabhängig.

## Vollständiger C-Quellcode

### Experiment

```c
#include "physim/experiment.h"
#include "physim/numerics.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* One vacuum model; integrator=0 Euler, 1 symplectic, 2 RK4, 3 Verlet, 4 RK45. */
typedef struct { double state[2], length, angle, method; } pendulum;
static const double gravity=9.80665;
static const char *methods[]={"Euler","symplectic Euler","RK4","velocity Verlet","Dormand-Prince 5(4)"};
static void slope(double time,const double *state,double *out,void *user) {
    (void)time;const pendulum *p=user;out[0]=state[1];out[1]=-gravity/p->length*sin(state[0]);
}
static void acceleration(double time,const double *q,double *out,void *user) {
    (void)time;const pendulum *p=user;out[0]=-gravity/p->length*sin(q[0]);
}
static ps_result measure(ps_context *c,const double state[2]) {
    pendulum *p=c->user;double a=state[0],w=state[1];
    double values[]={a,w,p->length*sin(a),-p->length*cos(a),
        .5*p->length*p->length*w*w+gravity*p->length*(1-cos(a)),a};
    for(unsigned i=0;i<6;i++)if(!isfinite(values[i]))return PS_NUMERIC;
    for(unsigned i=0;i<6;i++)c->values[i]=values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    pendulum *p=c->user;double initial[]={p->angle,0};ps_result r=measure(c,initial);
    if(r==PS_OK){p->state[0]=initial[0];p->state[1]=0;}return r;
}
static ps_result create(ps_context *c) {
    pendulum *p=calloc(1,sizeof *p);if(!p)return PS_MEMORY;c->user=p;
    ps_result r=ps_parameter_define_unit(c,"length","Pendulum length in metres",PS_METRE,1.5,.1,10,&p->length);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"initialAngle","Initial angle in radians",PS_RADIAN,.45,-1.5,1.5,&p->angle);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"integrator","0 Euler; 1 symplectic; 2 RK4; 3 Verlet; 4 RK45",PS_ONE,2,0,4,&p->method);
    if(r!=PS_OK)return r;
    if(p->method!=floor(p->method)){snprintf(c->error,sizeof c->error,"Integrator must be an integer from 0 to 4");return PS_INVALID;}
    ps_unit rate={{0,0,-1,0,0,0,0},1,"rad/s"};
    const char *names[]={"angle","angular_velocity","position.x","position.y","energy","sensor.angle"};
    ps_unit units[]={PS_RADIAN,rate,PS_METRE,PS_METRE,PS_JOULE,PS_RADIAN};
    for(unsigned i=0;i<6;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,
        "model=point pendulum, massless rigid rod, vacuum\nlength_m=%.17g\ninitial_angle_rad=%.17g\nmass_kg=1\ngravity_m_s2=9.80665\nintegrator=%s\nrk45_absolute_tolerance=1e-10\nrk45_relative_tolerance=1e-8\nexcluded=drag, drive, rod inertia, contacts, sensor noise\n",p->length,p->angle,methods[(unsigned)p->method]);
    return reset(c);
}
static ps_ode_options options(double proposed,double minimum,double maximum) {
    ps_ode_options o=ps_ode_options_default();o.absolute_tolerance=1e-10;o.relative_tolerance=1e-8;
    o.initial_step=proposed;o.minimum_step=minimum;o.maximum_step=maximum;return o;
}
static ps_result step(ps_context *c,double dt) {
    if(!isfinite(dt) || dt<=0 || !isfinite(c->time_s+dt) || c->time_s+dt==c->time_s)return PS_INVALID;
    pendulum *p=c->user;double next[]={p->state[0],p->state[1]};ps_result r=PS_OK;
    if(p->method==PS_SYMPLECTIC) {
        double d[2];slope(c->time_s,next,d,p);ps_symplectic_step(&next[0],&next[1],d[1],dt);
    } else if(p->method==PS_VERLET)r=ps_verlet_step(acceleration,p,c->time_s,dt,&next[0],&next[1],1);
    else if(p->method==PS_RK45) {
        ps_ode_options o=options(dt,fmin(1e-14,dt),dt);
        r=ps_ode_integrate(slope,p,c->time_s,c->time_s+dt,next,2,&o,NULL);
    } else r=ps_ode_step((ps_integrator)(unsigned)p->method,slope,p,c->time_s,dt,next,2);
    if(r==PS_OK)r=measure(c,next);
    if(r==PS_OK){p->state[0]=next[0];p->state[1]=next[1];}return r;
}
static ps_result adaptive_step(ps_context *c,double dt,double minimum,double maximum,ps_step_interval *interval) {
    pendulum *p=c->user;
    if(p->method!=PS_RK45){snprintf(c->error,sizeof c->error,"Adaptive mode requires integrator=4");return PS_INVALID;}
    double next[]={p->state[0],p->state[1]};ps_ode_options o=options(dt,minimum,maximum);
    ps_ode_report report;ps_ode_diagnostic diagnostic;
    ps_result r=ps_ode_step_diagnosed(slope,p,c->time_s,c->time_s+dt,next,2,&o,&report,&diagnostic);
    if(r!=PS_OK){snprintf(c->error,sizeof c->error,"%s",ps_ode_diagnostic_string(diagnostic.reason));return r;}
    r=measure(c,next);
    if(r==PS_OK){p->state[0]=next[0];p->state[1]=next[1];*interval=(ps_step_interval){report.reached_time-c->time_s,report.next_step};}
    return r;
}
static void scene(ps_context *c,ps_scene *s) {
    pendulum *p=c->user;ps_vec3 origin=ps_v3(0,0,0),bob=ps_v3(c->values[2],c->values[3],0);
    ps_scene_add_id(s,1,PS_LINE,origin,bob,0,0xb5c4d8ff);
    ps_scene_add_id(s,2,PS_SPHERE,origin,origin,.045,0xe6edf3ff);
    ps_scene_add_id(s,3,PS_SPHERE,bob,bob,.12,0x53dec2ff);
    ps_vec3 v=ps_v3(p->length*cos(c->values[0])*c->values[1],p->length*sin(c->values[0])*c->values[1],0);
    ps_scene_add_id(s,4,PS_ARROW,bob,ps_vadd(bob,ps_vscale(v,.3)),0,0xf2c572ff);
    (void)ps_scene_label_id(s,5,origin,"Aufhaengung",0xb5c4d8ff);
    (void)ps_scene_label_id(s,6,bob,"Pendelmasse",0x53dec2ff);
    (void)ps_scene_group(s,100,0,"Pendel");(void)ps_scene_group(s,101,100,"Bewegte Masse");
    (void)ps_scene_set_parent(s,1,100);(void)ps_scene_set_parent(s,2,100);(void)ps_scene_set_parent(s,3,101);
    (void)ps_scene_set_parent(s,4,3);(void)ps_scene_set_parent(s,5,2);(void)ps_scene_set_parent(s,6,3);
    /* One kilogram; force arrows use 0.05 metres per newton. */
    ps_vec3 weight_end=ps_vadd(bob,ps_v3(0,-.05*gravity,0));
    double constraint=gravity*cos(c->values[0])+p->length*c->values[1]*c->values[1];
    ps_vec3 rod_end=ps_vadd(bob,ps_vscale(bob,-.05*constraint/p->length));
    ps_scene_add_id(s,7,PS_ARROW,bob,weight_end,0,0xe87979ff);
    ps_scene_add_id(s,8,PS_ARROW,bob,rod_end,0,0x91d28aff);
    (void)ps_scene_label_id(s,10,weight_end,"Gewicht · 0.05 m/N",0xe87979ff);
    (void)ps_scene_label_id(s,11,rod_end,"Stangenkraft · 0.05 m/N",0x91d28aff);
    (void)ps_scene_set_parent(s,7,3);(void)ps_scene_set_parent(s,8,3);
    (void)ps_scene_set_parent(s,10,7);(void)ps_scene_set_parent(s,11,8);
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .capabilities=PS_EXPERIMENT_SCENE_HIERARCHY|PS_EXPERIMENT_ADAPTIVE_STEPS,.name="Pendulum integrator comparison",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy,.adaptive_step=adaptive_step};return &api;
}
```

### Analyse für einen bis acht Läufe

```c
#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static const char *methods[]={"Euler","symplectic Euler","RK4","velocity Verlet","Dormand-Prince 5(4)"};
static ps_result label(const ps_dataset_info *info,size_t index,char text[96]) {
    for(unsigned i=0;i<5;i++) {
        char token[64];snprintf(token,sizeof token,"\nintegrator=%s\n",methods[i]);
        if(strstr(info->metadata,token)) {snprintf(text,96,"Run %u / %s",(unsigned)index+1,methods[i]);return PS_OK;}
    }
    return PS_INVALID;
}
static ps_result plot(ps_report *report,const char *title,ps_unit unit,ps_plot_handle *handle) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    snprintf(info.x_label,sizeof info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result r=ps_report_unit_from(PS_SECOND,&info.x_unit);if(r==PS_OK)r=ps_report_unit_from(unit,&info.y_unit);
    return r==PS_OK?ps_report_add_plot(report,&info,handle):r;
}
static ps_result scalar(ps_analysis_context *c,ps_series s,uint64_t at,double *value) {
    size_t n=0;ps_result r=ps_series_read(c,s,at,value,1,&n);return r==PS_OK && n!=1?PS_INVALID:r;
}
static ps_result analyze_many(const char *const *inputs,size_t count,const char *prefix) {
    if(!count || count>8)return PS_INVALID;
    ps_analysis_context *c=NULL;ps_report *report=NULL;bool recovered=false;
    ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK)r=ps_report_create("Pendulum integrators","Vacuum motion, energy drift and crossing periods",&report);
    ps_plot_handle angle_plot={0},energy_plot={0};ps_table_handle summary={0},periods={0};
    if(r==PS_OK)r=plot(report,"Angle",PS_RADIAN,&angle_plot);
    if(r==PS_OK)r=plot(report,"Energy drift",PS_JOULE,&energy_plot);
    ps_table_info info={0};strcpy(info.title,"Run comparison");info.columns=4;
    const char *titles[]={"Samples","Maximum energy drift","Duration","Period intervals"};
    ps_unit units[]={PS_ONE,PS_JOULE,PS_SECOND,PS_ONE};
    for(unsigned i=0;r==PS_OK && i<4;i++){strcpy(info.column[i].label,titles[i]);r=ps_report_unit_from(units[i],&info.column[i].unit);}
    if(r==PS_OK)r=ps_report_add_table(report,&info,&summary);
    memset(&info,0,sizeof info);strcpy(info.title,"Measured periods");info.columns=1;
    strcpy(info.column[0].label,"Mean positive-crossing period");
    if(r==PS_OK)r=ps_report_unit_from(PS_SECOND,&info.column[0].unit);
    if(r==PS_OK)r=ps_report_add_table(report,&info,&periods);
    for(size_t i=0;r==PS_OK && i<count;i++) {
        ps_dataset dataset={0};ps_series time={0},angle={0},energy={0},drift={0};ps_dataset_info details;
        r=ps_analysis_open_run(c,inputs[i],&dataset);if(r==PS_RECOVERED){recovered=true;r=PS_OK;}
        if(r==PS_OK)r=ps_dataset_describe(c,dataset,&details);
        char name[96];if(r==PS_OK)r=label(&details,i,name);
        if(r==PS_OK)r=ps_dataset_series(c,dataset,"time",&time);
        if(r==PS_OK)r=ps_dataset_series(c,dataset,"angle",&angle);
        if(r==PS_OK)r=ps_dataset_series(c,dataset,"energy",&energy);
        double initial=0,first=0,last=0;
        if(r==PS_OK)r=scalar(c,energy,0,&initial);
        if(r==PS_OK)r=scalar(c,time,0,&first);
        if(r==PS_OK)r=scalar(c,time,details.samples-1,&last);
        if(r==PS_OK)r=ps_series_affine(c,energy,1,(ps_quantity){-initial,PS_JOULE},&drift);
        if(r==PS_OK)r=ps_report_add_series(report,angle_plot,c,time,angle,name,PS_PLOT_LINE);
        if(r==PS_OK)r=ps_report_add_series(report,energy_plot,c,time,drift,name,PS_PLOT_LINE);
        ps_statistics stats={0};if(r==PS_OK)r=ps_series_statistics(c,drift,&stats);
        double previous_a=0,previous_t=0,last_cross=0,sum=0;unsigned intervals=0;bool crossed=false;
        for(uint64_t at=0;r==PS_OK && at<details.samples;at++) {
            double t,a;r=scalar(c,time,at,&t);if(r==PS_OK)r=scalar(c,angle,at,&a);
            if(r!=PS_OK)break;
            if(at && previous_a<0 && a>=0) {
                double cross=previous_t+(t-previous_t)*(-previous_a)/(a-previous_a);
                if(crossed){sum+=cross-last_cross;intervals++;}crossed=true;last_cross=cross;
            }
            previous_t=t;previous_a=a;
        }
        ps_table_row row={0};snprintf(row.label,sizeof row.label,"%s",name);
        row.values[0]=(double)details.samples;row.values[1]=fmax(fabs(stats.min),fabs(stats.max));
        row.values[2]=last-first;row.values[3]=intervals;
        if(r==PS_OK)r=ps_report_add_row(report,summary,&row);
        if(r==PS_OK && intervals){row.values[0]=sum/intervals;r=ps_report_add_row(report,periods,&row);}
        char path[4096];int n=snprintf(path,sizeof path,"%s-pendulum_%u.csv",prefix,(unsigned)i+1);
        ps_series columns[]={time,angle,energy,drift};
        if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,columns,4,path);
        if(dataset.owner)ps_dataset_close(c,dataset);
    }
    if(r==PS_OK){char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);}
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
static ps_result analyze(const char *input,const char *prefix){return analyze_many(&input,1,prefix);}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Pendulum integrator comparison",.run=analyze,.run_many=analyze_many};return &api;
}
```

## Vollständiger Physim-Quellcode

### Experiment

```physim
// Same vacuum model and integrator numbers as pendulum_main.c.
let radians = Unit(0,0,0,0,0,0,0,1,"rad")
let metres = Unit(1,0,0,0,0,0,0,1,"m")
let rate = Unit(0,0,-1,0,0,0,0,1,"rad/s")
let joules = Unit(2,1,-2,0,0,0,0,1,"J")
let one = Unit(0,0,0,0,0,0,0,1,"1")
let length = parameterWithUnit("length",metres,1.5,0.1,10,"Pendulum length in metres")
let initialAngle = parameterWithUnit("initialAngle",radians,0.45,-1.5,1.5,"Initial angle in radians")
let method = parameterWithUnit("integrator",one,2,0,4,"0 Euler; 1 symplectic; 2 RK4; 3 Verlet; 4 RK45")
let methods = ["Euler","symplectic Euler","RK4","velocity Verlet","Dormand-Prince 5(4)"]
let gravity = 9.80665
let ca = Channel("angle",radians,"angle")
let cw = Channel("angular_velocity",rate,"angular_velocity")
let cx = Channel("position.x",metres,"position.x")
let cy = Channel("position.y",metres,"position.y")
let ce = Channel("energy",joules,"energy")
let cs = Channel("sensor.angle",radians,"sensor.angle")
var state = [initialAngle,0.0]
func slope(time: Float64, value: [Float64]) -> [Float64]:
    return [value[1],-gravity / length * sin(value[0])]
func acceleration(time: Float64, position: [Float64]) -> [Float64]:
    return [-gravity / length * sin(position[0])]
func measure(value: [Float64]):
    let x = length * sin(value[0])
    let y = -length * cos(value[0])
    let energy = 0.5 * length * length * value[1] * value[1] + gravity * length * (1 - cos(value[0]))
    ca.sample(value[0])
    cw.sample(value[1])
    cx.sample(x)
    cy.sample(y)
    ce.sample(energy)
    cs.sample(value[0])
func create():
    assert(method == floor(method),"Integrator must be an integer from 0 to 4")
    metadata("model=point pendulum, massless rigid rod, vacuum\nlength_m=" + String(length) + "\ninitial_angle_rad=" + String(initialAngle) + "\nmass_kg=1\ngravity_m_s2=9.80665\nintegrator=" + methods[Int64(method)] + "\nrk45_absolute_tolerance=1e-10\nrk45_relative_tolerance=1e-8\nexcluded=drag, drive, rod inertia, contacts, sensor noise\n")
func reset():
    let initial = [initialAngle,0.0]
    measure(initial)
    state = initial
func step(dt: Float64):
    assert(dt > 0 && simulationTime() + dt > simulationTime(),"Positive representable step required")
    var next = state
    if method == 0:
        next = eulerStep(slope,state,simulationTime(),dt)
    else if method == 1:
        let velocity = state[1] - dt * gravity / length * sin(state[0])
        next = [state[0] + dt * velocity,velocity]
    else if method == 2:
        next = rk4Step(slope,state,simulationTime(),dt)
    else if method == 3:
        next = verletStep(acceleration,state,simulationTime(),dt)
    else:
        next = rk45IntegrateWithSteps(slope,state,simulationTime(),simulationTime() + dt,
            1e-10,1e-8,100000,dt,min(1e-14,dt),dt)
    measure(next)
    state = next
func adaptiveStep(dt: Float64, minimum: Float64, maximum: Float64) -> StepInterval:
    assert(method == 4,"Adaptive mode requires integrator=4")
    let time = simulationTime()
    let result = rk45StepReported(slope,state,time,time + dt,1e-10,1e-8,100000,dt,minimum,maximum)
    measure(result.state)
    state = result.state
    return StepInterval(result.reachedTime - time,result.nextStep)
func scene():
    let origin = Vec3(0,0,0)
    let bob = Vec3(length * sin(state[0]),-length * cos(state[0]),0)
    let velocity = Vec3(length * cos(state[0]) * state[1],length * sin(state[0]) * state[1],0)
    line(origin,bob,0,0xB5C4D8FF,1)
    sphere(origin,0.045,0xE6EDF3FF,2)
    sphere(bob,0.12,0x53DEC2FF,3)
    arrow(bob,bob + 0.3 * velocity,0,0xF2C572FF,4)
    label(origin,"Aufhaengung",0xB5C4D8FF,5)
    label(bob,"Pendelmasse",0x53DEC2FF,6)
    group("Pendel",100,0)
    group("Bewegte Masse",101,100)
    sceneParent(1,100)
    sceneParent(2,100)
    sceneParent(3,101)
    sceneParent(4,3)
    sceneParent(5,2)
    sceneParent(6,3)
    // One kilogram; force arrows use 0.05 metres per newton.
    let weightEnd = bob + 0.05 * Vec3(0,-gravity,0)
    let constraint = gravity * cos(state[0]) + length * state[1] * state[1]
    let rodEnd = bob - (0.05 * constraint / length) * bob
    arrow(bob,weightEnd,0,0xE87979FF,7)
    arrow(bob,rodEnd,0,0x91D28AFF,8)
    label(weightEnd,"Gewicht · 0.05 m/N",0xE87979FF,10)
    label(rodEnd,"Stangenkraft · 0.05 m/N",0x91D28AFF,11)
    sceneParent(7,3)
    sceneParent(8,3)
    sceneParent(10,7)
    sceneParent(11,8)
```

### Analyse für einen bis acht Läufe

```physim
let methods = ["Euler","symplectic Euler","RK4","velocity Verlet","Dormand-Prince 5(4)"]
func runLabel(run: Dataset, index: Int64) -> String:
    let text = run.metadata()
    for method in methods:
        if text.contains("\nintegrator=" + method + "\n"):
            return "Run " + String(index + 1) + " / " + method
    assert(false,"Pendulum integrator metadata required")
    return ""
func analyze():
    assert(inputCount() > 0,"Select at least one pendulum run")
    report("Pendulum integrators")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let seconds = Unit(0,0,1,0,0,0,0,1,"s")
    let joules = Unit(2,1,-2,0,0,0,0,1,"J")
    let first = Dataset(0)
    let firstTime = first.series("time")
    let firstAngle = first.series("angle")
    let firstEnergy = first.series("energy")
    let firstDrift = firstEnergy.affine(1,-firstEnergy.value(0),joules)
    let anglePlot = firstAngle.plot(firstTime,"Angle",runLabel(first,0))
    let energyPlot = firstDrift.plot(firstTime,"Energy drift",runLabel(first,0))
    first.close()
    let summary = Table("Run comparison",["Samples","Maximum energy drift","Duration","Period intervals"],[one,joules,seconds,one])
    let periods = Table("Measured periods",["Mean positive-crossing period"],[seconds])
    for index in 0..<inputCount():
        let run = Dataset(index)
        let name = runLabel(run,index)
        let time = run.series("time")
        let angle = run.series("angle")
        let energy = run.series("energy")
        let drift = energy.affine(1,-energy.value(0),joules)
        if index > 0:
            anglePlot.curve(time,angle,name)
            energyPlot.curve(time,drift,name)
        var lastCross = 0.0
        var periodSum = 0.0
        var intervals: Int64 = 0
        var crossed = false
        for i in 1..<angle.count():
            let previous = angle.value(i - 1)
            let current = angle.value(i)
            if previous < 0 && current >= 0:
                let previousTime = time.value(i - 1)
                let crossing = previousTime + (time.value(i) - previousTime) * (-previous) / (current - previous)
                if crossed:
                    periodSum += crossing - lastCross
                    intervals += 1
                crossed = true
                lastCross = crossing
        let maximum = max(abs(drift.minimum()),abs(drift.maximum()))
        summary.row(name,[Quantity(Float64(angle.count()),one),Quantity(maximum,joules),
            Quantity(time.value(time.count() - 1) - time.value(0),seconds),Quantity(Float64(intervals),one)])
        if intervals > 0:
            periods.row(name,[Quantity(periodSum / Float64(intervals),seconds)])
        Series.exportColumns([time,angle,energy,drift],"pendulum_" + String(index + 1))
        run.close()
```

## Automatisierte Prüfung

`pendulum_tutorial` prüft fünf Verfahren in beiden Sprachen, Modellinstanzen,
SI-Metadaten, alle Szenenfelder, nichtlineare Referenzperiode und Taylorverfeinerung,
abweichende Längen und Winkel, adaptive Raster, acht gemischte Mehrlaufanalysen
sowie zwei kurze Einzellaufanalysen. Ein separater C-Prüfer vergleicht Bericht
und ursprüngliche Läufe; ein unabhängiger Parser kontrolliert vollständiges CSV.
`pendulum_tutorial_workflow` prüft den Weg durch beide App-Projektsprachen,
Parameterwahl, angehaltene Einzelschritte, Laufarchiv und Analyse.
`documentation_pendulum_source` stellt sicher, dass diese vier Codeblöcke den
tatsächlich gebauten Quelldateien entsprechen.
