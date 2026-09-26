#include "physim/measurement.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Measurement line %d: %s\n", __LINE__, #x);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int close_to(double a, double b) { return fabs(a - b) <= 1e-12 * fmax(1, fabs(b)); }
static ps_sensor_config config(void) {
    ps_sensor_config c = {0};
    c.unit = PS_METRE;
    c.rate_hz = 100;
    c.noise = (ps_distribution){PS_DIST_CONSTANT, 0, 0};
    return c;
}
int main(void) {
    ps_rng rng;
    ps_rng_seed(&rng, 42);
    ps_rng saved = rng;
    double value = 123, mean = 456, sd = 789;
    ps_distribution d = {PS_DIST_CONSTANT, 7, 0};
    CHECK(ps_distribution_sample(d, &rng, &value) == PS_OK && value == 7);
    CHECK(!memcmp(&rng, &saved, sizeof rng));
    CHECK(ps_distribution_moments(d, &mean, &sd) == PS_OK && mean == 7 && sd == 0);
    d = (ps_distribution){PS_DIST_UNIFORM, -2, 4};
    CHECK(ps_distribution_moments(d, &mean, &sd) == PS_OK && mean == 1 && close_to(sd, sqrt(3)));
    d = (ps_distribution){PS_DIST_UNIFORM, -DBL_MAX, DBL_MAX};
    CHECK(ps_distribution_moments(d, &mean, &sd) == PS_OK && mean == 0 && isfinite(sd));
    CHECK(ps_distribution_sample(d, &rng, &value) == PS_OK && isfinite(value));
    ps_distribution invalid[] = {{PS_DIST_CONSTANT, 1, 1}, {PS_DIST_NORMAL, 0, -1},
                                 {PS_DIST_UNIFORM, 3, 2},  {PS_DIST_NORMAL, INFINITY, 1},
                                 {PS_DIST_NORMAL, 1, NAN}, {(ps_distribution_kind)99, 0, 0}};
    for (unsigned i = 0; i < sizeof invalid / sizeof *invalid; i++) {
        saved = rng;
        value = 123;
        mean = 456;
        sd = 789;
        CHECK(ps_distribution_sample(invalid[i], &rng, &value) == PS_INVALID);
        CHECK(value == 123 && !memcmp(&rng, &saved, sizeof rng));
        CHECK(ps_distribution_moments(invalid[i], &mean, &sd) == PS_INVALID && mean == 456 &&
              sd == 789);
    }
    for (unsigned kind = 0; kind < 2; kind++) {
        d = kind ? (ps_distribution){PS_DIST_NORMAL, 2, 3}
                 : (ps_distribution){PS_DIST_UNIFORM, -2, 6};
        double sum = 0, squares = 0;
        ps_rng replay;
        ps_rng_seed(&rng, 90210);
        replay = rng;
        for (unsigned i = 0; i < 100000; i++) {
            double again;
            CHECK(ps_distribution_sample(d, &rng, &value) == PS_OK);
            CHECK(ps_distribution_sample(d, &replay, &again) == PS_OK && value == again);
            if (!kind)
                CHECK(value >= -2 && value <= 6);
            sum += value;
            squares += value * value;
        }
        CHECK(fabs(sum / 100000 - 2) < .04);
        CHECK(fabs(sqrt(squares / 100000 - pow(sum / 100000, 2)) - (kind ? 3 : 8 / sqrt(12))) <
              .04);
    }
    /* Find an overflowing draw; RNG and destination must survive it unchanged. */
    bool overflow = false;
    for (uint64_t seed = 0; seed < 32; seed++) {
        ps_rng_seed(&rng, seed);
        saved = rng;
        value = 123;
        if (ps_distribution_sample((ps_distribution){PS_DIST_NORMAL, DBL_MAX, DBL_MAX}, &rng,
                                   &value) == PS_NUMERIC) {
            CHECK(value == 123 && !memcmp(&rng, &saved, sizeof rng));
            overflow = true;
            break;
        }
    }
    CHECK(overflow);
    ps_sensor_config c = config();
    ps_sensor sensor;
    CHECK(ps_sensor_init(&sensor, &c, 42) == PS_OK);
    ps_measurement m;
    ps_quantity truth = {1, PS_METRE};
    CHECK(ps_sensor_read(&sensor, 0, truth, &m) == PS_OK && m.state == PS_MEASUREMENT_VALID &&
          m.value.value == 1 && m.index == 0);
    ps_sensor snapshot = sensor;
    CHECK(ps_sensor_read(&sensor, .005, truth, &m) == PS_OK && m.state == PS_MEASUREMENT_NOT_DUE);
    CHECK(!memcmp(&sensor, &snapshot, sizeof sensor));
    CHECK(ps_sensor_read(&sensor, nextafter(.01, 0), truth, &m) == PS_OK && m.index == 1 &&
          m.state == PS_MEASUREMENT_VALID);
    CHECK(ps_sensor_read(&sensor, .045, truth, &m) == PS_OK && m.index == 4 && m.skipped == 2 &&
          m.time_s == .045);
    double next;
    CHECK(ps_sensor_next_time(&sensor, &next) == PS_OK && next == .05);
    snapshot = sensor;
    ps_measurement previous = m;
    CHECK(ps_sensor_read(&sensor, .04, truth, &m) == PS_INVALID);
    CHECK(!memcmp(&sensor, &snapshot, sizeof sensor) && !memcmp(&m, &previous, sizeof m));
    truth.unit = PS_SECOND;
    CHECK(ps_sensor_read(&sensor, .05, truth, &m) == PS_INVALID);
    CHECK(!memcmp(&sensor, &snapshot, sizeof sensor) && !memcmp(&m, &previous, sizeof m));
    truth.unit = PS_METRE;
    CHECK(ps_sensor_read(&sensor, 1e20, truth, &m) == PS_LIMIT &&
          !memcmp(&sensor, &snapshot, sizeof sensor));
    c.rate_hz = 0;
    CHECK(ps_sensor_init(&sensor, &c, 0) == PS_INVALID &&
          !memcmp(&sensor, &snapshot, sizeof sensor));
    c = config();
    c.start_time_s = DBL_MAX;
    CHECK(ps_sensor_init(&sensor, &c, 0) == PS_NUMERIC &&
          !memcmp(&sensor, &snapshot, sizeof sensor));
    c = config();
    c.rate_hz = DBL_MIN / 4;
    CHECK(ps_sensor_config_validate(&c) == PS_NUMERIC);
    c = config();
    c.resolution = .5;
    CHECK(ps_sensor_init(&sensor, &c, 0) == PS_OK);
    const double inputs[] = {.25, .75, -.25, -.75}, expected[] = {0, 1, 0, -1};
    for (unsigned i = 0; i < 4; i++) {
        CHECK(ps_sensor_read(&sensor, i * .01, (ps_quantity){inputs[i], PS_METRE}, &m) == PS_OK);
        CHECK(m.value.value == expected[i] && close_to(m.standard_uncertainty, .5 / sqrt(12)));
    }
    c = config();
    c.offset = .2;
    c.drift_per_s = .3;
    c.start_time_s = 2;
    c.uncertainty_absolute = .04;
    c.uncertainty_relative = .03;
    c.noise = (ps_distribution){PS_DIST_UNIFORM, -.02, .02};
    CHECK(ps_sensor_init(&sensor, &c, 77) == PS_OK);
    ps_unit centimetre = PS_METRE;
    centimetre.scale = .01;
    centimetre.symbol = "cm";
    CHECK(ps_sensor_read(&sensor, 3, (ps_quantity){200, centimetre}, &m) == PS_OK);
    CHECK(m.value.value >= 2.48 && m.value.value <= 2.52 && m.skipped == 100);
    CHECK(close_to(m.standard_uncertainty, sqrt(.04 * .04 + .06 * .06 + .02 * .02 / 3)));
    CHECK(ps_sensor_reset(&sensor, 77) == PS_OK);
    ps_measurement again;
    CHECK(ps_sensor_read(&sensor, 3, (ps_quantity){200, centimetre}, &again) == PS_OK);
    CHECK(again.value.value == m.value.value && again.index == m.index);
    c = config();
    c.noise = (ps_distribution){PS_DIST_NORMAL, 0, .1};
    c.dropout_probability = .25;
    ps_sensor complete, sparse, no_drop;
    CHECK(ps_sensor_init(&complete, &c, 44) == PS_OK && ps_sensor_init(&sparse, &c, 44) == PS_OK);
    c.dropout_probability = 0;
    CHECK(ps_sensor_init(&no_drop, &c, 44) == PS_OK);
    unsigned dropped = 0, valid = 0;
    for (unsigned i = 0; i < 20000; i++) {
        CHECK(ps_sensor_read(&complete, i * .01, truth, &m) == PS_OK && m.index == i &&
              m.skipped == 0);
        CHECK(ps_sensor_read(&no_drop, i * .01, truth, &again) == PS_OK &&
              again.state == PS_MEASUREMENT_VALID);
        if (m.state == PS_MEASUREMENT_VALID) {
            CHECK(m.value.value == again.value.value);
            valid++;
        } else {
            CHECK(m.state == PS_MEASUREMENT_DROPPED && m.value.value == 0 &&
                  m.standard_uncertainty == 0);
            dropped++;
        }
        if (i % 7 == 0) {
            CHECK(ps_sensor_read(&sparse, i * .01, truth, &again) == PS_OK);
            CHECK(m.state == again.state && m.index == again.index &&
                  m.value.value == again.value.value);
            CHECK(again.skipped == (i ? 6u : 0u));
        }
        snapshot = complete;
        CHECK(ps_sensor_read(&complete, i * .01 + .001, truth, &again) == PS_OK &&
              again.state == PS_MEASUREMENT_NOT_DUE);
        CHECK(!memcmp(&snapshot, &complete, sizeof snapshot));
    }
    CHECK(valid + dropped == 20000 && dropped > 4750 && dropped < 5250);
    c.dropout_probability = 1;
    CHECK(ps_sensor_init(&sensor, &c, 0) == PS_OK);
    CHECK(ps_sensor_read(&sensor, 0, truth, &m) == PS_OK && m.state == PS_MEASUREMENT_DROPPED);
    c = config();
    c.offset = DBL_MAX;
    CHECK(ps_sensor_init(&sensor, &c, 0) == PS_OK);
    snapshot = sensor;
    previous = m;
    CHECK(ps_sensor_read(&sensor, 0, (ps_quantity){DBL_MAX, PS_METRE}, &m) == PS_NUMERIC);
    CHECK(!memcmp(&sensor, &snapshot, sizeof sensor) && !memcmp(&m, &previous, sizeof m));
    puts("Measurement distributions, timing, units, uncertainty, dropout and transactional state "
         "passed.");
    return 0;
}
