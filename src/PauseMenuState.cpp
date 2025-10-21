#include <PauseMenuState.hpp>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>

PauseMenuState::PauseMenuState(){};

void PauseMenuState::init(){
    resumeButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 75, 200, 50 };
    restartButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2, 200, 50 };
    exitButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 75, 200, 50 };
};

void PauseMenuState::handleInput(){
    // El input se maneja en update
};

void PauseMenuState::update(float deltaTime){
    mousePos = GetMousePosition();

    mouseOverResumeButton = CheckCollisionPointRec(mousePos, resumeButton);
    mouseOverRestartButton = CheckCollisionPointRec(mousePos, restartButton);
    mouseOverExitButton = CheckCollisionPointRec(mousePos, exitButton);

    // Reanudar el juego - quita el menú de pausa y vuelve al juego
    if (mouseOverResumeButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->state_machine->remove_state(false);
    }
    
    // Reiniciar el juego - reemplaza el estado actual del juego con uno nuevo
    if (mouseOverRestartButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Primero quita el menú de pausa
        this->state_machine->remove_state(false);
        // Luego reemplaza el juego con uno nuevo
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }

    // Volver al menú principal - reemplaza todo con el menú principal
    if (mouseOverExitButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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
    
    // Título centrado
    const char* title = "PAUSA";
    int titleWidth = MeasureText(title, 25);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, 30, 25, YELLOW);

    // Botón de reanudar
    DrawRectangleRec(resumeButton, mouseOverResumeButton ? GRAY : DARKGRAY);
    DrawText("REANUDAR", resumeButton.x + 45, resumeButton.y + 12, 20, BLACK);

    // Botón de reiniciar
    DrawRectangleRec(restartButton, mouseOverRestartButton ? GRAY : DARKGRAY);
    DrawText("REINICIAR", restartButton.x + 45, restartButton.y + 12, 20, BLACK);

    // Botón de salir
    DrawRectangleRec(exitButton, mouseOverExitButton ? GRAY : DARKGRAY);
    DrawText("MENU PRINCIPAL", exitButton.x + 15, exitButton.y + 12, 20, BLACK);

    EndDrawing();
};
