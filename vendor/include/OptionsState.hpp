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
    static bool dark_theme;
    static float volume;
    static Language current_language;
    
    Rectangle back_button;
    Rectangle language_button;
    Rectangle volume_slider;
    Rectangle volume_handle;
    
    bool dragging_volume;
    
    void updateVolumeSlider();
    
public:
    // Funciones estáticas para acceder a las opciones
    static bool getDarkTheme();
    static float getVolume();
    static Language getLanguage();
};