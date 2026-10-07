#include "physim/optics.h"
#include <float.h>
#include <math.h>
static bool positive(double x) { return isfinite(x) && x>0; }
static bool finite3(ps_vec3 v) { return isfinite(v.x) && isfinite(v.y) && isfinite(v.z); }
static double ratio(double a,double b,double d,double factor) {
    if(a==0 || b==0)return 0;
    int ea,eb,ed;double ma=frexp(a,&ea),mb=frexp(b,&eb),md=frexp(d,&ed);
    return scalbn((ma*mb/md)/factor,ea+eb-ed);
}
static ps_result directions(ps_vec3 *incident,ps_vec3 *normal,double *cosine) {
    if(!finite3(*incident) || !finite3(*normal))return PS_INVALID;
    double i=hypot(hypot(incident->x,incident->y),incident->z),n=hypot(hypot(normal->x,normal->y),normal->z);
    if(!isfinite(i) || !isfinite(n) || fabs(i-1)>1e-10 || fabs(n-1)>1e-10)return PS_INVALID;
    *incident=ps_vscale(*incident,1/i);*normal=ps_vscale(*normal,1/n);
    *cosine=-ps_vdot(*incident,*normal);
    if(*cosine < -1e-10)return PS_INVALID;
    if(*cosine<0)*cosine=0;
    if(*cosine>1)*cosine=1;
    return PS_OK;
}
ps_result ps_ray_reflect(ps_vec3 i,ps_vec3 n,ps_vec3 *out) {
    if(!out)return PS_INVALID;double cosine;
    ps_result r=directions(&i,&n,&cosine);if(r!=PS_OK)return r;
    ps_vec3 result=ps_v3(fma(2*cosine,n.x,i.x),fma(2*cosine,n.y,i.y),fma(2*cosine,n.z,i.z));
    *out=ps_vnormalize(result);return PS_OK;
}
ps_result ps_ray_refract(ps_vec3 i,ps_vec3 n,double n1,double n2,ps_vec3 *out) {
    if(!out || !positive(n1) || !positive(n2))return PS_INVALID;double cosine;
    ps_result r=directions(&i,&n,&cosine);if(r!=PS_OK)return r;
    ps_vec3 cross=ps_vcross(i,n);
    if(cross.x==0 && cross.y==0 && cross.z==0){*out=ps_vscale(n,-1);return PS_OK;}
    ps_vec3 tangent=ps_v3(fma(cosine,n.x,i.x),fma(cosine,n.y,i.y),fma(cosine,n.z,i.z));
    double sine=hypot(hypot(tangent.x,tangent.y),tangent.z),transverse=ratio(n1,sine,n2,1);
    if(transverse>1+32*DBL_EPSILON)return PS_SINGULAR;
    if(transverse>1)transverse=1;
    if(sine==0){*out=ps_vscale(n,-1);return PS_OK;}
    double longitudinal=sqrt(fmax(0,(1-transverse)*(1+transverse)));
    ps_vec3 result=ps_v3(fma(transverse,tangent.x/sine,-longitudinal*n.x),
                         fma(transverse,tangent.y/sine,-longitudinal*n.y),
                         fma(transverse,tangent.z/sine,-longitudinal*n.z));
    if(!finite3(result))return PS_NUMERIC;
    *out=ps_vnormalize(result);return PS_OK;
}
ps_result ps_thin_lens_image(double focal,double distance,ps_vec2 *out) {
    if(!out || !isfinite(focal) || focal==0 || !positive(distance))return PS_INVALID;
    if(distance==focal)return PS_SINGULAR;
    double denominator=distance-focal,factor=1;
    if(!isfinite(denominator)){denominator=distance*.5-focal*.5;factor=2;}
    ps_vec2 result={ratio(focal,distance,denominator,factor),ratio(-focal,1,denominator,factor)};
    if(!isfinite(result.x) || !isfinite(result.y))return PS_NUMERIC;
    *out=result;return PS_OK;
}
