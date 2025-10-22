#pragma once

extern "C" {
    #include <raylib.h>
}

#include <GameState.hpp>

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

public:
    public:
    enum Language {
        SPANISH = 0,
        ENGLISH = 1
    };

private:
    static float volume;
    static Language currentLanguage;

    Rectangle backButton;
    Rectangle languageButton;
    Rectangle volumeSlider;
    Rectangle volumeHandle;
    Rectangle musicVolumeSlider;
    Rectangle musicVolumeHandle;

    bool draggingVolume;
    bool draggingMusicVolume;
    Vector2 mousePos;

    
   
    bool mouseOverBackButton = false;
    bool mouseOverLanguageButton = false;
    bool mouseOverVolumeButton = false;
    bool mouseOverMusicVolumeButton = false;

    bool mouseExitedBackButton = true;
    bool mouseExitedLanguageButton = true;
    bool mouseExitedVolumeButton = true;
    bool mouseExitedMusicVolumeButton = true;

    void updateVolumeSlider();
    void updateMusicVolumeSlider();
    
public:
    // Funciones estáticas para acceder a las opciones
    static bool getDarkTheme();
    static float getVolume();
    static float getMusicVolume();
    static Language getLanguage();
};