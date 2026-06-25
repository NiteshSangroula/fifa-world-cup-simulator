#include "awards.h"
#include "config.h"
#include "crud.h"
#include "display.h"
#include "file_io.h"
#include "platform.h"
#include "simulation.h"
#include "structs.h"
#include "tournament.h"
#include "validation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------ Global Arrays ------------------------------*/

static Country    countries[MAX_TEAMS];
static Player     players[MAX_PLAYERS];
static Goalkeeper gks[MAX_GKS];

static TournamentTeam teams[MAX_TEAMS];
static PlayerStats    pstats[MAX_PLAYERS];
static GKStats        gkstats[MAX_GKS];
static Match          matches[MAX_MATCHES];
static Fixture        schedule[MAX_FIXTURES];

static TournamentState state;

static int c_count  = 0;
static int p_count  = 0;
static int g_count  = 0;
static int t_count  = 0;
static int ps_count = 0;
static int gs_count = 0;
static int m_count  = 0;
static int f_count  = 0;

/* --------- Input Helpers ----------------------------------*/

static int readInt(const char *prompt)
{
    printf("%s", prompt);
    char buf[32];
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return -1;
    int val;
    if (!sanitizeInt(buf, &val))
        return -1;
    return val;
}

static void readString(const char *prompt, char *out, int max)
{
    printf("%s", prompt);
    if (fgets(out, max, stdin) == NULL)
    {
        out[0] = '\0';
        return;
    }
    int len = (int)strlen(out);
    if (len > 0 && out[len - 1] == '\n')
        out[len - 1] = '\0';
}

static void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* ------------- Load Static Data ------------------------ */
static int loadStaticData(void)
{
    c_count = loadCountries(countries, MAX_TEAMS);
    p_count = loadPlayers(players, MAX_PLAYERS);
    g_count = loadGoalkeepers(gks, MAX_GKS);

    if (c_count <= 0)
    {
        printf("Fatal: could not load countries. Check data/static/.\n");
        return 0;
    }
    if (p_count <= 0)
    {
        printf("Fatal: could not load players. Check data/static/.\n");
        return 0;
    }
    if (g_count <= 0)
    {
        printf("Fatal: could not load goalkeepers. Check data/static/.\n");
        return 0;
    }
    return 1;
}

/* ------------- Load Dynamic Data ----------------------- */
static void loadDynamicData(void)
{
    t_count  = loadTournamentTeams(teams, MAX_TEAMS);
    ps_count = loadPlayerStats(pstats, MAX_PLAYERS);
    gs_count = loadGKStats(gkstats, MAX_GKS);
    m_count  = loadMatches(matches, MAX_MATCHES);
}

/* ----------- init pstats and gkstats for new tournament */
static void initStats(void)
{
    for (int i = 0; i < p_count; i++)
    {
        pstats[i].tournament_id  = TOURNAMENT_ID;
        pstats[i].player_id      = players[i].id;
        pstats[i].goals          = 0;
        pstats[i].assists        = 0;
        pstats[i].matches_played = 0;
        pstats[i].is_injured     = 0;
    }
    for (int i = 0; i < g_count; i++)
    {
        gkstats[i].tournament_id  = TOURNAMENT_ID;
        gkstats[i].gk_id          = gks[i].id;
        gkstats[i].clean_sheets   = 0;
        gkstats[i].goals_conceded = 0;
        gkstats[i].matches_played = 0;
        gkstats[i].is_injured     = 0;
    }
    gs_count = g_count;

    savePlayerStats(pstats, ps_count);
    saveGKStats(gkstats, gs_count);
}

/* ------ Crud Menu ------------------------------- */
static void handleCRUDMenu(void)
{
    int running = 1;
    while (running)
    {
        printCRUDMenu();
        int choice = readInt("");
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            viewAllGroups(teams, t_count, countries, c_count);
            break;

        case 2:
            viewAllCountries(countries, c_count);
            break;

        case 3:
        {
            viewAllCountries(countries, c_count);
            int id = readInt("  Enter Country ID: ");
            clearInputBuffer();
            viewTeamSquad(id,
                          players,
                          p_count,
                          gks,
                          g_count,
                          pstats,
                          ps_count,
                          gkstats,
                          gs_count,
                          countries,
                          c_count);
            break;
        }

        case 4:
        {
            char query[NAME_LEN];
            readString("  Enter player name: ", query, NAME_LEN);
            searchPlayer(query, players, p_count, gks, g_count, countries, c_count);
            break;
        }

        case 5:
        {
            int id = readInt("  Enter Player ID: ");
            clearInputBuffer();
            viewPlayerProfile(id, players, p_count, pstats, ps_count, countries, c_count);
            break;
        }

        case 6:
            running = 0;
            break;

        default:
            printf("  Invalid choice.\n");
        }
    }
}

/* ------ Stats Menu ------------------------------- */

static void handleStatsMenu(void)
{
    int running = 1;
    while (running)
    {
        printStatsMenu();
        int choice = readInt("");
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            printAllGroups(teams, t_count, countries, c_count);
            break;

        case 2:
            printTopScorers(players, p_count, pstats, ps_count, countries, c_count, 10);
            break;

        case 3:
            printBracket(matches, m_count, countries, c_count);
            break;

        case 4:
            printAllResults(matches, m_count, countries, c_count);
            break;

        case 5:
            running = 0;
            break;

        default:
            printf("  Invalid choice.\n");
        }
    }
}

/* ------ Tournament Menu ------------------------------- */
static void handleTournamentMenu(void)
{
    int running = 1;
    while (running)
    {
        /* check if tournament just finished */
        if (state.knockouts_completed)
        {
            printAwardsCeremony(pstats,
                                ps_count,
                                gkstats,
                                gs_count,
                                players,
                                p_count,
                                gks,
                                g_count,
                                teams,
                                t_count,
                                countries,
                                c_count,
                                state.tournament_winner);
            running = 0;
            break;
        }

        printTournamentMenu(state.current_stage);
        int choice = readInt("");
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            /* simulate one match */
            if (!state.groups_completed)
                simulateGroupStage(schedule,
                                   f_count,
                                   teams,
                                   t_count,
                                   matches,
                                   &m_count,
                                   players,
                                   p_count,
                                   gks,
                                   g_count,
                                   pstats,
                                   ps_count,
                                   gkstats,
                                   gs_count,
                                   countries,
                                   c_count,
                                   &state,
                                   1);
            else
                advanceKnockout(matches,
                                &m_count,
                                teams,
                                t_count,
                                players,
                                p_count,
                                gks,
                                g_count,
                                pstats,
                                ps_count,
                                gkstats,
                                gs_count,
                                countries,
                                c_count,
                                &state,
                                1);
            break;

        case 2:
            /* simulate all remaining in current stage */
            if (!state.groups_completed)
                simulateGroupStage(schedule,
                                   f_count,
                                   teams,
                                   t_count,
                                   matches,
                                   &m_count,
                                   players,
                                   p_count,
                                   gks,
                                   g_count,
                                   pstats,
                                   ps_count,
                                   gkstats,
                                   gs_count,
                                   countries,
                                   c_count,
                                   &state,
                                   72);
            else
                advanceKnockout(matches,
                                &m_count,
                                teams,
                                t_count,
                                players,
                                p_count,
                                gks,
                                g_count,
                                pstats,
                                ps_count,
                                gkstats,
                                gs_count,
                                countries,
                                c_count,
                                &state,
                                expectedMatches(state.current_stage));
            break;

        case 3:
            if (!state.groups_completed)
                printAllGroups(teams, t_count, countries, c_count);
            else
                printBracket(matches, m_count, countries, c_count);
            break;

        case 4:
            printTopScorers(players, p_count, pstats, ps_count, countries, c_count, 10);
            break;

        case 5:
            running = 0;
            break;

        default:
            printf("  Invalid choice.\n");
        }
    }
}

/* ------ start new tournament ------------------------------- */
static void startNewTournament(void)
{
    printf("\n  === NEW TOURNAMENT ===\n");

    char name[TOURNEY_NAME_LEN];
    readString("  Tournament name: ", name, TOURNEY_NAME_LEN);
    if (strlen(name) == 0)
        strncpy(name, "FIFA World Cup 2026", TOURNEY_NAME_LEN - 1);

    printf("  Random draw? (1 = yes, 0 = use real groups): ");
    int random_draw = readInt("");
    clearInputBuffer();
    if (random_draw != 0 && random_draw != 1)
        random_draw = 0;

    /* wipe previous data */
    wipeDynamicFiles();
    m_count = 0;

    /* set up state */
    state.tournament_id       = TOURNAMENT_ID;
    state.groups_completed    = 0;
    state.knockouts_completed = 0;
    state.tournament_winner   = 0;
    state.current_stage       = GROUP_STAGE;
    strncpy(state.tournament_name, name, TOURNEY_NAME_LEN - 1);

    /* init groups and schedule */
    initializeGroups(teams, TOURNAMENT_ID);
    t_count = MAX_TEAMS;

    generateMatchSchedule(schedule, teams, t_count, &f_count);

    /* init stats arrays */
    initStats();

    saveTournamentState(&state);
    saveTournamentTeams(teams, t_count);

    printf("\n  Tournament \"%s\" created!\n", name);
    printf("  %d teams, %d group fixtures generated.\n", t_count, f_count);

    handleTournamentMenu();
}

/* ------ resume tournament ------------------------------- */

static void resumeTournament(void)
{
    loadDynamicData();

    if (!state.groups_completed)
    {
        /* regenerate schedule and mark played fixtures */
        generateMatchSchedule(schedule, teams, t_count, &f_count);
        markPlayedFixtures(schedule, f_count, matches, m_count);

        int played = 0;
        for (int i = 0; i < f_count; i++)
            if (schedule[i].played)
                played++;

        printf("\n  Resuming group stage. %d / %d matches played.\n", played, f_count);
    }
    else if (!state.knockouts_completed)
    {
        printf("\n  Resuming %s.\n", stageToStr(state.current_stage));
    }
    else
    {
        printf("\n  Tournament already complete.\n");
    }

    handleTournamentMenu();
}

/* -------------- MAIN --------------------------------- */
int main(void)
{
    srand((unsigned int)time(NULL));

    printf("Loading data...\n");
    if (!loadStaticData())
        return 1;
    printf("countries: %d", c_count);
    printf("players: %d", p_count);
    printf("goalkeepers: %d", g_count);
    printf("Loaded %d countries, %d players, %d goalkeepers.\n", c_count, p_count, g_count);

    /*

    int has_tournament = loadTournamentState(&state);

    int running = 1;

    while (running)
    {
        printMainMenu();
        int choice = readInt("");
        clearInputBuffer();

        switch (choice)
        {
        case 1:
            startNewTournament();
            has_tournament = 1;
            break;

        case 2:
            if (has_tournament != 1)
                printf("\n No tournament found. Start a new one.\n");
            else
                resumeTournament();
            break;
        case 3:
            handleStatsMenu();
            break;
        case 4:
            handleCRUDMenu();
            break;
        case 5:
            running = 0;
            break;
        default:
            printf(" Invalid choice.\n");
        }
    }

    printf("\n Goodbye.\n");
    */
    return 0;
}
