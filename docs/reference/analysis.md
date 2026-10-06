# C-Referenz: Statistik und Analysemodule

Initialisiere ps_statistics mit null und füge endliche Werte mit ps_statistics_push hinzu. ps_statistics_stddev liefert die Stichprobenstreuung. ps_derivative berechnet Sekanten, ps_trapezoid ein bestimmtes Integral. ps_analyze_run erstellt eine Standardauswertung. Eigene Module exportieren ps_get_analysis; das vollständige Beispiel steht im Experimenttutorial.

[Anleitung und Beispiele](../series.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/analysis.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_ANALYSIS_API_BASE_SIZE offsetof(ps_analysis_api, run_many)
#define PS_ANALYSIS_API_MANY_SIZE offsetof(ps_analysis_api, run_diagnostic)
#define PS_ANALYSIS_MAX_INPUTS 8u
```

## Typen und Funktionen

### ps_statistics

```c
typedef struct {
    uint64_t count;
    double mean, m2, min, max;
} ps_statistics;
```

## ps_statistics_push

Fügt einer mit null initialisierten Statistik einen Wert hinzu; Mittelwert und Streuung werden online aktualisiert.

```c
void ps_statistics_push(ps_statistics *stats, double value);
```

## ps_statistics_stddev

Liefert die Stichprobenstandardabweichung der gesammelten Werte; für eine Aussage sind mindestens zwei Werte nötig.

```c
double ps_statistics_stddev(const ps_statistics *stats);
```

## ps_derivative

Schreibt Sekantenableitungen dy/dx in out, mit einseitigen Rändern.

```c
ps_result ps_derivative(const double *x, const double *y, size_t n, double *out);
```

## ps_trapezoid

Berechnet das bestimmte Integral der Werte y über x mit der Trapezregel.

```c
double ps_trapezoid(const double *x, const double *y, size_t n);
```

### ps_analysis_api

```c
typedef struct {
    uint32_t struct_size, abi_version;
    const char *name;
    ps_result (*run)(const char *input_run, const char *output_prefix);
    ps_result (*run_many)(const char *const *input_runs, size_t count, const char *output_prefix);
    ps_result (*run_diagnostic)(const char *const *input_runs, size_t count,
                                const char *output_prefix, ps_diagnostic *diagnostic);
} ps_analysis_api;
```

Optional tail extension of ABI 2. The runner checks struct_size before reading it. Receives 0..8 explicit paths, in selection order, no resampling. Zero inputs are for self-generated analyses; modules may reject them. Input strings are borrowed for the duration of this synchronous call.

Optional ABI-3 tail. Explicit diagnostic output owned by the caller. Receives 0..8 inputs; return result and write a matching error record if available. Clear the record on success. Inputs/prefix borrowed synchronously.

### ps_analysis_entry

```c
typedef const ps_analysis_api *(*ps_analysis_entry)(void);
```

## ps_analyze_run

Schreibt Standardstatistik, Vorschau und Analysemanifest für einen gespeicherten Lauf.

```c
ps_result ps_analyze_run(const char *input_run, const char *output_prefix);
```

Streaming statistics, bounded preview SVG and CSV table. Prefix is a filename prefix. Energy deviation uses energy.balance when present, otherwise energy; the manifest records energy_metric_channel. Sources are expected to use SI. Associated measurement .status channels mask statistics (only status=1). Empty statistics and the standard deviation for n<2 are blank in CSV.
