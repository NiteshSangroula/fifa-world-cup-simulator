#ifndef ENUMS_H
#define ENUMS_H

typedef enum
{
    CONFEDERATION_UNKNOWN = -1,
    UEFA,
    CONMEBOL,
    CONCACAF,
    AFC,
    CAF,
    OFC
} Confederation;

typedef enum
{
    GROUP_STAGE,
    ROUND_OF_32,
    ROUND_OF_16,
    QUARTER_FINAL,
    SEMI_FINAL,
    FINAL
} Stage;

typedef enum
{
    GK,
    DEF,
    MID,
    FWD
} Position;

#endif
