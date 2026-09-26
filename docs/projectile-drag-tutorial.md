# Wurfparabel mit Luftwiderstand: C und Physim-Sprache

## Lernziel und Modellannahmen

Dieses Beispiel erweitert den [Vakuumwurf](projectile-tutorial.md) um
quadratischen Luftwiderstand. Derselbe Körper startet bei `(-2 m, 0 m)`
mit `vx = 2 m/s`, `vy = 5 m/s` und `m = 1 kg`. Die Luft ruht, hat eine
konstante Dichte `ρ = 1,225 kg/m³`; der Widerstandsbeiwert ist `Cd = 0,47`
und die projizierte Fläche `A = 0,01 m²`. Die Schwerkraft beträgt
`g = 9,80665 m/s²`. Es gibt keinen Bodenkontakt, Auftrieb, Wind oder
Rotationswiderstand. Der feste Seed 42 wird nicht genutzt, macht die Läufe
aber vergleichbar.

Mit Position `r`, Geschwindigkeit `v` und der nach oben positiven Y-Achse
lautet das Modell:

```text
dr/dt = v
Fdrag = -0,5 · ρ · Cd · A · |v| · v
dv/dt = (0, -g) + Fdrag / m
E = 0,5 · m · |v|² + m · g · y
```

Beide Fassungen berechnen pro Zeitschritt vier RK4-Stufen aus demselben
Zustand. Anders als beim Vakuumwurf ist `E` hier nicht konstant: Der
Widerstand dissipiert Energie. Die C-Fassung gibt `A` direkt an
`ps_drag_force` weiter. Die Physim-Funktion `quadraticDrag` erwartet einen
Kugelradius; `sqrt(A/π)` stellt dieselbe Querschnittsfläche her.

## Projekt und Ergebnis

Lege ein Projekt mit einem C- oder Physim-Experiment an und ersetze die
Experimentquelle durch eine der folgenden Fassungen. Die Analysesprache
darf unabhängig davon C oder Physim sein. **F5** baut die Module, **F6**
startet den Lauf. Wähle `dt = 0,005 s`, stoppe nach 200 Schritten bei
`t = 1 s` und starte die Analyse des gespeicherten Laufs. Die Quellen liegen
unter `examples/projectile/main.c`, `examples/language/projectile_drag.phys`
und `examples/documentation/drag_analysis.*`.

Bei `t = 1 s` liegen die Werte ungefähr bei `x = -0,009598 m`,
`y = 0,086075 m`, `vx = 1,981052 m/s`, `vy = -4,808007 m/s` und
`E = 14,364852 J`. Der Vakuumwurf erreicht zur selben Zeit `x = 0 m`,
`y = 0,096675 m` und behält `14,5 J`. Die horizontale Geschwindigkeit
fällt im Auswertungsdiagramm leicht ab. Ein einzelner Lauf beweist die
Genauigkeit des numerischen Verfahrens nicht; prüfe für eigene Parameter
auch halbierte Schrittweiten.

## Experiment in C

Das C-Modul verwendet `ps_ode_step(PS_RK4, ...)`, Messkanäle und die
Szenenfunktionen der gemeinsamen Bibliothek. Eine Punktspur wird alle
zehn Schritte ergänzt und auf 64 Punkte begrenzt.

```c
#include "physim/experiment.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    double y[4];
    ps_vec3 trail[64];
    unsigned trail_count, ticks;
} projectile;
static void ode(double t, const double *y, double *d, void *u) {
    (void)t;
    (void)u;
    ps_vec3 drag = ps_drag_force(ps_v3(y[2], y[3], 0), PS_AIR, 0.47, 0.01);
    d[0] = y[2];
    d[1] = y[3];
    d[2] = drag.x;
    d[3] = -9.80665 + drag.y;
}
static void measure(ps_context *c) {
    projectile *p = c->user;
    for (int i = 0; i < 4; i++)
        c->values[i] = p->y[i];
    c->values[4] = 0.5 * (p->y[2] * p->y[2] + p->y[3] * p->y[3]) + 9.80665 * p->y[1];
}
static ps_result reset(ps_context *c) {
    projectile *p = c->user;
    p->y[0] = -2;
    p->y[1] = 0;
    p->y[2] = 2;
    p->y[3] = 5;
    p->trail_count = 1;
    p->ticks = 0;
    p->trail[0] = ps_v3(p->y[0], p->y[1], 0);
    measure(c);
    return PS_OK;
}
static ps_result create(ps_context *c) {
    c->user = calloc(1, sizeof(projectile));
    if (!c->user)
        return PS_MEMORY;
    ps_channel_add(c, "position.x", PS_METRE, "x");
    ps_channel_add(c, "position.y", PS_METRE, "y");
    ps_channel_add(c, "velocity.x", PS_VELOCITY, "vx");
    ps_channel_add(c, "velocity.y", PS_VELOCITY, "vy");
    ps_channel_add(c, "energy", PS_JOULE, "Mechanical energy");
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=projectile, gravity and quadratic drag, no "
             "ground\nmass_kg=1\nmedium=air\ndensity_kg_m3=1.225\nCd=0.47\narea_m2=0."
             "01\nintegrator=RK4");
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    projectile *p = c->user;
    ps_result r = ps_ode_step(PS_RK4, ode, NULL, c->time_s, dt, p->y, 4);
    measure(c);
    if (r == PS_OK && ++p->ticks % 10 == 0) {
        if (p->trail_count == 64) {
            memmove(p->trail, p->trail + 1, 63 * sizeof *p->trail);
            p->trail_count = 63;
        }
        p->trail[p->trail_count++] = ps_v3(p->y[0], p->y[1], 0);
    }
    return r;
}
static void scene(ps_context *c, ps_scene *s) {
    projectile *state = c->user;
    ps_vec3 p = ps_v3(c->values[0], c->values[1], 0);
    ps_scene_add_id(s, 1, PS_SPHERE, p, p, 0.1, 0x53dec2ff);
    ps_scene_add_id(s, 2, PS_ARROW, p,
                    ps_vadd(p, ps_v3(c->values[2] * 0.15, c->values[3] * 0.15, 0)), 0, 0xf2c572ff);
    if (state->trail_count >= 2)
        (void)ps_scene_polyline_id(s, 3, state->trail, state->trail_count, .008, 0x638aafff);
    ps_scene_add_id(s, 4, PS_POINT, ps_v3(-2, 0, 0), ps_v3(0, 0, 0), .03, 0xe2eaf2ff);
    (void)ps_scene_label_id(s, 5, p, "Wurfkoerper", 0xe2eaf2ff);
}
static void destroy(ps_context *c) { free(c->user); }
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0,      "Wurf mit Luftwiderstand", create, reset,
        step,       scene,          destroy};
    return &api;
}
```

## Experiment in der Physim-Sprache

Die Sprachfassung formuliert dieselben vier RK4-Stufen ausdrücklich.
`Vec4` enthält `x`, `y`, `vx` und `vy`. `Medium.air().dragForce` verwendet die
geprüfte Kraftfunktion der Mechanikbibliothek; der Sprachcode integriert
Kraft durch die Masse von 1 kg. Der Widerstandsbeiwert ist ein benannter
Experimentparameter mit dem Standardwert 0,47 und wird nach dem Build in der
App angezeigt. Arraymethoden verwalten die Punktspur.

```physim
// Same 1 kg projectile, quadratic air drag and RK4 stages as the C example.
let gravity = 9.80665
let air = Medium.air()
let dragCoefficient = parameter("dragCoefficient", 0.47, 0.0, 2.0, "Quadratic drag coefficient")
let crossSection = 0.01
let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
let speed = Unit(1, 0, -1, 0, 0, 0, 0, 1, "m/s")
let joules = Unit(2, 1, -2, 0, 0, 0, 0, 1, "J")
let cx = Channel("position.x", metres, "x")
let cy = Channel("position.y", metres, "y")
let cvx = Channel("velocity.x", speed, "vx")
let cvy = Channel("velocity.y", speed, "vy")
let ce = Channel("energy", joules, "Mechanical energy")
var state = Vec4(-2, 0, 2, 5)
var trail = [Vec3(-2, 0, 0)]
var ticks = 0

func slope(value: Vec4) -> Vec4:
    let drag = air.dragForce(Vec3(value.z, value.w, 0), dragCoefficient, crossSection)
    return Vec4(value.z, value.w, drag.x, -gravity + drag.y)

func measure():
    cx.sample(state.x)
    cy.sample(state.y)
    cvx.sample(state.z)
    cvy.sample(state.w)
    ce.sample(0.5 * (state.z * state.z + state.w * state.w) + gravity * state.y)

func create():
    metadata("model=projectile, gravity and quadratic drag, no ground\nmass_kg=1\nmedium=air\ndensity_kg_m3=1.225\nCd=0.47\narea_m2=0.01\nintegrator=RK4")

func reset():
    state = Vec4(-2, 0, 2, 5)
    trail = [Vec3(-2, 0, 0)]
    ticks = 0
    measure()

func step(dt: Float64):
    let k1 = slope(state)
    let k2 = slope(state + 0.5 * dt * k1)
    let k3 = slope(state + 0.5 * dt * k2)
    let k4 = slope(state + dt * k3)
    state += dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
    measure()
    ticks += 1
    if ticks % 10 == 0:
        if trail.count == 64:
            trail.remove(at: 0)
        trail.append(Vec3(state.x, state.y, 0))

func scene():
    let position = Vec3(state.x, state.y, 0)
    let tip = position + Vec3(state.z * 0.15, state.w * 0.15, 0)
    sphere(position, 0.1, 1407107839, 1)
    arrow(position, tip, 0, 4073026303, 2)
    if trail.count >= 2:
        polyline(trail, 0.008, 1670033407, 3)
    point(Vec3(-2, 0, 0), 0.03, 3807048447, 4)
    label(position, "Wurfkoerper", 3807048447, 5)
```

## Auswertung in C

Die Analyse liest `time` und `position.x`, bildet `dx/dt`, erzeugt das
Geschwindigkeitsdiagramm und exportiert die vollständige CSV.

```c
#include "physim/report.h"
#include <stdio.h>

static ps_result analyze(const char *input, const char *prefix) {
    ps_analysis_context *context = NULL;
    ps_report *report = NULL;
    ps_dataset run = {0};
    ps_series time = {0}, position = {0}, velocity = {0};
    ps_result result = ps_analysis_create(prefix, 0, &context);
    bool recovered = false;
    if (result == PS_OK) {
        result = ps_analysis_open_run(context, input, &run);
        recovered = result == PS_RECOVERED;
        if (recovered)
            result = PS_OK;
    }
    if (result == PS_OK)
        result = ps_dataset_series(context, run, "time", &time);
    if (result == PS_OK)
        result = ps_dataset_series(context, run, "position.x", &position);
    if (result == PS_OK)
        result = ps_series_derivative(context, position, time, &velocity);
    if (result == PS_OK)
        result = ps_report_create("Projectile with quadratic air drag", "Horizontal velocity", &report);
    ps_plot_info info = {0};
    snprintf(info.title, sizeof info.title, "Horizontal velocity");
    snprintf(info.x_label, sizeof info.x_label, "Time");
    snprintf(info.y_label, sizeof info.y_label, "Velocity");
    ps_plot_handle plot = {0};
    if (result == PS_OK)
        result = ps_report_unit_from(PS_SECOND, &info.x_unit);
    if (result == PS_OK)
        result = ps_report_unit_from(PS_VELOCITY, &info.y_unit);
    if (result == PS_OK)
        result = ps_report_add_plot(report, &info, &plot);
    if (result == PS_OK)
        result = ps_report_add_series(report, plot, context, time, velocity, "dx/dt", PS_PLOT_LINE);
    char path[4096];
    if (result == PS_OK) {
        int size = snprintf(path, sizeof path, "%s.psreport", prefix);
        result = size < 0 || (size_t)size >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    if (result == PS_OK) {
        int size = snprintf(path, sizeof path, "%s-horizontal_velocity.csv", prefix);
        ps_series columns[] = {time, position, velocity};
        result = size < 0 || (size_t)size >= sizeof path
                     ? PS_LIMIT
                     : ps_series_export_csv(context, columns, 3, path);
    }
    ps_report_destroy(report);
    ps_analysis_destroy(context);
    return result == PS_OK && recovered ? PS_RECOVERED : result;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {sizeof api, PS_ABI_VERSION,
                                        "Projectile with quadratic air drag analysis", analyze, NULL};
    return &api;
}
```

## Auswertung in der Physim-Sprache

Die Sprachfassung verwendet `Dataset`, `Series` und `Plot` für denselben
Bericht. Die Ableitung ist eine Auswertung diskreter Positionen; die direkt
gemessene `velocity.x` kann an Randpunkten geringfügig davon abweichen.

```physim
func analyze():
    report("Projectile with quadratic air drag")
    let run: Dataset = Dataset(0)
    let time: Series = run.series("time")
    let position: Series = run.series("position.x")
    let horizontalVelocity: Series = position.derivative(time)
    let plot: Plot = horizontalVelocity.plot(time, "Horizontal velocity", "dx/dt")
    horizontalVelocity.export(time, "horizontal_velocity")
    run.close()
```

## Prüfen und Grenzen

`ctest --test-dir build-language -C Debug -R language_projectile_drag_parity
--output-on-failure` startet beide Experimente im echten Runner. Der Test
vergleicht 201 gespeicherte Zeilen und alle fünf Kanäle, prüft Energieabnahme,
Szenenobjekte, Reset und beide Analysen gegen den direkt gemessenen
Geschwindigkeitskanal. Der Quellabgleich
`documentation_projectile_drag_source` hält die vier Codeblöcke mit den
gebauten Dateien synchron.

Die Formel setzt einen konstanten `Cd` und eine konstante Fläche voraus.
Bei anderen Geschwindigkeiten, Reynolds-Zahlen, Wind oder einem Aufprall
kann dieses Modell ungeeignet sein. Die Spur visualisiert diskrete Orte;
sie ist keine exakte Bahnkurve. Für Einheiten, Kräfte und numerische
Fehlerabschätzung siehe [Mechanik](mechanics.md), [Numerik](numerics.md),
[Sprachvertrag](language.md) und [Experiment-API](reference/experiment.md).
