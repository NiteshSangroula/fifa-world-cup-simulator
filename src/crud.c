#include "crud.h"

#include "config.h"
#include "display.h"
#include "structs.h"
#include "validation.h"

#include <stdio.h>
#include <string.h>

/* ── 1. view all groups ──────────────────────────────────── */

void viewAllGroups(TournamentTeam *teams, int t_count, Country *countries, int c_count)
{
    printAllGroups(teams, t_count, countries, c_count);
}

/* ── 2. view team squad ──────────────────────────────────── */

void viewTeamSquad(int          country_id,
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
    /* validate country exists */
    int found = 0;
    for (int i = 0; i < c_count; i++)
        if (countries[i].id == country_id)
        {
            found = 1;
            break;
        }

    if (!found)
    {
        printf("  Country ID %d not found.\n", country_id);
        return;
    }

    printTeamSquad(country_id,
                   players,
                   p_count,
                   gks,
                   g_count,
                   pstats,
                   ps_count,
                   gkstats,
                   gs_count,
                   countries,
                   c_count);
}

/* ── 3. view player profile ──────────────────────────────── */

void viewPlayerProfile(int          player_id,
                       Player      *players,
                       int          p_count,
                       PlayerStats *pstats,
                       int          ps_count,
                       Country     *countries,
                       int          c_count)
{
    Player *p = NULL;
    for (int i = 0; i < p_count; i++)
        if (players[i].id == player_id)
        {
            p = &players[i];
            break;
        }

    if (p == NULL)
    {
        printf("  Player ID %d not found.\n", player_id);
        return;
    }

    PlayerStats *ps = NULL;
    for (int i = 0; i < ps_count; i++)
        if (pstats[i].player_id == player_id)
        {
            ps = &pstats[i];
            break;
        }

    printPlayerProfile(p, ps, countries, c_count);
}

/* ── 4. view GK profile ──────────────────────────────────── */

void viewGKProfile(int         gk_id,
                   Goalkeeper *gks,
                   int         g_count,
                   GKStats    *gkstats,
                   int         gs_count,
                   Country    *countries,
                   int         c_count)
{
    Goalkeeper *gk = NULL;
    for (int i = 0; i < g_count; i++)
        if (gks[i].id == gk_id)
        {
            gk = &gks[i];
            break;
        }

    if (gk == NULL)
    {
        printf("  Goalkeeper ID %d not found.\n", gk_id);
        return;
    }

    GKStats *gs = NULL;
    for (int i = 0; i < gs_count; i++)
        if (gkstats[i].gk_id == gk_id)
        {
            gs = &gkstats[i];
            break;
        }

    printGKProfile(gk, gs, countries, c_count);
}

/* ── 5. view country list ────────────────────────────────── */

void viewAllCountries(Country *countries, int c_count)
{
    printf("\n  === ALL TEAMS ===\n");
    for (int i = 0; i < 44; i++)
        printf("-");
    printf("\n");
    printf("  %-4s  %-24s  %-10s  %s\n", "ID", "Country", "Confed", "FIFA Rank");
    for (int i = 0; i < 44; i++)
        printf("-");
    printf("\n");

    for (int i = 0; i < c_count; i++)
        printf("  %-4d  %-24s  %-10s  %d\n",
               countries[i].id,
               countries[i].name,
               confedToStr(countries[i].confederation),
               countries[i].fifa_ranking);

    for (int i = 0; i < 44; i++)
        printf("-");
    printf("\n");
}

/* ── 6. search player by name ────────────────────────────── */

void searchPlayer(const char *name_query,
                  Player     *players,
                  int         p_count,
                  Goalkeeper *gks,
                  int         g_count,
                  Country    *countries,
                  int         c_count)
{
    printf("\n  Search results for \"%s\":\n", name_query);
    for (int i = 0; i < 44; i++)
        printf("-");
    printf("\n");

    int found = 0;

    for (int i = 0; i < p_count; i++)
        if (strstr(players[i].name, name_query) != NULL)
        {
            const char *country = "Unknown";
            for (int j = 0; j < c_count; j++)
                if (countries[j].id == players[i].country_id)
                {
                    country = countries[j].name;
                    break;
                }

            printf("  [%-4d]  %-24s  %-16s  %s\n",
                   players[i].id,
                   players[i].name,
                   country,
                   positionToStr(players[i].position));
            found++;
        }

    for (int i = 0; i < g_count; i++)
        if (strstr(gks[i].name, name_query) != NULL)
        {
            const char *country = "Unknown";
            for (int j = 0; j < c_count; j++)
                if (countries[j].id == gks[i].country_id)
                {
                    country = countries[j].name;
                    break;
                }

            printf("  [%-4d]  %-24s  %-16s  GK\n", gks[i].id, gks[i].name, country);
            found++;
        }

    if (found == 0)
        printf("  No players found matching \"%s\".\n", name_query);

    for (int i = 0; i < 44; i++)
        printf("-");
    printf("\n");
}

/* ── 7. view tournament summary ──────────────────────────── */

// doesn't work
void viewTournamentSummary(TournamentState *state, Country *countries, int c_count)
{
    printf("\n  === TOURNAMENT SUMMARY ===\n");
    for (int i = 0; i < 40; i++)
        printf("-");
    printf("\n");
    printf("  Name:    %s\n", state->tournament_name);
    printf("  Stage:   %s\n", stageToStr(state->current_stage));
    printf("  Groups:  %s\n", state->groups_completed ? "Complete" : "In Progress");
    printf("  Knockouts: %s\n", state->knockouts_completed ? "Complete" : "In Progress");

    if (state->tournament_winner > 0)
    {
        const char *winner = "Unknown";
        for (int i = 0; i < c_count; i++)
            if (countries[i].id == state->tournament_winner)
            {
                winner = countries[i].name;
                break;
            }
        printf("  Champion: %s\n", winner);
    }
    for (int i = 0; i < 40; i++)
        printf("-");
    printf("\n");
}
