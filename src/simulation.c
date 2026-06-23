#include "simulation.h"

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    int player_index;
    int sort_stat;
} Candidate;

// descending comparator for overall
static int cmpCandidatesDesc(const void *a, const void *b)
{
    return ((Candidate *)b)->sort_stat - ((Candidate *)a)->sort_stat;
}

// ascending comparator for goal scored by minute
static int cmpGoalEventsByMinute(const void *a, const void *b)
{
    return ((GoalEvent *)a)->minute - ((GoalEvent *)b)->minute;
}

// returns overall of players based on position fwd, mid, def
// important stats for a position have higher weight
static int overallRating(const Player *p)
{
    switch (p->position)
    {
    case DEF:
        return (p->defending * 40 + p->physical * 25 + p->speed * 15 + p->passing * 12 +
                p->dribbling * 5 + p->shooting * 3) /
               100;

    case MID:
        return (p->passing * 30 + p->dribbling * 20 + p->physical * 15 + p->defending * 15 +
                p->shooting * 10 + p->speed * 10) /
               100;

    case FWD:
        return (p->shooting * 35 + p->speed * 20 + p->dribbling * 15 + p->physical * 15 +
                p->passing * 10 + p->defending * 5) /
               100;

    default:
        return 0;
    }
}

// overall of gk
static int overallGK(const Goalkeeper *gk)
{
    return (gk->reflexes * 25 + gk->diving * 20 + gk->positioning * 20 + gk->handling * 15 +
            gk->kicking * 12 + gk->speed * 8) /
           100;
}

/* ── internal helpers ────────────────────────────────────── */

/* finds pstats index for a given player_id, returns -1 if not found */
static int findPstatsIndex(int player_id, PlayerStats *pstats, int ps_count)
{
    for (int i = 0; i < ps_count; i++)
        if (pstats[i].player_id == player_id)
            return i;
    return -1;
}

/* finds gkstats index for a given gk_id, returns -1 if not found */
static int findGKStatsIndex(int gk_id, GKStats *gkstats, int gs_count)
{
    for (int i = 0; i < gs_count; i++)
        if (gkstats[i].gk_id == gk_id)
            return i;
    return -1;
}

/* finds player array index by id, returns -1 if not found */
static int findPlayerIndex(int player_id, Player *players, int p_count)
{
    /* data ID structure as hint: country = id/100, shirt = id%100 */
    int country_id = player_id / 100;
    int shirt      = player_id % 100;

    /* estimate starting index — players grouped by country */
    /* country N starts at roughly (N-1)*OUTFIELD_PER_TEAM */
    int hint = (country_id - 1) * OUTFIELD_PER_TEAM + (shirt - 1);
    if (hint >= 0 && hint < p_count && players[hint].id == player_id)
        return hint;

    /* fallback: linear scan */
    for (int i = 0; i < p_count; i++)
        if (players[i].id == player_id)
            return i;
    return -1;
}

// pick the best starting 11 from the whole player array
/*
 * first find all defs, mids, fwds, gks of the country
 * then sort them in ascending order based on ovr
 * pick the 4 top def, 1 gk, 3 mid, 3 fwd
 */
// Refactor
int pickStartingXI(int          country_id,
                   StartingXI  *out,
                   Player      *players,
                   int          p_count,
                   Goalkeeper  *gks,
                   int          g_count,
                   PlayerStats *pstats,
                   int          ps_count,
                   GKStats     *gkstats,
                   int          gs_count)
{
    Candidate defs[OUTFIELD_PER_TEAM];
    Candidate mids[OUTFIELD_PER_TEAM];
    Candidate fwds[OUTFIELD_PER_TEAM];
    size_t    def_count = 0, mid_count = 0, fwd_count = 0;

    // load all players from given country id to their respective positions
    for (int i = 0; i < p_count; i++)
    {
        if (players[i].country_id != country_id)
            continue;
        int pi = findPstatsIndex(players[i].id, pstats, ps_count);
        if (pi != -1 && pstats[pi].is_injured)
            continue;

        int ovr = overallRating(&players[i]);

        switch (players[i].position)
        {
        case DEF:
            defs[def_count].player_index = i;
            defs[def_count].sort_stat    = ovr;
            def_count++;
            break;
        case MID:
            mids[mid_count].player_index = i;
            mids[mid_count].sort_stat    = ovr;
            mid_count++;
            break;
        case FWD:
            fwds[fwd_count].player_index = i;
            fwds[fwd_count].sort_stat    = ovr;
            fwd_count++;
            break;
        default:
            break;
        }
    }

    // if not enough players at any position return 0
    if (def_count < XI_DEF || mid_count < XI_MID || fwd_count < XI_FWD)
        return 0;

    // sort players based on ovr in desc order
    qsort(defs, def_count, sizeof(Candidate), cmpCandidatesDesc);
    qsort(mids, mid_count, sizeof(Candidate), cmpCandidatesDesc);
    qsort(fwds, fwd_count, sizeof(Candidate), cmpCandidatesDesc);

    // select the top players at mid, fwd, def
    // fill in StartingXI struct
    for (int i = 0; i < XI_DEF; i++)
    {
        out->xi_ids[i]  = players[defs[i].player_index].id;
        out->def_ovr[i] = defs[i].sort_stat;
    }
    for (int i = 0; i < XI_MID; i++)
    {
        out->xi_ids[XI_DEF + i] = players[mids[i].player_index].id;
        out->mid_ovr[i]         = mids[i].sort_stat;
    }
    for (int i = 0; i < XI_FWD; i++)
    {
        out->xi_ids[XI_DEF + XI_MID + i] = players[fwds[i].player_index].id;
        out->fwd_ovr[i]                  = fwds[i].sort_stat;
    }

    /* GK */
    Candidate gk_pool[GKS_PER_TEAM];
    size_t    gk_pool_size = 0;
    for (int i = 0; i < g_count; i++)
    {
        if (gks[i].country_id != country_id)
            continue;
        int gi = findGKStatsIndex(gks[i].id, gkstats, gs_count);
        if (gi != -1 && gkstats[gi].is_injured)
            continue;
        gk_pool[gk_pool_size].player_index = i;
        gk_pool[gk_pool_size].sort_stat    = overallGK(&gks[i]);
        gk_pool_size++;
    }
    if (gk_pool_size < 1)
        return 0;
    qsort(gk_pool, gk_pool_size, sizeof(Candidate), cmpCandidatesDesc);

    out->gk_id  = gks[gk_pool[0].player_index].id;
    out->gk_ovr = gk_pool[0].sort_stat;

    return 1;
}

/*
 * atk = (ovr of all fwd + ovr of all mid) / (number of fwd + mid)
 * def = (ovr of all def + ovr of the gk)
 * attack is slightly favoured as 99 atk 55 def should perform better
 * than 55 atk and 99 def team on paper
 */
float calculateTeamStrength(StartingXI *xi)
{
    float atk = 0.0f, def = 0.0f;

    for (int i = 0; i < XI_FWD; i++)
        atk += (float)xi->fwd_ovr[i];
    for (int i = 0; i < XI_MID; i++)
        atk += (float)xi->mid_ovr[i];
    atk /= (XI_FWD + XI_MID);

    for (int i = 0; i < XI_DEF; i++)
        def += (float)xi->def_ovr[i];
    def = (def / XI_DEF + (float)xi->gk_ovr) / 2.0f;

    return (atk * 0.55f) + (def * 0.45f);
}

/* ── internal: realistic goal count ─────────────────────────
 * World Cup average: ~2.7 goals/game total
 * Distribution roughly:
 *   0 goals for a team: 35% chance
 *   1 goal:             40% chance
 *   2 goals:            18% chance
 *   3+ goals:            7% chance
 * ─────────────────────────────────────────────────────────── */
static int randomGoalCount(float team_strength, float opponent_strength)
{
    /* attacking pressure = how much stronger attacker is vs opponent defense */
    float pressure = team_strength / (team_strength + opponent_strength);

    /* base roll 0-99 */
    int roll = rand() % 100;

    /* pressure shifts thresholds — stronger team scores more */
    int threshold_0 = (int)(45.0f - (pressure - 0.5f) * 30.0f); /* ~30-60% */
    int threshold_1 = threshold_0 + 38;                         /* ~38% window */
    int threshold_2 = threshold_1 + 16;                         /* ~16% window */
    /* anything above threshold_2 = 3 goals, ~6% */

    if (roll < threshold_0)
        return 0;
    if (roll < threshold_1)
        return 1;
    if (roll < threshold_2)
        return 2;
    return 3;
}

/* ── internal: resolve draw in knockout ─────────────────────  */
static void resolveKnockoutDraw(float s1, float s2, int *score1, int *score2)
{
    /* extra time — slightly higher chance of a goal, winner bias */
    float win_prob    = s1 / (s1 + s2);
    float rand_factor = ((float)rand() / RAND_MAX) * 0.20f - 0.10f;
    float outcome     = win_prob + rand_factor;

    if (outcome > 0.52f)
    {
        (*score1)++; /* team1 wins in ET */
    }
    else if (outcome < 0.48f)
    {
        (*score2)++; /* team2 wins in ET */
    }
    else
    {
        /* penalties — pure coin flip with slight strength bias */
        if (((float)rand() / RAND_MAX) < win_prob)
            (*score1)++;
        else
            (*score2)++;
    }
}

/* ── simulateMatch ───────────────────────────────────────── */

void simulateMatch(int          team1_id,
                   int          team2_id,
                   int          is_knockout,
                   int         *score1,
                   int         *score2,
                   StartingXI  *out_xi1,
                   StartingXI  *out_xi2,
                   Player      *players,
                   int          p_count,
                   Goalkeeper  *gks,
                   int          g_count,
                   PlayerStats *pstats,
                   int          ps_count,
                   GKStats     *gkstats,
                   int          gs_count)
{
    StartingXI xi1, xi2;

    int ok1 = pickStartingXI(
        team1_id, &xi1, players, p_count, gks, g_count, pstats, ps_count, gkstats, gs_count);
    int ok2 = pickStartingXI(
        team2_id, &xi2, players, p_count, gks, g_count, pstats, ps_count, gkstats, gs_count);

    float s1 = ok1 ? calculateTeamStrength(&xi1) : 65.0f;
    float s2 = ok2 ? calculateTeamStrength(&xi2) : 65.0f;

    /* realistic goal counts — each team's score depends on
     * their attack vs opponent's defence component */
    float atk1 = s1 * 0.55f;
    float def2 = s2 * 0.45f;
    float atk2 = s2 * 0.55f;
    float def1 = s1 * 0.45f;

    *score1 = randomGoalCount(atk1, def2);
    *score2 = randomGoalCount(atk2, def1);

    /* enforce result consistency with a strength-biased nudge:
     * if scores are equal, stronger team has a chance to edge it */
    if (*score1 == *score2)
    {
        float win_prob    = s1 / (s1 + s2);
        float rand_factor = ((float)rand() / RAND_MAX) * RAND_SWING - RAND_OFFSET;
        float outcome     = win_prob + rand_factor;

        /* only nudge if there's a clear favourite (outcome far from 0.5) */
        if (outcome > 0.60f && *score1 > 0)
            (*score1)++; /* stronger team scores one more */
        else if (outcome < 0.40f && *score2 > 0)
            (*score2)++;
        /* else leave as a draw — upsets and draws are realistic */
    }

    /* knockout: no draws allowed */
    if (is_knockout && *score1 == *score2)
        resolveKnockoutDraw(s1, s2, score1, score2);

    if (out_xi1)
        *out_xi1 = xi1;
    if (out_xi2)
        *out_xi2 = xi2;
}

/* ── 4. generateGoalEvents ───────────────────────────────── */

int generateGoalEvents(int         team1_id,
                       int         team2_id,
                       int         score1,
                       int         score2,
                       StartingXI *xi1,
                       StartingXI *xi2,
                       Player     *players,
                       int         p_count,
                       GoalEvent  *events)
{
    size_t event_count = 0;

    StartingXI *xis[2]      = {xi1, xi2};
    int         team_ids[2] = {team1_id, team2_id};
    int         scores[2]   = {score1, score2};

    for (int t = 0; t < 2; t++)
    {
        int pool_ids[XI_OUTFIELD];
        int pool_weights[XI_OUTFIELD];
        int total_weight = 0;
        int pool_size    = 0;

        for (int i = 0; i < XI_OUTFIELD; i++)
        {
            int idx = findPlayerIndex(xis[t]->xi_ids[i], players, p_count);
            if (idx == -1)
                continue;

            int weight = 0;
            switch (players[idx].position)
            {
            case FWD:
                weight = players[idx].shooting;
                break;
            case MID:
                weight = players[idx].shooting / 2;
                break;
            case DEF:
                weight = players[idx].shooting / 6;
                break;
            default:
                weight = 1;
                break;
            }
            if (weight < 1)
                weight = 1;

            pool_ids[pool_size]     = players[idx].id;
            pool_weights[pool_size] = weight;
            total_weight += weight;
            pool_size++;
        }

        if (pool_size == 0 || total_weight == 0)
            continue;

        for (int g = 0; g < scores[t]; g++)
        {
            int pick      = rand() % total_weight;
            int acc       = 0;
            int scorer_id = pool_ids[0];

            for (int i = 0; i < pool_size; i++)
            {
                acc += pool_weights[i];
                if (pick < acc)
                {
                    scorer_id = pool_ids[i];
                    break;
                }
            }

            events[event_count].minute      = (rand() % 90) + 1;
            events[event_count].player_id   = scorer_id;
            events[event_count].assister_id = -1;
            events[event_count].team_id     = team_ids[t];
            events[event_count].is_own_goal = 0;
            event_count++;
        }
    }

    // sort goals based on minutes
    qsort(events, event_count, sizeof(GoalEvent), cmpGoalEventsByMinute);
    return (int)event_count;
}

/* ── 5. playbackEvents ───────────────────────────────────── */

void playbackEvents(GoalEvent *events,
                    int        count,
                    Player    *players,
                    int        p_count,
                    Country   *countries,
                    int        c_count,
                    int        team1_id,
                    int        team2_id,
                    int        score1,
                    int        score2)
{
    char team1_name[NAME_LEN] = "Team 1";
    char team2_name[NAME_LEN] = "Team 2";

    for (int i = 0; i < c_count; i++)
    {
        if (countries[i].id == team1_id)
            strncpy(team1_name, countries[i].name, NAME_LEN - 1);
        if (countries[i].id == team2_id)
            strncpy(team2_name, countries[i].name, NAME_LEN - 1);
    }

    printf("\n  %s vs %s\n", team1_name, team2_name);
    printf("  ─────────────────────────────────────\n");

    if (count == 0)
    {
        printf("  No goals scored.\n");
        printf("  ─────────────────────────────────────\n");
        printf("  FT: %s 0 - 0 %s\n\n", team1_name, team2_name);
        return;
    }

    int running1 = 0, running2 = 0;

    for (int i = 0; i < count; i++)
    {
        char scorer_name[NAME_LEN] = "Unknown";
        int  idx                   = findPlayerIndex(events[i].player_id, players, p_count);
        if (idx != -1)
            strncpy(scorer_name, players[idx].name, NAME_LEN - 1);

        if (events[i].team_id == team1_id)
            running1++;
        else
            running2++;

        printf("  %2d'  %-24s (%s)  %d - %d\n",
               events[i].minute,
               scorer_name,
               events[i].team_id == team1_id ? team1_name : team2_name,
               running1,
               running2);

        sleep_ms(GOAL_EVENT_DELAY);
    }

    printf("  ─────────────────────────────────────\n");
    printf("  FT: %s %d - %s %d\n\n", team1_name, score1, team2_name, score2);
}
