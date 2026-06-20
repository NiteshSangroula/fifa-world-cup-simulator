#ifndef FILE_IO_H
#define FILE_IO_H

#include "structs.h"

#include <stddef.h>

/* ── loaders ─────────────────────────────────────────────────────────────────
 * All return the number of records loaded (>= 0) or -1 on error.
 * Exception: loadTournamentState() returns 1 if loaded, 0 if no tournament
 * exists yet, -1 on error.
 * ─────────────────────────────────────────────────────────────────────────── */

int loadCountries(Country *arr, int max);
int loadPlayers(Player *arr, int max);
int loadGoalkeepers(Goalkeeper *arr, int max);
int loadGroups(TournamentTeam *arr, int max); /* reads wc_2026_groups.csv */

int loadTournamentState(TournamentState *state);
int loadTournamentTeams(TournamentTeam *arr, int max);
int loadPlayerStats(PlayerStats *arr, int max);
int loadGKStats(GKStats *arr, int max);
int loadMatches(Match *arr, int max);

/* ── savers ──────────────────────────────────────────────────────────────────
 * All return 1 on success, -1 on error.
 * saveMatch() appends one row. All others overwrite the whole file.
 * ─────────────────────────────────────────────────────────────────────────── */

int saveMatch(Match *m);
int savePlayerStats(PlayerStats *arr, int count);
int saveGKStats(GKStats *arr, int count);
int saveTournamentTeams(TournamentTeam *arr, int count);
int saveTournamentState(TournamentState *state);

/* ── reset ───────────────────────────────────────────────────────────────── */

/* Wipes all files in data/dynamic/ back to header-row-only.
 * Call when the user starts a new tournament. Returns 1 on success, -1 on error. */
int wipeDynamicFiles(void);

#endif
