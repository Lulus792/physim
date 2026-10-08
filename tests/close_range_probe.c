#include "physim/math.h"
#include <stdio.h>
int main(void) {
    double a, b, absolute, relative;
    while (scanf("%la %la %la %la", &a, &b, &absolute, &relative) == 4)
        printf("%d %d\n", ps_close(a, b, absolute, relative), ps_close(b, a, absolute, relative));
    return ferror(stdin) ? 2 : 0;
}
