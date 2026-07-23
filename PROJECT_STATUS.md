# Project status

Last updated: 2026-07-22

## Milestones

| Milestone | Status | Result |
|---|---|---|
| 1. Foundation | Complete in source | Structure, FetchContent CMake, window, 1600 x 900 render texture, procedural UI and player |
| 2. Enemies and words | Complete in source | Three word files, fallback lists, spawning, speed, active cap, overlap avoidance |
| 3. Typing | Complete in source | Lowest duplicate-letter target, progress colors, wrong input, completion |
| 4. Game rules | Complete in source | Health, score, combo, accuracy, WPM, levels, missed ships, game over |
| 5. Screens | Complete in source | Menu, pause, instructions, leaderboard, name input, game over |
| 6. Persistence | Complete in source | Validated top-10 JSON load/add/sort/save and reset behavior |
| 7. Effects/audio | Complete in source | Layered lasers, sparks, explosions, engine pulse, optional sounds/music |
| 8. Angkor styling | Complete in source | Environment-only Angkor artwork, readable bundled fonts, layered gold/brown panels, compact detailed fallback ships, asset hooks |

## Automated coverage included

- Scoring, combo, accuracy baseline, WPM, level, and non-negative penalties
- Duplicate starting-letter selection by lowest Y position
- Missing word files and fallback lists
- Word lowercase normalization and blank-line handling
- Corrupted JSON recovery, add/save/reload, sorting, and top-10 trimming

## Verification status

- Source and required path inspection: complete
- Default FetchContent configuration: dependency downloads succeeded; this Linux runtime then stopped at raylib's missing X11 development libraries
- Full GCC syntax/type pass against the actual fetched raylib 5.5 and nlohmann/json 3.11.3 headers: passed with `-Wall -Wextra -Wpedantic`
- Clean headless CMake configure/build/CTest after removing the previous build directory: passed, 1/1 tests
- Clean graphical Windows/macOS build: must be run on a machine with the README prerequisites
- Graphical/manual checks: pending on a desktop environment

Do not mark the last two checks complete until the commands in `README.md` pass on the presentation machine.

## Final art checklist

- Background-only `angkor_night.png` added and kept separate from dynamic gameplay layers
- Readable DejaVu gameplay/display fonts added with license
- Add transparent compact ship sprites
- Add optional Khmer frame/logo/heart art
- Add six sound effects/music files and credits
- Manually verify the upgraded UI at the presentation computer's Windows/macOS display scaling
