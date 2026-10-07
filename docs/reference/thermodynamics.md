# C-Referenz: Thermodynamik: Gasmodelle und Wärmefluss

SI-Werte, Kelvin, konstante Wärmekapazitäten und explizite lineare Leitwerte. Ideale und homogene Van-der-Waals-Zustandsgrößen ohne Phasenauswahl, Energie und Entropiedifferenzen sowie exakte Reservoir-/Zweikörperrelaxation. Keine Allokation, kein impliziter Integrator und keine Stofftabellen; Fehler erhalten Ausgaben.

[Anleitung und Beispiele](../thermodynamics.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/thermodynamics.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_MOLAR_GAS_CONSTANT 8.31446261815324
```

## Typen und Funktionen

## ps_ideal_gas_pressure

Berechnet p=nRT/V in Pa für positive SI-Zustandsgrößen eines idealen Gases.

```c
ps_result ps_ideal_gas_pressure(
    double amount_mol,
    double temperature_k,
    double volume_m3,
    double *pressure_pa);
```

Double representation of the exact SI molar gas constant, J/(mol K). No hidden state or allocations. Every argument must be finite. Absolute temperatures, amounts, volumes, input pressures and heat capacities are strictly positive. Conductance and dt may be zero. Every error preserves caller output; NULL output is PS_INVALID. PS_NUMERIC means a required positive result rounds to zero, or a result is not finite. Signed heat/power/entropy and vdW results may round to zero.

Ideal dilute gas only: p V = n R T. No phase transitions or real-gas terms.

## ps_ideal_gas_volume

Berechnet V=nRT/p in m³ für ein ideales Gas.

```c
ps_result ps_ideal_gas_volume(
    double amount_mol,
    double temperature_k,
    double pressure_pa,
    double *volume_m3);
```

## ps_ideal_gas_temperature

Berechnet T=pV/(nR) in Kelvin für ein ideales Gas.

```c
ps_result ps_ideal_gas_temperature(
    double amount_mol,
    double pressure_pa,
    double volume_m3,
    double *temperature_k);
```

## ps_ideal_gas_energy

Berechnet U=n cv T in J bei konstantem molarem cv und Referenz U=0 bei T=0.

```c
ps_result ps_ideal_gas_energy(
    double amount_mol,
    double molar_cv_j_mol_k,
    double temperature_k,
    double *energy_j);
```

Constant molar cv, with U=0 at T=0 as the chosen reference: U=n cv T.

## ps_ideal_gas_entropy_change

Berechnet die reversible Entropiedifferenz zwischen zwei Gleichgewichtszuständen desselben idealen Gases bei konstantem cv.

```c
ps_result ps_ideal_gas_entropy_change(
    double amount_mol,
    double molar_cv_j_mol_k,
    double initial_temperature_k,
    double initial_volume_m3,
    double final_temperature_k,
    double final_volume_m3,
    double *entropy_j_k);
```

Same amount and constant cv in both equilibrium states. Reversible state difference dS = n [cv ln(T1/T0) + R ln(V1/V0)]. Not an entropy-production estimate or a path heat integral. States need not have equal pressure.

## ps_vdw_gas_pressure

Wertet p=nRT/(V-nb)-an²/V² in Pa für konstante molare SI-Koeffizienten aus; V>nb. Keine Phasenauswahl.

```c
ps_result ps_vdw_gas_pressure(
    double amount_mol,double temperature_k,double volume_m3,
    double attraction_pa_m6_mol2,double covolume_m3_mol,double *pressure_pa);
```

Homogeneous classical van der Waals model, constant a,b and molar cv. a in Pa m^6/mol^2, b in m^3/mol, n,T,V>0, a,b>=0, V>n b. Available volume is evaluated with fma(-n,b,V). These functions evaluate the algebraic model only: negative pressure, energy or dp/dV are valid results. They do not choose stable/metastable branches, coexistence, latent heat or Maxwell constructions. dp/dV<0 is necessary for mechanical stability, not a complete equilibrium criterion. No gas-specific calibration is implicit. PS_INVALID for invalid/domain inputs; PS_NUMERIC for nonfinite results; signed underflow may round to zero. All errors preserve output.

## ps_vdw_gas_pressure_derivative

Wertet die Druckableitung bei festem n,T in Pa/m³ aus; positive Werte sind mechanisch instabile homogene Zustände.

```c
ps_result ps_vdw_gas_pressure_derivative(
    double amount_mol,double temperature_k,double volume_m3,
    double attraction_pa_m6_mol2,double covolume_m3_mol,
    double *derivative_pa_m3);
```

## ps_vdw_gas_energy

Wertet U=n cv T-an²/V in J bei konstantem a,cv aus; Referenz T→0,V→∞ und signierte Ergebnisse.

```c
ps_result ps_vdw_gas_energy(
    double amount_mol,double molar_cv_j_mol_k,double temperature_k,
    double volume_m3,double attraction_pa_m6_mol2,double *energy_j);
```

U=n cv T-a n^2/V, reference approaches zero at T=0 and V->infinity.

## ps_vdw_gas_entropy_change

Berechnet die Entropiedifferenz mit freiem Volumen V-nb bei konstantem cv,b; keine Entropieproduktion oder Phasenkoexistenz.

```c
ps_result ps_vdw_gas_entropy_change(
    double amount_mol,double molar_cv_j_mol_k,
    double initial_temperature_k,double initial_volume_m3,
    double final_temperature_k,double final_volume_m3,
    double covolume_m3_mol,double *entropy_j_k);
```

Same n and constant cv,b: n[cv ln(T1/T0)+R ln((V1-nb)/(V0-nb))]. Independent of constant a; not entropy production or a path heat integral.

## ps_heat_capacity

Berechnet C=m c in J/K bei konstanter spezifischer Wärmekapazität.

```c
ps_result ps_heat_capacity(
    double mass_kg,
    double specific_heat_j_kg_k,
    double *capacity_j_k);
```

Constant specific heat, no latent heat: C=m c, Q=C (T1-T0).

## ps_sensible_heat

Berechnet Q=C(T1-T0) in J, positiv bei Erwärmung.

```c
ps_result ps_sensible_heat(
    double capacity_j_k,
    double initial_temperature_k,
    double final_temperature_k,
    double *heat_j);
```

## ps_heat_flow

Berechnet P=G(Ta-Tb) in W, positiv von A nach B.

```c
ps_result ps_heat_flow(
    double conductance_w_k,
    double temperature_a_k,
    double temperature_b_k,
    double *power_w);
```

Lumped linear conduction, positive power flows from A to B: P=G(Ta-Tb). For a homogeneous slab, choose G=k A/L explicitly from its model data.

## ps_thermal_reservoir_step

Berechnet die exakte Relaxation an ein konstantes Reservoir ohne Zeitschritt-Stabilitätsgrenze.

```c
ps_result ps_thermal_reservoir_step(
    double capacity_j_k,
    double temperature_k,
    double reservoir_temperature_k,
    double conductance_w_k,
    double dt_s,
    double *next_temperature_k);
```

Exact relaxation to a prescribed constant-temperature reservoir: T(t+dt)=Tr+(T-Tr)exp(-G dt/C). No time-step stability restriction. Constant C,G; no radiation, phase change or spatial temperature field.

## ps_thermal_pair_step

Berechnet die exakte isolierte Zweikörperrelaxation mit konstanter Kapazität und Leitwert; beide Temperaturen stehen in Vec2.

```c
ps_result ps_thermal_pair_step(
    double capacity_a_j_k,
    double temperature_a_k,
    double capacity_b_j_k,
    double temperature_b_k,
    double conductance_w_k,
    double dt_s,
    ps_vec2 *out);
```

Exact isolated two-body exchange, constant capacities and conductance. out.x/out.y are final A/B temperatures; energy C_a T_a+C_b T_b is conserved to floating-point rounding. Both approach their capacity-weighted equilibrium monotonically, with rate G(1/C_a+1/C_b). Inputs can be out's own components.
