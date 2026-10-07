# C-Referenz: Archivierte Serien und Analyse-Hostdienste

Ein expliziter Analysehost stellt versionierte, synchrone Batch-Dienste bereit. Requests beschreiben getrennte Runner mit SI-Parametern, Seeds, Zeit- und Speichergrenzen. Ergebnisse enthalten validierte Teilfortschritte und Messstatus. Der Core startet keine Prozesse; der Analyse-Runner liefert die Dienste über den optionalen run_host-Tail. Geliehene Dienstzeiger leben nur während dieses Aufrufs.

[Anleitung und Beispiele](../batch-language.md) · [Lernpfade](../guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/batch.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_BATCH_MAX_RUNS 1000u
#define PS_BATCH_MAX_SAMPLES 5000000u
#define PS_BATCH_MAX_WORKERS 8u
#define PS_ANALYSIS_SERVICES_VERSION 1u
```

## Typen und Funktionen

### ps_batch_options

```c
typedef struct {
    char runner[4096], module[4096], source[4096], directory[4096], channel[48];
    char resume_from[4096];
    const char *source_text;
    size_t source_size;
    uint64_t seed;
    uint64_t memory_bytes;
    uint32_t runs, steps, workers;
    double dt, timeout_s;
    bool adaptive;
    double end_time, minimum_dt, maximum_dt;
    bool sweep;
    char sweep_name[48];
    double sweep_start, sweep_end;
    uint32_t parameter_count;
    struct {
        char name[48];
        double value;
    } parameters[PS_MAX_PARAMETERS];
} ps_batch_options;
```

Batch host request. All paths are absolute; output must be a new directory. Each worker has its own process and working directory. The host supplies runner.

Optional existing series; its files stay read-only.

Optional immutable UTF-8 source captured by the UI before the worker starts. Borrowed for the duration of ps_batch_run; source remains its original path.

Per runner; zero disables. Platform semantics: platform.h.

1..PS_BATCH_MAX_WORKERS; capped by runs.

end_time=0 retains the fixed sample-count workflow. A positive target clips the final interval; steps becomes an accepted-step budget. Adaptive series require a common target, not equal step counts.

Optional linear parameter study. Index 0 uses start, the final index end.

Fixed overrides applied to every run, excluding the sweep parameter.

### ps_batch_result

```c
typedef struct {
    uint32_t completed, started, active, peak_active, reused, valid;
    bool cancelled;
    char error[256];
    ps_channel channel;
    ps_parameter_unit sweep_unit;
    double values[PS_BATCH_MAX_RUNS];
    bool finished[PS_BATCH_MAX_RUNS];
    uint8_t endpoint_status[PS_BATCH_MAX_RUNS];
} ps_batch_result;
```

Journaled successful run, even if its endpoint is missing.

0=not due, 1=valid, 2=dropped. values[i] valid only at 1.

### ps_analysis_services

```c
typedef struct {
    uint32_t struct_size, version;
    void *user;
    ps_result (*run_batch)(void *user,const ps_batch_options *request,uint32_t stop_after,ps_batch_result *result);
    ps_result (*resume_batch)(void *user,const char *series,const char *directory,ps_batch_options *options);
} ps_analysis_services;
```

Borrowed synchronous services, explicitly supplied by an analysis host. Calls preserve input requests. Results may contain validated partial progress on failure; archived files remain on disk. No service is stored globally.

stop_after=0 runs the whole series; 1..runs-1 creates a resumable pause after that many validated completions (including reused runs).
