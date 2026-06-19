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
