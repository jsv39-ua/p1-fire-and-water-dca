#include <GameOverState.hpp>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>
#include <StateMachine.hpp>
#include <AudioManager.hpp>

extern "C" {
    #include <raylib.h>
}

GameOverState::GameOverState(bool victory) : victory(victory) {}

void GameOverState::init()
{
    restartButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 50, 200, 50 };
    menuButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 25, 200, 50 };
}

void GameOverState::handleInput()
{
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, restartButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->remove_state(false);
            this->state_machine->add_state(std::make_unique<MainGameState>(), true);
        }

        if (CheckCollisionPointRec(mouse, menuButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->remove_state(false);
            this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }
}

void GameOverState::update(float deltaTime)
{
    Vector2 mousePos = GetMousePosition();

    mouseOverRestartButton = CheckCollisionPointRec(mousePos, restartButton);
    mouseOverMenuButton = CheckCollisionPointRec(mousePos, menuButton);

    AudioManager* audioMgr = AudioManager::getInstance();

    if (mouseOverRestartButton && mouseExitedRestartButton) {
        audioMgr->playButtonHover();
        mouseExitedRestartButton = false;
    } else if (!mouseOverRestartButton) {
        mouseExitedRestartButton = true;
    }

    if (mouseOverMenuButton && mouseExitedMenuButton) {
        audioMgr->playButtonHover();
        mouseExitedMenuButton = false;
    } else if (!mouseOverMenuButton) {
        mouseExitedMenuButton = true;
    }
}

void GameOverState::render()
{
    BeginDrawing();

    ClearBackground(SKYBLUE);
    
    const char* message = victory ? "¡Nivel Completado!" : "Game Over";
    int messageWidth = MeasureText(message, 40);
    DrawText(message, GetScreenWidth()/2 - messageWidth/2, GetScreenHeight()/2 - 150, 40, victory ? GREEN : RED);

    DrawRectangleRec(restartButton, mouseOverRestartButton ? GRAY : DARKGRAY);
    DrawText("REINICIAR", restartButton.x + 40, restartButton.y + 12, 20, BLACK);

    DrawRectangleRec(menuButton, mouseOverMenuButton ? GRAY : DARKGRAY);
    DrawText("MENU PRINCIPAL", menuButton.x + 15, menuButton.y + 12, 20, BLACK);

    renderGlobalUI();
    
    EndDrawing();
}