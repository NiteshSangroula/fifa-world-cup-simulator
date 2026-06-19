#include "display.h"
#include "structs.h"
#include "config.h"
#include <stdio.h>

void printGroupStandings(TournamentTeam *group, int count, Country *countries)
{
    (void)countries; /* remove if used */

    printf("%-25s %3s %3s %3s %3s %4s %4s %4s %4s\n",
           "Team", "MP", "W", "D", "L", "GF", "GA", "GD", "PTS");

    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        printf("%-25s %3d %3d %3d %3d %4d %4d %4d %4d\n",
               countries[group[i].country_id - 1].name,
               group[i].matches_played,
               group[i].wins,
               group[i].draws,
               group[i].losses,
               group[i].goals_for,
               group[i].goals_against,
               group[i].goal_difference,
               group[i].points);
    }
}

void printAllGroups(TournamentTeam *teams,
                    int team_count,
                    Country *countries)
{
    for (char group = 'A'; group <= 'L'; group++)
    {
        TournamentTeam group_teams[4];   /* World Cup groups usually have 4 teams */
        int count = 0;

        for (int i = 0; i < team_count; i++)
        {
            if (teams[i].group_name[0] == group)
            {
                group_teams[count++] = teams[i];
            }
        }

        if (count > 0)
        {
            printf("\n=== Group %c ===\n", group);
            printGroupStandings(group_teams, count, countries);
        }
    }
}