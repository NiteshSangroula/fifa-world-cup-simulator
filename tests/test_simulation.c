#include "config.h"
#include "simulation.h"
#include "structs.h"
#include "test_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── stub data ───────────────────────────────────────────── */

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

/* ── shared fixture ──────────────────────────────────────── */

typedef struct
{
    Player      players[MAX_PLAYERS];
    Goalkeeper  gks[MAX_GKS];
    PlayerStats pstats[MAX_PLAYERS];
    GKStats     gkstats[MAX_GKS];
    int         p_count, g_count, ps_count, gs_count;
    StartingXI  xi1, xi2;
} SimFixture;

static void setupFixture(SimFixture *f)
{
    f->p_count = f->g_count = f->ps_count = f->gs_count = 0;
    buildSquad(1,
               f->players,
               &f->p_count,
               f->gks,
               &f->g_count,
               f->pstats,
               &f->ps_count,
               f->gkstats,
               &f->gs_count);
    buildSquad(2,
               f->players,
               &f->p_count,
               f->gks,
               &f->g_count,
               f->pstats,
               &f->ps_count,
               f->gkstats,
               &f->gs_count);

    pickStartingXI(1,
                   &f->xi1,
                   f->players,
                   f->p_count,
                   f->gks,
                   f->g_count,
                   f->pstats,
                   f->ps_count,
                   f->gkstats,
                   f->gs_count);
    pickStartingXI(2,
                   &f->xi2,
                   f->players,
                   f->p_count,
                   f->gks,
                   f->g_count,
                   f->pstats,
                   f->ps_count,
                   f->gkstats,
                   f->gs_count);
}

/* ── test: pickStartingXI ────────────────────────────────── */

void test_pickStartingXI(void)
{
    printf("\n--- pickStartingXI ---\n");

    SimFixture f;
    setupFixture(&f);

    /* basic success */
    CHECK("xi1 picked successfully", f.xi1.gk_id > 0);
    CHECK("xi2 picked successfully", f.xi2.gk_id > 0);

    /* correct position counts */
    int def_c = 0, mid_c = 0, fwd_c = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
    {
        for (int j = 0; j < f.p_count; j++)
        {
            if (f.players[j].id != f.xi1.xi_ids[i])
                continue;
            if (f.players[j].position == DEF)
                def_c++;
            if (f.players[j].position == MID)
                mid_c++;
            if (f.players[j].position == FWD)
                fwd_c++;
            break;
        }
    }
    CHECK("XI has 4 DEFs", def_c == XI_DEF);
    CHECK("XI has 3 MIDs", mid_c == XI_MID);
    CHECK("XI has 3 FWDs", fwd_c == XI_FWD);

    /* overalls stored correctly — must be in stat range */
    int ovr_valid = 1;
    for (int i = 0; i < XI_DEF; i++)
        if (f.xi1.def_ovr[i] < STAT_MIN || f.xi1.def_ovr[i] > STAT_MAX)
            ovr_valid = 0;
    for (int i = 0; i < XI_MID; i++)
        if (f.xi1.mid_ovr[i] < STAT_MIN || f.xi1.mid_ovr[i] > STAT_MAX)
            ovr_valid = 0;
    for (int i = 0; i < XI_FWD; i++)
        if (f.xi1.fwd_ovr[i] < STAT_MIN || f.xi1.fwd_ovr[i] > STAT_MAX)
            ovr_valid = 0;
    CHECK("all stored overalls in stat range", ovr_valid == 1);

    /* GK overall stored */
    CHECK("gk_ovr in stat range", f.xi1.gk_ovr >= STAT_MIN && f.xi1.gk_ovr <= STAT_MAX);

    /* all outfield from correct country */
    int wrong_country = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
        if (f.xi1.xi_ids[i] / 100 != 1)
            wrong_country++;
    CHECK("all outfield from correct country", wrong_country == 0);

    /* GK from correct country */
    int gk_country = 0;
    for (int i = 0; i < f.g_count; i++)
        if (f.gks[i].id == f.xi1.gk_id)
        {
            gk_country = f.gks[i].country_id;
            break;
        }
    CHECK("GK from correct country", gk_country == 1);

    /* no duplicates */
    int dupes = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
        for (int j = i + 1; j < XI_OUTFIELD; j++)
            if (f.xi1.xi_ids[i] == f.xi1.xi_ids[j])
                dupes++;
    CHECK("no duplicate players in XI", dupes == 0);

    /* best player always picked */
    f.players[f.p_count].id         = 199;
    f.players[f.p_count].country_id = 1;
    f.players[f.p_count].position   = DEF;
    f.players[f.p_count].speed      = 99;
    f.players[f.p_count].passing    = 99;
    f.players[f.p_count].shooting   = 99;
    f.players[f.p_count].defending  = 99;
    f.players[f.p_count].physical   = 99;
    f.players[f.p_count].dribbling  = 99;
    snprintf(f.players[f.p_count].name, NAME_LEN, "SuperDEF");
    f.pstats[f.ps_count].tournament_id = TOURNAMENT_ID;
    f.pstats[f.ps_count].player_id     = 199;
    f.pstats[f.ps_count].is_injured    = 0;
    f.p_count++;
    f.ps_count++;

    StartingXI xi_test;
    pickStartingXI(1,
                   &xi_test,
                   f.players,
                   f.p_count,
                   f.gks,
                   f.g_count,
                   f.pstats,
                   f.ps_count,
                   f.gkstats,
                   f.gs_count);
    int super_picked = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
        if (xi_test.xi_ids[i] == 199)
        {
            super_picked = 1;
            break;
        }
    CHECK("best overall DEF always picked", super_picked == 1);

    /* injured player excluded */
    for (int i = 0; i < f.ps_count; i++)
        if (f.pstats[i].player_id == 199)
        {
            f.pstats[i].is_injured = 1;
            break;
        }
    pickStartingXI(1,
                   &xi_test,
                   f.players,
                   f.p_count,
                   f.gks,
                   f.g_count,
                   f.pstats,
                   f.ps_count,
                   f.gkstats,
                   f.gs_count);
    int injured_picked = 0;
    for (int i = 0; i < XI_OUTFIELD; i++)
        if (xi_test.xi_ids[i] == 199)
        {
            injured_picked = 1;
            break;
        }
    CHECK("injured player not picked", injured_picked == 0);

    /* returns 0 when position can't be filled */
    for (int i = 0; i < f.ps_count; i++)
        for (int j = 0; j < f.p_count; j++)
            if (f.players[j].id == f.pstats[i].player_id && f.players[j].country_id == 1 &&
                f.players[j].position == DEF)
                f.pstats[i].is_injured = 1;

    int result = pickStartingXI(1,
                                &xi_test,
                                f.players,
                                f.p_count,
                                f.gks,
                                f.g_count,
                                f.pstats,
                                f.ps_count,
                                f.gkstats,
                                f.gs_count);
    CHECK("returns 0 when DEFs all injured", result == 0);
}

/* ── test: calculateTeamStrength ─────────────────────────── */

void test_calculateTeamStrength(void)
{
    printf("\n--- calculateTeamStrength ---\n");

    SimFixture f;
    setupFixture(&f);

    float s1 = calculateTeamStrength(&f.xi1);
    float s2 = calculateTeamStrength(&f.xi2);

    CHECK("strength > 0", s1 > 0.0f);
    CHECK("strength in realistic range", s1 >= 55.0f && s1 <= 99.0f);
    CHECK("both teams produce a strength", s2 > 0.0f);

    /* equal squads — similar strength */
    float diff = s1 - s2;
    if (diff < 0.0f)
        diff = -diff;
    CHECK("equal squads have similar strength", diff < 5.0f);

    /* 55/45 weighting — boost only DEFs of team1, atk stays same */
    /* s1 should increase when defence improves */
    float s1_before = s1;
    for (int i = 0; i < XI_DEF; i++)
        f.xi1.def_ovr[i] = 99;
    f.xi1.gk_ovr   = 99;
    float s1_after = calculateTeamStrength(&f.xi1);
    CHECK("improving defence raises strength", s1_after > s1_before);

    /* boost only attack */
    SimFixture f2;
    setupFixture(&f2);
    float s_before = calculateTeamStrength(&f2.xi1);
    for (int i = 0; i < XI_FWD; i++)
        f2.xi1.fwd_ovr[i] = 99;
    for (int i = 0; i < XI_MID; i++)
        f2.xi1.mid_ovr[i] = 99;
    float s_after = calculateTeamStrength(&f2.xi1);
    CHECK("improving attack raises strength", s_after > s_before);

    /* max possible strength sanity check */
    StartingXI max_xi;
    for (int i = 0; i < XI_DEF; i++)
        max_xi.def_ovr[i] = 99;
    for (int i = 0; i < XI_MID; i++)
        max_xi.mid_ovr[i] = 99;
    for (int i = 0; i < XI_FWD; i++)
        max_xi.fwd_ovr[i] = 99;
    max_xi.gk_ovr = 99;
    float s_max   = calculateTeamStrength(&max_xi);
    CHECK("max stats gives ~99 strength", s_max > 95.0f && s_max <= 99.0f);
}

/* ── test: simulateMatch ─────────────────────────────────── */

void test_simulateMatch(void)
{
    printf("\n--- simulateMatch ---\n");

    SimFixture f;
    setupFixture(&f);

    int s1, s2;

    /* basic sanity — scores non-negative */
    simulateMatch(1,
                  2,
                  0,
                  &s1,
                  &s2,
                  NULL,
                  NULL,
                  f.players,
                  f.p_count,
                  f.gks,
                  f.g_count,
                  f.pstats,
                  f.ps_count,
                  f.gkstats,
                  f.gs_count);
    CHECK("score1 >= 0", s1 >= 0);
    CHECK("score2 >= 0", s2 >= 0);

    /* run 100 group stage matches — realism checks */
    int wins1 = 0, wins2 = 0, draws = 0;
    int total_goals = 0;
    int max_score   = 0;
    int scoreless   = 0;

    for (int i = 0; i < 100; i++)
    {
        simulateMatch(1,
                      2,
                      0,
                      &s1,
                      &s2,
                      NULL,
                      NULL,
                      f.players,
                      f.p_count,
                      f.gks,
                      f.g_count,
                      f.pstats,
                      f.ps_count,
                      f.gkstats,
                      f.gs_count);
        if (s1 > s2)
            wins1++;
        else if (s2 > s1)
            wins2++;
        else
            draws++;

        total_goals += s1 + s2;
        if (s1 > max_score)
            max_score = s1;
        if (s2 > max_score)
            max_score = s2;
        if (s1 == 0 && s2 == 0)
            scoreless++;
    }

    float avg_goals = total_goals / 100.0f;

    CHECK("produces team1 wins", wins1 > 0);
    CHECK("produces team2 wins", wins2 > 0);
    CHECK("produces draws", draws > 0);
    CHECK("avg goals realistic (1.5-4.0)", avg_goals >= 1.5f && avg_goals <= 4.0f);
    CHECK("max single score realistic (<=6)", max_score <= 6);
    CHECK("not all matches 0-0", scoreless < 30);

    /* out_xi populated correctly */
    StartingXI out1, out2;
    simulateMatch(1,
                  2,
                  0,
                  &s1,
                  &s2,
                  &out1,
                  &out2,
                  f.players,
                  f.p_count,
                  f.gks,
                  f.g_count,
                  f.pstats,
                  f.ps_count,
                  f.gkstats,
                  f.gs_count);
    CHECK("out_xi1 gk_id populated", out1.gk_id > 0);
    CHECK("out_xi2 gk_id populated", out2.gk_id > 0);
    CHECK("out_xi1 first id from country 1", out1.xi_ids[0] / 100 == 1);
    CHECK("out_xi2 first id from country 2", out2.xi_ids[0] / 100 == 2);

    /* NULL out params don't crash */
    simulateMatch(1,
                  2,
                  0,
                  &s1,
                  &s2,
                  NULL,
                  NULL,
                  f.players,
                  f.p_count,
                  f.gks,
                  f.g_count,
                  f.pstats,
                  f.ps_count,
                  f.gkstats,
                  f.gs_count);
    CHECK("NULL out params don't crash", 1);

    /* knockout — no draws */
    int knockout_draws = 0;
    for (int i = 0; i < 50; i++)
    {
        simulateMatch(1,
                      2,
                      1,
                      &s1,
                      &s2,
                      NULL,
                      NULL,
                      f.players,
                      f.p_count,
                      f.gks,
                      f.g_count,
                      f.pstats,
                      f.ps_count,
                      f.gkstats,
                      f.gs_count);
        if (s1 == s2)
            knockout_draws++;
    }
    CHECK("knockout produces no draws", knockout_draws == 0);

    /* dominant team wins more often */
    for (int i = 0; i < f.p_count; i++)
        if (f.players[i].country_id == 1)
        {
            f.players[i].shooting  = 99;
            f.players[i].passing   = 99;
            f.players[i].defending = 99;
            f.players[i].physical  = 99;
            f.players[i].speed     = 99;
            f.players[i].dribbling = 99;
        }
    for (int i = 0; i < f.g_count; i++)
        if (f.gks[i].country_id == 1)
        {
            f.gks[i].reflexes    = 99;
            f.gks[i].positioning = 99;
            f.gks[i].diving      = 99;
            f.gks[i].handling    = 99;
        }

    int dominant_wins = 0;
    for (int i = 0; i < 30; i++)
    {
        simulateMatch(1,
                      2,
                      0,
                      &s1,
                      &s2,
                      NULL,
                      NULL,
                      f.players,
                      f.p_count,
                      f.gks,
                      f.g_count,
                      f.pstats,
                      f.ps_count,
                      f.gkstats,
                      f.gs_count);
        if (s1 > s2)
            dominant_wins++;
    }
    CHECK("dominant team wins majority", dominant_wins >= 16); /* >50% of 30 */
}

/* ── test: generateGoalEvents ────────────────────────────── */

void test_generateGoalEvents(void)
{
    printf("\n--- generateGoalEvents ---\n");

    SimFixture f;
    setupFixture(&f);

    GoalEvent events[MAX_GOAL_EVENTS];
    int       count;

    /* 0-0 */
    count = generateGoalEvents(1, 2, 0, 0, &f.xi1, &f.xi2, f.players, f.p_count, events);
    CHECK("0-0 produces 0 events", count == 0);

    /* event count matches total goals */
    count = generateGoalEvents(1, 2, 2, 1, &f.xi1, &f.xi2, f.players, f.p_count, events);
    CHECK("2-1 produces 3 events", count == 3);

    /* correct team attribution */
    int t1_goals = 0, t2_goals = 0;
    for (int i = 0; i < count; i++)
    {
        if (events[i].team_id == 1)
            t1_goals++;
        if (events[i].team_id == 2)
            t2_goals++;
    }
    CHECK("team1 credited 2 goals", t1_goals == 2);
    CHECK("team2 credited 1 goal", t2_goals == 1);

    /* sorted by minute */
    int sorted = 1;
    for (int i = 1; i < count; i++)
        if (events[i].minute < events[i - 1].minute)
        {
            sorted = 0;
            break;
        }
    CHECK("events sorted by minute", sorted == 1);

    /* minutes in valid range */
    int valid_min = 1;
    for (int i = 0; i < count; i++)
        if (events[i].minute < 1 || events[i].minute > 90)
        {
            valid_min = 0;
            break;
        }
    CHECK("all minutes in range 1-90", valid_min == 1);

    /* all scorer ids valid */
    int invalid = 0;
    for (int i = 0; i < count; i++)
    {
        int found = 0;
        for (int j = 0; j < f.p_count; j++)
            if (f.players[j].id == events[i].player_id)
            {
                found = 1;
                break;
            }
        if (!found)
            invalid++;
    }
    CHECK("all scorer ids are valid", invalid == 0);

    /* scorers from correct team */
    int wrong_team = 0;
    for (int i = 0; i < count; i++)
        if (events[i].player_id / 100 != events[i].team_id)
            wrong_team++;
    CHECK("all scorers from correct team", wrong_team == 0);

    /* assister_id always -1 */
    int bad_assist = 0;
    for (int i = 0; i < count; i++)
        if (events[i].assister_id != -1)
            bad_assist++;
    CHECK("assister_id is -1 for all", bad_assist == 0);

    /* FWDs score more than DEFs over many trials */
    int fwd_goals = 0, def_goals = 0;
    for (int run = 0; run < 50; run++)
    {
        int c = generateGoalEvents(1, 2, 3, 3, &f.xi1, &f.xi2, f.players, f.p_count, events);
        for (int i = 0; i < c; i++)
            for (int j = 0; j < f.p_count; j++)
            {
                if (f.players[j].id != events[i].player_id)
                    continue;
                if (f.players[j].position == FWD)
                    fwd_goals++;
                if (f.players[j].position == DEF)
                    def_goals++;
                break;
            }
    }
    CHECK("FWDs score more than DEFs", fwd_goals > def_goals);
}

/* ── test: playbackEvents ────────────────────────────────── */

void test_playbackEvents(void)
{
    printf("\n--- playbackEvents ---\n");

    SimFixture f;
    setupFixture(&f);

    Country countries[2];
    countries[0].id = 1;
    strncpy(countries[0].name, "France", NAME_LEN - 1);
    countries[1].id = 2;
    strncpy(countries[1].name, "Brazil", NAME_LEN - 1);

    GoalEvent events[MAX_GOAL_EVENTS];
    int       count;

    /* 0-0 no crash */
    printf("  [0-0 output]\n");
    playbackEvents(events, 0, f.players, f.p_count, countries, 2, 1, 2, 0, 0);
    CHECK("0-0 does not crash", 1);

    /* normal match */
    count = generateGoalEvents(1, 2, 2, 1, &f.xi1, &f.xi2, f.players, f.p_count, events);
    printf("  [2-1 output]\n");
    playbackEvents(events, count, f.players, f.p_count, countries, 2, 1, 2, 2, 1);
    CHECK("2-1 does not crash", 1);

    /* all 3 goals from team1 in a 3-0 */
    count      = generateGoalEvents(1, 2, 3, 0, &f.xi1, &f.xi2, f.players, f.p_count, events);
    int all_t1 = 1;
    for (int i = 0; i < count; i++)
        if (events[i].team_id != 1)
        {
            all_t1 = 0;
            break;
        }
    CHECK("3-0 all events belong to team1", all_t1 == 1);

    /* unknown country ids — fallback name no crash */
    printf("  [unknown country output]\n");
    playbackEvents(events, count, f.players, f.p_count, countries, 2, 99, 98, 3, 0);
    CHECK("unknown country ids don't crash", 1);

    /* empty player array — Unknown scorer no crash */
    Player empty[1];
    printf("  [unknown scorer output]\n");
    playbackEvents(events, count, empty, 0, countries, 2, 1, 2, 3, 0);
    CHECK("empty player array doesn't crash", 1);
}

/* ── entry point ─────────────────────────────────────────── */

void test_simulation(void)
{
    printf("========== simulation tests ==========\n");
    srand(42);

    test_pickStartingXI();
    test_calculateTeamStrength();
    test_simulateMatch();
    test_generateGoalEvents();
    test_playbackEvents();

    print_summary();
}
