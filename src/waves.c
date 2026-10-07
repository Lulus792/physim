#include "physim/waves.h"
#include <math.h>
#include <string.h>
static bool positive(double x) { return isfinite(x) && x>0; }
typedef struct { double m;int e; } scaled;
static scaled product(double a,double b,double divisor) {
    int ea,eb,ed;double ma=frexp(a,&ea),mb=frexp(b,&eb),md=frexp(divisor,&ed);
    return (scaled){ma*mb/md,ea+eb-ed};
}
static double sum(scaled *terms,size_t count) {
    for(size_t i=0;i<count;i++)for(size_t j=i+1;j<count;j++)
        if((terms[j].m!=0 && terms[i].m==0) || (terms[j].m!=0 && terms[j].e>terms[i].e)) {
            scaled swap=terms[i];terms[i]=terms[j];terms[j]=swap;
        }
    scaled result=terms[0];
    for(size_t i=1;i<count;i++) {
        if(terms[i].m==0)continue;
        if(result.m==0){result=terms[i];continue;}
        int exponent=result.e>terms[i].e?result.e:terms[i].e;
        double value=scalbn(result.m,result.e-exponent)+scalbn(terms[i].m,terms[i].e-exponent);
        int extra;result.m=frexp(value,&extra);result.e=exponent+extra;
    }
    return scalbn(result.m,result.e);
}
ps_result ps_harmonic_step(ps_vec2 state,double omega,double dt,ps_vec2 *out) {
    if(!out || !isfinite(state.x) || !isfinite(state.y) || !positive(omega) || !isfinite(dt) || dt<0)return PS_INVALID;
    if(dt==0){*out=state;return PS_OK;}
    scaled phase=product(omega,dt,1);double angle=scalbn(phase.m,phase.e);
    if(!isfinite(angle))return PS_NUMERIC;
    double s=sin(angle),c=cos(angle);
    scaled position[]={product(state.x,c,1),product(state.y,s,omega)};
    scaled velocity[]={product(state.y,c,1),product(state.x,-omega,1)};
    velocity[1].m*=s;
    ps_vec2 result={sum(position,2),sum(velocity,2)};
    if(!isfinite(result.x) || !isfinite(result.y))return PS_NUMERIC;
    *out=result;return PS_OK;
}
ps_result ps_string_wave_speed(double tension,double density,double *out) {
    if(!out || !positive(tension) || !positive(density))return PS_INVALID;
    int ea,eb;double ma=frexp(tension,&ea),mb=frexp(density,&eb);int exponent=ea-eb;
    if(exponent%2){ma*=2;exponent--;}
    double speed=scalbn(sqrt(ma/mb),exponent/2);
    if(!positive(speed))return PS_NUMERIC;
    *out=speed;return PS_OK;
}
ps_result ps_traveling_wave(double amplitude,double k,double omega,double phase,double x,double t,ps_vec3 *out) {
    if(!out || !isfinite(amplitude) || amplitude<0 || !positive(k) || !positive(omega) ||
        !isfinite(phase) || !isfinite(x) || !isfinite(t) || t<0)return PS_INVALID;
    scaled phases[]={product(k,x,1),product(-omega,t,1),product(phase,1,1)};
    double angle=sum(phases,3);if(!isfinite(angle))return PS_NUMERIC;
    double sine=sin(angle),cosine=cos(angle);
    scaled velocity=product(amplitude,-omega,1),slope=product(amplitude,k,1);
    velocity.m*=cosine;slope.m*=cosine;
    ps_vec3 result={amplitude*sine,scalbn(velocity.m,velocity.e),scalbn(slope.m,slope.e)};
    if(!isfinite(result.x) || !isfinite(result.y) || !isfinite(result.z))return PS_NUMERIC;
    *out=result;return PS_OK;
}
ps_result ps_string_wave_step(const double *previous,const double *current,size_t count,
                               double speed,double dx,double dt,double *out) {
    if(count>PS_STRING_WAVE_MAX_NODES)return PS_LIMIT;
    if(!previous || !current || !out || count<3 || !positive(speed) || !positive(dx) || !positive(dt))return PS_INVALID;
    scaled courant=product(speed,dt,dx);double lambda=scalbn(courant.m,courant.e);
    if(!isfinite(lambda) || lambda>1)return PS_INVALID;
    for(size_t j=0;j<count;j++)if(!isfinite(previous[j]) || !isfinite(current[j]))return PS_INVALID;
    if(previous[0]!=0 || previous[count-1]!=0 || current[0]!=0 || current[count-1]!=0)return PS_INVALID;
    double next[PS_STRING_WAVE_MAX_NODES];next[0]=next[count-1]=0;
    int lambda_exponent;double lambda_mantissa=frexp(lambda,&lambda_exponent);
    for(size_t j=1;j+1<count;j++) {
        scaled terms[]={product(current[j],2,1),product(previous[j],-1,1),
                        product(current[j],-2*lambda_mantissa*lambda_mantissa,1),
                        product(current[j-1],lambda_mantissa*lambda_mantissa,1),
                        product(current[j+1],lambda_mantissa*lambda_mantissa,1)};
        for(unsigned k=2;k<5;k++)terms[k].e+=2*lambda_exponent;
        next[j]=sum(terms,5);if(!isfinite(next[j]))return PS_NUMERIC;
    }
    memcpy(out,next,count*sizeof *out);return PS_OK;
}
