#ifndef AWARDS_H
#define AWARDS_H

#include "structs.h"

/* Each function reads data, sorts a local copy, and prints the winner.
 * Original arrays are never modified. */

void getGoldenBoot(PlayerStats *pstats,
                   int          ps_count,
                   Player      *players,
                   int          p_count,
                   Country     *countries,
                   int          c_count);

void getGoldenBall(PlayerStats *pstats,
                   int          ps_count,
                   Player      *players,
                   int          p_count,
                   Country     *countries,
                   int          c_count);

/* Golden Glove eligibility: GK whose team stage_reached >= QUARTER_FINAL. */
void getGoldenGlove(GKStats        *gkstats,
                    int             gs_count,
                    Goalkeeper     *gks,
                    int             g_count,
                    TournamentTeam *teams,
                    int             t_count,
                    Country        *countries,
                    int             c_count);

/* Best Young Player eligibility: age <= 21. */
void getBestYoungPlayer(PlayerStats *pstats,
                        int          ps_count,
                        Player      *players,
                        int          p_count,
                        Country     *countries,
                        int          c_count);

/* Calls all four award functions with sleep_ms(1200) between each.
 * Prints the tournament champion last. */
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
                         int             champion_country_id);

#endif
