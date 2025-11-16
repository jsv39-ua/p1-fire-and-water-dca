#include <GameOverState.hpp>
#include <MainGameState.hpp>
#include <StateMachine.hpp>
#include <AudioManager.hpp>
#include <memory>

extern "C" {
    #include <raylib.h>
}

GameOverState::GameOverState(bool isVictory) : isVictory(isVictory) {}

void GameOverState::init() {

    optionsButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 25, 200, 50 };
    exitButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 100, 200, 50 };
}

void GameOverState::handleInput() {
    if (IsKeyPressed(KEY_SPACE)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }
}

void GameOverState::update(float deltaTime) {
    mousePos = GetMousePosition();

    mouseOverOptionsButton = CheckCollisionPointRec(mousePos, optionsButton);
    mouseOverExitButton = CheckCollisionPointRec(mousePos, exitButton);

    // Lógica de hover para botón OPTIONS
    if (mouseOverOptionsButton && mouseExitedOptionsButton) {
        AudioManager::getInstance()->playButtonHover();
        mouseExitedOptionsButton = false;
    } else if (!mouseOverOptionsButton) {
        mouseExitedOptionsButton = true;
    }

    // Lógica de hover para botón EXIT
    if (mouseOverExitButton && mouseExitedExitButton) {
        AudioManager::getInstance()->playButtonHover();
        mouseExitedExitButton = false;
    } else if (!mouseOverExitButton) {
        mouseExitedExitButton = true;
    }

    if(mouseOverOptionsButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        // Añadir redireccion a estado de opciones
    }
    if(mouseOverExitButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }
}

void GameOverState::render() {
    BeginDrawing();
    ClearBackground(SKYBLUE);

    const char* message = isVictory ? "¡Nivel Completado!" : "Game Over";
    int textWidth = MeasureText(message, 30);
    int x = (GetScreenWidth() - textWidth) / 2;
    int y = (GetScreenHeight() / 2) - 100;

    const char* message2 = "Pulsa ESPACIO para reiniciar";
    int textWidth2 = MeasureText(message2, 20);
    int x2 = (GetScreenWidth() - textWidth2) / 2;
    int y2 = (GetScreenHeight() / 2) - 50;
    DrawText(message2, x2, y2, 20, DARKGRAY);
    DrawText(message, x, y, 30, YELLOW);

    DrawRectangleRec(optionsButton, mouseOverOptionsButton ? GRAY : DARKGRAY);
    DrawText("MENU DE PAUSA", optionsButton.x + 10, optionsButton.y + 12, 20, BLACK);
    DrawRectangleRec(exitButton, mouseOverExitButton ? GRAY : DARKGRAY);
    DrawText("MENU PRINCIPAL", exitButton.x + 10, exitButton.y + 12, 20, BLACK);

    renderGlobalUI();
    
    EndDrawing();
}