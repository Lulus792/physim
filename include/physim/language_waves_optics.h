#ifndef PHYSIM_LANGUAGE_WAVES_OPTICS_H
#define PHYSIM_LANGUAGE_WAVES_OPTICS_H
#include "language_runtime.h"
#include "language_array.h"
#include "waves.h"
#include "optics.h"
static inline ps_vec2 psrt_harmonic_step(ps_vec2 state,double omega,double dt,psrt_site site) {
    ps_vec2 value={0};ps_result r=ps_harmonic_step(state,omega,dt,&value);
    if(r!=PS_OK)psrt_raise(site,r,"Harmonic oscillator: invalid SI inputs or numeric phase/range",NULL);
    return value;
}
static inline double psrt_string_speed(double tension,double density,psrt_site site) {
    double value=0;ps_result r=ps_string_wave_speed(tension,density,&value);
    if(r!=PS_OK)psrt_raise(site,r,"String wave speed: invalid inputs or numeric range",NULL);
    return value;
}
static inline ps_vec3 psrt_traveling_wave(double amplitude,double k,double omega,double phase,double x,double t,psrt_site site) {
    ps_vec3 value={0};ps_result r=ps_traveling_wave(amplitude,k,omega,phase,x,t,&value);
    if(r!=PS_OK)psrt_raise(site,r,"Traveling wave: invalid SI inputs or numeric phase/range",NULL);
    return value;
}
static inline psrt_array psrt_string_wave_step(ps_allocator allocator,const double *previous,size_t previous_count,
                                               const double *current,size_t count,double speed,double dx,double dt,psrt_site site) {
    if(previous_count!=count)psrt_fail(site,"String wave arrays must have equal node counts");
    if(count>PS_STRING_WAVE_MAX_NODES)psrt_fail(site,"String wave supports at most 4096 nodes");
    static const psrt_element_type element={sizeof(double),NULL,NULL};
    psrt_array result;
    if(psrt_array_init(&element,allocator,PS_STRING_WAVE_MAX_NODES,&result)!=PS_OK ||
        psrt_array_build_begin(&result,count)!=PS_OK)
        psrt_fail(site,"String wave result memory budget or allocation exhausted");
    double *data=result.block?(double *)(result.block+1):NULL;
    ps_result r=ps_string_wave_step(previous,current,count,speed,dx,dt,data);
    if(r!=PS_OK){psrt_array_destroy(&result);psrt_raise(site,r,"String wave: invalid arrays, endpoints, CFL or numeric range",NULL);}
    result.block->value.count=count;return result;
}
static inline ps_vec3 psrt_ray_reflect(ps_vec3 incident,ps_vec3 normal,psrt_site site) {
    ps_vec3 value={0};ps_result r=ps_ray_reflect(incident,normal,&value);
    if(r!=PS_OK)psrt_raise(site,r,"Reflection: invalid unit directions or normal orientation",NULL);
    return value;
}
static inline ps_vec3 psrt_ray_refract(ps_vec3 incident,ps_vec3 normal,double n1,double n2,psrt_site site) {
    ps_vec3 value={0};ps_result r=ps_ray_refract(incident,normal,n1,n2,&value);
    if(r==PS_SINGULAR)psrt_raise(site,r,"Total internal reflection: no transmitted ray",NULL);
    if(r!=PS_OK)psrt_raise(site,r,"Refraction: invalid directions, indices or numeric range",NULL);
    return value;
}
static inline ps_vec2 psrt_thin_lens(double focal,double distance,psrt_site site) {
    ps_vec2 value={0};ps_result r=ps_thin_lens_image(focal,distance,&value);
    if(r==PS_SINGULAR)psrt_raise(site,r,"Thin lens: object at focal plane has image at infinity",NULL);
    if(r!=PS_OK)psrt_raise(site,r,"Thin lens: invalid distances or numeric range",NULL);
    return value;
}
#endif
