#pragma once

extern "C" {
    #include <raylib.h>
}

#include <GameState.hpp>
#include <StateMachine.hpp>
#include <Player.hpp>

class PauseMenuState : public GameState
{
public:
    PauseMenuState(Player & firePlayer, Player & waterPlayer);
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
    Rectangle optionsButton;
    bool mouseOverResumeButton = false;
    bool mouseOverRestartButton = false;
    bool mouseOverExitButton = false;
    bool mouseOverOptionsButton = false;
    
    // Variables para hover sounds
    bool mouseExitedResumeButton = true;
    bool mouseExitedRestartButton = true;
    bool mouseExitedExitButton = true;
    bool mouseExitedOptionsButton = true;

    Player* firePlayer;
    Player* waterPlayer;
};
