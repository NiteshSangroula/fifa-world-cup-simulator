#include "config.h"
#include "file_io.h"
#include "structs.h"

#include <stdio.h>
#include <string.h>

/* ── helpers ─────────────────────────────────────────────── */

static int tests_run    = 0;
static int tests_passed = 0;

#define CHECK(label, condition)                                                                    \
    do                                                                                             \
    {                                                                                              \
        tests_run++;                                                                               \
        if (condition)                                                                             \
        {                                                                                          \
            tests_passed++;                                                                        \
            printf("  PASS  %s\n", label);                                                         \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            printf("  FAIL  %s\n", label);                                                         \
        }                                                                                          \
    } while (0)

static void print_summary(void)
{
    printf("\n%d / %d tests passed\n", tests_passed, tests_run);
}

/* ── static loaders ──────────────────────────────────────── */

void test_loadCountries(void)
{
    printf("\n--- loadCountries ---\n");
    Country countries[MAX_TEAMS];
    int     count = loadCountries(countries, MAX_TEAMS);

    CHECK("count is 48", count == 48);
    CHECK("first id is 1", countries[0].id == 1);
    CHECK("first name not empty", strlen(countries[0].name) > 0);
    CHECK("last id is 48", countries[47].id == 48);
    CHECK("last name not empty", strlen(countries[47].name) > 0);
    CHECK("no name has trailing newline", strchr(countries[0].name, '\n') == NULL);
    CHECK("confederation in valid range",
          countries[0].confederation >= UEFA && countries[0].confederation <= OFC);
    CHECK("fifa ranking > 0", countries[0].fifa_ranking > 0);

    /* spot check a known entry — adjust id/name to match your CSV */
    CHECK("known country name matches", strcmp(countries[31].name, "Uruguay") == 0);
}

void test_loadPlayers(void)
{
    printf("\n--- loadPlayers ---\n");
    Player players[MAX_PLAYERS];
    int    count = loadPlayers(players, MAX_PLAYERS);

    CHECK("count > 0", count > 0);
    CHECK("count <= MAX_PLAYERS", count <= MAX_PLAYERS);
    CHECK("first id is 0102", players[0].id == 102);
    CHECK("first name not empty", strlen(players[0].name) > 0);
    CHECK("no name has trailing newline", strchr(players[0].name, '\n') == NULL);
    CHECK("country_id > 0", players[0].country_id > 0);
    CHECK("age in realistic range", players[0].age >= 15 && players[0].age <= 45);
    CHECK("position in valid range", players[0].position >= GK && players[0].position <= FWD);
    // CHECK("shooting in stat range",
    //       players[0].shooting >= STAT_MIN && players[0].shooting <= STAT_MAX);
    CHECK("passing in stat range",
          players[0].passing >= STAT_MIN && players[0].passing <= STAT_MAX);
    CHECK("defending in stat range",
          players[0].defending >= STAT_MIN && players[0].defending <= STAT_MAX);
    CHECK("speed in stat range", players[0].speed >= STAT_MIN && players[0].speed <= STAT_MAX);

    /* spot check mid-array */
    CHECK("mid-array name not empty", strlen(players[count / 2].name) > 0);
    CHECK("last player name not empty", strlen(players[count - 1].name) > 0);

    /* no outfield player should have GK position */
    int bad_gk = 0;
    for (int i = 0; i < count; i++)
        if (players[i].position == GK)
            bad_gk++;
    CHECK("no outfield player has GK position", bad_gk == 0);
}

void test_loadGoalkeepers(void)
{
    printf("\n--- loadGoalkeepers ---\n");
    Goalkeeper gks[MAX_GKS];
    int        count = loadGoalkeepers(gks, MAX_GKS);

    CHECK("count > 0", count > 0);
    CHECK("count <= MAX_GKS", count <= MAX_GKS);
    CHECK("first id is 0101", gks[0].id == 101);
    CHECK("first name not empty", strlen(gks[0].name) > 0);
    CHECK("no name has trailing newline", strchr(gks[0].name, '\n') == NULL);
    CHECK("position is always GK", gks[0].position == GK);
    CHECK("reflexes in stat range", gks[0].reflexes >= STAT_MIN && gks[0].reflexes <= STAT_MAX);
    CHECK("diving in stat range", gks[0].diving >= STAT_MIN && gks[0].diving <= STAT_MAX);
    CHECK("handling in stat range", gks[0].handling >= STAT_MIN && gks[0].handling <= STAT_MAX);
    CHECK("age in realistic range", gks[0].age >= 15 && gks[0].age <= 45);

    /* all GKs should have GK position */
    int bad_pos = 0;
    for (int i = 0; i < count; i++)
        if (gks[i].position != GK)
            bad_pos++;
    CHECK("all goalkeepers have GK position", bad_pos == 0);

    /* each country should have exactly GKS_PER_TEAM GKs */
    /* spot check country_id 1 */
    int gk_for_c1 = 0;
    for (int i = 0; i < count; i++)
        if (gks[i].country_id == 1)
            gk_for_c1++;
    CHECK("country 1 has correct GK count", gk_for_c1 == GKS_PER_TEAM);
}

void test_loadGroups(void)
{
    printf("\n--- loadGroups ---\n");
    TournamentTeam teams[MAX_TEAMS];
    int            count = loadGroups(teams, MAX_TEAMS);

    CHECK("count is 48", count == 48);
    CHECK("first country_id > 0", teams[0].country_id > 0);
    CHECK("group_name not empty", strlen(teams[0].group_name) > 0);
    CHECK("no group_name has newline", strchr(teams[0].group_name, '\n') == NULL);
    CHECK("tournament_id set correctly", teams[0].tournament_id == TOURNAMENT_ID);
    CHECK("stage_reached is GROUP_STAGE", teams[0].stage_reached == GROUP_STAGE);

    /* all stat fields must be zero */
    CHECK("wins zeroed", teams[0].wins == 0);
    CHECK("points zeroed", teams[0].points == 0);
    CHECK("goals_for zeroed", teams[0].goals_for == 0);

    /* group names must all be A–L */
    int bad_group = 0;
    for (int i = 0; i < count; i++)
    {
        char g = teams[i].group_name[0];
        if (g < 'A' || g > 'L')
            bad_group++;
    }
    CHECK("all group names in range A-L", bad_group == 0);

    /* each group must have exactly TEAMS_PER_GROUP teams */
    for (char g = 'A'; g <= 'L'; g++)
    {
        int n = 0;
        for (int i = 0; i < count; i++)
            if (teams[i].group_name[0] == g)
                n++;
        /* just check group A for brevity */
        if (g == 'A')
            CHECK("group A has 4 teams", n == TEAMS_PER_GROUP);
        if (g == 'L')
            CHECK("group L has 4 teams", n == TEAMS_PER_GROUP);
    }
}

/* ── save + reload round-trip tests ──────────────────────── */

void test_saveAndLoad_playerStats(void)
{
    printf("\n--- savePlayerStats / loadPlayerStats ---\n");

    PlayerStats original[3] = {{TOURNAMENT_ID, 1, 5, 3, 4, 0},
                               {TOURNAMENT_ID, 2, 0, 1, 2, 1},
                               {TOURNAMENT_ID, 3, 2, 2, 3, 0}};

    int save_result = savePlayerStats(original, 3);
    CHECK("save returns 1", save_result == 1);

    PlayerStats loaded[3];
    int         count = loadPlayerStats(loaded, 3);
    CHECK("reload count matches", count == 3);
    CHECK("player_id matches", loaded[0].player_id == 1);
    CHECK("goals match", loaded[0].goals == 5);
    CHECK("assists match", loaded[0].assists == 3);
    CHECK("is_injured matches", loaded[1].is_injured == 1);
    CHECK("tournament_id matches", loaded[2].tournament_id == TOURNAMENT_ID);
}

void test_saveAndLoad_gkStats(void)
{
    printf("\n--- saveGKStats / loadGKStats ---\n");

    GKStats original[2] = {{TOURNAMENT_ID, 1, 2, 3, 4, 0}, {TOURNAMENT_ID, 2, 0, 7, 3, 1}};

    int save_result = saveGKStats(original, 2);
    CHECK("save returns 1", save_result == 1);

    GKStats loaded[2];
    int     count = loadGKStats(loaded, 2);
    CHECK("reload count matches", count == 2);
    CHECK("gk_id matches", loaded[0].gk_id == 1);
    CHECK("clean_sheets match", loaded[0].clean_sheets == 2);
    CHECK("goals_conceded match", loaded[1].goals_conceded == 7);
    CHECK("is_injured matches", loaded[1].is_injured == 1);
}

void test_saveAndLoad_tournamentTeams(void)
{
    printf("\n--- saveTournamentTeams / loadTournamentTeams ---\n");

    TournamentTeam original[2] = {{TOURNAMENT_ID, 1, "A", 3, 2, 1, 0, 7, 3, 4, 7, ROUND_OF_32},
                                  {TOURNAMENT_ID, 2, "A", 3, 0, 1, 2, 2, 6, -4, 1, GROUP_STAGE}};

    int save_result = saveTournamentTeams(original, 2);
    CHECK("save returns 1", save_result == 1);

    TournamentTeam loaded[2];
    int            count = loadTournamentTeams(loaded, 2);
    CHECK("reload count matches", count == 2);
    CHECK("country_id matches", loaded[0].country_id == 1);
    CHECK("wins match", loaded[0].wins == 2);
    CHECK("goal_difference matches", loaded[0].goal_difference == 4);
    CHECK("points match", loaded[0].points == 7);
    CHECK("stage_reached matches", loaded[0].stage_reached == ROUND_OF_32);
    CHECK("negative GD survives round-trip", loaded[1].goal_difference == -4);
    CHECK("group_name matches", strcmp(loaded[1].group_name, "A") == 0);
}

void test_saveAndLoad_tournamentState(void)
{
    printf("\n--- saveTournamentState / loadTournamentState ---\n");

    TournamentState original;
    original.tournament_id       = TOURNAMENT_ID;
    original.groups_completed    = 0;
    original.knockouts_completed = 0;
    original.tournament_winner   = 0;
    original.current_stage       = GROUP_STAGE;
    strncpy(original.tournament_name, "FIFA World Cup 2026", TOURNEY_NAME_LEN - 1);

    int save_result = saveTournamentState(&original);
    CHECK("save returns 1", save_result == 1);

    TournamentState loaded;
    int             load_result = loadTournamentState(&loaded);
    CHECK("load returns 1", load_result == 1);
    CHECK("tournament_id matches", loaded.tournament_id == TOURNAMENT_ID);
    CHECK("tournament_name matches", strcmp(loaded.tournament_name, "FIFA World Cup 2026") == 0);
    CHECK("current_stage matches", loaded.current_stage == GROUP_STAGE);
    CHECK("groups_completed matches", loaded.groups_completed == 0);
    CHECK("tournament_winner matches", loaded.tournament_winner == 0);
}

void test_saveAndLoad_matches(void)
{
    printf("\n--- saveMatch / loadMatches ---\n");

    /* wipe first so we start clean */
    wipeDynamicFiles();

    Match m1 = {1, TOURNAMENT_ID, 1, 2, 3, 1, GROUP_STAGE, "A", 1, "2026-06-15"};
    Match m2 = {2, TOURNAMENT_ID, 3, 4, 0, 2, GROUP_STAGE, "B", 1, "2026-06-15"};

    CHECK("save match 1 returns 1", saveMatch(&m1) == 1);
    CHECK("save match 2 returns 1", saveMatch(&m2) == 1);

    Match loaded[3];
    int   count = loadMatches(loaded, 10);
    CHECK("reload count is 2", count == 2);
    CHECK("match id matches", loaded[0].id == 1);
    CHECK("team1_id matches", loaded[0].team1_id == 1);
    CHECK("team1_score matches", loaded[0].team1_score == 3);
    CHECK("team2_score matches", loaded[0].team2_score == 1);
    CHECK("stage matches", loaded[0].stage == GROUP_STAGE);
    CHECK("group_name matches", strcmp(loaded[0].group_name, "A") == 0);
    CHECK("is_simulated matches", loaded[0].is_simulated == 1);
    CHECK("second match team2_id", loaded[1].team2_id == 4);

    /* verify append — save a third match without wiping */
    Match m3 = {3, TOURNAMENT_ID, 5, 6, 1, 1, GROUP_STAGE, "C", 1, "2026-06-16"};
    saveMatch(&m3);
    int count2 = loadMatches(loaded, 10);
    CHECK("append gives 3 matches total", count2 == 3);
}

void test_wipeDynamicFiles(void)
{
    printf("\n--- wipeDynamicFiles ---\n");

    /* first write something into the files */
    PlayerStats ps = {TOURNAMENT_ID, 1, 9, 9, 9, 0};
    savePlayerStats(&ps, 1);

    int wipe_result = wipeDynamicFiles();
    CHECK("wipe returns 1", wipe_result == 1);

    /* after wipe, loaders should return 0 */
    PlayerStats     dummy[10];
    GKStats         dkstats[10];
    TournamentTeam  dteams[10];
    Match           dmatches[10];
    TournamentState dstate;

    CHECK("pstats empty after wipe", loadPlayerStats(dummy, 10) == 0);
    CHECK("gkstats empty after wipe", loadGKStats(dkstats, 10) == 0);
    CHECK("teams empty after wipe", loadTournamentTeams(dteams, 10) == 0);
    CHECK("matches empty after wipe", loadMatches(dmatches, 10) == 0);
    CHECK("state empty after wipe", loadTournamentState(&dstate) == 0);
}

/* ── entry point ─────────────────────────────────────────── */

//************************************************************
// seperate test data files from real
//************************************************************

void test_file_io(void)
{
    printf("========== file_io tests ==========\n");

    test_loadCountries();
    test_loadPlayers();
    test_loadGoalkeepers();
    test_loadGroups();

    /* wipe before any save/load tests so previous runs don't interfere */
    wipeDynamicFiles();

    test_saveAndLoad_playerStats();
    test_saveAndLoad_gkStats();
    test_saveAndLoad_tournamentTeams();
    test_saveAndLoad_tournamentState();
    test_saveAndLoad_matches();
    test_wipeDynamicFiles();

    print_summary();
}
