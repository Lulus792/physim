# Wurfparabel im Vakuum: C und Physim-Sprache

## Lernziel und Modell

Dieses Beispiel führt vom Quellcode über einen gespeicherten Lauf bis zum
Geschwindigkeitsdiagramm. Ein Körper mit 1 kg Masse startet bei
`(-2 m, 0 m)` mit `vx = 2 m/s` und `vy = 5 m/s`. Die konstante
Erdbeschleunigung beträgt `g = 9,80665 m/s²` nach unten. Luftwiderstand,
Bodenkontakt und Rotation sind ausgeschlossen. Der Seed 42 ist festgelegt,
obwohl das Modell keine Zufallszahlen verwendet.

Für Zeit `t` in Sekunden gelten exakt:

```text
x(t)  = -2 + 2t
y(t)  = 5t - 0,5 · 9,80665 · t²
vx(t) = 2
vy(t) = 5 - 9,80665t
E(t)  = 0,5 · (vx² + vy²) + g · y = 14,5 J
```

Wir verwenden 200 Schritte mit `dt = 0,005 s` und speichern auch den
Anfangswert: 201 Zeilen bis `t = 1 s`. Dann erwarten wir `x = 0 m`,
`y = 0,096675 m`, `vy = -4,80665 m/s` und unverändert `E = 14,5 J`.
Die horizontale Geschwindigkeit der abgeleiteten Position ist stets
`2 m/s`.

## Projekt anlegen und ausführen

Lege in Physim einen neuen Projektordner an. Wähle für Experiment und Analyse
jeweils C oder Physim-Sprache. Ersetze `main.c`/`analysis.c` beziehungsweise
`main.phys`/`analysis.phys` vollständig durch die folgenden Quellen.
Alle vier Dateien liegen unter `examples/documentation/`; die Sprachquelle
des Experiments liegt unter `examples/language/projectile.phys`.
**F5** baut die gewählte Kombination. Starte unter **Simulieren** mit **F6**,
stoppe nach einer simulierten Sekunde und starte unter **Auswerten** die
Analyse des neuen Laufs. Beide Analysen erzeugen ein Diagramm der aus
`position.x` abgeleiteten horizontalen Geschwindigkeit.

## Experiment in C

Die C-Fassung verwendet `experiment.h` für Kanäle, Szene und Modul-ABI.
`reset` setzt alle Zustandswerte und schreibt die Anfangsmessung. Der
Schritt integriert konstante Beschleunigung mit der exakten kinematischen
Formel. `destroy` gibt den zugewiesenen Zustand frei.

```c
#include "physim/experiment.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

enum { X, Y, VX, VY, ENERGY, CHANNELS };
static const double gravity = 9.80665;
typedef struct {
    double x, y, vx, vy;
} vacuum_projectile;

static void measure(ps_context *context) {
    vacuum_projectile *body = context->user;
    context->values[X] = body->x;
    context->values[Y] = body->y;
    context->values[VX] = body->vx;
    context->values[VY] = body->vy;
    context->values[ENERGY] =
        0.5 * (body->vx * body->vx + body->vy * body->vy) + gravity * body->y;
}
static ps_result reset(ps_context *context) {
    vacuum_projectile *body = context->user;
    *body = (vacuum_projectile){-2, 0, 2, 5};
    measure(context);
    return PS_OK;
}
static ps_result create(ps_context *context) {
    vacuum_projectile *body = calloc(1, sizeof *body);
    if (!body)
        return PS_MEMORY;
    context->user = body;
    if (ps_channel_add(context, "position.x", PS_METRE, "Horizontal position") != X ||
        ps_channel_add(context, "position.y", PS_METRE, "Vertical position") != Y ||
        ps_channel_add(context, "velocity.x", PS_VELOCITY, "Horizontal velocity") != VX ||
        ps_channel_add(context, "velocity.y", PS_VELOCITY, "Vertical velocity") != VY ||
        ps_channel_add(context, "energy", PS_JOULE, "Mechanical energy") != ENERGY) {
        free(body);
        context->user = NULL;
        snprintf(context->error, sizeof context->error, "Could not register projectile channels");
        return PS_LIMIT;
    }
    snprintf(context->model_metadata, sizeof context->model_metadata,
             "model=projectile, vacuum\nmass_kg=1\ngravity_m_s2=9.80665\n"
             "integrator=exact constant acceleration");
    return reset(context);
}
static ps_result step(ps_context *context, double dt) {
    vacuum_projectile *body = context->user;
    body->x += body->vx * dt;
    body->y += body->vy * dt - 0.5 * gravity * dt * dt;
    body->vy -= gravity * dt;
    measure(context);
    return PS_OK;
}
static void scene(ps_context *context, ps_scene *out) {
    vacuum_projectile *body = context->user;
    ps_vec3 point = ps_v3(body->x, body->y, 0);
    ps_vec3 tip = ps_v3(body->x + 0.15 * body->vx, body->y + 0.15 * body->vy, 0);
    (void)ps_scene_add_id(out, 1, PS_SPHERE, point, point, 0.1, 0x53dec2ff);
    (void)ps_scene_add_id(out, 2, PS_LINE, point, tip, 0.01, 0xf2c572ff);
}
static void destroy(ps_context *context) {
    free(context->user);
    context->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {sizeof api, PS_ABI_VERSION, 0,
                                          "Vacuum projectile", create, reset, step, scene, destroy, NULL};
    return &api;
}
```

## Auswertung in C

Die Analyse liest `time` und `position.x`, bildet `dx/dt`, erzeugt einen
Bericht und exportiert eine CSV. Sie räumt Bericht und Analysekontext auf,
auch wenn ein Zwischenschritt fehlschlägt.

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
        result = ps_report_create("Vacuum projectile", "Horizontal velocity", &report);
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
        int size = snprintf(path, sizeof path, "%s-horizontal-velocity.csv", prefix);
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
                                        "Vacuum projectile analysis", analyze, NULL};
    return &api;
}
```

## Experiment in der Physim-Sprache

Die Sprachfassung hält Position und Geschwindigkeit in einer eigenen
`Motion`-Struktur. `Unit`, `Quantity` und `Channel` beschreiben Einheiten
und Messwerte ausdrücklich. `create`, `reset`, `step` und `scene` sind die
Einstiegspunkte des nativen Experiments; der Compiler erzeugt daraus ein
Runner-Modul. Die Sprachversion verwendet dieselben Anfangswerte und
Gleichungen wie die C-Fassung.

```physim
// Constant-gravity vacuum model; exact kinematic step, no ground collision.
struct Motion:
    var position: Vec3
    var velocity: Vec3
let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
let seconds = Unit(0, 0, 1, 0, 0, 0, 0, 1, "s")
let kilograms = Unit(0, 1, 0, 0, 0, 0, 0, 1, "kg")
let speed = metres.divided(right: seconds, symbol: "m/s")
let energyUnit = kilograms.multiplied(right: speed.powered(exponent: 2, symbol: "m²/s²"), symbol: "J")
let cx = Channel("position.x", metres, "Horizontal position")
let cy = Channel("position.y", metres, "Vertical position")
let cvx = Channel("velocity.x", speed, "Horizontal velocity")
let cvy = Channel("velocity.y", speed, "Vertical velocity")
let ce = Channel("energy", energyUnit, "Mechanical energy")
let gravity = Quantity(9.80665, metres.divided(seconds.powered(2, "s²"), "m/s²"))
let g: Float64 = gravity.value
var state = Motion(Vec3(-2, 0, 0), Vec3(2, 5, 0))

func measure():
    cx.sample(state.position.x)
    cy.sample(state.position.y)
    cvx.sample(state.velocity.x)
    cvy.sample(state.velocity.y)
    ce.sample(0.5 * state.velocity.dot(state.velocity) + g * state.position.y)

func create():
    metadata("model=projectile, vacuum\nmass_kg=1\ngravity_m_s2=9.80665\nintegrator=exact constant acceleration")

func reset():
    state = Motion(Vec3(-2, 0, 0), Vec3(2, 5, 0))
    measure()

func step(dt: Float64):
    let acceleration = Vec3(0, -g, 0)
    state.position += state.velocity * dt + 0.5 * acceleration * dt * dt
    state.velocity += acceleration * dt
    measure()

func scene():
    sphere(state.position, 0.1, 1407107839, 1)
    let tip = state.position + state.velocity * 0.15
    line(state.position, tip, 0.01, 4073026303, 2)
```

## Auswertung in der Physim-Sprache

`Dataset(0)` öffnet den ausgewählten Lauf. `Series.derivative` verwendet
die Zeitreihe als Raster. `plot` schreibt das Ergebnis in den Bericht,
`export` die vollständige Datenreihe. `run.close()` beendet den
Dataset-Zugriff.

```physim
func analyze():
    report("Vacuum projectile")
    let run: Dataset = Dataset(0)
    let time: Series = run.series("time")
    let position: Series = run.series("position.x")
    let horizontalVelocity: Series = position.derivative(time)
    let plot: Plot = horizontalVelocity.plot(time, "Horizontal velocity", "dx/dt")
    horizontalVelocity.export(time, "horizontal_velocity")
    run.close()
```

## Ergebnis prüfen und Grenzen erkennen

Vergleiche unter **Simulieren** die fünf Kanäle `position.x`, `position.y`,
`velocity.x`, `velocity.y` und `energy`. Bei `t = 1 s` sollten die oben
genannten Werte erscheinen. Das Auswertungsdiagramm ist eine waagerechte
Linie bei `2 m/s`; seine CSV enthält 201 Werte. In der Szene zeigt eine
Kugel die Position, eine Linie die momentane Flugrichtung.

Im Quellbaum prüft `python3 tools/build.py --no-app --test --test-filter
documentation_vacuum_tutorial` beide Experimente und
beide Analysen gegen dieselben Gleichungen (Windows: `python`). Der Test kontrolliert außerdem
Reset, Szene, Kanalnamen, gespeicherte Läufe und Berichte. Ein Abweichen bei
halbiertem Zeitschritt sollte hier nur Rundung sein; das Integrationsschema
ist für konstante Beschleunigung exakt. Das Modell beendet den Flug **nicht**
am Boden und gilt nicht für Luftwiderstand. Für ein Modell mit Widerstand
benötigst du Kräfte, einen geeigneten Integrator und einen Konvergenztest.

Weiterführend: [Sprachvertrag](language.md), [Experiment-API in C](reference/experiment.md),
[Datenreihen](series.md), [Numerische Verfahren](numerics.md) und
[Fehler finden](troubleshooting.md).
