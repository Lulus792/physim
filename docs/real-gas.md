# Reale Gase: begrenztes Van-der-Waals-Modell

Die C-Funktionen in `physim/thermodynamics.h` und die vier Physim-Funktionen
`vdwGasPressure`, `vdwGasPressureDerivative`, `vdwGasEnergy` und
`vdwGasEntropyChange` werten dasselbe homogene Modell aus. Es benötigt konstante,
explizit angegebene molare Koeffizienten; kein Stoffkatalog ist eingebaut.

## Gleichungen und SI-Vertrag

Für n in mol, T in K, V in m³, a in Pa·m⁶/mol², b in m³/mol und konstantes
cv in J/(mol K) gelten:

- p = nRT/(V−nb) − an²/V².
- (∂p/∂V) bei festem n,T = −nRT/(V−nb)² + 2an²/V³, in Pa/m³.
- U = n cv T − an²/V, in J. Die gewählte Referenz nähert sich null für T→0,V→∞.
- ΔS = n[cv ln(T1/T0) + R ln((V1−nb)/(V0−nb))], in J/K.

Die Gleichungen folgen aus den [Oxford-Vorlesungsunterlagen zur Thermodynamik](https://www.physics.ox.ac.uk/system/files/file_attachments/all_thermo_notes.pdf),
Abschnitten 14–15. Dort werden Teilchenzahl N und kB verwendet. Hier sind sie
in Stoffmenge n und R umgerechnet; a,b sind entsprechend molare Koeffizienten.
Teilchenbezogene Zahlenwerte dürfen nicht unverändert eingesetzt werden.

Alle Argumente sind endlich. n,T,V,cv sind strikt positiv, a,b nichtnegativ.
Für Druck, Ableitung und Entropie muss jeder Zustand V>nb erfüllen.
Die Energieformel braucht kein b; der Aufrufer prüft den Zustandsbereich seines
vollständigen Modells. Freies Volumen wird mit `fma(-n,b,V)` berechnet.
Normierte Binärprodukte vermeiden unnötigen Zwischenüberlauf; signierte
Differenzen werden vor der Rückskalierung gebildet. Das ist Double-Arithmetik,
keine beliebig genaue Rechnung. Fehler erhalten C-Ausgaben; ungültige Eingaben
liefern `PS_INVALID`, nichtendliche Ergebnisse `PS_NUMERIC`. Signierter Unterlauf
kann auf null runden. Physim meldet Fehler an der Quellstelle; `attempt` fängt sie.

## Gültigkeitsgrenzen

Negative Drücke, Energien und positive Druckableitungen sind zulässige algebraische
Ausgaben. Positive Ableitung zeigt mechanische Instabilität des homogenen Zustands;
negative Ableitung allein beweist kein vollständiges Phasengleichgewicht.
Es gibt keine stabile Astwahl, Maxwell-Konstruktion, Koexistenz, latente Wärme,
Temperaturabhängigkeit der Koeffizienten oder experimentelle Kalibrierung.
ΔS ist eine Zustandsdifferenz, keine Entropieproduktion und kein Wärmepfadintegral.
Der ideale Grenzfall a=b=0 wird gegen die bestehenden idealen Funktionen geprüft.

## Vergleichsexperiment und Analyse

Die folgenden vollständigen Quellen benutzen synthetische Koeffizienten:
n=1 mol, T=450 K, a=0,4 Pa·m⁶/mol², b=0,00004 m³/mol,
cv=20,8 J/(mol K). Sie gehören zu keinem benannten realen Gas.
Das vorgeschriebene Volumen sinkt in einer Sekunde von 0,005 auf 0,001 m³ und
bleibt dann fest. Dies berechnet Zustandsgrößen, keine Kolbendynamik.
Sieben Kanäle speichern Volumen, beide Drücke, beide Energien, Entropiedifferenz
und Druckableitung mit SI-Dimensionen. Die Metadaten halten Koeffizienten,
Referenz, Herkunft und Ausschlüsse fest. Zwei Druckdiagramme vergleichen beide Modelle.

Für C `main.c` und `analysis.c`, für Physim `main.phys` und `analysis.phys`
in einem neuen Projekt verwenden. Das native Bauen entspricht dem
[Build-Leitfaden](build.md). Die Quellen liegen auch im SDK unter
`examples/documentation/real_gas_*`.

## main (c)

```c
#include "physim/units.h"
#include "physim/experiment.h"
#include "physim/thermodynamics.h"
#include <math.h>
#include <stdio.h>
/* Synthetic molar SI coefficients; prescribed isothermal compression. */
static ps_result measure(ps_context *c,double time) {
    double v=.005-.004*fmin(1,time),values[7]={v};ps_result r;
    if((r=ps_ideal_gas_pressure(1,450,v,&values[1]))!=PS_OK ||
       (r=ps_vdw_gas_pressure(1,450,v,.4,4e-5,&values[2]))!=PS_OK ||
       (r=ps_ideal_gas_energy(1,20.8,450,&values[3]))!=PS_OK ||
       (r=ps_vdw_gas_energy(1,20.8,450,v,.4,&values[4]))!=PS_OK ||
       (r=ps_vdw_gas_entropy_change(1,20.8,450,.005,450,v,4e-5,&values[5]))!=PS_OK ||
       (r=ps_vdw_gas_pressure_derivative(1,450,v,.4,4e-5,&values[6]))!=PS_OK)return r;
    for(unsigned i=0;i<7;i++)c->values[i]=values[i];return PS_OK;
}
static ps_result reset(ps_context *c){return measure(c,0);}
static ps_result create(ps_context *c) {
    const char *names[]={"volume","ideal_pressure","vdw_pressure","ideal_energy","vdw_energy","entropy_change","pressure_derivative"};
    ps_unit volume={{3,0,0,0,0,0,0},1,"m3"},entropy={{2,1,-2,0,-1,0,0},1,"J/K"},derivative={{-4,1,-2,0,0,0,0},1,"Pa/m3"};
    ps_unit units[]={volume,PS_PASCAL,PS_PASCAL,PS_JOULE,PS_JOULE,entropy,derivative};
    for(unsigned i=0;i<7;i++)if(ps_channel_add(c,names[i],units[i],names[i])!=(int)i)return PS_LIMIT;
    snprintf(c->model_metadata,sizeof c->model_metadata,"model=homogeneous van der Waals vs ideal gas\nsource=synthetic coefficients; not a calibrated gas\namount=1 mol\ntemperature=450 K\nattraction=0.4 Pa m6/mol2\ncovolume=0.00004 m3/mol\nmolar_cv=20.8 J/(mol K)\nvolume=0.005 to 0.001 m3 in 1 s; then fixed\nenergy_reference=U=n cv T-a n2/V\nexcluded=phase coexistence,Maxwell construction,latent heat,temperature-dependent coefficients\n");return reset(c);
}
static ps_result step(ps_context *c,double dt){return measure(c,c->time_s+dt);}
static void scene(ps_context *c,ps_scene *out){ps_scene_add_id(out,1,PS_SPHERE,ps_v3(c->time_s,0,0),ps_v3(0,0,0),.1,0x53aeefff);}
static void destroy(ps_context *c){(void)c;}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Real gas comparison",
        .create=create,.reset=reset,.step=step,.build_scene=scene,.destroy=destroy};return &api;
}
```

## main (physim)

```physim
// Synthetic molar SI coefficients; prescribed isothermal compression.
let volume = Channel("volume",Unit(3,0,0,0,0,0,0,1,"m3"),"volume")
let idealPressure = Channel("ideal_pressure",Unit(-1,1,-2,0,0,0,0,1,"Pa"),"ideal_pressure")
let pressure = Channel("vdw_pressure",Unit(-1,1,-2,0,0,0,0,1,"Pa"),"vdw_pressure")
let idealEnergy = Channel("ideal_energy",Unit(2,1,-2,0,0,0,0,1,"J"),"ideal_energy")
let energy = Channel("vdw_energy",Unit(2,1,-2,0,0,0,0,1,"J"),"vdw_energy")
let entropy = Channel("entropy_change",Unit(2,1,-2,0,-1,0,0,1,"J/K"),"entropy_change")
let derivative = Channel("pressure_derivative",Unit(-4,1,-2,0,0,0,0,1,"Pa/m3"),"pressure_derivative")
func measure(time: Float64):
    let v = 0.005 - 0.004*min(1,time)
    volume.sample(v)
    idealPressure.sample(idealGasPressure(1,450,v))
    pressure.sample(vdwGasPressure(1,450,v,0.4,4e-5))
    idealEnergy.sample(idealGasEnergy(1,20.8,450))
    energy.sample(vdwGasEnergy(1,20.8,450,v,0.4))
    entropy.sample(vdwGasEntropyChange(1,20.8,450,0.005,450,v,4e-5))
    derivative.sample(vdwGasPressureDerivative(1,450,v,0.4,4e-5))
func create():
    metadata("model=homogeneous van der Waals vs ideal gas\nsource=synthetic coefficients; not a calibrated gas\namount=1 mol\ntemperature=450 K\nattraction=0.4 Pa m6/mol2\ncovolume=0.00004 m3/mol\nmolar_cv=20.8 J/(mol K)\nvolume=0.005 to 0.001 m3 in 1 s; then fixed\nenergy_reference=U=n cv T-a n2/V\nexcluded=phase coexistence,Maxwell construction,latent heat,temperature-dependent coefficients\n")
    reset()
func reset():
    measure(0)
func step(dt: Float64):
    measure(simulationTime()+dt)
func scene():
    sphere(Vec3(simulationTime(),0,0),0.1,0x53aeefff,1)
```

## analysis (c)

```c
#include "physim/units.h"
#include "physim/report.h"
#include <stdio.h>
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *ctx=NULL;ps_dataset dataset;ps_series volume,pressure;
    ps_result r=ps_analysis_create(prefix,0,&ctx);
    if(r==PS_OK)r=ps_analysis_open_run(ctx,input,&dataset);
    if(r==PS_OK || r==PS_RECOVERED)r=ps_dataset_series(ctx,dataset,"volume",&volume);
    ps_report *report=NULL;
    if(r==PS_OK)r=ps_report_create("Real gas comparison","Synthetic coefficients; homogeneous isothermal algebra; no phase coexistence",&report);
    const char *names[]={"ideal_pressure","vdw_pressure"};
    for(unsigned i=0;i<2 && r==PS_OK;i++) {
        ps_plot_handle plot;ps_plot_info info={0};
        snprintf(info.title,sizeof info.title,"%s",names[i]);
        ps_unit unit={{3,0,0,0,0,0,0},1,"m3"};ps_report_unit_from(unit,&info.x_unit);ps_report_unit_from(PS_PASCAL,&info.y_unit);
        r=ps_dataset_series(ctx,dataset,names[i],&pressure);
        if(r==PS_OK)r=ps_report_add_plot(report,&info,&plot);
        if(r==PS_OK)r=ps_report_add_series(report,plot,ctx,volume,pressure,names[i],PS_PLOT_LINE);
    }
    char path[4096];snprintf(path,sizeof path,"%s.psreport",prefix);
    if(r==PS_OK)r=ps_report_save(report,path);
    ps_report_destroy(report);ps_analysis_destroy(ctx);return r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Real gas analysis",.run=analyze};return &api;
}
```

## analysis (physim)

```physim
func analyze():
    report("Real gas comparison")
    let run = Dataset(0)
    let volume = run.series("volume")
    run.series("ideal_pressure").plot(volume,"Ideal pressure","ideal_pressure")
    run.series("vdw_pressure").plot(volume,"Van der Waals pressure","vdw_pressure")
```

[API-Referenz](reference/thermodynamics.md) · [Wärmefluss und ideales Gas](thermodynamics.md)
