#include "KeyBindingsState.hpp"
#include "StateMachine.hpp"
#include "AudioManager.hpp"
#include <cctype>
#include <string>

extern "C" {
    #include <raylib.h>
}

// Función helper para obtener nombre de tecla (incluyendo especiales)
const char* getKeyDisplayName(int key) {
    // Primero intentar con GetKeyName
    const char* name = GetKeyName(key);
    if (name != nullptr && name[0] != '\0') {
        return name;
    }
    
    // Si falla, manejar teclas especiales manualmente
    switch(key) {
        case KEY_LEFT: return "LEFT";
        case KEY_RIGHT: return "RIGHT";
        case KEY_UP: return "UP";
        case KEY_DOWN: return "DOWN";
        case KEY_SPACE: return "SPACE";
        case KEY_ENTER: return "ENTER";
        case KEY_TAB: return "TAB";
        case KEY_BACKSPACE: return "BACKSPACE";
        case KEY_LEFT_SHIFT: return "L.SHIFT";
        case KEY_RIGHT_SHIFT: return "R.SHIFT";
        case KEY_LEFT_CONTROL: return "L.CTRL";
        case KEY_RIGHT_CONTROL: return "R.CTRL";
        case KEY_LEFT_ALT: return "L.ALT";
        case KEY_RIGHT_ALT: return "R.ALT";
        case 0: return "---";
        default: return "???";
    }
}

KeyBindingsState::KeyBindingsState() : currentlyEditing(NONE)
{
}

void KeyBindingsState::init() {
    // Cargar teclas actuales desde SettingsManager
    SettingsManager* settings = SettingsManager::getInstance();
    
    tempFireLeft = settings->getFireLeft();
    tempFireRight = settings->getFireRight();
    tempFireJump = settings->getFireJump();
    
    tempWaterLeft = settings->getWaterLeft();
    tempWaterRight = settings->getWaterRight();
    tempWaterJump = settings->getWaterJump();
    
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
    // Si estamos editando una tecla, capturar la nueva tecla
    if (currentlyEditing != NONE) {
        int key = GetKeyPressed();
        if (key != 0) {
            AudioManager::getInstance()->playButtonClick();
            
            switch(currentlyEditing) {
                case FIRE_LEFT: tempFireLeft = key; break;
                case FIRE_RIGHT: tempFireRight = key; break;
                case FIRE_JUMP: tempFireJump = key; break;
                case WATER_LEFT: tempWaterLeft = key; break;
                case WATER_RIGHT: tempWaterRight = key; break;
                case WATER_JUMP: tempWaterJump = key; break;
                default: break;
            }
            
            currentlyEditing = NONE;
        }
        return;
    }
    
    Vector2 mouse = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, saveButton)) {
            AudioManager::getInstance()->playButtonClick();
            saveKeys();
            this->state_machine->remove_state(false);
        } else if (CheckCollisionPointRec(mouse, cancelButton)) {
            AudioManager::getInstance()->playButtonClick();
            cancelChanges();
            this->state_machine->remove_state(false);
        }
        // Detectar click en botones de teclas
        else if (CheckCollisionPointRec(mouse, fireLeftButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = FIRE_LEFT;
        } else if (CheckCollisionPointRec(mouse, fireRightButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = FIRE_RIGHT;
        } else if (CheckCollisionPointRec(mouse, fireJumpButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = FIRE_JUMP;
        } else if (CheckCollisionPointRec(mouse, waterLeftButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = WATER_LEFT;
        } else if (CheckCollisionPointRec(mouse, waterRightButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = WATER_RIGHT;
        } else if (CheckCollisionPointRec(mouse, waterJumpButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = WATER_JUMP;
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        cancelChanges();
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
    auto drawKeyButton = [&](Rectangle button, const char* label, int key, bool isEditing) {
        Color bgColor = isEditing ? YELLOW : LIGHTGRAY;
        DrawRectangleRec(button, bgColor);
        DrawRectangleLinesEx(button, 2, BLACK);
        
        // Label (Izquierda/Derecha/Saltar)
        DrawText(label, button.x + 5, button.y + 5, 16, DARKGRAY);

        // Obtener el nombre de la tecla
        std::string upperKeyName;
        
        if (isEditing) {
            upperKeyName = "...";
        } else {
            const char* keyName = getKeyDisplayName(key);
            upperKeyName = keyName;
            // Convertir a mayúsculas para mejor visualización
            for (char& c : upperKeyName) {
                c = std::toupper(c);
            }
        }
        
        int keyWidth = MeasureText(upperKeyName.c_str(), 20);
        DrawText(upperKeyName.c_str(), button.x + button.width - keyWidth - 10, button.y + 10, 20, BLACK);
    };
    
    // Dibujar botones de teclas - Fuego
    drawKeyButton(fireLeftButton, "Izquierda:", tempFireLeft, currentlyEditing == FIRE_LEFT);
    drawKeyButton(fireRightButton, "Derecha:", tempFireRight, currentlyEditing == FIRE_RIGHT);
    drawKeyButton(fireJumpButton, "Saltar:", tempFireJump, currentlyEditing == FIRE_JUMP);
    
    // Dibujar botones de teclas - Agua
    drawKeyButton(waterLeftButton, "Izquierda:", tempWaterLeft, currentlyEditing == WATER_LEFT);
    drawKeyButton(waterRightButton, "Derecha:", tempWaterRight, currentlyEditing == WATER_RIGHT);
    drawKeyButton(waterJumpButton, "Saltar:", tempWaterJump, currentlyEditing == WATER_JUMP);
    
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
    const char* help = currentlyEditing != NONE ? 
        "Presiona una tecla..." : 
        "Click en una tecla para cambiarla | ESC: Cancelar";
    int helpWidth = MeasureText(help, 16);
    DrawText(help, centerX - helpWidth / 2, GetScreenHeight() - 40, 16, BLACK);
    
    EndDrawing();
}

void KeyBindingsState::resume() {
    // Recargar teclas al volver
    SettingsManager* settings = SettingsManager::getInstance();
    tempFireLeft = settings->getFireLeft();
    tempFireRight = settings->getFireRight();
    tempFireJump = settings->getFireJump();
    tempWaterLeft = settings->getWaterLeft();
    tempWaterRight = settings->getWaterRight();
    tempWaterJump = settings->getWaterJump();
}

void KeyBindingsState::saveKeys() {
    SettingsManager* settings = SettingsManager::getInstance();
    settings->setFireKeys(tempFireLeft, tempFireRight, tempFireJump);
    settings->setWaterKeys(tempWaterLeft, tempWaterRight, tempWaterJump);
    settings->saveSettings();
}

void KeyBindingsState::cancelChanges() {
    // No hacer nada, las teclas temporales se descartan
}