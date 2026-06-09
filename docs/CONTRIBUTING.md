# Contributing Guide

This document explains the development workflow for the FIFA World Cup Simulator project.

---

## Branch Workflow

### Your branch is your workspace

Each team member has a personal branch:

```
dev/nitesh   → Nitesh
dev/sushant  → Sushant
dev/amish    → Amish
```

Never work directly on `main`. Always work on your own branch.

### Day-to-day workflow

```bash
# Make sure your branch is up to date before starting work
git checkout dev/yourname
git pull origin dev/yourname

# Do your work, then stage and commit
git add .
git commit -m "feat: add group stage draw logic"

# Push your branch
git push origin dev/yourname
```

### Merging into main

1. Push your branch to GitHub
2. Open a **Pull Request** from `dev/yourname` → `main`
3. Write a short description of what you did
4. Nitesh reviews and merges

**Only Nitesh merges into main.**

---

## Commit Message Format

Use this format for all commits:

```
<type>: <short description>
```

### Types

| Type     | When to use                            |
|----------|----------------------------------------|
| `feat`   | Adding a new feature                   |
| `fix`    | Fixing a bug                           |
| `refactor` | Restructuring code without new features |
| `docs`   | Documentation only                     |
| `style`  | Formatting, whitespace (no logic change) |
| `test`   | Adding or updating tests               |
| `chore`  | Build system, CMake, config changes    |

### Examples

```
feat: add group stage simulation loop
fix: correct points calculation on draw
docs: update architecture diagram
chore: add -Wconversion compiler flag
```

---

## Code Style

We use `clang-format` to enforce consistent style. **Always format before committing.**

```bash
# Format a single file
clang-format -i src/yourfile.c

# Format all source files at once
find src include -name "*.c" -o -name "*.h" | xargs clang-format -i
```

Key rules (defined in `.clang-format`):
- 4-space indentation, no tabs
- Allman brace style (opening brace on new line)
- Max 100 characters per line
- Pointer on the right: `int *ptr` not `int* ptr`

---

## Building Before You Commit

Always make sure the project builds cleanly before pushing:

```bash
cmake -S . -B build
cmake --build build
```

If it does not build, fix it before pushing. **Do not push broken code.**

---

## Questions?

Ask in the group chat or ping Nitesh.
