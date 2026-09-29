# Ein eigenes Experiment in C

Ziel: Eine Kugel bewegt sich mit 1,5 m/s entlang der X-Achse. Wir zeichnen ihre
Position auf, berechnen daraus die Geschwindigkeit und zeigen sie in einem eigenen
Bericht. Nach einer Sekunde erwarten wir 1,5 m, nach zwei Sekunden 3 m.
Es gibt keine Kräfte und keine Kollisionen; der Zeitschritt ist für dieses Modell exakt.

## Projekt vorbereiten

Lege in der App ein **Pendel**-Projekt mit **C-Auswertung** in einem neuen Ordner an.
Ersetze den gesamten Experimentcode durch das erste Beispiel und den gesamten
Analysecode durch das zweite. Physim pflegt die `physim.project`; der mitgelieferte
Builder übernimmt SDK-Pfade, Bibliothek und Modulnamen. Die gleichen Quellen liegen im SDK unter
`examples/documentation/main.c` und `examples/documentation/analysis.c`.

Du musst dafür nur C-Funktionen, Strukturen und Zeiger kennen. Die benötigten
API-Aufrufe werden hier erklärt; ihre vollständigen Signaturen stehen in der
[Experimentreferenz](reference/experiment.md) und [Berichtsreferenz](reference/report.md).

## Experimentcode

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
                                          destroy};
    return &api;
}
```

## Wie der Host dein Modell aufruft

`ps_get_experiment` liefert eine dauerhaft lebende Beschreibung des Moduls.
`struct_size` und `PS_ABI_VERSION` erlauben dem Host eine Kompatibilitätsprüfung.
Alle fünf Callbacks sind erforderlich. Der Zustand liegt in `context->user`,
nicht in einer globalen veränderlichen Variable.

`create` reserviert Zustand und registriert `position.x` einmal mit der Einheit
Meter. Der Kanalindex ist ein Integer; -1 bedeutet Fehler. Danach ruft das Beispiel
`reset` auf und setzt damit auch die Anfangsmessung bei t = 0.
`reset` stellt Position und Geschwindigkeit wieder her und registriert keine Kanäle.

`step` erhält den Zeitabstand in Sekunden. Beim Eintritt enthält `context->time_s`
noch die alte Zeit. Schreibe die Werte des neuen Zustands nach `context->values`;
erst danach schreibt der Host den Messpunkt zum neuen Zeitpunkt. Setze die
Hostzeit nicht selbst. Metadaten beschreiben Modell, Parameter und Verfahren.

`scene` liefert nur Geometriedaten. Die Szene ist bei jedem Aufruf leer und gehört
dem Host. Kugelradius und Position sind in Metern; Y zeigt nach oben. Die Farbe
ist `0xRRGGBBAA`. Die ID 1 hält Auswahl und Sichtbarkeit an derselben Kugel fest.
Der feste Einzelkörper passt sicher in die Szenenkapazität; deshalb ignoriert
dieses Minimalbeispiel den Rückgabewert des Szenenhelfers.

`destroy` gibt den modulseitigen Zustand frei, auch nach teilweise fehlgeschlagenem
`create`. Gib den Context und die Szene selbst nicht frei. Kanäle müssen während
eines Laufs stabil bleiben. Höchstens 16 Kanäle und 32 Szenenobjekte sind zulässig.

## Analysecode

```c
#include "physim/report.h"
#include <stdio.h>

static ps_result analyze(const char *input, const char *prefix) {
    ps_analysis_context *ctx = NULL;
    ps_report *report = NULL;
    ps_dataset run = {0};
    ps_series time = {0}, position = {0}, velocity = {0};
    ps_result r = ps_analysis_create(prefix, 0, &ctx);
    bool recovered = false;
    if (r == PS_OK) {
        r = ps_analysis_open_run(ctx, input, &run);
        recovered = r == PS_RECOVERED;
        if (recovered)
            r = PS_OK;
    }
    if (r == PS_OK)
        r = ps_dataset_series(ctx, run, "time", &time);
    if (r == PS_OK)
        r = ps_dataset_series(ctx, run, "position.x", &position);
    if (r == PS_OK)
        r = ps_series_derivative(ctx, position, time, &velocity);
    if (r == PS_OK)
        r = ps_report_create("Uniform motion", "Velocity from position derivative", &report);
    ps_plot_info info = {0};
    snprintf(info.title, sizeof info.title, "Velocity");
    snprintf(info.x_label, sizeof info.x_label, "Time");
    snprintf(info.y_label, sizeof info.y_label, "Velocity");
    ps_plot_handle plot = {0};
    if (r == PS_OK)
        r = ps_report_unit_from(PS_SECOND, &info.x_unit);
    if (r == PS_OK)
        r = ps_report_unit_from(PS_VELOCITY, &info.y_unit);
    if (r == PS_OK)
        r = ps_report_add_plot(report, &info, &plot);
    if (r == PS_OK)
        r = ps_report_add_series(report, plot, ctx, time, velocity, "dx/dt", PS_PLOT_LINE);
    char path[4096];
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s.psreport", prefix);
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT : ps_report_save(report, path);
    }
    if (r == PS_OK) {
        int n = snprintf(path, sizeof path, "%s-velocity.csv", prefix);
        ps_series columns[] = {time, position, velocity};
        r = n < 0 || (size_t)n >= sizeof path ? PS_LIMIT
                                              : ps_series_export_csv(ctx, columns, 3, path);
    }
    ps_report_destroy(report);
    ps_analysis_destroy(ctx);
    return r == PS_OK && recovered ? PS_RECOVERED : r;
}
PS_EXPORT const ps_analysis_api *ps_get_analysis(void) {
    static const ps_analysis_api api = {sizeof(ps_analysis_api), PS_ABI_VERSION,
                                        "Uniform motion analysis", analyze, NULL};
    return &api;
}
```

## Was die Auswertung tut

Der Runner übergibt `input` als Laufpfad und `prefix` als Ausgabepräfix. Eine Analyse
besitzt einen eigenen Prozess und darf keine Zeiger aus der Simulation verwenden.
`ps_analysis_create` legt einen Context mit temporären Arbeitsdateien an; Quote 0
wählt den Standard von 1 GiB. Der Zielordner muss schon existieren.

`ps_analysis_open_run` erzeugt einen Datensnapshot. `PS_RECOVERED` erlaubt eine
Auswertung des lesbaren Teils; wir merken uns das und geben den Status am Ende weiter.
`ps_dataset_series` holt die Zeit und Position. `ps_series_derivative` bildet dx/dt;
die Einheit wird automatisch m/s. Das Diagramm erhält passend Sekunden und m/s.

`ps_report_add_series` übernimmt die Kurve, `ps_report_save` schreibt die von der
App erwartete Datei mit Endung `.psreport`. Der CSV-Export enthält zusätzlich die
vollständigen Spalten Zeit, Position und Geschwindigkeit. Jeder fehlbare Schritt
wird geprüft. Context und Bericht werden auch nach einem Fehler zerstört.

## Bauen, starten und Ergebnis prüfen

1. Drücke F5 und warte auf einen erfolgreichen Build.
2. Stelle vor dem Start den Zeitschritt auf 0,01 s. Starte die Simulation und stoppe nach mindestens zwei Simulationssekunden.
3. In den Messdaten muss position.x linear steigen. Bei t = 2 s ist x näherungsweise 3 m.
4. Starte die Analyse. Im Analyseergebnis muss die Geschwindigkeit eine horizontale Linie bei 1,5 m/s sein, bis auf Rundungsfehler.
5. Öffne die erzeugte `-velocity.csv`. Auch die Randwerte der Ableitung sollten bei diesem linearen Modell 1,5 m/s sein.

Die Zahl der gerenderten Bilder ist unabhängig von der Zahl der Physikschritte.
Zum exakten Wiederholen außerhalb der App kann der Runner eine Schrittzahl vorgeben:

```powershell
# Vom Projektordner aus; SDK-Pfad und Modulpfade an die Installation anpassen.
& D:\Physim\build\bin\physim-runner.exe .\build\bin\experiment.dll .\runs\motion.psrun --steps 200 --dt 0.01 --seed 42
& D:\Physim\build\bin\physim-analysis-runner.exe .\build\bin\analysis.dll .\runs\motion.psrun .\runs\motion-analysis
```

Die Moduldateien können je nach Generator und Projektkonfiguration in einem
anderen Build-Unterordner liegen; die tatsächlichen Pfade stehen im Buildprotokoll.
Verwende für eine Wiederholung neue Ausgabenamen, da vorhandene Dateien geschützt sind.

## Das Modell erweitern

Ändere zuerst nur die Geschwindigkeit und prüfe die neue Steigung. Füge danach
eine konstante Beschleunigung hinzu: `x += v*dt + 0.5*a*dt*dt`, anschließend
`v += a*dt`. Registriere dafür einen zusätzlichen Geschwindigkeitskanal.
Für variable Kräfte verwende einen geeigneten Integrator und vergleiche einen Lauf
mit halbiertem Zeitschritt. Für Kräfte und Kontakte stehen eigene Anleitungen bereit.

[Integratoren und Toleranzen](numerics.md)

[Kräfte, Kontakte, Gelenke und Medien](mechanics.md)

[Sensoren hinzufügen](measurement.md)

[Weitere Reihenoperationen](series.md)

[Diagramme und Tabellen erweitern](reports.md)
