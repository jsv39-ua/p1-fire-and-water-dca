#pragma once

#include <GameState.hpp>
#include <SettingsManager.hpp>

extern "C" {
    #include <raylib.h>
}

class KeyBindingsState : public GameState
{
public:
    KeyBindingsState();
    ~KeyBindingsState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause() {};
    void resume() override;

private:
    // Estado de edición
    enum EditingKey {
        NONE,
        FIRE_LEFT,
        FIRE_RIGHT,
        FIRE_JUMP,
        FIRE_ATTACK,
        WATER_LEFT,
        WATER_RIGHT,
        WATER_JUMP,
        WATER_ATTACK
    };
    
    // Botones para cada tecla
    Rectangle fireLeftButton;
    Rectangle fireRightButton;
    Rectangle fireJumpButton;
    Rectangle fireAttackButton;
    
    Rectangle waterLeftButton;
    Rectangle waterRightButton;
    Rectangle waterJumpButton;
    Rectangle waterAttackButton;
    
    // Botones de acción
    Rectangle saveButton;
    Rectangle cancelButton;
    
    EditingKey currentlyEditing;
    
    // Teclas temporales (se guardan al presionar "Guardar")
    int tempFireLeft, tempFireRight, tempFireJump, tempFireAttack;
    int tempWaterLeft, tempWaterRight, tempWaterJump, tempWaterAttack;
    
    // Mouse hover states
    bool mouseOverSaveButton = false;
    bool mouseOverCancelButton = false;
    bool mouseExitedSaveButton = true;
    bool mouseExitedCancelButton = true;
    
    // Hover states para botones de teclas
    bool mouseOverFireLeft = false;
    bool mouseOverFireRight = false;
    bool mouseOverFireJump = false;
    bool mouseOverFireAttack = false;
    bool mouseOverWaterLeft = false;
    bool mouseOverWaterRight = false;
    bool mouseOverWaterJump = false;
    bool mouseOverWaterAttack = false;
    
    bool mouseExitedFireLeft = true;
    bool mouseExitedFireRight = true;
    bool mouseExitedFireJump = true;
    bool mouseExitedFireAttack = true;
    bool mouseExitedWaterLeft = true;
    bool mouseExitedWaterRight = true;
    bool mouseExitedWaterJump = true;
    bool mouseExitedWaterAttack = true;
    
    // Estado de mensaje de confirmación
    bool showSaveConfirmation = false;
    float saveConfirmationTimer = 0.0f;
    
    // Funciones privadas
    void saveKeys();
    void cancelChanges();
    bool isKeyInUse(int key, EditingKey excludeThis);
};