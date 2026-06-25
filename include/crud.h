#ifndef CRUD_H
#define CRUD_H

#include "structs.h"

// 1
void viewAllTeams(Country *countries, int c_count, TournamentTeam *teams, int t_count);

// 2
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
                   int          c_count);

// 3
void viewPlayerProfile(int          player_id,
                       Player      *players,
                       int          p_count,
                       PlayerStats *pstats,
                       int          ps_count,
                       Country     *countries,
                       int          c_count);

// 4
void viewGKProfile(int         gk_id,
                   Goalkeeper *gks,
                   int         g_count,
                   GKStats    *gkstats,
                   int         gs_count,
                   Country    *countries,
                   int         c_count);

// 5
void viewAllCountries(Country *countries, int c_count);

// 6
void searchPlayer(const char *name_query,
                  Player     *players,
                  int         p_count,
                  Goalkeeper *gks,
                  int         g_count,
                  Country    *countries,
                  int         c_count);

// 7
void viewTournamentSummary(TournamentState *state, Country *countries, int c_count);

#endif
