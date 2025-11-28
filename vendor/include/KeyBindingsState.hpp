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
        WATER_LEFT,
        WATER_RIGHT,
        WATER_JUMP
    };
    
    // Botones para cada tecla
    Rectangle fireLeftButton;
    Rectangle fireRightButton;
    Rectangle fireJumpButton;
    
    Rectangle waterLeftButton;
    Rectangle waterRightButton;
    Rectangle waterJumpButton;
    
    // Botones de acción
    Rectangle saveButton;
    Rectangle cancelButton;
    
    EditingKey currentlyEditing;
    
    // Teclas temporales (se guardan al presionar "Guardar")
    int tempFireLeft, tempFireRight, tempFireJump;
    int tempWaterLeft, tempWaterRight, tempWaterJump;
    
    // Mouse hover states
    bool mouseOverSaveButton = false;
    bool mouseOverCancelButton = false;
    bool mouseExitedSaveButton = true;
    bool mouseExitedCancelButton = true;
    
    // Hover states para botones de teclas
    bool mouseOverFireLeft = false;
    bool mouseOverFireRight = false;
    bool mouseOverFireJump = false;
    bool mouseOverWaterLeft = false;
    bool mouseOverWaterRight = false;
    bool mouseOverWaterJump = false;
    
    bool mouseExitedFireLeft = true;
    bool mouseExitedFireRight = true;
    bool mouseExitedFireJump = true;
    bool mouseExitedWaterLeft = true;
    bool mouseExitedWaterRight = true;
    bool mouseExitedWaterJump = true;
    
    // Estado de mensaje de confirmación
    bool showSaveConfirmation = false;
    float saveConfirmationTimer = 0.0f;
    
    // Funciones privadas
    void saveKeys();
    void cancelChanges();
    bool isKeyInUse(int key, EditingKey excludeThis);
};