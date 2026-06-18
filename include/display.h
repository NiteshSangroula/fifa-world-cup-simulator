#ifndef DISPLAY_H
#define DISPLAY_H

#include "structs.h"

/* No logic in display.c — pure output only.
 * Every function receives all data as parameters. Never reads files directly. */

void printGroupStandings(TournamentTeam *group, int count, Country *countries);

void printAllGroups(TournamentTeam *teams, int team_count, Country *countries);

void printBracket(Match *matches, int count, Country *countries);

void printPlayerProfile(Player *p, PlayerStats *ps, Country *countries);

void printMatchResult(Match *m, Country *countries);

void printAllResults(Match *matches, int count, Country *countries);

void printTopScorers(
    Player *players, PlayerStats *pstats, int count, Country *countries, int top_n);

void printMainMenu(void);
void printTournamentMenu(Stage current);
void printCRUDMenu(void);
void printStatsMenu(void);

#endif
