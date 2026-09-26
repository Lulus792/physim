#define PSRT_SOURCE "array-exit.phys"
#include "physim/language_runtime.h"
#include "physim/language_array.h"
#include "test_allocator.h"

static test_allocator tracker;
static const psrt_element_type integers = {sizeof(int64_t), NULL, NULL};
static void verify_cleanup(void) {
    if (tracker.live_blocks || tracker.live_bytes || tracker.invalid)
        _Exit(99);
    puts("standalone owners released");
}
int main(void) {
    psrt_array array;
    if (atexit(verify_cleanup) != 0 ||
        psrt_array_init(&integers, test_domain(&tracker), 10, &array) != PS_OK)
        return 1;
    psrt_cleanup owner;
    psrt_cleanup_push(&owner, &array, psrt_array_destroy);
    int64_t value = 5;
    if (psrt_array_replace(&array, 0, 0, &value, 1) != PS_OK)
        return 2;
    psrt_fail(PSRT_AT(8, 4), "owned array exit probe");
}
