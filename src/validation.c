#include "validation.h"

#include "config.h"
#include "structs.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int validateStat(int stat)
{
    if (stat < STAT_MIN)
        return STAT_MIN;
    if (stat > STAT_MAX)
        return STAT_MAX;
    return stat;
}

int sanitizeInt(const char *str, int *out)
{
    char *end;
    errno    = 0;
    long val = strtol(str, &end, 10);
    if (errno != 0 || end == str || (*end != '\0' && *end != '\n'))
        return 0;
    *out = (int)val;
    return 1;
}

int confirmAction(const char *prompt)
{
    printf("%s [y/n]: ", prompt);
    char buf[8];
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;
    return (buf[0] == 'y' || buf[0] == 'Y');
}

/* ── Stage ───────────────────────────────────────────────── */

const char *stageToStr(Stage s)
{
    switch (s)
    {
    case GROUP_STAGE:
        return "GROUP_STAGE";
    case ROUND_OF_32:
        return "ROUND_OF_32";
    case ROUND_OF_16:
        return "ROUND_OF_16";
    case QUARTER_FINAL:
        return "QUARTER_FINAL";
    case SEMI_FINAL:
        return "SEMI_FINAL";
    case FINAL:
        return "FINAL";
    default:
        return "UNKNOWN";
    }
}

Stage strToStage(const char *str)
{
    if (str == NULL)
        return GROUP_STAGE;
    if (strcmp(str, "GROUP_STAGE") == 0)
        return GROUP_STAGE;
    if (strcmp(str, "ROUND_OF_32") == 0)
        return ROUND_OF_32;
    if (strcmp(str, "ROUND_OF_16") == 0)
        return ROUND_OF_16;
    if (strcmp(str, "QUARTER_FINAL") == 0)
        return QUARTER_FINAL;
    if (strcmp(str, "SEMI_FINAL") == 0)
        return SEMI_FINAL;
    if (strcmp(str, "FINAL") == 0)
        return FINAL;
    return GROUP_STAGE;
}

/* ── Confederation ───────────────────────────────────────── */

const char *confedToStr(Confederation c)
{
    switch (c)
    {
    case UEFA:
        return "UEFA";
    case CONMEBOL:
        return "CONMEBOL";
    case CONCACAF:
        return "CONCACAF";
    case AFC:
        return "AFC";
    case CAF:
        return "CAF";
    case OFC:
        return "OFC";
    case CONFEDERATION_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

Confederation strToConfed(const char *str)
{
    if (strcmp(str, "UEFA") == 0)
        return UEFA;
    if (strcmp(str, "CONMEBOL") == 0)
        return CONMEBOL;
    if (strcmp(str, "CONCACAF") == 0)
        return CONCACAF;
    if (strcmp(str, "AFC") == 0)
        return AFC;
    if (strcmp(str, "CAF") == 0)
        return CAF;
    if (strcmp(str, "OFC") == 0)
        return OFC;
    return CONFEDERATION_UNKNOWN;
}

/* ── Position ────────────────────────────────────────────── */

const char *positionToStr(Position p)
{
    switch (p)
    {
    case GK:
        return "GK";
    case DEF:
        return "DEF";
    case MID:
        return "MID";
    case FWD:
        return "FWD";
    default:
        return "UNKNOWN";
    }
}

Position strToPosition(const char *str)
{
    if (strcmp(str, "GK") == 0)
        return GK;
    if (strcmp(str, "DEF") == 0)
        return DEF;
    if (strcmp(str, "MID") == 0)
        return MID;
    if (strcmp(str, "FWD") == 0)
        return FWD;
    return MID;
}

Position mapPosition(const char *pos_str)
{
    if (strcmp(pos_str, "GK") == 0)
        return GK;

    if (strcmp(pos_str, "CB") == 0 || strcmp(pos_str, "RB") == 0 || strcmp(pos_str, "LB") == 0 ||
        strcmp(pos_str, "RWB") == 0 || strcmp(pos_str, "LWB") == 0)
        return DEF;

    if (strcmp(pos_str, "CDM") == 0 || strcmp(pos_str, "CM") == 0 || strcmp(pos_str, "CAM") == 0 ||
        strcmp(pos_str, "LM") == 0 || strcmp(pos_str, "RM") == 0)
        return MID;

    if (strcmp(pos_str, "ST") == 0 || strcmp(pos_str, "LW") == 0 || strcmp(pos_str, "RW") == 0 ||
        strcmp(pos_str, "CF") == 0)
        return FWD;

    return MID;
}
