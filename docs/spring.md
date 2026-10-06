# Feder–Masse–Dämpfer: C und Physim

## Lernziel und Ablauf

Untersuche, wie Dämpfung eine Schwingung verändert, und trenne physikalischen
Energieverlust vom Integrationsfehler.

1. Unter **Öffnen oder anlegen** die Vorlage **Feder–Masse–Dämpfer** wählen.
   Wähle C oder Physim für das Experiment und unabhängig davon die Analysesprache.
   Ersetze die Analysedatei durch die passende vollständige Fassung weiter unten.
2. Mit F5 bauen, unter **Simulieren** starten und mindestens einige Sekunden laufen lassen.
3. Stoppen, **Auswerten**, **Analyse starten**. Im Diagrammmenü **Energy accounting** wählen. Die drei Kurven zeigen mechanische Energie, dissipierte
   Energie und deren Summe. Die Ergebnistabelle **Energy balance** nennt die maximale Abweichung der Summe von ihrem Anfangswert.
4. Unter **Simulieren → Inspector → Laufeinstellungen** den Parameter `damping`
   ändern (0 bis 64 N·s/m) und einen neuen Lauf starten; der bestehende Build genügt.
   Unter **Läufe & Berichte** beide Läufe auswählen und vergleichen. Der Vergleich
   zeigt Position, Geschwindigkeit und die interpolierte Positionsdifferenz.

Die C-Vorlage verwendet `physim/experiment.h` für Lebenszyklus, Messkanäle und Szene,
`physim/core.h` über diesen Header für RK4 und Vektoren sowie
`physim/mechanics.h` für die axiale Federkraft mit viskoser Dämpfung.
`analysis.c` ist dieselbe bearbeitbare Analysevorlage wie beim Pendel. Sie erkennt
die zusätzlich vorhandenen Energiekanäle automatisch. Bereits bestehende Projekte
behalten ihren bisherigen Analysecode.

## Modell und Gleichungen

Eine Masse bewegt sich auf einer vorgeschriebenen horizontalen Führung. Eine
masselose lineare Feder und ein linearer viskoser Dämpfer verbinden sie mit einem
festen Punkt. Die Gleichgewichtslage liegt bei X=0, der Anker bei X=-L. Solange die
Masse rechts des Ankers bleibt, ist X zugleich die Federdehnung:

```text
m*x'' + c*x' + k*x = 0
Federkraft = -k*x       Dämpfungskraft = -c*v
E_mechanisch = m*v²/2 + k*x²/2
dE_dissipiert/dt = c*v²
E_Bilanz = E_mechanisch + E_dissipiert ≈ E_mechanisch(0)
```

Die Dämpfungsfälle folgen aus `omega0 = sqrt(k/m)` und `alpha = c/(2*m)`.
Unterkritisch gilt `x = exp(-alpha*t)*(A*cos(w*t)+B*sin(w*t))` mit
`w = sqrt(omega0²-alpha²)`. Kritisch gilt `x = (A+B*t)*exp(-alpha*t)`;
überkritisch ist die Lösung die Summe zweier reeller abklingender Exponentialterme.
Die Konstanten folgen aus Anfangsort und Anfangsgeschwindigkeit. Gleichung,
Dämpfungsfälle und Energierate sind in [MIT: Damped Oscillations, Abschnitte 2–3](https://ocw.mit.edu/courses/res-8-009-introduction-to-oscillations-and-waves-summer-2017/mitres_8_009su17_lec4.pdf)
hergeleitet.

## Parameter und erwartete Ergebnisse

Die Dämpfung ist in beiden Vorlagen ein typisierter SI-Experimentparameter.
Wert, Standard, Grenzen, Einheit und Dimension stehen in der Messdatei.
`PS_SPRING_DAMPING` bleibt der kompilierte C-Standard für bestehende Referenzvarianten.

Standard sind m=1 kg, k=16 N/m, c=1,2 N·s/m, L=1 m, x(0)=0,35 m und v(0)=0.
Die anfängliche Energie beträgt 0,98 J. Die Schwingung klingt ab; ihre gedämpfte
Periode beträgt etwa 1,589 s. Alle Parameter stehen im Quellcode und in den
Laufmetadaten.

| Dämpfung c in N·s/m | Verhalten bei diesen Anfangswerten |
| --- | --- |
| 0 | ungedämpfte Schwingung, Periode pi/2 s |
| 1,2 | abklingende Schwingung |
| 8 | kritische Dämpfung, Rückkehr ohne Überschwingen |
| 12 | überkritische, langsamere Rückkehr ohne Überschwingen |

Die elf Kanäle enthalten Position, Geschwindigkeit, kinetische und elastische
Energie, mechanische Energie, dissipierte Energie, Energiebilanz, beide Kräfte,
Gesamtkraft und dissipierte Leistung. Die Bilanz nutzt eine separat integrierte
Arbeitsvariable, die dieselben RK4-Stufen wie die Bewegung verwendet; sie wird
nicht nachträglich auf konstante Energie gesetzt.

Die Szene zeigt eine blaue Masse, die Feder als räumliche Helix, einen schematischen
Dämpfer und eine weiße Gleichgewichtsmarkierung. Grüne Pfeile stellen Geschwindigkeit
dar, gelbe Federkraft und rosa Dämpfungskraft. Die angegebenen Pfeilskalen sind
Darstellungshilfen; Federwindungen und Dämpferkörper tragen keine eigene Dynamik.

## Numerische Grenzen und Prüfungen

RK4 ist explizit und nicht für beliebig steife Federn oder große Dämpfung bei
unverändertem Zeitschritt stabil. Zeitschritt verkleinern und Ergebnisse vergleichen.
Die Vorlage enthält weder Gravitation noch Kontakt, Federanschläge, trockene Reibung,
nichtlineare Kennlinien, Federmasse oder ein Temperaturmodell. Die Führung ist
vorgeschrieben; sie verwendet keinen allgemeinen Constraint-Solver. Das Durchqueren
des Ankers wird als außerhalb des Modells abgewiesen.

Automatische Tests vergleichen vier reale Läufe über je zehn Sekunden mit den
geschlossenen Lösungen. Bei dt=0,005 s gelten absolute Grenzen von 3e-8 m für den
Ort, 1e-7 m/s für die Geschwindigkeit und 1e-7 J für Energie und Bilanz. Die
Halbierung von dt=0,05 auf 0,025 s bei t=1 muss den Zustandsfehler um einen Faktor
zwischen 14 und 18 verkleinern. Diese Grenzen betreffen genau die getesteten
Parameter. Zusätzlich werden alle Punkte der drei Energiekurven, die Bilanzkennzahl
und der im Analysemanifest benannte Messkanal geprüft.

Die Standardanalyse verwendet `energy.balance` für die Energieabweichung, falls
dieser Kanal vorhanden ist; sonst verwendet sie weiterhin `energy`.
`energy_metric_channel` im Analysemanifest macht die Auswahl nachvollziehbar.
Ein abnehmender mechanischer Energieinhalt allein ist bei aktiver Dämpfung kein
Nachweis eines numerischen Fehlers.

## Vollständiger Lernweg in beiden Sprachen

Die folgenden vier Quellen ergeben ein eigenes Experiment mit Energieanalyse.
Die Experimentquellen sind dieselben Dateien, aus denen die App ihre Feder-
Vorlagen erzeugt. Für die hier gezeigte Analyse ersetze die Analysedatei durch
`examples/documentation/spring_analysis.c` oder `spring_analysis.phys`; beide
können C- und Physim-Läufe lesen. Die bisherige gemeinsame Vorlagenanalyse bleibt
weiter verfügbar. **F5** baut Experiment und Analyse; **F6** startet den Versuch.
Wähle `dt=0,005 s`, stoppe nach etwa zehn Sekunden und starte die Analyse.

Deren drei Diagramme zeigen Dehnung, Geschwindigkeit und mechanische Energie,
dissipierte Arbeit und Gesamtbilanz. Die Tabelle nennt die vollständige Zeilenzahl
und die maximale absolute Bilanzabweichung vom Anfangswert. Die Datei
`<prefix>-spring_check.csv` enthält sämtliche Zeit-, Zustands- und Energiewerte.
Sie verwendet keine reduzierten Vorschaupunkte. Bei t=0 beträgt die Bilanz 0,98 J.

Die Dämpfungsfälle 0, 1,2, 8 und 12 N·s/m müssen keine unterschiedlichen Quellen
verwenden: Ändere nur `damping` zwischen den Läufen. Wähle mehrere gespeicherte
Läufe über **Läufe & Berichte**, um ihre Messwerte zu vergleichen. Die unten
gezeigte Analyse liest eine ausgewählte Datei; sie ist bewusst keine Mehrlaufanalyse.
Sehr große Dämpfung verlangt kleinere Schritte. Die Eingabegrenze 64 N·s/m ist
kein Nachweis, dass dt=0,005 s für sämtliche Zwecke ausreichend genau ist.

`spring_tutorial` vergleicht vier echte Läufe pro Sprache mit der unabhängigen
analytischen Lösung: insgesamt 16.008 Messzeilen über zehn Sekunden. Es prüft
alle elf Kanäle samt Namen und SI-Dimensionen, vollständige aufgezeichnete Szenen
mit 13 Objekten und 65 Helixpunkten, Energie-/Arbeitsmonotonie, Parameterprovenienz
und RK4-Verfeinerung. Sechzehn gemischte Analysewege werden einschließlich aller
fünf vollständigen Kurven, ihrer Einheiten, CSV und der exakten Tabellenstatistik
geprüft. Zwei gleichzeitige Modellinstanzen mit unterschiedlicher Dämpfung müssen
unabhängig bleiben; Reset erhält die eigenen Parameter, ein ungültiger Schritt
verändert die Messwerte nicht. Die bisherigen `spring_reference`- und Sprach-
Lebenszyklusprüfungen bleiben erhalten.

`documentation_spring_source` gleicht jeden folgenden Codeblock mit seiner
getesteten Quelldatei ab. `spring_tutorial_workflow` legt echte C- und Physim-
Vorlagen an, wählt kritische Dämpfung, führt 40 Einzelschritte aus und öffnet die
41 gespeicherten Messzeilen mit der Energieanalyse in der App.
Tatsächlich ausgeführte Umgebungen stehen im [Plattformnachweis](platform-validation.md).

## Experiment in C

```c
#include "physim/experiment.h"
#include "physim/mechanics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Horizontal slider: SI parameters. Edit and rebuild with F5. */
#ifndef PS_SPRING_DAMPING
#define PS_SPRING_DAMPING 1.2
#endif
static const double mass_kg = 1, stiffness_n_m = 16, default_damping_ns_m = PS_SPRING_DAMPING;
static const double rest_length_m = 1, initial_extension_m = .35, initial_velocity_m_s = 0;
typedef struct {
    double y[3];
    double damping_ns_m;
} experiment; /* extension, velocity, dissipated work */
typedef struct { const experiment *state; ps_result error; } derivative_context;
static ps_result force(const experiment *e, double x, double v, ps_vec3 *out) {
    /* Crossing the anchor leaves this one-dimensional axial model's domain. */
    if (x <= -rest_length_m)
        return PS_INVALID;
    return ps_spring_force(ps_v3(x, 0, 0), ps_v3(v, 0, 0), ps_v3(-rest_length_m, 0, 0),
                           ps_v3(0, 0, 0), stiffness_n_m, rest_length_m, e->damping_ns_m, out);
}
static void derivative(double t, const double *y, double *dy, void *user) {
    (void)t;
    derivative_context *context = user;
    const experiment *e = context->state;
    ps_vec3 f;
    ps_result r = force(e, y[0], y[1], &f);
    if (r != PS_OK) {
        context->error = r;
        dy[0] = dy[1] = dy[2] = NAN;
        return;
    }
    dy[0] = y[1];
    dy[1] = f.x / mass_kg;
    dy[2] = e->damping_ns_m * y[1] * y[1];
}
static ps_result measure(ps_context *c, const double *y) {
    const experiment *e = c->user;
    ps_vec3 f;
    ps_result r = force(e, y[0], y[1], &f);
    if (r != PS_OK)
        return r;
    double kinetic = .5 * mass_kg * y[1] * y[1], potential = .5 * stiffness_n_m * y[0] * y[0];
    double values[] = {y[0],
                       y[1],
                       kinetic,
                       potential,
                       kinetic + potential,
                       y[2],
                       kinetic + potential + y[2],
                       -stiffness_n_m * y[0],
                       -e->damping_ns_m * y[1],
                       f.x,
                       e->damping_ns_m * y[1] * y[1]};
    for (unsigned i = 0; i < sizeof values / sizeof values[0]; i++)
        if (!isfinite(values[i]))
            return PS_NUMERIC;
    memcpy(c->values, values, sizeof values);
    return PS_OK;
}
static ps_result reset(ps_context *c) {
    experiment *e = c->user;
    double y[] = {initial_extension_m, initial_velocity_m_s, 0};
    ps_result r = measure(c, y);
    if (r == PS_OK)
        memcpy(e->y, y, sizeof y);
    return r;
}
static ps_result create(ps_context *c) {
    if (!isfinite(mass_kg) || mass_kg <= 0 || !isfinite(stiffness_n_m) || stiffness_n_m <= 0 ||
        !isfinite(default_damping_ns_m) || default_damping_ns_m < 0 || !isfinite(rest_length_m) ||
        rest_length_m <= 0 || !isfinite(initial_extension_m) || !isfinite(initial_velocity_m_s))
        return PS_INVALID;
    c->user = calloc(1, sizeof(experiment));
    if (!c->user)
        return PS_MEMORY;
    experiment *e = c->user;
    const ps_unit damping_unit = {{0, 1, -1, 0, 0, 0, 0}, 1, "N s/m"};
    ps_result parameter = ps_parameter_define_unit(c, "damping", "Viscous damping coefficient", damping_unit,
                                                  default_damping_ns_m, 0, 64, &e->damping_ns_m);
    if (parameter != PS_OK) return parameter;
    const ps_unit newton = {{1, 1, -2, 0, 0, 0, 0}, 1, "N"};
    const ps_unit watt = {{2, 1, -3, 0, 0, 0, 0}, 1, "W"};
    ps_channel_add(c, "position.x", PS_METRE, "Extension from equilibrium at X=0");
    ps_channel_add(c, "velocity.x", PS_VELOCITY, "Slider velocity");
    ps_channel_add(c, "energy.kinetic", PS_JOULE, "Kinetic energy");
    ps_channel_add(c, "energy.spring", PS_JOULE, "Elastic potential energy");
    ps_channel_add(c, "energy", PS_JOULE, "Mechanical energy; damping removes energy");
    ps_channel_add(c, "energy.dissipated", PS_JOULE,
                   "Integral of c*v^2, same RK4 stages as motion");
    ps_channel_add(c, "energy.balance", PS_JOULE,
                   "Mechanical plus dissipated energy; compare with initial");
    ps_channel_add(c, "force.spring.x", newton, "Elastic spring force");
    ps_channel_add(c, "force.damper.x", newton, "Viscous damping force");
    ps_channel_add(c, "force.total.x", newton, "Total axial force from mechanics API");
    ps_channel_add(c, "power.dissipated", watt, "Nonnegative damping power c*v^2");
    snprintf(c->model_metadata, sizeof c->model_metadata,
             "model=horizontal spring-mass-damper\nintegrator=RK4\n"
             "mass_kg=%.17g\nstiffness_n_m=%.17g\ndamping_ns_m=%.17g\n"
             "rest_length_m=%.17g\ninitial_extension_m=%.17g\ninitial_velocity_m_s=%.17g\n"
             "anchor_x_m=%.17g\nequilibrium_x_m=0\n"
             "constraints=prescribed 1D horizontal guide, no constraint solver\n"
             "gravity=none\ncontact=none\nspring_mass=0\nthermal_model=none\n"
             "dissipated_work=integrated c*v^2\ndomain=slider right of anchor\n",
             mass_kg, stiffness_n_m, e->damping_ns_m, rest_length_m, initial_extension_m,
             initial_velocity_m_s, -rest_length_m);
    return reset(c);
}
static ps_result step(ps_context *c, double dt) {
    experiment *e = c->user;
    double y[3];
    memcpy(y, e->y, sizeof y);
    derivative_context context = {e, PS_OK};
    ps_result r = ps_ode_step(PS_RK4, derivative, &context, c->time_s, dt, y, 3);
    if (context.error != PS_OK) {
        snprintf(c->error, sizeof c->error,
                 "Spring domain exceeded or invalid force; refine dt and check initial energy.");
        return context.error;
    }
    if (r == PS_OK)
        r = measure(c, y);
    if (r == PS_OK)
        memcpy(e->y, y, sizeof y);
    return r;
}
static void scene(ps_context *c, ps_scene *s) {
    double x = c->values[0], left = -rest_length_m;
    ps_vec3 coil[65];
    for (unsigned i = 0; i < 65; i++) {
        double u = (double)i / 64, radius = .075 * fmin(1, 12 * fmin(u, 1 - u));
        coil[i] = ps_v3(left + (x - left) * u, radius * sin(12 * PS_PI * u),
                        radius * cos(12 * PS_PI * u));
    }
    (void)ps_scene_polyline_id(s, 1, coil, 65, .009, 0x91a9c5ff);
    ps_scene_add_id(s, 2, PS_BOX, ps_v3(left - .06, 0, 0), ps_v3(.12, .7, .25), 0, 0x596675ff);
    ps_scene_add_id(s, 3, PS_BOX, ps_v3(x, 0, 0), ps_v3(.3, .3, .3), 0, 0x70b1eeff);
    ps_scene_add_id(s, 4, PS_LINE, ps_v3(left, -.18, 0), ps_v3(.7, -.18, 0), .008, 0x596675ff);
    /* Parallel damper symbol; geometry is illustrative, not a cylinder model. */
    ps_scene_add_id(s, 5, PS_LINE, ps_v3(left, -.3, 0), ps_v3(x, -.3, 0), .008, 0xa2b0c1ff);
    ps_scene_add_id(s, 6, PS_BOX, ps_v3(left * .6, -.3, 0), ps_v3(.3, .09, .09), 0, 0x596675ff);
    ps_scene_add_id(s, 7, PS_LINE, ps_v3(x, -.3, 0), ps_v3(x, -.15, 0), .008, 0xa2b0c1ff);
    ps_scene_add_id(s, 8, PS_POINT, ps_v3(0, -.18, .1), ps_v3(0, -.18, .1), .025, 0xe6edf3ff);
    const double y[] = {.25, .42, .59}, scale[] = {.2, .06, .06};
    const unsigned channel[] = {1, 7, 8};
    const uint32_t color[] = {0x6dcf94ff, 0xf2c572ff, 0xff8eafff};
    for (unsigned i = 0; i < 3; i++)
        ps_scene_add_id(s, 20 + i, PS_ARROW, ps_v3(x, y[i], 0),
                        ps_v3(x + c->values[channel[i]] * scale[i], y[i], 0), .008, color[i]);
    (void)ps_scene_label_id(s, 30, ps_v3(left, .85, 0), "Feder–Masse–Dämpfer", 0xe6edf3ff);
    (void)ps_scene_label_id(s, 31, ps_v3(left, -.6, 0), "v: 0.2 m pro m/s · Kräfte: 0.06 m pro N",
                            0xc0c6cfff);
}
static void destroy(ps_context *c) {
    free(c->user);
    c->user = NULL;
}
PS_EXPORT const ps_experiment_api *ps_get_experiment(void) {
    static const ps_experiment_api api = {
        sizeof api, PS_ABI_VERSION, 0, "Feder–Masse–Dämpfer", create, reset, step, scene, destroy, NULL};
    return &api;
}
```

## Experiment in Physim

```physim
// Horizontal spring-mass-damper, SI units; RK4 for motion and dissipated work.
let mass = 1.0
let stiffness = 16.0
let dampingUnit = Unit(0,1,-1,0,0,0,0,1,"N s/m")
let damping = parameterWithUnit("damping",dampingUnit,1.2,0,64,"Viscous damping coefficient")
let restLength = 1.0
let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
let speed = Unit(1, 0, -1, 0, 0, 0, 0, 1, "m/s")
let joules = Unit(2, 1, -2, 0, 0, 0, 0, 1, "J")
let newtons = Unit(1, 1, -2, 0, 0, 0, 0, 1, "N")
let watts = Unit(2, 1, -3, 0, 0, 0, 0, 1, "W")
let positionChannel = Channel("position.x", metres, "Extension from equilibrium")
let velocityChannel = Channel("velocity.x", speed, "Slider velocity")
let kineticChannel = Channel("energy.kinetic", joules, "Kinetic energy")
let potentialChannel = Channel("energy.spring", joules, "Spring energy")
let energyChannel = Channel("energy", joules, "Mechanical energy")
let workChannel = Channel("energy.dissipated", joules, "Integral of damping power")
let balanceChannel = Channel("energy.balance", joules, "Mechanical plus dissipated energy")
let springChannel = Channel("force.spring.x", newtons, "Elastic force")
let dampingChannel = Channel("force.damper.x", newtons, "Damping force")
let forceChannel = Channel("force.total.x", newtons, "Total force from mechanics library")
let powerChannel = Channel("power.dissipated", watts, "Damping power")
var state = Vec3(0.35, 0, 0)

func force(value: Vec3) -> Vec3:
    assert(value.x > -restLength)
    return springForce(Vec3(value.x, 0, 0), Vec3(value.y, 0, 0), Vec3(-restLength, 0, 0), Vec3(0, 0, 0), stiffness, restLength, damping)

func derivative(value: Vec3) -> Vec3:
    return Vec3(value.y, force(value).x / mass, damping * value.y * value.y)

func measure():
    let kinetic = 0.5 * mass * state.y * state.y
    let potential = 0.5 * stiffness * state.x * state.x
    positionChannel.sample(state.x)
    velocityChannel.sample(state.y)
    kineticChannel.sample(kinetic)
    potentialChannel.sample(potential)
    energyChannel.sample(kinetic + potential)
    workChannel.sample(state.z)
    balanceChannel.sample(kinetic + potential + state.z)
    springChannel.sample(-stiffness * state.x)
    dampingChannel.sample(-damping * state.y)
    forceChannel.sample(force(state).x)
    powerChannel.sample(damping * state.y * state.y)

func create():
    metadata("model=horizontal spring-mass-damper\nintegrator=RK4\nmass_kg=1\nstiffness_n_m=16\ndamping_ns_m=" + String(damping) + "\nrest_length_m=1\ninitial_extension_m=0.35\ninitial_velocity_m_s=0\nanchor_x_m=-1\nequilibrium_x_m=0\nconstraints=prescribed 1D horizontal guide, no constraint solver\ngravity=none\ncontact=none\nspring_mass=0\nthermal_model=none\ndissipated_work=integrated c*v^2\ndomain=slider right of anchor\n")

func reset():
    state = Vec3(0.35, 0, 0)
    measure()

func step(dt: Float64):
    assert(dt > 0,"Spring step requires positive dt")
    let k1 = derivative(state)
    let k2 = derivative(state + 0.5 * dt * k1)
    let k3 = derivative(state + 0.5 * dt * k2)
    let k4 = derivative(state + dt * k3)
    state += (dt / 6) * (k1 + 2 * k2 + 2 * k3 + k4)
    measure()

func scene():
    let left = -restLength
    let x = state.x
    var coil: [Vec3] = []
    for i in 0...64:
        let u = Float64(i) / 64
        let radius = 0.075 * min(1, 12 * min(u, 1 - u))
        coil.append(Vec3(left + (x - left) * u,
                         radius * sin(12 * 3.141592653589793 * u),
                         radius * cos(12 * 3.141592653589793 * u)))
    polyline(coil, 0.009, 0x91a9c5ff, 1)
    box(Vec3(left - 0.06, 0, 0), Vec3(0.12, 0.7, 0.25), Quat(0, 0, 0, 1), 0x596675ff, 2)
    box(Vec3(x, 0, 0), Vec3(0.3, 0.3, 0.3), Quat(0, 0, 0, 1), 0x70b1eeff, 3)
    line(Vec3(left, -0.18, 0), Vec3(0.7, -0.18, 0), 0.008, 0x596675ff, 4)
    line(Vec3(left, -0.3, 0), Vec3(x, -0.3, 0), 0.008, 0xa2b0c1ff, 5)
    box(Vec3(left * 0.6, -0.3, 0), Vec3(0.3, 0.09, 0.09), Quat(0, 0, 0, 1), 0x596675ff, 6)
    line(Vec3(x, -0.3, 0), Vec3(x, -0.15, 0), 0.008, 0xa2b0c1ff, 7)
    point(Vec3(0, -0.18, 0.1), 0.025, 0xe6edf3ff, 8)
    arrow(Vec3(x, 0.25, 0), Vec3(x + state.y * 0.2, 0.25, 0), 0.008, 0x6dcf94ff, 20)
    arrow(Vec3(x, 0.42, 0), Vec3(x - stiffness * state.x * 0.06, 0.42, 0), 0.008, 0xf2c572ff, 21)
    arrow(Vec3(x, 0.59, 0), Vec3(x - damping * state.y * 0.06, 0.59, 0), 0.008, 0xff8eafff, 22)
    label(Vec3(left, 0.85, 0), "Feder–Masse–Dämpfer", 0xe6edf3ff, 30)
    label(Vec3(left, -0.6, 0), "v: 0.2 m pro m/s · Kräfte: 0.06 m pro N", 0xc0c6cfff, 31)
```

## Auswertung in C

```c
#include "physim/report.h"
#include <math.h>
#include <stdio.h>
static ps_result add_plot(ps_report *report,ps_analysis_context *context,ps_series time,
    ps_series *values,const char *const *labels,unsigned count,const char *title,ps_unit unit) {
    ps_plot_info info={0};snprintf(info.title,sizeof info.title,"%s",title);
    snprintf(info.x_label,sizeof info.x_label,"Time");snprintf(info.y_label,sizeof info.y_label,"%s",title);
    ps_result r=ps_report_unit_from(PS_SECOND,&info.x_unit);if(r==PS_OK)r=ps_report_unit_from(unit,&info.y_unit);
    ps_plot_handle handle;if(r==PS_OK)r=ps_report_add_plot(report,&info,&handle);
    for(unsigned i=0;r==PS_OK && i<count;i++)r=ps_report_add_series(report,handle,context,time,values[i],labels[i],PS_PLOT_LINE);
    return r;
}
static ps_result analyze(const char *input,const char *prefix) {
    ps_analysis_context *context=NULL;ps_report *report=NULL;ps_dataset run={0};
    ps_series time={0},values[5]={0};bool recovered=false;ps_result r=ps_analysis_create(prefix,0,&context);
    if(r==PS_OK){r=ps_analysis_open_run(context,input,&run);recovered=r==PS_RECOVERED;if(recovered)r=PS_OK;}
    if(r==PS_OK)r=ps_dataset_series(context,run,"time",&time);
    const char *names[]={"position.x","velocity.x","energy","energy.dissipated","energy.balance"};
    for(unsigned i=0;r==PS_OK && i<5;i++)r=ps_dataset_series(context,run,names[i],&values[i]);
    if(r==PS_OK)r=ps_report_create("Spring mass damper","Motion and energy accounting",&report);
    const char *position[]={"extension"},*velocity[]={"velocity"},*energies[]={"mechanical","dissipated","total"};
    if(r==PS_OK)r=add_plot(report,context,time,&values[0],position,1,"Extension",PS_METRE);
    if(r==PS_OK)r=add_plot(report,context,time,&values[1],velocity,1,"Velocity",PS_VELOCITY);
    if(r==PS_OK)r=add_plot(report,context,time,&values[2],energies,3,"Energy accounting",PS_JOULE);
    ps_statistics stats={0};double initial=0;size_t read=0;
    if(r==PS_OK)r=ps_series_statistics(context,values[4],&stats);
    if(r==PS_OK)r=ps_series_read(context,values[4],0,&initial,1,&read);
    if(r==PS_OK && read!=1)r=PS_INVALID;
    ps_table_info table={0};snprintf(table.title,sizeof table.title,"Energy balance");table.columns=2;
    snprintf(table.column[0].label,sizeof table.column[0].label,"Samples");snprintf(table.column[1].label,sizeof table.column[1].label,"Maximum drift");
    if(r==PS_OK)r=ps_report_unit_from(PS_ONE,&table.column[0].unit);
    if(r==PS_OK)r=ps_report_unit_from(PS_JOULE,&table.column[1].unit);
    ps_table_handle handle;if(r==PS_OK)r=ps_report_add_table(report,&table,&handle);
    ps_table_row row={0};snprintf(row.label,sizeof row.label,"complete run");row.values[0]=(double)stats.count;
    row.values[1]=fmax(fabs(stats.min-initial),fabs(stats.max-initial));
    if(r==PS_OK)r=ps_report_add_row(report,handle,&row);
    char path[4096];if(r==PS_OK){int n=snprintf(path,sizeof path,"%s.psreport",prefix);r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_report_save(report,path);}
    if(r==PS_OK){int n=snprintf(path,sizeof path,"%s-spring_check.csv",prefix);ps_series columns[]={time,values[0],values[1],values[2],values[3],values[4]};r=n<0 || (size_t)n>=sizeof path?PS_LIMIT:ps_series_export_csv(context,columns,6,path);}
    ps_report_destroy(report);ps_analysis_destroy(context);return r==PS_OK && recovered?PS_RECOVERED:r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api={.struct_size=sizeof api,.abi_version=PS_ABI_VERSION,.name="Spring mass damper analysis",.run=analyze};return &api;
}
```

## Auswertung in Physim

```physim
func analyze():
    report("Spring mass damper")
    let run = Dataset(0)
    let time = run.series("time")
    let position = run.series("position.x")
    let velocity = run.series("velocity.x")
    position.plot(time,"Extension","extension")
    velocity.plot(time,"Velocity","velocity")
    let mechanical = run.series("energy")
    let dissipated = run.series("energy.dissipated")
    let balance = run.series("energy.balance")
    let energy = mechanical.plot(time,"Energy accounting","mechanical")
    energy.curve(time,dissipated,"dissipated")
    energy.curve(time,balance,"total")
    let joules = Unit(2,1,-2,0,0,0,0,1,"J")
    let one = Unit(0,0,0,0,0,0,0,1,"1")
    let initial = balance.value(0)
    let drift = max(abs(balance.minimum()-initial),abs(balance.maximum()-initial))
    let summary = Table("Energy balance",["Samples","Maximum drift"],[one,joules])
    summary.row("complete run",[Quantity(Float64(balance.count()),one),Quantity(drift,joules)])
    Series.exportColumns([time,position,velocity,mechanical,dissipated,balance],"spring_check")
    run.close()
```
