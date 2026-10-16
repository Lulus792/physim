/* Completes the function around the unchanged published statements. */
    return result;
}
#include <stdio.h>
int main(void) {
    for(unsigned i=0;i<8;i++) {
        ps_result result=published_memory_example();
        if(result!=PS_OK){fprintf(stderr,"Published arena report: %d\n",(int)result);return 1;}
    }
    puts("Published memory example: arena report construction and cleanup passed eight times");
    return 0;
}
