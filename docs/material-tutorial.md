# Eigenes Material und eigenes Medium: C und Physim

## Lernziel

Definiere Material- und Fluiddaten selbst, leite daraus die Kugelmasse ab und
kombiniere die gemeinsamen Kraftfunktionen. Vergleiche einen idealen Vakuumfall
mit einem viskosen Medium. Experiment und Auswertung dürfen unabhängig C oder
Physim verwenden. Der Quellcode und alle veränderten Parameter werden im Lauf
archiviert; ein Seed von 42 wird hier nicht für Zufallszahlen verwendet.

## Modell und Gültigkeit

Eine homogene Kugel mit Radius `r` liegt vollständig in einem unbegrenzten,
ruhenden, Newtonschen Fluid. Positive Y-Werte zeigen nach oben. Die Materialdichte
ist `rho_s`, die Fluiddichte `rho_f`, die dynamische Viskosität `mu`. Es gibt
keine Oberfläche oder Kontakte. Das Beispiel verwendet ausschließlich die
Materialdichte; Restitution und Reibung des Materialwerts bleiben null und
wirken ohne Kontakte nicht. Die Namen und Zahlen sind eigene Demonstrationsdaten,
keine Stoffdatenbank oder gemessene Kennwerte.

```text
V = 4*pi*r³/3                 m = rho_s*V
F_weight = -m*g               F_buoyancy = rho_f*V*g
F_drag = -6*pi*mu*r*v          lambda = 6*pi*mu*r/m
y' = v                       v' = (F_weight + F_buoyancy + F_drag)/m
Re = 2*r*rho_f*abs(v)/mu      g = 9.80665 m/s²
```

Stokes-Widerstand setzt eine kleine Reynolds-Zahl voraus. Das Beispiel beendet
Läufe oberhalb `Re=0,1`; das ist eine konservative Lehrgrenze, keine universelle
Fehlergarantie. Nichtzero Fluiddichte verlangt positive Viskosität. Vakuum wird
explizit durch `rho_f=mu=0` dargestellt. Für RK4 gilt zusätzlich
`lambda*dt <= 0,25`; Zeitschrittverfeinerung bleibt erforderlich.
Die Kraftbilanz und das Stokes-Sinken erläutern die
[MIT-Unterlagen, Abschnitt 4.5](https://ocw.mit.edu/courses/res-12-001-topics-in-fluid-dynamics-fall-2024/mitres_12_001_f24_essay2.pdf);
die [NASA-Reynolds-Erklärung](https://www.grc.nasa.gov/WWW/K-12/airplane/reynolds.html)
definiert die Kennzahl über Dichte, Geschwindigkeit, Länge und Viskosität.

## Analytische Referenz und Energie

Mit `a = -g*(1-rho_f/rho_s)`, `y0=0,5 m`, `v0=0` ist die Referenz für `lambda>0`:

```text
v(t) = a/lambda * (1-exp(-lambda*t))
y(t) = y0 + a/lambda * (t-(1-exp(-lambda*t))/lambda)
v_terminal = a/lambda
```

Für Vakuum ist `v=a*t`, `y=y0+a*t²/2`. Der integrierte Zustand besteht aus Ort,
Geschwindigkeit und dissipierter Arbeit. Die Referenzkanäle steuern die Integration
nicht. Für das vereinfachte Modell gilt:

```text
E_kinetic = m*v²/2
E_potential = -(F_weight+F_buoyancy)*y
E_dissipated' = -F_drag*v
E_balance = E_kinetic+E_potential+E_dissipated ≈ E_balance(0)
```

Die potentielle Energie ist die effektive Gewicht-/Auftriebsenergie dieses
konstanten Fluidmodells. Sie ist nicht nur die gravitative Körperenergie und
keine vollständige Flüssigkeitsenergiebilanz. Bei einer steigenden Kugel können
Potential und Gesamtbilanz negativ sein; Dissipation und kinetische Energie
bleiben nichtnegativ. Hinzugefügte Masse, hydrodynamische Gedächtniskräfte,
Wände und nicht-Newtonsche Stoffgesetze sind ausgeschlossen. Die
Beschleunigungsphase ist deshalb nur eine Lösung dieses reduzierten Modells.

## Projekt anlegen und vergleichen

1. Lege ein beliebiges C- oder Physim-Experimentprojekt an. Wähle die gewünschte
   Analysesprache unabhängig davon. Ersetze Experiment und Analyse durch die
   zugehörigen vollständigen Quellen weiter unten.
2. Baue mit **F5**. Stelle unter **Simulieren → Inspector → Laufeinstellungen**
   `dt=0,001 s` und Seed 42 ein. Die vier Experimentparameter bleiben zunächst
   auf `materialDensity=2500`, `mediumDensity=1000`, `viscosity=100`, `radius=0,05`.
3. Starte mit **F6**, stoppe ungefähr bei einer Sekunde und starte die Analyse.
   Die Szene zeigt Kugel und drei Kraftpfeile: Gewicht gelb, Auftrieb blau,
   Widerstand rosa. Pfeillängen verwenden `0,03 m/N`; ihre seitlichen Versätze
   dienen nur der Lesbarkeit und erzeugen keine Drehmomente.
4. Der Bericht zeigt Geschwindigkeit und analytische Referenz, deren Differenz
   sowie kinetische, potentielle und dissipierte Energie und ihre Summe.
   `<prefix>-material_check.csv` enthält alle Zeiten und Kontrollwerte.
5. Setze für den Vakuumvergleich **beide** Fluidparameter auf null und starte
   einen neuen Lauf. Die alten Daten bleiben erhalten. Bei einer Sekunde sind
   `y=-4,403325 m`, `v=-9,80665 m/s`; die Kugel kann den aktuellen Kameraausschnitt
   verlassen. Stelle die Kamera um oder vergleiche die gespeicherten Zahlen.
6. Für eine steigende Kugel setze `materialDensity=500` und `dt=0,0005 s`.
   Für Dichtegleichheit setze `materialDensity=1000`; Ort und Geschwindigkeit
   bleiben bei diesem Anfangszustand konstant.

Die Quelldateien sind `examples/documentation/material_main.c`,
`material_main.phys`, `material_analysis.c` und `material_analysis.phys`.
Ein nativer C17-Compiler und die Physim-Kernbibliothek genügen; weitere
Abhängigkeiten oder eigene CMake-Dateien sind nicht nötig.

## Erwartete Werte und Prüfungen

Die Standardkugel hat `V≈0,0005235988 m³` und `m≈1,308996939 kg`.
Nach einer Sekunde sind `y≈0,419412946 m`, `v≈-0,0817220833 m/s`,
`Re≈0,0817221` und `E_balance≈3,85106245 J`. Die Gleichgewichts-Endgeschwindigkeit
ist kein beliebig universeller Sedimentationswert: Sie folgt den hier gewählten
Demonstrationsparametern und Modellannahmen.

`material_tutorial` prüft jeweils 1001 komplette Messzeilen und CRC-geschützte
Szenen für sieben Konfigurationen in beiden Sprachen. Ein unabhängiger Parser
berechnet die exakte Lösung, Masse, Kräfte und Energien. Ort muss innerhalb
`1e-8 m`, Geschwindigkeit innerhalb `2e-7 m/s` und Energiebilanz innerhalb
`1e-7 J` liegen. Die Halbierung des Standardzeitschritts muss den größten
Geschwindigkeitsfehler mindestens um Faktor 14 verkleinern. Alle vier gemischten
Analysewege werden einschließlich drei Plots, sieben vollständigen Kurven,
SI-Dimensionen und CSV geprüft. Unzulässige Viskosität, Reynolds-Zahl und
Zeitschritte werden in beiden Sprachen abgewiesen.

`documentation_material_source` verlangt bytegetreue Übereinstimmung der
folgenden Codeblöcke mit den tatsächlich gebauten Quellen.
`material_tutorial_workflow` baut echte C- und Physim-Projekte in der App,
führt jeweils 40 Einzelschritte aus und öffnet den aufgezeichneten Lauf und
seinen Analysebericht. Tatsächlich ausgeführte Plattformprüfungen stehen im
[Plattformnachweis](platform-validation.md).

## Experiment in C

```c
#include "physim/experiment.h"
#include "physim/mechanics.h"
#include "physim/units.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    ps_material material;
    ps_medium medium;
    double radius, volume, mass, lambda;
    double state[3]; /* y, vertical velocity, dissipated work; SI */
} settling;
static const double gravity = 9.80665;
static ps_result forces(const settling *s, double velocity, double *weight,
                        double *buoyancy, double *drag) {
    ps_vec3 b, d;
    ps_result result = ps_buoyancy_force(s->medium.density_kg_m3, s->volume,
                                       ps_v3(0, -gravity, 0), &b);
    if (result == PS_OK)
        result = ps_sphere_drag(ps_v3(0, velocity, 0), s->medium,
                                PS_DRAG_STOKES, s->radius, 0, &d);
    if (result != PS_OK) return result;
    *weight = -s->mass * gravity; *buoyancy = b.y; *drag = d.y;
    return PS_OK;
}
static void slope(double time, const double *state, double *out, void *user) {
    (void)time;
    settling *s = user; double w, b, d;
    if (forces(s, state[1], &w, &b, &d) != PS_OK) {
        out[0] = out[1] = out[2] = NAN; return;
    }
    out[0] = state[1]; out[1] = (w + b + d) / s->mass;
    out[2] = -d * state[1];
}
static ps_result measure(ps_context *c, const double *state, double time) {
    settling *s = c->user; double w, b, d;
    ps_result result = forces(s, state[1], &w, &b, &d);
    if (result != PS_OK) return result;
    double re = s->medium.viscosity_pa_s > 0
        ? 2 * s->medium.density_kg_m3 * s->radius * fabs(state[1]) / s->medium.viscosity_pa_s : 0;
    if (!isfinite(re) || re > .1) {
        snprintf(c->error, sizeof c->error, "Stokes tutorial requires Reynolds <= 0.1"); return PS_INVALID;
    }
    double acceleration = (w + b) / s->mass, exact_y, exact_v;
    double z = s->lambda * time;
    if (z <= .1) {
        double phi1 = 1, phi2 = .5, term1 = 1, term2 = .5;
        for (unsigned k = 1; k <= 8; k++) {
            term1 *= -z / (k + 1); term2 *= -z / (k + 2);
            phi1 += term1; phi2 += term2;
        }
        exact_v = acceleration * time * phi1;
        exact_y = .5 + acceleration * time * time * phi2;
    } else {
        double onset = -expm1(-z);
        exact_v = acceleration / s->lambda * onset;
        exact_y = .5 + acceleration / s->lambda * (time - onset / s->lambda);
    }
    double potential = -(w + b) * state[0], kinetic = .5 * s->mass * state[1] * state[1];
    const double values[] = {state[0], state[1], s->mass, w, b, d, re, kinetic,
                             state[2], potential, kinetic + potential + state[2], exact_y, exact_v};
    for (unsigned i = 0; i < 13; i++) {
        if (!isfinite(values[i])) return PS_NUMERIC;
    }
    for (unsigned i = 0; i < 13; i++) c->values[i] = values[i];
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    settling *s = c->user; s->state[0] = .5; s->state[1] = s->state[2] = 0;
    return measure(c, s->state, 0);
}
static ps_result create(ps_context *c) {
    settling *s = calloc(1, sizeof *s); if (!s) return PS_MEMORY; c->user = s;
    s->material = (ps_material){2500, 0, 0, "custom sphere"};
    s->medium = (ps_medium){1000, 100, "custom viscous fluid"};
    ps_result r = ps_parameter_define(c, "materialDensity", "Material density in kg/m^3", 2500, 1, 10000, &s->material.density_kg_m3);
    if (r == PS_OK) r = ps_parameter_define(c, "mediumDensity", "Medium density in kg/m^3", 1000, 0, 10000, &s->medium.density_kg_m3);
    if (r == PS_OK) r = ps_parameter_define(c, "viscosity", "Dynamic viscosity in Pa s", 100, 0, 10000, &s->medium.viscosity_pa_s);
    if (r == PS_OK) r = ps_parameter_define(c, "radius", "Sphere radius in m", .05, .001, 1, &s->radius);
    if (r != PS_OK) return r;
    if (s->medium.density_kg_m3 > 0 && s->medium.viscosity_pa_s == 0) {
        snprintf(c->error, sizeof c->error, "Nonzero fluid density requires positive viscosity"); return PS_INVALID;
    }
    s->volume = 4.0 / 3.0 * PS_PI * s->radius * s->radius * s->radius;
    s->mass = s->material.density_kg_m3 * s->volume;
    s->lambda = 6 * PS_PI * s->medium.viscosity_pa_s * s->radius / s->mass;
    const char *names[] = {"position.y", "velocity.y", "mass", "force.weight", "force.buoyancy", "force.drag", "reynolds", "energy.kinetic", "energy.dissipated", "energy.potential", "energy.balance", "reference.y", "reference.velocity"};
    for (unsigned i = 0; i < 13; i++) {
        ps_unit unit = i == 0 || i == 11 ? PS_METRE : i == 1 || i == 12 ? PS_VELOCITY : i == 2 ? PS_KILOGRAM : i < 6 ? PS_NEWTON : i == 6 ? PS_ONE : PS_JOULE;
        if (ps_channel_add(c, names[i], unit, names[i]) != (int)i) return PS_LIMIT;
    }
    snprintf(c->model_metadata, sizeof c->model_metadata,
        "model=custom-material sphere in uniform custom medium\nmaterial=custom sphere\nmedium=custom viscous fluid\nmaterial_density_kg_m3=%.17g\nmedium_density_kg_m3=%.17g\nviscosity_pa_s=%.17g\nradius_m=%.17g\nmass_kg=%.17g\ngravity_m_s2=9.80665\nintegrator=RK4\nforces=weight, full-volume buoyancy, Stokes drag\nvalidity=Re<=0.1; lambda*dt<=0.25\nexcluded=walls, contacts, surface, added mass, history force, non-Newtonian rheology\n", s->material.density_kg_m3, s->medium.density_kg_m3, s->medium.viscosity_pa_s, s->radius, s->mass);
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    settling *s = c->user;
    if (s->lambda * dt > .25) {
        snprintf(c->error, sizeof c->error, "RK4 tutorial requires lambda*dt <= 0.25"); return PS_INVALID;
    }
    double next[3] = {s->state[0], s->state[1], s->state[2]};
    ps_result r = ps_ode_step(PS_RK4, slope, s, c->time_s, dt, next, 3);
    if (r == PS_OK) r = measure(c, next, c->time_s + dt);
    if (r == PS_OK) for (unsigned i = 0; i < 3; i++) s->state[i] = next[i];
    return r;
}
static void scene(ps_context *c, ps_scene *out) {
    settling *s = c->user; ps_vec3 p = ps_v3(0, s->state[0], 0);
    ps_scene_add_id(out, 1, PS_SPHERE, p, p, s->radius, 0x53dec2ff);
    for (unsigned i = 0; i < 3; i++) {
        ps_vec3 a = ps_v3((i + 1) * .12, s->state[0], 0);
        ps_vec3 b = ps_vadd(a, ps_v3(0, .03 * c->values[3 + i], 0));
        const uint32_t colors[] = {0xf2c572ff, 0x53aeefff, 0xf57f9bff};
        ps_scene_add_id(out, i + 2, PS_ARROW, a, b, .003, colors[i]);
    }
    ps_scene_label_id(out, 5, ps_v3(-.35, .7, 0), "Custom material + medium", 0xc8d7eaff);
}
static void destroy(ps_context *c) { free(c->user); c->user = NULL; }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api, PS_ABI_VERSION, 0,
        "Custom material and medium", create, reset, step, scene, destroy, NULL}; return &api;
}
```

## Experiment in Physim

```physim
// Same RK4 state, forces and validity limits as material_main.c.
let gravity = 9.80665
let radius = parameter("radius",0.05,0.001,1,"Sphere radius in m")
let density = parameter("materialDensity",2500,1,10000,"Material density in kg/m^3")
let fluidDensity = parameter("mediumDensity",1000,0,10000,"Medium density in kg/m^3")
let viscosity = parameter("viscosity",100,0,10000,"Dynamic viscosity in Pa s")
let material = Material(density,0,0)
let medium = Medium(fluidDensity,viscosity)
let volume = 4.0 / 3.0 * 3.14159265358979323846 * radius * radius * radius
let mass = material.density * volume
let lambda = 6 * 3.14159265358979323846 * medium.viscosity * radius / mass
let metres = Unit(1,0,0,0,0,0,0,1,"m")
let speed = Unit(1,0,-1,0,0,0,0,1,"m/s")
let kilograms = Unit(0,1,0,0,0,0,0,1,"kg")
let newtons = Unit(1,1,-2,0,0,0,0,1,"N")
let joules = Unit(2,1,-2,0,0,0,0,1,"J")
let one = Unit(0,0,0,0,0,0,0,1,"1")
let cy = Channel("position.y",metres,"position.y")
let cv = Channel("velocity.y",speed,"velocity.y")
let cm = Channel("mass",kilograms,"mass")
let cw = Channel("force.weight",newtons,"force.weight")
let cb = Channel("force.buoyancy",newtons,"force.buoyancy")
let cd = Channel("force.drag",newtons,"force.drag")
let cr = Channel("reynolds",one,"reynolds")
let ck = Channel("energy.kinetic",joules,"energy.kinetic")
let ce = Channel("energy.dissipated",joules,"energy.dissipated")
let cp = Channel("energy.potential",joules,"energy.potential")
let balance = Channel("energy.balance",joules,"energy.balance")
let ry = Channel("reference.y",metres,"reference.y")
let rv = Channel("reference.velocity",speed,"reference.velocity")
let weight = -mass * gravity
let buoyancy = buoyancyForce(medium.density,volume,Vec3(0,-gravity,0)).y
var state = Vec4(0.5,0,0,0)
var elapsed = 0.0
func drag(velocity: Float64) -> Float64:
    return medium.stokesDrag(Vec3(0,velocity,0),radius).y
func slope(value: Vec4) -> Vec4:
    let force = drag(value.y)
    return Vec4(value.y,(weight + buoyancy + force) / mass,-force * value.y,0)
func measure():
    let force = drag(state.y)
    var re = 0.0
    if medium.viscosity > 0:
        re = 2 * medium.density * radius * abs(state.y) / medium.viscosity
    assert(re <= 0.1,"Stokes tutorial requires Reynolds <= 0.1")
    let acceleration = (weight + buoyancy) / mass
    let z = lambda * elapsed
    var exactY = 0.5
    var exactV = 0.0
    if z <= 0.1:
        var phi1 = 1.0
        var phi2 = 0.5
        var term1 = 1.0
        var term2 = 0.5
        for k in 1..<9:
            term1 *= -z / Float64(k + 1)
            term2 *= -z / Float64(k + 2)
            phi1 += term1
            phi2 += term2
        exactV = acceleration * elapsed * phi1
        exactY = 0.5 + acceleration * elapsed * elapsed * phi2
    else:
        let onset = 1 - exp(-z)
        exactV = acceleration / lambda * onset
        exactY = 0.5 + acceleration / lambda * (elapsed - onset / lambda)
    let kinetic = 0.5 * mass * state.y * state.y
    let potential = -(weight + buoyancy) * state.x
    cy.sample(state.x)
    cv.sample(state.y)
    cm.sample(mass)
    cw.sample(weight)
    cb.sample(buoyancy)
    cd.sample(force)
    cr.sample(re)
    ck.sample(kinetic)
    ce.sample(state.z)
    cp.sample(potential)
    balance.sample(kinetic + potential + state.z)
    ry.sample(exactY)
    rv.sample(exactV)
func create():
    assert(medium.density == 0 || medium.viscosity > 0,"Nonzero fluid density requires positive viscosity")
    metadata("model=custom-material sphere in uniform custom medium\nmaterial=custom sphere\nmedium=custom viscous fluid\nmaterial_density_kg_m3=" + String(material.density) + "\nmedium_density_kg_m3=" + String(medium.density) + "\nviscosity_pa_s=" + String(medium.viscosity) + "\nradius_m=" + String(radius) + "\nmass_kg=" + String(mass) + "\ngravity_m_s2=9.80665\nintegrator=RK4\nforces=weight, full-volume buoyancy, Stokes drag\nvalidity=Re<=0.1; lambda*dt<=0.25\nexcluded=walls, contacts, surface, added mass, history force, non-Newtonian rheology\n")
func reset():
    state = Vec4(0.5,0,0,0)
    elapsed = 0
    measure()
func step(dt: Float64):
    assert(lambda * dt <= 0.25,"RK4 tutorial requires lambda*dt <= 0.25")
    let k1 = slope(state)
    let k2 = slope(state + 0.5 * dt * k1)
    let k3 = slope(state + 0.5 * dt * k2)
    let k4 = slope(state + dt * k3)
    state += dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
    elapsed += dt
    measure()
func scene():
    sphere(Vec3(0,state.x,0),radius,1407107839,1)
    let forces = [weight,buoyancy,drag(state.y)]
    let colors = [4073026303,1403973631,4118780927]
    for i in 0..<3:
        let start = Vec3(Float64(i+1) * 0.12,state.x,0)
        arrow(start,start + Vec3(0,0.03 * forces[i],0),0.003,colors[i],i+2)
    label(Vec3(-0.35,0.7,0),"Custom material + medium",3369593599,5)
```

## Auswertung in C

```c
#include "physim/report.h"
#include <stdio.h>
static ps_result plot(ps_report *report, ps_analysis_context *context, ps_series time,
                      ps_series *values, const char *const *labels, unsigned count,
                      const char *title, ps_unit unit) {
    ps_plot_info info = {0}; snprintf(info.title, sizeof info.title, "%s", title);
    snprintf(info.x_label, sizeof info.x_label, "Time"); snprintf(info.y_label, sizeof info.y_label, "%s", title);
    ps_result r = ps_report_unit_from(PS_SECOND, &info.x_unit);
    if (r == PS_OK) r = ps_report_unit_from(unit, &info.y_unit);
    ps_plot_handle handle;
    if (r == PS_OK) r = ps_report_add_plot(report, &info, &handle);
    for (unsigned i = 0; r == PS_OK && i < count; i++)
        r = ps_report_add_series(report, handle, context, time, values[i], labels[i], PS_PLOT_LINE);
    return r;
}
static ps_result analyze(const char *input, const char *prefix) {
    ps_analysis_context *context = NULL; ps_report *report = NULL; ps_dataset run = {0};
    ps_series time = {0}, values[7] = {0}; bool recovered = false;
    ps_result r = ps_analysis_create(prefix, 0, &context);
    if (r == PS_OK) { r = ps_analysis_open_run(context, input, &run); recovered = r == PS_RECOVERED; if (recovered) r = PS_OK; }
    if (r == PS_OK) r = ps_dataset_series(context, run, "time", &time);
    const char *names[] = {"velocity.y", "reference.velocity", "energy.kinetic", "energy.potential", "energy.dissipated", "energy.balance"};
    for (unsigned i = 0; r == PS_OK && i < 6; i++) r = ps_dataset_series(context, run, names[i], &values[i]);
    if (r == PS_OK) r = ps_series_combine(context, PS_SERIES_SUBTRACT, values[0], values[1], &values[6]);
    if (r == PS_OK) r = ps_report_create("Custom material and medium", "Stokes settling and energy accounting", &report);
    const char *velocity_labels[] = {"RK4", "analytic"}, *energy_labels[] = {"kinetic", "effective potential", "dissipated", "total"}, *error_labels[] = {"RK4 minus analytic"};
    if (r == PS_OK) r = plot(report, context, time, values, velocity_labels, 2, "Vertical velocity", PS_VELOCITY);
    if (r == PS_OK) r = plot(report, context, time, &values[6], error_labels, 1, "Velocity error", PS_VELOCITY);
    if (r == PS_OK) r = plot(report, context, time, &values[2], energy_labels, 4, "Energy accounting", PS_JOULE);
    char path[4096];
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s.psreport", prefix);
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s-material_check.csv", prefix);
        ps_series columns[] = {time, values[0], values[1], values[6], values[5]};
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_series_export_csv(context, columns, 5, path);
    }
    ps_report_destroy(report); ps_analysis_destroy(context); return r == PS_OK && recovered ? PS_RECOVERED : r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {.struct_size=sizeof api, .abi_version=PS_ABI_VERSION, .name="Custom material and medium analysis", .run=analyze}; return &api;
}
```

## Auswertung in Physim

```physim
func analyze():
    report("Custom material and medium")
    let run = Dataset(0)
    let time = run.series("time")
    let velocity = run.series("velocity.y")
    let reference = run.series("reference.velocity")
    let error = velocity.subtracting(reference)
    let graph = velocity.plot(time,"Vertical velocity","RK4")
    graph.curve(time,reference,"analytic")
    error.plot(time,"Velocity error","RK4 minus analytic")
    let kinetic = run.series("energy.kinetic")
    let potential = run.series("energy.potential")
    let dissipated = run.series("energy.dissipated")
    let total = run.series("energy.balance")
    let energy = kinetic.plot(time,"Energy accounting","kinetic")
    energy.curve(time,potential,"effective potential")
    energy.curve(time,dissipated,"dissipated")
    energy.curve(time,total,"total")
    Series.exportColumns([time,velocity,reference,error,total],"material_check")
    run.close()
```

Für `z = λt ≤ 0,1` berechnen beide Beispiele die Referenz mit Taylorreihen
bis einschließlich `z⁸`. Damit bleiben Geschwindigkeit und Position auch beim
Übergang zu verschwindender Viskosität stabil; die Subtraktion fast gleicher
Exponentialwerte entfällt. Die erste ausgelassene Potenz beträgt im
Geschwindigkeitsfaktor höchstens `0,1⁹ / 10!`, im Positionsfaktor
`0,1⁹ / 11!`.
