# Einen gespeicherten Lauf auswerten: C und Physim

Lernziel: Trenne Simulation und Auswertung. Erzeuge einmal einen Messlauf,
übernimm sein Archiv in ein eigenständiges Analyseprojekt und berechne aus der
aufgezeichneten Position Geschwindigkeit und eine rekonstruierte Position.
Öffne den gespeicherten Bericht anschließend ohne erneuten Build oder Lauf.
Beide Analysesprachen lesen C- und Physim-Archive gleich.

## Modell und Gleichungen

Das Beispielarchiv stammt von einer gleichförmigen Bewegung entlang x ohne
Kräfte, Kollisionen oder Sensorfehler: x(0)=0 m und v=1,5 m/s. Für die
aufgezeichneten Zeitpunkte gilt x(t)=1,5t. 32 Schritte zu 0,0625 s ergeben mit
Anfangsmessung 33 Zeilen bis t=2 s und x=3 m. Der Seed 42 ist protokolliert,
obwohl das Modell keine Zufallszahlen verwendet. Rundungsfehler bleiben möglich.

Die Analyse benötigt genau einen Lauf mit mindestens zwei Messpunkten,
streng steigender Zeitachse und einem Längenkanal `position.x`. Sie ist für
andere Bewegungen wiederverwendbar und setzt deren Geschwindigkeit nicht auf
1,5 m/s. Eine gespeicherte Laufserie oder eine Projektquelle ist kein Eingang;
verwende eine einzelne `.psrun`-Datei.

Für innere Punkte verwendet die Analyse eine zentrale Sekante:

`v_i=(x_(i+1)−x_(i−1))/(t_(i+1)−t_(i−1))`.

An den Enden gilt die einseitige Sekante des ersten beziehungsweise letzten
Segments. Die Rückintegration startet mit der tatsächlich gespeicherten
Anfangsposition und benutzt Trapeze:

`r_0=x_0; r_i=r_(i−1)+½(v_(i−1)+v_i)(t_i−t_(i−1))`.

Das Residuum ist `e_i=r_i−x_i`. Die Tabelle meldet außerdem Δx=x_N−x_0 und
Sekantengeschwindigkeit Δx/(t_N−t_0); das ist keine ungewichtete Mittelung der
Geschwindigkeitssamples. Zeitachsen werden nicht verschoben oder interpoliert.
Auch unregelmäßige Raster, eine spätere Startzeit und negative Bewegung sind
zulässig. Bei unregelmäßigem Raster ist die zentrale Sekante keine allgemeine
exakte Ableitung am mittleren Zeitpunkt.

C verwendet `experiment` für das erzeugende Modell sowie `series`, `analysis`
und `report` für Dataset-Lesen, Ableitung, Integration, Statistik und Export.
Physim verwendet die entsprechenden `Channel`-, `Dataset`-, `Series`-, `Plot`-
und `Table`-Bindungen. Parameter und Kräfte sind für dieses Modell nicht nötig.
Die Analyse prüft Längen- und Zeitdimensionen durch einen kompatiblen Nulloffset.
Archivierte Reihen verwenden SI-Werte; ein anderes Anzeigeeinheitensymbol
ändert diese Werte nicht.

## In der App durchführen

1. Lege ein Experimentprojekt in C oder Physim an. Ersetze seine Experimentquelle
   durch die entsprechende vollständige gleichförmige Bewegung unten. Eine
   vorhandene Analysevorlage kann im erzeugenden Projekt bleiben.
2. Wähle Release, h=0,0625 s und Seed 42, baue und zeichne 2 s auf. Stoppe den
   Lauf und bewahre seine `.psrun`-Datei im ursprünglichen `runs/`-Ordner auf.
3. Wähle **Datei → Neues Projekt … → Nur Auswertung gespeicherter Läufe**.
   Erstelle ein neues Analyseprojekt in C oder Physim. Es enthält nur
   `analysis.c` beziehungsweise `analysis.phys` und keine Experimentquelle.
4. Ersetze die Analysequelle durch die passende vollständige Quelle unten.
   Baue mit F5. **Simulieren** ist für ein reines Analyseprojekt nicht verfügbar.
5. Öffne **Auswerten → Messlauf importieren …** und wähle das gespeicherte Archiv.
   Der Import prüft das Archiv und kopiert es samt vorhandenen Begleitdateien in
   den neuen `runs/`-Ordner. Das Original bleibt erhalten. Die aktuelle Ansicht
   und die Vergleichsauswahl werden auf die importierte Datei gesetzt.
6. Wähle **Analyse starten**. Vergleiche Position mit Rückintegration, abgeleitete
   Geschwindigkeit, Residuum und die beiden Tabellen. Der vollständige CSV-Export
   `<präfix>-motion.csv` enthält Zeit, Position, Geschwindigkeit, Rekonstruktion
   und Residuum in dieser Reihenfolge.
7. Schließe das Projekt und öffne es erneut. Wähle unter **Läufe & Berichte →
   Analyseberichte** den Bericht und **Öffnen**. Die gespeicherten Kurven und
   Tabellen sind ohne Build und ohne ursprüngliches Experimentmodul verfügbar.

Leere vor diesem Ablauf eine alte Mehrlaufauswahl. Diese Analyse verarbeitet
genau einen Lauf; ein Mehrlaufauftrag wird zurückgewiesen. Die Bibliotheksansicht
zeigt nur Dateien direkt im jeweiligen `runs/`-Ordner. Breite Tabellen lassen
sich horizontal scrollen. [Import, Auswahl und Herkunft](runs.md),
[Analyseprojekte bauen](build.md).

## Erwartete Ergebnisse und Grenzen

Für den Beispiellauf liegen die aufgezeichnete und rekonstruierte Position
übereinander: x_0=0 m, x_N=3 m, Δx=3 m, Sekantengeschwindigkeit=1,5 m/s und
eine Ableitung von 1,5 m/s an allen Punkten. Das Residuum sollte nur im Bereich
von Gleitkommarundung liegen. Die erste Tabelle nennt 33 Samples, Start 0 s,
Ende 2 s und **Recovered prefix=0**. Drei Plots und zwei Tabellen werden gespeichert.

Ein gültiger lesbarer Teil eines unvollständigen Archivs darf ausgewertet werden.
Dann zeigt **Recovered prefix=1** die Wiederherstellung an; Anzahl und Endzeit
beschreiben ausschließlich diesen Teil. Fehlende spätere Messpunkte werden
nicht erfunden. Zu kurze, fehlende Kanäle, falsche Dimensionen oder ungeeignete
Zeitachsen führen zu einem Fehler ohne fertigen Bericht. Bei Messlücken ist
keine vollständige Rekonstruktion möglich; dieses Beispiel füllt sie nicht auf.

Differenzieren verstärkt Messrauschen. Endpunktsekanten, grobe oder unregelmäßige
Raster erzeugen bei beschleunigter Bewegung Rekonstruktionsfehler. Ein kleines
Residuum beweist keine physikalisch richtige Bewegung und keine korrekte
Sensor-Kalibrierung: Es kontrolliert nur die Verarbeitung derselben Positionsdaten.
Diagramme enthalten höchstens 2048 Vorschaupunkte pro Kurve; CSV enthält alle
berechneten Zeilen. Einheiten und Achsen bleiben im Bericht erhalten.

## Aus einem SDK bauen

Die folgenden Befehle gelten in einem SDK-Verzeichnis unter macOS/Linux.
Benutze neue Ausgabepfade. Unter Windows sind Shellbefehle und Endungen anzupassen.

```sh
mkdir producer
cp examples/documentation/main.c producer/main.c
cp examples/documentation/analysis.c producer/analysis.c
printf '%s\n' physim_project=1 > producer/physim.project
bin/physim-build --project producer --sdk . --output producer/build/Release --physimc bin/physimc --profile Release
bin/physim-runner producer/build/Release/experiment.so uniform.psrun --steps 32 --dt 0.0625 --seed 42
mkdir saved-analysis
cp examples/documentation/saved_run_analysis.c saved-analysis/analysis.c
printf '%s\n' physim_project=2 kind=analysis analysis=analysis.c > saved-analysis/physim.project
bin/physim-build --project saved-analysis --sdk . --output saved-analysis/build/Release --physimc bin/physimc --profile Release
bin/physim-analysis-runner saved-analysis/build/Release/analysis.so uniform.psrun saved-result
bin/physim-analysis-runner bin/language-saved_run_analysis.so uniform.psrun saved-result-phys
```

Das reine Analyseprojekt baut kein Experimentmodul. Für einen Physim-Erzeuger
kannst du `examples/documentation/language_main.phys` als `main.phys` verwenden
und im Producer-Manifest `experiment=main.phys` angeben. Die Analysten brauchen
anschließend nur das Archiv, ihr eigenes Modul und einen neuen Ausgabepräfix.

## Vollständige Quellen

### Experiment in C

```c
#include "physim/experiment.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double position, velocity;
    int channel;
} motion;

static ps_result reset(ps_context *c) {
    motion *m = c->user;
    m->position = 0;
    m->velocity = 1.5;
    c->values[m->channel] = m->position;
    return PS_OK;
}
static ps_result create(ps_context *c) {
    motion *m = calloc(1, sizeof *m);
    if (!m)
        return PS_MEMORY;
    c->user = m;
    m->channel = ps_channel_add(c, "position.x", PS_METRE, "Position along X");
    if (m->channel < 0) {
        snprintf(c->error, sizeof c->error, "Could not register position.x");
        return PS_LIMIT;
    }
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=uniform motion\nvelocity_m_s=1.5\nintegrator=exact");
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    motion *m = c->user;
    m->position += m->velocity * dt;
    c->values[m->channel] = m->position;
    return PS_OK;
}
static void scene(ps_context *c, ps_scene *s) {
    motion *m = c->user;
    (void)ps_scene_add_id(s, 1, PS_SPHERE, ps_v3(m->position, 0, 0), ps_v3(0, 0, 0), 0.15,
                          0x64aaffff);
}
static void destroy(ps_context *c) {
    free(c->user);
    c->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof(ps_experiment_api),
                                          PS_ABI_VERSION,
                                          0,
                                          "Uniform motion",
                                          create,
                                          reset,
                                          step,
                                          scene,
                                          destroy, NULL};
    return &api;
}
```

### Experiment in Physim

```physim
// Uniform motion: 1.5 m/s along X, without forces or collisions.
let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
let position = Channel("position.x", metres, "Position along X")
var x: Float64 = 0

func create():
    metadata("model=uniform motion\nvelocity_m_s=1.5\nintegrator=exact")

func reset():
    x = 0
    position.sample(x)

func step(dt: Float64):
    x += 1.5 * dt
    position.sample(x)

func scene():
    sphere(Vec3(x, 0, 0), 0.15, 1688924159, 1)
```

### Analyse in C

```c
#include "physim/report.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static ps_result at(ps_analysis_context *c,ps_series s,uint64_t i,double *out) {
    size_t n=0;ps_result r=ps_series_read(c,s,i,out,1,&n);return r==PS_OK && n!=1?PS_INVALID:r;
}
static ps_result plot(ps_report *report,ps_analysis_context *c,ps_series time,
    ps_series *series,const char *const *labels,unsigned count,const char *title,ps_unit unit) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    strcpy(info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result r=ps_report_unit_from(PS_SECOND,&info.x_unit);
    if(r==PS_OK)r=ps_report_unit_from(unit,&info.y_unit);
    ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&info,&handle);
    for(unsigned i=0;r==PS_OK && i<count;i++)r=ps_report_add_series(report,handle,c,time,series[i],labels[i],PS_PLOT_LINE);
    return r;
}
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *c=NULL;ps_report *report=NULL;ps_dataset run={0};ps_dataset_info info={0};
    ps_series time={0},position={0},velocity={0},reconstructed={0},residual={0};
    bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&c);
    if(r==PS_OK){r=ps_analysis_open_run(c,input,&run);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_describe(c,run,&info);
    if(r==PS_OK && info.samples<2)r=PS_INVALID;
    if(r==PS_OK)r=ps_dataset_series(c,run,"time",&time);
    if(r==PS_OK)r=ps_dataset_series(c,run,"position.x",&position);
    /* A zero offset checks the physical dimensions; run series use SI values. */
    if(r==PS_OK)r=ps_series_affine(c,time,1,(ps_quantity){0,PS_SECOND},&time);
    if(r==PS_OK)r=ps_series_affine(c,position,1,(ps_quantity){0,PS_METRE},&position);
    double start=0,end=0,first=0,last=0;
    if(r==PS_OK)r=at(c,time,0,&start);
    if(r==PS_OK)r=at(c,time,info.samples-1,&end);
    if(r==PS_OK)r=at(c,position,0,&first);
    if(r==PS_OK)r=at(c,position,info.samples-1,&last);
    if(r==PS_OK)r=ps_series_derivative(c,position,time,&velocity);
    if(r==PS_OK)r=ps_series_integral(c,velocity,time,(ps_quantity){first,PS_METRE},&reconstructed);
    if(r==PS_OK)r=ps_series_combine(c,PS_SERIES_SUBTRACT,reconstructed,position,&residual);
    ps_statistics error={0};if(r==PS_OK)r=ps_series_statistics(c,residual,&error);
    if(r==PS_OK)r=ps_report_create("Saved run analysis",recovered?"Recovered readable prefix; recording incomplete":"Stored position, central secants and trapezoidal reconstruction",&report);
    ps_series positions[]={reconstructed,position};const char *labels[]={"reconstructed","recorded"};
    if(r==PS_OK)r=plot(report,c,time,positions,labels,2,"Position and reconstruction",PS_METRE);
    const char *v[]={"dx/dt"},*e[]={"reconstructed minus recorded"};
    if(r==PS_OK)r=plot(report,c,time,&velocity,v,1,"Derived velocity",PS_VELOCITY);
    if(r==PS_OK)r=plot(report,c,time,&residual,e,1,"Reconstruction residual",PS_METRE);
    ps_table_info table={0};strcpy(table.title,"Recording");table.columns=4;
    const char *record_labels[]={"Samples","Start","End","Recovered prefix"};ps_unit record_units[]={PS_ONE,PS_SECOND,PS_SECOND,PS_ONE};
    for(unsigned i=0;r==PS_OK && i<4;i++){strcpy(table.column[i].label,record_labels[i]);r=ps_report_unit_from(record_units[i],&table.column[i].unit);}
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};strcpy(row.label,"run");row.values[0]=(double)info.samples;row.values[1]=start;row.values[2]=end;row.values[3]=recovered;
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    memset(&table,0,sizeof table);strcpy(table.title,"Motion");table.columns=5;
    const char *motion_labels[]={"Initial x","Final x","Displacement","Secant velocity","Max residual"};ps_unit motion_units[]={PS_METRE,PS_METRE,PS_METRE,PS_VELOCITY,PS_METRE};
    for(unsigned i=0;r==PS_OK && i<5;i++){strcpy(table.column[i].label,motion_labels[i]);r=ps_report_unit_from(motion_units[i],&table.column[i].unit);}
    if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    row.values[0]=first;row.values[1]=last;row.values[2]=last-first;row.values[3]=(last-first)/(end-start);row.values[4]=fmax(fabs(error.min),fabs(error.max));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];int n=snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);
    n=snprintf(path,sizeof path,"%s-motion.csv",prefix);ps_series columns[]={time,position,velocity,reconstructed,residual};
    if(r==PS_OK)r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(c,columns,5,path);
    ps_report_destroy(report);ps_analysis_destroy(c);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Saved position analysis",.run=analyze};return &api;
}
```

### Analyse in Physim

```physim
func analyze():
    assert(inputCount() == 1,"Select one stored position run")
    let run = Dataset(0)
    let count = run.sampleCount()
    assert(count >= 2,"At least two position samples required")
    let metres = Unit(1,0,0,0,0,0,0,1,"m")
    let seconds = Unit(0,0,1,0,0,0,0,1,"s")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let speed = Unit(1,0,-1,0,0,0,0,1,"m/s")
    let time = run.series("time").affine(1,0,seconds)
    let position = run.series("position.x").affine(1,0,metres)
    let start = time.value(0)
    let end = time.value(count - 1)
    let first = position.value(0)
    let last = position.value(count - 1)
    let velocity = position.derivative(time)
    let reconstructed = velocity.integral(time,first,metres)
    let residual = reconstructed.subtracting(position)
    report("Saved run analysis")
    // The integral supplies canonical metres for the shared plot axis.
    let positions = reconstructed.plot(time,"Position and reconstruction","reconstructed")
    positions.curve(time,position,"recorded")
    velocity.plot(time,"Derived velocity","dx/dt")
    residual.plot(time,"Reconstruction residual","reconstructed minus recorded")
    let recording = Table("Recording",["Samples","Start","End","Recovered prefix"],[one,seconds,seconds,one])
    var recovered = 0.0
    if run.recovered():
        recovered = 1
    recording.row("run",[Quantity(Float64(count),one),Quantity(start,seconds),Quantity(end,seconds),Quantity(recovered,one)])
    let motion = Table("Motion",["Initial x","Final x","Displacement","Secant velocity","Max residual"],[metres,metres,metres,speed,metres])
    let maximum = max(abs(residual.minimum()),abs(residual.maximum()))
    motion.row("run",[Quantity(first,metres),Quantity(last,metres),Quantity(last - first,metres),Quantity((last - first)/(end - start),speed),Quantity(maximum,metres)])
    Series.exportColumns([time,position,velocity,reconstructed,residual],"motion")
    run.close()
```
