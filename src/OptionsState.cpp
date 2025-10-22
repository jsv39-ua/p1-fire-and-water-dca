#include "OptionsState.hpp"
#include "StateMachine.hpp"
#include <AudioManager.hpp>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

// Variables estáticas para las opciones
float OptionsState::volume = 0.5f;
float OptionsState::musicVolume = getMusicVolumeFromAudioManager();
OptionsState::Language OptionsState::currentLanguage = OptionsState::SPANISH;

OptionsState::OptionsState() : draggingVolume(false), draggingMusicVolume(false)
{
}

float OptionsState::getMusicVolumeFromAudioManager() {
    return AudioManager::getInstance()->getMusicVolume();
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
    backButton = { 300, 400, 200, 50 };
    languageButton = { 250, 150, 300, 50 };
    volumeSlider = { 250, 250, 300, 20 };
    volumeHandle = { 250 + (volume * 280), 245, 20, 30 };

    musicVolumeSlider = { 250, 350, 300, 20 };
    musicVolumeHandle = { 250 + (musicVolume * 280), 345, 20, 30 };
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
        if (CheckCollisionPointRec(mouse, musicVolumeHandle)) {
            draggingMusicVolume = true;
        }
    }
    
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        draggingVolume = false;
        draggingMusicVolume = false;
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
        musicVolume = x / (musicVolumeSlider.width - musicVolumeHandle.width);

        AudioManager::getInstance()->setMusicVolume(musicVolume);
        updateMusicVolumeSlider();
    }

    mouseOverBackButton = CheckCollisionPointRec(mousePos, backButton);
    mouseOverLanguageButton = CheckCollisionPointRec(mousePos, languageButton);
    mouseOverVolumeButton = CheckCollisionPointRec(mousePos, volumeSlider);
    mouseOverMusicVolumeButton = CheckCollisionPointRec(mousePos, musicVolumeSlider);

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

void OptionsState::updateMusicVolumeSlider()
{
    musicVolumeHandle.x = musicVolumeSlider.x + (musicVolume * (musicVolumeSlider.width - musicVolumeHandle.width));
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


    // Slider música
    DrawText("VOLUMEN MÚSICA", 250, 320, 20, BLACK);        
   
    DrawRectangleRec(musicVolumeSlider, LIGHTGRAY);
    DrawRectangleLinesEx(musicVolumeSlider, 2, BLACK);

    Rectangle musicFill = { musicVolumeSlider.x, musicVolumeSlider.y, musicVolume * musicVolumeSlider.width, musicVolumeSlider.height };
    DrawRectangleRec(musicFill, DARKBLUE);

    DrawRectangleRec(musicVolumeHandle, mouseOverMusicVolumeButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(musicVolumeHandle, 2, BLACK);

    char music_vol_text[16];
    sprintf(music_vol_text, "%.0f%%", musicVolume * 100);
    DrawText(music_vol_text, 570, 335, 20, DARKGRAY);

    // Botón volver
    DrawRectangleRec(backButton, mouseOverBackButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(backButton, 2, BLACK);
    DrawText("VOLVER", 345, 337+80, 20, BLACK);
    
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