#include "OptionsState.hpp"
#include "StateMachine.hpp"
#include "KeyBindingsState.hpp"
#include <AudioManager.hpp>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

// Variables estáticas para las opciones
// Solo el idioma es estático, los volúmenes se obtienen de AudioManager
OptionsState::Language OptionsState::currentLanguage = OptionsState::SPANISH;

OptionsState::OptionsState() : 
    draggingMasterVolume(false), 
    draggingMusicVolume(false),
    draggingSfxVolume(false)
{
    // Los volúmenes (masterVolume, musicVolume, sfxVolume) se inicializarán
    // en init() obteniendo los valores actuales de AudioManager
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
    // Obtener volúmenes actuales de AudioManager
    AudioManager* audioMgr = AudioManager::getInstance();
    masterVolume = audioMgr->getMasterVolume();
    musicVolume = audioMgr->getMusicVolume();
    sfxVolume = audioMgr->getSfxVolume();
    
    // Obtener tamaño de pantalla
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Calcular posiciones centradas relativamente al tamaño de pantalla
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    
    // Espaciado vertical entre elementos
    float sliderSpacing = 80.0f;
    float startY = centerY - 200.0f;
    
    // Configurar botón de idioma
    languageButton = { centerX - 150, startY, 300, 50 };
    
    // Botón de asignar teclas
    keyBindingsButton = { centerX - 150, startY + 60, 300, 50 };
    
    // Slider de volumen general (master)
    float masterSliderY = startY + 130.0f;
    masterVolumeSlider = { centerX - 150, masterSliderY, 300, 20 };
    masterVolumeHandle = { centerX - 150 + (masterVolume * 280), masterSliderY - 5, 20, 30 };
    
    // Slider de volumen de música
    float musicSliderY = masterSliderY + sliderSpacing;
    musicVolumeSlider = { centerX - 150, musicSliderY, 300, 20 };
    musicVolumeHandle = { centerX - 150 + (musicVolume * 280), musicSliderY - 5, 20, 30 };
    
    // Slider de volumen de efectos (SFX)
    float sfxSliderY = musicSliderY + sliderSpacing;
    sfxVolumeSlider = { centerX - 150, sfxSliderY, 300, 20 };
    sfxVolumeHandle = { centerX - 150 + (sfxVolume * 280), sfxSliderY - 5, 20, 30 };
    
    // Botón volver (más abajo para acomodar el botón de teclas)
    backButton = { centerX - 100, sfxSliderY + 70, 200, 50 };
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

        if (CheckCollisionPointRec(mouse, keyBindingsButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->add_state(std::make_unique<KeyBindingsState>(), false);
        }

        // Detección de click en los handles de los sliders
        if (CheckCollisionPointRec(mouse, masterVolumeHandle)) {
            draggingMasterVolume = true;
        }
        if (CheckCollisionPointRec(mouse, musicVolumeHandle)) {
            draggingMusicVolume = true;
        }
        if (CheckCollisionPointRec(mouse, sfxVolumeHandle)) {
            draggingSfxVolume = true;
        }
    }
    
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        draggingMasterVolume = false;
        draggingMusicVolume = false;
        draggingSfxVolume = false;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
    }
}

void OptionsState::update(float deltaTime)
{
    Vector2 mousePos = GetMousePosition();
    AudioManager* audioMgr = AudioManager::getInstance();

    // Lógica de dragging para slider de volumen general (master)
    if (draggingMasterVolume) {
        Vector2 mouse = GetMousePosition();
        float x = mouse.x - masterVolumeSlider.x;
        if (x < 0) x = 0;
        if (x > masterVolumeSlider.width - masterVolumeHandle.width) x = masterVolumeSlider.width - masterVolumeHandle.width;
        masterVolume = x / (masterVolumeSlider.width - masterVolumeHandle.width);
        
        audioMgr->setMasterVolume(masterVolume);
        updateMasterVolumeSlider();
    }

    // Lógica de dragging para slider de volumen de música
    if (draggingMusicVolume) {
        Vector2 mouse = GetMousePosition();
        float x = mouse.x - musicVolumeSlider.x;
        if (x < 0) x = 0;
        if (x > musicVolumeSlider.width - musicVolumeHandle.width) x = musicVolumeSlider.width - musicVolumeHandle.width;
        musicVolume = x / (musicVolumeSlider.width - musicVolumeHandle.width);

        audioMgr->setMusicVolume(musicVolume);
        updateMusicVolumeSlider();
    }

    // Lógica de dragging para slider de volumen de efectos (SFX)
    if (draggingSfxVolume) {
        Vector2 mouse = GetMousePosition();
        float x = mouse.x - sfxVolumeSlider.x;
        if (x < 0) x = 0;
        if (x > sfxVolumeSlider.width - sfxVolumeHandle.width) x = sfxVolumeSlider.width - sfxVolumeHandle.width;
        sfxVolume = x / (sfxVolumeSlider.width - sfxVolumeHandle.width);

        audioMgr->setSfxVolume(sfxVolume);
        updateSfxVolumeSlider();
    }

    // Actualizar estados de mouse over
    mouseOverBackButton = CheckCollisionPointRec(mousePos, backButton);
    mouseOverLanguageButton = CheckCollisionPointRec(mousePos, languageButton);
    mouseOverKeyBindingsButton = CheckCollisionPointRec(mousePos, keyBindingsButton);
    mouseOverMasterVolumeButton = CheckCollisionPointRec(mousePos, masterVolumeSlider);
    mouseOverMusicVolumeButton = CheckCollisionPointRec(mousePos, musicVolumeSlider);
    mouseOverSfxVolumeButton = CheckCollisionPointRec(mousePos, sfxVolumeSlider);

    // Lógica de hover para botón BACK
    if (mouseOverBackButton && mouseExitedBackButton) {
        audioMgr->playButtonHover();
        mouseExitedBackButton = false;
    } else if (!mouseOverBackButton) {
        mouseExitedBackButton = true;
    }

    // Lógica de hover para botón LANGUAGE
    if (mouseOverLanguageButton && mouseExitedLanguageButton) {
        audioMgr->playButtonHover();
        mouseExitedLanguageButton = false;
    } else if (!mouseOverLanguageButton) {
        mouseExitedLanguageButton = true;
    }

    // Lógica de hover para botón KEY BINDINGS
    if (mouseOverKeyBindingsButton && mouseExitedKeyBindingsButton) {
        audioMgr->playButtonHover();
        mouseExitedKeyBindingsButton = false;
    } else if (!mouseOverKeyBindingsButton) {
        mouseExitedKeyBindingsButton = true;
    }
}

void OptionsState::updateMasterVolumeSlider()
{
    masterVolumeHandle.x = masterVolumeSlider.x + (masterVolume * (masterVolumeSlider.width - masterVolumeHandle.width));
}

void OptionsState::updateMusicVolumeSlider()
{
    musicVolumeHandle.x = musicVolumeSlider.x + (musicVolume * (musicVolumeSlider.width - musicVolumeHandle.width));
}

void OptionsState::updateSfxVolumeSlider()
{
    sfxVolumeHandle.x = sfxVolumeSlider.x + (sfxVolume * (sfxVolumeSlider.width - sfxVolumeHandle.width));
}

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
    
    // Botón asignar teclas
    DrawRectangleRec(keyBindingsButton, mouseOverKeyBindingsButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(keyBindingsButton, 2, BLACK);
    const char* keyBindText = "ASIGNAR TECLAS";
    int keyBindWidth = MeasureText(keyBindText, 20);
    DrawText(keyBindText, keyBindingsButton.x + (keyBindingsButton.width - keyBindWidth) / 2, keyBindingsButton.y + 15, 20, BLACK);
    
    // slider volumen general
    DrawText("VOLUMEN GENERAL", masterVolumeSlider.x, masterVolumeSlider.y - 30, 20, BLACK);
    
    // Barra del slider
    DrawRectangleRec(masterVolumeSlider, LIGHTGRAY);
    DrawRectangleLinesEx(masterVolumeSlider, 2, BLACK);

    // Fill proporcional
    Rectangle masterFill = { masterVolumeSlider.x, masterVolumeSlider.y, masterVolume * masterVolumeSlider.width, masterVolumeSlider.height };
    DrawRectangleRec(masterFill, DARKBLUE);

    // Handle
    DrawRectangleRec(masterVolumeHandle, mouseOverMasterVolumeButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(masterVolumeHandle, 2, BLACK);
    
    // Porcentaje
    char master_vol_text[16];
    sprintf(master_vol_text, "%.0f%%", masterVolume * 100);
    DrawText(master_vol_text, masterVolumeSlider.x + masterVolumeSlider.width + 20, masterVolumeSlider.y + 5, 20, DARKGRAY);

    // slider volumen musica
    DrawText("VOLUMEN MUSICA", musicVolumeSlider.x, musicVolumeSlider.y - 30, 20, BLACK);        
   
    DrawRectangleRec(musicVolumeSlider, LIGHTGRAY);
    DrawRectangleLinesEx(musicVolumeSlider, 2, BLACK);

    Rectangle musicFill = { musicVolumeSlider.x, musicVolumeSlider.y, musicVolume * musicVolumeSlider.width, musicVolumeSlider.height };
    DrawRectangleRec(musicFill, DARKBLUE);

    DrawRectangleRec(musicVolumeHandle, mouseOverMusicVolumeButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(musicVolumeHandle, 2, BLACK);

    char music_vol_text[16];
    sprintf(music_vol_text, "%.0f%%", musicVolume * 100);
    DrawText(music_vol_text, musicVolumeSlider.x + musicVolumeSlider.width + 20, musicVolumeSlider.y + 5, 20, DARKGRAY);

    // slider volumen efectos
    DrawText("VOLUMEN EFECTOS", sfxVolumeSlider.x, sfxVolumeSlider.y - 30, 20, BLACK);
    
    DrawRectangleRec(sfxVolumeSlider, LIGHTGRAY);
    DrawRectangleLinesEx(sfxVolumeSlider, 2, BLACK);

    Rectangle sfxFill = { sfxVolumeSlider.x, sfxVolumeSlider.y, sfxVolume * sfxVolumeSlider.width, sfxVolumeSlider.height };
    DrawRectangleRec(sfxFill, DARKBLUE);

    DrawRectangleRec(sfxVolumeHandle, mouseOverSfxVolumeButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(sfxVolumeHandle, 2, BLACK);

    char sfx_vol_text[16];
    sprintf(sfx_vol_text, "%.0f%%", sfxVolume * 100);
    DrawText(sfx_vol_text, sfxVolumeSlider.x + sfxVolumeSlider.width + 20, sfxVolumeSlider.y + 5, 20, DARKGRAY);

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

OptionsState::Language OptionsState::getLanguage() 
{
    return OptionsState::currentLanguage;
}