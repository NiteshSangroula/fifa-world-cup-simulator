#include "awards.h"

#include "config.h"
#include "display.h"
#include "platform.h"
#include "structs.h"
#include "validation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── internal helpers ────────────────────────────────────── */

static const char *findCountryName(int country_id, Country *countries, int c_count)
{
    for (int i = 0; i < c_count; i++)
        if (countries[i].id == country_id)
            return countries[i].name;
    return "Unknown";
}

static void announceAward(const char *award_name)
{
    printf("\n");
    for (int i = 0; i < 44; i++)
        printf("=");
    printf("\n  %s\n", award_name);
    for (int i = 0; i < 44; i++)
        printf("=");
    printf("\n");
    sleep_ms(600);
}

/* ── 1. golden boot ──────────────────────────────────────── */

void getGoldenBoot(PlayerStats *pstats,
                   int          ps_count,
                   Player      *players,
                   int          p_count,
                   Country     *countries,
                   int          c_count)
{
    announceAward("GOLDEN BOOT — Top Scorer");

    int best = -1;
    for (int i = 0; i < ps_count; i++)
        if (best == -1 || pstats[i].goals > pstats[best].goals)
            best = i;

    if (best == -1 || pstats[best].goals == 0)
    {
        printf("  No goals scored in the tournament.\n");
        return;
    }

    Player *p = NULL;
    for (int i = 0; i < p_count; i++)
        if (players[i].id == pstats[best].player_id)
        {
            p = &players[i];
            break;
        }

    if (p == NULL)
        return;

    const char *country = findCountryName(p->country_id, countries, c_count);
    printf("\n  %s  (%s)\n", p->name, country);
    printf("  Goals:   %d\n", pstats[best].goals);
    printf("  Assists: %d\n", pstats[best].assists);
}

/* ── 2. golden ball ──────────────────────────────────────── */

void getGoldenBall(PlayerStats *pstats,
                   int          ps_count,
                   Player      *players,
                   int          p_count,
                   Country     *countries,
                   int          c_count)
{
    announceAward("GOLDEN BALL — Best Player");

    int best         = -1;
    int best_contrib = -1;

    for (int i = 0; i < ps_count; i++)
    {
        int contrib = pstats[i].goals + pstats[i].assists;
        if (contrib > best_contrib)
        {
            best_contrib = contrib;
            best         = i;
        }
    }

    if (best == -1 || best_contrib == 0)
    {
        printf("  No contributions recorded.\n");
        return;
    }

    Player *p = NULL;
    for (int i = 0; i < p_count; i++)
        if (players[i].id == pstats[best].player_id)
        {
            p = &players[i];
            break;
        }

    if (p == NULL)
        return;

    const char *country = findCountryName(p->country_id, countries, c_count);
    printf("\n  %s  (%s)\n", p->name, country);
    printf("  Goals:   %d\n", pstats[best].goals);
    printf("  Assists: %d\n", pstats[best].assists);
    printf("  Total Contributions: %d\n", best_contrib);
}

/* ── 3. golden glove ─────────────────────────────────────── */

void getGoldenGlove(GKStats        *gkstats,
                    int             gs_count,
                    Goalkeeper     *gks,
                    int             g_count,
                    TournamentTeam *teams,
                    int             t_count,
                    Country        *countries,
                    int             c_count)
{
    announceAward("GOLDEN GLOVE — Best Goalkeeper");

    /* filter: GK whose team reached >= QUARTER_FINAL */
    int best    = -1;
    int best_cs = -1;
    int best_gc = 999;

    for (int i = 0; i < gs_count; i++)
    {
        /* find this GK's country */
        int gk_country = -1;
        for (int j = 0; j < g_count; j++)
            if (gks[j].id == gkstats[i].gk_id)
            {
                gk_country = gks[j].country_id;
                break;
            }
        if (gk_country == -1)
            continue;

        /* find team stage_reached */
        Stage reached = GROUP_STAGE;
        for (int j = 0; j < t_count; j++)
            if (teams[j].country_id == gk_country)
            {
                reached = teams[j].stage_reached;
                break;
            }

        if (reached < QUARTER_FINAL)
            continue;

        /* pick by clean sheets DESC, then goals conceded ASC */
        if (best == -1 || gkstats[i].clean_sheets > best_cs ||
            (gkstats[i].clean_sheets == best_cs && gkstats[i].goals_conceded < best_gc))
        {
            best    = i;
            best_cs = gkstats[i].clean_sheets;
            best_gc = gkstats[i].goals_conceded;
        }
    }

    if (best == -1)
    {
        printf("  No eligible goalkeeper found.\n");
        return;
    }

    Goalkeeper *gk = NULL;
    for (int i = 0; i < g_count; i++)
        if (gks[i].id == gkstats[best].gk_id)
        {
            gk = &gks[i];
            break;
        }

    if (gk == NULL)
        return;

    const char *country = findCountryName(gk->country_id, countries, c_count);
    printf("\n  %s  (%s)\n", gk->name, country);
    printf("  Clean Sheets:   %d\n", gkstats[best].clean_sheets);
    printf("  Goals Conceded: %d\n", gkstats[best].goals_conceded);
    printf("  Matches Played: %d\n", gkstats[best].matches_played);
}

/* ── 4. best young player ────────────────────────────────── */

void getBestYoungPlayer(PlayerStats *pstats,
                        int          ps_count,
                        Player      *players,
                        int          p_count,
                        Country     *countries,
                        int          c_count)
{
    announceAward("BEST YOUNG PLAYER — Under 21");

    int best         = -1;
    int best_contrib = -1;

    for (int i = 0; i < ps_count; i++)
    {
        /* find player */
        Player *p = NULL;
        for (int j = 0; j < p_count; j++)
            if (players[j].id == pstats[i].player_id)
            {
                p = &players[j];
                break;
            }
        if (p == NULL)
            continue;
        if (p->age > 21)
            continue;

        int contrib = pstats[i].goals + pstats[i].assists;
        if (contrib > best_contrib)
        {
            best_contrib = contrib;
            best         = i;
        }
    }

    if (best == -1 || best_contrib == 0)
    {
        printf("  No eligible young player found.\n");
        return;
    }

    Player *p = NULL;
    for (int i = 0; i < p_count; i++)
        if (players[i].id == pstats[best].player_id)
        {
            p = &players[i];
            break;
        }

    if (p == NULL)
        return;

    const char *country = findCountryName(p->country_id, countries, c_count);
    printf("\n  %s  (%s)\n", p->name, country);
    printf("  Age:     %d\n", p->age);
    printf("  Goals:   %d\n", pstats[best].goals);
    printf("  Assists: %d\n", pstats[best].assists);
}

/* ── 5. awards ceremony ──────────────────────────────────── */

void printAwardsCeremony(PlayerStats    *pstats,
                         int             ps_count,
                         GKStats        *gkstats,
                         int             gs_count,
                         Player         *players,
                         int             p_count,
                         Goalkeeper     *gks,
                         int             g_count,
                         TournamentTeam *teams,
                         int             t_count,
                         Country        *countries,
                         int             c_count,
                         int             champion_country_id)
{
    printf("\n\n");
    for (int i = 0; i < 44; i++)
        printf("*");
    printf("\n");
    printf("     FIFA WORLD CUP 2026 AWARDS\n");
    for (int i = 0; i < 44; i++)
        printf("*");
    printf("\n");
    sleep_ms(1200);

    getGoldenBoot(pstats, ps_count, players, p_count, countries, c_count);
    sleep_ms(1200);

    getGoldenBall(pstats, ps_count, players, p_count, countries, c_count);
    sleep_ms(1200);

    getGoldenGlove(gkstats, gs_count, gks, g_count, teams, t_count, countries, c_count);
    sleep_ms(1200);

    getBestYoungPlayer(pstats, ps_count, players, p_count, countries, c_count);
    sleep_ms(1500);

    printChampion(champion_country_id, countries, c_count);
}
