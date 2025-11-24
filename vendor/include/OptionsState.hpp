#pragma once

#include <GameState.hpp>
#include <Player.hpp>

extern "C" {
    #include <raylib.h>
}

class OptionsState : public GameState
{
public:
    enum Language {
        SPANISH = 0,
        ENGLISH = 1
    };

    OptionsState();
    OptionsState(Player & firePlayer, Player & waterPlayer);
    ~OptionsState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause(){};
    void resume() {};

    static Language getLanguage();

private:
    static Language currentLanguage;

    Player* firePlayer;
    Player* waterPlayer;
    
    // Botones
    Rectangle languageButton;
    Rectangle keyBindingsButton;
    Rectangle masterVolumeSlider;
    Rectangle musicVolumeSlider;
    Rectangle sfxVolumeSlider;
    Rectangle backButton;

    // Volúmenes (obtenidos de AudioManager)
    float masterVolume;
    float musicVolume;
    float sfxVolume;

    // Estados de arrastre
    bool draggingMasterVolume;
    bool draggingMusicVolume;
    bool draggingSfxVolume;

    // Mouse hover states
    bool mouseOverLanguageButton = false;
    bool mouseOverKeyBindingsButton = false;
    bool mouseOverBackButton = false;
    bool mouseExitedLanguageButton = true;
    bool mouseExitedKeyBindingsButton = true;
    bool mouseExitedBackButton = true;

    void updateMasterVolumeSlider();
    void updateMusicVolumeSlider();
    void updateSfxVolumeSlider();
};