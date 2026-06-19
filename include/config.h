#ifndef CONFIG_H
#define CONFIG_H

/* ── stat bounds ─────────────────────────────────────────── */
#define STAT_MIN 55
#define STAT_MAX 99

/* ── array capacities ────────────────────────────────────── */
#define MAX_PLAYERS 1300
#define MAX_GKS 200
#define MAX_TEAMS 48
#define MAX_GROUPS 12
#define MAX_MATCHES 200
#define MAX_FIXTURES 200
#define MAX_GOAL_EVENTS 20

/* ── squad layout ────────────────────────────────────────── */
#define SQUAD_SIZE 26
#define GKS_PER_TEAM 3
#define OUTFIELD_PER_TEAM 23
#define XI_OUTFIELD 10 /* outfield slots in starting XI */
#define XI_DEF 4
#define XI_MID 3
#define XI_FWD 3

/* ── simulation ──────────────────────────────────────────── */
#define GOAL_EVENT_DELAY 800 /* ms between goal playback events */
#define WIN_THRESHOLD 0.55f
#define LOSS_THRESHOLD 0.45f
#define RAND_SWING 0.30f  /* full width of random factor     */
#define RAND_OFFSET 0.15f /* subtract to centre around 0     */

/* ── string buffer lengths ───────────────────────────────── */
#define NAME_LEN 64
#define GROUP_NAME_LEN 4
#define DATE_LEN 12
#define HOST_LEN 128
#define TOURNEY_NAME_LEN 128
#define LINE_BUF 256

/* ── tournament format ───────────────────────────────────── */
#define NUM_GROUPS 12
#define TEAMS_PER_GROUP 4
#define GROUPS_ADVANCE 2       /* top 2 per group qualify        */
#define BEST_THIRD_ADVANCE 8   /* best 8 third-place teams       */
#define KNOCKOUT_START_SIZE 32 /* R32 field size                 */

/* ── tournament id (single-tournament build) ─────────────── */
#define TOURNAMENT_ID 1

#endif
