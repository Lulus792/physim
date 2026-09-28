#include <stdlib.h>

/* Deliberate outstanding allocation counter: validate the test harness itself
 * on both normal returns and the exit path used by language runtime traps. */
static struct {
    size_t live_bytes;
} psmemory;

static int ps_test_main(void) {
#ifdef PHYSIM_MEMORY_PROBE_LEAK
    psmemory.live_bytes = 1;
#endif
#ifdef PHYSIM_MEMORY_PROBE_TRAP
    exit(70);
#endif
    return 0;
}

#include "language_memory_balance.h"
