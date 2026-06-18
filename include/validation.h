#ifndef VALIDATION_H
#define VALIDATION_H

#include "structs.h"

/* Clamps stat to [STAT_MIN, STAT_MAX]. */
int validateStat(int stat);

/* Parses str as a base-10 integer into *out.
 * Returns 1 if valid, 0 if the string is not a valid integer. */
int sanitizeInt(const char *str, int *out);

/* Prints prompt followed by " [y/n]: " and reads the response.
 * Returns 1 if the user enters 'y' or 'Y', 0 otherwise. */
int confirmAction(const char *prompt);

/* ── enum ↔ string converters ────────────────────────────────────────────── */

const char *stageToStr(Stage s);
Stage       strToStage(const char *str);

const char   *confedToStr(Confederation c);
Confederation strToConfed(const char *str);

const char *positionToStr(Position p);
Position    strToPosition(const char *str);

/* Maps a detailed position string from CSV ("CAM", "RWB", "ST", etc.)
 * to the four-value Position enum. Falls back to MID if unrecognised. */
Position mapPosition(const char *pos_str);

#endif
