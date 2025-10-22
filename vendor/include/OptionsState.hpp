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

    bool draggingVolume;
    Vector2 mousePos;
    
   
    bool mouseOverBackButton = false;
    bool mouseOverLanguageButton = false;
    bool mouseOverVolumeButton = false;
    bool mouseExitedBackButton = true;
    bool mouseExitedLanguageButton = true;
    bool mouseExitedVolumeButton = true;

    void updateVolumeSlider();
    
public:
    // Funciones estáticas para acceder a las opciones
    static bool getDarkTheme();
    static float getVolume();
    static Language getLanguage();
};