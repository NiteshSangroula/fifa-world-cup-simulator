#ifndef STRUCTS_H
#define STRUCTS_H

#include "config.h"
#include "enums.h"

/* ── static data (loaded once, never modified at runtime) ─── */

typedef struct
{
    int           id;
    char          name[NAME_LEN];
    Confederation confederation;
    int           fifa_ranking;
} Country;

typedef struct
{
    int      id;
    char     name[NAME_LEN];
    int      country_id;
    int      age;
    Position position;
    int      speed;
    int      passing;
    int      shooting;
    int      defending;
    int      physical;
    int      dribbling;
} Player;

typedef struct
{
    int      id;
    char     name[NAME_LEN];
    int      country_id;
    int      age;
    Position position; /* Always GK */
    int      diving;
    int      handling;
    int      kicking;
    int      reflexes;
    int      speed;
    int      positioning;
} Goalkeeper;

/* ── dynamic data (wiped and rebuilt each tournament) ─────── */

typedef struct
{
    int tournament_id;
    int player_id;
    int goals;
    int assists;
    int matches_played;
    int is_injured; /* 0 = fit, 1 = injured */
} PlayerStats;

typedef struct
{
    int tournament_id;
    int gk_id;
    int clean_sheets;
    int goals_conceded;
    int matches_played;
    int is_injured; /* 0 = fit, 1 = injured */
} GKStats;

typedef struct
{
    int   tournament_id;
    int   country_id;
    char  group_name[GROUP_NAME_LEN];
    int   matches_played;
    int   wins;
    int   draws;
    int   losses;
    int   goals_for;
    int   goals_against;
    int   goal_difference;
    int   points;
    Stage stage_reached;
} TournamentTeam;

typedef struct
{
    int   id;
    int   tournament_id;
    int   team1_id;
    int   team2_id;
    int   team1_score;
    int   team2_score;
    Stage stage;
    char  group_name[GROUP_NAME_LEN]; /* empty string for knockout matches */
    int   is_simulated;               /* 0 = not yet played, 1 = done     */
    char  match_date[DATE_LEN];
} Match;

typedef struct
{
    int   tournament_id;
    char  tournament_name[128];
    Stage current_stage;
    int   groups_completed;    /* 0 or 1 */
    int   knockouts_completed; /* 0 or 1 */
    int   tournament_winner;   /* country_id, 0 if not finished */
} TournamentState;

typedef struct
{
    char name[128];
    int  num_teams;
    int  num_groups;
    int  teams_per_group;
    int  groups_advance;
    int  best_third_advance;
    char host[128];
    int  random_draw; /* 0 = fixed draw, 1 = randomise */
} TournamentConfig;

/* ── simulation internals ────────────────────────────────── */

typedef struct
{
    int xi_ids[XI_OUTFIELD];
    int gk_id;
    int def_ovr[XI_DEF]; /* overalls already computed */
    int mid_ovr[XI_MID];
    int fwd_ovr[XI_FWD];
    int gk_ovr;
} StartingXI;

typedef struct
{
    int minute;
    int player_id;
    int assister_id; /* -1 if no assist recorded */
    int team_id;
    int is_own_goal; /* 0 = normal, 1 = own goal */
} GoalEvent;

typedef struct
{
    int team1_id;
    int team2_id;
    int played; /* 0 = fixture pending, 1 = simulated */
} Fixture;

#endif
