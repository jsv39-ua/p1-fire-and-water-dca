#include "states/KeyBindingsState.hpp"
#include "states/StateMachine.hpp"
#include "systems/AudioManager.hpp"
#include <cctype>
#include <string>
#include <raylib.h>
#include "systems/LocalizationManager.hpp"
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

KeyBindingsState::KeyBindingsState() : 
    currentlyEditing(NONE),
    showSaveConfirmation(false),
    saveConfirmationTimer(0.0f)
{
}

void KeyBindingsState::init() {
    // Cargar teclas actuales desde SettingsManager
    SettingsManager* settings = SettingsManager::getInstance();
    
    tempFireLeft = settings->getFireLeft();
    tempFireRight = settings->getFireRight();
    tempFireJump = settings->getFireJump();
    tempFireAttack = settings->getFireAttack();
    
    tempWaterLeft = settings->getWaterLeft();
    tempWaterRight = settings->getWaterRight();
    tempWaterJump = settings->getWaterJump();
    tempWaterAttack = settings->getWaterAttack();
    
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
    fireAttackButton = { leftColumnX, startY + spacing * 3, buttonWidth, buttonHeight };
    
    // Columna derecha - Jugador Agua
    waterLeftButton = { rightColumnX, startY, buttonWidth, buttonHeight };
    waterRightButton = { rightColumnX, startY + spacing, buttonWidth, buttonHeight };
    waterJumpButton = { rightColumnX, startY + spacing * 2, buttonWidth, buttonHeight };
    waterAttackButton = { rightColumnX, startY + spacing * 3, buttonWidth, buttonHeight };
    
    // Botones de acción
    saveButton = { centerX - 210, startY + spacing * 5, 200, 50 };
    cancelButton = { centerX + 10, startY + spacing * 5, 200, 50 };
}

// Función helper para verificar si una tecla ya está en uso
bool KeyBindingsState::isKeyInUse(int key, EditingKey excludeThis) {
    if (key == 0) return false; // Tecla no asignada no cuenta como "en uso"
    
    if (excludeThis != FIRE_LEFT && key == tempFireLeft) return true;
    if (excludeThis != FIRE_RIGHT && key == tempFireRight) return true;
    if (excludeThis != FIRE_JUMP && key == tempFireJump) return true;
    if (excludeThis != FIRE_ATTACK && key == tempFireAttack) return true;
    if (excludeThis != WATER_LEFT && key == tempWaterLeft) return true;
    if (excludeThis != WATER_RIGHT && key == tempWaterRight) return true;
    if (excludeThis != WATER_JUMP && key == tempWaterJump) return true;
    if (excludeThis != WATER_ATTACK && key == tempWaterAttack) return true;
    
    return false;
}

void KeyBindingsState::handleInput() {
    // Si estamos editando una tecla, capturar la nueva tecla
    if (currentlyEditing != NONE) {
        // Permitir cancelar la edición con ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = NONE;
            return;
        }
        
        // Si se hace click en Guardar o Cancelar mientras se edita, cancelar la edición
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, saveButton) || CheckCollisionPointRec(mouse, cancelButton)) {
                AudioManager::getInstance()->playButtonClick();
                currentlyEditing = NONE;
                return;
            }
        }
        
        int key = GetKeyPressed();
        if (key != 0) {
            // Verificar si la tecla ya está en uso
            if (isKeyInUse(key, currentlyEditing)) {
                // Reproducir sonido de error y no asignar la tecla
                AudioManager::getInstance()->playButtonClick();
                TraceLog(LOG_WARNING, "Key already in use!");
            } else {
                AudioManager::getInstance()->playButtonClick();
                
                switch(currentlyEditing) {
                    case FIRE_LEFT: tempFireLeft = key; break;
                    case FIRE_RIGHT: tempFireRight = key; break;
                    case FIRE_JUMP: tempFireJump = key; break;
                    case FIRE_ATTACK: tempFireAttack = key; break;
                    case WATER_LEFT: tempWaterLeft = key; break;
                    case WATER_RIGHT: tempWaterRight = key; break;
                    case WATER_JUMP: tempWaterJump = key; break;
                    case WATER_ATTACK: tempWaterAttack = key; break;
                    default: break;
                }
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
            showSaveConfirmation = true;
            saveConfirmationTimer = 2.0f; // Mostrar mensaje por 2 segundos
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
        } else if (CheckCollisionPointRec(mouse, fireAttackButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = FIRE_ATTACK;
        } else if (CheckCollisionPointRec(mouse, waterLeftButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = WATER_LEFT;
        } else if (CheckCollisionPointRec(mouse, waterRightButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = WATER_RIGHT;
        } else if (CheckCollisionPointRec(mouse, waterJumpButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = WATER_JUMP;
        } else if (CheckCollisionPointRec(mouse, waterAttackButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentlyEditing = WATER_ATTACK;
        }
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        cancelChanges();
        this->state_machine->remove_state(false);
    }
}

void KeyBindingsState::update(float deltaTime) {
    Vector2 mousePos = GetMousePosition();
    
    // Actualizar timer de confirmación
    if (showSaveConfirmation) {
        saveConfirmationTimer -= deltaTime;
        if (saveConfirmationTimer <= 0.0f) {
            showSaveConfirmation = false;
            // Volver al estado anterior después de mostrar el mensaje
            this->state_machine->remove_state(false);
        }
    }
    
    // Actualizar estados de hover para botones de acción
    mouseOverSaveButton = CheckCollisionPointRec(mousePos, saveButton);
    mouseOverCancelButton = CheckCollisionPointRec(mousePos, cancelButton);
    
    // Actualizar estados de hover para botones de teclas
    mouseOverFireLeft = CheckCollisionPointRec(mousePos, fireLeftButton);
    mouseOverFireRight = CheckCollisionPointRec(mousePos, fireRightButton);
    mouseOverFireJump = CheckCollisionPointRec(mousePos, fireJumpButton);
    mouseOverFireAttack = CheckCollisionPointRec(mousePos, fireAttackButton);
    mouseOverWaterLeft = CheckCollisionPointRec(mousePos, waterLeftButton);
    mouseOverWaterRight = CheckCollisionPointRec(mousePos, waterRightButton);
    mouseOverWaterJump = CheckCollisionPointRec(mousePos, waterJumpButton);
    mouseOverWaterAttack = CheckCollisionPointRec(mousePos, waterAttackButton);
    
    AudioManager* audioMgr = AudioManager::getInstance();
    
    // Hover para botón Guardar
    if (mouseOverSaveButton && mouseExitedSaveButton) {
        audioMgr->playButtonHover();
        mouseExitedSaveButton = false;
    } else if (!mouseOverSaveButton) {
        mouseExitedSaveButton = true;
    }
    
    // Hover para botón Cancelar
    if (mouseOverCancelButton && mouseExitedCancelButton) {
        audioMgr->playButtonHover();
        mouseExitedCancelButton = false;
    } else if (!mouseOverCancelButton) {
        mouseExitedCancelButton = true;
    }
    
    // Hover para botones de teclas - Fire
    if (mouseOverFireLeft && mouseExitedFireLeft) {
        audioMgr->playButtonHover();
        mouseExitedFireLeft = false;
    } else if (!mouseOverFireLeft) {
        mouseExitedFireLeft = true;
    }
    
    if (mouseOverFireRight && mouseExitedFireRight) {
        audioMgr->playButtonHover();
        mouseExitedFireRight = false;
    } else if (!mouseOverFireRight) {
        mouseExitedFireRight = true;
    }
    
    if (mouseOverFireJump && mouseExitedFireJump) {
        audioMgr->playButtonHover();
        mouseExitedFireJump = false;
    } else if (!mouseOverFireJump) {
        mouseExitedFireJump = true;
    }
    
    // Hover para botones de teclas - Water
    if (mouseOverWaterLeft && mouseExitedWaterLeft) {
        audioMgr->playButtonHover();
        mouseExitedWaterLeft = false;
    } else if (!mouseOverWaterLeft) {
        mouseExitedWaterLeft = true;
    }
    
    if (mouseOverWaterRight && mouseExitedWaterRight) {
        audioMgr->playButtonHover();
        mouseExitedWaterRight = false;
    } else if (!mouseOverWaterRight) {
        mouseExitedWaterRight = true;
    }
    
    if (mouseOverWaterJump && mouseExitedWaterJump) {
        audioMgr->playButtonHover();
        mouseExitedWaterJump = false;
    } else if (!mouseOverWaterJump) {
        mouseExitedWaterJump = true;
    }
    
    if (mouseOverFireAttack && mouseExitedFireAttack) {
        audioMgr->playButtonHover();
        mouseExitedFireAttack = false;
    } else if (!mouseOverFireAttack) {
        mouseExitedFireAttack = true;
    }
    
    if (mouseOverWaterAttack && mouseExitedWaterAttack) {
        audioMgr->playButtonHover();
        mouseExitedWaterAttack = false;
    } else if (!mouseOverWaterAttack) {
        mouseExitedWaterAttack = true;
    }
}

void KeyBindingsState::render() {
    BeginDrawing();
    ClearBackground(SKYBLUE);
    
    int screenWidth = GetScreenWidth();
    float centerX = screenWidth / 2.0f;
    
    // Título
    const char* title = _("CONFIGURACION DE TECLAS");
    int titleWidth = MeasureText(title, 40);
    DrawText(title, centerX - titleWidth / 2, 50, 40, BLACK);
    
    // Headers de columnas
    DrawText(_("JUGADOR FUEGO"), fireLeftButton.x, 120, 20, RED);
    DrawText(_("JUGADOR AGUA"), waterLeftButton.x, 120, 20, BLUE);
    
    // Función helper para dibujar botón de tecla
    auto drawKeyButton = [&](Rectangle button, const char* label, int key, bool isEditing, bool isDuplicate, bool isHovered) {
        Color bgColor;
        if (isEditing) {
            bgColor = YELLOW;
        } else if (isDuplicate) {
            bgColor = Color{ 255, 100, 100, 255 }; // Rojo claro para indicar duplicado
        } else if (isHovered) {
            bgColor = GRAY; // Color de hover
        } else {
            bgColor = LIGHTGRAY;
        }
        
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
    
    // Verificar duplicados para cada tecla
    bool fireLeftDup = isKeyInUse(tempFireLeft, FIRE_LEFT);
    bool fireRightDup = isKeyInUse(tempFireRight, FIRE_RIGHT);
    bool fireJumpDup = isKeyInUse(tempFireJump, FIRE_JUMP);
    bool fireAttackDup = isKeyInUse(tempFireAttack, FIRE_ATTACK);
    bool waterLeftDup = isKeyInUse(tempWaterLeft, WATER_LEFT);
    bool waterRightDup = isKeyInUse(tempWaterRight, WATER_RIGHT);
    bool waterJumpDup = isKeyInUse(tempWaterJump, WATER_JUMP);
    bool waterAttackDup = isKeyInUse(tempWaterAttack, WATER_ATTACK);
    
    // Dibujar botones de teclas - Fuego
    drawKeyButton(fireLeftButton, _("Izquierda:"), tempFireLeft, currentlyEditing == FIRE_LEFT, fireLeftDup, mouseOverFireLeft);
    drawKeyButton(fireRightButton, _("Derecha:"), tempFireRight, currentlyEditing == FIRE_RIGHT, fireRightDup, mouseOverFireRight);
    drawKeyButton(fireJumpButton, _("Saltar:"), tempFireJump, currentlyEditing == FIRE_JUMP, fireJumpDup, mouseOverFireJump);
    drawKeyButton(fireAttackButton, _("Ataque:"), tempFireAttack, currentlyEditing == FIRE_ATTACK, fireAttackDup, mouseOverFireAttack);
    
    // Dibujar botones de teclas - Agua
    drawKeyButton(waterLeftButton, _("Izquierda:"), tempWaterLeft, currentlyEditing == WATER_LEFT, waterLeftDup, mouseOverWaterLeft);
    drawKeyButton(waterRightButton, _("Derecha:"), tempWaterRight, currentlyEditing == WATER_RIGHT, waterRightDup, mouseOverWaterRight);
    drawKeyButton(waterJumpButton, _("Saltar:"), tempWaterJump, currentlyEditing == WATER_JUMP, waterJumpDup, mouseOverWaterJump);
    drawKeyButton(waterAttackButton, _("Ataque:"), tempWaterAttack, currentlyEditing == WATER_ATTACK, waterAttackDup, mouseOverWaterAttack);
    
    // Botón Guardar
    DrawRectangleRec(saveButton, mouseOverSaveButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(saveButton, 2, BLACK);
    const char* saveText = _("GUARDAR");
    int saveWidth = MeasureText(saveText, 20);
    DrawText(saveText, saveButton.x + (saveButton.width - saveWidth) / 2, saveButton.y + 15, 20, BLACK);
    
    // Botón Cancelar
    DrawRectangleRec(cancelButton, mouseOverCancelButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(cancelButton, 2, BLACK);
    const char* cancelText = _("CANCELAR");
    int cancelWidth = MeasureText(cancelText, 20);
    DrawText(cancelText, cancelButton.x + (cancelButton.width - cancelWidth) / 2, cancelButton.y + 15, 20, BLACK);
    
    // Ayuda
    const char* help = currentlyEditing != NONE ? 
        _("Presiona una tecla...") : 
        _("Click en una tecla para cambiarla | ESC: Cancelar");
    int helpWidth = MeasureText(help, 16);
    DrawText(help, centerX - helpWidth / 2, GetScreenHeight() - 40, 16, BLACK);
    
    // Advertencia si hay duplicados
    if (fireLeftDup || fireRightDup || fireJumpDup || waterLeftDup || waterRightDup || waterJumpDup) {
        const char* warning = _("¡Advertencia: Tecla duplicada!");
        int warningWidth = MeasureText(warning, 20);
        DrawText(warning, centerX - warningWidth / 2, GetScreenHeight() - 80, 20, RED);
    }
    
    // Mensaje de confirmación al guardar
    if (showSaveConfirmation) {
        // Fondo semi-transparente
        DrawRectangle(0, 0, screenWidth, GetScreenHeight(), Color{ 0, 0, 0, 150 });
        
        // Cuadro de confirmación
        Rectangle confirmBox = { centerX - 200, GetScreenHeight() / 2.0f - 60, 400, 120 };
        DrawRectangleRec(confirmBox, Color{ 50, 200, 50, 255 }); // Verde
        DrawRectangleLinesEx(confirmBox, 3, BLACK);
        
        const char* confirmText = _("¡CONFIGURACION GUARDADA!");
        int confirmWidth = MeasureText(confirmText, 25);
        DrawText(confirmText, centerX - confirmWidth / 2, confirmBox.y + 35, 25, WHITE);
        
        const char* subText = _("Volviendo...");
        int subWidth = MeasureText(subText, 18);
        DrawText(subText, centerX - subWidth / 2, confirmBox.y + 75, 18, WHITE);
    }
    
    EndDrawing();
}

void KeyBindingsState::resume() {
    // Recargar teclas al volver
    SettingsManager* settings = SettingsManager::getInstance();
    tempFireLeft = settings->getFireLeft();
    tempFireRight = settings->getFireRight();
    tempFireJump = settings->getFireJump();
    tempFireAttack = settings->getFireAttack();
    tempWaterLeft = settings->getWaterLeft();
    tempWaterRight = settings->getWaterRight();
    tempWaterJump = settings->getWaterJump();
    tempWaterAttack = settings->getWaterAttack();
}

void KeyBindingsState::saveKeys() {
    SettingsManager* settings = SettingsManager::getInstance();
    settings->setFireKeys(tempFireLeft, tempFireRight, tempFireJump, tempFireAttack);
    settings->setWaterKeys(tempWaterLeft, tempWaterRight, tempWaterJump, tempWaterAttack);
    settings->saveSettings();
}

void KeyBindingsState::cancelChanges() {
    // No hacer nada, las teclas temporales se descartan
}