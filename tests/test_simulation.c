#include "config.h"
#include "simulation.h"
#include "structs.h"
#include "test_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── stub data builders ──────────────────────────────────── */

/* builds a minimal squad for one country
 * country_id 1 → player ids 101-123, gk ids start at 1
 * country_id 2 → player ids 201-223, gk ids start at 4 */
static void buildSquad(int          country_id,
                       Player      *players,
                       int         *p_count,
                       Goalkeeper  *gks,
                       int         *g_count,
                       PlayerStats *pstats,
                       int         *ps_count,
                       GKStats     *gkstats,
                       int         *gs_count)
{
    int base_id = country_id * 100;
    int gk_base = (country_id - 1) * GKS_PER_TEAM;
    int shirt   = 1;

    /* 4 DEF */
    for (int i = 0; i < 4; i++, shirt++)
    {
        int idx                 = *p_count;
        players[idx].id         = base_id + shirt;
        players[idx].country_id = country_id;
        players[idx].age        = 25;
        players[idx].position   = DEF;
        players[idx].speed      = 70 + i;
        players[idx].passing    = 65 + i;
        players[idx].shooting   = 60 + i;
        players[idx].defending  = 80 + i;
        players[idx].physical   = 75 + i;
        players[idx].dribbling  = 62 + i;
        snprintf(players[idx].name, NAME_LEN, "DEF%d_C%d", i + 1, country_id);

        pstats[idx].tournament_id  = TOURNAMENT_ID;
        pstats[idx].player_id      = players[idx].id;
        pstats[idx].goals          = 0;
        pstats[idx].assists        = 0;
        pstats[idx].matches_played = 0;
        pstats[idx].is_injured     = 0;

        (*p_count)++;
        (*ps_count)++;
    }

    /* 3 MID */
    for (int i = 0; i < 3; i++, shirt++)
    {
        int idx                 = *p_count;
        players[idx].id         = base_id + shirt;
        players[idx].country_id = country_id;
        players[idx].age        = 24;
        players[idx].position   = MID;
        players[idx].speed      = 72 + i;
        players[idx].passing    = 82 + i;
        players[idx].shooting   = 70 + i;
        players[idx].defending  = 65 + i;
        players[idx].physical   = 74 + i;
        players[idx].dribbling  = 78 + i;
        snprintf(players[idx].name, NAME_LEN, "MID%d_C%d", i + 1, country_id);

        pstats[idx].tournament_id  = TOURNAMENT_ID;
        pstats[idx].player_id      = players[idx].id;
        pstats[idx].goals          = 0;
        pstats[idx].assists        = 0;
        pstats[idx].matches_played = 0;
        pstats[idx].is_injured     = 0;

        (*p_count)++;
        (*ps_count)++;
    }

    /* 3 FWD */
    for (int i = 0; i < 3; i++, shirt++)
    {
        int idx                 = *p_count;
        players[idx].id         = base_id + shirt;
        players[idx].country_id = country_id;
        players[idx].age        = 23;
        players[idx].position   = FWD;
        players[idx].speed      = 85 + i;
        players[idx].passing    = 75 + i;
        players[idx].shooting   = 88 + i;
        players[idx].defending  = 55 + i;
        players[idx].physical   = 76 + i;
        players[idx].dribbling  = 84 + i;
        snprintf(players[idx].name, NAME_LEN, "FWD%d_C%d", i + 1, country_id);

        pstats[idx].tournament_id  = TOURNAMENT_ID;
        pstats[idx].player_id      = players[idx].id;
        pstats[idx].goals          = 0;
        pstats[idx].assists        = 0;
        pstats[idx].matches_played = 0;
        pstats[idx].is_injured     = 0;

        (*p_count)++;
        (*ps_count)++;
    }

    /* 3 GK */
    for (int i = 0; i < GKS_PER_TEAM; i++)
    {
        int idx              = gk_base + i;
        gks[idx].id          = gk_base + i + 1;
        gks[idx].country_id  = country_id;
        gks[idx].age         = 28;
        gks[idx].position    = GK;
        gks[idx].diving      = 80 + i;
        gks[idx].handling    = 78 + i;
        gks[idx].kicking     = 70 + i;
        gks[idx].reflexes    = 82 + i;
        gks[idx].speed       = 65 + i;
        gks[idx].positioning = 80 + i;
        snprintf(gks[idx].name, NAME_LEN, "GK%d_C%d", i + 1, country_id);

        gkstats[idx].tournament_id  = TOURNAMENT_ID;
        gkstats[idx].gk_id          = gks[idx].id;
        gkstats[idx].clean_sheets   = 0;
        gkstats[idx].goals_conceded = 0;
        gkstats[idx].matches_played = 0;
        gkstats[idx].is_injured     = 0;

        (*g_count)++;
        (*gs_count)++;
    }
}

static void buildTwoTeams(Player      *players,
                          int         *p_count,
                          Goalkeeper  *gks,
                          int         *g_count,
                          PlayerStats *pstats,
                          int         *ps_count,
                          GKStats     *gkstats,
                          int         *gs_count)
{
    *p_count = *g_count = *ps_count = *gs_count = 0;
    buildSquad(1, players, p_count, gks, g_count, pstats, ps_count, gkstats, gs_count);
    buildSquad(2, players, p_count, gks, g_count, pstats, ps_count, gkstats, gs_count);
}

/* ── test 1: pickStartingXI ──────────────────────────────── */

void test_pickStartingXI(void)
{
    printf("\n--- pickStartingXI ---\n");

    Player      players[MAX_PLAYERS];
    Goalkeeper  gks[MAX_GKS];
    PlayerStats pstats[MAX_PLAYERS];
    GKStats     gkstats[MAX_GKS];
    int         p_count, g_count, ps_count, gs_count;
    buildTwoTeams(players, &p_count, gks, &g_count, pstats, &ps_count, gkstats, &gs_count);

    int xi[XI_OUTFIELD], gk_id;

    /* basic success */
    int result = pickStartingXI(1, xi, &gk_id, players, p_count, gks, g_count, pstats, gkstats);
    CHECK("returns 1 for valid squad", result == 1);
    CHECK("gk_id > 0", gk_id > 0);
    CHECK("xi has XI_OUTFIELD entries", xi[XI_OUTFIELD - 1] != 0);

    /* correct counts per position */
    int def_c = 0, mid_c = 0, fwd_c = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
    {
        /* derive country from id */
        int pidx = -1;
        for (int j = 0; j < p_count; j++)
            if (players[j].id == xi[i])
            {
                pidx = j;
                break;
            }
        if (pidx == -1)
            continue;
        if (players[pidx].position == DEF)
            def_c++;
        if (players[pidx].position == MID)
            mid_c++;
        if (players[pidx].position == FWD)
            fwd_c++;
    }
    CHECK("XI has 4 DEFs", def_c == XI_DEF);
    CHECK("XI has 3 MIDs", mid_c == XI_MID);
    CHECK("XI has 3 FWDs", fwd_c == XI_FWD);

    /* all picked players belong to correct country */
    int wrong_country = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
        if (xi[i] / 100 != 1)
            wrong_country++;
    CHECK("all outfield from correct country", wrong_country == 0);

    /* GK belongs to correct country */
    int gk_country = 0;
    for (int i = 0; i < g_count; i++)
        if (gks[i].id == gk_id)
        {
            gk_country = gks[i].country_id;
            break;
        }
    CHECK("GK from correct country", gk_country == 1);

    /* no duplicates in XI */
    int dupes = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
        for (int j = i + 1; j < XI_OUTFIELD; j++)
            if (xi[i] == xi[j])
                dupes++;
    CHECK("no duplicate players in XI", dupes == 0);

    /* injured players excluded */
    /* injure all DEFs of country 1 */
    for (int i = 0; i < ps_count; i++)
        if (pstats[i].player_id / 100 == 1)
        {
            int pidx = -1;
            for (int j = 0; j < p_count; j++)
                if (players[j].id == pstats[i].player_id)
                {
                    pidx = j;
                    break;
                }
            if (pidx != -1 && players[pidx].position == DEF)
                pstats[i].is_injured = 1;
        }
    int result_injured =
        pickStartingXI(1, xi, &gk_id, players, p_count, gks, g_count, pstats, gkstats);
    CHECK("returns 0 when DEFs all injured", result_injured == 0);

    /* best overall player is picked — add a standout DEF and verify chosen */
    /* reset injuries first */
    for (int i = 0; i < ps_count; i++)
        pstats[i].is_injured = 0;

    /* add a clearly superior DEF to country 1 */
    int idx                 = p_count;
    players[idx].id         = 199; /* country 1, shirt 99 */
    players[idx].country_id = 1;
    players[idx].position   = DEF;
    players[idx].speed      = 99;
    players[idx].passing    = 99;
    players[idx].shooting   = 99;
    players[idx].defending  = 99;
    players[idx].physical   = 99;
    players[idx].dribbling  = 99;
    snprintf(players[idx].name, NAME_LEN, "SuperDEF");
    pstats[idx].tournament_id = TOURNAMENT_ID;
    pstats[idx].player_id     = 199;
    pstats[idx].is_injured    = 0;
    p_count++;
    ps_count++;

    pickStartingXI(1, xi, &gk_id, players, p_count, gks, g_count, pstats, gkstats);
    int super_picked = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
        if (xi[i] == 199)
            super_picked = 1;
    CHECK("best overall DEF is always picked", super_picked == 1);
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
