#ifndef PHYSIM_OPTICS_H
#define PHYSIM_OPTICS_H
#include "math.h"
/* Pure geometric optics; no allocation or hidden state. Finite inputs only.
 * All errors preserve output. PS_INVALID: bad input/NULL output; PS_NUMERIC:
 * unrepresentable result; PS_SINGULAR: total internal reflection or focal plane.
 * Directions/normals must be unit vectors within 1e-10; accepted values are
 * normalized. Normal points into incident medium; incident dot normal<=1e-10.
 * Homogeneous isotropic media, sharp interface, no Fresnel amplitude,
 * polarization, absorption, diffraction, interference or automatic ray tracing. */
ps_result ps_ray_reflect(ps_vec3 incident, ps_vec3 normal, ps_vec3 *reflected);
/* Snell's law with positive indices n1/n2. Critical-angle sin(theta2) within
 * 32*DBL_EPSILON above one is clamped to one. Beyond it PS_SINGULAR denotes TIR;
 * call reflection explicitly if that is the desired physical branch. */
ps_result ps_ray_refract(ps_vec3 incident, ps_vec3 normal, double incident_index,
                          double transmitted_index, ps_vec3 *transmitted);
/* Paraxial thin lens: signed nonzero focal length f, positive real-object
 * distance d, both m. out.x image distance=f*d/(d-f), out.y magnification=-f/(d-f).
 * Negative image distance means virtual; negative magnification means inverted.
 * d==f has image at infinity and returns PS_SINGULAR. No thick lenses/aberration. */
ps_result ps_thin_lens_image(double focal_length_m, double object_distance_m, ps_vec2 *out);
#endif
