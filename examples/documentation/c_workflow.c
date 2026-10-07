#include <math.h>
#include <stdbool.h>
#include <stdio.h>
typedef struct {double mass_kg,speed_m_s;} motion;
static double kinetic_energy(motion value){return .5*value.mass_kg*value.speed_m_s*value.speed_m_s;}
/* Failure preserves caller output, like checked Physim operations. */
static bool positive(double value,double *out){if(!out || !isfinite(value) || value<=0)return false;*out=value;return true;}
int main(void){
    motion state={2,3};double energy=kinetic_energy(state);
    double speeds[]={3,4},total=0;
    for(unsigned i=0;i<2;i++){motion sample=state;sample.speed_m_s=speeds[i];total+=kinetic_energy(sample);}
    motion copy=state;copy.speed_m_s=5;
    double selected=7;bool found=positive(-1,&selected);
    if(energy!=9 || total/2!=12.5 || state.speed_m_s!=3 || copy.speed_m_s!=5 || found || selected!=7)return 1;
    printf("Energy: %.1f J\nMean: %.1f J\n",energy,total/2);return 0;
}
