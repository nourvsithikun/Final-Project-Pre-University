# Typing Space Shooter — Angkor Edition

A real-time 2D typing arcade game built for a university C++ final project. Enemy ships descend over an Angkor Wat night scene, and the player destroys them by typing their English words before they cross the danger line.

The game includes a background-only Angkor night scene, readable bundled fonts, and procedural raylib ships/UI. Optional transparent PNG and audio assets can replace individual layers without changing gameplay code.

## Features

- Six screens: main menu, playing, paused, how to play, leaderboard, and game over
- Unique-ID target lock; duplicate starting letters select the lowest active enemy
- Per-character feedback, short blue lasers, explosions, hit sparks, and engine glow
- Score, WPM, accuracy, combo, highest combo, level, health, character counts, destroyed words, and active time
- Three word tiers and gradual speed/spawn scaling with a 0.7-second minimum interval
- Top-10 JSON leaderboard with safe handling of missing, empty, invalid, or corrupted data
- 1600 x 900 virtual canvas with aspect-ratio-preserving letterboxing
- Automatically maximized resizable window to avoid Windows display-scaling crop
- Independent background, UI, ships, words, values, particles, and audio layers
- Runtime fallbacks and clear warnings for every optional missing asset

## Technology

- C++17
- raylib 5.5
- CMake FetchContent
- nlohmann/json 3.11.3
- `std::vector`, `std::chrono`-compatible active timing, file streams, filesystem, and standard random utilities

## Build

Requirements: a C++17 compiler, CMake 3.20+, Git, and an internet connection for the first FetchContent configuration.

### Windows (Visual Studio or VS Code)

Install Visual Studio Build Tools with **Desktop development with C++**, CMake, and Git. In a Developer PowerShell:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\TypingSpaceShooter.exe
```

With a single-config generator such as MinGW or Ninja, the executable is normally `build/TypingSpaceShooter.exe`.

### macOS (VS Code or Terminal)

Install the command-line tools, CMake, and Git:

```bash
xcode-select --install
brew install cmake git
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/TypingSpaceShooter
```

The build copies `assets/` beside the executable. The game also supports being launched from the repository root.

### Headless core verification

On CI or a container without desktop graphics development headers, the data rules can still be configured and tested:

```bash
cmake -S . -B build-headless -DTYPING_HEADLESS_TESTS=ON
cmake --build build-headless
ctest --test-dir build-headless --output-on-failure
```

This option intentionally builds only non-graphical tests. Leave it off for the real game.

## Controls

| Context | Control | Action |
|---|---|---|
| Menus | Up / Down | Change selection |
| Menus | Enter | Confirm |
| Gameplay | A–Z | Lock a word and type it |
| Gameplay | Escape | Pause |
| Pause | Escape | Resume |
| Sub-screens | Escape | Return |
| Game over | Letters, numbers, space | Enter guardian name |
| Game over | Backspace | Delete a name character |

## Gameplay

At the start of a word, typing a visible first letter locks the matching enemy closest to the bottom. The same keystroke counts as the word's first correct character. Once locked, input is checked only against that enemy until the word is completed or the enemy is missed. Correct letters turn green and fire a laser. Wrong letters count as mistakes and reset the combo without discarding the target.

The player has three health points. A ship that reaches the red danger line costs one health point and 20 score. The game ends at zero health.

### Scoring

- Completed word: `word length × 10`
- Combo bonus: `combo × 2`
- Wrong letter: `−2`
- Missed enemy: `−20`
- Score is clamped to zero

### Difficulty

- Level = `words destroyed / 10 + 1`
- Levels 1–2: easy words
- Levels 3–4: medium words
- Level 5+: hard words
- Ship speed rises each level; spawn delay falls to a minimum near 0.7 seconds
- Active ships are capped between 6 and 10 according to level

### Statistics formulas

```text
WPM = (correctCharacters / 5.0) / activeGameplayMinutes
Accuracy = correctCharacters / totalCharactersTyped * 100
```

Accuracy is 100% before the first keystroke to avoid division by zero. Only the `Playing` state adds active seconds, so paused time does not reduce WPM.

## Leaderboard format

`assets/data/leaderboard.json` stores:

```json
{
  "entries": [
    {
      "playerName": "PLAYER",
      "score": 1250,
      "wpm": 42.5,
      "accuracy": 94.2,
      "wordsDestroyed": 18
    }
  ]
}
```

Entries are validated, sorted by score (then WPM for ties), and trimmed to ten. Invalid rows are skipped. A malformed file produces an empty in-memory board instead of a crash.

## Project structure

```text
TypingSpaceShooter/
├── CMakeLists.txt
├── AGENTS.md
├── README.md
├── PROJECT_STATUS.md
├── PRESENTATION_SUMMARY.md
├── assets/
│   ├── backgrounds/  ships/  ui/  fonts/  sounds/
│   └── data/
├── include/
├── src/
└── tests/CoreTests.cpp
```

`Game` owns the steady-clock loop, state transitions, virtual render target, spawning, and system coordination. Entity classes own their state and drawing. `WordManager`, `ScoreManager`, and `Leaderboard` contain the independent data rules. `AssetManager` loads each resource once, returns optional layers, and unloads safely.

## Team responsibilities

| Team member | Responsibility |
|---|---|
| Member 1: __________ | Game loop and state management |
| Member 2: __________ | Enemy spawning, typing, and difficulty |
| Member 3: __________ | UI, Angkor artwork, and audio assets |
| Member 4: __________ | Statistics, leaderboard, testing, and presentation |

Adjust the placeholders to match the actual team. Every member should be able to explain the main loop and their assigned class.

## Adding final assets

Place files at the exact paths documented inside each asset folder. The background must contain only the environment. Do not bake ships, words, numbers, HUD values, lasers, or statistics into it. Transparent ship and UI PNGs are scaled by the game; source artwork should keep generous transparent padding to a minimum.

## Asset credits

- Procedural fallback art and interface: project source code
- Angkor background: generated specifically for this project as an environment-only layer
- UI/body font: DejaVu Sans (Bitstream Vera license; see `assets/fonts/DEJAVU_LICENSE.txt`)
- Display font: DejaVu Serif Bold (Bitstream Vera license; see `assets/fonts/DEJAVU_LICENSE.txt`)
- Ship sprites: ____________________
- Khmer ornament and logo: ____________________
- Sound effects and music: ____________________

Only use assets with permission and record the author, source URL, and license before submission.

## Known limitations

- Ships, hearts, and ornamental frames use polished procedural fallbacks until optional final transparent PNGs are supplied; audio remains silent until sound files are added.
- Keyboard text handling intentionally accepts English A–Z during gameplay.
- Mouse menu navigation is not included; keyboard navigation is complete.
- Very large custom fonts or frame textures may need art-specific scaling adjustments.
