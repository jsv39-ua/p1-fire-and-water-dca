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
    // Obtener tamaño de pantalla
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Calcular posiciones centradas relativamente al tamaño de pantalla
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    
    // Configurar las posiciones de los elementos de la UI (centrados)
    languageButton = { centerX - 150, centerY - 210, 300, 50 };
    
    volumeSlider = { centerX - 150, centerY - 110, 300, 20 };
    volumeHandle = { centerX - 150 + (volume * 280), centerY - 115, 20, 30 };

    musicVolumeSlider = { centerX - 150, centerY - 10, 300, 20 };
    musicVolumeHandle = { centerX - 150 + (musicVolume * 280), centerY - 15, 20, 30 };
    
    backButton = { centerX - 100, centerY + 90, 200, 50 };
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

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float centerX = screenWidth / 2.0f;
    
    ClearBackground(SKYBLUE);
    
    // Título (centrado)
    const char* title = "OPCIONES";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, centerX - titleWidth / 2, 50, 40, BLACK);
    
    Vector2 mouse = GetMousePosition();
    
    // Botón idioma
    DrawRectangleRec(languageButton, mouseOverLanguageButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(languageButton, 2, BLACK);
    const char* lang = getLanguageName(currentLanguage);
    int langWidth = MeasureText(lang, 20);
    DrawText(lang, languageButton.x + (languageButton.width - langWidth) / 2, languageButton.y + 15, 20, BLACK);
    
    // Volumen
    DrawText("VOLUMEN", volumeSlider.x, volumeSlider.y - 30, 20, BLACK);
    
    // Slider
    DrawRectangleRec(volumeSlider, LIGHTGRAY);
    DrawRectangleLinesEx(volumeSlider, 2, BLACK);

    Rectangle fill = { volumeSlider.x, volumeSlider.y, volume * volumeSlider.width, volumeSlider.height };
    DrawRectangleRec(fill, DARKBLUE);


    DrawRectangleRec(volumeHandle, mouseOverVolumeButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(volumeHandle, 2, BLACK);
    
    char vol_text[16];
    sprintf(vol_text, "%.0f%%", volume * 100);
    DrawText(vol_text, volumeSlider.x + volumeSlider.width + 20, volumeSlider.y + 5, 20, DARKGRAY);


    // Slider música
    DrawText("VOLUMEN MÚSICA", musicVolumeSlider.x, musicVolumeSlider.y - 30, 20, BLACK);        
   
    DrawRectangleRec(musicVolumeSlider, LIGHTGRAY);
    DrawRectangleLinesEx(musicVolumeSlider, 2, BLACK);

    Rectangle musicFill = { musicVolumeSlider.x, musicVolumeSlider.y, musicVolume * musicVolumeSlider.width, musicVolumeSlider.height };
    DrawRectangleRec(musicFill, DARKBLUE);

    DrawRectangleRec(musicVolumeHandle, mouseOverMusicVolumeButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(musicVolumeHandle, 2, BLACK);

    char music_vol_text[16];
    sprintf(music_vol_text, "%.0f%%", musicVolume * 100);
    DrawText(music_vol_text, musicVolumeSlider.x + musicVolumeSlider.width + 20, musicVolumeSlider.y - 15, 20, DARKGRAY);

    // Botón volver
    DrawRectangleRec(backButton, mouseOverBackButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(backButton, 2, BLACK);
    const char* backText = "VOLVER";
    int backWidth = MeasureText(backText, 20);
    DrawText(backText, backButton.x + (backButton.width - backWidth) / 2, backButton.y + 15, 20, BLACK);
    
    // Ayuda (centrada en la parte inferior)
    const char* help = "ESC: Volver";
    int helpWidth = MeasureText(help, 16);
    DrawText(help, centerX - helpWidth / 2, screenHeight - 60, 16, BLACK);
    
    renderGlobalUI();
    
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