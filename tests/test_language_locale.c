#include <stdio.h>
#include <locale.h>
#include "physim/language_string.h"
#include "test_allocator.h"

#define CHECK(condition)                                                                          \
    do {                                                                                          \
        if (!(condition)) {                                                                       \
            fprintf(stderr, "Language locale line %d: %s\n", __LINE__, #condition);              \
            return 1;                                                                             \
        }                                                                                         \
    } while (0)

int main(void) {
    const char *previous = setlocale(LC_NUMERIC, NULL);
    char saved[128];
    CHECK(previous && strlen(previous) < sizeof saved);
    memcpy(saved, previous, strlen(previous) + 1);
    const char *candidates[] = {"de-DE", "German_Germany.1252", "French_France.1252",
                                "de_DE.UTF-8", "fr_FR.UTF-8", "de_DE.utf8", "fr_FR.utf8"};
    bool alternate = false;
    for (size_t i = 0; i < sizeof candidates / sizeof candidates[0]; i++) {
        if (setlocale(LC_NUMERIC, candidates[i]) &&
            strcmp(localeconv()->decimal_point, ".") != 0) {
            alternate = true;
            break;
        }
    }
    char selected[128];
    const char *active = setlocale(LC_NUMERIC, NULL);
    CHECK(active && strlen(active)<sizeof selected);
    strcpy(selected,active);
#ifndef _WIN32
    locale_t previous_thread = uselocale((locale_t)0);
    CHECK(previous_thread);
#endif
    psrt_site site = {"locale-test", 1, 1};
    CHECK(psrt_number("1.25", site) == 1.25);
    CHECK(psrt_parse_float64("-.5", site) == -0.5);
    double parsed = 0;
    CHECK(psrt_try_parse_float64("-.5", &parsed, site) == PSRT_PARSE_VALID);
    CHECK(parsed == -0.5);
    CHECK(psrt_try_parse_float64("1,5", &parsed, site) == PSRT_PARSE_INVALID);
    CHECK(psrt_try_parse_float64("1e309", &parsed, site) == PSRT_PARSE_RANGE);
    CHECK(parsed == -0.5);
    int64_t integer = 0;
    CHECK(psrt_try_parse_int64("-9223372036854775808", &integer) == PSRT_PARSE_VALID);
    CHECK(integer == INT64_MIN);
    CHECK(psrt_try_parse_int64("9223372036854775808", &integer) == PSRT_PARSE_RANGE);
    CHECK(integer == INT64_MIN);
    char buffer[64];
    CHECK(psrt_format_float64(1.25, buffer) == 4);
    CHECK(!strcmp(buffer, "1.25"));
    CHECK(psrt_format_float64(-0.0, buffer) == 2);
    CHECK(!strcmp(buffer, "-0"));
    test_allocator tracker = {0};
    ps_allocator allocator = test_domain(&tracker);
    psrt_string value;
    CHECK(psrt_string_from_float64(0.1, allocator, &value) == PS_OK);
    CHECK(strchr(psrt_string_cstr(&value), '.') != NULL);
    CHECK(psrt_parse_float64(psrt_string_cstr(&value), site) == 0.1);
    psrt_string_destroy(&value);
    CHECK(!tracker.invalid && !tracker.live_bytes && !tracker.live_blocks);
    CHECK(!strcmp(setlocale(LC_NUMERIC,NULL),selected));
#ifndef _WIN32
    CHECK(uselocale((locale_t)0)==previous_thread);
    if(alternate) {
        locale_t caller = newlocale(LC_NUMERIC_MASK,selected,(locale_t)0);
        CHECK(caller && uselocale(caller));
        CHECK(psrt_parse_float64("2.75",site)==2.75);
        CHECK(uselocale((locale_t)0)==caller && strcmp(localeconv()->decimal_point,".")!=0);
        CHECK(uselocale(previous_thread));
        freelocale(caller);
    }
#endif
    CHECK(setlocale(LC_NUMERIC, saved) != NULL);
    puts(alternate ? "C decimal conversion passed with an alternate numeric locale"
                   : "C decimal conversion passed; no alternate locale installed");
    return 0;
}
