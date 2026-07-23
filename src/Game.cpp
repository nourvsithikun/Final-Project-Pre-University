#include "Game.h"

#include "Targeting.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
constexpr Color Midnight{4, 12, 32, 255};
constexpr Color PanelFill{7, 12, 22, 238};
constexpr Color PanelBrown{28, 19, 13, 242};
constexpr Color Gold{191, 123, 36, 255};
constexpr Color BrightGold{255, 205, 85, 255};
constexpr Color WarmWhite{245, 239, 222, 255};
constexpr Color SoftBlue{75, 202, 255, 255};
constexpr Color SuccessGreen{106, 235, 83, 255};
constexpr Color DangerRed{245, 68, 73, 255};
constexpr Rectangle PlayArea{250.0F, 112.0F, 1100.0F, 660.0F};
constexpr float DangerLineY = 744.0F;

void DrawTextShadow(Font font, const char* text, Vector2 position, float fontSize,
                    float spacing, Color color, float shadowOffset = 2.0F) {
    DrawTextEx(font, text, {position.x + shadowOffset, position.y + shadowOffset},
               fontSize, spacing, Fade(BLACK, 0.82F));
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

void DrawLotusMark(Vector2 center, float scale, Color color) {
    DrawTriangle({center.x, center.y - 8.0F * scale},
                 {center.x - 5.0F * scale, center.y + 2.0F * scale},
                 {center.x + 5.0F * scale, center.y + 2.0F * scale}, color);
    DrawTriangle({center.x - 4.0F * scale, center.y - 2.0F * scale},
                 {center.x - 11.0F * scale, center.y + 5.0F * scale},
                 {center.x, center.y + 4.0F * scale}, Fade(color, 0.82F));
    DrawTriangle({center.x + 4.0F * scale, center.y - 2.0F * scale},
                 {center.x, center.y + 4.0F * scale},
                 {center.x + 11.0F * scale, center.y + 5.0F * scale}, Fade(color, 0.82F));
    DrawLineEx({center.x - 11.0F * scale, center.y + 7.0F * scale},
               {center.x + 11.0F * scale, center.y + 7.0F * scale},
               std::max(1.0F, scale), Fade(color, 0.7F));
}

bool IsConfirmPressed() {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
}

int WrapSelection(int selection, int count) {
    if (selection < 0) return count - 1;
    if (selection >= count) return 0;
    return selection;
}
}

Game::Game()
    : leaderboard_(), player_({800.0F, 718.0F}), random_(std::random_device{}()) {}

Game::~Game() {
    Shutdown();
}

void Game::Run() {
    Initialize();
    while (!WindowShouldClose() && !exitRequested_) {
        const auto currentFrameTime = std::chrono::steady_clock::now();
        const float deltaTime = std::min(
            std::chrono::duration<float>(currentFrameTime - previousFrameTime_).count(),
            0.05F
        );
        previousFrameTime_ = currentFrameTime;
        Update(deltaTime);

        BeginTextureMode(renderTarget_);
        Draw();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawToWindow();
        EndDrawing();
    }
}

void Game::Initialize() {
    if (initialized_) {
        return;
    }
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Typing Space Shooter — Angkor Edition");
    SetWindowMinSize(800, 450);
    MaximizeWindow();
    SetTargetFPS(60);
    InitAudioDevice();

    renderTarget_ = LoadRenderTexture(VirtualWidth, VirtualHeight);
    if (renderTarget_.id == 0) {
        CloseWindow();
        throw std::runtime_error("Could not create the 1600 x 900 render texture.");
    }
    SetTextureFilter(renderTarget_.texture, TEXTURE_FILTER_BILINEAR);

    const std::string assetRoot = FindAssetRoot();
    assets_.LoadAll(assetRoot);
    wordManager_.LoadAll(assetRoot + "/data");
    leaderboard_.SetFilePath(assetRoot + "/data/leaderboard.json");
    leaderboard_.Load();
    assets_.StartMusic();
    previousFrameTime_ = std::chrono::steady_clock::now();
    initialized_ = true;
}

void Game::Shutdown() {
    if (!initialized_) {
        return;
    }
    assets_.UnloadAll();
    UnloadRenderTexture(renderTarget_);
    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }
    CloseWindow();
    initialized_ = false;
}

void Game::Update(float deltaTime) {
    assets_.UpdateMusic();
    switch (screen_) {
        case GameScreen::MainMenu: UpdateMainMenu(); break;
        case GameScreen::Playing: UpdatePlaying(deltaTime); break;
        case GameScreen::Paused: UpdatePaused(); break;
        case GameScreen::HowToPlay: UpdateHowToPlay(); break;
        case GameScreen::Leaderboard: UpdateLeaderboard(); break;
        case GameScreen::GameOver: UpdateGameOver(); break;
    }
}

void Game::Draw() {
    ClearBackground(Midnight);
    switch (screen_) {
        case GameScreen::MainMenu: DrawMainMenu(); break;
        case GameScreen::Playing: DrawPlaying(); break;
        case GameScreen::Paused:
            DrawPlaying();
            DrawPaused();
            break;
        case GameScreen::HowToPlay: DrawHowToPlay(); break;
        case GameScreen::Leaderboard: DrawLeaderboard(); break;
        case GameScreen::GameOver:
            DrawPlaying();
            DrawGameOver();
            break;
    }
}

void Game::DrawToWindow() const {
    const float windowWidth = static_cast<float>(GetScreenWidth());
    const float windowHeight = static_cast<float>(GetScreenHeight());
    const float scale = std::min(windowWidth / static_cast<float>(VirtualWidth),
                                 windowHeight / static_cast<float>(VirtualHeight));
    const float outputWidth = static_cast<float>(VirtualWidth) * scale;
    const float outputHeight = static_cast<float>(VirtualHeight) * scale;
    const Rectangle source{0.0F, 0.0F, static_cast<float>(VirtualWidth), -static_cast<float>(VirtualHeight)};
    const Rectangle destination{(windowWidth - outputWidth) / 2.0F, (windowHeight - outputHeight) / 2.0F,
                                outputWidth, outputHeight};
    DrawTexturePro(renderTarget_.texture, source, destination, {0.0F, 0.0F}, 0.0F, WHITE);
}

void Game::UpdateMainMenu() {
    if (IsKeyPressed(KEY_DOWN)) ++mainMenuSelection_;
    if (IsKeyPressed(KEY_UP)) --mainMenuSelection_;
    mainMenuSelection_ = WrapSelection(mainMenuSelection_, 4);

    if (!IsConfirmPressed()) {
        return;
    }
    switch (mainMenuSelection_) {
        case 0: StartNewGame(); break;
        case 1: screen_ = GameScreen::HowToPlay; break;
        case 2:
            leaderboard_.Load();
            screen_ = GameScreen::Leaderboard;
            break;
        case 3: exitRequested_ = true; break;
        default: break;
    }
}

void Game::UpdatePlaying(float deltaTime) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen_ = GameScreen::Paused;
        pauseMenuSelection_ = 0;
        assets_.PauseMusic();
        return;
    }

    scoreManager_.UpdateActiveTime(deltaTime);
    player_.Update(deltaTime);
    spawnTimer_ += deltaTime;

    const int level = scoreManager_.GetLevel();
    const int maximumEnemies = std::min(10, 5 + level);
    if (spawnTimer_ >= nextSpawnDelay_ && static_cast<int>(enemies_.size()) < maximumEnemies) {
        SpawnEnemy();
        spawnTimer_ = 0.0F;
        std::uniform_real_distribution<float> jitter(0.0F, 0.24F);
        nextSpawnDelay_ = std::max(0.7F, 2.15F - static_cast<float>(level - 1) * 0.12F) + jitter(random_);
    }

    for (auto& enemy : enemies_) {
        enemy.Update(deltaTime);
        if (enemy.IsActive() && enemy.GetPosition().y >= DangerLineY) {
            MissEnemy(enemy);
        }
    }

    if (!player_.IsAlive()) {
        screen_ = GameScreen::GameOver;
        gameOverSelection_ = 0;
        scoreSaved_ = false;
        assets_.PauseMusic();
        return;
    }

    for (auto& laser : lasers_) laser.Update(deltaTime);
    for (auto& particle : particles_) particle.Update(deltaTime);

    int character = GetCharPressed();
    while (character > 0) {
        if (character >= 'A' && character <= 'Z') character += 'a' - 'A';
        if (character >= 'a' && character <= 'z') {
            ProcessTypedCharacter(static_cast<char>(character));
        }
        character = GetCharPressed();
    }

    enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
                                  [](const Enemy& enemy) { return !enemy.IsActive(); }), enemies_.end());
    lasers_.erase(std::remove_if(lasers_.begin(), lasers_.end(),
                                [](const Laser& laser) { return !laser.IsAlive(); }), lasers_.end());
    particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
                                   [](const Particle& particle) { return !particle.IsAlive(); }), particles_.end());
}

void Game::UpdatePaused() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        screen_ = GameScreen::Playing;
        assets_.ResumeMusic();
        return;
    }
    if (IsKeyPressed(KEY_DOWN)) ++pauseMenuSelection_;
    if (IsKeyPressed(KEY_UP)) --pauseMenuSelection_;
    pauseMenuSelection_ = WrapSelection(pauseMenuSelection_, 3);
    if (!IsConfirmPressed()) return;

    if (pauseMenuSelection_ == 0) {
        screen_ = GameScreen::Playing;
        assets_.ResumeMusic();
    } else if (pauseMenuSelection_ == 1) {
        StartNewGame();
    } else {
        screen_ = GameScreen::MainMenu;
        assets_.ResumeMusic();
    }
}

void Game::UpdateHowToPlay() {
    if (IsKeyPressed(KEY_ESCAPE) || IsConfirmPressed()) {
        screen_ = GameScreen::MainMenu;
    }
}

void Game::UpdateLeaderboard() {
    if (IsKeyPressed(KEY_ESCAPE) || IsConfirmPressed()) {
        screen_ = GameScreen::MainMenu;
    }
}

void Game::UpdateGameOver() {
    int character = GetCharPressed();
    while (character > 0) {
        if ((std::isalnum(static_cast<unsigned char>(character)) != 0 || character == ' ')
            && playerName_.size() < 12) {
            if (playerName_ == "PLAYER") playerName_.clear();
            playerName_.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(character))));
        }
        character = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !playerName_.empty()) {
        playerName_.pop_back();
    }
    if (IsKeyPressed(KEY_DOWN)) ++gameOverSelection_;
    if (IsKeyPressed(KEY_UP)) --gameOverSelection_;
    gameOverSelection_ = WrapSelection(gameOverSelection_, 3);

    if (IsKeyPressed(KEY_ESCAPE)) {
        screen_ = GameScreen::MainMenu;
        assets_.ResumeMusic();
        return;
    }
    if (!IsConfirmPressed()) return;

    if (gameOverSelection_ == 0) {
        if (!scoreSaved_) {
            if (playerName_.empty()) playerName_ = "PLAYER";
            const LeaderboardEntry entry{playerName_, scoreManager_.GetScore(), scoreManager_.GetWpm(),
                                         scoreManager_.GetAccuracy(), scoreManager_.GetWordsDestroyed()};
            scoreSaved_ = leaderboard_.AddEntry(entry);
        }
    } else if (gameOverSelection_ == 1) {
        StartNewGame();
    } else {
        screen_ = GameScreen::MainMenu;
        assets_.ResumeMusic();
    }
}

void Game::StartNewGame() {
    enemies_.clear();
    lasers_.clear();
    particles_.clear();
    wordManager_.ResetLevelPool();
    scoreManager_.Reset();
    player_.Reset();
    nextEnemyId_ = 1;
    targetEnemyId_ = 0;
    spawnTimer_ = 0.0F;
    nextSpawnDelay_ = 0.55F;
    scoreSaved_ = false;
    playerName_ = "PLAYER";
    screen_ = GameScreen::Playing;
    assets_.ResumeMusic();
}

void Game::SpawnEnemy() {
    std::uniform_real_distribution<float> xPosition(PlayArea.x + 90.0F, PlayArea.x + PlayArea.width - 90.0F);
    float x = xPosition(random_);
    for (int attempt = 0; attempt < 20; ++attempt) {
        bool overlaps = false;
        for (const auto& enemy : enemies_) {
            if (enemy.GetPosition().y < PlayArea.y + 145.0F && std::abs(enemy.GetPosition().x - x) < 155.0F) {
                overlaps = true;
                break;
            }
        }
        if (!overlaps) break;
        x = xPosition(random_);
    }

    const int level = scoreManager_.GetLevel();
    const std::string word = wordManager_.GetRandomWord(level);
    if (word.empty()) {
        return;
    }
    std::uniform_real_distribution<float> speedVariation(0.0F, 20.0F);
    const float speed = 38.0F + static_cast<float>(level) * 6.5F + speedVariation(random_);
    std::uniform_int_distribution<int> variant(0, 4);
    enemies_.emplace_back(nextEnemyId_++, word,
                          Vector2{x, PlayArea.y + 74.0F}, speed, variant(random_));
}

void Game::ProcessTypedCharacter(char typed) {
    Enemy* target = FindTarget();
    if (target == nullptr) {
        const auto selected = SelectLowestEnemyStartingWith(enemies_, typed);
        if (!selected.has_value()) {
            scoreManager_.RecordWrongCharacter();
            assets_.PlayWrong();
            return;
        }
        targetEnemyId_ = *selected;
        target = FindTarget();
        if (target != nullptr) target->SetTargeted(true);
    }

    if (target != nullptr && target->GetNextCharacter() == typed) {
        scoreManager_.RecordCorrectCharacter();
        target->AdvanceProgress();
        lasers_.emplace_back(player_.GetPosition(), target->GetPosition());
        SpawnHitSparks(target->GetPosition());
        assets_.PlayCorrect();
        assets_.PlayLaser();
        if (target->IsComplete()) {
            DestroyEnemy(*target);
        }
    } else {
        scoreManager_.RecordWrongCharacter();
        assets_.PlayWrong();
    }
}

Enemy* Game::FindTarget() {
    const auto iterator = std::find_if(enemies_.begin(), enemies_.end(), [this](const Enemy& enemy) {
        return enemy.IsActive() && enemy.GetId() == targetEnemyId_;
    });
    return iterator == enemies_.end() ? nullptr : &*iterator;
}

const Enemy* Game::FindTarget() const {
    const auto iterator = std::find_if(enemies_.begin(), enemies_.end(), [this](const Enemy& enemy) {
        return enemy.IsActive() && enemy.GetId() == targetEnemyId_;
    });
    return iterator == enemies_.end() ? nullptr : &*iterator;
}

void Game::ClearTarget() {
    for (auto& enemy : enemies_) enemy.SetTargeted(false);
    targetEnemyId_ = 0;
}

void Game::DestroyEnemy(Enemy& enemy) {
    const Vector2 position = enemy.GetPosition();
    const int length = static_cast<int>(enemy.GetWord().size());
    enemy.Deactivate();
    scoreManager_.RecordCompletedWord(length);
    ClearTarget();
    SpawnExplosion(position, BrightGold);
    assets_.PlayExplosion();
}

void Game::MissEnemy(Enemy& enemy) {
    const bool wasTarget = enemy.GetId() == targetEnemyId_;
    enemy.Deactivate();
    scoreManager_.RecordMissedEnemy();
    player_.Damage();
    SpawnExplosion({enemy.GetPosition().x, DangerLineY}, DangerRed);
    assets_.PlayPlayerHit();
    if (wasTarget) ClearTarget();
}

void Game::SpawnExplosion(Vector2 position, Color color) {
    std::uniform_real_distribution<float> angle(0.0F, 6.2831853F);
    std::uniform_real_distribution<float> speed(70.0F, 230.0F);
    std::uniform_real_distribution<float> life(0.35F, 0.85F);
    std::uniform_real_distribution<float> size(3.0F, 8.0F);
    for (int index = 0; index < 28; ++index) {
        const float direction = angle(random_);
        const float velocity = speed(random_);
        particles_.emplace_back(position, Vector2{std::cos(direction) * velocity, std::sin(direction) * velocity},
                                life(random_), size(random_), index % 3 == 0 ? WHITE : color);
    }
}

void Game::SpawnHitSparks(Vector2 position) {
    std::uniform_real_distribution<float> offset(-1.0F, 1.0F);
    for (int index = 0; index < 4; ++index) {
        particles_.emplace_back(position, Vector2{offset(random_) * 80.0F, offset(random_) * 80.0F},
                                0.2F, 3.0F, SoftBlue);
    }
}

std::string Game::FindAssetRoot() const {
    if (std::filesystem::exists("assets/data")) {
        return "assets";
    }
    const std::filesystem::path executableAssets = std::filesystem::path(GetApplicationDirectory()) / "assets";
    if (std::filesystem::exists(executableAssets / "data")) {
        return executableAssets.string();
    }
    return "assets";
}

void Game::DrawBackground() const {
    if (const Texture2D* background = assets_.GetBackground(); background != nullptr) {
        const Rectangle source{0.0F, 0.0F, static_cast<float>(background->width), static_cast<float>(background->height)};
        const Rectangle destination{0.0F, 0.0F, static_cast<float>(VirtualWidth), static_cast<float>(VirtualHeight)};
        DrawTexturePro(*background, source, destination, {0.0F, 0.0F}, 0.0F, WHITE);
        return;
    }

    DrawRectangleGradientV(0, 0, VirtualWidth, 525, Color{4, 15, 44, 255}, Color{11, 48, 83, 255});
    DrawRectangleGradientV(0, 525, VirtualWidth, 375, Color{8, 44, 72, 255}, Color{3, 17, 35, 255});

    for (int index = 0; index < 145; ++index) {
        const int x = (index * 137 + index * index * 19) % VirtualWidth;
        const int y = 20 + (index * 79 + index * index * 7) % 430;
        const float radius = index % 11 == 0 ? 2.0F : 1.0F;
        DrawCircle(x, y, radius, Fade(index % 7 == 0 ? SKYBLUE : RAYWHITE, 0.65F));
    }
    DrawCircleGradient(315, 175, 45.0F, Color{255, 251, 220, 255}, Color{220, 228, 220, 20});
    DrawCircle(301, 164, 5.0F, Fade(GRAY, 0.35F));
    DrawCircle(326, 187, 7.0F, Fade(GRAY, 0.25F));

    DrawAngkorSilhouette();
    DrawRectangleGradientV(0, 525, VirtualWidth, 92, Color{10, 53, 78, 150}, Color{4, 30, 58, 210});
    for (int index = 0; index < 28; ++index) {
        const float y = 536.0F + static_cast<float>(index) * 10.5F;
        const float width = 180.0F + static_cast<float>((index * 47) % 330);
        const float x = 800.0F - width / 2.0F + static_cast<float>((index % 3) - 1) * 35.0F;
        DrawLineEx({x, y}, {x + width, y}, index % 4 == 0 ? 2.0F : 1.0F,
                   Fade(index % 5 == 0 ? Gold : SoftBlue, 0.18F));
    }
    DrawRectangleGradientV(0, 720, VirtualWidth, 180, Color{14, 23, 34, 255}, Color{2, 7, 15, 255});
}

void Game::DrawAngkorSilhouette() const {
    const Color farStone{16, 25, 38, 255};
    const Color stone{24, 36, 50, 255};
    const Color litStone{43, 42, 42, 255};
    DrawRectangle(330, 430, 940, 120, farStone);
    DrawRectangle(420, 398, 760, 152, stone);
    DrawRectangle(520, 355, 560, 195, litStone);

    const std::array<int, 5> centers{{520, 670, 800, 930, 1080}};
    const std::array<int, 5> heights{{135, 205, 285, 205, 135}};
    for (std::size_t index = 0; index < centers.size(); ++index) {
        const int center = centers[index];
        const int height = heights[index];
        const int baseY = 520;
        const int topY = baseY - height;
        DrawRectangle(center - 38, topY + 45, 76, height - 45, Color{31, 38, 45, 255});
        DrawTriangle({static_cast<float>(center), static_cast<float>(topY)},
                     {static_cast<float>(center - 49), static_cast<float>(topY + 60)},
                     {static_cast<float>(center + 49), static_cast<float>(topY + 60)},
                     Color{28, 34, 42, 255});
        DrawTriangle({static_cast<float>(center), static_cast<float>(topY + 24)},
                     {static_cast<float>(center - 42), static_cast<float>(topY + 70)},
                     {static_cast<float>(center + 42), static_cast<float>(topY + 70)},
                     Color{35, 40, 44, 255});
        for (int window = 0; window < 3; ++window) {
            DrawRectangle(center - 27 + window * 20, baseY - 38, 6, 13, Fade(BrightGold, 0.52F));
        }
    }
    for (int x = 370; x <= 1230; x += 55) {
        DrawRectangle(x, 501, 5, 19, Fade(BrightGold, 0.45F));
        DrawCircle(x + 2, 499, 3.0F, Fade(BrightGold, 0.5F));
    }
}

void Game::DrawOrnamentalFrame() const {
    DrawRectangle(0, 0, 238, 116, Color{18, 13, 10, 247});
    DrawRectangle(1362, 0, 238, 116, Color{18, 13, 10, 247});
    DrawRectangle(0, 0, 18, VirtualHeight, Color{26, 18, 13, 252});
    DrawRectangle(VirtualWidth - 18, 0, 18, VirtualHeight, Color{26, 18, 13, 252});
    DrawRectangleLinesEx({5.0F, 5.0F, 1590.0F, 890.0F}, 5.0F, Color{67, 42, 21, 255});
    DrawRectangleLinesEx({13.0F, 13.0F, 1574.0F, 874.0F}, 2.0F, Gold);
    DrawRectangleLinesEx({18.0F, 18.0F, 1564.0F, 864.0F}, 1.0F, Fade(BrightGold, 0.42F));

    for (int y = 45; y < 865; y += 82) {
        DrawLotusMark({9.0F, static_cast<float>(y)}, 0.55F, Gold);
        DrawLotusMark({1591.0F, static_cast<float>(y)}, 0.55F, Gold);
    }
    for (const Vector2 corner : std::array<Vector2, 4>{{
             {22.0F, 22.0F}, {1578.0F, 22.0F}, {22.0F, 878.0F}, {1578.0F, 878.0F}
         }}) {
        DrawPoly(corner, 4, 8.0F, 45.0F, BrightGold);
        DrawCircleLines(static_cast<int>(corner.x), static_cast<int>(corner.y), 12.0F, Fade(Gold, 0.65F));
    }
}

void Game::DrawPlaying() const {
    DrawBackground();
    DrawOrnamentalFrame();

    DrawRectangleRec({PlayArea.x, PlayArea.y, PlayArea.width, 3.0F}, Fade(Gold, 0.62F));
    DrawRectangleLinesEx(PlayArea, 1.0F, Fade(Gold, 0.32F));
    BeginScissorMode(static_cast<int>(PlayArea.x), static_cast<int>(PlayArea.y),
                     static_cast<int>(PlayArea.width), static_cast<int>(PlayArea.height));
    DrawLineEx({PlayArea.x, DangerLineY}, {PlayArea.x + PlayArea.width, DangerLineY}, 2.0F,
               Fade(DangerRed, 0.7F));
    DrawTextShadow(assets_.GetGameFont(), "DANGER LINE", {PlayArea.x + 12.0F, DangerLineY - 27.0F},
                   17.0F, 0.5F, Fade(DangerRed, 0.88F), 1.0F);
    for (const auto& laser : lasers_) laser.Draw();
    for (const auto& enemy : enemies_) {
        enemy.Draw(assets_.GetGameFont(), assets_.GetEnemyTexture(enemy.GetVariant()));
    }
    for (const auto& particle : particles_) particle.Draw();
    player_.Draw(assets_.GetPlayerTexture());
    EndScissorMode();

    DrawTopHud();
    DrawLeftPanel();
    DrawRightPanel();
    DrawTypingArea();
}

void Game::DrawTopHud() const {
    struct HudValue { const char* label; std::string value; Color color; };
    const std::array<HudValue, 5> values{{
        {"SCORE", TextFormat("%06d", scoreManager_.GetScore()), BrightGold},
        {"WPM", TextFormat("%.0f", scoreManager_.GetWpm()), SoftBlue},
        {"LEVEL", std::to_string(scoreManager_.GetLevel()), BrightGold},
        {"ACCURACY", TextFormat("%.0f%%", scoreManager_.GetAccuracy()), SuccessGreen},
        {"COMBO", "x" + std::to_string(scoreManager_.GetCombo()), Color{216, 111, 255, 255}}
    }};
    const Font font = assets_.GetGameFont();
    const Font displayFont = assets_.GetDisplayFont();
    for (std::size_t index = 0; index < values.size(); ++index) {
        const Rectangle card{245.0F + static_cast<float>(index) * 221.0F, 14.0F, 208.0F, 91.0F};
        DrawPanel(card, index == 2 ? BrightGold : Gold);
        const Vector2 labelSize = MeasureTextEx(displayFont, values[index].label, 17.0F, 0.5F);
        DrawTextShadow(displayFont, values[index].label,
                       {card.x + (card.width - labelSize.x) / 2.0F, card.y + 11.0F},
                       17.0F, 0.5F, Color{255, 225, 144, 255}, 1.0F);
        DrawLineEx({card.x + 34.0F, card.y + 36.0F}, {card.x + card.width - 34.0F, card.y + 36.0F},
                   1.0F, Fade(Gold, 0.35F));
        const Vector2 valueSize = MeasureTextEx(font, values[index].value.c_str(), 32.0F, 0.5F);
        DrawTextShadow(font, values[index].value.c_str(),
                       {card.x + (card.width - valueSize.x) / 2.0F, card.y + 44.0F},
                       32.0F, 0.5F, values[index].color);
        if (index < values.size() - 1) {
            DrawLotusMark({card.x + card.width + 6.5F, card.y + card.height / 2.0F}, 0.55F, BrightGold);
        }
    }
}

void Game::DrawLeftPanel() const {
    const Font font = assets_.GetGameFont();
    const Font displayFont = assets_.GetDisplayFont();
    const Rectangle title{24.0F, 140.0F, 212.0F, 338.0F};
    DrawPanel(title, Gold);
    DrawLotusMark({title.x + title.width / 2.0F, title.y + 35.0F}, 1.15F, BrightGold);
    const std::array<const char*, 3> titleLines{{"TYPING", "SPACE", "SHOOTER"}};
    for (std::size_t index = 0; index < titleLines.size(); ++index) {
        const Vector2 size = MeasureTextEx(displayFont, titleLines[index], 26.0F, 0.7F);
        DrawTextShadow(displayFont, titleLines[index],
                       {title.x + (title.width - size.x) / 2.0F, title.y + 72.0F + 45.0F * static_cast<float>(index)},
                       26.0F, 0.7F, Color{225, 180, 103, 255});
    }
    DrawLineEx({title.x + 42.0F, title.y + 218.0F}, {title.x + title.width - 42.0F, title.y + 218.0F},
               1.0F, Fade(Gold, 0.75F));
    DrawPoly({title.x + title.width / 2.0F, title.y + 218.0F}, 4, 5.0F, 45.0F, BrightGold);
    const std::array<const char*, 2> editionLines{{"ANGKOR", "EDITION"}};
    for (std::size_t index = 0; index < editionLines.size(); ++index) {
        const Vector2 size = MeasureTextEx(displayFont, editionLines[index], 21.0F, 0.6F);
        DrawTextShadow(displayFont, editionLines[index],
                       {title.x + (title.width - size.x) / 2.0F, title.y + 245.0F + 34.0F * static_cast<float>(index)},
                       21.0F, 0.6F, BrightGold, 1.0F);
    }

    const Rectangle instructions{24.0F, 530.0F, 212.0F, 340.0F};
    DrawPanel(instructions, Gold);
    const char* instructionTitle = "HOW TO PLAY";
    const Vector2 instructionTitleSize = MeasureTextEx(displayFont, instructionTitle, 19.0F, 0.4F);
    DrawTextShadow(displayFont, instructionTitle,
                   {instructions.x + (instructions.width - instructionTitleSize.x) / 2.0F, instructions.y + 16.0F},
                   19.0F, 0.4F, BrightGold, 1.0F);
    DrawLineEx({instructions.x + 18.0F, instructions.y + 49.0F},
               {instructions.x + instructions.width - 18.0F, instructions.y + 49.0F},
               1.0F, Fade(Gold, 0.48F));

    const std::array<std::array<const char*, 2>, 4> rows{{
        {{"Type each word", "before the red line"}},
        {{"Correct letters", "fire a blue laser"}},
        {{"Finish the word", "to destroy a ship"}},
        {{"ESC pauses", "the game"}}
    }};
    for (std::size_t index = 0; index < rows.size(); ++index) {
        const float rowY = instructions.y + 65.0F + static_cast<float>(index) * 66.0F;
        const Vector2 icon{instructions.x + 29.0F, rowY + 20.0F};
        if (index == 0) {
            DrawRectangleRounded({icon.x - 14.0F, icon.y - 10.0F, 28.0F, 20.0F}, 0.2F, 4, Fade(SoftBlue, 0.22F));
            DrawRectangleRoundedLinesEx({icon.x - 14.0F, icon.y - 10.0F, 28.0F, 20.0F}, 0.2F, 4, 1.0F, SoftBlue);
            for (int key = 0; key < 4; ++key) DrawRectangle(static_cast<int>(icon.x - 10.0F + key * 6.0F), static_cast<int>(icon.y - 3.0F), 4, 3, SoftBlue);
        } else if (index == 1) {
            DrawLineEx({icon.x - 12.0F, icon.y + 10.0F}, {icon.x + 12.0F, icon.y - 11.0F}, 4.0F, SoftBlue);
            DrawCircleGradient(static_cast<int>(icon.x + 12.0F), static_cast<int>(icon.y - 11.0F), 8.0F, WHITE, BLANK);
        } else if (index == 2) {
            DrawPoly(icon, 8, 14.0F, 22.5F, DangerRed);
            DrawCircleV(icon, 7.0F, BrightGold);
        } else {
            DrawTriangle({icon.x, icon.y - 14.0F}, {icon.x - 14.0F, icon.y + 12.0F}, {icon.x + 14.0F, icon.y + 12.0F}, DangerRed);
            DrawTextEx(font, "!", {icon.x - 3.0F, icon.y - 7.0F}, 17.0F, 0.0F, WarmWhite);
        }
        DrawTextShadow(font, rows[index][0], {instructions.x + 53.0F, rowY + 1.0F}, 15.5F, 0.2F, WarmWhite, 1.0F);
        DrawTextShadow(font, rows[index][1], {instructions.x + 53.0F, rowY + 24.0F}, 15.5F, 0.2F, WarmWhite, 1.0F);
        if (index < rows.size() - 1) {
            DrawLineEx({instructions.x + 16.0F, rowY + 57.0F}, {instructions.x + instructions.width - 16.0F, rowY + 57.0F},
                       1.0F, Fade(Gold, 0.2F));
        }
    }
}

void Game::DrawRightPanel() const {
    const Font font = assets_.GetGameFont();
    const Font displayFont = assets_.GetDisplayFont();
    const Rectangle healthPanel{1364.0F, 142.0F, 212.0F, 148.0F};
    DrawPanel(healthPanel, Gold);
    const Vector2 healthTitleSize = MeasureTextEx(displayFont, "HEALTH", 20.0F, 0.5F);
    DrawTextShadow(displayFont, "HEALTH",
                   {healthPanel.x + (healthPanel.width - healthTitleSize.x) / 2.0F, healthPanel.y + 15.0F},
                   20.0F, 0.5F, BrightGold, 1.0F);
    DrawLineEx({healthPanel.x + 28.0F, healthPanel.y + 48.0F},
               {healthPanel.x + healthPanel.width - 28.0F, healthPanel.y + 48.0F},
               1.0F, Fade(Gold, 0.42F));
    for (int index = 0; index < 3; ++index) {
        const Vector2 position{healthPanel.x + 22.0F + static_cast<float>(index) * 61.0F, healthPanel.y + 77.0F};
        const bool full = index < player_.GetHealth();
        if (const Texture2D* heart = assets_.GetHeartTexture(); heart != nullptr) {
            DrawTexturePro(*heart, {0.0F, 0.0F, static_cast<float>(heart->width), static_cast<float>(heart->height)},
                           {position.x, position.y, 46.0F, 42.0F}, {0.0F, 0.0F}, 0.0F,
                           full ? WHITE : Fade(GRAY, 0.35F));
        } else {
            DrawCircleV({position.x + 12.0F, position.y + 11.0F}, 12.0F, full ? DangerRed : Fade(GRAY, 0.35F));
            DrawCircleV({position.x + 29.0F, position.y + 11.0F}, 12.0F, full ? DangerRed : Fade(GRAY, 0.35F));
            DrawTriangle({position.x, position.y + 14.0F}, {position.x + 41.0F, position.y + 14.0F},
                         {position.x + 20.5F, position.y + 39.0F}, full ? DangerRed : Fade(GRAY, 0.35F));
        }
    }

    const Rectangle stats{1364.0F, 526.0F, 212.0F, 344.0F};
    DrawPanel(stats, Gold);
    const std::array<std::pair<std::string, std::string>, 5> values{{
        {"Correct letters", std::to_string(scoreManager_.GetCorrectCharacters())},
        {"Wrong letters", std::to_string(scoreManager_.GetWrongCharacters())},
        {"Total typed", std::to_string(scoreManager_.GetTotalCharacters())},
        {"Words destroyed", std::to_string(scoreManager_.GetWordsDestroyed())},
        {"Time elapsed", FormatTime(scoreManager_.GetActiveSeconds())}
    }};
    const std::array<Color, 5> colors{{SuccessGreen, DangerRed, SoftBlue, BrightGold, RAYWHITE}};
    for (std::size_t index = 0; index < values.size(); ++index) {
        const float y = stats.y + 28.0F + static_cast<float>(index) * 59.0F;
        DrawTextShadow(font, values[index].first.c_str(), {stats.x + 16.0F, y}, 15.5F, 0.2F, colors[index], 1.0F);
        const Vector2 valueSize = MeasureTextEx(font, values[index].second.c_str(), 18.0F, 0.2F);
        DrawTextShadow(font, values[index].second.c_str(),
                       {stats.x + stats.width - valueSize.x - 16.0F, y + 24.0F},
                       18.0F, 0.2F, index == 4 ? BrightGold : colors[index], 1.0F);
        if (index < values.size() - 1) {
            DrawLineEx({stats.x + 16.0F, y + 49.0F}, {stats.x + stats.width - 16.0F, y + 49.0F},
                       1.0F, Fade(Gold, 0.2F));
        }
    }
}

void Game::DrawTypingArea() const {
    const Font font = assets_.GetGameFont();
    const Font displayFont = assets_.GetDisplayFont();
    const Rectangle panel{340.0F, 793.0F, 920.0F, 99.0F};
    DrawPanel(panel, BrightGold, 3.0F);
    const Rectangle header{570.0F, 775.0F, 460.0F, 41.0F};
    DrawRectangleRounded(header, 0.12F, 7, PanelBrown);
    DrawRectangleRoundedLinesEx(header, 0.12F, 7, 2.0F, Gold);
    DrawRectangleRoundedLinesEx({header.x + 5.0F, header.y + 5.0F, header.width - 10.0F, header.height - 10.0F},
                                0.12F, 7, 1.0F, Fade(BrightGold, 0.3F));
    const char* caption = "TYPE THE WORD";
    const Vector2 captionSize = MeasureTextEx(displayFont, caption, 21.0F, 0.4F);
    DrawTextShadow(displayFont, caption, {800.0F - captionSize.x / 2.0F, 783.0F},
                   21.0F, 0.4F, Color{255, 220, 126, 255}, 1.0F);
    DrawRectangleRounded({panel.x + 25.0F, panel.y + 24.0F, panel.width - 50.0F, 61.0F},
                         0.1F, 7, Color{3, 7, 14, 232});
    DrawRectangleRoundedLinesEx({panel.x + 25.0F, panel.y + 24.0F, panel.width - 50.0F, 61.0F},
                                0.1F, 7, 1.0F, Fade(Gold, 0.24F));

    const Enemy* target = FindTarget();
    if (target == nullptr) {
        const char* hint = "TYPE A FIRST LETTER TO LOCK A TARGET";
        const Vector2 size = MeasureTextEx(font, hint, 21.0F, 0.4F);
        DrawTextShadow(font, hint, {800.0F - size.x / 2.0F, 835.0F}, 21.0F, 0.4F,
                       Fade(WarmWhite, 0.72F), 1.0F);
        if (std::fmod(GetTime(), 1.0) < 0.55) {
            DrawRectangle(static_cast<int>(800.0F + size.x / 2.0F + 8.0F), 833, 2, 27, Fade(RAYWHITE, 0.8F));
        }
        return;
    }

    const std::string& word = target->GetWord();
    const float fontSize = 42.0F;
    const Vector2 totalSize = MeasureTextEx(font, word.c_str(), fontSize, 1.0F);
    float x = 800.0F - totalSize.x / 2.0F;
    const float y = 826.0F;
    for (std::size_t index = 0; index < word.size(); ++index) {
        const char text[2]{word[index], '\0'};
        const Vector2 characterSize = MeasureTextEx(font, text, fontSize, 1.0F);
        if (index == target->GetTypedProgress()) {
            DrawRectangleRounded({x - 4.0F, y - 2.0F, characterSize.x + 8.0F, 49.0F}, 0.15F, 5,
                                 Fade(BrightGold, 0.24F));
        }
        const Color color = index < target->GetTypedProgress() ? SuccessGreen
                            : index == target->GetTypedProgress() ? BrightGold
                            : Fade(WarmWhite, 0.62F);
        DrawTextShadow(font, text, {x, y}, fontSize, 1.0F, color, 1.5F);
        x += characterSize.x;
    }
    if (std::fmod(GetTime(), 1.0) < 0.55) {
        DrawRectangle(static_cast<int>(x + 7.0F), static_cast<int>(y + 3.0F), 2, 40, Fade(RAYWHITE, 0.9F));
    }
}

void Game::DrawMainMenu() const {
    DrawBackground();
    DrawOrnamentalFrame();
    DrawRectangle(0, 0, VirtualWidth, VirtualHeight, Fade(Midnight, 0.34F));

    const Rectangle titlePanel{390.0F, 92.0F, 820.0F, 225.0F};
    DrawPanel(titlePanel, BrightGold, 3.0F);
    if (const Texture2D* logo = assets_.GetLogoTexture(); logo != nullptr) {
        DrawTexturePro(*logo, {0.0F, 0.0F, static_cast<float>(logo->width), static_cast<float>(logo->height)},
                       {435.0F, 125.0F, 145.0F, 145.0F}, {0.0F, 0.0F}, 0.0F, WHITE);
    } else {
        DrawPoly({507.0F, 193.0F}, 6, 55.0F, 30.0F, Fade(Gold, 0.25F));
        DrawPolyLines({507.0F, 193.0F}, 6, 55.0F, 30.0F, BrightGold);
        DrawTriangle({507.0F, 139.0F}, {475.0F, 225.0F}, {539.0F, 225.0F}, Gold);
    }
    DrawTextShadow(assets_.GetDisplayFont(), "TYPING SPACE SHOOTER", {612.0F, 135.0F}, 37.0F, 1.0F, BrightGold);
    DrawTextShadow(assets_.GetDisplayFont(), "ANGKOR EDITION", {660.0F, 204.0F}, 30.0F, 1.0F, SoftBlue);
    DrawTextShadow(assets_.GetGameFont(), "A Cambodian typing arcade adventure", {640.0F, 258.0F},
                   20.0F, 0.5F, Fade(WarmWhite, 0.78F), 1.0F);

    const Rectangle menuPanel{555.0F, 362.0F, 490.0F, 350.0F};
    DrawPanel(menuPanel, Gold);
    DrawMenuItems({"START GAME", "HOW TO PLAY", "LEADERBOARD", "EXIT"}, mainMenuSelection_, 402.0F);
    DrawCenteredText("UP / DOWN TO SELECT     ENTER TO CONFIRM", 755.0F, 20.0F, Fade(RAYWHITE, 0.65F));
    DrawCenteredText("Built with C++17 + raylib", 824.0F, 17.0F, Fade(SoftBlue, 0.62F));
}

void Game::DrawPaused() const {
    DrawRectangle(0, 0, VirtualWidth, VirtualHeight, Fade(BLACK, 0.68F));
    const Rectangle panel{525.0F, 205.0F, 550.0F, 490.0F};
    DrawPanel(panel, BrightGold, 3.0F);
    DrawCenteredText("GAME PAUSED", 250.0F, 48.0F, BrightGold);
    DrawCenteredText("Active time and WPM are frozen", 317.0F, 22.0F, SoftBlue);
    DrawMenuItems({"RESUME", "RESTART", "MAIN MENU"}, pauseMenuSelection_, 395.0F);
    DrawCenteredText("ESC also resumes", 640.0F, 19.0F, Fade(RAYWHITE, 0.58F));
}

void Game::DrawHowToPlay() const {
    DrawBackground();
    DrawOrnamentalFrame();
    DrawRectangle(0, 0, VirtualWidth, VirtualHeight, Fade(Midnight, 0.55F));
    const Rectangle panel{270.0F, 75.0F, 1060.0F, 750.0F};
    DrawPanel(panel, BrightGold, 3.0F);
    DrawCenteredText("HOW TO PLAY", 112.0F, 46.0F, BrightGold);
    DrawCenteredText("Defend Angkor by typing the words carried by enemy ships", 176.0F, 25.0F, SoftBlue);

    const Font font = assets_.GetGameFont();
    const std::array<std::pair<const char*, const char*>, 6> steps{{
        {"1", "Type the first letter of a visible word to lock a target."},
        {"2", "If words share that letter, the ship closest to the danger line is selected."},
        {"3", "Keep typing the selected word. Every correct letter fires a laser."},
        {"4", "A wrong letter costs 2 points and resets the combo, but keeps the target."},
        {"5", "Destroy ships before the red danger line. A missed ship costs one heart."},
        {"6", "Every 10 words raises the level, speeds up ships, and changes word difficulty."}
    }};
    float y = 250.0F;
    for (const auto& [number, text] : steps) {
        DrawCircle(340, static_cast<int>(y + 18.0F), 25.0F, Fade(Gold, 0.32F));
        const Vector2 numberSize = MeasureTextEx(font, number, 24.0F, 1.0F);
        DrawTextEx(font, number, {340.0F - numberSize.x / 2.0F, y + 5.0F}, 24.0F, 1.0F, BrightGold);
        DrawTextEx(font, text, {390.0F, y + 4.0F}, 22.0F, 1.0F, RAYWHITE);
        y += 77.0F;
    }
    DrawCenteredText("Controls: letters = type   |   ESC = pause/back   |   arrows + ENTER = menus",
                     731.0F, 21.0F, Color{255, 228, 151, 255});
    DrawCenteredText("Press ENTER or ESC to return", 783.0F, 18.0F, Fade(RAYWHITE, 0.6F));
}

void Game::DrawLeaderboard() const {
    DrawBackground();
    DrawOrnamentalFrame();
    DrawRectangle(0, 0, VirtualWidth, VirtualHeight, Fade(Midnight, 0.63F));
    const Rectangle panel{235.0F, 60.0F, 1130.0F, 785.0F};
    DrawPanel(panel, BrightGold, 3.0F);
    DrawCenteredText("HALL OF GUARDIANS", 95.0F, 44.0F, BrightGold);
    DrawCenteredText("TOP 10 LEADERBOARD", 151.0F, 22.0F, SoftBlue);

    const Font font = assets_.GetGameFont();
    const std::array<float, 6> columns{{285.0F, 375.0F, 750.0F, 900.0F, 1050.0F, 1220.0F}};
    const std::array<const char*, 6> headers{{"RANK", "PLAYER", "SCORE", "WPM", "ACC.", "WORDS"}};
    for (std::size_t index = 0; index < headers.size(); ++index) {
        DrawTextEx(font, headers[index], {columns[index], 214.0F}, 21.0F, 1.0F, BrightGold);
    }
    DrawLineEx({275.0F, 250.0F}, {1325.0F, 250.0F}, 2.0F, Gold);

    const auto& entries = leaderboard_.GetEntries();
    if (entries.empty()) {
        DrawCenteredText("No scores yet — become Angkor's first guardian!", 430.0F, 27.0F, Fade(RAYWHITE, 0.7F));
    } else {
        for (std::size_t index = 0; index < entries.size(); ++index) {
            const float y = 280.0F + static_cast<float>(index) * 49.0F;
            if (index < 3) {
                DrawRectangleRounded({268.0F, y - 8.0F, 1065.0F, 40.0F}, 0.12F, 5,
                                     Fade(index == 0 ? Gold : SoftBlue, index == 0 ? 0.15F : 0.07F));
            }
            const LeaderboardEntry& entry = entries[index];
            const Color rowColor = index == 0 ? BrightGold : RAYWHITE;
            DrawTextEx(font, std::to_string(index + 1).c_str(), {columns[0] + 18.0F, y}, 21.0F, 1.0F, rowColor);
            DrawTextEx(font, entry.playerName.c_str(), {columns[1], y}, 21.0F, 1.0F, rowColor);
            DrawTextEx(font, std::to_string(entry.score).c_str(), {columns[2], y}, 21.0F, 1.0F, rowColor);
            DrawTextEx(font, TextFormat("%.0f", entry.wpm), {columns[3], y}, 21.0F, 1.0F, SoftBlue);
            DrawTextEx(font, TextFormat("%.0f%%", entry.accuracy), {columns[4], y}, 21.0F, 1.0F, SuccessGreen);
            DrawTextEx(font, std::to_string(entry.wordsDestroyed).c_str(), {columns[5], y}, 21.0F, 1.0F, rowColor);
        }
    }
    DrawCenteredText("Press ENTER or ESC to return", 795.0F, 18.0F, Fade(RAYWHITE, 0.6F));
}

void Game::DrawGameOver() const {
    DrawRectangle(0, 0, VirtualWidth, VirtualHeight, Fade(BLACK, 0.76F));
    const Rectangle panel{355.0F, 62.0F, 890.0F, 795.0F};
    DrawPanel(panel, DangerRed, 3.0F);
    DrawCenteredText("THE LAST DEFENSE HAS FALLEN", 95.0F, 38.0F, DangerRed);
    DrawCenteredText("GAME OVER", 151.0F, 54.0F, BrightGold);

    const Font font = assets_.GetGameFont();
    const std::array<std::pair<const char*, std::string>, 6> stats{{
        {"FINAL SCORE", std::to_string(scoreManager_.GetScore())},
        {"WPM", TextFormat("%.1f", scoreManager_.GetWpm())},
        {"ACCURACY", TextFormat("%.1f%%", scoreManager_.GetAccuracy())},
        {"WORDS DESTROYED", std::to_string(scoreManager_.GetWordsDestroyed())},
        {"HIGHEST COMBO", "x" + std::to_string(scoreManager_.GetHighestCombo())},
        {"TIME SURVIVED", FormatTime(scoreManager_.GetActiveSeconds())}
    }};
    for (std::size_t index = 0; index < stats.size(); ++index) {
        const float columnX = index < 3 ? 440.0F : 810.0F;
        const float y = 248.0F + static_cast<float>(index % 3) * 61.0F;
        DrawTextEx(font, stats[index].first, {columnX, y}, 19.0F, 1.0F, Fade(RAYWHITE, 0.65F));
        DrawTextEx(font, stats[index].second.c_str(), {columnX, y + 24.0F}, 27.0F, 1.0F,
                   index == 0 ? BrightGold : SoftBlue);
    }

    DrawTextEx(font, "GUARDIAN NAME", {517.0F, 463.0F}, 21.0F, 1.0F, BrightGold);
    const Rectangle input{515.0F, 498.0F, 570.0F, 58.0F};
    DrawRectangleRounded(input, 0.18F, 8, Color{4, 10, 20, 245});
    DrawRectangleRoundedLinesEx(input, 0.18F, 8, 2.0F, scoreSaved_ ? SuccessGreen : Gold);
    DrawTextEx(font, playerName_.c_str(), {input.x + 18.0F, input.y + 13.0F}, 30.0F, 2.0F, RAYWHITE);
    if (!scoreSaved_ && std::fmod(GetTime(), 1.0) < 0.55) {
        const Vector2 nameSize = MeasureTextEx(font, playerName_.c_str(), 30.0F, 2.0F);
        DrawRectangle(static_cast<int>(input.x + 21.0F + nameSize.x), static_cast<int>(input.y + 13.0F), 2, 32, RAYWHITE);
    }
    if (scoreSaved_) {
        DrawCenteredText("SCORE SAVED", 570.0F, 19.0F, SuccessGreen);
    }
    DrawMenuItems({scoreSaved_ ? "SCORE SAVED" : "SAVE SCORE", "PLAY AGAIN", "MAIN MENU"},
                  gameOverSelection_, 612.0F);
    DrawCenteredText("Type your name, then use arrows and ENTER", 808.0F, 17.0F, Fade(RAYWHITE, 0.55F));
}

void Game::DrawPanel(Rectangle bounds, Color border, float thickness) const {
    DrawRectangleRounded({bounds.x + 5.0F, bounds.y + 7.0F, bounds.width, bounds.height},
                         0.06F, 8, Fade(BLACK, 0.72F));
    DrawRectangleRounded(bounds, 0.06F, 8, PanelBrown);
    const Rectangle inner{bounds.x + 5.0F, bounds.y + 5.0F, bounds.width - 10.0F, bounds.height - 10.0F};
    DrawRectangleRounded(inner, 0.05F, 8, PanelFill);
    if (const Texture2D* frame = assets_.GetFrameTexture(); frame != nullptr) {
        DrawTexturePro(*frame, {0.0F, 0.0F, static_cast<float>(frame->width), static_cast<float>(frame->height)},
                       bounds, {0.0F, 0.0F}, 0.0F, WHITE);
    } else {
        DrawRectangleRoundedLinesEx(bounds, 0.06F, 8, thickness, Color{83, 51, 24, 255});
        DrawRectangleRoundedLinesEx({bounds.x + 2.0F, bounds.y + 2.0F, bounds.width - 4.0F, bounds.height - 4.0F},
                                    0.06F, 8, 1.5F, border);
        DrawRectangleRoundedLinesEx(inner, 0.05F, 8, 1.0F, Fade(BrightGold, 0.32F));
        for (const Vector2 corner : std::array<Vector2, 4>{{
                 {bounds.x + 7.0F, bounds.y + 7.0F},
                 {bounds.x + bounds.width - 7.0F, bounds.y + 7.0F},
                 {bounds.x + 7.0F, bounds.y + bounds.height - 7.0F},
                 {bounds.x + bounds.width - 7.0F, bounds.y + bounds.height - 7.0F}
             }}) {
            DrawPoly(corner, 4, 5.5F, 45.0F, BrightGold);
            DrawCircleLines(static_cast<int>(corner.x), static_cast<int>(corner.y), 8.0F, Fade(border, 0.6F));
        }
        DrawLotusMark({bounds.x + bounds.width / 2.0F, bounds.y + 3.0F}, 0.35F, BrightGold);
    }
}

void Game::DrawCenteredText(const std::string& text, float y, float fontSize, Color color) const {
    const Font font = assets_.GetGameFont();
    const Vector2 size = MeasureTextEx(font, text.c_str(), fontSize, 1.0F);
    DrawTextShadow(font, text.c_str(), {(static_cast<float>(VirtualWidth) - size.x) / 2.0F, y},
                   fontSize, 1.0F, color, 1.0F);
}

void Game::DrawMenuItems(const std::vector<std::string>& items, int selected, float startY) const {
    const Font font = assets_.GetGameFont();
    for (std::size_t index = 0; index < items.size(); ++index) {
        const Rectangle item{600.0F, startY + static_cast<float>(index) * 70.0F, 400.0F, 52.0F};
        const bool active = static_cast<int>(index) == selected;
        DrawRectangleRounded(item, 0.2F, 8, active ? Fade(Gold, 0.32F) : Fade(BLACK, 0.22F));
        DrawRectangleRoundedLinesEx(item, 0.2F, 8, active ? 2.0F : 1.0F,
                                    active ? BrightGold : Fade(Gold, 0.25F));
        const Vector2 size = MeasureTextEx(font, items[index].c_str(), 25.0F, 1.0F);
        DrawTextEx(font, items[index].c_str(), {item.x + (item.width - size.x) / 2.0F, item.y + 13.0F},
                   25.0F, 1.0F, active ? BrightGold : Fade(RAYWHITE, 0.78F));
        if (active) {
            DrawPoly({item.x + 26.0F, item.y + 26.0F}, 3, 8.0F, 90.0F, BrightGold);
            DrawPoly({item.x + item.width - 26.0F, item.y + 26.0F}, 3, 8.0F, -90.0F, BrightGold);
        }
    }
}

std::string Game::FormatTime(float seconds) {
    const int totalSeconds = std::max(0, static_cast<int>(seconds));
    const int minutes = totalSeconds / 60;
    const int remainingSeconds = totalSeconds % 60;
    std::ostringstream output;
    output << std::setfill('0') << std::setw(2) << minutes << ':' << std::setw(2) << remainingSeconds;
    return output.str();
}
