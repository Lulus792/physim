# C-Referenz: Laminare Rohre, Netze und Tracertransport

Lehrmodelle für inkompressible Newtonsche Rohre, passive stationäre Drucknetze und konservativen periodischen 1D-Tracer. Explizite Stoffdaten, Größen- und Stabilitätsgrenzen; keine turbulente oder mehrdimensionale CFD.

[Anleitung und Beispiele](../fluid.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/fluid.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_PIPE_NETWORK_MAX_NODES 16u
#define PS_PIPE_NETWORK_MAX_EDGES 32u
#define PS_TRANSPORT_MAX_CELLS 4096u
```

## Typen und Funktionen

## ps_pipe_conductance

Berechnet den Hagen-Poiseuille-Leitwert eines idealen laminaren Rundrohrs.

```c
ps_result ps_pipe_conductance(
    double radius_m,
    double length_m,
    double viscosity_pa_s,
    double *conductance_m3_s_pa);
```

Educational incompressible/lumped models, all SI. Finite inputs only, no heap allocation or hidden state. Errors preserve every output. PS_INVALID: bad input/NULL output; PS_NUMERIC: range/scaling failure; PS_SINGULAR: unanchored or numerically unresolved network; PS_LIMIT: size. Signed pressures/flows and concentration contributions may round to zero.

Fully developed laminar Newtonian flow in a rigid straight circular pipe: G=pi*r^4/(8*mu*L), in m^3/(s Pa). r,L,mu strictly positive. No entrance, turbulence, compressibility, bends, wall deformation or automatic Re check.

## ps_pipe_flow

Berechnet den signierten Volumenstrom G(pa-pb).

```c
ps_result ps_pipe_flow(
    double conductance_m3_s_pa,
    double pressure_a_pa,
    double pressure_b_pa,
    double *flow_m3_s);
```

Q=G*(pa-pb), G>=0; positive from A to B. Pressure can be signed gauge pressure.

## ps_pipe_power

Berechnet die nichtnegative hydraulische Verlustleistung.

```c
ps_result ps_pipe_power(
    double conductance_m3_s_pa,
    double pressure_a_pa,
    double pressure_b_pa,
    double *power_w);
```

Dissipated hydraulic power G*(pa-pb)^2 >=0, in W.

## ps_reynolds_number

Berechnet die dimensionslose Reynolds-Zahl aus expliziten SI-Stoffdaten.

```c
ps_result ps_reynolds_number(
    double density_kg_m3,
    double velocity_m_s,
    double diameter_m,
    double viscosity_pa_s,
    double *out);
```

Re=rho*abs(v)*diameter/mu, rho,diameter,mu>0. Model validity is caller policy.

## ps_hydrostatic_pressure

Berechnet den hydrostatischen Relativdruck mit signierter Tiefe.

```c
ps_result ps_hydrostatic_pressure(
    double reference_pa,
    double density_kg_m3,
    double gravity_m_s2,
    double depth_m,
    double *pressure_pa);
```

Gauge pressure p=p0+rho*g*depth, density>0, g>=0, signed depth downward in m.

### ps_pipe_edge

```c
typedef struct { uint32_t a,b; double conductance_m3_s_pa; } ps_pipe_edge;
```

## ps_pipe_network_solve

Löst ein verankertes passives lineares Drucknetz atomar und liefert Kantenflüsse.

```c
ps_result ps_pipe_network_solve(
    const uint8_t *fixed,
    const double *fixed_pressures_pa,
    size_t nodes,
    const ps_pipe_edge *edges,
    size_t edge_count,
    double *pressures_pa,
    double *flows_m3_s);
```

Steady passive linear network: each free node has sum(outgoing Q)=0. 1..16 nodes, 0..32 edges, finite G>=0, no self edges; parallel edges allowed. fixed[i] is 0/1. Only fixed pressures are read; free placeholders are ignored. Every component connected by G>0 needs a fixed node. G=0 closes an edge. Positive conductances that disappear under global scaling return PS_NUMERIC. Pressure scaling is per connected component; loss of a nonzero fixed pressure under that component scaling also returns PS_NUMERIC; ill-conditioned pivots return PS_SINGULAR. No pumps, inertia or compressibility. Outputs: node pressures and A->B edge flows. They must be disjoint; either can alias input storage. NULL edges/flows allowed only for zero edge count.

## ps_transport_periodic_step

Berechnet einen atomaren konservativen Upwind-/Diffusionsschritt für einen periodischen nichtnegativen Tracer.

```c
ps_result ps_transport_periodic_step(
    const double *concentration_kg_m3,
    size_t count,
    double velocity_m_s,
    double diffusivity_m2_s,
    double dx_m,
    double dt_s,
    double *next_kg_m3);
```

Periodic 1D passive tracer, uniform dx, constant signed velocity and D>=0: first-order upwind advection + centered explicit diffusion. Concentration is nonnegative kg/m^3; 3..4096 cells, dx>0, dt>=0. Requires abs(v)*dt/dx + 2*D*dt/dx^2 <=1. Convex update preserves mass/max principle to floating-point rounding. dt=0 copies input. Bounded 32 KiB stack scratch permits output aliasing any input. No fluid momentum/pressure solve, reactions, nonuniform grid, forcing or open boundaries; not a CFD solver.
