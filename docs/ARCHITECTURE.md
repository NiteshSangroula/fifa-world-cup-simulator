# Architecture

This document describes the complete design and structure of the World Cup Simulator.
It serves as the primary technical reference for all team members throughout development.

> **Note:** This is a living document. Update it when structural decisions change during development.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Design Principles](#2-design-principles)
3. [Folder Structure](#3-folder-structure)
4. [Data Layer](#4-data-layer)
5. [Structs](#5-structs)
6. [Enums and Constants](#6-enums-and-constants)
7. [Module Breakdown](#7-module-breakdown)
8. [Menu System](#8-menu-system)
9. [Simulation Engine](#9-simulation-engine)
10. [Tournament Format — 2026 Rules](#10-tournament-format--2026-rules)
11. [CRUD Operations](#11-crud-operations)
12. [Awards System](#12-awards-system)
13. [Cross-Platform Support](#13-cross-platform-support)
14. [Build System](#14-build-system)
15. [Git Workflow](#15-git-workflow)
16. [Scope and Extensibility](#16-scope-and-extensibility)

---

## 1. Project Overview

A terminal-based FIFA World Cup 2026 simulator written in C. Simulates a complete
tournament — 48 teams, 12 groups, group stage through to the Final — using real country
and player data, a stat-driven match engine with goal-by-goal playback, full CRUD
operations, and an end-of-tournament awards ceremony.

One active tournament is stored at a time. All state is persisted to CSV files and
is fully resumable on relaunch.

The system is architected for reusability: all tournament parameters are driven by a
config file, meaning the same binary can run any national tournament (Euros, Copa
America, AFCON, etc.) by swapping data and config files — no source code changes required.

---

## 2. Design Principles

| Principle | Description |
|---|---|
| **Data-driven** | No hardcoded team or player data anywhere in C source. Everything loaded from CSV files. |
| **Separation of concerns** | Country identity (who a team is) is kept strictly separate from tournament performance (how they did). |
| **Reusability** | Tournament format driven by a `.cfg` config file. Swap config and data files to run any tournament. |
| **Single responsibility** | Each `.c` module has one clearly defined job. No module reaches into another's responsibility. |
| **Extensibility** | Architecture supports growth from a WC simulator to a full football engine without restructuring. |
| **Portability** | Cross-platform from day one — Linux, macOS, Windows handled via `platform.h`. |

---

## 3. Folder Structure

```
world_cup_simulator/
│
├── data/
│   ├── countries.csv            # Static — country identity
│   ├── players.csv              # Static — outfield player identity and base stats
│   ├── goalkeepers.csv          # Static — goalkeeper identity and base stats
│   ├── wc_2026_groups.csv       # Config — predetermined 2026 group draw
│   ├── tournament_state.csv     # Dynamic — active tournament state
│   ├── tournament_teams.csv     # Dynamic — team performance this tournament
│   ├── player_stats.csv         # Dynamic — outfield player stats this tournament
│   ├── gk_stats.csv             # Dynamic — goalkeeper stats this tournament
│   └── matches.csv              # Dynamic — all match results
│
├── tournaments/
│   └── wc_2026.cfg              # Tournament format config
│
├── include/
│   ├── structs.h                # All struct definitions
│   ├── enums.h                  # All enum definitions
│   ├── config.h                 # Compile-time constants
│   └── platform.h               # Cross-platform macros (sleep_ms)
│
├── src/
│   ├── main.c                   # Entry point, menu loop, startup flow
│   ├── file_io.c                # All CSV read/write operations
│   ├── tournament.c             # Group stage, standings, knockout logic
│   ├── simulation.c             # Match engine, goal events, tick playback
│   ├── crud.c                   # CRUD operations
│   ├── display.c                # All terminal output
│   ├── awards.c                 # End-of-tournament award calculations
│   └── validation.c             # Input validation and stat range checks
│
├── build/                       # CMake build output (gitignored)
├── tests/                       # Test files
├── docs/                        # Report and proposal
├── CMakeLists.txt
├── .clang-format
├── .gitignore
├── LICENSE
└── README.md
```

---

## 4. Data Layer

### Static vs Dynamic files

**Static files** contain identity data. They are pre-populated before the tournament
starts and are never overwritten during a tournament run.

**Dynamic files** contain tournament-specific performance data. They are created fresh
when a new tournament begins. When the user confirms a new tournament, all dynamic files
are wiped and rewritten from scratch.

A `tournament_id` field in all dynamic files links every record to a specific tournament
run, making the schema ready for multi-tournament history if extended in future.

### CSV Schemas

#### `countries.csv` — Static
```
id, name, confederation, fifa_ranking
```

#### `players.csv` — Static
```
id, name, country_id, age, position,
speed, passing, shooting, defending, physical, dribbling
```

#### `goalkeepers.csv` — Static
```
id, name, country_id, age, position,
diving, handling, kicking, reflexes, speed, positioning
```

#### `wc_2026_groups.csv` — Config
```
country_id, group_name
```

#### `tournament_state.csv` — Dynamic
```
tournament_id, tournament_name, current_stage,
groups_completed, knockouts_completed, tournament_winner
```

#### `tournament_teams.csv` — Dynamic
```
tournament_id, country_id, group_name,
matches_played, wins, draws, losses,
goals_for, goals_against, goal_difference, points, stage_reached
```

#### `player_stats.csv` — Dynamic
```
tournament_id, player_id, goals, assists, matches_played, is_injured
```

#### `gk_stats.csv` — Dynamic
```
tournament_id, gk_id, clean_sheets, goals_conceded, matches_played, is_injured
```

#### `matches.csv` — Dynamic
```
id, tournament_id, team1_id, team2_id,
team1_score, team2_score, stage, group_name,
is_simulated, match_date
```

### Tournament config file — `tournaments/wc_2026.cfg`

```
tournament_name     = FIFA World Cup 2026
num_teams           = 48
num_groups          = 12
teams_per_group     = 4
groups_advance      = 2
best_third_advance  = 8
format              = GROUP_THEN_KNOCKOUT
host                = USA, Canada, Mexico
random_draw         = 0
```

---

## 5. Structs

All structs defined in `include/structs.h`.

```c
typedef struct {
    int  id;
    char name[64];
    Confederation confederation;
    int  fifa_ranking;
} Country;

typedef struct {
    int      id;
    char     name[64];
    int      country_id;
    int      age;
    Position position;
    int      speed, passing, shooting, defending, physical, dribbling;
} Player;

typedef struct {
    int      id;
    char     name[64];
    int      country_id;
    int      age;
    Position position;
    int      diving, handling, kicking, reflexes, speed, positioning;
} Goalkeeper;

typedef struct {
    int   tournament_id;
    int   player_id;
    int   goals, assists, matches_played;
    int   is_injured;
} PlayerStats;

typedef struct {
    int   tournament_id;
    int   gk_id;
    int   clean_sheets, goals_conceded, matches_played;
    int   is_injured;
} GKStats;

typedef struct {
    int   tournament_id;
    int   country_id;
    char  group_name[4];
    int   matches_played, wins, draws, losses;
    int   goals_for, goals_against, goal_difference, points;
    Stage stage_reached;
} TournamentTeam;

typedef struct {
    int   id;
    int   tournament_id;
    int   team1_id, team2_id;
    int   team1_score, team2_score;
    Stage stage;
    char  group_name[4];
    int   is_simulated;
    char  match_date[12];
} Match;

typedef struct {
    int   tournament_id;
    char  tournament_name[128];
    Stage current_stage;
    int   groups_completed;
    int   knockouts_completed;
    int   tournament_winner;
} TournamentState;

typedef struct {
    char name[128];
    int  num_teams, num_groups, teams_per_group;
    int  groups_advance, best_third_advance;
    char host[128];
    int  random_draw;
} TournamentConfig;

typedef struct {
    int minute;
    int player_id;
    int team_id;
    int is_own_goal;
} GoalEvent;
```

---

## 6. Enums and Constants

### `include/enums.h`

```c
typedef enum {
    UEFA, CONMEBOL, CONCACAF, AFC, CAF, OFC
} Confederation;

typedef enum {
    GROUP_STAGE,
    ROUND_OF_32,
    ROUND_OF_16,
    QUARTER_FINAL,
    SEMI_FINAL,
    FINAL
} Stage;

typedef enum {
    GK, DEF, MID, FWD
} Position;
```

### `include/config.h`

```c
#define STAT_MIN            55
#define STAT_MAX            99
#define MAX_PLAYERS       1300
#define MAX_GKS            200
#define MAX_TEAMS           48
#define MAX_GROUPS          12
#define SQUAD_SIZE          26
#define GKS_PER_TEAM         3
#define OUTFIELD_PER_TEAM   23
#define MAX_MATCHES        200
#define GOAL_EVENT_DELAY   800    /* milliseconds between goal events */
#define GROUP_NAME_LEN       4
#define NAME_LEN            64
```

### `include/platform.h`

```c
#ifdef _WIN32
    #include <windows.h>
    #define sleep_ms(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
#endif
```

---

## 7. Module Breakdown

| Module | File | Responsibility | Key Functions |
|---|---|---|---|
| Entry point | `src/main.c` | Startup logic, main menu loop, program flow | `main()`, `handleStartup()`, `showMainMenu()` |
| File I/O | `src/file_io.c` | All CSV read/write. Load all files into memory on startup. Write back after every state change. | `loadCountries()`, `loadPlayers()`, `loadGoalkeepers()`, `loadTournamentState()`, `saveMatch()`, `saveTournamentState()`, `savePlayerStats()` |
| Tournament | `src/tournament.c` | Group stage, standings with tiebreakers, best third-place selection, knockout bracket, stage progression | `initializeGroups()`, `simulateGroupStage()`, `calculateStandings()`, `selectBestThird()`, `generateKnockoutBracket()`, `advanceKnockout()` |
| Simulation | `src/simulation.c` | Starting XI selection, team strength calculation, scoreline generation, goal event creation, tick playback | `pickStartingXI()`, `calculateTeamStrength()`, `simulateMatch()`, `generateGoalEvents()`, `playbackEvents()` |
| CRUD | `src/crud.c` | All four CRUD operations. Calls `file_io.c` to persist changes. | `addCustomTeam()`, `viewPlayersByTeam()`, `updatePlayerStat()`, `injurePlayer()` |
| Display | `src/display.c` | All terminal output. No logic — pure presentation. | `printGroupStandings()`, `printBracket()`, `printPlayerProfile()`, `printMatchResult()`, `printMenu()` |
| Awards | `src/awards.c` | End-of-tournament award calculations. Runs only after Final. | `getGoldenBoot()`, `getGoldenBall()`, `getGoldenGlove()`, `getBestYoungPlayer()`, `printAwardsCeremony()` |
| Validation | `src/validation.c` | Stat range enforcement, input sanitization, enum conversion, destructive action confirm prompts | `validateStat()`, `sanitizeInput()`, `confirmAction()`, `stageToStr()`, `confedToStr()` |

---

## 8. Menu System

### Startup

```
Launch binary
  → Load tournament_state.csv
  → Tournament found?
      YES → "Resume existing tournament? [y/n]"
              y → load all CSVs → main menu
              n → "All progress will be lost. Continue? [y/n]" → new setup
      NO  → New tournament setup
```

### New tournament setup

```
1. Load 2026 WC  (predetermined groups from wc_2026_groups.csv)
2. Random draw   (shuffle all 48 teams into groups randomly)
0. Back
```

### Main menu

```
1. Tournament
2. Teams and players
3. Statistics
4. Save and exit
```

### Tournament submenu

```
1. Group stage
   1. Simulate next match
   2. Simulate next 5 matches
   3. Simulate full matchday
   4. Enter score manually
   5. View group standings  (select group A–L)
   6. View match results
   0. Back
2. Knockout stage           (unlocks after group stage completes)
   1. Simulate next match
   2. Simulate next 5 matches
   3. Enter score manually  (includes extra time / penalties option)
   4. View bracket
   5. Tournament awards     (unlocks after Final is played)
   0. Back
3. View full bracket
0. Back to main menu
```

### Teams and players submenu (CRUD)

```
1. View all teams
2. View team squad
3. View player profile
4. Update player stat        (pre-tournament only)
5. Injure a player           (confirm prompt — cannot be undone)
6. Add custom wildcard team
0. Back to main menu
```

### Statistics submenu

```
1. Top scorers
2. Top assists
3. Clean sheets leaderboard
4. Team performance table
5. Awards preview            (current leaders before tournament ends)
0. Back to main menu
```

---

## 9. Simulation Engine

### Match simulation flow

```
1. pickStartingXI()
   └─ Select best 11 non-injured players per team
      GK: highest overall GK rating
      Outfield: weighted by position (1 GK, 4 DEF, 3 MID, 3 FWD)

2. calculateTeamStrength()
   └─ attack_strength  = weighted_avg(FWD.shooting, MID.passing)
      defense_strength = weighted_avg(DEF.defending, GK.reflexes)
      team_strength    = (attack × 0.6) + (defense × 0.4)

3. simulateMatch()
   └─ win_probability = team1_strength / (team1_strength + team2_strength)
      outcome         = win_probability + random_float(−0.15, +0.15)
      outcome > 0.55  → team1 wins
      outcome < 0.45  → team2 wins
      else            → draw
      generate scoreline consistent with result

4. generateGoalEvents()
   └─ for each goal:
        minute  = rand(1, 90)
        team    = weighted by scoreline
        scorer  = weighted by player.shooting / team_total_shooting
      sort events[] by minute ASC

5. playbackEvents()
   └─ for each event:
        print "  {minute}' — GOAL! {player} ({team})  {score}"
        sleep_ms(GOAL_EVENT_DELAY)

6. Update state
   └─ Write to matches.csv
      Update player_stats.csv  (goals, assists, matches_played)
      Update gk_stats.csv      (clean_sheets, goals_conceded)
      Update tournament_teams.csv (points, GD, wins/draws/losses)
```

---

## 10. Tournament Format — 2026 Rules

| Parameter | Value |
|---|---|
| Total teams | 48 |
| Groups | 12 (A through L) |
| Teams per group | 4 |
| Auto-qualify per group | Top 2 |
| Additional qualifiers | Best 8 third-place teams |
| Round of 32 teams | 32 (24 + 8 best thirds) |
| Knockout rounds | R32 → R16 → QF → SF → Final |
| Total group stage matches | 48 |
| Total knockout matches | 32 |
| Grand total | 104 matches |

### Standings tiebreaker order

1. Points
2. Goal difference
3. Goals for
4. Head-to-head points
5. Head-to-head goal difference
6. FIFA ranking (lower number = higher rank)

### Best third-place selection

After all group matches, collect one third-place team per group (12 total).
Rank using the same tiebreaker order above.
Top 8 advance to Round of 32.

---

## 11. CRUD Operations

| Operation | Feature | Details |
|---|---|---|
| **Create** | Run a match simulation | Simulates a match between two teams, generates goal events, and writes a new match record to matches.csv along with updated player and team stats. |
| **Read** | View all teams | All 48 countries with group, confederation, current standings. |
| **Read** | View team squad | Full 26-player roster with stats and injury status for a selected country. |
| **Read** | View player profile | Individual player — all base stats, tournament goals/assists, matches played, injury status. |
| **Update** | Edit player stat | Pre-tournament only. Validated against `STAT_MIN`/`STAT_MAX`. Persisted immediately to CSV. |
| **Delete** | Injure player | Sets `is_injured = 1` in `player_stats.csv` or `gk_stats.csv`. Excluded from all future match selection. Requires confirm prompt. Cannot be undone. |

---

## 12. Awards System

All awards calculated in `awards.c` from in-memory data after the Final.
No additional file reads required at calculation time.

| Award | Eligibility | Metric |
|---|---|---|
| **Golden Boot** | All outfield players | Most goals — sort `player_stats` by `goals` DESC |
| **Golden Ball** | All outfield players | Most goals + assists — sort by `(goals + assists)` DESC |
| **Golden Glove** | GKs whose team reached QF or further | Least goals conceded — filter by `stage_reached >= QUARTER_FINAL`, sort `goals_conceded` ASC |
| **Best Young Player** | Players aged 21 or under (`age <= 21`) | Most goals + assists — same G+A metric, filtered by age |

Awards are printed sequentially with `sleep_ms` between each for dramatic effect.

---

## 13. Cross-Platform Support

| Platform | Member | Compiler | Setup |
|---|---|---|---|
| Linux | Member A | GCC (native) | Primary development environment |
| macOS | Member C | GCC via Xcode CLI | `xcode-select --install` |
| Windows | Member B | GCC via WSL2 | Recommended: WSL2 with Ubuntu |

The only meaningful platform difference is the sleep function.
This is abstracted in `include/platform.h` using the `sleep_ms(ms)` macro so all
source files call `sleep_ms()` without any platform conditionals in business logic.

---

## 14. Build System

CMake 3.20+ with C17 standard.

```bash
# Configure
cmake -S . -B build

# Build
cmake --build build

# Run
./build/wc_simulator
```

Source files are picked up from `src/`. Headers are in `include/`.
Build artifacts go into `build/` which is gitignored.

---

## 15. Git Workflow

### Branch structure

```
main          — stable, always compiles and runs cleanly
dev           — integration branch, merge here first and test
feature/file-io
feature/simulation
feature/tournament
feature/display
feature/crud
```

### Rules

- Never push broken code to `main`. Teammates depend on it compiling.
- All feature branches merge into `dev` first.
- Only push `dev` → `main` when a full feature is tested and stable.
- Commit messages should say what changed, not just "update" or "fix".

---

## 16. Scope and Extensibility

The architecture is intentionally designed beyond the immediate project scope.

| Level | Description | What needs to change |
|---|---|---|
| **1 — Current** | World Cup 2026, terminal, C | Nothing — this is what is being built |
| **2 — Post-submission** | Any national tournament (Euros, Copa, AFCON, Nations League) | New `.cfg` file + country subset + groups CSV |
| **3 — Extended** | Club tournaments (Champions League, etc.) | Add `clubs.csv`, `player_club_id` field to players |
| **4 — Full engine** | Seasons, transfers, form over time | Persistent season state, form weighting in simulation |
| **5 — Multi-sport** | Rugby World Cup, Cricket, Olympics | Match simulation formula configurable per sport |

Level 2 is supported right now with zero code changes.
Every level above that builds on the existing foundation without requiring restructuring.

---

*World Cup Simulator — Architecture Document — Last updated June 2026*
