#ifndef STRUCTS_H
#define STRUCTS_H

#include "config.h"
#include "enums.h"

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
    Position position;
    int      diving;
    int      handling;
    int      kicking;
    int      reflexes;
    int      speed;
    int      positioning;
} Goalkeeper;

typedef struct
{
    int tournament_id;
    int player_id;
    int goals;
    int assists;
    int matches_played;
    int is_injured;
} PlayerStats;

typedef struct
{
    int tournament_id;
    int gk_id;
    int clean_sheets;
    int goals_conceded;
    int matches_played;
    int is_injured;
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
    char  group_name[GROUP_NAME_LEN];
    int   is_simulated;
    char  match_date[DATE_LEN];
} Match;

typedef struct
{
    int   tournament_id;
    char  tournament_name[128];
    Stage current_stage;
    int   groups_completed;
    int   knockouts_completed;
    int   tournament_winner;
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
    int  random_draw;
} TournamentConfig;

typedef struct
{
    int minute;
    int player_id;
    int assister_id;
    int team_id;
    int is_own_goal;
} GoalEvent;

typedef struct
{
    int team1_id;
    int team2_id;
    int played;
} Fixture;

#endif
