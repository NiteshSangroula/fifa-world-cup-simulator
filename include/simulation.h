#ifndef SIMULATION_H
#define SIMULATION_H

#include "structs.h"

/* Selects the best non-injured starting XI for country_id.
 * Formation: 1 GK + 4 DEF + 3 MID + 3 FWD (always fixed).
 * Fills xi_ids[XI_OUTFIELD] with outfield player ids,
 * and *xi_gk_id with the goalkeeper id.
 * Returns 1 on success, 0 if a position cannot be filled. */
int pickStartingXI(int          country_id,
                   StartingXI  *out,
                   Player      *players,
                   int          p_count,
                   Goalkeeper  *gks,
                   int          g_count,
                   PlayerStats *pstats,
                   int          ps_count,
                   GKStats     *gkstats,
                   int          gs_count);

/*
 * Calculate Team Strength
 */
float calculateTeamStrength(StartingXI *xi);

/*
 * Simulate Match
 */
void simulateMatch(int          team1_id,
                   int          team2_id,
                   int          is_knockout,
                   int         *score1,
                   int         *score2,
                   StartingXI  *out_xi1,
                   StartingXI  *out_xi2,
                   Player      *players,
                   int          p_count,
                   Goalkeeper  *gks,
                   int          g_count,
                   PlayerStats *pstats,
                   int          ps_count,
                   GKStats     *gkstats,
                   int          gs_count);

/* Distributes goals across random minutes 1–90.
 * Assigns scorers weighted by shooting stat (FWD full, MID half, DEF sixth).
 * Populates events[] sorted by minute.
 * Returns total number of events written. */
int generateGoalEvents(int         team1_id,
                       int         team2_id,
                       int         score1,
                       int         score2,
                       StartingXI *xi1,
                       StartingXI *xi2,
                       Player     *players,
                       int         p_count,
                       GoalEvent  *events);

/* Prints each goal event with minute, scorer name, team, and running score.
 * Calls sleep_ms(GOAL_EVENT_DELAY) between each event. */
void playbackEvents(GoalEvent *events,
                    int        count,
                    Player    *players,
                    int        p_count,
                    Country   *countries,
                    int        c_count,
                    int        team1_id,
                    int        team2_id,
                    int        score1,
                    int        score2);

int overallRating(const Player *p);

int overallGK(const Goalkeeper *gk);

#endif
