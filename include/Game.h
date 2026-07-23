#pragma once

#include "AssetManager.h"
#include "Enemy.h"
#include "GameScreen.h"
#include "Laser.h"
#include "Leaderboard.h"
#include "Particle.h"
#include "Player.h"
#include "ScoreManager.h"
#include "WordManager.h"

#include "raylib.h"

#include <chrono>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    static constexpr int VirtualWidth = 1600;
    static constexpr int VirtualHeight = 900;

    void Initialize();
    void Shutdown();
    void Update(float deltaTime);
    void Draw();
    void DrawToWindow() const;

    void UpdateMainMenu();
    void UpdatePlaying(float deltaTime);
    void UpdatePaused();
    void UpdateHowToPlay();
    void UpdateLeaderboard();
    void UpdateGameOver();

    void StartNewGame();
    void SpawnEnemy();
    void ProcessTypedCharacter(char typed);
    Enemy* FindTarget();
    const Enemy* FindTarget() const;
    void ClearTarget();
    void DestroyEnemy(Enemy& enemy);
    void MissEnemy(Enemy& enemy);
    void SpawnExplosion(Vector2 position, Color color);
    void SpawnHitSparks(Vector2 position);
    std::string FindAssetRoot() const;

    void DrawBackground() const;
    void DrawAngkorSilhouette() const;
    void DrawOrnamentalFrame() const;
    void DrawPlaying() const;
    void DrawMainMenu() const;
    void DrawPaused() const;
    void DrawHowToPlay() const;
    void DrawLeaderboard() const;
    void DrawGameOver() const;
    void DrawTopHud() const;
    void DrawLeftPanel() const;
    void DrawRightPanel() const;
    void DrawTypingArea() const;
    void DrawPanel(Rectangle bounds, Color border, float thickness = 2.0F) const;
    void DrawCenteredText(const std::string& text, float y, float fontSize, Color color) const;
    void DrawMenuItems(const std::vector<std::string>& items, int selected, float startY) const;
    static std::string FormatTime(float seconds);

    bool initialized_ = false;
    RenderTexture2D renderTarget_{};
    GameScreen screen_ = GameScreen::MainMenu;
    AssetManager assets_;
    WordManager wordManager_;
    ScoreManager scoreManager_;
    Leaderboard leaderboard_;
    Player player_;
    std::vector<Enemy> enemies_;
    std::vector<Laser> lasers_;
    std::vector<Particle> particles_;
    std::mt19937 random_;
    std::uint64_t nextEnemyId_ = 1;
    std::uint64_t targetEnemyId_ = 0;
    float spawnTimer_ = 0.0F;
    float nextSpawnDelay_ = 1.0F;
    int mainMenuSelection_ = 0;
    int pauseMenuSelection_ = 0;
    int gameOverSelection_ = 0;
    std::string playerName_ = "PLAYER";
    bool scoreSaved_ = false;
    bool exitRequested_ = false;
    std::chrono::steady_clock::time_point previousFrameTime_{};
};
