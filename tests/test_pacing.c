#include "pacing.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "Pacing line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void) {
    CHECK(ps_speed_valid(0) && ps_speed_valid(.1) && ps_speed_valid(16));
    CHECK(!ps_speed_valid(NAN) && !ps_speed_valid(INFINITY) && !ps_speed_valid(-1) &&
          !ps_speed_valid(.09) && !ps_speed_valid(16.01));
    ps_pacer p = {.speed = 1, .running = true};
    /* Irregular rendering intervals leave fixed steps in the accumulator. */
    CHECK(!ps_pacer_due(&p, .003, .01));
    CHECK(!ps_pacer_due(&p, .007, .01));
    CHECK(ps_pacer_due(&p, .016, .01));
    CHECK(!ps_pacer_due(&p, .016, .01));
    CHECK(ps_pacer_due(&p, .026, .01));
    /* A suspended reader cannot induce a many-second catch-up burst. */
    unsigned steps = 0;
    while (ps_pacer_due(&p, 10, .01)) steps++;
    CHECK(steps >= 24 && steps <= 25);
    CHECK(!ps_pacer_due(&p, 10, .01));
    /* Pause and speed changes never spend time accumulated before the change. */
    p.running = false;
    CHECK(!ps_pacer_due(&p, 20, .01) && p.credit == 0);
    p.speed = 4;
    ps_pacer_restart(&p, 20);
    p.running = true;
    CHECK(!ps_pacer_due(&p, 20, .01));
    CHECK(ps_pacer_due(&p, 20.003, .01));
    p.speed = .1;
    ps_pacer_restart(&p, 0);
    /* Large dt must progress even when the catch-up cap is shorter than dt. */
    for (unsigned i = 1; i <= 39; i++) CHECK(!ps_pacer_due(&p, i * .25, 1));
    CHECK(ps_pacer_due(&p, 10.01, 1));
    p.speed = 0;
    ps_pacer_restart(&p,10.01);
    for(unsigned i=0;i<100000;i++)CHECK(ps_pacer_due(&p,10.01,.01));
    CHECK(p.last==10.01 && p.credit==0);
    p.running = false;
    CHECK(!ps_pacer_due(&p, 10.01, .01));
    p=(ps_pacer){.running=true,.speed=1};
    CHECK(ps_pacer_due(&p,.1,.1) && p.credit==0);
    ps_pacer_refund(&p,.1,.03);
    CHECK(fabs(p.credit-.07)<1e-12 && ps_pacer_due(&p,.1,.05));
    CHECK(!ps_pacer_due(&p,.1,.03));
    p.running=false;double credit=p.credit;ps_pacer_refund(&p,.1,.01);CHECK(p.credit==credit);
    puts("Fixed-step pacing, speed changes, paused/offline modes and bounded debt passed.");
    return 0;
}
