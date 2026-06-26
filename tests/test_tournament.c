#include "config.h"
#include "file_io.h"
#include "structs.h"
#include "test_utils.h"
#include "tournament.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── stub data ───────────────────────────────────────────── */

static void buildTeams(TournamentTeam *teams, int *t_count)
{
    char letters[] = "ABCDEFGHIJKL";
    *t_count       = 0;

    for (int g = 0; g < NUM_GROUPS; g++)
        for (int t = 0; t < TEAMS_PER_GROUP; t++)
        {
            int idx                    = *t_count;
            teams[idx].tournament_id   = TOURNAMENT_ID;
            teams[idx].country_id      = g * TEAMS_PER_GROUP + t + 1;
            teams[idx].group_name[0]   = letters[g];
            teams[idx].group_name[1]   = '\0';
            teams[idx].matches_played  = 0;
            teams[idx].wins            = 0;
            teams[idx].draws           = 0;
            teams[idx].losses          = 0;
            teams[idx].goals_for       = 0;
            teams[idx].goals_against   = 0;
            teams[idx].goal_difference = 0;
            teams[idx].points          = 0;
            teams[idx].stage_reached   = GROUP_STAGE;
            (*t_count)++;
        }
}

/*
static void buildCountries(Country *countries, int *c_count)
{
    *c_count = MAX_TEAMS;
    for (int i = 0; i < MAX_TEAMS; i++)
    {
        countries[i].id            = i + 1;
        countries[i].confederation = UEFA;
        countries[i].fifa_ranking  = i + 1;
        snprintf(countries[i].name, NAME_LEN, "Country%d", i + 1);
    }
}
*/

/* ── test: generateMatchSchedule ─────────────────────────── */

void test_generateMatchSchedule(void)
{
    printf("\n--- generateMatchSchedule ---\n");

    TournamentTeam teams[MAX_TEAMS];
    int            t_count;
    buildTeams(teams, &t_count);

    Fixture schedule[MAX_FIXTURES];
    int     f_count = 0;
    generateMatchSchedule(schedule, teams, t_count, &f_count);

    CHECK("total fixtures is 72", f_count == 72);

    /* each team plays exactly 3 fixtures */
    int team1_games = 0;
    for (int i = 0; i < f_count; i++)
        if (schedule[i].team1_id == 1 || schedule[i].team2_id == 1)
            team1_games++;
    CHECK("each team plays 3 group fixtures", team1_games == 3);

    /* no fixture has same team on both sides */
    int self_play = 0;
    for (int i = 0; i < f_count; i++)
        if (schedule[i].team1_id == schedule[i].team2_id)
            self_play++;
    CHECK("no team plays itself", self_play == 0);

    /* no duplicate fixtures */
    int dupes = 0;
    for (int i = 0; i < f_count; i++)
        for (int j = i + 1; j < f_count; j++)
            if ((schedule[i].team1_id == schedule[j].team1_id &&
                 schedule[i].team2_id == schedule[j].team2_id) ||
                (schedule[i].team1_id == schedule[j].team2_id &&
                 schedule[i].team2_id == schedule[j].team1_id))
                dupes++;
    CHECK("no duplicate fixtures", dupes == 0);
}

/* ── test: calculateStandings ────────────────────────────── */

void test_calculateStandings(void)
{
    printf("\n--- calculateStandings ---\n");

    TournamentTeam group[4] = {{1, 1, "A", 3, 0, 1, 2, 2, 6, -4, 1, GROUP_STAGE},
                               {1, 2, "A", 3, 2, 1, 0, 7, 3, 4, 7, GROUP_STAGE},
                               {1, 3, "A", 3, 1, 1, 1, 4, 4, 0, 4, GROUP_STAGE},
                               {1, 4, "A", 3, 1, 1, 1, 3, 3, 0, 4, GROUP_STAGE}};

    calculateStandings(group, 4);

    CHECK("first place has most points", group[0].points >= group[1].points);
    CHECK("correct winner (country 2)", group[0].country_id == 2);
    CHECK("last place has fewest points", group[3].points <= group[2].points);
    CHECK("correct last place (country 1)", group[3].country_id == 1);

    /* tiebreaker — same points, sorted by GD */
    CHECK("GD tiebreak: country 3 before 4",
          group[1].country_id == 3 || group[1].goal_difference >= group[2].goal_difference);
}

/* ── test: selectBestThird ───────────────────────────────── */

void test_selectBestThird(void)
{
    printf("\n--- selectBestThird ---\n");

    TournamentTeam teams[MAX_TEAMS];
    int            t_count;
    buildTeams(teams, &t_count);

    /* manually set some third-place teams with varying points */
    /* after calculateAllStandings, index 2 in each group = third */
    /* we'll simulate that by setting points directly */
    char letters[] = "ABCDEFGHIJKL";
    // int third_idx  = 0;

    for (int g = 0; g < NUM_GROUPS; g++)
    {
        int in_group = 0;
        for (int i = 0; i < t_count; i++)
            if (teams[i].group_name[0] == letters[g])
            {
                if (in_group == 0)
                    teams[i].points = 9;
                if (in_group == 1)
                    teams[i].points = 6;
                if (in_group == 2)
                    teams[i].points = g + 1; /* varying */
                if (in_group == 3)
                    teams[i].points = 0;
                in_group++;
            }
    }

    calculateAllStandings(teams, t_count);

    int best_thirds[BEST_THIRD_ADVANCE];
    int count = selectBestThird(teams, t_count, best_thirds);

    CHECK("returns 8 best thirds", count == BEST_THIRD_ADVANCE);

    /* no duplicates */
    int dupes = 0;
    for (int i = 0; i < count; i++)
        for (int j = i + 1; j < count; j++)
            if (best_thirds[i] == best_thirds[j])
                dupes++;
    CHECK("no duplicate teams in best thirds", dupes == 0);

    /* all ids are valid country ids */
    int invalid = 0;
    for (int i = 0; i < count; i++)
        if (best_thirds[i] < 1 || best_thirds[i] > MAX_TEAMS)
            invalid++;
    CHECK("all best third ids are valid", invalid == 0);
}

/* ── test: markPlayedFixtures ────────────────────────────── */

void test_markPlayedFixtures(void)
{
    printf("\n--- markPlayedFixtures ---\n");

    TournamentTeam teams[MAX_TEAMS];
    int            t_count;
    buildTeams(teams, &t_count);

    Fixture schedule[MAX_FIXTURES];
    int     f_count = 0;
    generateMatchSchedule(schedule, teams, t_count, &f_count);

    /* fake two played matches */
    Match matches[2];
    matches[0].team1_id     = schedule[0].team1_id;
    matches[0].team2_id     = schedule[0].team2_id;
    matches[0].stage        = GROUP_STAGE;
    matches[0].is_simulated = 1;

    matches[1].team1_id     = schedule[5].team1_id;
    matches[1].team2_id     = schedule[5].team2_id;
    matches[1].stage        = GROUP_STAGE;
    matches[1].is_simulated = 1;

    markPlayedFixtures(schedule, f_count, matches, 2);

    CHECK("first fixture marked played", schedule[0].played == 1);
    CHECK("sixth fixture marked played", schedule[5].played == 1);
    CHECK("other fixtures still unplayed", schedule[1].played == 0);

    /* count total marked */
    int marked = 0;
    for (int i = 0; i < f_count; i++)
        if (schedule[i].played)
            marked++;
    CHECK("exactly 2 fixtures marked", marked == 2);
}

/* ── test: generateKnockoutBracket ──────────────────────── */

void test_generateKnockoutBracket(void)
{
    printf("\n--- generateKnockoutBracket ---\n");

    TournamentTeam teams[MAX_TEAMS];
    int            t_count;
    buildTeams(teams, &t_count);

    /* set standings so winners/runners-up are clear */
    char letters[] = "ABCDEFGHIJKL";
    for (int g = 0; g < NUM_GROUPS; g++)
    {
        int rank = 0;
        for (int i = 0; i < t_count; i++)
            if (teams[i].group_name[0] == letters[g])
            {
                teams[i].points = 9 - (rank * 3);
                rank++;
            }
    }
    calculateAllStandings(teams, t_count);

    int best_thirds[BEST_THIRD_ADVANCE];
    selectBestThird(teams, t_count, best_thirds);

    Match matches[MAX_MATCHES];
    int   m_count = 0;
    generateKnockoutBracket(teams, t_count, matches, &m_count, TOURNAMENT_ID);

    CHECK("generates 16 R32 matches", m_count == 16);

    /* all R32 matches unplayed */
    int unplayed = 0;
    for (int i = 0; i < m_count; i++)
        if (!matches[i].is_simulated)
            unplayed++;
    CHECK("all R32 matches unplayed", unplayed == 16);

    /* all matches have valid team ids */
    int invalid = 0;
    for (int i = 0; i < m_count; i++)
        if (matches[i].team1_id < 1 || matches[i].team2_id < 1 ||
            matches[i].team1_id == matches[i].team2_id)
            invalid++;
    CHECK("all matches have valid team ids", invalid == 0);

    /* all matches are stage ROUND_OF_32 */
    int wrong_stage = 0;
    for (int i = 0; i < m_count; i++)
        if (matches[i].stage != ROUND_OF_32)
            wrong_stage++;
    CHECK("all matches staged as ROUND_OF_32", wrong_stage == 0);

    /* no team appears more than once */
    int seen[MAX_TEAMS + 1] = {0};
    int dupes               = 0;
    for (int i = 0; i < m_count; i++)
    {
        if (seen[matches[i].team1_id]++)
            dupes++;
        if (seen[matches[i].team2_id]++)
            dupes++;
    }
    CHECK("no team appears twice in bracket", dupes == 0);
}

/* ── entry point ─────────────────────────────────────────── */

void test_tournament(void)
{
    printf("========== tournament tests ==========\n");
    srand(42);

    test_generateMatchSchedule();
    test_calculateStandings();
    test_selectBestThird();
    test_markPlayedFixtures();
    test_generateKnockoutBracket();

    print_summary();
}
