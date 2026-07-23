#include "AssetManager.h"

#include <array>
#include <iostream>

bool AssetManager::LoadAll(const std::string& assetRoot) {
    const auto path = [&assetRoot](const std::string& relative) {
        return assetRoot + "/" + relative;
    };

    LoadTextureAsset(path("backgrounds/angkor_night.png"), background_);
    LoadTextureAsset(path("ships/player.png"), player_);

    const std::array<const char*, 5> enemyFiles{{
        "ships/enemy_blue.png",
        "ships/enemy_red.png",
        "ships/enemy_green.png",
        "ships/enemy_purple.png",
        "ships/enemy_yellow.png"
    }};

    for (std::size_t index = 0; index < enemies_.size(); ++index) {
        LoadTextureAsset(path(enemyFiles[index]), enemies_[index]);
    }

    LoadTextureAsset(path("ui/heart.png"), heart_);
    LoadTextureAsset(path("ui/khmer_frame.png"), frame_);
    LoadTextureAsset(path("ui/logo.png"), logo_);

    const std::string fontPath = path("fonts/game_font.ttf");

    if (FileExists(fontPath.c_str())) {
        gameFont_ = LoadFontEx(fontPath.c_str(), 56, nullptr, 0);
        gameFontLoaded_ = gameFont_.texture.id != 0;

        if (gameFontLoaded_) {
            SetTextureFilter(gameFont_.texture, TEXTURE_FILTER_BILINEAR);
        }
    } else {
        std::cerr << "Warning: missing " << fontPath
                  << "; using raylib default font.\n";
    }

    const std::string displayFontPath = path("fonts/display_font.ttf");

    if (FileExists(displayFontPath.c_str())) {
        displayFont_ = LoadFontEx(displayFontPath.c_str(), 64, nullptr, 0);
        displayFontLoaded_ = displayFont_.texture.id != 0;

        if (displayFontLoaded_) {
            SetTextureFilter(displayFont_.texture, TEXTURE_FILTER_BILINEAR);
        }
    } else {
        std::cerr << "Warning: missing " << displayFontPath
                  << "; using the regular game font for headings.\n";
    }

    const std::string khmerFontPath = path("fonts/khmer_font.ttf");

    if (FileExists(khmerFontPath.c_str())) {
        khmerFont_ = LoadFontEx(khmerFontPath.c_str(), 48, nullptr, 0);
        khmerFontLoaded_ = khmerFont_.texture.id != 0;
    } else {
        std::cerr << "Warning: missing " << khmerFontPath
                  << "; Khmer text will use the game font.\n";
    }

    if (IsAudioDeviceReady()) {
        LoadSoundAsset(path("sounds/laser.wav"), laser_);
        LoadSoundAsset(path("sounds/explosion.wav"), explosion_);
        LoadSoundAsset(path("sounds/correct.wav"), correct_);
        LoadSoundAsset(path("sounds/wrong.wav"), wrong_);
        LoadSoundAsset(path("sounds/player_hit.wav"), playerHit_);

        // Make sound effects quieter than the background music.
        if (laser_.loaded) {
            SetSoundVolume(laser_.value, 0.2F);
        }

        if (explosion_.loaded) {
            SetSoundVolume(explosion_.value, 0.40F);
        }

        if (correct_.loaded) {
            SetSoundVolume(correct_.value, 0.25F);
        }

        if (wrong_.loaded) {
            SetSoundVolume(wrong_.value, 0.30F);
        }

        if (playerHit_.loaded) {
            SetSoundVolume(playerHit_.value, 0.40F);
        }

        const std::string musicPath = path("sounds/music.wav");

        if (FileExists(musicPath.c_str())) {
            music_ = LoadMusicStream(musicPath.c_str());
            musicLoaded_ = music_.ctxData != nullptr;

            if (musicLoaded_) {
                SetMusicVolume(music_, 0.80F);
            }
        } else {
            std::cerr << "Warning: missing " << musicPath
                      << "; continuing without music.\n";
        }
    } else {
        std::cerr << "Warning: audio device is unavailable; "
                  << "continuing without audio.\n";
    }

    return true;
}

void AssetManager::UnloadAll() {
    if (musicLoaded_) {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
        musicLoaded_ = false;
    }

    for (SoundAsset* sound :
         {&laser_, &explosion_, &correct_, &wrong_, &playerHit_}) {
        if (sound->loaded) {
            UnloadSound(sound->value);
            sound->loaded = false;
        }
    }

    if (gameFontLoaded_) {
        UnloadFont(gameFont_);
        gameFontLoaded_ = false;
    }

    if (displayFontLoaded_) {
        UnloadFont(displayFont_);
        displayFontLoaded_ = false;
    }

    if (khmerFontLoaded_) {
        UnloadFont(khmerFont_);
        khmerFontLoaded_ = false;
    }

    for (TextureAsset* texture :
         {&background_, &player_, &enemies_[0], &enemies_[1],
          &enemies_[2], &enemies_[3], &enemies_[4],
          &heart_, &frame_, &logo_}) {
        if (texture->loaded) {
            UnloadTexture(texture->value);
            texture->loaded = false;
        }
    }
}

Font AssetManager::GetGameFont() const {
    return gameFontLoaded_ ? gameFont_ : GetFontDefault();
}

Font AssetManager::GetDisplayFont() const {
    return displayFontLoaded_ ? displayFont_ : GetGameFont();
}

const Texture2D* AssetManager::GetBackground() const {
    return background_.loaded ? &background_.value : nullptr;
}

const Texture2D* AssetManager::GetPlayerTexture() const {
    return player_.loaded ? &player_.value : nullptr;
}

const Texture2D* AssetManager::GetEnemyTexture(int variant) const {
    const std::size_t index =
        static_cast<std::size_t>((variant % 5 + 5) % 5);

    return enemies_[index].loaded ? &enemies_[index].value : nullptr;
}

const Texture2D* AssetManager::GetHeartTexture() const {
    return heart_.loaded ? &heart_.value : nullptr;
}

const Texture2D* AssetManager::GetFrameTexture() const {
    return frame_.loaded ? &frame_.value : nullptr;
}

const Texture2D* AssetManager::GetLogoTexture() const {
    return logo_.loaded ? &logo_.value : nullptr;
}

void AssetManager::PlayLaser() const {
    if (laser_.loaded) {
        PlaySound(laser_.value);
    }
}

void AssetManager::PlayExplosion() const {
    if (explosion_.loaded) {
        PlaySound(explosion_.value);
    }
}

void AssetManager::PlayCorrect() const {
    if (correct_.loaded) {
        PlaySound(correct_.value);
    }
}

void AssetManager::PlayWrong() const {
    if (wrong_.loaded) {
        PlaySound(wrong_.value);
    }
}

void AssetManager::PlayPlayerHit() const {
    if (playerHit_.loaded) {
        PlaySound(playerHit_.value);
    }
}

void AssetManager::StartMusic() {
    if (musicLoaded_) {
        PlayMusicStream(music_);
    }
}

void AssetManager::PauseMusic() {
    if (musicLoaded_) {
        PauseMusicStream(music_);
    }
}

void AssetManager::ResumeMusic() {
    if (musicLoaded_) {
        ResumeMusicStream(music_);
    }
}

void AssetManager::UpdateMusic() {
    if (musicLoaded_) {
        UpdateMusicStream(music_);
    }
}

bool AssetManager::LoadTextureAsset(
    const std::string& path,
    TextureAsset& destination
) {
    if (!FileExists(path.c_str())) {
        std::cerr << "Warning: missing " << path
                  << "; using a procedural fallback.\n";
        return false;
    }

    destination.value = LoadTexture(path.c_str());
    destination.loaded = destination.value.id != 0;

    return destination.loaded;
}

bool AssetManager::LoadSoundAsset(
    const std::string& path,
    SoundAsset& destination
) {
    if (!FileExists(path.c_str())) {
        std::cerr << "Warning: missing " << path
                  << "; sound disabled for this effect.\n";
        return false;
    }

    destination.value = LoadSound(path.c_str());
    destination.loaded = destination.value.frameCount > 0;

    return destination.loaded;
}