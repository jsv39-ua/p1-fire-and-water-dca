#pragma once

extern "C" {
    #include <raylib.h>
}

#include <GameState.hpp>
#include <StateMachine.hpp>

class PauseMenuState : public GameState
{
public:
    PauseMenuState();
    virtual ~PauseMenuState() = default;

    virtual void init() override;
    virtual void handleInput() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;

    virtual void pause(){};
    virtual void resume(){};

private:
    // Variables para los botones y manejar el ratón
    Vector2 mousePos;
    Rectangle resumeButton;
    Rectangle restartButton;
    Rectangle exitButton;
    bool mouseOverResumeButton = false;
    bool mouseOverRestartButton = false;
    bool mouseOverExitButton = false;
};
