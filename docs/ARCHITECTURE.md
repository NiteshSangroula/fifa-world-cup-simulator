# Architecture

This document describes the design and structure of the FIFA World Cup Simulator.

> **Note:** This document will be updated as the project grows. Treat it as a living reference.

---

## Overview

The simulator models a FIFA World Cup tournament from the group stage through to the final. It runs entirely in the terminal and produces match results, standings, and a final champion.

---

## Module Breakdown

The project is divided into focused modules. Each module is a `.c` file in `src/` with a corresponding `.h` header in `include/`.

| Module           | File                  | Responsibility                                      |
|------------------|-----------------------|-----------------------------------------------------|
| Entry point      | `src/main.c`          | Program startup, tournament orchestration           |
| Team             | `src/team.c`          | Team data structure and helpers                     |
| Group            | `src/group.c`         | Group stage simulation, standings, tiebreakers      |
| Match            | `src/match.c`         | Match simulation and outcome engine                 |
| Knockout         | `src/knockout.c`      | R16, QF, SF, Final bracket logic                   |
| Display          | `src/display.c`       | Terminal output, tables, results formatting         |
| Utils            | `src/utils.c`         | Random number helpers, general utilities            |

---

## Data Flow

```
main()
  │
  ├─ Load teams (data/teams.csv)
  │
  ├─ Group Stage
  │    ├─ Simulate all group matches
  │    └─ Compute standings & qualify top 2 per group
  │
  └─ Knockout Stage
       ├─ Round of 16
       ├─ Quarter-finals
       ├─ Semi-finals
       └─ Final → Champion
```

---

## Key Data Structures

These will be defined in the header files. Listed here for planning purposes.

```c
/* A single team */
typedef struct {
    char name[64];
    char code[4];       /* e.g. "BRA", "FRA" */
    int  ranking;       /* FIFA ranking */
    int  strength;      /* Internal strength rating (TBD) */
} Team;

/* Result of one match */
typedef struct {
    Team *home;
    Team *away;
    int   home_goals;
    int   away_goals;
} Match;

/* One group (4 teams) */
typedef struct {
    char  name[4];      /* e.g. "A", "B" */
    Team *teams[4];
    int   points[4];
    int   gd[4];        /* Goal difference */
    int   gf[4];        /* Goals for */
} Group;
```

---

## Match Outcome Engine

*(To be decided by the team — this section will be filled in once the engine is designed.)*

Options under consideration:
- Pure random
- FIFA ranking–weighted probability
- Custom strength ratings

---

## File: `data/teams.csv`

Stores the 32 World Cup teams. Format:

```
name,code,group,ranking
Brazil,BRA,A,1
France,FRA,B,2
...
```

---

## Build System

CMake 3.20+ with C17 standard. See `CMakeLists.txt` for full configuration.

```bash
cmake -S . -B build
cmake --build build
./build/bin/fifa_world_cup_simulator
```
