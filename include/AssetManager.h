#pragma once

#include "raylib.h"

#include <array>
#include <string>

class AssetManager {
public:
    bool LoadAll(const std::string& assetRoot);
    void UnloadAll();

    Font GetGameFont() const;
    Font GetDisplayFont() const;
    const Texture2D* GetBackground() const;
    const Texture2D* GetPlayerTexture() const;
    const Texture2D* GetEnemyTexture(int variant) const;
    const Texture2D* GetHeartTexture() const;
    const Texture2D* GetFrameTexture() const;
    const Texture2D* GetLogoTexture() const;

    void PlayLaser() const;
    void PlayExplosion() const;
    void PlayCorrect() const;
    void PlayWrong() const;
    void PlayPlayerHit() const;
    void StartMusic();
    void PauseMusic();
    void ResumeMusic();
    void UpdateMusic();

private:
    struct TextureAsset {
        Texture2D value{};
        bool loaded = false;
    };
    struct SoundAsset {
        Sound value{};
        bool loaded = false;
    };

    static bool LoadTextureAsset(const std::string& path, TextureAsset& destination);
    static bool LoadSoundAsset(const std::string& path, SoundAsset& destination);

    TextureAsset background_;
    TextureAsset player_;
    std::array<TextureAsset, 5> enemies_{};
    TextureAsset heart_;
    TextureAsset frame_;
    TextureAsset logo_;
    Font gameFont_{};
    bool gameFontLoaded_ = false;
    Font displayFont_{};
    bool displayFontLoaded_ = false;
    Font khmerFont_{};
    bool khmerFontLoaded_ = false;
    SoundAsset laser_;
    SoundAsset explosion_;
    SoundAsset correct_;
    SoundAsset wrong_;
    SoundAsset playerHit_;
    Music music_{};
    bool musicLoaded_ = false;
};
