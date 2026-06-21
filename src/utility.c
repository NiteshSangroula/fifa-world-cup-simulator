#include "utility.h"

#include "enums.h"

const char *confederationToString(Confederation conf)
{
    switch (conf)
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

const char *stageToString(Stage stage)
{
    switch (stage)
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
