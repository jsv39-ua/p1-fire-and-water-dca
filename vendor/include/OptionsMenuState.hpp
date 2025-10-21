#pragma once

extern "C" {
    #include <raylib.h>
}

#include <GameState.hpp>
#include <StateMachine.hpp>

class OptionsMenuState : public GameState
{
public:
    OptionsMenuState(bool* musicEnabled);
    virtual ~OptionsMenuState() = default;

    virtual void init() override;
    virtual void handleInput() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;

    virtual void pause(){};
    virtual void resume(){};

private:
    // Variables para los botones y manejar el ratón
    Vector2 mousePos;
    Rectangle musicToggleButton;
    Rectangle backButton;
    bool mouseOverMusicToggle = false;
    bool mouseOverBackButton = false;
    
    // Puntero a la variable global de música
    bool* musicEnabled;
};
