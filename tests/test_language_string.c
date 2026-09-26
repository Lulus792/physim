#include "physim/language_string.h"
#include "test_allocator.h"
#include <stdio.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language string line %d: %s\n", __LINE__, #x);                       \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static ps_result copy_string_element(void *destination, const void *source) {
    return psrt_string_clone(source, destination);
}
static const psrt_element_type scalar_element_type = {
    sizeof(psrt_string), copy_string_element, psrt_string_destroy
};

int main(void) {
    test_allocator tracker = {0};
    ps_allocator allocator = test_domain(&tracker);
    psrt_string empty, word, copy, punctuation, joined;
    CHECK(psrt_string_make(allocator, NULL, 0, &empty) == PS_OK);
    CHECK(psrt_string_valid(&empty) && psrt_string_byte_count(&empty) == 0);
    CHECK(!strcmp(psrt_string_cstr(&empty), ""));
    CHECK(psrt_string_make(allocator, "Grüße", strlen("Grüße"), &word) == PS_OK);
    CHECK(psrt_string_byte_count(&word) == strlen("Grüße"));
    CHECK(psrt_string_clone(&word, &copy) == PS_OK);
    CHECK(psrt_array_data(&word) == psrt_array_data(&copy));
    CHECK(psrt_string_make(allocator, "!", 1, &punctuation) == PS_OK);
    CHECK(psrt_string_concat(&word, &punctuation, allocator, &joined) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&joined), "Grüße!"));
    CHECK(psrt_string_byte_count(&joined) == strlen("Grüße!"));
    psrt_string repeated = {0};
    size_t repeat_live_before = tracker.live_bytes;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(psrt_string_repeat(&word, 3, allocator, &repeated) == PS_MEMORY);
    CHECK(!repeated.type && !repeated.block && tracker.live_bytes == repeat_live_before);
    tracker.fail_on = 0;
    CHECK(psrt_string_repeat(&word, 3, allocator, &repeated) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&repeated), "GrüßeGrüßeGrüße"));
    CHECK(psrt_string_scalar_count(&repeated) == 15);
    psrt_string_destroy(&repeated);
    CHECK(psrt_string_repeat(&word, 0, allocator, &repeated) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&repeated), ""));
    psrt_string_destroy(&repeated);
    CHECK(psrt_string_repeat(&word, 1, allocator, &repeated) == PS_OK);
    CHECK(psrt_array_data(&word) == psrt_array_data(&repeated));
    psrt_string_destroy(&repeated);
    CHECK(psrt_string_repeat(&word, -1, allocator, &repeated) == PS_INVALID);
    CHECK(psrt_string_repeat(&word, INT64_MAX, allocator, &repeated) == PS_LIMIT);
    CHECK(psrt_string_repeat(&word, 2, allocator, &word) == PS_INVALID);
    CHECK(!repeated.type && !repeated.block &&
          !strcmp(psrt_string_cstr(&word), "Grüße"));
    psrt_string padded, trimmed = {0};
    const char *padded_text = "\xC2\xA0\tGrüße \xE3\x80\x80";
    CHECK(psrt_string_make(allocator, padded_text, strlen(padded_text), &padded) == PS_OK);
    size_t trim_live_before = tracker.live_bytes;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(psrt_string_trimmed(&padded, allocator, &trimmed) == PS_MEMORY);
    CHECK(!trimmed.type && !trimmed.block && tracker.live_bytes == trim_live_before);
    tracker.fail_on = 0;
    CHECK(psrt_string_trimmed(&padded, allocator, &trimmed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&trimmed), "Grüße"));
    CHECK(!strcmp(psrt_string_cstr(&padded), padded_text));
    psrt_string_destroy(&trimmed);
    CHECK(psrt_string_trimmed(&word, allocator, &trimmed) == PS_OK);
    CHECK(psrt_array_data(&word) == psrt_array_data(&trimmed));
    psrt_string_destroy(&trimmed);
    CHECK(psrt_string_trimmed(&padded, allocator, &padded) == PS_INVALID);
    psrt_string_destroy(&padded);
    psrt_string white;
    const char *white_text = "\xC2\x85\xE2\x80\xA8\xE3\x80\x80";
    CHECK(psrt_string_make(allocator, white_text, strlen(white_text), &white) == PS_OK);
    CHECK(psrt_string_trimmed(&white, allocator, &trimmed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&trimmed), ""));
    psrt_string_destroy(&trimmed);
    psrt_string_destroy(&white);
    CHECK(!psrt_string_white_space(0x200Bu) && !psrt_string_white_space(0xFEFFu));
    psrt_string reversed = {0};
    size_t reverse_live_before = tracker.live_bytes;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(psrt_string_reversed(&word, allocator, &reversed) == PS_MEMORY);
    CHECK(!reversed.type && !reversed.block && tracker.live_bytes == reverse_live_before);
    tracker.fail_on = 0;
    CHECK(psrt_string_reversed(&word, allocator, &reversed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&reversed), "eßürG"));
    CHECK(!strcmp(psrt_string_cstr(&word), "Grüße"));
    psrt_string_destroy(&reversed);
    CHECK(psrt_string_reversed(&empty, allocator, &reversed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&reversed), ""));
    psrt_string_destroy(&reversed);
    psrt_string combining;
    CHECK(psrt_string_make(allocator, "\x61\xCC\x81", 3, &combining) == PS_OK);
    CHECK(psrt_string_reversed(&combining, allocator, &reversed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&reversed), "\xCC\x81\x61"));
    psrt_string_destroy(&reversed);
    psrt_string_destroy(&combining);
    CHECK(psrt_string_reversed(&word, allocator, &word) == PS_INVALID);
    CHECK(psrt_string_scalar_count(&empty) == 0);
    CHECK(psrt_string_scalar_count(&word) == 5);
    psrt_string edge = {0};
    CHECK(psrt_string_select_edge(&word, -1, false, false, allocator, &edge) == PS_LIMIT);
    CHECK(psrt_string_select_edge(&word, 1, false, false, allocator, &word) == PS_INVALID);
    size_t edge_live_before = tracker.live_bytes;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(psrt_string_select_edge(&word, 2, false, false, allocator, &edge) == PS_MEMORY);
    CHECK(!edge.type && !edge.block && tracker.live_bytes == edge_live_before);
    tracker.fail_on = 0;
    const char *expected_edges[] = {"Gr", "ße", "üße", "Grü"};
    for (int drop = 0; drop <= 1; drop++) {
        for (int last = 0; last <= 1; last++) {
            CHECK(psrt_string_select_edge(&word, 2, drop != 0, last != 0,
                                          allocator, &edge) == PS_OK);
            CHECK(!strcmp(psrt_string_cstr(&edge), expected_edges[drop * 2 + last]));
            CHECK(psrt_array_data(&edge) != psrt_array_data(&word));
            psrt_string_destroy(&edge);
            CHECK(tracker.live_bytes == edge_live_before);
        }
    }
    CHECK(psrt_string_select_edge(&word, INT64_MAX, false, true, allocator, &edge) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&edge), "Grüße"));
    psrt_string_destroy(&edge);
    CHECK(psrt_string_select_edge(&empty, 1, true, false, allocator, &edge) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&edge), ""));
    psrt_string_destroy(&edge);
    size_t cursor = 0;
    psrt_string scalar = {0};
    size_t scalar_live_before = tracker.live_bytes, scalar_blocks_before = tracker.live_blocks;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(psrt_string_next_scalar(&word, &cursor, allocator, &scalar) == PS_MEMORY);
    CHECK(cursor == 0 && !scalar.block && !scalar.type);
    CHECK(tracker.live_bytes == scalar_live_before && tracker.live_blocks == scalar_blocks_before);
    tracker.fail_on = 0;
    const char *expected_scalars[] = {"G", "r", "ü", "ß", "e"};
    for (size_t i = 0; i < 5; i++) {
        CHECK(psrt_string_next_scalar(&word, &cursor, allocator, &scalar) == PS_OK);
        CHECK(!strcmp(psrt_string_cstr(&scalar), expected_scalars[i]));
        psrt_string_destroy(&scalar);
    }
    CHECK(cursor == psrt_string_byte_count(&word));
    CHECK(psrt_string_next_scalar(&word, &cursor, allocator, &scalar) == PS_LIMIT);
    cursor = 3; /* A UTF-8 continuation byte in "Grüße". */
    CHECK(psrt_string_next_scalar(&word, &cursor, allocator, &scalar) == PS_LIMIT);
    CHECK(cursor == 3 && !scalar.block);
    cursor = psrt_string_byte_count(&word);
    scalar_live_before = tracker.live_bytes;
    scalar_blocks_before = tracker.live_blocks;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(psrt_string_previous_scalar(&word, &cursor, allocator, &scalar) == PS_MEMORY);
    CHECK(cursor == psrt_string_byte_count(&word) && !scalar.block && !scalar.type);
    CHECK(tracker.live_bytes == scalar_live_before && tracker.live_blocks == scalar_blocks_before);
    tracker.fail_on = 0;
    const char *expected_reverse_scalars[] = {"e", "ß", "ü", "r", "G"};
    for (size_t i = 0; i < 5; i++) {
        CHECK(psrt_string_previous_scalar(&word, &cursor, allocator, &scalar) == PS_OK);
        CHECK(!strcmp(psrt_string_cstr(&scalar), expected_reverse_scalars[i]));
        psrt_string_destroy(&scalar);
    }
    CHECK(cursor == 0);
    CHECK(psrt_string_previous_scalar(&word, &cursor, allocator, &scalar) == PS_LIMIT);
    cursor = 3;
    CHECK(psrt_string_previous_scalar(&word, &cursor, allocator, &scalar) == PS_LIMIT);
    CHECK(cursor == 3 && !scalar.block);
    cursor = psrt_string_byte_count(&word) + 1;
    CHECK(psrt_string_previous_scalar(&word, &cursor, allocator, &scalar) == PS_LIMIT);
    CHECK(psrt_string_previous_scalar(&word, &cursor, allocator, &word) == PS_INVALID);
    psrt_array scalar_array = {0};
    size_t scalar_array_attempts_before = tracker.attempts;
    CHECK(psrt_string_scalars(&word, &scalar_element_type, allocator, &scalar_array) == PS_OK);
    size_t scalar_array_attempts = tracker.attempts - scalar_array_attempts_before;
    CHECK(psrt_array_count(&scalar_array) == 5);
    for (size_t i = 0; i < 5; i++) {
        const psrt_string *item = (const psrt_string *)psrt_array_data(&scalar_array) + i;
        CHECK(!strcmp(psrt_string_cstr(item), expected_scalars[i]));
    }
    psrt_array_destroy(&scalar_array);
    CHECK(psrt_string_scalars(&empty, &scalar_element_type, allocator, &scalar_array) == PS_OK);
    CHECK(psrt_array_count(&scalar_array) == 0);
    psrt_array_destroy(&scalar_array);
    CHECK(psrt_string_scalars(&word, &scalar_element_type, allocator, &word) == PS_INVALID);
    size_t scalar_array_failures = 0;
    for (size_t offset = 1; offset <= scalar_array_attempts; offset++) {
        size_t live_bytes = tracker.live_bytes, live_blocks = tracker.live_blocks;
        tracker.fail_on = tracker.attempts + offset;
        ps_result status = psrt_string_scalars(&word, &scalar_element_type, allocator,
                                               &scalar_array);
        CHECK(status == PS_MEMORY || status == PS_OK);
        if (status == PS_MEMORY) {
            scalar_array_failures++;
            CHECK(!scalar_array.type && !scalar_array.block);
        } else {
            CHECK(psrt_array_count(&scalar_array) == 5);
            psrt_array_destroy(&scalar_array);
        }
        CHECK(tracker.live_bytes == live_bytes && tracker.live_blocks == live_blocks);
        tracker.fail_on = 0;
    }
    CHECK(scalar_array_failures != 0);
    psrt_string slice;
    CHECK(psrt_string_slice_bounds(&word, 1, 2, 1, 3, 1, 1, allocator, &slice) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&slice), "üß"));
    psrt_string_destroy(&slice);
    CHECK(psrt_string_slice_bounds(&word, 0, 0, 0, 0, 0, 2, allocator, &slice) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&slice), "Güe"));
    psrt_string_destroy(&slice);
    CHECK(psrt_string_slice_bounds(&word, 1, 5, 0, 0, 1, 1, allocator, &slice) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&slice), ""));
    psrt_string_destroy(&slice);
    CHECK(psrt_string_slice_bounds(&empty, 0, 0, 0, 0, 1, 1, allocator, &slice) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&slice), ""));
    psrt_string_destroy(&slice);
    CHECK(psrt_string_slice_bounds(&word, 0, 0, 0, 0, 0, INT64_MAX, allocator, &slice) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&slice), "G"));
    psrt_string_destroy(&slice);
    CHECK(psrt_string_slice_bounds(&word, 1, -1, 1, 2, 0, 1, allocator, &slice) == PS_LIMIT);
    CHECK(psrt_string_slice_bounds(&word, 1, 5, 1, 5, 1, 1, allocator, &slice) == PS_LIMIT);
    CHECK(psrt_string_slice_bounds(&word, 0, 0, 0, 0, 0, 0, allocator, &slice) == PS_LIMIT);
    psrt_string needle;
    bool found = false;
    int64_t position = -1;
    CHECK(psrt_string_make(allocator, "üß", strlen("üß"), &needle) == PS_OK);
    CHECK(psrt_string_find(&word, &needle, &found, &position) == PS_OK);
    CHECK(found && position == 2);
    CHECK(psrt_string_find_last(&word, &needle, &found, &position) == PS_OK);
    CHECK(found && position == 2);
    CHECK(psrt_string_matches_edge(&word, &needle, false, &found) == PS_OK);
    CHECK(!found);
    CHECK(psrt_string_matches_edge(&word, &needle, true, &found) == PS_OK);
    CHECK(!found);
    psrt_string_destroy(&needle);
    CHECK(psrt_string_find(&word, &empty, &found, &position) == PS_OK);
    CHECK(found && position == 0);
    CHECK(psrt_string_find_last(&word, &empty, &found, &position) == PS_OK);
    CHECK(found && position == 5);
    CHECK(psrt_string_matches_edge(&word, &empty, false, &found) == PS_OK && found);
    CHECK(psrt_string_matches_edge(&word, &empty, true, &found) == PS_OK && found);
    CHECK(psrt_string_make(allocator, "xyz", 3, &needle) == PS_OK);
    position = 77;
    CHECK(psrt_string_find(&word, &needle, &found, &position) == PS_OK);
    CHECK(!found && position == 77);
    CHECK(psrt_string_find_last(&word, &needle, &found, &position) == PS_OK);
    CHECK(!found && position == 77);
    CHECK(psrt_string_matches_edge(&word, &needle, false, &found) == PS_OK && !found);
    CHECK(psrt_string_matches_edge(&word, &needle, true, &found) == PS_OK && !found);
    CHECK(psrt_string_find(&word, NULL, &found, &position) == PS_INVALID);
    CHECK(psrt_string_find_last(&word, NULL, &found, &position) == PS_INVALID);
    CHECK(psrt_string_matches_edge(&word, NULL, false, &found) == PS_INVALID);
    psrt_string_destroy(&needle);
    psrt_string search, replacement, changed;
    CHECK(psrt_string_make(allocator, "üß", strlen("üß"), &search) == PS_OK);
    CHECK(psrt_string_make(allocator, "U", 1, &replacement) == PS_OK);
    CHECK(psrt_string_replace(&word, &search, &replacement, allocator, &changed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&changed), "GrUe"));
    CHECK(!strcmp(psrt_string_cstr(&word), "Grüße"));
    psrt_string_destroy(&changed);
    CHECK(psrt_string_replace(&word, &search, &replacement, allocator, &word) == PS_INVALID);
    CHECK(psrt_string_replace(&word, NULL, &replacement, allocator, &changed) == PS_INVALID);
    CHECK(psrt_string_replace(&word, &search, &replacement, (ps_allocator){0}, &changed) == PS_INVALID);
    psrt_string_destroy(&search);
    psrt_string_destroy(&replacement);
    CHECK(psrt_string_make(allocator, ".", 1, &replacement) == PS_OK);
    CHECK(psrt_string_replace(&word, &empty, &replacement, allocator, &changed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&changed), ".G.r.ü.ß.e."));
    psrt_string_destroy(&changed);
    CHECK(psrt_string_replace(&empty, &empty, &replacement, allocator, &changed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&changed), "."));
    psrt_string_destroy(&changed);
    CHECK(psrt_string_replace(&word, &empty, &empty, allocator, &changed) == PS_OK);
    CHECK(psrt_array_data(&word) == psrt_array_data(&changed));
    psrt_string_destroy(&changed);
    CHECK(psrt_string_make(allocator, "xyz", 3, &search) == PS_OK);
    CHECK(psrt_string_replace(&word, &search, &replacement, allocator, &changed) == PS_OK);
    CHECK(psrt_array_data(&word) == psrt_array_data(&changed));
    psrt_string_destroy(&changed);
    psrt_string_destroy(&search);
    size_t before_replace = tracker.attempts;
    tracker.fail_on = before_replace + 1;
    CHECK(psrt_string_replace(&word, &empty, &replacement, allocator, &joined) == PS_MEMORY);
    CHECK(!strcmp(psrt_string_cstr(&joined), "Grüße!"));
    tracker.fail_on = 0;
    psrt_string_destroy(&replacement);
    psrt_array parts;
    CHECK(psrt_string_split(&word, &empty, INT64_MAX, true, allocator, &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 5);
    const psrt_string *scalars = psrt_array_data(&parts);
    CHECK(!strcmp(psrt_string_cstr(&scalars[2]), "ü"));
    psrt_array_destroy(&parts);
    CHECK(psrt_string_split(&empty, &empty, INT64_MAX, true, allocator, &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 0);
    psrt_array_destroy(&parts);
    CHECK(psrt_string_make(allocator, "a,b", 3, &search) == PS_OK);
    CHECK(psrt_string_make(allocator, ",", 1, &replacement) == PS_OK);
    CHECK(psrt_string_split(&search, &replacement, INT64_MAX, true, allocator, &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 2);
    const psrt_string *fields = psrt_array_data(&parts);
    CHECK(!strcmp(psrt_string_cstr(&fields[0]), "a"));
    CHECK(!strcmp(psrt_string_cstr(&fields[1]), "b"));
    CHECK(psrt_string_join(&parts, &replacement, allocator, &changed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&changed), "a,b"));
    psrt_string_destroy(&changed);
    CHECK(psrt_string_join(&parts, &empty, allocator, &changed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&changed), "ab"));
    psrt_string_destroy(&changed);
    CHECK(psrt_string_join(&parts, &replacement, allocator, &replacement) == PS_INVALID);
    CHECK(psrt_string_join(&parts, &replacement, (ps_allocator){0}, &changed) == PS_INVALID);
    size_t before_join = tracker.live_bytes;
    tracker.fail_on = tracker.attempts + 1;
    CHECK(psrt_string_join(&parts, &replacement, allocator, &joined) == PS_MEMORY);
    CHECK(tracker.live_bytes == before_join);
    CHECK(!strcmp(psrt_string_cstr(&joined), "Grüße!"));
    tracker.fail_on = 0;
    psrt_array_destroy(&parts);
    psrt_string doubled;
    CHECK(psrt_string_make(allocator, "a,,b", 4, &doubled) == PS_OK);
    CHECK(psrt_string_split(&doubled, &replacement, INT64_MAX, true,
                            allocator, &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 2);
    psrt_array_destroy(&parts);
    CHECK(psrt_string_split(&doubled, &replacement, 2, false,
                            allocator, &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 3);
    fields = psrt_array_data(&parts);
    CHECK(!strcmp(psrt_string_cstr(&fields[1]), ""));
    psrt_array_destroy(&parts);
    CHECK(psrt_string_split(&doubled, &replacement, 1, true,
                            allocator, &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 2);
    fields = psrt_array_data(&parts);
    CHECK(!strcmp(psrt_string_cstr(&fields[1]), ",b"));
    psrt_array_destroy(&parts);
    psrt_string_destroy(&doubled);
    CHECK(psrt_string_make(allocator, "a,,b,c", 6, &doubled) == PS_OK);
    CHECK(psrt_string_split(&doubled, &replacement, 2, true,
                            allocator, &parts) == PS_OK);
    CHECK(psrt_array_count(&parts) == 3);
    fields = psrt_array_data(&parts);
    CHECK(!strcmp(psrt_string_cstr(&fields[2]), "c"));
    psrt_array_destroy(&parts);
    psrt_string_destroy(&doubled);
    CHECK(psrt_array_init(&psrt_array_element_type, allocator, 0, &parts) == PS_OK);
    CHECK(psrt_string_join(&parts, &replacement, allocator, &changed) == PS_OK);
    CHECK(!strcmp(psrt_string_cstr(&changed), ""));
    psrt_string_destroy(&changed);
    psrt_array_destroy(&parts);
    CHECK(psrt_string_split(&search, &replacement, INT64_MAX, true, allocator, &search) == PS_INVALID);
    CHECK(psrt_string_split(&search, NULL, INT64_MAX, true, allocator, &parts) == PS_INVALID);
    CHECK(psrt_string_split(&search, &replacement, -1, true, allocator, &parts) == PS_INVALID);
    CHECK(psrt_string_split(&search, &replacement, INT64_MAX, true,
                            (ps_allocator){0}, &parts) == PS_INVALID);
    for (size_t offset = 2; offset <= 3; offset++) {
        size_t live_before = tracker.live_bytes;
        tracker.fail_on = tracker.attempts + offset;
        CHECK(psrt_string_split(&search, &replacement, INT64_MAX, true,
                                allocator, &joined) == PS_MEMORY);
        CHECK(tracker.live_bytes == live_before);
        CHECK(!strcmp(psrt_string_cstr(&joined), "Grüße!"));
        tracker.fail_on = 0;
    }
    psrt_string_destroy(&search);
    psrt_string_destroy(&replacement);
    CHECK(psrt_string_concat(&empty, &word, allocator, &word) == PS_INVALID);
    CHECK(!strcmp(psrt_string_cstr(&word), "Grüße"));
    CHECK(psrt_string_make(allocator, "a\0b", 3, &joined) == PS_INVALID);
    CHECK(psrt_string_make(allocator, NULL, 1, &joined) == PS_INVALID);
    CHECK(!strcmp(psrt_string_cstr(&joined), "Grüße!"));
    const unsigned char valid_four[] = {0xf0, 0x9f, 0x8c, 0x8d};
    const unsigned char overlong[] = {0xc0, 0x80};
    const unsigned char surrogate[] = {0xed, 0xa0, 0x80};
    const unsigned char beyond_unicode[] = {0xf4, 0x90, 0x80, 0x80};
    const unsigned char truncated[] = {0xe2, 0x82};
    const unsigned char stray[] = {0x80};
    CHECK(psrt_string_utf8_valid(valid_four, sizeof valid_four));
    const struct { const unsigned char *bytes; size_t length; } invalid[] = {
        {overlong, sizeof overlong}, {surrogate, sizeof surrogate},
        {beyond_unicode, sizeof beyond_unicode}, {truncated, sizeof truncated},
        {stray, sizeof stray}};
    size_t invalid_attempts = tracker.attempts;
    CHECK(psrt_string_make(allocator, valid_four, SIZE_MAX, &word) == PS_LIMIT);
    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; i++) {
        CHECK(!psrt_string_utf8_valid(invalid[i].bytes, invalid[i].length));
        CHECK(psrt_string_make(allocator, invalid[i].bytes, invalid[i].length, &word) == PS_INVALID);
    }
    CHECK(tracker.attempts == invalid_attempts);
    psrt_string unicode;
    CHECK(psrt_string_make(allocator, valid_four, sizeof valid_four, &unicode) == PS_OK);
    CHECK(psrt_string_byte_count(&unicode) == sizeof valid_four);
    psrt_string_destroy(&unicode);

    psrt_array list;
    CHECK(psrt_array_init(&psrt_array_element_type, allocator, 0, &list) == PS_OK);
    CHECK(psrt_array_replace(&list, 0, 0, &word, 1) == PS_OK);
    psrt_string_destroy(&word);
    psrt_string_destroy(&copy);
    const psrt_string *element = psrt_array_data(&list);
    CHECK(psrt_string_valid(element) && !strcmp(psrt_string_cstr(element), "Grüße"));
    const char *labels[1] = {"untouched"};
    CHECK(psrt_string_array_cstr(&list, labels, 0) == PS_LIMIT);
    CHECK(!strcmp(labels[0], "untouched"));
    CHECK(psrt_string_array_cstr(&list, labels, 1) == PS_OK);
    CHECK(!strcmp(labels[0], "Grüße"));

    size_t attempts = tracker.attempts;
    tracker.fail_on = attempts + 1;
    CHECK(psrt_string_concat(element, &punctuation, allocator, &joined) == PS_MEMORY);
    CHECK(!strcmp(psrt_string_cstr(&joined), "Grüße!"));
    tracker.fail_on = 0;
    tracker.budget = tracker.live_bytes;
    CHECK(psrt_string_make(allocator, "x", 1, &word) == PS_MEMORY);
    tracker.budget = 0;

    psrt_memory module_memory = {0};
    psrt_string symbol, duplicate, another;
    const char *pinned = NULL, *reused = NULL;
    CHECK(psrt_string_make(allocator, "m/s", 3, &symbol) == PS_OK);
    CHECK(psrt_string_pin_cstr(&module_memory, &symbol, &pinned) == PS_OK);
    CHECK(module_memory.live_bytes == sizeof(psrt_string_pin));
    psrt_string_destroy(&symbol);
    CHECK(!strcmp(pinned, "m/s"));
    CHECK(psrt_string_make(allocator, "m/s", 3, &duplicate) == PS_OK);
    CHECK(psrt_string_pin_cstr(&module_memory, &duplicate, &reused) == PS_OK);
    CHECK(reused == pinned && module_memory.live_bytes == sizeof(psrt_string_pin));
    psrt_string_destroy(&duplicate);
    CHECK(psrt_string_make(allocator, "N", 1, &another) == PS_OK);
    size_t saved_live = module_memory.live_bytes;
    module_memory.live_bytes = PSRT_MEMORY_LIMIT;
    const char *unchanged = pinned;
    CHECK(psrt_string_pin_cstr(&module_memory, &another, &unchanged) == PS_MEMORY);
    CHECK(unchanged == pinned && module_memory.string_pins != NULL);
    module_memory.live_bytes = saved_live;
    CHECK(psrt_string_pin_cstr(&module_memory, &another, &reused) == PS_OK);
    psrt_string_destroy(&another);
    CHECK(!strcmp(reused, "N"));
    psrt_string_pins_destroy(&module_memory);
    CHECK(!module_memory.live_bytes && !module_memory.string_pins);

    psrt_string_destroy(&joined);
    psrt_string_destroy(&punctuation);
    psrt_string_destroy(&empty);
    psrt_array_destroy(&list);
    CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    puts("Language string storage: ownership, UTF-8 bytes, concat and rollback passed");
    return 0;
}
