#ifndef PHYSIM_THERMODYNAMICS_H
#define PHYSIM_THERMODYNAMICS_H
#include "math.h"

/* Double representation of the exact SI molar gas constant, J/(mol K). No hidden state or allocations.
 * Every argument must be finite. Absolute temperatures, amounts, volumes,
 * pressures and heat capacities are strictly positive. Conductance and dt may
 * be zero. Every error preserves caller output; NULL output is PS_INVALID.
 * PS_NUMERIC means a required positive result rounds to zero, or a result is
 * not finite. Signed heat/power/entropy may round to zero. */
#define PS_MOLAR_GAS_CONSTANT 8.31446261815324
/* Ideal dilute gas only: p V = n R T. No phase transitions or real-gas terms. */
ps_result ps_ideal_gas_pressure(double amount_mol, double temperature_k, double volume_m3,
                                double *pressure_pa);
ps_result ps_ideal_gas_volume(double amount_mol, double temperature_k, double pressure_pa,
                              double *volume_m3);
ps_result ps_ideal_gas_temperature(double amount_mol, double pressure_pa, double volume_m3,
                                   double *temperature_k);
/* Constant molar cv, with U=0 at T=0 as the chosen reference: U=n cv T. */
ps_result ps_ideal_gas_energy(double amount_mol, double molar_cv_j_mol_k,
                              double temperature_k, double *energy_j);
/* Same amount and constant cv in both equilibrium states. Reversible state
 * difference dS = n [cv ln(T1/T0) + R ln(V1/V0)]. Not an entropy-production
 * estimate or a path heat integral. States need not have equal pressure. */
ps_result ps_ideal_gas_entropy_change(double amount_mol, double molar_cv_j_mol_k,
                                      double initial_temperature_k, double initial_volume_m3,
                                      double final_temperature_k, double final_volume_m3,
                                      double *entropy_j_k);
/* Constant specific heat, no latent heat: C=m c, Q=C (T1-T0). */
ps_result ps_heat_capacity(double mass_kg, double specific_heat_j_kg_k, double *capacity_j_k);
ps_result ps_sensible_heat(double capacity_j_k, double initial_temperature_k,
                            double final_temperature_k, double *heat_j);
/* Lumped linear conduction, positive power flows from A to B: P=G(Ta-Tb).
 * For a homogeneous slab, choose G=k A/L explicitly from its model data. */
ps_result ps_heat_flow(double conductance_w_k, double temperature_a_k,
                        double temperature_b_k, double *power_w);
/* Exact relaxation to a prescribed constant-temperature reservoir:
 * T(t+dt)=Tr+(T-Tr)exp(-G dt/C). No time-step stability restriction.
 * Constant C,G; no radiation, phase change or spatial temperature field. */
ps_result ps_thermal_reservoir_step(double capacity_j_k, double temperature_k,
                                    double reservoir_temperature_k, double conductance_w_k,
                                    double dt_s, double *next_temperature_k);
/* Exact isolated two-body exchange, constant capacities and conductance.
 * out.x/out.y are final A/B temperatures; energy C_a T_a+C_b T_b is conserved
 * to floating-point rounding. Both approach their capacity-weighted equilibrium
 * monotonically, with rate G(1/C_a+1/C_b). Inputs can be out's own components. */
ps_result ps_thermal_pair_step(double capacity_a_j_k, double temperature_a_k,
                               double capacity_b_j_k, double temperature_b_k,
                               double conductance_w_k, double dt_s, ps_vec2 *out);
#endif
