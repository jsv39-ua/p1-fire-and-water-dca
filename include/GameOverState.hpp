#pragma once

#include <GameState.hpp>
#include <memory>
#include <MainMenuState.hpp>

extern "C" {
    #include <raylib.h>
}

class GameOverState : public GameState
{
public:
    GameOverState(bool isVictory = false);
    ~GameOverState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;
    
    void pause() override {};
    void resume() override {};

private:
    Vector2 mousePos;
    Rectangle optionsButton;
    Rectangle exitButton;
    bool mouseOverOptionsButton = false;
    bool mouseOverExitButton = false;
    
    
    bool mouseExitedOptionsButton = true;
    bool mouseExitedExitButton = true;
    
    bool isVictory; // true si ganó, false si perdió
};