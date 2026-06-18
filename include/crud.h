#ifndef CRUD_H
#define CRUD_H

#include "structs.h"

void viewAllTeams(Country *countries, int c_count, TournamentTeam *teams, int t_count);

void viewTeamSquad(int          country_id,
                   Player      *players,
                   int          p_count,
                   Goalkeeper  *gks,
                   int          g_count,
                   PlayerStats *pstats,
                   GKStats     *gkstats);

void viewPlayerProfile(
    int player_id, Player *players, PlayerStats *pstats, int ps_count, Country *countries);

/* Blocked if tournament has already started (state->groups_completed == 1).
 * Prompts user for stat name and value. Validates. Updates array only —
 * does NOT write back to the static CSV. */
void updatePlayerStat(int player_id, Player *players, int p_count, TournamentState *state);

/* Calls confirmAction() before marking is_injured = 1.
 * Persists via savePlayerStats() immediately. */
void injurePlayer(int player_id, PlayerStats *pstats, int count);

/* Routes to simulateGroupStage() or advanceKnockout() based on current stage.
 * Passes all global arrays through. */
void runSimulation(int              batch_size,
                   TournamentState *state,
                   Fixture         *schedule,
                   int              fixture_count,
                   TournamentTeam  *teams,
                   int              t_count,
                   Match           *matches,
                   int             *match_count,
                   Player          *players,
                   Goalkeeper      *gks,
                   PlayerStats     *pstats,
                   GKStats         *gkstats,
                   Country         *countries);

#endif
