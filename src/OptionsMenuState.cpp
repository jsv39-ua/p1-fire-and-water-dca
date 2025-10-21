#include <OptionsMenuState.hpp>
#include <MainMenuState.hpp>

OptionsMenuState::OptionsMenuState(bool* musicEnabled) : musicEnabled(musicEnabled) {};

void OptionsMenuState::init(){
    musicToggleButton = { (float)GetScreenWidth()/2 - 150, (float)GetScreenHeight()/2 - 50, 300, 50 };
    backButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 75, 200, 50 };
};

void OptionsMenuState::handleInput(){
    // El input se maneja en update
};

void OptionsMenuState::update(float deltaTime){
    mousePos = GetMousePosition();

    mouseOverMusicToggle = CheckCollisionPointRec(mousePos, musicToggleButton);
    mouseOverBackButton = CheckCollisionPointRec(mousePos, backButton);

    // Alternar música
    if (mouseOverMusicToggle && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *musicEnabled = !(*musicEnabled);
    }

    // Volver al menú principal
    if (mouseOverBackButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->state_machine->remove_state(false);
    }

    // También permitir volver con Escape
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
    }
};

void OptionsMenuState::render(){
    BeginDrawing();

    ClearBackground(SKYBLUE);
    
    // Título centrado
    const char* title = "OPCIONES";
    int titleWidth = MeasureText(title, 25);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, 30, 25, YELLOW);

    // Botón de música con estado
    DrawRectangleRec(musicToggleButton, mouseOverMusicToggle ? GRAY : DARKGRAY);
    const char* musicText = *musicEnabled ? "MUSICA: ACTIVADA" : "MUSICA: DESACTIVADA";
    int musicTextWidth = MeasureText(musicText, 20);
    DrawText(musicText, musicToggleButton.x + (musicToggleButton.width - musicTextWidth)/2, 
             musicToggleButton.y + 12, 20, *musicEnabled ? GREEN : RED);

    // Botón de volver
    DrawRectangleRec(backButton, mouseOverBackButton ? GRAY : DARKGRAY);
    DrawText("VOLVER", backButton.x + 65, backButton.y + 12, 20, BLACK);

    EndDrawing();
};
