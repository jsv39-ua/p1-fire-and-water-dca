#pragma once

#include <GameState.hpp>
#include <memory>

extern "C" {
    #include <raylib.h>
}

class GameOverState : public GameState
{
public:
    GameOverState();
    ~GameOverState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;
    
    void pause() override {};
    void resume() override {};
};