#include "physim/numerics.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "scalar range line %d: %s\n", __LINE__, #x);                           \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static unsigned calls;
static unsigned bad_at;
static double flat(double x, void *user) {
    (void)x;
    (void)user;
    calls++;
    return bad_at && calls == bad_at ? NAN : 1;
}
static double linear(double x, void *user) {
    calls++;
    return bad_at && calls == bad_at ? INFINITY : x - *(double *)user;
}
static bool same(const ps_scalar_report *a, const ps_scalar_report *b) {
    return a->x == b->x && a->value == b->value && a->lower == b->lower && a->upper == b->upper &&
           a->iterations == b->iterations && a->evaluations == b->evaluations;
}
int main(void) {
    ps_scalar_report r, old = {7, 8, 9, 10, 11, 12};
    double tiny = DBL_TRUE_MIN, target = .25;
    /* Three subnormal units are wider than twice the requested unit tolerance. */
    CHECK(ps_minimize_golden(flat, NULL, -27 * tiny, -19 * tiny, tiny, 0, 100, &r) == PS_OK);
    CHECK((r.upper - r.lower) <= 2 * tiny && r.lower <= r.x && r.x <= r.upper);
    CHECK(ps_minimize_golden(flat, NULL, -21 * tiny, -18 * tiny, tiny, 0, 1, &r) == PS_OK);
    CHECK(r.upper - r.lower <= 2 * tiny && r.iterations == 1 && r.evaluations == 3);
    CHECK(ps_minimize_golden(flat, NULL, tiny, 4 * tiny, tiny, 0, 100, &r) == PS_OK);
    CHECK(r.iterations == 1 && r.evaluations == 3 && r.upper - r.lower <= 2 * tiny);
    double tiny_target = 4 * tiny;
    calls = 0;
    CHECK(ps_root_bisect(linear, &tiny_target, 3 * tiny, 6 * tiny, tiny, 0, 100, &r) == PS_OK);
    CHECK(r.x == tiny_target && r.iterations == 1 && r.evaluations == 3);
    calls = bad_at = 0;
    CHECK(ps_minimize_golden(flat, NULL, -DBL_MAX, DBL_MAX, DBL_MAX, .5, 10, &r) == PS_OK);
    CHECK(r.evaluations == calls && r.x >= r.lower && r.x <= r.upper);
    CHECK(ps_minimize_golden(flat, NULL, 0, 0x1p1023, 1, .999, 100, &r) == PS_OK);
    CHECK(ps_minimize_golden(flat, NULL, -DBL_MAX, -nextafter(DBL_MAX, 0), DBL_TRUE_MIN, 0, 100,
                             &r) == PS_LIMIT);
    calls = bad_at = 0;
    CHECK(ps_root_bisect(linear, &target, 0, 1, 1e-15, 0, 100, &r) == PS_OK);
    CHECK(r.x == target && r.value == 0 && r.evaluations == calls &&
          r.evaluations == r.iterations + 2);
    target = 0;
    calls = 0;
    CHECK(ps_root_bisect(linear, &target, 0, 1, tiny, 0, 100, &r) == PS_OK && r.iterations == 0 &&
          calls == 2);
    for (unsigned minimize = 0; minimize < 2; minimize++) {
        ps_result (*search)(ps_scalar_fn, void *, double, double, double, double, unsigned,
                            ps_scalar_report *) = minimize ? ps_minimize_golden : ps_root_bisect;
        ps_scalar_fn fn = minimize ? flat : linear;
        target = .25;
        for (unsigned n = 1; n <= 3; n++) {
            calls = 0;
            bad_at = n;
            r = old;
            CHECK(search(fn, &target, 0, 1, tiny, 0, 100, &r) == PS_NUMERIC && same(&r, &old));
        }
        bad_at = 0;
        r = old;
        calls = 0;
        CHECK(search(fn, &target, 0, 1, 1e-12, 0, UINT_MAX, &r) == PS_OK);
        CHECK(search(fn, &target, 0, 1, 1e-12, 0, UINT_MAX - 1, &r) == PS_OK);
        r = old;
        calls = 0;
        CHECK(search(fn, &target, 0, 1, 1e-12, 0, 0, &r) == PS_INVALID && same(&r, &old));
        CHECK(search(fn, &target, 1, 0, 1e-12, 0, 100, &r) == PS_INVALID && same(&r, &old));
        CHECK(search(fn, &target, 0, 1, 0, 0, 100, &r) == PS_INVALID && same(&r, &old));
        CHECK(search(fn, &target, 0, 1, 1e-12, 1, 100, &r) == PS_INVALID && same(&r, &old));
        CHECK(search(fn, &target, NAN, 1, 1e-12, 0, 100, &r) == PS_INVALID && same(&r, &old));
        CHECK(search(NULL, &target, 0, 1, 1e-12, 0, 100, &r) == PS_INVALID && same(&r, &old));
        CHECK(search(fn, &target, 0, 1, 1e-12, 0, 100, NULL) == PS_INVALID);
    }
    puts("Scalar search: subnormal/huge brackets, exact stopping, endpoint roots, representable "
         "exhaustion, callback errors and report rollback passed");
    return 0;
}
