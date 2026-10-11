#include "physim/measurement.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv) {
    if(argc!=5)return 2;
    ps_mean_interval out={0};ps_result r=ps_student_mean_interval(strtod(argv[1],NULL),strtod(argv[2],NULL),
        (uint64_t)strtoull(argv[3],NULL,10),strtod(argv[4],NULL),&out);
    printf("%d %.17g %.17g %.17g %.17g %.17g\n",r,out.critical_value,out.standard_error,out.lower,out.upper,out.confidence);
    return 0;
}
