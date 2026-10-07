#ifndef PHYSIM_FLUID_H
#define PHYSIM_FLUID_H
#include "core.h"
/* Educational incompressible/lumped models, all SI. Finite inputs only,
 * no heap allocation or hidden state. Errors preserve every output.
 * PS_INVALID: bad input/NULL output; PS_NUMERIC: range/scaling failure;
 * PS_SINGULAR: unanchored or numerically unresolved network; PS_LIMIT: size.
 * Signed pressures/flows and concentration contributions may round to zero. */
/* Fully developed laminar Newtonian flow in a rigid straight circular pipe:
 * G=pi*r^4/(8*mu*L), in m^3/(s Pa). r,L,mu strictly positive. No entrance,
 * turbulence, compressibility, bends, wall deformation or automatic Re check. */
ps_result ps_pipe_conductance(double radius_m, double length_m, double viscosity_pa_s,
                               double *conductance_m3_s_pa);
/* Q=G*(pa-pb), G>=0; positive from A to B. Pressure can be signed gauge pressure. */
ps_result ps_pipe_flow(double conductance_m3_s_pa, double pressure_a_pa,
                        double pressure_b_pa, double *flow_m3_s);
/* Dissipated hydraulic power G*(pa-pb)^2 >=0, in W. */
ps_result ps_pipe_power(double conductance_m3_s_pa, double pressure_a_pa,
                         double pressure_b_pa, double *power_w);
/* Re=rho*abs(v)*diameter/mu, rho,diameter,mu>0. Model validity is caller policy. */
ps_result ps_reynolds_number(double density_kg_m3, double velocity_m_s,
                              double diameter_m, double viscosity_pa_s, double *out);
/* Gauge pressure p=p0+rho*g*depth, density>0, g>=0, signed depth downward in m. */
ps_result ps_hydrostatic_pressure(double reference_pa, double density_kg_m3,
                                  double gravity_m_s2, double depth_m, double *pressure_pa);
#define PS_PIPE_NETWORK_MAX_NODES 16u
#define PS_PIPE_NETWORK_MAX_EDGES 32u
typedef struct { uint32_t a,b; double conductance_m3_s_pa; } ps_pipe_edge;
/* Steady passive linear network: each free node has sum(outgoing Q)=0.
 * 1..16 nodes, 0..32 edges, finite G>=0, no self edges; parallel edges allowed.
 * fixed[i] is 0/1. Only fixed pressures are read; free placeholders are ignored.
 * Every component connected by G>0 needs a fixed node. G=0 closes an edge.
 * Positive conductances that disappear under global scaling return PS_NUMERIC.
 * Pressure scaling is per connected component; loss of a nonzero fixed
 * pressure under that component scaling also returns PS_NUMERIC;
 * ill-conditioned pivots return PS_SINGULAR. No pumps, inertia or compressibility.
 * Outputs: node pressures and A->B edge flows. They must be disjoint; either
 * can alias input storage. NULL edges/flows allowed only for zero edge count. */
ps_result ps_pipe_network_solve(const uint8_t *fixed, const double *fixed_pressures_pa,
                                size_t nodes, const ps_pipe_edge *edges, size_t edge_count,
                                double *pressures_pa, double *flows_m3_s);
#define PS_TRANSPORT_MAX_CELLS 4096u
/* Periodic 1D passive tracer, uniform dx, constant signed velocity and D>=0:
 * first-order upwind advection + centered explicit diffusion. Concentration
 * is nonnegative kg/m^3; 3..4096 cells, dx>0, dt>=0. Requires
 * abs(v)*dt/dx + 2*D*dt/dx^2 <=1. Convex update preserves mass/max principle
 * to floating-point rounding. dt=0 copies input. Bounded 32 KiB stack scratch
 * permits output aliasing any input. No fluid momentum/pressure solve,
 * reactions, nonuniform grid, forcing or open boundaries; not a CFD solver. */
ps_result ps_transport_periodic_step(const double *concentration_kg_m3, size_t count,
                                      double velocity_m_s, double diffusivity_m2_s,
                                      double dx_m, double dt_s, double *next_kg_m3);
#endif
