#define PSRT_MODULE
#define PSRT_SOURCE "array-values.phys"
#include "physim/language_runtime.h"
#include "physim/language_array.h"
#include "test_allocator.h"
#include <stdio.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language values line %d: %s\n", __LINE__, #x);                        \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static const psrt_element_type integers = {sizeof(int64_t), NULL, NULL};
static const psrt_element_type floats = {sizeof(double), NULL, NULL};
static bool same_integer(const void *left, const void *right) {
    return *(const int64_t *)left == *(const int64_t *)right;
}
static int split_values(void) {
    test_allocator tracker = {0};
    psrt_array source, parts = {0};
    int64_t values[] = {0, 1, 0, 0, 2, 0}, separator = 0;
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 8, &source) == PS_OK);
    CHECK(psrt_array_replace(&source, 0, 0, values, 6) == PS_OK);
    CHECK(psrt_array_split(&source, &separator, INT64_MAX, true, same_integer,
                           test_domain(&tracker), &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 2);
    const psrt_array *groups = psrt_array_data(&parts);
    CHECK(psrt_array_count(&groups[0]) == 1 && psrt_array_count(&groups[1]) == 1);
    CHECK(*(const int64_t *)psrt_array_data(&groups[0]) == 1);
    CHECK(*(const int64_t *)psrt_array_data(&groups[1]) == 2);
    psrt_array_destroy(&parts);
    CHECK(psrt_array_split(&source, &separator, 1, false, same_integer,
                           test_domain(&tracker), &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 2);
    groups = psrt_array_data(&parts);
    CHECK(psrt_array_count(&groups[0]) == 0 && psrt_array_count(&groups[1]) == 5);
    psrt_array_destroy(&parts);
    for (size_t offset = 1; offset <= 3; offset++) {
        size_t before = tracker.live_bytes;
        tracker.fail_on = tracker.attempts + offset;
        ps_result status = psrt_array_split(&source, &separator, INT64_MAX, true,
                                            same_integer, test_domain(&tracker), &parts);
        if (offset == 2) {
            CHECK(status == PS_OK); /* Builder falls back to exact capacity. */
            psrt_array_destroy(&parts);
        } else
            CHECK(status == PS_MEMORY);
        CHECK(!parts.type && !parts.block && tracker.live_bytes == before);
        tracker.fail_on = 0;
    }
    CHECK(!memcmp(psrt_array_data(&source), values, sizeof values));
    psrt_array_destroy(&source);
    CHECK(!tracker.live_bytes && !tracker.live_blocks && !tracker.invalid);
    return 0;
}
static int sorting_numbers(void) {
    test_allocator tracker = {0};
    psrt_array source, sorted = {0};
    CHECK(psrt_array_init(&floats, test_domain(&tracker), 8, &source) == PS_OK);
    double values[] = {2.0, NAN, -1.0};
    CHECK(psrt_array_replace(&source, 0, 0, values, 3) == PS_OK);
    size_t extreme = SIZE_MAX;
    CHECK(psrt_array_extremum_index(&source, psrt_compare_float64,
                                   psrt_finite_float64_element, false, &extreme) == PS_NUMERIC);
    CHECK(extreme == SIZE_MAX);
    CHECK(psrt_array_sorted(&source, psrt_compare_float64,
                            psrt_finite_float64_element, &sorted) == PS_NUMERIC);
    CHECK(!sorted.type && !sorted.block && isnan(((const double *)psrt_array_data(&source))[1]));
    double replacement = 1.0;
    CHECK(psrt_array_replace(&source, 1, 1, &replacement, 1) == PS_OK);
    CHECK(psrt_array_extremum_index(&source, psrt_compare_float64,
                                   psrt_finite_float64_element, false, &extreme) == PS_OK);
    CHECK(extreme == 2);
    CHECK(psrt_array_extremum_index(&source, psrt_compare_float64,
                                   psrt_finite_float64_element, true, &extreme) == PS_OK);
    CHECK(extreme == 0);
    CHECK(psrt_array_sorted(&source, psrt_compare_float64,
                            psrt_finite_float64_element, &sorted) == PS_OK);
    const double *ordered = psrt_array_data(&sorted);
    CHECK(ordered[0] == -1.0 && ordered[1] == 1.0 && ordered[2] == 2.0);
    psrt_array_destroy(&sorted);
    psrt_array_destroy(&source);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    return 0;
}
static int snapshots(void) {
    test_allocator tracker = {0};
    psrt_array array, snapshot;
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 128, &array) == PS_OK);
    CHECK(!psrt_array_count(&array) && !psrt_array_data(&array) && !tracker.attempts);
    int64_t initial[] = {10, 20, 30, 40};
    CHECK(psrt_array_replace(&array, 0, 0, initial, 4) == PS_OK);
    CHECK((uintptr_t)psrt_array_data(&array) % PS_MEMORY_ALIGNMENT == 0);
    size_t attempts = tracker.attempts;
    CHECK(psrt_array_clone(&array, &snapshot) == PS_OK && tracker.attempts == attempts);
    CHECK(psrt_array_data(&array) == psrt_array_data(&snapshot));
    /* Self-insertion must read the old snapshot throughout publication. */
    CHECK(psrt_array_replace(&array, 1, 1, psrt_array_data(&array), 4) == PS_OK);
    const int64_t expected[] = {10, 10, 20, 30, 40, 30, 40};
    CHECK(psrt_array_count(&array) == 7 &&
          !memcmp(psrt_array_data(&array), expected, sizeof expected));
    CHECK(psrt_array_count(&snapshot) == 4 &&
          !memcmp(psrt_array_data(&snapshot), initial, sizeof initial));
    const void *value = NULL;
    CHECK(psrt_array_at(&array, 6, &value) == PS_OK && *(const int64_t *)value == 40);
    const void *saved = value;
    CHECK(psrt_array_at(&array, -1, &value) == PS_LIMIT && value == saved);
    CHECK(psrt_array_at(&array, 7, &value) == PS_LIMIT && value == saved);
    CHECK(psrt_array_at(&array, INT64_MAX, &value) == PS_LIMIT && value == saved);
    CHECK(psrt_array_replace(&array, 0, 7, NULL, 0) == PS_OK);
    CHECK(!psrt_array_count(&array) && !psrt_array_data(&array));
    psrt_array_destroy(&array);
    psrt_array_destroy(&array);
    CHECK(!memcmp(psrt_array_data(&snapshot), initial, sizeof initial));
    psrt_array_destroy(&snapshot);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    return 0;
}

static int slices(void) {
    test_allocator tracker = {0};
    psrt_array source;
    int64_t values[] = {10, 20, 30, 40};
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 10, &source) == PS_OK);
    CHECK(psrt_array_replace(&source, 0, 0, values, 4) == PS_OK);
    size_t bytes = tracker.live_bytes;
    for (int closed = 0; closed <= 1; closed++) {
        for (int64_t begin = -1; begin <= 5; begin++) {
            for (int64_t end = -1; end <= 5; end++) {
                psrt_array result = {0};
                int valid = begin >= 0 && end >= begin && end <= (closed ? 3 : 4);
                size_t start = SIZE_MAX, span = SIZE_MAX;
                CHECK(psrt_array_range(&source, begin, end, closed, &start, &span) ==
                      (valid ? PS_OK : PS_LIMIT));
                if (valid)
                    CHECK(start == (size_t)begin && span == (size_t)(end - begin + closed));
                ps_result status = psrt_array_slice(&source, begin, end, closed, &result);
                CHECK(status == (valid ? PS_OK : PS_LIMIT));
                if (valid) {
                    size_t count = (size_t)(end - begin + closed);
                    CHECK(psrt_array_count(&result) == count);
                    if (count) {
                        CHECK(!memcmp(psrt_array_data(&result), values + begin,
                                      count * sizeof(int64_t)));
                        int64_t replacement = 99;
                        CHECK(psrt_array_replace(&result, 0, 1, &replacement, 1) == PS_OK);
                    }
                    psrt_array_destroy(&result);
                } else
                    CHECK(!result.type && !result.block);
                CHECK(tracker.live_bytes == bytes && source.block->value.references == 1);
                CHECK(!memcmp(psrt_array_data(&source), values, sizeof values));
            }
        }
    }
    psrt_array result = {0};
    size_t start = SIZE_MAX, count = SIZE_MAX;
    CHECK(psrt_array_range_bounds(&source, 0, 0, 0, 0, 1, &start, &count) == PS_OK);
    CHECK(start == 0 && count == 4);
    CHECK(psrt_array_range_bounds(&source, 1, 2, 0, 0, 1, &start, &count) == PS_OK);
    CHECK(start == 2 && count == 2);
    CHECK(psrt_array_range_bounds(&source, 0, 0, 1, 2, 0, &start, &count) == PS_OK);
    CHECK(start == 0 && count == 2);
    CHECK(psrt_array_range_bounds(&source, 1, 5, 0, 0, 0, &start, &count) == PS_LIMIT);
    CHECK(psrt_array_slice_bounds(&source, 1, 4, 0, 0, 1, &result) == PS_OK);
    CHECK(psrt_array_count(&result) == 0);
    psrt_array_destroy(&result);
    CHECK(psrt_array_slice(&source, 0, INT64_MAX, 1, &result) == PS_LIMIT);
    CHECK(psrt_array_slice(&source, INT64_MIN, 0, 0, &result) == PS_LIMIT);
    CHECK(psrt_array_slice(&source, 0, 4, 0, &source) == PS_INVALID);
    CHECK(psrt_array_slice_strided_bounds(&source, 1, 0, 1, 4, 0, 2, &result) == PS_OK);
    CHECK(psrt_array_count(&result) == 2);
    CHECK(((const int64_t *)psrt_array_data(&result))[0] == 10);
    CHECK(((const int64_t *)psrt_array_data(&result))[1] == 30);
    psrt_array_destroy(&result);
    CHECK(psrt_array_slice_strided_bounds(&source, 0, 0, 0, 0, 1, 0, &result) == PS_LIMIT);
    int64_t replacement_values[] = {11, 33};
    psrt_array replacement;
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 10, &replacement) == PS_OK);
    CHECK(psrt_array_replace(&replacement, 0, 0, replacement_values, 2) == PS_OK);
    CHECK(psrt_array_scatter(&source, 0, 4, 2, &replacement) == PS_OK);
    CHECK(psrt_array_count(&source) == 4);
    CHECK(((const int64_t *)psrt_array_data(&source))[0] == 11);
    CHECK(((const int64_t *)psrt_array_data(&source))[1] == 20);
    CHECK(((const int64_t *)psrt_array_data(&source))[2] == 33);
    CHECK(psrt_array_scatter(&source, 0, 4, 3, &replacement) == PS_OK);
    CHECK(psrt_array_scatter(&source, 0, 4, 1, &replacement) == PS_LIMIT);
    CHECK(psrt_array_scatter(&source, 0, 4, 0, &replacement) == PS_LIMIT);
    psrt_array_destroy(&replacement);
    psrt_array_destroy(&source);
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 10, &source) == PS_OK);
    CHECK(psrt_array_slice(&source, 0, 0, 0, &result) == PS_OK);
    CHECK(!psrt_array_count(&result));
    psrt_array_destroy(&result);
    CHECK(psrt_array_slice(&source, 0, 0, 1, &result) == PS_LIMIT);
    psrt_array_destroy(&source);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    return 0;
}
static int nested(void) {
    test_allocator tracker = {0};
    psrt_array child, parent, snapshot, changed;
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 10, &child) == PS_OK);
    CHECK(psrt_array_init(&psrt_array_element_type, test_domain(&tracker), 10, &parent) == PS_OK);
    int64_t value = 7;
    CHECK(psrt_array_replace(&child, 0, 0, &value, 1) == PS_OK);
    CHECK(psrt_array_replace(&parent, 0, 0, &child, 1) == PS_OK);
    CHECK(psrt_array_clone(&parent, &snapshot) == PS_OK);
    CHECK(psrt_array_clone((const psrt_array *)psrt_array_data(&parent), &changed) == PS_OK);
    value = 9;
    CHECK(psrt_array_replace(&changed, 0, 1, &value, 1) == PS_OK);
    CHECK(psrt_array_replace(&parent, 0, 1, &changed, 1) == PS_OK);
    psrt_array_destroy(&changed);
    psrt_array_destroy(&child);
    const psrt_array *old_child = psrt_array_data(&snapshot);
    const psrt_array *new_child = psrt_array_data(&parent);
    CHECK(*(const int64_t *)psrt_array_data(old_child) == 7);
    CHECK(*(const int64_t *)psrt_array_data(new_child) == 9);
    psrt_array_destroy(&snapshot);
    CHECK(*(const int64_t *)psrt_array_data(new_child) == 9);
    psrt_array_destroy(&parent);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    return 0;
}

static int nested_rollback(void) {
    test_allocator tracker = {0};
    psrt_array children[2], parent;
    for (size_t i = 0; i < 2; i++) {
        int64_t value = (int64_t)i;
        CHECK(psrt_array_init(&integers, test_domain(&tracker), 10, &children[i]) == PS_OK);
        CHECK(psrt_array_replace(&children[i], 0, 0, &value, 1) == PS_OK);
    }
    CHECK(psrt_array_init(&psrt_array_element_type, test_domain(&tracker), 10, &parent) == PS_OK);
    CHECK(psrt_array_replace(&parent, 0, 0, children, 2) == PS_OK);
    const void *old = psrt_array_data(&parent);
    size_t bytes = tracker.live_bytes;
    children[1].block->value.references = SIZE_MAX;
    CHECK(psrt_array_replace(&parent, 0, 0, children, 1) == PS_LIMIT);
    CHECK(psrt_array_data(&parent) == old && psrt_array_count(&parent) == 2);
    CHECK(children[0].block->value.references == 2);
    CHECK(children[1].block->value.references == SIZE_MAX && tracker.live_bytes == bytes);
    children[1].block->value.references = 2;
    psrt_array_destroy(&parent);
    psrt_array_destroy(&children[0]);
    psrt_array_destroy(&children[1]);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    return 0;
}

typedef struct owned {
    int64_t *value;
} owned;
static test_allocator copier_tracker;
static ps_result owned_copy(void *destination, const void *source) {
    void *allocation = NULL;
    ps_result result =
        ps_memory_allocate(test_domain(&copier_tracker), sizeof(int64_t), &allocation);
    if (result != PS_OK)
        return result;
    *(int64_t *)allocation = *((const owned *)source)->value;
    *(owned *)destination = (owned){allocation};
    return PS_OK;
}
static void owned_destroy(void *object) {
    ps_memory_free(test_domain(&copier_tracker), ((owned *)object)->value, sizeof(int64_t));
}
static int compare_owned_descending(const void *left, const void *right) {
    int64_t a = *(((const owned *)left)->value), b = *(((const owned *)right)->value);
    return (a < b) - (a > b);
}
static const psrt_element_type owning = {sizeof(owned), owned_copy, owned_destroy};
static int builder_appends(void) {
    test_allocator tracker = {0};
    psrt_array values, snapshot;
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 513, &values) == PS_OK);
    for (int64_t i = 0; i < 512; i++)
        CHECK(psrt_array_builder_append(&values, &i, 1) == PS_OK);
    CHECK(tracker.attempts <= 8 && psrt_array_count(&values) == 512);
    const int64_t *numbers = psrt_array_data(&values);
    for (int64_t i = 0; i < 512; i++)
        CHECK(numbers[i] == i);
    CHECK(psrt_array_clone(&values, &snapshot) == PS_OK);
    int64_t extra = 512;
    CHECK(psrt_array_builder_append(&values, &extra, 1) == PS_INVALID);
    CHECK(psrt_array_builder_append(&values, NULL, 0) == PS_INVALID);
    CHECK(psrt_array_count(&snapshot) == 512 && psrt_array_data(&snapshot) == numbers);
    psrt_array_destroy(&snapshot);
    CHECK(psrt_array_builder_append(&values, &extra, 1) == PS_OK);
    CHECK(psrt_array_builder_append(&values, &extra, 1) == PS_LIMIT);
    psrt_array_destroy(&values);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);

    test_allocator budget = {0};
    budget.budget = sizeof(psrt_array_block) + sizeof(int64_t);
    CHECK(psrt_array_init(&integers, test_domain(&budget), 8, &values) == PS_OK);
    CHECK(psrt_array_builder_append(&values, &extra, 1) == PS_OK);
    CHECK(psrt_array_count(&values) == 1);
    CHECK(psrt_array_builder_append(&values, &extra, 1) == PS_MEMORY);
    CHECK(psrt_array_count(&values) == 1 && *(const int64_t *)psrt_array_data(&values) == 512);
    psrt_array_destroy(&values);
    CHECK(!budget.live_blocks && !budget.live_bytes && !budget.invalid);

    copier_tracker = (test_allocator){0};
    int64_t raw[] = {10, 20, 30, 40};
    owned source[] = {{raw}, {raw + 1}, {raw + 2}, {raw + 3}};
    CHECK(psrt_array_init(&owning, test_domain(&copier_tracker), 8, &values) == PS_OK);
    CHECK(psrt_array_builder_append(&values, source, 2) == PS_OK);
    size_t bytes = copier_tracker.live_bytes;
    const void *before = psrt_array_data(&values);
    copier_tracker.fail_on = copier_tracker.attempts + 2;
    CHECK(psrt_array_builder_append(&values, source + 2, 2) == PS_MEMORY);
    CHECK(psrt_array_count(&values) == 2 && psrt_array_data(&values) == before);
    CHECK(copier_tracker.live_bytes == bytes);
    copier_tracker.fail_on = 0;
    CHECK(psrt_array_builder_append(&values, source + 2, 2) == PS_OK);
    bytes = copier_tracker.live_bytes;
    before = psrt_array_data(&values);
    copier_tracker.fail_on = copier_tracker.attempts + 3;
    CHECK(psrt_array_builder_append(&values, source, 1) == PS_MEMORY);
    CHECK(psrt_array_count(&values) == 4 && psrt_array_data(&values) == before);
    CHECK(copier_tracker.live_bytes == bytes);
    copier_tracker.fail_on = 0;
    CHECK(psrt_array_builder_append(&values, psrt_array_data(&values), 4) == PS_OK);
    CHECK(psrt_array_count(&values) == 8);
    const owned *copied = psrt_array_data(&values);
    for (size_t i = 0; i < 8; i++)
        CHECK(*copied[i].value == raw[i % 4]);
    psrt_array_destroy(&values);
    CHECK(!copier_tracker.live_blocks && !copier_tracker.live_bytes && !copier_tracker.invalid);
    return 0;
}
static int failures(void) {
    psrt_array array, snapshot;
    CHECK(psrt_array_init(&owning, test_domain(&copier_tracker), 10, &array) == PS_OK);
    int64_t values[] = {1, 2, 3};
    owned source[] = {{values}, {values + 1}, {values + 2}};
    CHECK(psrt_array_replace(&array, 0, 0, source, 3) == PS_OK);
    CHECK(psrt_array_clone(&array, &snapshot) == PS_OK);
    size_t bytes = copier_tracker.live_bytes, blocks = copier_tracker.live_blocks;
    const void *old = psrt_array_data(&array);
    /* Fail the block allocation, then each element construction in turn. */
    for (size_t failure = 1; failure <= 5; failure++) {
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_replace(&array, 1, 1, source, 2) == PS_MEMORY);
        CHECK(psrt_array_data(&array) == old && psrt_array_count(&array) == 3);
        CHECK(array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
        for (size_t i = 0; i < 3; i++)
            CHECK(*((const owned *)old)[i].value == (int64_t)i + 1);
    }
    copier_tracker.fail_on = 0;
    for (size_t failure = 1; failure <= 3; failure++) {
        psrt_array slice = {0};
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_slice_strided_bounds(&array, 0, 0, 0, 0, 0, 2, &slice) == PS_MEMORY);
        CHECK(!slice.type && !slice.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    CHECK(psrt_array_select_edge(&array, -1, false, false, &array) == PS_INVALID);
    psrt_array edge = {0};
    CHECK(psrt_array_select_edge(&array, -1, false, false, &edge) == PS_LIMIT);
    for (size_t failure = 1; failure <= 3; failure++) {
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_select_edge(&array, 2, false, false, &edge) == PS_MEMORY);
        CHECK(!edge.type && !edge.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    for (int drop = 0; drop <= 1; drop++) {
        for (int last = 0; last <= 1; last++) {
            CHECK(psrt_array_select_edge(&array, 1, drop != 0, last != 0, &edge) == PS_OK);
            const owned *selected = psrt_array_data(&edge);
            CHECK(psrt_array_count(&edge) == (drop ? 2u : 1u));
            CHECK(*selected[0].value == (drop && !last ? 2 : !drop && last ? 3 : 1));
            if (drop)
                CHECK(*selected[1].value == (last ? 2 : 3));
            CHECK(edge.block != array.block);
            psrt_array_destroy(&edge);
            CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
        }
    }
    for (size_t failure = 1; failure <= 4; failure++) {
        psrt_array reversed = {0};
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_reversed(&array, &reversed) == PS_MEMORY);
        CHECK(!reversed.type && !reversed.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array reversed = {0};
    CHECK(psrt_array_reversed(&array, &reversed) == PS_OK);
    CHECK(reversed.block != array.block && psrt_array_count(&reversed) == 3);
    const owned *reversed_values = psrt_array_data(&reversed);
    CHECK(*reversed_values[0].value == 3 && *reversed_values[1].value == 2 &&
          *reversed_values[2].value == 1);
    psrt_array_destroy(&reversed);
    for (size_t failure = 1; failure <= 7; failure++) {
        psrt_array repeated = {0};
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_repeated(&array, 2, &repeated) == PS_MEMORY);
        CHECK(!repeated.type && !repeated.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array repeated = {0};
    CHECK(psrt_array_repeated(&array, 2, &repeated) == PS_OK);
    CHECK(repeated.block != array.block && psrt_array_count(&repeated) == 6);
    const owned *repeated_values = psrt_array_data(&repeated);
    for (size_t i = 0; i < 6; i++)
        CHECK(*repeated_values[i].value == (int64_t)(i % 3) + 1);
    psrt_array_destroy(&repeated);
    CHECK(psrt_array_repeated(&array, 0, &repeated) == PS_OK);
    CHECK(repeated.type == array.type && !repeated.block);
    psrt_array_destroy(&repeated);
    CHECK(psrt_array_repeated(&array, 1, &repeated) == PS_OK);
    CHECK(repeated.block == array.block && array.block->value.references == 3);
    psrt_array_destroy(&repeated);
    CHECK(psrt_array_repeated(&array, -1, &repeated) == PS_INVALID);
    CHECK(psrt_array_repeated(&array, INT64_MAX, &repeated) == PS_LIMIT);
    CHECK(psrt_array_repeated(&array, 2, &array) == PS_INVALID);
    CHECK(!repeated.type && !repeated.block);
    for (size_t failure = 1; failure <= 4; failure++) {
        psrt_array sorted = {0};
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_sorted(&array, compare_owned_descending, NULL, &sorted) == PS_MEMORY);
        CHECK(!sorted.type && !sorted.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array sorted = {0};
    CHECK(psrt_array_sorted(&array, compare_owned_descending, NULL, &sorted) == PS_OK);
    const owned *sorted_values = psrt_array_data(&sorted);
    CHECK(*sorted_values[0].value == 3 && *sorted_values[1].value == 2 &&
          *sorted_values[2].value == 1);
    psrt_array_destroy(&sorted);
    for (size_t failure = 1; failure <= 4; failure++) {
        psrt_array copied = {0};
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_copy_elements(&array, &copied) == PS_MEMORY);
        CHECK(!copied.type && !copied.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array copied = {0};
    CHECK(psrt_array_copy_elements(&array, &copied) == PS_OK);
    CHECK(copied.block != array.block && copied.block != snapshot.block);
    const owned *copied_values = psrt_array_data(&copied);
    for (size_t i = 0; i < 3; i++)
        CHECK(copied_values[i].value != ((const owned *)old)[i].value &&
              *copied_values[i].value == (int64_t)i + 1);
    psrt_sort_scratch scratch = {0};
    size_t copied_bytes = copier_tracker.live_bytes;
    copier_tracker.fail_on = copier_tracker.attempts + 1;
    CHECK(psrt_sort_scratch_init(&scratch, test_domain(&copier_tracker), 3,
                                 sizeof(owned)) == PS_MEMORY);
    CHECK(!scratch.data && copier_tracker.live_bytes == copied_bytes);
    copier_tracker.fail_on = 0;
    CHECK(psrt_sort_scratch_init(&scratch, test_domain(&copier_tracker), 3,
                                 sizeof(owned)) == PS_OK);
    CHECK(scratch.data && scratch.bytes == 3 * sizeof(owned));
    psrt_sort_scratch_destroy(&scratch);
    CHECK(!scratch.data && copier_tracker.live_bytes == copied_bytes);
    psrt_array_destroy(&copied);
    CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    const unsigned char keep[] = {1, 0, 1};
    for (size_t failure = 1; failure <= 3; failure++) {
        psrt_array selected = {0};
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_select_mask(&array, keep, &selected) == PS_MEMORY);
        CHECK(!selected.type && !selected.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array selected = {0};
    CHECK(psrt_array_select_mask(&array, keep, &selected) == PS_OK);
    CHECK(psrt_array_count(&selected) == 2 && selected.block != array.block);
    const owned *selected_values = psrt_array_data(&selected);
    CHECK(*selected_values[0].value == 1 && *selected_values[1].value == 3);
    psrt_array_destroy(&selected);
    CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    CHECK(psrt_array_insert_contents(&array, -1, &array) == PS_LIMIT);
    CHECK(psrt_array_insert_contents(&array, 4, &array) == PS_LIMIT);
    for (size_t failure = 1; failure <= 7; failure++) {
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_insert_contents(&array, 1, &array) == PS_MEMORY);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array inserted = {0};
    CHECK(psrt_array_clone(&array, &inserted) == PS_OK);
    CHECK(psrt_array_insert_contents(&inserted, 1, &inserted) == PS_OK);
    CHECK(psrt_array_count(&inserted) == 6 && psrt_array_data(&inserted) != old);
    const owned *inserted_values = psrt_array_data(&inserted);
    const int64_t expected_inserted[] = {1, 1, 2, 3, 2, 3};
    for (size_t i = 0; i < 6; i++)
        CHECK(*inserted_values[i].value == expected_inserted[i]);
    psrt_array_destroy(&inserted);
    CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
    CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    CHECK(psrt_array_swap_at(&array, -1, 0) == PS_LIMIT);
    CHECK(psrt_array_swap_at(&array, 0, 3) == PS_LIMIT);
    CHECK(psrt_array_swap_at(&array, 1, 1) == PS_OK);
    CHECK(psrt_array_data(&array) == old);
    for (size_t failure = 1; failure <= 4; failure++) {
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_swap_at(&array, 0, 2) == PS_MEMORY);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array swapped = {0};
    CHECK(psrt_array_clone(&array, &swapped) == PS_OK);
    CHECK(psrt_array_swap_at(&swapped, 0, 2) == PS_OK);
    const owned *swapped_values = psrt_array_data(&swapped);
    CHECK(*swapped_values[0].value == 3 && *swapped_values[1].value == 2 &&
          *swapped_values[2].value == 1);
    CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
    psrt_array_destroy(&swapped);
    CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    size_t attempts_before_empty = copier_tracker.attempts;
    CHECK(psrt_array_remove_edge_count(&array, 0, false) == PS_OK);
    CHECK(psrt_array_remove_edge_count(&array, 0, true) == PS_OK);
    CHECK(copier_tracker.attempts == attempts_before_empty && psrt_array_data(&array) == old);
    CHECK(psrt_array_remove_edge_count(&array, -1, false) == PS_LIMIT);
    CHECK(psrt_array_remove_edge_count(&array, 4, true) == PS_LIMIT);
    for (int last = 0; last <= 1; last++) {
        for (size_t failure = 1; failure <= 3; failure++) {
            copier_tracker.fail_on = copier_tracker.attempts + failure;
            CHECK(psrt_array_remove_edge_count(&array, 1, last != 0) == PS_MEMORY);
            CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
            CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
        }
    }
    copier_tracker.fail_on = 0;
    for (int last = 0; last <= 1; last++) {
        psrt_array shortened = {0};
        CHECK(psrt_array_clone(&array, &shortened) == PS_OK);
        CHECK(psrt_array_remove_edge_count(&shortened, 1, last != 0) == PS_OK);
        CHECK(psrt_array_count(&shortened) == 2);
        const owned *shortened_values = psrt_array_data(&shortened);
        CHECK(*shortened_values[0].value == (last ? 1 : 2));
        CHECK(*shortened_values[1].value == (last ? 2 : 3));
        psrt_array_destroy(&shortened);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    psrt_array mutable_copy = {0};
    CHECK(psrt_array_clone(&array, &mutable_copy) == PS_OK);
    for (size_t failure = 1; failure <= 4; failure++) {
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_reverse(&mutable_copy) == PS_MEMORY);
        CHECK(psrt_array_data(&mutable_copy) == old && array.block->value.references == 3);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    CHECK(psrt_array_reverse(&mutable_copy) == PS_OK);
    CHECK(psrt_array_data(&mutable_copy) != old && array.block->value.references == 2);
    const owned *mutated_values = psrt_array_data(&mutable_copy);
    CHECK(*mutated_values[0].value == 3 && *mutated_values[1].value == 2 &&
          *mutated_values[2].value == 1);
    psrt_array_destroy(&mutable_copy);
    CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    psrt_array scattered = {0};
    CHECK(psrt_array_slice_strided_bounds(&snapshot, 0, 0, 0, 0, 0, 2, &scattered) == PS_OK);
    size_t scatter_bytes = copier_tracker.live_bytes, scatter_blocks = copier_tracker.live_blocks;
    for (size_t failure = 1; failure <= 4; failure++) {
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_scatter(&array, 0, 3, 2, &scattered) == PS_MEMORY);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == scatter_bytes &&
              copier_tracker.live_blocks == scatter_blocks);
    }
    copier_tracker.fail_on = 0;
    psrt_array_destroy(&scattered);
    for (size_t failure = 1; failure <= 4; failure++) {
        psrt_array slice = {0};
        copier_tracker.fail_on = copier_tracker.attempts + failure;
        CHECK(psrt_array_slice(&array, 0, 3, 0, &slice) == PS_MEMORY);
        CHECK(!slice.type && !slice.block);
        CHECK(psrt_array_data(&array) == old && array.block->value.references == 2);
        CHECK(copier_tracker.live_bytes == bytes && copier_tracker.live_blocks == blocks);
    }
    copier_tracker.fail_on = 0;
    CHECK(psrt_array_replace(&array, 1, 1, source, 2) == PS_OK);
    CHECK(psrt_array_count(&array) == 4 && psrt_array_count(&snapshot) == 3);
    psrt_array_destroy(&array);
    psrt_array_destroy(&snapshot);
    CHECK(!copier_tracker.live_bytes && !copier_tracker.live_blocks && !copier_tracker.invalid);
    return 0;
}

static int bounds(void) {
    test_allocator tracker = {0};
    psrt_array array = {0};
    CHECK(psrt_array_init(&integers, test_domain(&tracker), SIZE_MAX, &array) == PS_LIMIT);
    CHECK(!array.type && !tracker.attempts);
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 2, &array) == PS_OK);
    int64_t values[] = {1, 2, 3};
    CHECK(psrt_array_replace(&array, 0, 0, values, 3) == PS_LIMIT);
    CHECK(psrt_array_replace(&array, 1, 0, values, 1) == PS_LIMIT);
    CHECK(psrt_array_replace(&array, 0, 1, NULL, 0) == PS_LIMIT);
    CHECK(psrt_array_replace(&array, 0, 0, values, SIZE_MAX) == PS_LIMIT);
    CHECK(!tracker.attempts);
    CHECK(psrt_array_replace(&array, 0, 0, values, 2) == PS_OK);
    const void *old = psrt_array_data(&array);
    CHECK(psrt_array_insert(&array, -1, values + 2) == PS_LIMIT);
    CHECK(psrt_array_insert(&array, 3, values + 2) == PS_LIMIT);
    CHECK(psrt_array_insert(&array, 1, values + 2) == PS_LIMIT);
    CHECK(psrt_array_data(&array) == old && psrt_array_count(&array) == 2);
    CHECK(psrt_array_replace(&array, 0, SIZE_MAX, NULL, 0) == PS_LIMIT);
    CHECK(psrt_array_replace(&array, SIZE_MAX, 0, values, 1) == PS_LIMIT);
    CHECK(psrt_array_data(&array) == old);
    tracker.budget = tracker.live_bytes;
    CHECK(psrt_array_replace(&array, 0, 1, values + 2, 1) == PS_MEMORY);
    CHECK(psrt_array_data(&array) == old && *(const int64_t *)old == 1);
    tracker.budget = 0;
    array.block->value.references = SIZE_MAX;
    psrt_array clone = {0};
    CHECK(psrt_array_clone(&array, &clone) == PS_LIMIT && !clone.type);
    array.block->value.references = 1;
    psrt_array_destroy(&array);
    CHECK(!tracker.live_bytes && !tracker.invalid);
    return 0;
}

static int model_operations(void) {
    test_allocator tracker = {0};
    psrt_array array;
    CHECK(psrt_array_init(&integers, test_domain(&tracker), 64, &array) == PS_OK);
    int64_t reference[64] = {0};
    size_t length = 0;
    ps_rng rng;
    ps_rng_seed(&rng, 493);
    for (unsigned step = 0; step < 2000; step++) {
        psrt_array snapshot;
        CHECK(psrt_array_clone(&array, &snapshot) == PS_OK);
        size_t index = (size_t)(ps_rng_uniform(&rng) * (length + 1));
        size_t removed = (size_t)(ps_rng_uniform(&rng) * (length - index + 1));
        size_t added = (size_t)(ps_rng_uniform(&rng) * (65 - length + removed));
        int64_t input[64];
        for (size_t i = 0; i < added; i++)
            input[i] = (int64_t)step * 100 + (int64_t)i;
        CHECK(psrt_array_replace(&array, index, removed, input, added) == PS_OK);
        CHECK(psrt_array_count(&snapshot) == length);
        CHECK(!length || !memcmp(psrt_array_data(&snapshot), reference, length * sizeof(int64_t)));
        memmove(reference + index + added, reference + index + removed,
                (length - index - removed) * sizeof(int64_t));
        memcpy(reference + index, input, added * sizeof(int64_t));
        length = length - removed + added;
        CHECK(psrt_array_count(&array) == length);
        CHECK(!length || !memcmp(psrt_array_data(&array), reference, length * sizeof(int64_t)));
        psrt_array_destroy(&snapshot);
    }
    psrt_array_destroy(&array);
    CHECK(!tracker.live_blocks && !tracker.live_bytes && !tracker.invalid);
    return 0;
}

static test_allocator unwind_tracker;
static int destroyed[4], destroyed_count;
static void record_destroy(void *object) { destroyed[destroyed_count++] = *(int *)object; }
static int scope_marks(void) {
    psrt_trap trap = {0};
    psrt_current = &trap;
    int first = 1, second = 2, third = 3;
    psrt_cleanup a, b, c;
    psrt_cleanup_push(&a, &first, record_destroy);
    psrt_cleanup *mark = psrt_cleanups;
    psrt_cleanup_push(&b, &second, record_destroy);
    psrt_cleanup_unwind(mark);
    CHECK(destroyed_count == 1 && destroyed[0] == 2 && psrt_cleanups == &a);
    psrt_cleanup_push(&c, &third, record_destroy);
    psrt_cleanup_unwind(mark);
    CHECK(destroyed_count == 2 && destroyed[1] == 3 && psrt_cleanups == &a);
    psrt_cleanup_unwind(NULL);
    CHECK(destroyed_count == 3 && destroyed[2] == 1 && !psrt_cleanups);
    destroyed_count = 0;
    psrt_current = NULL;
    return 0;
}
static void fail_in_inner_function(void) {
    psrt_array local;
    if (psrt_array_init(&integers, test_domain(&unwind_tracker), 10, &local) != PS_OK)
        abort();
    psrt_cleanup owner;
    psrt_cleanup_push(&owner, &local, psrt_array_destroy);
    int64_t value = 11;
    if (psrt_array_replace(&local, 0, 0, &value, 1) != PS_OK)
        abort();
    int first = 1, second = 2;
    psrt_cleanup a, b;
    psrt_cleanup_push(&a, &first, record_destroy);
    psrt_cleanup_push(&b, &second, record_destroy);
    psrt_fail(PSRT_AT(42, 7), "array failure probe");
}
static int unwinding(void) {
    /* Static trap objects avoid indeterminate modified automatic values after longjmp. */
    static psrt_trap outer, inner;
    static char error[256];
    psrt_current = &outer;
    int outer_value = 3;
    psrt_cleanup retained;
    psrt_cleanup_push(&retained, &outer_value, record_destroy);
    inner.previous = &outer;
    inner.error = error;
    inner.capacity = sizeof error;
    psrt_current = &inner;
    if (!setjmp(inner.jump))
        fail_in_inner_function();
    CHECK(!inner.cleanup && destroyed_count == 2 && destroyed[0] == 2 && destroyed[1] == 1);
    CHECK(!unwind_tracker.live_bytes && !unwind_tracker.live_blocks && !unwind_tracker.invalid);
    CHECK(strstr(error, "array-values.phys:42:7: runtime error: array failure probe"));
    psrt_current = inner.previous;
    CHECK(outer.cleanup == &retained);
    psrt_cleanup_unwind(NULL);
    CHECK(destroyed_count == 3 && destroyed[2] == 3 && !outer.cleanup);
    psrt_current = NULL;
    return 0;
}

int main(void) {
    CHECK(split_values() == 0);
    CHECK(sorting_numbers() == 0);
    CHECK(snapshots() == 0);
    CHECK(slices() == 0);
    CHECK(nested() == 0);
    CHECK(nested_rollback() == 0);
    CHECK(failures() == 0);
    CHECK(builder_appends() == 0);
    CHECK(bounds() == 0);
    CHECK(model_operations() == 0);
    CHECK(scope_marks() == 0);
    CHECK(unwinding() == 0);
    puts(
        "Language array ownership: snapshots, nested values, rollback and pre-jump cleanup passed");
    return 0;
}
