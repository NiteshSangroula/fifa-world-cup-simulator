#include "display.h"

#include "config.h"
#include "simulation.h"
#include "structs.h"
#include "validation.h"

#include <stdio.h>
#include <string.h>

/* ── internal helpers ────────────────────────────────────── */

static const char *findCountryName(int country_id, Country *countries, int c_count)
{
    for (int i = 0; i < c_count; i++)
        if (countries[i].id == country_id)
            return countries[i].name;
    return "Unknown";
}

static void printDivider(int width)
{
    for (int i = 0; i < width; i++)
        printf("-");
    printf("\n");
}

/* ── menus ───────────────────────────────────────────────── */

void printMainMenu(void)
{
    printf("\n");
    printDivider(40);
    printf("   FIFA WORLD CUP 2026 SIMULATOR\n");
    printDivider(40);
    printf("  1. Start New Tournament\n");
    printf("  2. Resume Tournament\n");
    printf("  3. Statistics\n");
    printf("  4. Teams and Players\n");
    printf("  5. Quit\n");
    printDivider(40);
    printf("  Choice: ");
}

void printTournamentMenu(Stage current)
{
    printf("\n");
    printDivider(40);
    printf("  TOURNAMENT  [%s]\n", stageToStr(current));
    printDivider(40);
    printf("  1. Simulate next match\n");
    printf("  2. Simulate all remaining in stage\n");
    printf("  3. View standings / bracket\n");
    printf("  4. View top scorers\n");
    printf("  5. Back\n");
    printDivider(40);
    printf("  Choice: ");
}

void printCRUDMenu(void)
{
    printf("\n");
    printDivider(40);
    printf("  TEAMS AND PLAYERS\n");
    printDivider(40);
    printf("  1. View all groups\n");
    printf("  2. View team squad\n");
    printf("  3. View player profile\n");
    printf("  4. search player profile\n");
    printf("  5. view player profile\n");
    printf("  6. Back\n");
    printDivider(40);
    printf("  Choice: ");
}

void printStatsMenu(void)
{
    printf("\n");
    printDivider(40);
    printf("  STATISTICS\n");
    printDivider(40);
    printf("  1. Group standings\n");
    printf("  2. Top scorers\n");
    printf("  3. Knockout bracket\n");
    printf("  4. All results\n");
    printf("  5. Back\n");
    printDivider(40);
    printf("  Choice: ");
}

/* ── group standings ─────────────────────────────────────── */

void printGroupStandings(TournamentTeam *group, int count, Country *countries, int c_count)
{
    printf("  %-23s %3s %3s %3s %3s %4s %4s %4s %4s\n",
           "Team",
           "MP",
           "W",
           "D",
           "L",
           "GF",
           "GA",
           "GD",
           "PTS");
    printDivider(56);

    for (int i = 0; i < count; i++)
    {
        const char *name = findCountryName(group[i].country_id, countries, c_count);
        printf("  %-23s %3d %3d %3d %3d %4d %4d %4d %4d\n",
               name,
               group[i].matches_played,
               group[i].wins,
               group[i].draws,
               group[i].losses,
               group[i].goals_for,
               group[i].goals_against,
               group[i].goal_difference,
               group[i].points);
    }
}

void printAllGroups(TournamentTeam *teams, int t_count, Country *countries, int c_count)
{
    char letters[] = "ABCDEFGHIJKL";

    for (int g = 0; g < NUM_GROUPS; g++)
    {
        TournamentTeam group[TEAMS_PER_GROUP];
        int            found = 0;

        for (int i = 0; i < t_count && found < TEAMS_PER_GROUP; i++)
            if (teams[i].group_name[0] == letters[g])
                group[found++] = teams[i];

        if (found == 0)
            continue;

        printf("\n  === GROUP %c ===\n", letters[g]);
        printGroupStandings(group, found, countries, c_count);
    }
}

/* ── bracket ─────────────────────────────────────────────── */

void printBracket(Match *matches, int m_count, Country *countries, int c_count)
{
    Stage       stages[] = {ROUND_OF_32, ROUND_OF_16, QUARTER_FINAL, SEMI_FINAL, FINAL};
    const char *labels[] = {"ROUND OF 32", "ROUND OF 16", "QUARTER FINALS", "SEMI FINALS", "FINAL"};
    int         n_stages = 5;

    for (int s = 0; s < n_stages; s++)
    {
        int has_matches = 0;
        for (int i = 0; i < m_count; i++)
            if (matches[i].stage == stages[s])
            {
                has_matches = 1;
                break;
            }
        if (!has_matches)
            continue;

        printf("\n  === %s ===\n", labels[s]);
        printDivider(44);

        for (int i = 0; i < m_count; i++)
        {
            if (matches[i].stage != stages[s])
                continue;

            if (!matches[i].is_simulated)
            {
                printf("  TBD vs TBD\n");
            }
            else
            {
                const char *t1 = findCountryName(matches[i].team1_id, countries, c_count);
                const char *t2 = findCountryName(matches[i].team2_id, countries, c_count);
                printf("  %-20s %d - %d  %s\n",
                       t1,
                       matches[i].team1_score,
                       matches[i].team2_score,
                       t2);
            }
        }
    }
}

/* ── match result ────────────────────────────────────────── */

void printMatchResult(Match *m, Country *countries, int c_count)
{
    const char *t1 = findCountryName(m->team1_id, countries, c_count);
    const char *t2 = findCountryName(m->team2_id, countries, c_count);

    if (m->stage == GROUP_STAGE)
        printf("  %-23s %d - %d  %-20s  [Group %s]\n",
               t1,
               m->team1_score,
               m->team2_score,
               t2,
               m->group_name);
    else
        printf("  %-23s %d - %d  %-20s  [%s]\n",
               t1,
               m->team1_score,
               m->team2_score,
               t2,
               stageToStr(m->stage));
}

void printAllResults(Match *matches, int m_count, Country *countries, int c_count)
{
    Stage stages[] = {GROUP_STAGE, ROUND_OF_32, ROUND_OF_16, QUARTER_FINAL, SEMI_FINAL, FINAL};
    const char *labels[] = {
        "GROUP STAGE", "ROUND OF 32", "ROUND OF 16", "QUARTER FINALS", "SEMI FINALS", "FINAL"};

    for (int s = 0; s < 6; s++)
    {
        int has = 0;
        for (int i = 0; i < m_count; i++)
            if (matches[i].stage == stages[s] && matches[i].is_simulated)
            {
                has = 1;
                break;
            }
        if (!has)
            continue;

        printf("\n  === %s ===\n", labels[s]);
        printDivider(56);

        for (int i = 0; i < m_count; i++)
            if (matches[i].stage == stages[s] && matches[i].is_simulated)
                printMatchResult(&matches[i], countries, c_count);
    }
}

/* ── player profile ──────────────────────────────────────── */

void printPlayerProfile(Player *p, PlayerStats *ps, Country *countries, int c_count)
{
    const char *country = findCountryName(p->country_id, countries, c_count);

    printf("\n");
    printDivider(40);
    printf("  %-20s  %s\n", p->name, country);
    printDivider(40);
    printf("  Age:       %d\n", p->age);
    printf("  Position:  %s\n", positionToStr(p->position));
    printf("\n");
    printf("  --- Attributes ---\n");
    printf("  Speed:     %d\n", p->speed);
    printf("  Passing:   %d\n", p->passing);
    printf("  Shooting:  %d\n", p->shooting);
    printf("  Defending: %d\n", p->defending);
    printf("  Physical:  %d\n", p->physical);
    printf("  Dribbling: %d\n", p->dribbling);

    if (ps != NULL)
    {
        printf("\n");
        printf("  --- Tournament Stats ---\n");
        printf("  Goals:          %d\n", ps->goals);
        printf("  Assists:        %d\n", ps->assists);
        printf("  Matches Played: %d\n", ps->matches_played);
        printf("  Status:         %s\n", ps->is_injured ? "INJURED" : "FIT");
    }
    printDivider(40);
}

void printGKProfile(Goalkeeper *gk, GKStats *gs, Country *countries, int c_count)
{
    const char *country = findCountryName(gk->country_id, countries, c_count);

    printf("\n");
    printDivider(40);
    printf("  %-20s  %s\n", gk->name, country);
    printDivider(40);
    printf("  Age:       %d\n", gk->age);
    printf("  Position:  GK\n");
    printf("\n");
    printf("  --- Attributes ---\n");
    printf("  Diving:      %d\n", gk->diving);
    printf("  Handling:    %d\n", gk->handling);
    printf("  Kicking:     %d\n", gk->kicking);
    printf("  Reflexes:    %d\n", gk->reflexes);
    printf("  Speed:       %d\n", gk->speed);
    printf("  Positioning: %d\n", gk->positioning);

    if (gs != NULL)
    {
        printf("\n");
        printf("  --- Tournament Stats ---\n");
        printf("  Clean Sheets:   %d\n", gs->clean_sheets);
        printf("  Goals Conceded: %d\n", gs->goals_conceded);
        printf("  Matches Played: %d\n", gs->matches_played);
        printf("  Status:         %s\n", gs->is_injured ? "INJURED" : "FIT");
    }
    printDivider(40);
}

/* ── top scorers ─────────────────────────────────────────── */

void printTopScorers(Player      *players,
                     int          p_count,
                     PlayerStats *pstats,
                     int          ps_count,
                     Country     *countries,
                     int          c_count,
                     int          top_n)
{
    /* build index array, sort by goals DESC */
    int idx[MAX_PLAYERS];
    for (int i = 0; i < ps_count; i++)
        idx[i] = i;

    /* simple insertion sort — ps_count max 1248, fast enough */
    for (int i = 1; i < ps_count; i++)
    {
        int key = idx[i];
        int j   = i - 1;
        while (j >= 0 && pstats[idx[j]].goals < pstats[key].goals)
        {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }

    printf("\n  === TOP SCORERS ===\n");
    printDivider(52);
    printf("  %-4s %-20s %-16s %5s %5s\n", "Rank", "Player", "Country", "Goals", "Ast");
    printDivider(52);

    int printed = 0;
    for (int i = 0; i < ps_count && printed < top_n; i++)
    {
        PlayerStats *ps = &pstats[idx[i]];
        if (ps->goals == 0)
            break;

        /* find player */
        Player *p = NULL;
        for (int j = 0; j < p_count; j++)
            if (players[j].id == ps->player_id)
            {
                p = &players[j];
                break;
            }
        if (p == NULL)
            continue;

        const char *country = findCountryName(p->country_id, countries, c_count);

        printf(
            "  %-4d %-20s %-16s %5d %5d\n", printed + 1, p->name, country, ps->goals, ps->assists);
        printed++;
    }
    printDivider(52);
}

/* ── team squad ──────────────────────────────────────────── */

void printTeamSquad(int          country_id,
                    Player      *players,
                    int          p_count,
                    Goalkeeper  *gks,
                    int          g_count,
                    PlayerStats *pstats,
                    int          ps_count,
                    GKStats     *gkstats,
                    int          gs_count,
                    Country     *countries,
                    int          c_count)
{
    const char *name = findCountryName(country_id, countries, c_count);

    printf("\n");
    printDivider(50);
    printf("  SQUAD: %s\n", name);
    printDivider(50);

    /* goalkeepers */
    printf("\n  GOALKEEPERS\n");
    for (int i = 0; i < g_count; i++)
    {
        if (gks[i].country_id != country_id)
            continue;
        GKStats *gs = NULL;
        for (int j = 0; j < gs_count; j++)
            if (gkstats[j].gk_id == gks[i].id)
            {
                gs = &gkstats[j];
                break;
            }
        printf("  %-24s OVR:%-3d  %s\n",
               gks[i].name,
               overallGK(&gks[i]),
               (gs && gs->is_injured) ? "[INJURED]" : "");
    }

    /* outfield by position */
    Position    order[]      = {DEF, MID, FWD};
    const char *pos_labels[] = {"DEFENDERS", "MIDFIELDERS", "FORWARDS"};

    for (int p = 0; p < 3; p++)
    {
        printf("\n  %s\n", pos_labels[p]);
        for (int i = 0; i < p_count; i++)
        {
            if (players[i].country_id != country_id)
                continue;
            if (players[i].position != order[p])
                continue;

            PlayerStats *ps = NULL;
            for (int j = 0; j < ps_count; j++)
                if (pstats[j].player_id == players[i].id)
                {
                    ps = &pstats[j];
                    break;
                }

            printf("  %-24s OVR:%-3d  %s\n",
                   players[i].name,
                   overallRating(&players[i]),
                   (ps && ps->is_injured) ? "[INJURED]" : "");
        }
    }
    printDivider(50);
}

/* ── champion announcement ───────────────────────────────── */

void printChampion(int country_id, Country *countries, int c_count)
{
    const char *name = findCountryName(country_id, countries, c_count);
    printf("\n");
    printDivider(50);
    printf("\n");
    printf("      WORLD CUP 2026 CHAMPION\n\n");
    printf("          ** %s **\n\n", name);
    printDivider(50);
    printf("\n");
}
