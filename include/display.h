#ifndef DISPLAY_H
#define DISPLAY_H

#include "structs.h"

/* No logic in display.c — pure output only.
 * Every function receives all data as parameters. Never reads files directly. */

void printGroupStandings(TournamentTeam *group, int count, Country *countries, int c_count);

void printAllGroups(TournamentTeam *teams, int t_count, Country *countries, int c_count);

void printBracket(Match *matches, int m_count, Country *countries, int c_count);

void printPlayerProfile(Player *p, PlayerStats *ps, Country *countries, int c_count);

void printMatchResult(Match *m, Country *countries, int c_count);

void printAllResults(Match *matches, int m_count, Country *countries, int c_count);

void printTopScorers(Player      *players,
                     int          p_count,
                     PlayerStats *pstats,
                     int          ps_count,
                     Country     *countries,
                     int          c_count,
                     int          top_n);

void printMainMenu(void);
void printTournamentMenu(Stage current);
void printCRUDMenu(void);
void printStatsMenu(void);

void printChampion(int country_id, Country *countries, int c_count);

#endif
