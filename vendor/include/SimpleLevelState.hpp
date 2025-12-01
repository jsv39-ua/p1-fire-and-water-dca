 #pragma once

#include <GameState.hpp>
#include <MainMenuState.hpp>
#include <string>

extern "C" {
    #include <raylib.h>
}

class SimpleLevelState : public GameState {
public:
    SimpleLevelState();
    ~SimpleLevelState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause() override {};
    void resume() override {};
private:
    Vector2 mousePos;
};
