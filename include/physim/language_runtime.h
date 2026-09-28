#ifndef PHYSIM_LANGUAGE_RUNTIME_H
#define PHYSIM_LANGUAGE_RUNTIME_H
/* Experimental support for generated C17. Not a stable public ABI.
 * One generated translation unit. Decimal text uses the C numeric locale. */
#ifndef _WIN32
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#endif
#include <float.h>
#include <inttypes.h>
#include <locale.h>
#ifdef __APPLE__
#include <xlocale.h>
#endif
#include <math.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(double) == 8 && DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024,
               "Physim Float64 requires IEEE 754 binary64");

#ifndef PSRT_SOURCE
#define PSRT_SOURCE "<source>"
#endif
typedef struct psrt_site {
    const char *file;
    size_t line, column;
} psrt_site;
#define PSRT_AT(line, column) ((psrt_site){PSRT_SOURCE, line, column})

/* Generated owners register stack-local cleanup entries while their object is
 * alive. Cleanup callbacks must not fail, longjmp, or modify this chain. */
typedef struct psrt_cleanup {
    struct psrt_cleanup *previous;
    void *object;
    void (*destroy)(void *object);
} psrt_cleanup;

#ifdef PSRT_MODULE
typedef struct psrt_trap {
    jmp_buf jump;
    struct psrt_trap *previous;
    char *error;
    size_t capacity;
    unsigned depth;
    psrt_cleanup *cleanup;
} psrt_trap;
#ifdef _MSC_VER
static __declspec(thread) psrt_trap *psrt_current;
#else
static _Thread_local psrt_trap *psrt_current;
#endif
#define psrt_depth (psrt_current->depth)
#define psrt_cleanups (psrt_current->cleanup)
#else
static unsigned psrt_depth;
static psrt_cleanup *psrt_cleanups;
#endif

/* An attempt catches failures within the current host trap. Its mark preserves
 * owners created before the expression, while nested calls may add new ones. */
typedef struct psrt_try_frame {
    jmp_buf jump;
    struct psrt_try_frame *previous;
    psrt_cleanup *mark;
    unsigned depth;
} psrt_try_frame;
#ifdef _MSC_VER
static __declspec(thread) psrt_try_frame *psrt_try_current;
#else
static _Thread_local psrt_try_frame *psrt_try_current;
#endif

static inline void psrt_cleanup_push(psrt_cleanup *entry, void *object, void (*destroy)(void *)) {
    *entry = (psrt_cleanup){psrt_cleanups, object, destroy};
    psrt_cleanups = entry;
}
/* mark is NULL or a still-live ancestor entry, captured before entering a scope.
 * Call before leaving its C stack frame, including return/break/continue. */
static inline void psrt_cleanup_unwind(psrt_cleanup *mark) {
    while (psrt_cleanups != mark) {
        psrt_cleanup *entry = psrt_cleanups;
        psrt_cleanups = entry->previous;
        entry->destroy(entry->object);
    }
}

static _Noreturn void psrt_fail(psrt_site site, const char *message) {
    if (psrt_try_current) {
        psrt_try_frame *frame = psrt_try_current;
        psrt_cleanup_unwind(frame->mark);
        psrt_depth = frame->depth;
        psrt_try_current = frame->previous;
        longjmp(frame->jump, 1);
    }
#ifdef PSRT_MODULE
    if (psrt_current) {
        snprintf(psrt_current->error, psrt_current->capacity, "%s:%zu:%zu: runtime error: %s",
                 site.file, site.line, site.column, message);
        /* Still inside the failing call: registered stack addresses remain live. */
        psrt_cleanup_unwind(NULL);
        longjmp(psrt_current->jump, 1);
    }
#endif
    fprintf(stderr, "%s:%zu:%zu: runtime error: %s\n", site.file, site.line, site.column, message);
#ifndef PSRT_MODULE
    psrt_cleanup_unwind(NULL);
#endif
    exit(70);
}
static inline void psrt_initialized(bool ready, psrt_site site) {
    if (!ready)
        psrt_fail(site, "Global used before initialization");
}
static inline void psrt_local_initialized(bool ready, psrt_site site) {
    if (!ready)
        psrt_fail(site, "Local used before initialization");
}
static inline void psrt_enter(psrt_site site) {
    if (psrt_depth >= 256)
        psrt_fail(site, "Function recursion limit exceeded (256)");
    psrt_depth++;
}
static inline void psrt_leave(void) { psrt_depth--; }
static inline int64_t psrt_add(int64_t a, int64_t b, psrt_site site) {
    if ((b > 0 && a > INT64_MAX - b) || (b < 0 && a < INT64_MIN - b))
        psrt_fail(site, "Int64 addition overflow");
    return a + b;
}
static inline int64_t psrt_sub(int64_t a, int64_t b, psrt_site site) {
    if ((b < 0 && a > INT64_MAX + b) || (b > 0 && a < INT64_MIN + b))
        psrt_fail(site, "Int64 subtraction overflow");
    return a - b;
}
static inline int64_t psrt_mul(int64_t a, int64_t b, psrt_site site) {
    uint64_t x = a < 0 ? (uint64_t)(-(a + 1)) + 1 : (uint64_t)a;
    uint64_t y = b < 0 ? (uint64_t)(-(b + 1)) + 1 : (uint64_t)b;
    bool negative = (a < 0) != (b < 0);
    uint64_t limit = (uint64_t)INT64_MAX + (negative ? 1u : 0u);
    if (y && x > limit / y)
        psrt_fail(site, "Int64 multiplication overflow");
    uint64_t result = x * y;
    if (negative && result == (uint64_t)INT64_MAX + 1u)
        return INT64_MIN;
    return negative ? -(int64_t)result : (int64_t)result;
}
static inline int64_t psrt_neg(int64_t a, psrt_site site) {
    if (a == INT64_MIN)
        psrt_fail(site, "Int64 negation overflow");
    return -a;
}
static inline int64_t psrt_int_abs(int64_t value, psrt_site site) {
    if (value == INT64_MIN)
        psrt_fail(site, "Int64 absolute value overflow");
    return value < 0 ? -value : value;
}
static inline bool psrt_int64_is_multiple(int64_t value, int64_t divisor) {
    if (divisor == 0)
        return value == 0;
    if (divisor == -1)
        return true;
    return value % divisor == 0;
}
static inline int64_t psrt_int64_signum(int64_t value) {
    return (int64_t)(value > 0) - (int64_t)(value < 0);
}
static inline int64_t psrt_int_min(int64_t left, int64_t right, psrt_site site) {
    (void)site;
    return left <= right ? left : right;
}
static inline int64_t psrt_int_max(int64_t left, int64_t right, psrt_site site) {
    (void)site;
    return left >= right ? left : right;
}
static inline int64_t psrt_int_clamp(int64_t value, int64_t lower, int64_t upper,
                                     psrt_site site) {
    if (lower > upper)
        psrt_fail(site, "Clamp lower bound exceeds upper bound");
    return value < lower ? lower : value > upper ? upper : value;
}
static inline int64_t psrt_div(int64_t a, int64_t b, psrt_site site) {
    if (!b)
        psrt_fail(site, "Division by zero");
    if (a == INT64_MIN && b == -1)
        psrt_fail(site, "Int64 division overflow");
    return a / b;
}
static inline int64_t psrt_mod(int64_t a, int64_t b, psrt_site site) {
    if (!b)
        psrt_fail(site, "Remainder by zero");
    if (a == INT64_MIN && b == -1)
        return 0;
    return a % b;
}
/* Bitwise Int64 operations use the complete two's-complement bit pattern. */
static inline int64_t psrt_bit_pattern(uint64_t bits) {
    int64_t value;
    memcpy(&value, &bits, sizeof value);
    return value;
}
static inline int64_t psrt_bit_and(int64_t a, int64_t b, psrt_site site) {
    (void)site;
    return psrt_bit_pattern((uint64_t)a & (uint64_t)b);
}
static inline int64_t psrt_bit_or(int64_t a, int64_t b, psrt_site site) {
    (void)site;
    return psrt_bit_pattern((uint64_t)a | (uint64_t)b);
}
static inline int64_t psrt_bit_xor(int64_t a, int64_t b, psrt_site site) {
    (void)site;
    return psrt_bit_pattern((uint64_t)a ^ (uint64_t)b);
}
static inline int64_t psrt_bit_not(int64_t a) {
    return psrt_bit_pattern(~(uint64_t)a);
}
static inline int64_t psrt_shift_left(int64_t a, int64_t count, psrt_site site) {
    if (count < 0 || count >= 64)
        psrt_fail(site, "Bit shift count must be between 0 and 63");
    return psrt_bit_pattern((uint64_t)a << (unsigned)count);
}
static inline int64_t psrt_shift_right(int64_t a, int64_t count, psrt_site site) {
    if (count < 0 || count >= 64)
        psrt_fail(site, "Bit shift count must be between 0 and 63");
    uint64_t bits = (uint64_t)a >> (unsigned)count;
    if (a < 0 && count)
        bits |= UINT64_MAX << (64u - (unsigned)count);
    return psrt_bit_pattern(bits);
}
static inline double psrt_finite(double value, psrt_site site) {
    if (!isfinite(value))
        psrt_fail(site, "Non-finite Float64 result");
    return value;
}
/* Keep decimal text stable even when an embedding process changes LC_NUMERIC.
 * The common C-locale path avoids constructing a locale for each literal. */
static inline double psrt_decimal_value(const char *text, char **end, psrt_site site) {
    const struct lconv *current = localeconv();
    if (current && current->decimal_point && !strcmp(current->decimal_point, "."))
        return strtod(text, end);
#ifdef _WIN32
    _locale_t numeric = _create_locale(LC_NUMERIC, "C");
    if (!numeric)
        psrt_fail(site, "Cannot initialize C numeric locale");
    double value = _strtod_l(text, end, numeric);
    _free_locale(numeric);
#else
    locale_t numeric = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
    if (!numeric)
        psrt_fail(site, "Cannot initialize C numeric locale");
    double value = strtod_l(text, end, numeric);
    freelocale(numeric);
#endif
    return value;
}
static inline size_t psrt_format_float64(double value, char text[64]) {
    if (!isfinite(value))
        return 0;
    int length = snprintf(text, 64, "%.17g", value);
    if (length <= 0 || length >= 64)
        return 0;
    const struct lconv *current = localeconv();
    const char *decimal = current ? current->decimal_point : NULL;
    if (decimal && strcmp(decimal, ".")) {
        size_t width = strlen(decimal);
        if (!width)
            return 0;
        char *position = strstr(text, decimal);
        if (position) {
            memmove(position + 1, position + width, strlen(position + width) + 1);
            *position = '.';
            length -= (int)width - 1;
        }
    }
    return (size_t)length;
}
static inline double psrt_number(const char *text, psrt_site site) {
    char *end;
    double value = psrt_decimal_value(text, &end, site);
    if (*end)
        psrt_fail(site, "Invalid Float64 literal");
    return psrt_finite(value, site);
}
/* String conversions accept ASCII decimal syntax without surrounding space. */
typedef enum {
    PSRT_PARSE_VALID,
    PSRT_PARSE_INVALID,
    PSRT_PARSE_RANGE
} psrt_parse_status;
static inline psrt_parse_status psrt_try_parse_int64(const char *text, int64_t *result) {
    if (!text || !*text)
        return PSRT_PARSE_INVALID;
    const unsigned char *cursor = (const unsigned char *)text;
    bool negative = *cursor == '-';
    if (*cursor == '-' || *cursor == '+')
        cursor++;
    if (*cursor < '0' || *cursor > '9')
        return PSRT_PARSE_INVALID;
    uint64_t limit = (uint64_t)INT64_MAX + (negative ? 1u : 0u);
    uint64_t magnitude = 0;
    for (; *cursor; cursor++) {
        if (*cursor < '0' || *cursor > '9')
            return PSRT_PARSE_INVALID;
        unsigned digit = *cursor - '0';
        if (magnitude > (limit - digit) / 10)
            return PSRT_PARSE_RANGE;
        magnitude = magnitude * 10 + digit;
    }
    *result = negative && magnitude == (uint64_t)INT64_MAX + 1u
                  ? INT64_MIN : negative ? -(int64_t)magnitude : (int64_t)magnitude;
    return PSRT_PARSE_VALID;
}
static inline int64_t psrt_parse_int64(const char *text, psrt_site site) {
    int64_t result = 0;
    psrt_parse_status status = psrt_try_parse_int64(text, &result);
    if (status != PSRT_PARSE_VALID)
        psrt_fail(site, status == PSRT_PARSE_RANGE ? "Int64 text out of range"
                                                   : "Invalid Int64 text");
    return result;
}
static inline psrt_parse_status psrt_try_parse_float64(const char *text, double *result,
                                                       psrt_site site) {
    if (!text || !*text)
        return PSRT_PARSE_INVALID;
    const unsigned char *cursor = (const unsigned char *)text;
    if (*cursor == '-' || *cursor == '+')
        cursor++;
    size_t digits = 0;
    while (*cursor >= '0' && *cursor <= '9') {
        cursor++;
        digits++;
    }
    if (*cursor == '.') {
        cursor++;
        while (*cursor >= '0' && *cursor <= '9') {
            cursor++;
            digits++;
        }
    }
    if (!digits)
        return PSRT_PARSE_INVALID;
    if (*cursor == 'e' || *cursor == 'E') {
        cursor++;
        if (*cursor == '-' || *cursor == '+')
            cursor++;
        if (*cursor < '0' || *cursor > '9')
            return PSRT_PARSE_INVALID;
        while (*cursor >= '0' && *cursor <= '9')
            cursor++;
    }
    if (*cursor)
        return PSRT_PARSE_INVALID;
    char *end = NULL;
    double value = psrt_decimal_value(text, &end, site);
    if (*end)
        return PSRT_PARSE_INVALID;
    if (!isfinite(value))
        return PSRT_PARSE_RANGE;
    *result = value;
    return PSRT_PARSE_VALID;
}
static inline double psrt_parse_float64(const char *text, psrt_site site) {
    double result = 0;
    psrt_parse_status status = psrt_try_parse_float64(text, &result, site);
    if (status != PSRT_PARSE_VALID)
        psrt_fail(site, status == PSRT_PARSE_RANGE ? "Float64 text out of range"
                                                   : "Invalid Float64 text");
    return result;
}
static inline int64_t psrt_int64(double value, psrt_site site) {
    /* The upper limit is exclusive: (double)INT64_MAX rounds to 2^63.
     * Test before casting so no out-of-range C conversion can occur. */
    if (!isfinite(value) || value < -9223372036854775808.0 || value >= 9223372036854775808.0)
        psrt_fail(site, "Float64 to Int64 conversion out of range");
    return (int64_t)value; /* Defined truncation toward zero. */
}
static inline double psrt_float64(int64_t value, psrt_site site) {
    (void)site;
    uint64_t magnitude = value < 0 ? (uint64_t)(-(value + 1)) + 1 : (uint64_t)value;
    unsigned shift = 0;
    uint64_t significand = magnitude;
    while (significand > UINT64_C(9007199254740991)) {
        significand >>= 1;
        shift++;
    }
    if (shift) {
        uint64_t remainder = magnitude & ((UINT64_C(1) << shift) - 1);
        uint64_t halfway = UINT64_C(1) << (shift - 1);
        if (remainder > halfway || (remainder == halfway && (significand & 1)))
            significand++;
    }
    /* Both operations are exact after explicit nearest/ties-to-even rounding. */
    double result = ldexp((double)significand, (int)shift);
    return value < 0 ? -result : result;
}
static inline double psrt_fdiv(double a, double b, psrt_site site) {
    if (b == 0.0)
        psrt_fail(site, "Division by zero");
    return psrt_finite(a / b, site);
}
static inline void psrt_assert(bool condition, psrt_site site) {
    if (!condition)
        psrt_fail(site, "Assertion failed");
}
static inline void psrt_assert_message(bool condition, const char *message, psrt_site site) {
    if (!condition)
        psrt_fail(site, message && *message ? message : "Assertion failed");
}
static inline void psrt_print_i(int64_t value, psrt_site site) {
    if (printf("%" PRId64 "\n", value) < 0)
        psrt_fail(site, "Output failed");
}
static inline void psrt_print_f(double value, psrt_site site) {
    char text[64];
    if (!psrt_format_float64(value, text) || puts(text) < 0)
        psrt_fail(site, "Output failed");
}
static inline void psrt_print_s(const char *value, psrt_site site) {
    if (puts(value) < 0)
        psrt_fail(site, "Output failed");
}
static inline void psrt_print_b(bool value, psrt_site site) {
    psrt_print_s(value ? "true" : "false", site);
}
#endif
