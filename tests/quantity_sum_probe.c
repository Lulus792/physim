#include "physim/units.h"
#include <stdio.h>
int main(void) {
    double a,b,sa,sb;int subtract;
    while(scanf("%la %la %la %la %d",&a,&b,&sa,&sb,&subtract)==5) {
        ps_unit ua=PS_METRE,ub=PS_METRE;ua.scale=sa;ua.symbol="a";ub.scale=sb;ub.symbol="b";
        ps_quantity result={19,PS_SECOND};
        ps_result status=subtract?ps_quantity_subtract((ps_quantity){a,ua},(ps_quantity){b,ub},&result):
                                  ps_quantity_add((ps_quantity){a,ua},(ps_quantity){b,ub},&result);
        printf("%d %.17g %.17g %d %d\n",status,result.value,result.unit.scale,result.unit.dimension[0],result.unit.dimension[2]);
        if(status==PS_OK && result.unit.symbol!=ua.symbol)return 3;
    }
    return ferror(stdin)||ferror(stdout)?2:0;
}
