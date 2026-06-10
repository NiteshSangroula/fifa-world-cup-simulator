# ⚽ FIFA World Cup Simulator

> A terminal-based FIFA World Cup tournament simulator written in C.
> Built as a first-semester college project by a team of three.

---

## 👥 Team

| Name    | Branch         | Role              |
|---------|----------------|-------------------|
| Nitesh  | `dev/nitesh`   | Lead / Repo Owner |
| Sushant | `dev/sushant`  | Developer         |
| Amish   | `dev/amish`    | Developer         |

---

## 📋 Features

- [ ] Group stage simulation (12 groups × 4 teams)
- [ ] Round of 32
- [ ] Round of 16
- [ ] Quarter-finals, Semi-finals, Final
- [ ] Match outcome engine
- [ ] Results display

> Features will be checked off as they are completed.

---

## 🏗️ Project Structure

```
fifa-world-cup-simulator/
├── CMakeLists.txt        # Build system
├── .clang-format         # Code style
├── src/                  # Source files (.c)
├── include/              # Header files (.h)
├── tests/                # Unit tests
├── data/                 # Teams, groups, fixtures
└── docs/                 # Project documentation
    ├── ARCHITECTURE.md
    ├── CONTRIBUTING.md
    └── CHANGELOG.md
```

---

## 🚀 Building the Project

### Prerequisites

- GCC or Clang (C17 support)
- CMake ≥ 3.20
- Make or Ninja

### Build steps

```bash
# Clone the repo
git clone https://github.com/YOUR_USERNAME/fifa-world-cup-simulator.git
cd fifa-world-cup-simulator

# Configure and build
cmake -S . -B build
cmake --build build

# Run
./build/bin/fifa_world_cup_simulator
```

### Build in Release mode

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## 🌿 Branching Strategy

```
main              ← stable, always builds — protected
├── dev/nitesh
├── dev/sushant
└── dev/amish
```

- **Nobody pushes directly to `main`**
- Work on your `dev/<name>` branch
- Open a Pull Request → Nitesh reviews & merges
- See [CONTRIBUTING.md](docs/CONTRIBUTING.md) for full workflow

---

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.
