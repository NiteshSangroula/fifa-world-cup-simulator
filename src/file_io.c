/*
 * 1. parse strings to enum
 * 2. load data from static files to array of structs
 * 3. load data from dynamic files to array of structs
 * 4. save data from array of structs to dynamic files
 * 5. saveMatch() -> appends instead of writing the whole file
 * 6. wipeDynamicFiles() -> clear all dynamic files
 */
#include "file_io.h"

#include "enums.h"
#include "validation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── file path constants ─────────────────────────────────── */
#define PATH_COUNTRIES "data/static/countries.csv"
#define PATH_PLAYERS "data/static/players.csv"
#define PATH_GOALKEEPERS "data/static/goalkeepers.csv"
#define PATH_GROUPS "data/static/wc_2026_groups.csv"
#define PATH_STATE "data/dynamic/tournament_state.csv"
#define PATH_TEAMS "data/dynamic/tournament_teams.csv"
#define PATH_PSTATS "data/dynamic/player_stats.csv"
#define PATH_GKSTATS "data/dynamic/gk_stats.csv"
#define PATH_MATCHES "data/dynamic/matches.csv"

// ****************************************************************
// ******************** 2. Load Static files **********************
// ****************************************************************

/*
 * 1. open
 * 2. skip header
 * 3. loop
 * 4. parse
 * 5. fill struct
 * 6. return count
 */

// 1. Load Countries
int loadCountries(Country *arr, int max)
{
    FILE *file = fopen(PATH_COUNTRIES, "r");
    if (file == NULL)
    {
        return -1;
    }

    char line[LINE_BUF];

    // skip header
    fgets(line, sizeof(line), file);

    int count = 0;
    // load all countries to the array
    while (count < max && fgets(line, sizeof(line), file))
    {
        arr[count].id = atoi(strtok(line, ","));
        strcpy(arr[count].name, strtok(NULL, ","));
        arr[count].confederation = strToConfed(strtok(NULL, ","));
        arr[count].fifa_ranking  = atoi(strtok(NULL, ","));
        count++;
    }

    fclose(file);

    return count;
}

// 2. Load Players
int loadPlayers(Player *arr, int max)
{
    FILE *file = fopen(PATH_PLAYERS, "r");
    if (file == NULL)
    {
        return -1;
    }

    char line[LINE_BUF];

    fgets(line, sizeof(line), file); // skip header

    int count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        arr[count].id = atoi(strtok(line, ","));
        strcpy(arr[count].name, strtok(NULL, ","));
        arr[count].country_id = atoi(strtok(NULL, ","));
        arr[count].age        = atoi(strtok(NULL, ","));
        arr[count].position   = mapPosition(strtok(NULL, ","));
        arr[count].speed      = atoi(strtok(NULL, ","));
        arr[count].passing    = atoi(strtok(NULL, ","));
        arr[count].shooting   = atoi(strtok(NULL, ","));
        arr[count].defending  = atoi(strtok(NULL, ","));
        arr[count].physical   = atoi(strtok(NULL, ","));
        arr[count].dribbling  = atoi(strtok(NULL, ","));

        count++;
    }

    fclose(file);

    return count;
}

// 3. load GK's
int loadGoalkeepers(Goalkeeper *arr, int max)
{
    FILE *file = fopen(PATH_GOALKEEPERS, "r");
    if (file == NULL)
    {
        return -1;
    }

    char line[LINE_BUF];

    fgets(line, sizeof(line), file); // skip header

    int count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        arr[count].id = atoi(strtok(line, ","));
        strcpy(arr[count].name, strtok(NULL, ","));
        arr[count].country_id  = atoi(strtok(NULL, ","));
        arr[count].age         = atoi(strtok(NULL, ","));
        arr[count].position    = mapPosition(strtok(NULL, ","));
        arr[count].diving      = atoi(strtok(NULL, ","));
        arr[count].handling    = atoi(strtok(NULL, ","));
        arr[count].kicking     = atoi(strtok(NULL, ","));
        arr[count].reflexes    = atoi(strtok(NULL, ","));
        arr[count].speed       = atoi(strtok(NULL, ","));
        arr[count].positioning = atoi(strtok(NULL, ","));

        count++;
    }

    fclose(file);

    return count;
}

// 4. LoadGroups fill in tournament teams
int loadGroups(TournamentTeam *arr, int max)
{
    FILE *file = fopen(PATH_GROUPS, "r");
    if (file == NULL)
        return -1;

    char line[LINE_BUF];
    fgets(line, sizeof(line), file);

    int count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        int   country_id = atoi(strtok(line, ","));
        char *group      = strtok(NULL, ",\n");

        arr[count].tournament_id = TOURNAMENT_ID;
        arr[count].country_id    = country_id;
        strncpy(arr[count].group_name, group, GROUP_NAME_LEN - 1);
        arr[count].group_name[GROUP_NAME_LEN - 1] = '\0';

        /* zero all stats */
        arr[count].matches_played  = 0;
        arr[count].wins            = 0;
        arr[count].draws           = 0;
        arr[count].losses          = 0;
        arr[count].goals_for       = 0;
        arr[count].goals_against   = 0;
        arr[count].goal_difference = 0;
        arr[count].points          = 0;
        arr[count].stage_reached   = GROUP_STAGE;

        count++;
    }

    fclose(file);
    return count;
}

// ****************************************************************
// ******************** 3. Load Dynamic files *********************
// ****************************************************************

int loadTournamentTeams(TournamentTeam *arr, int max)
{
    FILE *file = fopen(PATH_TEAMS, "r");
    if (file == NULL)
        return 0; /* no file yet = new tournament, not an error */

    char line[LINE_BUF];
    fgets(line, sizeof(line), file); /* skip header */

    int count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {

        /* skip blank lines */
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
            continue;

        arr[count].tournament_id = atoi(strtok(line, ","));
        arr[count].country_id    = atoi(strtok(NULL, ","));
        strncpy(arr[count].group_name, strtok(NULL, ","), GROUP_NAME_LEN - 1);
        arr[count].matches_played  = atoi(strtok(NULL, ","));
        arr[count].wins            = atoi(strtok(NULL, ","));
        arr[count].draws           = atoi(strtok(NULL, ","));
        arr[count].losses          = atoi(strtok(NULL, ","));
        arr[count].goals_for       = atoi(strtok(NULL, ","));
        arr[count].goals_against   = atoi(strtok(NULL, ","));
        arr[count].goal_difference = atoi(strtok(NULL, ","));
        arr[count].points          = atoi(strtok(NULL, ","));
        arr[count].stage_reached   = strToStage(strtok(NULL, ",\n"));
        count++;
    }

    fclose(file);
    return count;
}

int loadPlayerStats(PlayerStats *arr, int max)
{
    FILE *file = fopen(PATH_PSTATS, "r");
    if (file == NULL)
        return 0;

    char line[LINE_BUF];
    fgets(line, sizeof(line), file);

    int count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        /* skip blank lines */
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
            continue;

        arr[count].tournament_id  = atoi(strtok(line, ","));
        arr[count].player_id      = atoi(strtok(NULL, ","));
        arr[count].goals          = atoi(strtok(NULL, ","));
        arr[count].assists        = atoi(strtok(NULL, ","));
        arr[count].matches_played = atoi(strtok(NULL, ","));
        arr[count].is_injured     = atoi(strtok(NULL, ",\n"));
        count++;
    }

    fclose(file);
    return count;
}

int loadGKStats(GKStats *arr, int max)
{
    FILE *file = fopen(PATH_GKSTATS, "r");
    if (file == NULL)
        return 0;

    char line[LINE_BUF];
    fgets(line, sizeof(line), file);

    int count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        /* skip blank lines */
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
            continue;

        arr[count].tournament_id  = atoi(strtok(line, ","));
        arr[count].gk_id          = atoi(strtok(NULL, ","));
        arr[count].clean_sheets   = atoi(strtok(NULL, ","));
        arr[count].goals_conceded = atoi(strtok(NULL, ","));
        arr[count].matches_played = atoi(strtok(NULL, ","));
        arr[count].is_injured     = atoi(strtok(NULL, ",\n"));
        count++;
    }

    fclose(file);
    return count;
}

int loadMatches(Match *arr, int max)
{
    FILE *file = fopen(PATH_MATCHES, "r");
    if (file == NULL)
        return 0;

    char line[LINE_BUF];
    fgets(line, sizeof(line), file);

    int count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        /* skip blank lines */
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
            continue;

        arr[count].id            = atoi(strtok(line, ","));
        arr[count].tournament_id = atoi(strtok(NULL, ","));
        arr[count].team1_id      = atoi(strtok(NULL, ","));
        arr[count].team2_id      = atoi(strtok(NULL, ","));
        arr[count].team1_score   = atoi(strtok(NULL, ","));
        arr[count].team2_score   = atoi(strtok(NULL, ","));
        arr[count].stage         = strToStage(strtok(NULL, ","));
        strncpy(arr[count].group_name, strtok(NULL, ","), GROUP_NAME_LEN - 1);
        arr[count].is_simulated = atoi(strtok(NULL, ","));
        // strncpy(arr[count].match_date, strtok(NULL, ",\n"), DATE_LEN - 1);
        count++;
    }

    fclose(file);
    return count;
}

int loadTournamentState(TournamentState *state)
{
    FILE *file = fopen(PATH_STATE, "r");
    if (file == NULL)
        return 0; /* file doesn't exist yet — no tournament */

    char line[LINE_BUF];
    fgets(line, sizeof(line), file); /* skip header */

    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 0; /* file exists but empty — no tournament */
    }

    state->tournament_id = atoi(strtok(line, ","));
    strncpy(state->tournament_name, strtok(NULL, ","), TOURNEY_NAME_LEN - 1);
    state->current_stage       = strToStage(strtok(NULL, ","));
    state->groups_completed    = atoi(strtok(NULL, ","));
    state->knockouts_completed = atoi(strtok(NULL, ","));
    state->tournament_winner   = atoi(strtok(NULL, ",\n"));

    fclose(file);
    return 1;
}

// ****************************************************************
// ******************** 4. SAVE FUNCTIONS *************************
// ****************************************************************

int savePlayerStats(PlayerStats *arr, int count)
{
    FILE *file = fopen(PATH_PSTATS, "w");
    if (file == NULL)
        return -1;

    fprintf(file, "tournament_id,player_id,goals,assists,matches_played,is_injured\n");

    for (int i = 0; i < count; i++)
    {
        fprintf(file,
                "%d,%d,%d,%d,%d,%d\n",
                arr[i].tournament_id,
                arr[i].player_id,
                arr[i].goals,
                arr[i].assists,
                arr[i].matches_played,
                arr[i].is_injured);
    }

    fclose(file);
    return 1;
}

int saveGKStats(GKStats *arr, int count)
{
    FILE *file = fopen(PATH_GKSTATS, "w");
    if (file == NULL)
        return -1;

    fprintf(file, "tournament_id,gk_id,clean_sheets,goals_conceded,matches_played,is_injured\n");

    for (int i = 0; i < count; i++)
    {
        fprintf(file,
                "%d,%d,%d,%d,%d,%d\n",
                arr[i].tournament_id,
                arr[i].gk_id,
                arr[i].clean_sheets,
                arr[i].goals_conceded,
                arr[i].matches_played,
                arr[i].is_injured);
    }

    fclose(file);
    return 1;
}

int saveTournamentTeams(TournamentTeam *arr, int count)
{
    FILE *file = fopen(PATH_TEAMS, "w");
    if (file == NULL)
        return -1;

    fprintf(file,
            "tournament_id,country_id,group_name,matches_played,"
            "wins,draws,losses,goals_for,goals_against,"
            "goal_difference,points,stage_reached\n");

    for (int i = 0; i < count; i++)
    {
        fprintf(file,
                "%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                arr[i].tournament_id,
                arr[i].country_id,
                arr[i].group_name,
                arr[i].matches_played,
                arr[i].wins,
                arr[i].draws,
                arr[i].losses,
                arr[i].goals_for,
                arr[i].goals_against,
                arr[i].goal_difference,
                arr[i].points,
                stageToStr(arr[i].stage_reached));
    }

    fclose(file);
    return 1;
}

int saveTournamentState(TournamentState *state)
{
    FILE *file = fopen(PATH_STATE, "w");
    if (file == NULL)
        return -1;

    fprintf(file,
            "tournament_id,tournament_name,current_stage,"
            "groups_completed,knockouts_completed,tournament_winner\n");

    fprintf(file,
            "%d,%s,%s,%d,%d,%d\n",
            state->tournament_id,
            state->tournament_name,
            stageToStr(state->current_stage),
            state->groups_completed,
            state->knockouts_completed,
            state->tournament_winner);

    fclose(file);
    return 1;
}

/* ----------------------------------------------------------------
 *
 */
int saveMatch(Match *m)
{
    FILE *file = fopen(PATH_MATCHES, "a"); /* append — not "w" */
    if (file == NULL)
        return -1;

    fprintf(file,
            "%d,%d,%d,%d,%d,%d,%s,%s,%d\n",
            m->id,
            m->tournament_id,
            m->team1_id,
            m->team2_id,
            m->team1_score,
            m->team2_score,
            stageToStr(m->stage),
            m->group_name,
            m->is_simulated);

    fclose(file);
    return 1;
}

//*************************************************************************
//*********** WIPE ALL DYNAMIC FILES **************************************
//*************************************************************************
int wipeDynamicFiles(void)
{
    const char *paths[]   = {PATH_STATE, PATH_TEAMS, PATH_PSTATS, PATH_GKSTATS, PATH_MATCHES};
    const char *headers[] = {
        "tournament_id,tournament_name,current_stage,groups_completed,knockouts_completed,"
        "tournament_winner\n",
        "tournament_id,country_id,group_name,matches_played,wins,draws,losses,goals_for,goals_"
        "against,goal_difference,points,stage_reached\n",
        "tournament_id,player_id,goals,assists,matches_played,is_injured\n",
        "tournament_id,gk_id,clean_sheets,goals_conceded,matches_played,is_injured\n",
        "id,tournament_id,team1_id,team2_id,team1_score,team2_score,stage,group_name,is_"
        "simulated\n"};

    int n = sizeof(paths) / sizeof(paths[0]);

    for (int i = 0; i < n; i++)
    {
        FILE *f = fopen(paths[i], "w");
        if (f == NULL)
            return -1;
        fprintf(f, "%s", headers[i]);
        fclose(f);
    }

    return 1;
}
