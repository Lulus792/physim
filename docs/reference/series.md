# C-Referenz: Datenreihen

Erzeuge einen Analysecontext, öffne einen Datensatz und hole time sowie Messkanäle als Reihen. Transformationen erzeugen neue Handles. Wähle gemeinsam ausgerichtete Reihen; Länge allein garantiert keine Zuordnung. Nach ps_analysis_destroy sind alle zugehörigen Handles ungültig.

[Anleitung und Beispiele](../series.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/series.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_ANALYSIS_MAX_DATASETS 8u
#define PS_ANALYSIS_MAX_SERIES 128u
#define PS_SERIES_BLOCK_SIZE 256u
#define PS_SERIES_MAX_WINDOW 4096u
```

## Typen und Funktionen

### ps_analysis_context

```c
typedef struct ps_analysis_context ps_analysis_context;
```

### ps_dataset

```c
typedef struct {
    const ps_analysis_context *owner;
    uint32_t slot, generation;
} ps_dataset;
```

Handles belong to one live context; zero is invalid. Do not use after destroy. Closing/releasing and reusing a slot never resurrects its old handles.

### ps_series

```c
typedef struct {
    const ps_analysis_context *owner;
    uint32_t slot, generation;
} ps_series;
```

### ps_dataset_info

```c
typedef struct {
    uint64_t samples;
    uint32_t channel_count;
    bool recovered;
    char metadata[8192];
    ps_channel channels[PS_MAX_CHANNELS];
} ps_dataset_info;
```

### ps_series_info

```c
typedef struct {
    uint64_t count;
    int8_t dimension[7];
    double scale;
    char name[64], symbol[64];
} ps_series_info;
```

### ps_series_operator

```c
typedef enum {
    PS_SERIES_ADD,
    PS_SERIES_SUBTRACT,
    PS_SERIES_MULTIPLY,
    PS_SERIES_DIVIDE
} ps_series_operator;
```

## ps_analysis_create

Erzeugt einen Analysecontext mit Ausgabepräfix und temporärer Speicherquote.

```c
ps_result ps_analysis_create(
    const char *work_prefix,
    uint64_t scratch_byte_limit,
    ps_analysis_context **out);
```

work_prefix is an existing writable directory plus a filename prefix, NOT a directory to create. Scratch files are exclusive and deleted on close/process exit. Quota covers all open scratch file payloads; 0 selects 1 GiB. Contexts are independent, but each context requires external synchronization.

## ps_analysis_create_with_allocator

Erzeugt einen Analysecontext in einer eigenen Allokationsdomäne.

```c
ps_result ps_analysis_create_with_allocator(
    const char *work_prefix,
    uint64_t scratch_byte_limit,
    ps_allocator allocator,
    ps_analysis_context **out);
```

Custom allocator owns the context; C stdio and OS buffers remain outside its domain. Descriptor copied by value, callbacks/user must outlive the context.

## ps_analysis_destroy

Gibt Context und Arbeitsdateien frei; alle darin vergebenen Handles werden ungültig.

```c
void ps_analysis_destroy(ps_analysis_context *ctx);
```

## ps_analysis_scratch_bytes

Meldet die aktuell belegten Nutzdatenbytes der temporären Reihenablage.

```c
uint64_t ps_analysis_scratch_bytes(const ps_analysis_context *ctx);
```

## ps_analysis_open_run

Validiert einen Lauf und öffnet einen unveränderlichen Datensnapshot; PS_RECOVERED ist lesbarer Teilerfolg.

```c
ps_result ps_analysis_open_run(
    ps_analysis_context *ctx,
    const char *path,
    ps_dataset *out);
```

Validated immutable disk snapshot. PS_RECOVERED is success with a partial run; other errors leave the output handle unchanged. Non-increasing times fail.

## ps_dataset_describe

Kopiert Samplezahl, Kanalschema, Metadaten und Wiederherstellungsstatus.

```c
ps_result ps_dataset_describe(
    ps_analysis_context *ctx,
    ps_dataset dataset,
    ps_dataset_info *out);
```

## ps_dataset_close

Schließt den Datensatz und invalidiert alle ihm zugehörigen Reihen.

```c
ps_result ps_dataset_close(ps_analysis_context *ctx, ps_dataset dataset);
```

Closes the dataset and invalidates ALL its source and derived series.

## ps_dataset_series

Holt die Reihe eines benannten Kanals oder die Zeitachse time.

```c
ps_result ps_dataset_series(
    ps_analysis_context *ctx,
    ps_dataset dataset,
    const char *name,
    ps_series *out);
```

"time" is the time axis, otherwise an exact channel name. .psrun v1 values are SI; unit scales are 1. Duplicate/ambiguous channel names are rejected.

## ps_series_from_values

Kopiert endliche Werte in eine eigenständige Datenreihe ohne Eingabedatensatz; Einheit und Name werden geprüft.

```c
ps_result ps_series_from_values(
    ps_analysis_context *ctx,
    const double *values,
    size_t count,
    ps_unit unit,
    const char *name,
    ps_series *out);
```

Copy finite values into scratch storage. Independent roots have distinct alignment identities; aligned values share the anchor's dataset lifetime, sample range and alignment, and must have the same count. Both forms are transactional, quota-checked, and valid without an input dataset.

## ps_series_aligned_values

Kopiert endliche Werte in eine neue Reihe mit der Samplezuordnung einer vorhandenen Ankerreihe.

```c
ps_result ps_series_aligned_values(
    ps_analysis_context *ctx,
    ps_series anchor,
    const double *values,
    size_t count,
    ps_unit unit,
    const char *name,
    ps_series *out);
```

## ps_series_describe

Kopiert Reihenname, Anzahl und Einheit in den Ausgabedeskriptor.

```c
ps_result ps_series_describe(
    ps_analysis_context *ctx,
    ps_series series,
    ps_series_info *out);
```

## ps_series_release

Gibt dieses Reihenhandle frei; bereits abgeleitete Reihen bleiben erhalten.

```c
ps_result ps_series_release(ps_analysis_context *ctx, ps_series series);
```

## ps_series_aligned

Prüft, ob zwei Reihen dieselbe Samplezuordnung besitzen.

```c
ps_result ps_series_aligned(ps_analysis_context *ctx, ps_series left, ps_series right);
```

PS_OK only for live series on the same dataset, selection and sample range.

## ps_series_read

Kopiert einen Ausschnitt ab einem Sampleindex in den Puffer des Aufrufers.

```c
ps_result ps_series_read(
    ps_analysis_context *ctx,
    ps_series series,
    uint64_t index,
    double *values,
    size_t capacity,
    size_t *read_count);
```

Copies up to capacity values starting at index. PS_OK includes an empty read at count; index > count is invalid. Outputs are unspecified on I/O error.

## ps_series_mask

Erhält alle Zeilen und das Alignment; kombiniert die Eingabemasken mit einem expliziten dimensionlosen Selektorwert.

```c
ps_result ps_series_mask(
    ps_analysis_context *ctx,
    ps_series input,
    ps_series selector,
    double accepted,
    ps_series *out);
```

Explicit masks retain every row and the original alignment. Validity is the conjunction of the input/selector masks and selector==accepted. Invalid numeric values remain finite placeholders; use read_masked or validity to distinguish them. Source channels remain unmasked unless explicitly masked. Mask bytes count against the scratch quota. All operations are transactional.

## ps_series_read_masked

Liest numerische Werte samt Gültigkeit pro Zeile blockweise. Fehlende Werte sind nur mit ihrem Flag zu interpretieren.

```c
ps_result ps_series_read_masked(
    ps_analysis_context *ctx,
    ps_series input,
    uint64_t index,
    double *values,
    uint8_t *valid,
    size_t capacity,
    size_t *read_count);
```

## ps_series_is_masked

Prüft, ob die Reihe eine ausdrücklich gespeicherte Gültigkeitsmaske besitzt, auch wenn alle Zeilen gültig sind.

```c
ps_result ps_series_is_masked(ps_analysis_context *ctx, ps_series input, bool *out);
```

## ps_series_validity

Erzeugt eine ausgerichtete, unmaskierte Reihe exakter 0/1-Gültigkeitswerte.

```c
ps_result ps_series_validity(ps_analysis_context *ctx, ps_series input, ps_series *out);
```

An aligned, unmasked dimensionless series of exact 0/1 flags.

## ps_series_slice

Erzeugt eine neue Reihe für einen zusammenhängenden Samplebereich.

```c
ps_result ps_series_slice(
    ps_analysis_context *ctx,
    ps_series input,
    uint64_t first,
    uint64_t count,
    ps_series *out);
```

Derived series own scratch data and survive release of their input series. Operations preserve alignment, except slice/select/resample (see below). Matching slices within the same selection can still be paired. Binary operations and x/y operations require the same dataset and sample range; implicit cross-run resampling is deliberately forbidden.

## ps_series_select

Filtert mehrere Reihen gemeinsam nach einem exakten Selektorwert, etwa gültigem Sensorstatus 1.

```c
ps_result ps_series_select(
    ps_analysis_context *ctx,
    const ps_series *columns,
    size_t count,
    ps_series selector,
    double accepted,
    ps_series *out);
```

Keep rows whose dimensionless selector equals accepted exactly (expressed in selector's stored units, e.g. status=1). Select 1..32 aligned columns together, including their x axis. All columns and selector must be aligned. No implicit handling of .status channels: the caller chooses the selector explicitly. Outputs preserve units and row order, own their data, and share a NEW alignment identity. Separate calls are not aligned, even if they select identical rows. Empty selections succeed; downstream operations keep their normal minimum count requirements. Derivatives/integrals connect retained samples across gaps. Outputs and scratch usage are unchanged on failure. Input/output arrays may alias. Bounded block memory; selected data counts against the scratch quota.

## ps_series_affine

Skaliert eine Reihe mit dimensionslosem Faktor und addiert einen dimensionsgeprüften Offset.

```c
ps_result ps_series_affine(
    ps_analysis_context *ctx,
    ps_series input,
    double factor,
    ps_quantity offset,
    ps_series *out);
```

## ps_series_combine

Verknüpft gepaarte Reihen per Summe, Differenz, Produkt oder Quotient und prüft ihre Einheiten.

```c
ps_result ps_series_combine(
    ps_analysis_context *ctx,
    ps_series_operator op,
    ps_series left,
    ps_series right,
    ps_series *out);
```

## ps_series_derivative

Erzeugt dy/dx mit zentralen Sekanten und einseitigen Rändern.

```c
ps_result ps_series_derivative(
    ps_analysis_context *ctx,
    ps_series y,
    ps_series x,
    ps_series *out);
```

Central secants, one-sided segment endpoints; x must strictly increase. Missing neighbors are never bridged; isolated valid samples have no derivative. Scaled arithmetic preserves representable slopes even if endpoint differences overflow. PS_NUMERIC for unrepresentable slopes; output/quota unchanged.

## ps_series_integral

Erzeugt ein kumulatives Trapezintegral mit explizitem Anfangswert und Anfangseinheit.

```c
ps_result ps_series_integral(
    ps_analysis_context *ctx,
    ps_series y,
    ps_series x,
    ps_quantity initial,
    ps_series *out);
```

Cumulative trapezoidal integral, first value = initial. A missing x/y sample makes this and all later cumulative values unknown; no invented gap area. Scaled interval arithmetic avoids intermediate overflow and loss of subnormal means. PS_NUMERIC for unrepresentable areas/sums; output/quota unchanged.

## ps_series_moving_average

Erzeugt einen kausalen Mittelwert über bis zu window Samples.

```c
ps_result ps_series_moving_average(
    ps_analysis_context *ctx,
    ps_series input,
    size_t window,
    ps_series *out);
```

Causal moving average, window <= 4096. A missing sample resets the window; only consecutive valid observations contribute.

## ps_series_resample_linear

Interpoliert y(x) linear auf ein neues Zeit-/X-Raster ohne Extrapolation.

```c
ps_result ps_series_resample_linear(
    ps_analysis_context *ctx,
    ps_series y,
    ps_series x,
    ps_series target_x,
    ps_series *out);
```

Explicit linear interpolation of aligned (x,y) onto target_x, possibly from another dataset in this context. Both axes must be nonempty and strictly increasing, with compatible units. No extrapolation: all converted target values must lie in the closed source range (PS_INVALID otherwise). Output has y's unit and target_x's dataset/sample alignment. It survives closing the source dataset, but not the target dataset. Bounded block memory; all source x values are validated, including those beyond the last target. Both axes must have valid coordinates. Linear/PCHIP intervals require two valid y knots; nearest/previous propagate the chosen knot validity. PCHIP slopes use contiguous valid neighbors and treat gap edges as endpoints.

### ps_resample_method

```c
typedef enum { PS_RESAMPLE_LINEAR, PS_RESAMPLE_NEAREST, PS_RESAMPLE_PREVIOUS,
               PS_RESAMPLE_PCHIP } ps_resample_method;
```

## ps_series_resample

Resampling mit expliziter Methode: linear, nächster oder vorheriger Stützpunkt.

```c
ps_result ps_series_resample(
    ps_analysis_context *ctx,
    ps_series y,
    ps_series x,
    ps_series target_x,
    ps_resample_method method,
    ps_series *out);
```

Same axis, unit, lifetime and transactional contract as resample_linear. NEAREST selects the earlier sample at equal distances. PREVIOUS holds the last source value at or before each target. Exact source coordinates always return that source value. No extrapolation for any method.

PCHIP is a local, monotonicity-preserving cubic Hermite interpolant with continuous first derivatives. Flat spans and extrema have zero knot slopes; endpoints use limited one-sided slopes. Two source points reduce to linear interpolation; a single point is valid only at that exact coordinate. Bounded block memory, finite-input arithmetic without wider numeric types.

## ps_series_statistics

Berechnet Statistik über die vollständige Reihe, unabhängig von Plotvorschauen.

```c
ps_result ps_series_statistics(
    ps_analysis_context *ctx,
    ps_series input,
    ps_statistics *out);
```

## ps_series_quantile

Berechnet das Typ-7-Quantil einer nichtleeren Reihe für eine endliche Wahrscheinlichkeit in [0, 1]. Verwendet temporären Speicher des Analyseallocators.

```c
ps_result ps_series_quantile(
    ps_analysis_context *ctx,
    ps_series input,
    double probability,
    double *out);
```

Type-7 quantile for a nonempty series. probability must be finite in [0, 1]. Uses only valid rows; all-missing input is PS_INVALID. Reads the whole series into allocator-backed temporary memory; output is unchanged on error.

## ps_series_quantile_with_allocator

Berechnet dasselbe Quantil mit einem separat angegebenen Allocator für den temporären Sortierpuffer.

```c
ps_result ps_series_quantile_with_allocator(
    ps_analysis_context *ctx,
    ps_series input,
    double probability,
    ps_allocator allocator,
    double *out);
```

Variant for hosts with a separate temporary-memory budget.

## ps_series_export_csv

Exportiert alle Werte gemeinsam ausgerichteter Spalten als neue CSV-Datei.

```c
ps_result ps_series_export_csv(
    ps_analysis_context *ctx,
    const ps_series *columns,
    size_t count,
    const char *path);
```

Exports every aligned row without loading them in memory. Masked values are empty fields; export an explicit validity series for flags. Exclusive output. On I/O failure a partial output may remain. At most 32 columns.
