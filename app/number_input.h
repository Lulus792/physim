#ifndef PS_NUMBER_INPUT_H
#define PS_NUMBER_INPUT_H
#include "number_parse.h"
#include <string.h>
/* UI numeric input accepts either decimal mark. Wire/project parsers keep
 * their own strict canonical grammar. Never modify the output on failure. */
static inline bool ps_parse_input_number(const char *text,double *value) {
    if(!text || !value)return false;
    size_t n=strlen(text);if(!n || n>=64)return false;
    char normalized[64];unsigned commas=0,dots=0;
    for(size_t i=0;i<=n;i++){char c=text[i];commas+=c==',';dots+=c=='.';normalized[i]=c==','?'.':c;}
    return commas<=1 && !(commas && dots) && ps_parse_finite_number(normalized,NULL,value);
}
#endif
