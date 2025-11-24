#pragma once

#include <GameState.hpp>
#include <Player.hpp>
#include <string>

extern "C" {
    #include <raylib.h>
}

class KeyBindingsState : public GameState {
public:
    KeyBindingsState(Player& firePlayer, Player& waterPlayer);
    ~KeyBindingsState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause() {};
    void resume() {};

private:

    Player* firePlayer;
    Player* waterPlayer;

    // Botones UI
    Rectangle fireLeftButton;
    Rectangle fireRightButton;
    Rectangle fireJumpButton;
    Rectangle waterLeftButton;
    Rectangle waterRightButton;
    Rectangle waterJumpButton;
    Rectangle saveButton;
    Rectangle cancelButton;

    bool mouseOverSaveButton = false;
    bool mouseOverCancelButton = false;
    bool mouseExitedSaveButton = true;
    bool mouseExitedCancelButton = true;
};