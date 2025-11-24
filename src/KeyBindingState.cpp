#include "KeyBindingsState.hpp"
#include "StateMachine.hpp"
#include "AudioManager.hpp"

extern "C" {
    #include <raylib.h>
}

KeyBindingsState::KeyBindingsState(Player& firePlayer, Player& waterPlayer)
{
    this->firePlayer = &firePlayer;
    this->waterPlayer = &waterPlayer;
}

void KeyBindingsState::init() {
    int screenWidth = GetScreenWidth();
    float centerX = screenWidth / 2.0f;
    
    float leftColumnX = centerX - 250;
    float rightColumnX = centerX + 50;
    float buttonWidth = 180;
    float buttonHeight = 40;
    float startY = 150;
    float spacing = 60;
    
    // Columna izquierda - Jugador Fuego
    fireLeftButton = { leftColumnX, startY, buttonWidth, buttonHeight };
    fireRightButton = { leftColumnX, startY + spacing, buttonWidth, buttonHeight };
    fireJumpButton = { leftColumnX, startY + spacing * 2, buttonWidth, buttonHeight };
    
    // Columna derecha - Jugador Agua
    waterLeftButton = { rightColumnX, startY, buttonWidth, buttonHeight };
    waterRightButton = { rightColumnX, startY + spacing, buttonWidth, buttonHeight };
    waterJumpButton = { rightColumnX, startY + spacing * 2, buttonWidth, buttonHeight };
    
    // Botones de acción
    saveButton = { centerX - 210, startY + spacing * 4, 200, 50 };
    cancelButton = { centerX + 10, startY + spacing * 4, 200, 50 };
}

void KeyBindingsState::handleInput() {
    Vector2 mouse = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, saveButton)) {
            AudioManager::getInstance()->playButtonClick();
            // TODO: Guardar cambios
            this->state_machine->remove_state(false);
        } else if (CheckCollisionPointRec(mouse, cancelButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->remove_state(false);
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
    }
}

void KeyBindingsState::update(float deltaTime) {
    Vector2 mousePos = GetMousePosition();
    
    mouseOverSaveButton = CheckCollisionPointRec(mousePos, saveButton);
    mouseOverCancelButton = CheckCollisionPointRec(mousePos, cancelButton);
    
    AudioManager* audioMgr = AudioManager::getInstance();
    
    if (mouseOverSaveButton && mouseExitedSaveButton) {
        audioMgr->playButtonHover();
        mouseExitedSaveButton = false;
    } else if (!mouseOverSaveButton) {
        mouseExitedSaveButton = true;
    }
    
    if (mouseOverCancelButton && mouseExitedCancelButton) {
        audioMgr->playButtonHover();
        mouseExitedCancelButton = false;
    } else if (!mouseOverCancelButton) {
        mouseExitedCancelButton = true;
    }
}

void KeyBindingsState::render() {
    BeginDrawing();
    ClearBackground(SKYBLUE);
    
    int screenWidth = GetScreenWidth();
    float centerX = screenWidth / 2.0f;
    
    // Título
    const char* title = "CONFIGURACION DE TECLAS";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, centerX - titleWidth / 2, 50, 40, BLACK);
    
    // Headers de columnas
    DrawText("JUGADOR FUEGO", fireLeftButton.x, 120, 20, RED);
    DrawText("JUGADOR AGUA", waterLeftButton.x, 120, 20, BLUE);
    
    // Función helper para dibujar botón de tecla
    auto drawKeyButton = [&](Rectangle button, const char* label) {
        DrawRectangleRec(button, LIGHTGRAY);
        DrawRectangleLinesEx(button, 2, BLACK);
        
        // Label (Izquierda/Derecha/Saltar)
        DrawText(label, button.x + 5, button.y + 5, 16, DARKGRAY);
        
        // Placeholder para la tecla
        const char* keyName = "A"; // Placeholder
        int keyWidth = MeasureText(keyName, 20);
        DrawText(keyName, button.x + button.width - keyWidth - 10, button.y + 10, 20, BLACK);
    };
    
    // Dibujar botones de teclas - Fuego
    drawKeyButton(fireLeftButton, "Izquierda:");
    drawKeyButton(fireRightButton, "Derecha:");
    drawKeyButton(fireJumpButton, "Saltar:");
    
    // Dibujar botones de teclas - Agua
    drawKeyButton(waterLeftButton, "Izquierda:");
    drawKeyButton(waterRightButton, "Derecha:");
    drawKeyButton(waterJumpButton, "Saltar:");
    
    // Botón Guardar
    DrawRectangleRec(saveButton, mouseOverSaveButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(saveButton, 2, BLACK);
    const char* saveText = "GUARDAR";
    int saveWidth = MeasureText(saveText, 20);
    DrawText(saveText, saveButton.x + (saveButton.width - saveWidth) / 2, saveButton.y + 15, 20, BLACK);
    
    // Botón Cancelar
    DrawRectangleRec(cancelButton, mouseOverCancelButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(cancelButton, 2, BLACK);
    const char* cancelText = "CANCELAR";
    int cancelWidth = MeasureText(cancelText, 20);
    DrawText(cancelText, cancelButton.x + (cancelButton.width - cancelWidth) / 2, cancelButton.y + 15, 20, BLACK);
    
    // Ayuda
    const char* help = "Click en una tecla para cambiarla | ESC: Cancelar";
    int helpWidth = MeasureText(help, 16);
    DrawText(help, centerX - helpWidth / 2, GetScreenHeight() - 40, 16, BLACK);
    
    EndDrawing();
}