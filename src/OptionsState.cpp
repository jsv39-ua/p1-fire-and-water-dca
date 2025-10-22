#include "OptionsState.hpp"
#include "StateMachine.hpp"
#include <AudioManager.hpp>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

// Variables estáticas para las opciones
float OptionsState::volume = 0.5f;
OptionsState::Language OptionsState::currentLanguage = OptionsState::SPANISH;

OptionsState::OptionsState() : draggingVolume(false)
{
}

const char* getLanguageName(OptionsState::Language lang) {
    switch(lang) {
        case OptionsState::SPANISH: return "ESPAÑOL";
        case OptionsState::ENGLISH: return "ENGLISH (WIP)";
        default: return "ESPAÑOL";
    }
}

void OptionsState::init()
{
    // Configurar las posiciones de los elementos de la UI (centrados)
    backButton = { 300, 320, 200, 50 };
    languageButton = { 250, 150, 300, 50 };
    volumeSlider = { 250, 250, 300, 20 };
    volumeHandle = { 250 + (volume * 280), 245, 20, 30 };
}

void OptionsState::handleInput()
{
    Vector2 mouse = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, backButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->remove_state(false);
        }

        if (CheckCollisionPointRec(mouse, languageButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentLanguage = (Language)((currentLanguage + 1) % 2);
        }

        if (CheckCollisionPointRec(mouse, volumeHandle)) {
            draggingVolume = true;
        }
    }
    
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        draggingVolume = false;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
    }
}

void OptionsState::update(float deltaTime)
{
    Vector2 mousePos = GetMousePosition();

    if (draggingVolume) {
        Vector2 mouse = GetMousePosition();
        float x = mouse.x - volumeSlider.x;
        if (x < 0) x = 0;
        if (x > volumeSlider.width - volumeHandle.width) x = volumeSlider.width - volumeHandle.width;
        volume = x / (volumeSlider.width - volumeHandle.width);
        SetMasterVolume(volume);
        updateVolumeSlider();
    }

    if (draggingMusicVolume) {
        Vector2 mouse = GetMousePosition();
        float x = mouse.x - musicVolumeSlider.x;
        if (x < 0) x = 0;
        if (x > musicVolumeSlider.width - musicVolumeHandle.width) x = musicVolumeSlider.width - musicVolumeHandle.width;
        volume = x / (musicVolumeSlider.width - musicVolumeHandle.width);
        SetMusicVolume(backgroundMusic, volume);
        updateMusicVolumeSlider();
    }

    mouseOverBackButton = CheckCollisionPointRec(mousePos, backButton);
    mouseOverLanguageButton = CheckCollisionPointRec(mousePos, languageButton);
    mouseOverVolumeButton = CheckCollisionPointRec(mousePos, volumeSlider);

    // Lógica de hover para botón BACK
    if (mouseOverBackButton && mouseExitedBackButton) {
        AudioManager::getInstance()->playButtonHover();
        mouseExitedBackButton = false;
    } else if (!mouseOverBackButton) {
        mouseExitedBackButton = true;
    }

    // Lógica de hover para botón LANGUAGE
    if (mouseOverLanguageButton && mouseExitedLanguageButton) {
        AudioManager::getInstance()->playButtonHover();
        mouseExitedLanguageButton = false;
    } else if (!mouseOverLanguageButton) {
        mouseExitedLanguageButton = true;
    }
}

void OptionsState::updateVolumeSlider()
{
    volumeHandle.x = volumeSlider.x + (volume * (volumeSlider.width - volumeHandle.width));
}

// Funciones estáticas para acceder a las opciones desde otros estados

void OptionsState::render()
{
    BeginDrawing();

    
    ClearBackground(SKYBLUE);
    
    // Título
    DrawText("OPCIONES", 300, 50, 40, BLACK);
    
    Vector2 mouse = GetMousePosition();
    
    // Botón idioma
    DrawRectangleRec(languageButton, mouseOverLanguageButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(languageButton, 2, BLACK);
    const char* lang = getLanguageName(currentLanguage);
    DrawText(lang, 330, 165, 20, BLACK);
    
    // Volumen
    DrawText("VOLUMEN", 250, 220, 20, BLACK);
    
    // Slider
    DrawRectangleRec(volumeSlider, LIGHTGRAY);
    DrawRectangleLinesEx(volumeSlider, 2, BLACK);

    Rectangle fill = { volumeSlider.x, volumeSlider.y, volume * volumeSlider.width, volumeSlider.height };
    DrawRectangleRec(fill, DARKBLUE);


    DrawRectangleRec(volumeHandle, mouseOverVolumeButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(volumeHandle, 2, BLACK);
    
    char vol_text[16];
    sprintf(vol_text, "%.0f%%", volume * 100);
    DrawText(vol_text, 570, 255, 20, DARKGRAY);
    
    // Botón volver
    DrawRectangleRec(backButton, mouseOverBackButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(backButton, 2, BLACK);
    DrawText("VOLVER", 345, 337, 20, BLACK);
    
    // Ayuda
    DrawText("ESC: Volver", 305, 500, 16, BLACK);
    
    EndDrawing();
}

float OptionsState::getVolume() 
{
    return OptionsState::volume;
}

OptionsState::Language OptionsState::getLanguage() 
{
    return OptionsState::currentLanguage;
}