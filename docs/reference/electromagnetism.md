# C-Referenz: Elektromagnetismus und RC-Schaltungen

Reine SI-Funktionen für homogene Punktladungsfelder, Potential, Lorentzkraft, Widerstände, Kondensatorenergie und exakte RC-Schritte. Modelle liefern Permittivität und Feldwerte ausdrücklich; Singularität und Fehler bewahren Ausgaben. Kein Maxwell- oder beliebiger Netzwerk-Solver.

[Anleitung und Beispiele](../electromagnetism.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/electromagnetism.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_VACUUM_PERMITTIVITY 8.8541878188e-12
```

## Typen und Funktionen

## ps_point_charge_field

Berechnet das homogene Coulombfeld in V/m; der Quellpunkt ist singulär.

```c
ps_result ps_point_charge_field(
    double charge_c,
    ps_vec3 source_m,
    ps_vec3 point_m,
    double permittivity_f_m,
    ps_vec3 *field_v_m);
```

CODATA 2022 vacuum permittivity, F/m; measured, not an exact SI constant. Standard uncertainty: 1.4e-21 F/m. Models supply permittivity explicitly.

Pure SI operations, no allocation or hidden state. Finite arguments only; permittivity, resistance and capacitance are strictly positive; dt>=0. PS_INVALID: bad input/NULL output; PS_NUMERIC: nonfinite result (or a required positive resistance rounds to zero). Errors preserve output. Signed field, potential, force, current and voltage may round to zero. These are electrostatic/lumped models, not a Maxwell field solver.

Homogeneous isotropic infinite medium. E=q(r-r0)/(4*pi*epsilon*|r-r0|^3) in V/m; potential=q/(4*pi*epsilon*|r-r0|) in V, with zero at infinity. Coincident positions return PS_SINGULAR, including q=0; no softening, interfaces, boundaries, retardation, screening or implicit superposition.

## ps_point_charge_potential

Berechnet das Punktladungspotential in V mit Nullpunkt im Unendlichen.

```c
ps_result ps_point_charge_potential(
    double charge_c,
    ps_vec3 source_m,
    ps_vec3 point_m,
    double permittivity_f_m,
    double *potential_v);
```

## ps_lorentz_force

Berechnet q(E+v×B) in N für ausdrücklich übergebene SI-Felder.

```c
ps_result ps_lorentz_force(
    double charge_c,
    ps_vec3 electric_v_m,
    ps_vec3 velocity_m_s,
    ps_vec3 magnetic_t,
    ps_vec3 *force_n);
```

F=q(E+v cross B), nonrelativistic test charge, supplied E in V/m and B in T. No field generation, radiation reaction or automatic trajectory integration.

## ps_resistor_current

Berechnet I=V/R in A.

```c
ps_result ps_resistor_current(
    double voltage_v,
    double resistance_ohm,
    double *current_a);
```

Linear ideal resistor: I=V/R, V=IR, dissipated power=V^2/R.

## ps_resistor_voltage

Berechnet V=IR in V.

```c
ps_result ps_resistor_voltage(
    double current_a,
    double resistance_ohm,
    double *voltage_v);
```

## ps_resistor_power

Berechnet die nichtnegative Verlustleistung V²/R in W.

```c
ps_result ps_resistor_power(double voltage_v, double resistance_ohm, double *power_w);
```

## ps_resistance_series

Addiert zwei positive Widerstände.

```c
ps_result ps_resistance_series(double first_ohm, double second_ohm, double *total_ohm);
```

Two strictly positive resistors; no arbitrary circuit graph solver.

## ps_resistance_parallel

Berechnet den Gesamtwiderstand zweier positiver Parallelwiderstände.

```c
ps_result ps_resistance_parallel(
    double first_ohm,
    double second_ohm,
    double *total_ohm);
```

## ps_capacitor_energy

Berechnet die ideale Kondensatorenergie 0,5 C V² in J.

```c
ps_result ps_capacitor_energy(double capacitance_f, double voltage_v, double *energy_j);
```

Ideal capacitor stored energy=0.5*C*V^2, with C in farad.

## ps_rc_voltage_step

Berechnet einen exakten konstanten RC-Spannungsschritt ohne Zeitschritt-Stabilitätsgrenze.

```c
ps_result ps_rc_voltage_step(
    double resistance_ohm,
    double capacitance_f,
    double voltage_v,
    double source_voltage_v,
    double dt_s,
    double *next_voltage_v);
```

Exact constant-source series RC step: Vc'=Vs+(Vc-Vs)exp(-dt/(R*C)). Resistance/capacitance remain constant; no stability restriction on dt. dt=0 preserves Vc exactly. No inductance, parasitics or switching within dt.
