# Elastischer und inelastischer Stoß

Lernziel: Untersuche den zentralen Stoß zweier Kugeln mit gleichen und ungleichen
Massen. Prüfe Impulserhaltung, Geschwindigkeiten und die Aufteilung der Energie.
Vollständige Experimente und Auswertungen liegen in C und Physim vor.

## Modell und Gleichungen

Zwei homogene starre Kugeln bewegen sich im Vakuum auf der x-Achse. Ihre Radien
betragen jeweils 0,2 m, die Anfangspositionen −1 m und +1 m. Schwerkraft,
Widerstand, Reibung und Anfangsrotation sind ausgeschlossen. Die Restitution e
liegt zwischen 0 und 1: e=1 beschreibt den elastischen, 0<e<1 den teilweise
inelastischen und e=0 den vollkommen inelastischen zentralen Stoß. Bei e=0 bewegen
sich die Kugeln anschließend mit gleicher Geschwindigkeit; das Modell erstellt
keinen gemeinsamen Körper und keine zusätzliche Klebeverbindung.

Für Annäherung `g = uA−uB > 0`, Gesamtmasse `M = mA+mB` und reduzierte Masse
`μ = mA mB/M` gilt:

- Stoßzeit `tc = (2−2r)/g = 1,6/g`.
- Positiver Impulsbetrag `J = (1+e) μg`; auf A wirkt `−J`, auf B `+J`.
- `vA = uA−J/mA`, `vB = uB+J/mB`.
- Impuls `P = mA vA+mB vB` bleibt erhalten.
- `K = ½mA vA²+½mB vB²` und `D = ½μ(1−e²)g²`; `K+D` bleibt erhalten.

Die [MIT-Unterlagen zur Stoßtheorie, Kapitel 15](https://ocw.mit.edu/courses/8-01sc-classical-mechanics-fall-2016/mit8_01scs22_chapter15.pdf)
erläutern Restitution und die Erhaltungsgrößen. Der Lernpfad verwendet einen
instantanen Normalimpuls; Kontaktkraftverlauf und Deformation werden nicht
berechnet. Die dissipierte Energie wird unabhängig aus Restitution, reduzierter
Masse und Annäherungsgeschwindigkeit bestimmt. Die Bilanz ist dadurch eine
Kontrolle der berechneten Nachstoßgeschwindigkeiten. Bei `uA≤uB` bleiben die
anfangs getrennten Kugeln ohne Stoß.

Die kontinuierliche Kollisionsabfrage sucht die erste Berührung entlang der
linearen Bewegung. Das Experiment integriert bis zum Kontakt, löst den Impuls
und integriert die verbleibende Schrittzeit mit den neuen Geschwindigkeiten.
Nach diesem einen Stoß ist die Bewegung wieder kräftefrei. Dieser Ablauf ist
für das isolierte zentrale Paar vollständig; Mehrkörperkontakte, weitere
Stoßereignisse und beschleunigte oder gekrümmte Bahnen brauchen eine eigene
Ereignissteuerung. [Kollisionserkennung und Mechanik](mechanics.md).

## Parameter und Messwerte

| Parameter | Standard | Bereich und Einheit |
| --- | --- | --- |
| `massA`, `massB` | jeweils 1 | 0,1..10 kg |
| `velocityA` | +0,6 | −5..5 m/s |
| `velocityB` | −0,6 | −5..5 m/s |
| `restitution` | 1 | dimensionslos, 0..1 |

Parameter gehören zu jeder Modellinstanz und bleiben bei Reset erhalten.
Manifest und Kanaldefinitionen speichern SI-Dimensionen und Einheiten. C nutzt
`experiment`, `mechanics`, `collision`, `units`, `series` und `report`; Physim
verwendet deren `Body`-, `Sweep`-, Kontakt-, Parameter-, Kanal- und Analysebindungen.
Die Bibliothek berechnet die Kontaktantwort; die unabhängige Testreferenz benutzt
die obigen geschlossenen Formeln.

Elf Kanäle erfassen Positionen und Geschwindigkeiten beider Kugeln, K, P, D,
K+D, den Impuls auf A sowie Stoßzeit und Stoßzahl. `a.impulse` enthält nur im
Ereignisschritt einen Wert und hat die Einheit kg m/s (gleich N s).
`collision.time` ist erst bei `collision.count=1` definiert; vorher speichert
das Modell null. Die Ereignistabelle bleibt bei ausbleibendem Stoß leer.
Ein Impuls ist kein Kraftwert. Die Pfeile in der Szene zeigen Geschwindigkeiten
mit dem gezeichneten Maßstab 1 m pro m/s; der Kontaktmarker kennzeichnet den
Ort des bereits erfolgten Ereignisses.

## In der App durchführen

1. Lege ein Projekt mit der Vorlage **Kugelstoß & Medien** in C oder Physim an.
2. Ersetze dessen Experiment- und Analysequelle durch die entsprechenden
   vollständigen Quellen unten. Wähle Release, setze h=0,005 s und baue.
3. Starte mit `restitution=1` und speichere mindestens 2 s simulierte Zeit.
   Analysiere diesen einzelnen Lauf.
4. Wiederhole mit `restitution=0.5` und 0 bei denselben Massen und Geschwindigkeiten.
   Die Auswertung zeigt, wie K abnimmt und D zunimmt, während K+D konstant bleibt.
5. Setze `massA=2`, lasse `massB=1` und wiederhole den vollkommen inelastischen
   Fall. Beide Kugeln bewegen sich anschließend gemeinsam nach rechts; ein
   vollkommen inelastischer Stoß muss im gewählten Bezugssystem nicht zum
   Stillstand führen.
6. Prüfe ohne Annäherung (`velocityA=-0.6`, `velocityB=0.6`) die leere
   Ereignistabelle. Für eine grobe Aufzeichnung teste `velocityA=5`,
   `velocityB=-5`, h=0,3 s: Die Stoßzeit liegt bei 0,16 s zwischen Messpunkten.

Die Analyse verarbeitet genau einen ausgewählten Lauf und erzeugt vier Plots:
Positionen, Geschwindigkeiten, Gesamtimpuls und Energieabrechnung mit K, D und
K+D. Eine Tabelle nennt Zeilenzahl, maximale Impulsabweichung, D am Laufende,
maximale Bilanzabweichung und Stoßzahl. Die Ereignistabelle enthält tatsächliche
Stoßzeit und gesamten Impuls auf A, sofern ein Stoß aufgezeichnet ist.
C- und Physim-Auswertung lesen beide Laufsprachen gleich. Der CSV-Export
`<prefix>-collision.csv` enthält sämtliche elf Kanäle plus Zeitachse.

## Aus einem SDK bauen und gespeicherte Läufe analysieren

Baue einen Release-SDK mit `--examples`. Folgende Befehle gelten für macOS/Linux
im SDK-Verzeichnis; unter Windows sind Dateiendungen und Shellbefehle anzupassen.

```sh
mkdir collision-c
cp examples/documentation/collision_main.c collision-c/main.c
cp examples/documentation/collision_analysis.c collision-c/analysis.c
printf '%s\n' physim_project=1 > collision-c/physim.project
bin/physim-build --project collision-c --sdk . --output collision-c/build/Release --physimc bin/physimc --profile Release
```

Das Experiment liegt danach als `collision-c/build/Release/experiment.so`, die
Analyse als `analysis.so` vor. Der native Builder wählt die Plattformoptionen.
Die beiden Physim-Module sind im SDK bereits enthalten:

```sh
bin/physim-runner bin/language-collision_main.so elastic.psrun --steps 400 --dt 0.005 --param restitution=1
bin/physim-runner bin/language-collision_main.so partial.psrun --steps 400 --dt 0.005 --param restitution=0.5
bin/physim-runner bin/language-collision_main.so inelastic.psrun --steps 400 --dt 0.005 --param restitution=0
bin/physim-runner bin/language-collision_main.so unequal.psrun --steps 400 --dt 0.005 --param massA=2 --param restitution=0
bin/physim-analysis-runner bin/language-collision_analysis.so unequal.psrun unequal-result
bin/physim-analysis-runner collision-c/build/Release/analysis.so unequal.psrun unequal-result-c
```

Für ein C-Experiment ersetze das Experimentmodul durch den genannten C-Pfad.
Verwende neue Ausgabepfade für jeden Lauf und jedes Analyseergebnis. Bereits
existierende Lauf- oder Ergebnisdateien werden nicht überschrieben.

## Erwartete Ergebnisse und Grenzen

Für mA=mB=1 kg und uA=−uB=0,6 m/s beträgt tc=4/3 s, P=0 und K+D=0,36 J.

| e | vA danach | vB danach | K danach | D |
| --- | --- | --- | --- | --- |
| 1 | −0,6 m/s | +0,6 m/s | 0,36 J | 0 |
| 0,5 | −0,3 m/s | +0,3 m/s | 0,09 J | 0,27 J |
| 0 | 0 | 0 | 0 | 0,36 J |

Bei mA=2 kg, mB=1 kg und e=0 beträgt P=0,6 kg m/s, die gemeinsame
Nachstoßgeschwindigkeit 0,2 m/s, K=0,06 J und D=0,48 J. Die Gesamtbilanz bleibt
0,54 J. Nicht erhaltene Bewegungsenergie ist in diesem Modell Dissipation;
Abweichungen von P oder K+D wären dagegen numerische Fehler.

Die Stoßzeit wird innerhalb des Schritts berechnet, aber Messwerte werden nur
an den regulären Schrittenden gespeichert. Die Darstellung verbindet diese
Punkte; ein gezeichneter Übergang zwischen Geschwindigkeitswerten ist keine
berechnete endliche Stoßdauer. Große Abtastschritte verdecken Zwischenpositionen,
obwohl die Kontaktantwort richtig ist. Für den feineren Verlauf h verkleinern
und die gespeicherte Ereigniszeit prüfen. Vergleichsschranken berücksichtigen
die Rundung von Kontaktgeometrie und Zeit; das Modell garantiert keine exakte
Arithmetik bei beliebigen Größenverhältnissen.

Die Quelle berechnet neue Messwerte vor der Übernahme des Zustands. Fehlgeschlagene
Schritte veröffentlichen keine Teilwerte. Der Physim-Adapter markiert bei einem
Laufzeitfehler nur die betroffene Instanz als fehlgeschlagen; Reset oder Neuanlegen
stellt sie wieder her. Das Modell schließt komplexe Deformation, Rotation,
Tangentialreibung, Materialabhängigkeit der Restitution, Wände, mehrere Kugeln
und nachfolgende Kontakte ausdrücklich aus. Dafür stehen gesonderte
[Mechanik- und Kontaktmodelle](contact-world.md) zur Verfügung.

## Vollständiger C-Quellcode

### Experiment

```c
#include "physim/collision.h"
#include "physim/experiment.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* One central collision in vacuum; no external force, friction or initial spin. */
typedef struct {
    double mass_a,mass_b,speed_a,speed_b,restitution;
    ps_body a,b;
    double lost,event_time,impulse;
    ps_vec3 event_point;
    bool collided;
} experiment;
static const double radius=.2;
static ps_result measure(ps_context *c,const experiment *e) {
    double ka,kb;ps_result r=ps_body_kinetic_energy(&e->a,&ka);
    if(r==PS_OK)r=ps_body_kinetic_energy(&e->b,&kb);
    if(r!=PS_OK)return r;
    double values[]={e->a.position_m.x,e->b.position_m.x,e->a.velocity_m_s.x,e->b.velocity_m_s.x,
        ka+kb,e->mass_a*e->a.velocity_m_s.x+e->mass_b*e->b.velocity_m_s.x,e->lost,
        ka+kb+e->lost,e->impulse,e->event_time,e->collided?1:0};
    for(unsigned i=0;i<11;i++)if(!isfinite(values[i]))return PS_NUMERIC;
    for(unsigned i=0;i<11;i++)c->values[i]=values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e=c->user,next=*e;
    ps_result r=ps_body_sphere(e->mass_a,radius,&next.a);
    if(r==PS_OK)r=ps_body_sphere(e->mass_b,radius,&next.b);
    if(r!=PS_OK)return r;
    next.a.position_m=ps_v3(-1,0,0);next.b.position_m=ps_v3(1,0,0);
    next.a.velocity_m_s=ps_v3(e->speed_a,0,0);next.b.velocity_m_s=ps_v3(e->speed_b,0,0);
    next.lost=next.event_time=next.impulse=0;next.event_point=ps_v3(0,0,0);next.collided=false;
    r=measure(c,&next);if(r==PS_OK)*e=next;return r;
}
static ps_result create(ps_context *c) {
    experiment *e=calloc(1,sizeof *e);if(!e)return PS_MEMORY;c->user=e;
    ps_result r=ps_parameter_define_unit(c,"massA","Sphere A mass in kilograms",PS_KILOGRAM,1,.1,10,&e->mass_a);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"massB","Sphere B mass in kilograms",PS_KILOGRAM,1,.1,10,&e->mass_b);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"velocityA","Sphere A initial X velocity",PS_VELOCITY,.6,-5,5,&e->speed_a);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"velocityB","Sphere B initial X velocity",PS_VELOCITY,-.6,-5,5,&e->speed_b);
    if(r==PS_OK)r=ps_parameter_define_unit(c,"restitution","Normal coefficient of restitution",PS_ONE,1,0,1,&e->restitution);
    if(r!=PS_OK)return r;
    ps_unit momentum={{1,1,-1,0,0,0,0},1,"kg m/s"};
    const char *names[]={"a.position","b.position","a.velocity","b.velocity","energy","momentum.x",
        "energy.dissipated","energy.balance","a.impulse","collision.time","collision.count"};
    ps_unit units[]={PS_METRE,PS_METRE,PS_VELOCITY,PS_VELOCITY,PS_JOULE,momentum,PS_JOULE,PS_JOULE,momentum,PS_SECOND,PS_ONE};
    for(unsigned i=0;i<11;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,
        "model=one central collision of two rigid spheres\nmedium=vacuum\ngravity=none\nfriction=0\ninitial_spin=0\nradius_m=0.2\ninitial_positions_m=-1,1\nmass_a_kg=%.17g\nmass_b_kg=%.17g\nvelocity_a_m_s=%.17g\nvelocity_b_m_s=%.17g\nrestitution=%.17g\nccd=linear sphere sweep, impulse, remaining time\ndissipation=0.5*reduced_mass*(1-e^2)*closing_speed^2\nevent_time_valid=collision.count==1\nexcluded=external forces, rotation, deformation, further contacts\n",e->mass_a,e->mass_b,e->speed_a,e->speed_b,e->restitution);
    return reset(c);
}
static ps_result step(ps_context *c,double dt) {
    if(!isfinite(dt) || dt<=0 || !isfinite(c->time_s+dt) || c->time_s+dt==c->time_s)return PS_INVALID;
    experiment *e=c->user,next=*e;ps_vec3 zero=ps_v3(0,0,0);next.impulse=0;
    double first=dt;bool touching=false;ps_sweep_hit hit;ps_result r=PS_OK;
    if(!next.collided) {
        r=ps_sweep_spheres(&next.a,radius,ps_vscale(next.a.velocity_m_s,dt),&next.b,radius,
            ps_vscale(next.b.velocity_m_s,dt),&hit,&touching);
        if(r!=PS_OK)return r;
        if(touching)first=dt*hit.fraction;
    }
    if(first>0)r=ps_body_step(&next.a,zero,zero,first);
    if(r==PS_OK && first>0)r=ps_body_step(&next.b,zero,zero,first);
    if(r==PS_OK && touching) {
        double closing=next.a.velocity_m_s.x-next.b.velocity_m_s.x;
        double reduced=next.mass_a*next.mass_b/(next.mass_a+next.mass_b);
        ps_vec3 impulse;
        r=ps_contact_resolve(&next.a,&next.b,&hit.contact,next.restitution,0,&impulse);
        if(r==PS_OK) {
            next.impulse=impulse.x;next.lost=.5*reduced*(1-next.restitution*next.restitution)*closing*closing;
            next.event_time=c->time_s+first;next.event_point=hit.contact.point_m;next.collided=true;
            double remaining=dt-first;
            if(remaining>0)r=ps_body_step(&next.a,zero,zero,remaining);
            if(r==PS_OK && remaining>0)r=ps_body_step(&next.b,zero,zero,remaining);
        }
    }
    if(r==PS_OK)r=measure(c,&next);
    if(r==PS_OK)*e=next;
    return r;
}
static void scene(ps_context *c,ps_scene *s) {
    experiment *e=c->user;
    ps_scene_add_id(s,1,PS_SPHERE,e->a.position_m,e->a.position_m,radius,0x53dec2ff);
    ps_scene_add_id(s,2,PS_ARROW,e->a.position_m,ps_vadd(e->a.position_m,e->a.velocity_m_s),.006,0x53dec2ff);
    (void)ps_scene_label_id(s,3,e->a.position_m,"Kugel A",0x53dec2ff);
    ps_scene_add_id(s,4,PS_SPHERE,e->b.position_m,e->b.position_m,radius,0xf2c572ff);
    ps_scene_add_id(s,5,PS_ARROW,e->b.position_m,ps_vadd(e->b.position_m,e->b.velocity_m_s),.006,0xf2c572ff);
    (void)ps_scene_label_id(s,6,e->b.position_m,"Kugel B",0xf2c572ff);
    (void)ps_scene_label_id(s,7,ps_v3(0,.6,0),"Pfeile: Geschwindigkeit [1 m pro m/s]",0xb9c0cdff);
    (void)ps_scene_group(s,100,0,"Zentraler Stoss");
    for(unsigned i=1;i<=7;i++)(void)ps_scene_set_parent(s,i,100);
    if(e->collided) {
        ps_scene_add_id(s,8,PS_POINT,e->event_point,e->event_point,.035,0xff8eafff);
        (void)ps_scene_label_id(s,9,e->event_point,"Stosszeit siehe collision.time",0xff8eafff);
        (void)ps_scene_set_parent(s,8,100);(void)ps_scene_set_parent(s,9,100);
    }
}
static void destroy(ps_context *c){free(c->user);c->user=NULL;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .capabilities=PS_EXPERIMENT_SCENE_HIERARCHY,.name="Central elastic and inelastic collision",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

### Analyse

```c
#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static ps_result plot(ps_report *r,ps_analysis_context *c,ps_series time,ps_series *series,
    const char *const *labels,unsigned count,const char *title,ps_unit unit) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    strcpy(info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result result=ps_report_unit_from(PS_SECOND,&info.x_unit);
    if(result==PS_OK)result=ps_report_unit_from(unit,&info.y_unit);
    ps_plot_handle handle;if(result==PS_OK)result=ps_report_add_plot(r,&info,&handle);
    for(unsigned i=0;result==PS_OK && i<count;i++)result=ps_report_add_series(r,handle,c,time,series[i],labels[i],PS_PLOT_LINE);
    return result;
}
static ps_result value(ps_analysis_context *c,ps_series s,uint64_t index,double *out) {
    size_t n=0;ps_result r=ps_series_read(c,s,index,out,1,&n);return r==PS_OK && n!=1?PS_INVALID:r;
}
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_dataset dataset={0};ps_dataset_info info={0};
    ps_series series[12]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&dataset);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,dataset,&info);
    const char *names[]={"time","a.position","b.position","a.velocity","b.velocity","energy","momentum.x",
        "energy.dissipated","energy.balance","a.impulse","collision.time","collision.count"};
    for(unsigned i=0;r==PS_OK && i<12;i++)r=ps_dataset_series(c,dataset,names[i],&series[i]);
    if(r==PS_OK)r=ps_report_create("Elastic and inelastic collision","Central vacuum collision: momentum and energy accounting",&report);
    const char *bodies[]={"Sphere A","Sphere B"},*p[]={"total"},*e[]={"kinetic","dissipated","balance"};
    ps_unit momentum={{1,1,-1,0,0,0,0},1,"kg m/s"};
    if(r==PS_OK)r=plot(report,c,series[0],&series[1],bodies,2,"Positions",PS_METRE);
    if(r==PS_OK)r=plot(report,c,series[0],&series[3],bodies,2,"Velocities",PS_VELOCITY);
    if(r==PS_OK)r=plot(report,c,series[0],&series[6],p,1,"Momentum",momentum);
    ps_series energies[]={series[5],series[7],series[8]};
    if(r==PS_OK)r=plot(report,c,series[0],energies,e,3,"Energy accounting",PS_JOULE);
    double initial_p=0,initial_balance=0,final_lost=0,count=0,event_time=0;
    ps_statistics ps={0},balance={0},impulse={0};
    if(r==PS_OK)r=value(c,series[6],0,&initial_p);
    if(r==PS_OK)r=value(c,series[8],0,&initial_balance);
    if(r==PS_OK)r=value(c,series[7],info.samples-1,&final_lost);
    if(r==PS_OK)r=value(c,series[11],info.samples-1,&count);
    if(r==PS_OK)r=value(c,series[10],info.samples-1,&event_time);
    if(r==PS_OK)r=ps_series_statistics(c,series[6],&ps);
    if(r==PS_OK)r=ps_series_statistics(c,series[8],&balance);
    if(r==PS_OK)r=ps_series_statistics(c,series[9],&impulse);
    ps_table_info table={0};strcpy(table.title,"Conservation checks");table.columns=5;
    const char *labels[]={"Samples","Maximum momentum drift","Dissipated energy","Maximum balance drift","Collisions"};
    ps_unit units[]={PS_ONE,momentum,PS_JOULE,PS_JOULE,PS_ONE};
    for(unsigned i=0;r==PS_OK && i<5;i++){strcpy(table.column[i].label,labels[i]);r=ps_report_unit_from(units[i],&table.column[i].unit);}
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"complete run");row.values[0]=(double)info.samples;
    row.values[1]=fmax(fabs(ps.min-initial_p),fabs(ps.max-initial_p));row.values[2]=final_lost;
    row.values[3]=fmax(fabs(balance.min-initial_balance),fabs(balance.max-initial_balance));row.values[4]=count;
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    memset(&table,0,sizeof table);strcpy(table.title,"Collision event");table.columns=2;
    strcpy(table.column[0].label,"Contact time");strcpy(table.column[1].label,"Impulse on A");
    if(r==PS_OK)r=ps_report_unit_from(PS_SECOND,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(momentum,&table.column[1].unit);
    if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    if(r==PS_OK && count==1){strcpy(row.label,"central collision");row.values[0]=event_time;row.values[1]=impulse.mean*(double)impulse.count;r=ps_report_add_row(report,handle,&row);}
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-collision.csv",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,series,12,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,
        .name="Elastic and inelastic collision analysis",.run=analyze};return &api;
}
```

## Vollständiger Physim-Quellcode

### Experiment

```physim
// One central collision in vacuum; same parameters and channels as collision_main.c.
let metres = Unit(1,0,0,0,0,0,0,1,"m")
let speed = Unit(1,0,-1,0,0,0,0,1,"m/s")
let kilograms = Unit(0,1,0,0,0,0,0,1,"kg")
let joules = Unit(2,1,-2,0,0,0,0,1,"J")
let momentum = Unit(1,1,-1,0,0,0,0,1,"kg m/s")
let seconds = Unit(0,0,1,0,0,0,0,1,"s")
let one = Unit(0,0,0,0,0,0,0,1,"1")
let massA = parameterWithUnit("massA",kilograms,1,0.1,10,"Sphere A mass in kilograms")
let massB = parameterWithUnit("massB",kilograms,1,0.1,10,"Sphere B mass in kilograms")
let velocityA = parameterWithUnit("velocityA",speed,0.6,-5,5,"Sphere A initial X velocity")
let velocityB = parameterWithUnit("velocityB",speed,-0.6,-5,5,"Sphere B initial X velocity")
let restitution = parameterWithUnit("restitution",one,1,0,1,"Normal coefficient of restitution")
let radius = 0.2
let zero = Vec3(0,0,0)
let ax = Channel("a.position",metres,"a.position")
let bx = Channel("b.position",metres,"b.position")
let av = Channel("a.velocity",speed,"a.velocity")
let bv = Channel("b.velocity",speed,"b.velocity")
let energy = Channel("energy",joules,"energy")
let totalMomentum = Channel("momentum.x",momentum,"momentum.x")
let dissipated = Channel("energy.dissipated",joules,"energy.dissipated")
let balance = Channel("energy.balance",joules,"energy.balance")
let impulse = Channel("a.impulse",momentum,"a.impulse")
let eventTime = Channel("collision.time",seconds,"collision.time")
let eventCount = Channel("collision.count",one,"collision.count")
var a = Body.sphere(massA,radius)
var b = Body.sphere(massB,radius)
var lost = 0.0
var collisionTime = 0.0
var impulseA = 0.0
var eventPoint = zero
var collided = false
func measure(nextA: Body, nextB: Body, nextLost: Float64, nextImpulse: Float64, nextTime: Float64, nextCollided: Bool):
    let kinetic = nextA.kineticEnergy() + nextB.kineticEnergy()
    let conserved = kinetic + nextLost
    let total = massA * nextA.velocity.x + massB * nextB.velocity.x
    ax.sample(nextA.position.x)
    bx.sample(nextB.position.x)
    av.sample(nextA.velocity.x)
    bv.sample(nextB.velocity.x)
    energy.sample(kinetic)
    totalMomentum.sample(total)
    dissipated.sample(nextLost)
    balance.sample(conserved)
    impulse.sample(nextImpulse)
    eventTime.sample(nextTime)
    var count = 0.0
    if nextCollided:
        count = 1
    eventCount.sample(count)
func create():
    metadata("model=one central collision of two rigid spheres\nmedium=vacuum\ngravity=none\nfriction=0\ninitial_spin=0\nradius_m=0.2\ninitial_positions_m=-1,1\nmass_a_kg=" + String(massA) + "\nmass_b_kg=" + String(massB) + "\nvelocity_a_m_s=" + String(velocityA) + "\nvelocity_b_m_s=" + String(velocityB) + "\nrestitution=" + String(restitution) + "\nccd=linear sphere sweep, impulse, remaining time\ndissipation=0.5*reduced_mass*(1-e^2)*closing_speed^2\nevent_time_valid=collision.count==1\nexcluded=external forces, rotation, deformation, further contacts\n")
func reset():
    var initialA = Body.sphere(massA,radius)
    var initialB = Body.sphere(massB,radius)
    initialA.setState(Vec3(-1,0,0),Vec3(velocityA,0,0),Quat(0,0,0,1),zero)
    initialB.setState(Vec3(1,0,0),Vec3(velocityB,0,0),Quat(0,0,0,1),zero)
    measure(initialA,initialB,0,0,0,false)
    a = initialA
    b = initialB
    lost = 0
    collisionTime = 0
    impulseA = 0
    eventPoint = zero
    collided = false
func step(dt: Float64):
    assert(dt > 0 && simulationTime() + dt > simulationTime(),"Positive representable step required")
    var nextA = a
    var nextB = b
    var nextLost = lost
    var nextTime = collisionTime
    var nextCollided = collided
    var nextPoint = eventPoint
    var nextImpulse = 0.0
    var first = dt
    var event: Sweep? = nil
    if !collided:
        let hit = Sweep.spheres(nextA,radius,nextA.velocity * dt,nextB,radius,nextB.velocity * dt)
        if hit.hit:
            event = Optional.some(hit)
            first = dt * hit.fraction()
    if first > 0:
        nextA.step(zero,zero,first)
        nextB.step(zero,zero,first)
    if let hit = event:
        let closing = nextA.velocity.x - nextB.velocity.x
        let reduced = massA * massB / (massA + massB)
        let contacts = hit.contacts()
        let result = contacts.resolveSingle(nextA,nextB,restitution,0)
        nextA = result.bodyA
        nextB = result.bodyB
        nextImpulse = result.impulse(0).x
        nextLost = 0.5 * reduced * (1 - restitution * restitution) * closing * closing
        nextTime = simulationTime() + first
        nextPoint = contacts.point(0)
        nextCollided = true
        let remaining = dt - first
        if remaining > 0:
            nextA.step(zero,zero,remaining)
            nextB.step(zero,zero,remaining)
    measure(nextA,nextB,nextLost,nextImpulse,nextTime,nextCollided)
    a = nextA
    b = nextB
    lost = nextLost
    collisionTime = nextTime
    impulseA = nextImpulse
    eventPoint = nextPoint
    collided = nextCollided
func scene():
    sphere(a.position,radius,0x53DEC2FF,1)
    arrow(a.position,a.position + a.velocity,0.006,0x53DEC2FF,2)
    label(a.position,"Kugel A",0x53DEC2FF,3)
    sphere(b.position,radius,0xF2C572FF,4)
    arrow(b.position,b.position + b.velocity,0.006,0xF2C572FF,5)
    label(b.position,"Kugel B",0xF2C572FF,6)
    label(Vec3(0,0.6,0),"Pfeile: Geschwindigkeit [1 m pro m/s]",0xB9C0CDFF,7)
    group("Zentraler Stoss",100,0)
    for id in 1..<8:
        sceneParent(id,100)
    if collided:
        point(eventPoint,0.035,0xFF8EAFFF,8)
        label(eventPoint,"Stosszeit siehe collision.time",0xFF8EAFFF,9)
        sceneParent(8,100)
        sceneParent(9,100)
```

### Analyse

```physim
func analyze():
    assert(inputCount() == 1,"Select exactly one collision run")
    report("Elastic and inelastic collision")
    let run = Dataset(0)
    let time = run.series("time")
    let ax = run.series("a.position")
    let bx = run.series("b.position")
    let av = run.series("a.velocity")
    let bv = run.series("b.velocity")
    let energy = run.series("energy")
    let momentum = run.series("momentum.x")
    let lost = run.series("energy.dissipated")
    let balance = run.series("energy.balance")
    let impulse = run.series("a.impulse")
    let eventTime = run.series("collision.time")
    let eventCount = run.series("collision.count")
    let positions = ax.plot(time,"Positions","Sphere A")
    positions.curve(time,bx,"Sphere B")
    let velocities = av.plot(time,"Velocities","Sphere A")
    velocities.curve(time,bv,"Sphere B")
    momentum.plot(time,"Momentum","total")
    let energies = energy.plot(time,"Energy accounting","kinetic")
    energies.curve(time,lost,"dissipated")
    energies.curve(time,balance,"balance")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let joules = Unit(2,1,-2,0,0,0,0,1,"J")
    let momentumUnit = Unit(1,1,-1,0,0,0,0,1,"kg m/s")
    let seconds = Unit(0,0,1,0,0,0,0,1,"s")
    let count = eventCount.value(eventCount.count() - 1)
    let p0 = momentum.value(0)
    let b0 = balance.value(0)
    let pDrift = max(abs(momentum.minimum() - p0),abs(momentum.maximum() - p0))
    let bDrift = max(abs(balance.minimum() - b0),abs(balance.maximum() - b0))
    let summary = Table("Conservation checks",["Samples","Maximum momentum drift","Dissipated energy","Maximum balance drift","Collisions"],[one,momentumUnit,joules,joules,one])
    summary.row("complete run",[Quantity(Float64(time.count()),one),Quantity(pDrift,momentumUnit),
        Quantity(lost.value(lost.count() - 1),joules),Quantity(bDrift,joules),Quantity(count,one)])
    let event = Table("Collision event",["Contact time","Impulse on A"],[seconds,momentumUnit])
    if count == 1:
        event.row("central collision",[Quantity(eventTime.value(eventTime.count() - 1),seconds),
            Quantity(impulse.mean() * Float64(impulse.count()),momentumUnit)])
    Series.exportColumns([time,ax,bx,av,bv,energy,momentum,lost,balance,impulse,eventTime,eventCount],"collision")
    run.close()
```

## Automatisierte Nachweise

`collision_tutorial` vergleicht elf Szenarien in beiden Sprachen mit unabhängigen
Stoß-, Energie-, Impuls- und Trajektorienformeln. Darunter sind Restitution 0, 0,5,
1 und 0,35, ungleiche Massen, ausbleibende Kontakte, grobe Schritte und ein Kontakt
auf einer Schrittgrenze. Der Test prüft komplette Szenen, alle SI-Metadaten,
44 gemischte Experiment-/Analysepfade und jedes CSV-Feld. Ein separater C-Prüfer
vergleicht die acht Berichtskurven und beide Tabellen mit den Originaldaten.
Die Instanzprobe prüft gleichzeitig verschiedene Massen und Restitutionen,
Reset und abgewiesene Schritte. `collision_tutorial_workflow` prüft beide
App-Projektsprachen mit einem ungleichen vollkommen inelastischen Stoß,
Einzelschritten, Archiv und Analyse. `documentation_collision_source` prüft die
vier dokumentierten Quellcodeblöcke gegen die tatsächlich gebauten Dateien.
