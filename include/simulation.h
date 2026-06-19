#ifndef SIMULATION_H
#define SIMULATION_H

#include "structs.h"

/* Selects the best non-injured starting XI for country_id.
 * Formation: 1 GK + 4 DEF + 3 MID + 3 FWD (always fixed).
 * Fills xi_ids[XI_OUTFIELD] with outfield player ids,
 * and *xi_gk_id with the goalkeeper id.
 * Returns 1 on success, 0 if a position cannot be filled. */
int pickStartingXI(int          country_id,
                   int         *xi_ids,
                   int         *xi_gk_id,
                   Player      *players,
                   int          p_count,
                   Goalkeeper  *gks,
                   int          g_count,
                   PlayerStats *pstats,
                   GKStats     *gkstats);

/* Computes weighted team strength from the starting XI.
 * Attack  = avg(FWD shooting, MID passing)  × 0.6
 * Defence = avg(DEF defending, GK rating)   × 0.4
 * Returns a float typically in the 65–85 range. */
float calculateTeamStrength(int *xi_ids, int gk_id, Player *players, Goalkeeper *gks);

/* Simulates one match between team1_id and team2_id.
 * Calls pickStartingXI and calculateTeamStrength internally.
 * Writes final scores into *score1 and *score2. */
void simulateMatch(int          team1_id,
                   int          team2_id,
                   int         *score1,
                   int         *score2,
                   Player      *players,
                   Goalkeeper  *gks,
                   PlayerStats *pstats,
                   GKStats     *gkstats);

/* Distributes goals across random minutes 1–90.
 * Assigns scorers weighted by shooting stat (FWD full, MID half, DEF sixth).
 * Populates events[] sorted by minute.
 * Returns total number of events written. */
int generateGoalEvents(int        team1_id,
                       int        team2_id,
                       int        score1,
                       int        score2,
                       int       *xi1,
                       int       *xi2,
                       Player    *players,
                       GoalEvent *events);

/* Prints each goal event with minute, scorer name, team, and running score.
 * Calls sleep_ms(GOAL_EVENT_DELAY) between each event. */
void playbackEvents(GoalEvent *events,
                    int        count,
                    Player    *players,
                    Country   *countries,
                    int        team1_id,
                    int        team2_id,
                    int        score1,
                    int        score2);

#endif
