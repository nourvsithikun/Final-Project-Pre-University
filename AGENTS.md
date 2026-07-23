# Typing Space Shooter — Contributor Guide

## Commands

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

On a headless machine without desktop graphics headers, verify core logic with:

```bash
cmake -S . -B build-headless -DTYPING_HEADLESS_TESTS=ON
cmake --build build-headless
ctest --test-dir build-headless --output-on-failure
```

Run `./build/TypingSpaceShooter` on macOS/Linux or `build\\Release\\TypingSpaceShooter.exe` on Windows. Assets are copied beside the executable after every build.

## Architecture rules

- Use C++17, raylib, one non-blocking loop, and delta time for movement.
- Keep update/state logic separate from drawing and never load resources per frame.
- Target enemies by unique ID, not vector index; erase only after iteration.
- Pause gameplay, spawn, input, and active-time counters together.
- Keep dynamic text and statistics out of textures; missing assets must use fallbacks.
- Keep changes compact and preserve the 1600 x 900 virtual canvas.

## Verification

- Build after material changes and run `ctest`.
- Exercise missing word files and malformed leaderboard JSON.
- Check duplicate first-letter targeting chooses the lowest enemy.
- Manually verify menu, pause/resume, restart, game over, and aspect-ratio scaling.
