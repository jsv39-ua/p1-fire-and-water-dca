#pragma once

extern "C" {
    #include <raylib.h>
}

#include <states/GameState.hpp>

class OptionsState : public GameState
{
public:
    OptionsState();
    ~OptionsState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause() {};
    void resume() {};

    enum Language {
        SPANISH = 0,
        ENGLISH = 1
    };

private:
    // Variables de instancia para UI (NO son la fuente de verdad)
    // La fuente de verdad es AudioManager
    float masterVolume;      // Cache local del volumen general
    float musicVolume;       // Cache local del volumen de música
    float sfxVolume;         // Cache local del volumen de efectos
    
    static Language currentLanguage;

    Rectangle backButton;
    Rectangle languageButton;
    Rectangle keyBindingsButton;
    
    // Sliders de volumen
    Rectangle masterVolumeSlider;
    Rectangle masterVolumeHandle;
    Rectangle musicVolumeSlider;
    Rectangle musicVolumeHandle;
    Rectangle sfxVolumeSlider;
    Rectangle sfxVolumeHandle;

    // Estados de dragging
    bool draggingMasterVolume;
    bool draggingMusicVolume;
    bool draggingSfxVolume;
    
    Vector2 mousePos;

    // Estados de hover
    bool mouseOverBackButton = false;
    bool mouseOverLanguageButton = false;
    bool mouseOverKeyBindingsButton = false;
    bool mouseOverMasterVolumeButton = false;
    bool mouseOverMusicVolumeButton = false;
    bool mouseOverSfxVolumeButton = false;

    bool mouseExitedBackButton = true;
    bool mouseExitedLanguageButton = true;
    bool mouseExitedKeyBindingsButton = true;
    bool mouseExitedMasterVolumeButton = true;
    bool mouseExitedMusicVolumeButton = true;
    bool mouseExitedSfxVolumeButton = true;

    void updateMasterVolumeSlider();
    void updateMusicVolumeSlider();
    void updateSfxVolumeSlider();
    
public:
    // Función estática para acceder al idioma
    static Language getLanguage();
};