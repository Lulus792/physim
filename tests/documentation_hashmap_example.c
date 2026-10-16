/* Appended to the exact published C block from docs/hashmap.md. */
#include <stdio.h>
int main(void) {
    ps_hashmap *map=NULL;
    if(store_parameters(NULL)!=PS_INVALID || store_parameters(&map)!=PS_OK)return 1;
    double mass=0;
    int result=ps_hashmap_count(map)!=1 ||
        ps_hashmap_get(map,(ps_string_view){"mass_kg",7},&mass)!=PS_OK || mass!=2.5;
    mass=123;
    if(ps_hashmap_get(map,(ps_string_view){"missing",7},&mass)!=PS_EOF || mass!=123)result=1;
    ps_hashmap_destroy(map);
    if(!result)puts("Published hashmap example: copied parameter, lookup and preserved missing output passed");
    return result;
}
