#include <states/PauseMenuState.hpp>
#include <states/MainGameState.hpp>
#include <states/MainMenuState.hpp>
#include <states/OptionsState.hpp>
#include <systems/AudioManager.hpp>
#include <systems/LocalizationManager.hpp>
PauseMenuState::PauseMenuState(){};

void PauseMenuState::init(){
    resumeButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 75, 200, 50 };
    restartButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2, 200, 50 };
    optionsButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 75, 200, 50 };
    exitButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 150, 200, 50 };
};

void PauseMenuState::handleInput(){
    // El input se maneja en update
};

void PauseMenuState::update(float deltaTime){
    mousePos = GetMousePosition();

    mouseOverResumeButton = CheckCollisionPointRec(mousePos, resumeButton);
    mouseOverRestartButton = CheckCollisionPointRec(mousePos, restartButton);
    mouseOverOptionsButton = CheckCollisionPointRec(mousePos, optionsButton);
    mouseOverExitButton = CheckCollisionPointRec(mousePos, exitButton);

    // Lógica de hover para botón RESUME
    if (mouseOverResumeButton && mouseExitedResumeButton) {
        AudioManager::getInstance()->playButtonHover();
        mouseExitedResumeButton = false;
    } else if (!mouseOverResumeButton) {
        mouseExitedResumeButton = true;
    }

    // Lógica de hover para botón RESTART
    if (mouseOverRestartButton && mouseExitedRestartButton) {
        AudioManager::getInstance()->playButtonHover();
        mouseExitedRestartButton = false;
    } else if (!mouseOverRestartButton) {
        mouseExitedRestartButton = true;
    }

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

    // Reanudar el juego - quita el menú de pausa y vuelve al juego
    if (mouseOverResumeButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->remove_state(false);
    }
    
    // Reiniciar el juego - reemplaza el estado actual del juego con uno nuevo
    if (mouseOverRestartButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        // Primero quita el menú de pausa
        this->state_machine->remove_state(false);
        // Luego reemplaza el juego con uno nuevo
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }

    // Abrir opciones - añade el estado de opciones sobre el menú de pausa
    if (mouseOverOptionsButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->add_state(std::make_unique<OptionsState>(), false);
    }

    // Volver al menú principal - reemplaza todo con el menú principal
    if (mouseOverExitButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        // Quita el menú de pausa
        this->state_machine->remove_state(false);
        // Reemplaza el estado de juego con el menú principal
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }

    // También permitir reanudar con Escape
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
    }
};

void PauseMenuState::render(){
    BeginDrawing();

    // Fondo igual que el menú principal
    ClearBackground(SKYBLUE);
    
    const char* title = _("PAUSA");
    int titleWidth = MeasureText(title, 25);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, 30, 25, YELLOW);

    // Botón de reanudar
    DrawRectangleRec(resumeButton, mouseOverResumeButton ? GRAY : DARKGRAY);
    const char* resumeText = _("REANUDAR");
    int resumeTextWidth = MeasureText(resumeText, 20);
    DrawText(resumeText, resumeButton.x + (resumeButton.width - resumeTextWidth) / 2, resumeButton.y + (resumeButton.height - 20) / 2, 20, BLACK);

    // Botón de reiniciar
    DrawRectangleRec(restartButton, mouseOverRestartButton ? GRAY : DARKGRAY);
    const char* restartText = _("REINICIAR");
    int restartTextWidth = MeasureText(restartText, 20);
    DrawText(restartText, restartButton.x + (restartButton.width - restartTextWidth) / 2, restartButton.y + (restartButton.height - 20) / 2, 20, BLACK);

    // Botón de opciones
    DrawRectangleRec(optionsButton, mouseOverOptionsButton ? GRAY : DARKGRAY);
    const char* optionsText = _("OPCIONES");
    int optionsTextWidth = MeasureText(optionsText, 20);
    DrawText(optionsText, optionsButton.x + (optionsButton.width - optionsTextWidth) / 2, optionsButton.y + (optionsButton.height - 20) / 2, 20, BLACK);

    // Botón de salir
    DrawRectangleRec(exitButton, mouseOverExitButton ? GRAY : DARKGRAY);
    const char* exitText = _("MENU PRINCIPAL");
    int exitTextWidth = MeasureText(exitText, 20);
    DrawText(exitText, exitButton.x + (exitButton.width - exitTextWidth) / 2, exitButton.y + (exitButton.height - 20) / 2, 20, BLACK);
    renderGlobalUI();
    
    EndDrawing();
};
