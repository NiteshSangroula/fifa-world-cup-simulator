#include "simulation.h"

#include <stdlib.h>

typedef struct
{
    int player_index;
    int sort_stat;
} Candidate;

static int cmpCandidatesDesc(const void *a, const void *b)
{
    return ((Candidate *)b)->sort_stat - ((Candidate *)a)->sort_stat;
}

static int overallRating(const Player *p)
{
    switch (p->position)
    {
    case DEF:
        return (p->defending * 40 + p->physical * 25 + p->speed * 15 + p->passing * 12 +
                p->dribbling * 5 + p->shooting * 3) /
               100;

    case MID:
        return (p->passing * 30 + p->dribbling * 20 + p->physical * 15 + p->defending * 15 +
                p->shooting * 10 + p->speed * 10) /
               100;

    case FWD:
        return (p->shooting * 35 + p->speed * 20 + p->dribbling * 15 + p->physical * 15 +
                p->passing * 10 + p->defending * 5) /
               100;

    default:
        return 0;
    }
}

static int overallGK(const Goalkeeper *gk)
{
    return (gk->reflexes * 25 + gk->diving * 20 + gk->positioning * 20 + gk->handling * 15 +
            gk->kicking * 12 + gk->speed * 8) /
           100;
}

/* ── internal helpers ────────────────────────────────────── */

/* finds pstats index for a given player_id, returns -1 if not found */
static int findPstatsIndex(int player_id, PlayerStats *pstats, int ps_count)
{
    for (int i = 0; i < ps_count; i++)
        if (pstats[i].player_id == player_id)
            return i;
    return -1;
}

/* finds gkstats index for a given gk_id, returns -1 if not found */
static int findGKStatsIndex(int gk_id, GKStats *gkstats, int gs_count)
{
    for (int i = 0; i < gs_count; i++)
        if (gkstats[i].gk_id == gk_id)
            return i;
    return -1;
}

/* finds player array index by id, returns -1 if not found */
// static int findPlayerIndex(int player_id, Player *players, int p_count)
// {
//     /* data ID structure as hint: country = id/100, shirt = id%100 */
//     int country_id = player_id / 100;
//     int shirt      = player_id % 100;
//
//     /* estimate starting index — players grouped by country */
//     /* country N starts at roughly (N-1)*OUTFIELD_PER_TEAM */
//     int hint = (country_id - 1) * OUTFIELD_PER_TEAM + (shirt - 1);
//     if (hint >= 0 && hint < p_count && players[hint].id == player_id)
//         return hint;
//
//     /* fallback: linear scan */
//     for (int i = 0; i < p_count; i++)
//         if (players[i].id == player_id)
//             return i;
//     return -1;
// }

// pick the best starting 11 from the whole player array
int pickStartingXI(int          country_id,
                   int         *xi_ids,
                   int         *xi_gk_id,
                   Player      *players,
                   int          p_count,
                   Goalkeeper  *gks,
                   int          g_count,
                   PlayerStats *pstats,
                   GKStats     *gkstats)
{

    Candidate pool[OUTFIELD_PER_TEAM];
    size_t    pool_size;

    /*
    typedef struct
    {
        int      xi_offset;
        Position pos;
        int      stat_offset;
    } PositionCfg;
    */

    /* --- DEF --- */
    pool_size = 0;
    for (int i = 0; i < p_count; i++)
    {
        if (players[i].country_id != country_id)
            continue;
        if (players[i].position != DEF)
            continue;
        int pi = findPstatsIndex(players[i].id, pstats, p_count);
        if (pi != -1 && pstats[pi].is_injured)
            continue;
        pool[pool_size].player_index = i;
        pool[pool_size].sort_stat    = overallRating(&players[i]);
        pool_size++;
    }
    if (pool_size < XI_DEF)
        return 0;
    qsort(pool, pool_size, sizeof(Candidate), cmpCandidatesDesc);

    for (int i = 0; i < XI_DEF; i++)
        xi_ids[i] = players[pool[i].player_index].id;

    /* ── MID ── */
    pool_size = 0;
    for (int i = 0; i < p_count; i++)
    {
        if (players[i].country_id != country_id)
            continue;
        if (players[i].position != MID)
            continue;
        int pi = findPstatsIndex(players[i].id, pstats, p_count);
        if (pi != -1 && pstats[pi].is_injured)
            continue;
        pool[pool_size].player_index = i;
        pool[pool_size].sort_stat    = overallRating(&players[i]);
        pool_size++;
    }
    if (pool_size < XI_MID)
        return 0;
    qsort(pool, pool_size, sizeof(Candidate), cmpCandidatesDesc);
    for (int i = 0; i < XI_MID; i++)
        xi_ids[XI_DEF + i] = players[pool[i].player_index].id;

    /* ── FWD ── */
    pool_size = 0;
    for (int i = 0; i < p_count; i++)
    {
        if (players[i].country_id != country_id)
            continue;
        if (players[i].position != FWD)
            continue;
        int pi = findPstatsIndex(players[i].id, pstats, p_count);
        if (pi != -1 && pstats[pi].is_injured)
            continue;
        pool[pool_size].player_index = i;
        pool[pool_size].sort_stat    = overallRating(&players[i]);
        pool_size++;
    }
    if (pool_size < XI_FWD)
        return 0;
    qsort(pool, pool_size, sizeof(Candidate), cmpCandidatesDesc);
    for (int i = 0; i < XI_FWD; i++)
        xi_ids[XI_DEF + XI_MID + i] = players[pool[i].player_index].id;

    /* ── GK ── */
    Candidate gk_pool[GKS_PER_TEAM];
    size_t    gk_pool_size = 0;
    for (int i = 0; i < g_count; i++)
    {
        if (gks[i].country_id != country_id)
            continue;
        int gi = findGKStatsIndex(gks[i].id, gkstats, g_count);
        if (gi != -1 && gkstats[gi].is_injured)
            continue;
        gk_pool[gk_pool_size].player_index = i;
        gk_pool[gk_pool_size].sort_stat    = overallGK(&gks[i]);
        gk_pool_size++;
    }
    if (gk_pool_size < 1)
        return 0;
    qsort(gk_pool, gk_pool_size, sizeof(Candidate), cmpCandidatesDesc);
    *xi_gk_id = gks[gk_pool[0].player_index].id;

    return 1;
}
