#ifndef PS_NUMBER_PARSE_H
#define PS_NUMBER_PARSE_H
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>

/* Finite numeric input, including representable subnormals. ERANGE with zero
 * or infinity is rejected. With end=NULL require the complete string; otherwise
 * expose the first unconsumed byte. Outputs are unchanged on failure. */
static inline bool ps_parse_finite_number(const char *text,char **end,double *out) {
    if(!text || !out)return false;
    char *stop;errno=0;double value=strtod(text,&stop);
    if(stop==text || !isfinite(value) || (!end && *stop) ||
       (errno && !(errno==ERANGE && value!=0)))return false;
    if(end)*end=stop;
    *out=value;return true;
}
#endif
