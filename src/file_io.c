#include "file_io.h"

#include "enums.h"

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

/*
    UEFA,
    CONMEBOL,
    CONCACAF,
    AFC,
    CAF,
    OFC
    */
static Confederation parseConfederations(char *confederation)
{
    if (strcmp(confederation, "UEFA") == 0)
        return UEFA;
    if (strcmp(confederation, "CONMEBOL") == 0)
        return CONMEBOL;
    if (strcmp(confederation, "CONCACAF") == 0)
        return CONCACAF;
    if (strcmp(confederation, "AFC") == 0)
        return AFC;
    if (strcmp(confederation, "CAF") == 0)
        return CAF;
    if (strcmp(confederation, "OFC") == 0)
        return OFC;
    return CONFEDERATION_UNKNOWN;
}

static Position parsePosition(char *position)
{
    if (strcmp(position, "GK") == 0)
        return GK;
    if (strcmp(position, "RB") == 0 || strcmp(position, "LB") == 0 || strcmp(position, "CB") == 0)
        return DEF;
    if (strcmp(position, "CM") == 0 || strcmp(position, "CAM") == 0 ||
        strcmp(position, "LM") == 0 || strcmp(position, "RM") == 0 || strcmp(position, "CDM") == 0)
        return MID;
    if (strcmp(position, "ST") == 0 || strcmp(position, "LW") == 0 || strcmp(position, "RW") == 0)
        return FWD;
    return MID;
}

size_t loadCountries(Country *arr, size_t max)
{
    FILE *file = fopen(PATH_COUNTRIES, "r");
    if (file == NULL)
    {
        return 1;
    }

    char line[256];

    // skip header
    fgets(line, sizeof(line), file);

    size_t count = 0;
    // load all countries to the array
    while (count < max && fgets(line, sizeof(line), file))
    {
        arr[count].id = atoi(strtok(line, ","));
        strcpy(arr[count].name, strtok(NULL, ","));
        arr[count].confederation = parseConfederations(strtok(NULL, ","));
        arr[count].fifa_ranking  = atoi(strtok(NULL, ","));
        count++;
    }

    fclose(file);

    return count;
}

size_t loadPlayers(Player *arr, size_t max)
{
    FILE *file = fopen(PATH_PLAYERS, "r");
    if (file == NULL)
    {
        return 1;
    }

    char line[256];

    fgets(line, sizeof(line), file); // skip header

    size_t count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        arr[count].id = atoi(strtok(line, ","));
        strcpy(arr[count].name, strtok(NULL, ","));
        arr[count].country_id = atoi(strtok(NULL, ","));
        arr[count].age        = atoi(strtok(NULL, ","));
        arr[count].position   = parsePosition(strtok(NULL, ","));
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
size_t loadGoalkeepers(Goalkeeper *arr, size_t max)
{
    FILE *file = fopen(PATH_GOALKEEPERS, "r");
    if (file == NULL)
    {
        return 1;
    }

    char line[256];

    fgets(line, sizeof(line), file); // skip header

    size_t count = 0;

    while (count < max && fgets(line, sizeof(line), file) != NULL)
    {
        arr[count].id = atoi(strtok(line, ","));
        strcpy(arr[count].name, strtok(NULL, ","));
        arr[count].country_id  = atoi(strtok(NULL, ","));
        arr[count].age         = atoi(strtok(NULL, ","));
        arr[count].position    = parsePosition(strtok(NULL, ","));
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
