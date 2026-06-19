#ifndef AWARDS_H
#define AWARDS_H

#include "structs.h"

/* Each function reads data, sorts a local copy, and prints the winner.
 * Original arrays are never modified. */

void getGoldenBoot(PlayerStats *pstats, int count, Player *players, Country *countries);

void getGoldenBall(PlayerStats *pstats, int count, Player *players, Country *countries);

/* Golden Glove eligibility: GK whose team stage_reached >= QUARTER_FINAL. */
void getGoldenGlove(
    GKStats *gstats, int count, Goalkeeper *gks, TournamentTeam *teams, Country *countries);

/* Best Young Player eligibility: age <= 21. */
void getBestYoungPlayer(PlayerStats *pstats, int count, Player *players, Country *countries);

/* Calls all four award functions with sleep_ms(1200) between each.
 * Prints the tournament champion last. */
void printAwardsCeremony(PlayerStats    *pstats,
                         int             p_count,
                         GKStats        *gstats,
                         int             g_count,
                         Player         *players,
                         Goalkeeper     *gks,
                         TournamentTeam *teams,
                         int             t_count,
                         Country        *countries,
                         int             champion_country_id);

#endif
