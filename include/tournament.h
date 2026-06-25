#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "structs.h"

/* Assigns all 48 countries to groups A–L and zeroes all stats.
 * If random_draw == 0 reads wc_2026_groups.csv via file_io.
 * If random_draw == 1 shuffles randomly. */
void initializeGroups(TournamentTeam *teams, int t_id);

/* Generates all round-robin group fixtures (72 total) in matchday order.
 * Writes into schedule[] and sets *fixture_count. */
void generateMatchSchedule(Fixture        *schedule,
                           TournamentTeam *teams,
                           int             t_count,
                           int            *fixture_count);

void markPlayedFixtures(Fixture *schedule, int f_count, Match *matches, int m_count);

/* Simulates up to batch_size unplayed group fixtures.
 * Updates team stats and persists to CSV after each match. */
void simulateGroupStage(Fixture         *schedule,
                        int              f_count,
                        TournamentTeam  *teams,
                        int              t_count,
                        Match           *matches,
                        int             *m_count,
                        Player          *players,
                        int              p_count,
                        Goalkeeper      *gks,
                        int              g_count,
                        PlayerStats     *pstats,
                        int              ps_count,
                        GKStats         *gkstats,
                        int              gs_count,
                        Country         *countries,
                        int              c_count,
                        TournamentState *state,
                        int              batch_size);

/* Sorts teams within one group by points DESC, GD DESC, GF DESC.
 * group_teams must point to exactly TEAMS_PER_GROUP entries. */
void calculateStandings(TournamentTeam *group_teams, int count);

void calculateAllStandings(TournamentTeam *teams, int t_count);

/* Collects the 12 third-place teams, picks the best 8 by ranking criteria.
 * Writes their country_ids into best_third_ids[].
 * Returns the number written (always 8 if all groups are complete). */
int selectBestThird(TournamentTeam *all_teams, int team_count, int *best_third_ids);

/* Seeds the 32 qualified teams into the R32 bracket.
 * Creates Match records for all R32 fixtures with is_simulated = 0. */
void generateKnockoutBracket(
    TournamentTeam *teams, int t_count, Match *matches, int *m_count, int tournament_id);

/* Simulates up to batch_size unplayed knockout matches.
 * Advances winners, sets stage_reached for eliminated teams,
 * generates the next round when the current one completes. */
void advanceKnockout(Match           *matches,
                     int             *m_count,
                     TournamentTeam  *teams,
                     int              t_count,
                     Player          *players,
                     int              p_count,
                     Goalkeeper      *gks,
                     int              g_count,
                     PlayerStats     *pstats,
                     int              ps_count,
                     GKStats         *gkstats,
                     int              gs_count,
                     Country         *countries,
                     int              c_count,
                     TournamentState *state,
                     int              batch_size);

#endif
