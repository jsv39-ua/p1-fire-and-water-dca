#pragma once

#include <GameState.hpp>

extern "C" {
    #include <raylib.h>
}

// Forward declaration en lugar de include
class MainMenuState;

class GameOverState : public GameState
{
public:
    GameOverState(bool victory = false);
    ~GameOverState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause(){};
    void resume(){};

private:
    bool victory;
    Rectangle restartButton;
    Rectangle menuButton;
    
    bool mouseOverRestartButton = false;
    bool mouseOverMenuButton = false;
    bool mouseExitedRestartButton = true;
    bool mouseExitedMenuButton = true;
};