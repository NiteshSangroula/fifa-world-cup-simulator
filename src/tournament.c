#include "tournament.h"

#include "config.h"
#include "display.h"
#include "file_io.h"
#include "simulation.h"
#include "structs.h"
#include "validation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── internal comparator ─────────────────────────────────── */

static int cmpStandings(const void *a, const void *b)
{
    const TournamentTeam *ta = (const TournamentTeam *)a;
    const TournamentTeam *tb = (const TournamentTeam *)b;

    if (tb->points != ta->points)
        return tb->points - ta->points;
    if (tb->goal_difference != ta->goal_difference)
        return tb->goal_difference - ta->goal_difference;
    return tb->goals_for - ta->goals_for;
}

static int cmpStandingsPtr(const void *a, const void *b)
{
    const TournamentTeam *ta = *(const TournamentTeam **)a;
    const TournamentTeam *tb = *(const TournamentTeam **)b;

    if (tb->points != ta->points)
        return tb->points - ta->points;
    if (tb->goal_difference != ta->goal_difference)
        return tb->goal_difference - ta->goal_difference;
    return tb->goals_for - ta->goals_for;
}

/* ── internal helpers ────────────────────────────────────── */

/* finds TournamentTeam index by country_id, returns -1 if not found */
static int findTeamIndex(int country_id, TournamentTeam *teams, int t_count)
{
    for (int i = 0; i < t_count; i++)
        if (teams[i].country_id == country_id)
            return i;
    return -1;
}

/* finds next unplayed match for a given stage, returns -1 if none */
/*
static int findNextUnplayed(Match *matches, int m_count, Stage stage)
{
    for (int i = 0; i < m_count; i++)
        if (matches[i].stage == stage && matches[i].is_simulated == 0)
            return i;
    return -1;
}
*/

/* counts simulated matches for a given stage */
static int countSimulated(Match *matches, int m_count, Stage stage)
{
    int count = 0;
    for (int i = 0; i < m_count; i++)
        if (matches[i].stage == stage && matches[i].is_simulated == 1)
            count++;
    return count;
}

/* returns the next stage after the current one */
static Stage nextStage(Stage current)
{
    switch (current)
    {
    case ROUND_OF_32:
        return ROUND_OF_16;
    case ROUND_OF_16:
        return QUARTER_FINAL;
    case QUARTER_FINAL:
        return SEMI_FINAL;
    case SEMI_FINAL:
        return FINAL;
    default:
        return FINAL;
    }
}

/* returns expected match count for a knockout stage */
int expectedMatches(Stage stage)
{
    switch (stage)
    {
    case ROUND_OF_32:
        return 16;
    case ROUND_OF_16:
        return 8;
    case QUARTER_FINAL:
        return 4;
    case SEMI_FINAL:
        return 2;
    case FINAL:
        return 1;
    default:
        return 0;
    }
}

/* --- 1. Initialize Groups ----------------------------------- */

void initializeGroups(TournamentTeam *teams, int t_id)
{
    int loaded = loadGroups(teams, MAX_TEAMS);
    if (loaded <= 0)
    {
        printf("Error: Couldn't load group data!");
        return;
    }
    for (int i = 0; i < loaded; i++)
        teams[i].tournament_id = t_id;
}

/* --- 2. Generate Match Schedule ----------------------------------- */
void generateMatchSchedule(Fixture        *schedule,
                           TournamentTeam *teams,
                           int             t_count,
                           int            *fixture_count)
{
    *fixture_count       = 0;
    char group_letters[] = "ABCDEFGHIJKL";

    for (int g = 0; g < NUM_GROUPS; g++)
    {
        int group_ids[TEAMS_PER_GROUP];
        int found = 0;

        for (int i = 0; i < t_count && found < TEAMS_PER_GROUP; i++)
            if (teams[i].group_name[0] == group_letters[g])
                group_ids[found++] = teams[i].country_id;

        if (found < TEAMS_PER_GROUP)
            continue;

        /* round-robin pairings: AxB+CxD, AxC+BxD, AxD+BxC */
        int pairs[3][2][2] = {{{0, 1}, {2, 3}}, {{0, 2}, {1, 3}}, {{0, 3}, {1, 2}}};

        for (int round = 0; round < 3; round++)
        {
            for (int match = 0; match < 2; match++)
            {
                int idx                = *fixture_count;
                schedule[idx].team1_id = group_ids[pairs[round][match][0]];
                schedule[idx].team2_id = group_ids[pairs[round][match][1]];
                schedule[idx].played   = 0;
                (*fixture_count)++;
            }
        }
    }
}

/* ── 3. markPlayedFixtures ───────────────────────────────── */

void markPlayedFixtures(Fixture *schedule, int f_count, Match *matches, int m_count)
{
    for (int i = 0; i < f_count; i++)
        for (int j = 0; j < m_count; j++)
            if (matches[j].stage == GROUP_STAGE && matches[j].is_simulated &&
                matches[j].team1_id == schedule[i].team1_id &&
                matches[j].team2_id == schedule[i].team2_id)
            {
                schedule[i].played = 1;
                break;
            }
}

/* ── 4. simulateGroupStage ───────────────────────────────── */

void simulateGroupStage(Fixture         *schedule,
                        int              f_count,
                        TournamentTeam  *teams,
                        int              t_count,
                        Match           *matches,
                        int             *m_count,
                        Player          *players,
                        int              p_count,
                        Goalkeeper      *gks,
                        int              g_count,
                        PlayerStats     *pstats,
                        int              ps_count,
                        GKStats         *gkstats,
                        int              gs_count,
                        Country         *countries,
                        int              c_count,
                        TournamentState *state,
                        int              batch_size)
{
    int simulated = 0;

    for (int i = 0; i < f_count && simulated < batch_size; i++)
    {
        if (schedule[i].played)
            continue;

        int        team1_id = schedule[i].team1_id;
        int        team2_id = schedule[i].team2_id;
        int        s1, s2;
        StartingXI xi1, xi2;

        simulateMatch(team1_id,
                      team2_id,
                      0,
                      &s1,
                      &s2,
                      &xi1,
                      &xi2,
                      players,
                      p_count,
                      gks,
                      g_count,
                      pstats,
                      ps_count,
                      gkstats,
                      gs_count);

        /* goal events and playback */
        GoalEvent events[MAX_GOAL_EVENTS];
        int       event_count =
            generateGoalEvents(team1_id, team2_id, s1, s2, &xi1, &xi2, players, p_count, events);

        playbackEvents(
            events, event_count, players, p_count, countries, c_count, team1_id, team2_id, s1, s2);

        /* update team stats */
        int t1 = findTeamIndex(team1_id, teams, t_count);
        int t2 = findTeamIndex(team2_id, teams, t_count);

        for (int e = 0; e < event_count; e++)
        {
            for (int j = 0; j < ps_count; j++)
                if (pstats[j].player_id == events[e].player_id)
                {
                    pstats[j].goals++;
                    pstats[j].matches_played++;
                    break;
                }
        }

        /* update GK stats */
        /* team that conceded = losing team's GK */
        int conceded_by    = s1 > s2 ? team2_id : team1_id;
        int goals_conceded = s1 > s2 ? s1 : s2;
        int clean_sheet    = (s1 == 0 || s2 == 0);

        for (int j = 0; j < gs_count; j++)
        {
            for (int k = 0; k < g_count; k++)
                if (gks[k].id == gkstats[j].gk_id && gks[k].country_id == conceded_by)
                {
                    gkstats[j].goals_conceded += goals_conceded;
                    gkstats[j].matches_played++;
                    if (clean_sheet)
                        gkstats[j].clean_sheets++;
                    break;
                }
        }

        savePlayerStats(pstats, ps_count);
        saveGKStats(gkstats, gs_count);
        // new

        if (t1 != -1)
        {
            teams[t1].matches_played++;
            teams[t1].goals_for += s1;
            teams[t1].goals_against += s2;
            teams[t1].goal_difference = teams[t1].goals_for - teams[t1].goals_against;
            if (s1 > s2)
            {
                teams[t1].wins++;
                teams[t1].points += 3;
            }
            else if (s1 == s2)
            {
                teams[t1].draws++;
                teams[t1].points += 1;
            }
            else
                teams[t1].losses++;
        }
        if (t2 != -1)
        {
            teams[t2].matches_played++;
            teams[t2].goals_for += s2;
            teams[t2].goals_against += s1;
            teams[t2].goal_difference = teams[t2].goals_for - teams[t2].goals_against;
            if (s2 > s1)
            {
                teams[t2].wins++;
                teams[t2].points += 3;
            }
            else if (s1 == s2)
            {
                teams[t2].draws++;
                teams[t2].points += 1;
            }
            else
                teams[t2].losses++;
        }

        /* find group name for this fixture */
        char group_name[GROUP_NAME_LEN] = "";
        if (t1 != -1)
            strncpy(group_name, teams[t1].group_name, GROUP_NAME_LEN - 1);

        /* build and save match record */
        Match m;
        m.id            = *m_count + 1;
        m.tournament_id = TOURNAMENT_ID;
        m.team1_id      = team1_id;
        m.team2_id      = team2_id;
        m.team1_score   = s1;
        m.team2_score   = s2;
        m.stage         = GROUP_STAGE;
        m.is_simulated  = 1;
        strncpy(m.group_name, group_name, GROUP_NAME_LEN - 1);
        // strncpy(m.match_date, "", DATE_LEN - 1); /* date not tracked for now */

        matches[*m_count] = m;
        (*m_count)++;

        saveMatch(&m);
        saveTournamentTeams(teams, t_count);

        schedule[i].played = 1;
        simulated++;
    }

    /* check if all group fixtures are done */
    int all_done = 1;
    for (int i = 0; i < f_count; i++)
        if (!schedule[i].played)
        {
            all_done = 0;
            break;
        }

    if (all_done && !state->groups_completed)
    {
        /* sort all groups */
        calculateAllStandings(teams, t_count);

        state->groups_completed = 1;
        state->current_stage    = ROUND_OF_32;
        saveTournamentState(state);
        printf("\n  Group stage complete!\n");
    }
}

/* ── 5. calculateStandings ───────────────────────────────── */

void calculateStandings(TournamentTeam *group_teams, int count)
{
    qsort(group_teams, (size_t)count, sizeof(TournamentTeam), cmpStandings);
}

void calculateAllStandings(TournamentTeam *teams, int t_count)
{
    char group_letters[] = "ABCDEFGHIJKL";

    for (int g = 0; g < NUM_GROUPS; g++)
    {
        /* collect pointers to this group's teams */
        TournamentTeam *group[TEAMS_PER_GROUP];
        int             found = 0;

        for (int i = 0; i < t_count && found < TEAMS_PER_GROUP; i++)
            if (teams[i].group_name[0] == group_letters[g])
                group[found++] = &teams[i];

        if (found < TEAMS_PER_GROUP)
            continue;

        /* sort by swapping the actual structs */
        /* simple insertion sort on pointers, then copy back */
        for (int a = 0; a < found - 1; a++)
            for (int b = a + 1; b < found; b++)
                if (cmpStandingsPtr(&group[a], &group[b]) > 0)
                {
                    TournamentTeam tmp = *group[a];
                    *group[a]          = *group[b];
                    *group[b]          = tmp;
                }
    }
}

/* ── 6. selectBestThird ──────────────────────────────────── */

int selectBestThird(TournamentTeam *teams, int t_count, int *best_third_ids)
{
    /* after calculateAllStandings, third place in each group
     * is at index 2 of each group's sorted slice */
    TournamentTeam *thirds[NUM_GROUPS];
    int             found           = 0;
    char            group_letters[] = "ABCDEFGHIJKL";

    for (int g = 0; g < NUM_GROUPS && found < NUM_GROUPS; g++)
    {
        int group_count = 0;
        for (int i = 0; i < t_count; i++)
            if (teams[i].group_name[0] == group_letters[g])
            {
                if (group_count == 2) /* index 2 = third place */
                {
                    thirds[found++] = &teams[i];
                    break;
                }
                group_count++;
            }
    }

    /* sort the 12 third-place teams */
    for (int a = 0; a < found - 1; a++)
        for (int b = a + 1; b < found; b++)
            if (cmpStandingsPtr(&thirds[a], &thirds[b]) > 0)
            {
                TournamentTeam *tmp = thirds[a];
                thirds[a]           = thirds[b];
                thirds[b]           = tmp;
            }

    /* take top BEST_THIRD_ADVANCE */
    int taken = found < BEST_THIRD_ADVANCE ? found : BEST_THIRD_ADVANCE;
    for (int i = 0; i < taken; i++)
        best_third_ids[i] = thirds[i]->country_id;

    return taken;
}

/* ── 7. generateKnockoutBracket ──────────────────────────── */

void generateKnockoutBracket(
    TournamentTeam *teams, int t_count, Match *matches, int *m_count, int tournament_id)
{
    /* collect winners (rank 0) and runners-up (rank 1) per group */
    int  winners[NUM_GROUPS];
    int  runners[NUM_GROUPS];
    char group_letters[] = "ABCDEFGHIJKL";

    for (int g = 0; g < NUM_GROUPS; g++)
    {
        int rank = 0;
        for (int i = 0; i < t_count; i++)
            if (teams[i].group_name[0] == group_letters[g])
            {
                if (rank == 0)
                    winners[g] = teams[i].country_id;
                if (rank == 1)
                    runners[g] = teams[i].country_id;
                rank++;
                if (rank > 1)
                    break;
            }
    }

    int best_thirds[BEST_THIRD_ADVANCE];
    selectBestThird(teams, t_count, best_thirds);

    /* seed 16 R32 matches
     * pairing: winner[g] vs runner[g+6] or best_third
     * simplified seeding — pair by group offset */
    int team1s[16], team2s[16];

    /* 12 winners vs 12 runners-up — pair opposite halves */
    for (int g = 0; g < 6; g++)
    {
        team1s[g]     = winners[g];
        team2s[g]     = runners[g + 6];
        team1s[g + 6] = winners[g + 6];
        team2s[g + 6] = runners[g];
    }
    /* last 4 matches: winners vs best thirds */
    for (int i = 0; i < 4; i++)
    {
        team1s[12 + i] = winners[i]; /* reuse first 4 winners */
        team2s[12 + i] = best_thirds[i];
    }

    for (int i = 0; i < 16; i++)
    {
        Match m;
        m.id            = *m_count + 1;
        m.tournament_id = tournament_id;
        m.team1_id      = team1s[i];
        m.team2_id      = team2s[i];
        m.team1_score   = 0;
        m.team2_score   = 0;
        m.stage         = ROUND_OF_32;
        m.group_name[0] = '\0';
        m.is_simulated  = 0;
        // strncpy(m.match_date, "", DATE_LEN - 1);

        matches[*m_count] = m;
        (*m_count)++;
    }
}

/* ── 8. advanceKnockout ──────────────────────────────────── */

void advanceKnockout(Match           *matches,
                     int             *m_count,
                     TournamentTeam  *teams,
                     int              t_count,
                     Player          *players,
                     int              p_count,
                     Goalkeeper      *gks,
                     int              g_count,
                     PlayerStats     *pstats,
                     int              ps_count,
                     GKStats         *gkstats,
                     int              gs_count,
                     Country         *countries,
                     int              c_count,
                     TournamentState *state,
                     int              batch_size)
{
    Stage current   = state->current_stage;
    int   simulated = 0;

    for (int i = 0; i < *m_count && simulated < batch_size; i++)
    {
        if (matches[i].stage != current)
            continue;
        if (matches[i].is_simulated)
            continue;

        int        team1_id = matches[i].team1_id;
        int        team2_id = matches[i].team2_id;
        int        s1, s2;
        StartingXI xi1, xi2;

        /* is_knockout = 1 — no draws */
        simulateMatch(team1_id,
                      team2_id,
                      1,
                      &s1,
                      &s2,
                      &xi1,
                      &xi2,
                      players,
                      p_count,
                      gks,
                      g_count,
                      pstats,
                      ps_count,
                      gkstats,
                      gs_count);

        GoalEvent events[MAX_GOAL_EVENTS];
        int       event_count =
            generateGoalEvents(team1_id, team2_id, s1, s2, &xi1, &xi2, players, p_count, events);
        playbackEvents(
            events, event_count, players, p_count, countries, c_count, team1_id, team2_id, s1, s2);

        int winner_id = s1 > s2 ? team1_id : team2_id;
        int loser_id  = s1 > s2 ? team2_id : team1_id;

        /* update stage_reached */
        int wi = findTeamIndex(winner_id, teams, t_count);
        int li = findTeamIndex(loser_id, teams, t_count);

        /* update player stats for goal scorers */
        for (int e = 0; e < event_count; e++)
        {
            for (int j = 0; j < ps_count; j++)
                if (pstats[j].player_id == events[e].player_id)
                {
                    pstats[j].goals++;
                    pstats[j].matches_played++;
                    break;
                }
        }

        /* update GK stats */
        /* team that conceded = losing team's GK */
        int conceded_by    = s1 > s2 ? team2_id : team1_id;
        int goals_conceded = s1 > s2 ? s1 : s2;
        int clean_sheet    = (s1 == 0 || s2 == 0);

        for (int j = 0; j < gs_count; j++)
        {
            for (int k = 0; k < g_count; k++)
                if (gks[k].id == gkstats[j].gk_id && gks[k].country_id == conceded_by)
                {
                    gkstats[j].goals_conceded += goals_conceded;
                    gkstats[j].matches_played++;
                    if (clean_sheet)
                        gkstats[j].clean_sheets++;
                    break;
                }
        }

        savePlayerStats(pstats, ps_count);
        saveGKStats(gkstats, gs_count);

        // new

        if (li != -1)
            teams[li].stage_reached = current;
        if (wi != -1)
            teams[wi].stage_reached = nextStage(current);

        /* mark match done */
        matches[i].team1_score  = s1;
        matches[i].team2_score  = s2;
        matches[i].is_simulated = 1;

        saveMatch(&matches[i]);
        saveTournamentTeams(teams, t_count);

        simulated++;
    }

    /* check if current round is complete */
    int done     = countSimulated(matches, *m_count, current);
    int expected = expectedMatches(current);

    if (done == expected)
    {
        if (current == FINAL)
        {
            /* find winner of the final */
            for (int i = 0; i < *m_count; i++)
                if (matches[i].stage == FINAL && matches[i].is_simulated)
                {
                    int winner                 = matches[i].team1_score > matches[i].team2_score
                                                     ? matches[i].team1_id
                                                     : matches[i].team2_id;
                    state->tournament_winner   = winner;
                    state->knockouts_completed = 1;

                    int wi = findTeamIndex(winner, teams, t_count);
                    if (wi != -1)
                        teams[wi].stage_reached = FINAL;

                    saveTournamentState(state);
                    printf("\n  Tournament complete!\n");
                    break;
                }
        }
        else
        {
            /* generate next round matches */
            Stage next                 = nextStage(current);
            int   match_count_for_next = expected / 2;

            /* collect winners of current round in order */
            int next_team1[8], next_team2[8];
            /*int pair = 0;*/
            int w[16]; /* winners in match order */
            int wc = 0;

            for (int i = 0; i < *m_count; i++)
                if (matches[i].stage == current && matches[i].is_simulated)
                    w[wc++] = matches[i].team1_score > matches[i].team2_score ? matches[i].team1_id
                                                                              : matches[i].team2_id;

            /* pair winners sequentially: w[0] vs w[1], w[2] vs w[3]... */
            for (int i = 0; i < match_count_for_next; i++)
            {
                next_team1[i] = w[i * 2];
                next_team2[i] = w[i * 2 + 1];
            }

            for (int i = 0; i < match_count_for_next; i++)
            {
                Match m;
                m.id            = *m_count + 1;
                m.tournament_id = TOURNAMENT_ID;
                m.team1_id      = next_team1[i];
                m.team2_id      = next_team2[i];
                m.team1_score   = 0;
                m.team2_score   = 0;
                m.stage         = next;
                m.group_name[0] = '\0';
                m.is_simulated  = 0;
                // strncpy(m.match_date, "", DATE_LEN - 1);

                matches[*m_count] = m;
                (*m_count)++;
            }

            state->current_stage = next;
            saveTournamentState(state);
            printf("\n  %s complete! %s begins.\n", stageToStr(current), stageToStr(next));
        }
    }
}
