#include "platform.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Usage %d: %s\n", __LINE__, #x); return 1; } } while (0)
static volatile double result;
int main(void) {
    CHECK(!ps_process_usage_self(NULL));
    ps_process_usage before, after;
    CHECK(ps_process_usage_self(&before));
    CHECK(isfinite(before.user_seconds) && before.user_seconds >= 0 &&
          isfinite(before.system_seconds) && before.system_seconds >= 0 &&
          before.peak_resident_bytes > 0);
    /* Commit pages, then consume actual CPU. No hardware-specific time/RAM gate. */
    size_t size = 32u * 1024u * 1024u;
    volatile unsigned char *memory = malloc(size);
    CHECK(memory);
    for (size_t i = 0; i < size; i += 4096) memory[i] = (unsigned char)i;
    double start = ps_clock();
    do {
        for (unsigned i = 1; i < 10000; ++i) result += sqrt((double)i);
    } while (ps_clock() - start < .04);
    CHECK(ps_process_usage_self(&after));
    fprintf(stderr,
            "Resources before: user=%.9f system=%.9f peak=%llu; "
            "after: user=%.9f system=%.9f peak=%llu; allocation=%llu\n",
            before.user_seconds, before.system_seconds,
            (unsigned long long)before.peak_resident_bytes,
            after.user_seconds, after.system_seconds,
            (unsigned long long)after.peak_resident_bytes, (unsigned long long)size);
    CHECK(after.user_seconds >= before.user_seconds &&
          after.system_seconds >= before.system_seconds &&
          after.user_seconds + after.system_seconds > before.user_seconds + before.system_seconds &&
          after.peak_resident_bytes >= before.peak_resident_bytes &&
          after.peak_resident_bytes >= size);
    free((void *)memory);
    ps_sleep(20);
    ps_process_usage released;
    CHECK(ps_process_usage_self(&released) && released.peak_resident_bytes >= after.peak_resident_bytes);
    printf("Process resources: user/system CPU and lifetime peak resident bytes verified\n");
    return 0;
}
