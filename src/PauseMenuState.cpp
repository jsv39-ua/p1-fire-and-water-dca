#include <PauseMenuState.hpp>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>
#include <OptionsState.hpp>
#include <AudioManager.hpp>
#include <Player.hpp>

PauseMenuState::PauseMenuState(Player & firePlayer, Player & waterPlayer){ 
    this->firePlayer = &firePlayer;
    this->waterPlayer = &waterPlayer;
};

void PauseMenuState::init(){
    resumeButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 75, 200, 50 };
    restartButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2, 200, 50 };
    optionsButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 75, 200, 50 };
    exitButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 150, 200, 50 };
};

void PauseMenuState::handleInput()
{
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, resumeButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->remove_state(false);
        }

        if (CheckCollisionPointRec(mouse, optionsButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->add_state(
                std::make_unique<OptionsState>(*firePlayer, *waterPlayer), 
                false
            );
        }

        if (CheckCollisionPointRec(mouse, exitButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->remove_state(false);
            this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
    }
}

void PauseMenuState::update(float deltaTime){
    mousePos = GetMousePosition();

    mouseOverResumeButton = CheckCollisionPointRec(mousePos, resumeButton);
    mouseOverRestartButton = CheckCollisionPointRec(mousePos, restartButton);
    mouseOverOptionsButton = CheckCollisionPointRec(mousePos, optionsButton);
    mouseOverExitButton = CheckCollisionPointRec(mousePos, exitButton);

    AudioManager* audioMgr = AudioManager::getInstance();

    if (mouseOverResumeButton && mouseExitedResumeButton) {
        audioMgr->playButtonHover();
        mouseExitedResumeButton = false;
    } else if (!mouseOverResumeButton) {
        mouseExitedResumeButton = true;
    }

    if (mouseOverRestartButton && mouseExitedRestartButton) {
        audioMgr->playButtonHover();
        mouseExitedRestartButton = false;
    } else if (!mouseOverRestartButton) {
        mouseExitedRestartButton = true;
    }

    if (mouseOverOptionsButton && mouseExitedOptionsButton) {
        audioMgr->playButtonHover();
        mouseExitedOptionsButton = false;
    } else if (!mouseOverOptionsButton) {
        mouseExitedOptionsButton = true;
    }

    if (mouseOverExitButton && mouseExitedExitButton) {
        audioMgr->playButtonHover();
        mouseExitedExitButton = false;
    } else if (!mouseOverExitButton) {
        mouseExitedExitButton = true;
    }
};

void PauseMenuState::render(){
    BeginDrawing();

    ClearBackground(SKYBLUE);
    
    const char* title = "PAUSA";
    int titleWidth = MeasureText(title, 25);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, 30, 25, YELLOW);

    DrawRectangleRec(resumeButton, mouseOverResumeButton ? GRAY : DARKGRAY);
    DrawText("REANUDAR", resumeButton.x + 45, resumeButton.y + 12, 20, BLACK);

    DrawRectangleRec(restartButton, mouseOverRestartButton ? GRAY : DARKGRAY);
    DrawText("REINICIAR", restartButton.x + 45, restartButton.y + 12, 20, BLACK);

    DrawRectangleRec(optionsButton, mouseOverOptionsButton ? GRAY : DARKGRAY);
    DrawText("OPCIONES", optionsButton.x + 45, optionsButton.y + 12, 20, BLACK);

    DrawRectangleRec(exitButton, mouseOverExitButton ? GRAY : DARKGRAY);
    DrawText("MENU PRINCIPAL", exitButton.x + 15, exitButton.y + 12, 20, BLACK);

    renderGlobalUI();
    
    EndDrawing();
};
