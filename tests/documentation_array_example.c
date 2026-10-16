/* Appended to the exact published C block from docs/array.md. */
#include <math.h>
#include <stdio.h>
int main(void) {
    ps_array positions={0};
    if(make_positions(NULL)!=PS_INVALID || make_positions(&positions)!=PS_OK)return 1;
    int result=0;
    if(positions.count!=100 || positions.element_size!=sizeof(ps_vec3) || positions.maximum_count!=4096)result=1;
    const ps_vec3 *points=positions.data;
    for(size_t i=0;!result && i<positions.count;i++)
        if(fabs(points[i].x-(double)i*.01)>1e-14 || points[i].y!=0 || points[i].z!=0)result=1;
    ps_array_destroy(&positions);
    if(positions.data || positions.count || positions.capacity)result=1;
    if(!result)puts("Published array example: 100 positions, coordinates and ownership passed");
    return result;
}
