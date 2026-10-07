#ifndef PHYSIM_WAVES_H
#define PHYSIM_WAVES_H
#include "math.h"
/* Pure SI functions; finite inputs, no allocation or hidden state. Invalid
 * inputs/NULL outputs return PS_INVALID; numeric range/phase failure returns
 * PS_NUMERIC. Every error preserves all caller output. Signed results may
 * round to zero. */
/* Exact undamped harmonic oscillator: state.x=position m, state.y=velocity m/s.
 * omega>0 in rad/s, dt>=0. No forcing/damping; dt=0 copies state exactly. */
ps_result ps_harmonic_step(ps_vec2 state, double omega_rad_s, double dt_s, ps_vec2 *out);
/* Ideal taut string speed sqrt(tension/linear density), in m/s; both positive. */
ps_result ps_string_wave_speed(double tension_n, double linear_density_kg_m, double *speed_m_s);
/* A sin(k*x-omega*t+phase), with amplitude>=0, k>0, omega>0, time>=0.
 * out.x displacement m, out.y velocity m/s, out.z dimensionless slope du/dx.
 * Positive k/omega propagates toward +x. No dispersion relation is imposed:
 * choose omega=c*k explicitly. Trig accuracy is limited for huge phase values. */
ps_result ps_traveling_wave(double amplitude_m, double wavenumber_rad_m,
                            double omega_rad_s, double phase_rad, double position_m,
                            double time_s, ps_vec3 *out);
#define PS_STRING_WAVE_MAX_NODES 4096u
/* Fixed-zero-endpoint 1D wave equation u_tt=c^2*u_xx, centered in space/time:
 * next[j]=2*(1-lambda^2)*current[j]+lambda^2*(current[j-1]+current[j+1])-previous[j].
 * 3..4096 nodes, constant positive speed, dx, dt; lambda=c*dt/dx<=1.
 * previous/current are at t-dt/t with the SAME dt, both endpoints exactly zero.
 * Arrays contain displacement in m. No initial-velocity inference, forcing,
 * damping, variable medium, adaptive time step or higher-dimensional PDE.
 * Exceeding the node limit returns PS_LIMIT; CFL violation PS_INVALID.
 * A bounded 32 KiB stack buffer makes failure atomic and permits output aliasing
 * any input storage. Output requires count doubles; caller owns all buffers. */
ps_result ps_string_wave_step(const double *previous_m, const double *current_m,
                               size_t count, double speed_m_s, double dx_m,
                               double dt_s, double *next_m);
#endif
