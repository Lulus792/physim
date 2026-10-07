# C-Referenz: Numerische Verfahren

Wähle Integrator, Zustand und Ableitung passend zum Modell. Numerische Callbacks müssen alle Komponenten setzen und frei von sichtbaren Nebenwirkungen sein. Adaptive Zwischenstufen sind keine Messzeitpunkte. Prüfe Rückgabewert und gegebenenfalls Diagnose vor Verwendung des Ergebnisses.

[Anleitung und Beispiele](../numerics.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/numerics.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_NUMERIC_MAX_DIMENSION 32u
```

## Typen und Funktionen

## ps_linear_solve

Löst A*x=b mit skalierter Pivotisierung; A ist zeilenweise gespeichert, Eingaben bleiben erhalten.

```c
ps_result ps_linear_solve(
    const double *a,
    const double *b,
    size_t n,
    double pivot_tolerance,
    double *x);
```

Row-major A, scaled partial pivoting. Inputs are preserved except where x aliases them on success; x may alias A or b. 1 <= n <= 32; pivot_tolerance=0 selects n*DBL_EPSILON. Finite inputs and tolerance in [0,1) required. Each RHS/solution component has an independent internal exponent; weighted sums avoid premature range failure. Coefficients and precision remain Double; no condition estimate or exact-rounding bound. Final nonfinite solutions yield PS_NUMERIC. Output unchanged on every error.

### ps_scalar_fn

```c
typedef double (*ps_scalar_fn)(double x, void *user);
```

### ps_scalar_report

```c
typedef struct {
    double x, value, lower, upper;
    unsigned iterations, evaluations;
} ps_scalar_report;
```

## ps_root_bisect

Sucht eine Nullstelle einer stetigen Funktion in einem Intervall mit Vorzeichenwechsel.

```c
ps_result ps_root_bisect(
    ps_scalar_fn fn,
    void *user,
    double lower,
    double upper,
    double absolute_x,
    double relative_x,
    unsigned max_iterations,
    ps_scalar_report *report);
```

Continuous function, opposite signs at ordered endpoints (or an endpoint root). Terminates on bracket half-width <= absolute_x + relative_x*abs(x). Binary input values are compared exactly for this stopping condition. Report is also returned on PS_LIMIT (iteration budget or representable-point exhaustion); no guarantee for discontinuous functions. Nonfinite callbacks yield PS_NUMERIC. Output unchanged on PS_INVALID/PS_NUMERIC. Finite lo < hi, absolute_x > 0, 0 <= relative_x < 1, max_iterations > 0 required. Callback zeros end bisection immediately.

## ps_minimize_golden

Sucht ein Minimum einer unimodalen Funktion im vorgegebenen Intervall.

```c
ps_result ps_minimize_golden(
    ps_scalar_fn fn,
    void *user,
    double lower,
    double upper,
    double absolute_x,
    double relative_x,
    unsigned max_iterations,
    ps_scalar_report *report);
```

Unimodal function on [lower,upper]; golden-section minimization, same argument, x-tolerance and error/report contracts. Does not claim a derivative, global minimum on a multimodal interval, or an error bound for callback values.

### ps_acceleration_fn

```c
typedef void (*ps_acceleration_fn)(double time, const double *position, double *acceleration,
                                   void *user);
```

## ps_verlet_step

Integriert Position und Geschwindigkeit mit Velocity Verlet für eine orts-/zeitabhängige Beschleunigung.

```c
ps_result ps_verlet_step(
    ps_acceleration_fn fn,
    void *user,
    double time,
    double dt,
    double *position,
    double *velocity,
    size_t n);
```

Velocity Verlet for q''=a(t,q), no velocity-dependent acceleration. Arrays must be distinct, n<=32. Scaled weighted updates retain small accelerations. Position/velocity unchanged if any evaluation or final state is invalid.

### ps_ode_options

```c
typedef struct {
    double absolute_tolerance, relative_tolerance;
    double initial_step, minimum_step, maximum_step;
    unsigned maximum_steps;
    const double *component_absolute_tolerance;
} ps_ode_options;
```

accepted + rejected trial steps

optional n positive tolerances

### ps_ode_report

```c
typedef struct {
    unsigned accepted_steps, rejected_steps, evaluations;
    double reached_time, next_step, error_norm;
} ps_ode_report;
```

## ps_ode_options_default

Liefert die Standardtoleranzen und Schrittgrenzen für adaptive Integration.

```c
ps_ode_options ps_ode_options_default(void);
```

## ps_ode_integrate

Integriert ein nichtsteifes ODE-System mit Dormand–Prince 5(4) bis zur Zielzeit.

```c
ps_result ps_ode_integrate(
    ps_ode_fn fn,
    void *user,
    double start,
    double end,
    double *state,
    size_t n,
    const ps_ode_options *options,
    ps_ode_report *report);
```

Dormand-Prince 5(4), explicit non-stiff ODEs. Integrates forward or backward to the requested endpoint, n<=32. Infinity norm of component-wise scaled error. Scaled weighted stage/error sums avoid premature range loss. Every stage must still be representable; tolerance scales remain finite Double values. Local error control is not a global-error bound. State changes only on PS_OK. Callbacks must be deterministic and must not change externally visible state: rejected stages and trial evaluations are normal. No events/dense output yet.

### ps_ode_diagnostic_reason

```c
typedef enum {
    PS_ODE_DIAG_NONE,
    PS_ODE_DIAG_ARGUMENT,
    PS_ODE_DIAG_INITIAL_STATE,
    PS_ODE_DIAG_COMPONENT_TOLERANCE,
    PS_ODE_DIAG_STEP_BUDGET,
    PS_ODE_DIAG_TIME_RESOLUTION,
    PS_ODE_DIAG_STAGE_STATE,
    PS_ODE_DIAG_DERIVATIVE,
    PS_ODE_DIAG_ERROR_ESTIMATE,
    PS_ODE_DIAG_MINIMUM_STEP
} ps_ode_diagnostic_reason;
```

### ps_ode_diagnostic

```c
typedef struct {
    ps_ode_diagnostic_reason reason;
    double time;
    size_t component;
    unsigned stage;
} ps_ode_diagnostic;
```

Failure evaluation time; reached time for limit/success.

Zero-based, SIZE_MAX when not applicable.

Zero-based Dormand-Prince stage, UINT_MAX otherwise.

## ps_ode_integrate_diagnosed

Wie ode_integrate, ergänzt um die konkrete Abbruchursache, Komponente und Stufe.

```c
ps_result ps_ode_integrate_diagnosed(
    ps_ode_fn fn,
    void *user,
    double start,
    double end,
    double *state,
    size_t n,
    const ps_ode_options *options,
    ps_ode_report *report,
    ps_ode_diagnostic *diagnostic);
```

Same integration and transactional state contract. Optional caller-owned diagnostic is written on EVERY return, including invalid arguments; report remains untouched on invalid arguments. No allocation or global error state. Diagnostic storage, report and state must not overlap. NONE denotes success. For invalid arguments, time is start (possibly nonfinite).

## ps_ode_step_diagnosed

Akzeptiert genau einen Dormand–Prince-Schritt in Richtung end; verworfene Versuche ändern den Zustand nicht. Bericht und Diagnose enthalten tatsächliche Zielzeit, nächste Schrittweite und Fehlernorm.

```c
ps_result ps_ode_step_diagnosed(
    ps_ode_fn fn,
    void *user,
    double start,
    double end,
    double *state,
    size_t n,
    const ps_ode_options *options,
    ps_ode_report *report,
    ps_ode_diagnostic *diagnostic);
```

Same transactional error control, but returns after the first accepted step toward end. report->reached_time is the actual endpoint; next_step is the suggested signed duration. Rejected trials consume maximum_steps. A nonzero interval is required; final endpoint clipping follows the integrate contract.

## ps_ode_diagnostic_string

Liefert die statische Textbeschreibung einer ODE-Diagnose.

```c
const char *ps_ode_diagnostic_string(ps_ode_diagnostic_reason reason);
```

Static English description; unknown reason values return "Unknown ODE diagnostic".
