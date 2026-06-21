#include "config.h"
#include "simulation.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>

/* ── entry point ─────────────────────────────────────────── */

void test_pickStartingXI(void)
{
    printf("Hello World");
}

void test_simulation(void)
{
    printf("========== simulation tests ==========\n");

    /* seed once for the whole suite */
    srand(42); /* fixed seed so dominant-team test is reproducible */

    test_pickStartingXI();
    // test_calculateTeamStrength();
    // test_simulateMatch();
    // test_generateGoalEvents();
    // test_playbackEvents();

    // print_summary();
}
