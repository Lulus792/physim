#ifndef PHYSIM_DETAIL_RUN_IDENTITY_H
#define PHYSIM_DETAIL_RUN_IDENTITY_H
#include <stdbool.h>
/* Internal shared lexical contract. Identity is 32 lower-case hex bytes. */
static inline bool ps_run_identity_valid(const char *text) {
    if(!text)return false;
    for(unsigned i=0;i<32;i++)
        if(!((text[i]>='0' && text[i]<='9') || (text[i]>='a' && text[i]<='f')))return false;
    return text[32]==0;
}
#endif
