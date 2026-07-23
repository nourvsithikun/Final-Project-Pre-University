# Short team presentation summary

## 60-second project introduction

“Our project is **Typing Space Shooter — Angkor Edition**, a real-time 2D arcade game made in C++17 with raylib. Enemy spaceships carry English words and fall toward Angkor Wat. The player types a first letter to lock the closest matching enemy, then completes its word to fire lasers and destroy it. The game measures score, WPM, accuracy, combo, health, level, and active play time. It has difficulty scaling, pause-safe timing, and a persistent JSON top-10 leaderboard.”

## Technical points to demonstrate

1. Show the menu and 1600 x 900 interface scaling in a resized window.
2. Spawn two words with the same first letter and explain why the lower ship is selected.
3. Type correct and wrong letters to show lasers, progress color, score, accuracy, and combo.
4. Pause for several seconds and show that gameplay and active time remain frozen.
5. Reach game over, enter a name, save, and reopen the leaderboard.
6. Temporarily rename an optional asset or word file to demonstrate safe fallbacks.

## Architecture explanation

- `Game` controls the loop and connects screens and systems.
- `Enemy`, `Player`, `Laser`, and `Particle` hold dynamic entity state.
- `WordManager` loads and chooses normalized words.
- `ScoreManager` owns all formulas and active time.
- `Leaderboard` validates and persists JSON.
- `AssetManager` loads each optional resource once and supplies fallbacks.

## Closing sentence

“The Angkor theme is separated into independent layers, so every ship, word, laser, statistic, and effect can change in real time while the project remains playable even before final artwork is installed.”

