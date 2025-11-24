#include "OptionsState.hpp"
#include "StateMachine.hpp"
#include "KeyBindingsState.hpp"
#include "MainGameState.hpp"
#include <AudioManager.hpp>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

// Variables estáticas para las opciones
OptionsState::Language OptionsState::currentLanguage = OptionsState::SPANISH;

OptionsState::OptionsState(Player& firePlayer, Player& waterPlayer) : 
    draggingMasterVolume(false), 
    draggingMusicVolume(false),
    draggingSfxVolume(false),
    firePlayer(&firePlayer),
    waterPlayer(&waterPlayer)
{
}

OptionsState::OptionsState() : 
    draggingMasterVolume(false), 
    draggingMusicVolume(false),
    draggingSfxVolume(false),
    firePlayer(new Player()),
    waterPlayer(new Player())
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
    // Obtener volúmenes actuales de AudioManager
    AudioManager* audioMgr = AudioManager::getInstance();
    masterVolume = audioMgr->getMasterVolume();
    musicVolume = audioMgr->getMusicVolume();
    sfxVolume = audioMgr->getSfxVolume();
    
    // Obtener tamaño de pantalla
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Calcular posiciones centradas
    float centerX = screenWidth / 2.0f;
    float startY = 100.0f;
    
    // Botón de idioma
    languageButton = { centerX - 150, startY, 300, 50 };
    
    // Botón de teclas asignadas (nuevo)
    keyBindingsButton = { centerX - 150, startY + 70, 300, 50 };
    
    // Sliders de volumen (ajustados hacia abajo)
    float sliderY = startY + 140.0f;
    float sliderWidth = 300.0f;
    float sliderHeight = 20.0f;
    float sliderSpacing = 80.0f;

    masterVolumeSlider = { centerX - sliderWidth / 2, sliderY, sliderWidth, sliderHeight };
    musicVolumeSlider = { centerX - sliderWidth / 2, sliderY + sliderSpacing, sliderWidth, sliderHeight };
    sfxVolumeSlider = { centerX - sliderWidth / 2, sliderY + sliderSpacing * 2, sliderWidth, sliderHeight };

    // Botón de volver
    backButton = { centerX - 100, sliderY + sliderSpacing * 3 + 20, 200, 50 };
}

void OptionsState::handleInput()
{
    Vector2 mouse = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, backButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->remove_state(false);
            return;
        }

        if (CheckCollisionPointRec(mouse, languageButton)) {
            AudioManager::getInstance()->playButtonClick();
            currentLanguage = (Language)((currentLanguage + 1) % 2);
        }
        
        // Botón de teclas asignadas - Usar desreferencia de punteros
        if (CheckCollisionPointRec(mouse, keyBindingsButton)) {
            AudioManager::getInstance()->playButtonClick();
            this->state_machine->add_state(
                std::make_unique<KeyBindingsState>(
                    *this->firePlayer,   // Desreferenciar puntero
                    *this->waterPlayer   // Desreferenciar puntero
                ), 
                false
            );
            return;
        }

        // Comprobar si se hace clic en algún slider
        if (CheckCollisionPointRec(mouse, masterVolumeSlider)) {
            draggingMasterVolume = true;
        }
        if (CheckCollisionPointRec(mouse, musicVolumeSlider)) {
            draggingMusicVolume = true;
        }
        if (CheckCollisionPointRec(mouse, sfxVolumeSlider)) {
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
    
    mouseOverLanguageButton = CheckCollisionPointRec(mousePos, languageButton);
    mouseOverKeyBindingsButton = CheckCollisionPointRec(mousePos, keyBindingsButton);  // Nuevo
    mouseOverBackButton = CheckCollisionPointRec(mousePos, backButton);
    
    // Hover effects
    AudioManager* audioMgr = AudioManager::getInstance();
    
    if (mouseOverLanguageButton && mouseExitedLanguageButton) {
        audioMgr->playButtonHover();
        mouseExitedLanguageButton = false;
    } else if (!mouseOverLanguageButton) {
        mouseExitedLanguageButton = true;
    }
    
    // Hover para botón de teclas (nuevo)
    if (mouseOverKeyBindingsButton && mouseExitedKeyBindingsButton) {
        audioMgr->playButtonHover();
        mouseExitedKeyBindingsButton = false;
    } else if (!mouseOverKeyBindingsButton) {
        mouseExitedKeyBindingsButton = true;
    }
    
    if (mouseOverBackButton && mouseExitedBackButton) {
        audioMgr->playButtonHover();
        mouseExitedBackButton = false;
    } else if (!mouseOverBackButton) {
        mouseExitedBackButton = true;
    }

    // Actualizar sliders si se están arrastrando
    if (draggingMasterVolume) {
        updateMasterVolumeSlider();
    }
    if (draggingMusicVolume) {
        updateMusicVolumeSlider();
    }
    if (draggingSfxVolume) {
        updateSfxVolumeSlider();
    }
}

void OptionsState::updateMasterVolumeSlider()
{
    Vector2 mousePos = GetMousePosition();
    float relativeX = mousePos.x - masterVolumeSlider.x;
    masterVolume = relativeX / masterVolumeSlider.width;
    masterVolume = (masterVolume < 0.0f) ? 0.0f : (masterVolume > 1.0f) ? 1.0f : masterVolume;
    
    AudioManager::getInstance()->setMasterVolume(masterVolume);
}

void OptionsState::updateMusicVolumeSlider()
{
    Vector2 mousePos = GetMousePosition();
    float relativeX = mousePos.x - musicVolumeSlider.x;
    musicVolume = relativeX / musicVolumeSlider.width;
    musicVolume = (musicVolume < 0.0f) ? 0.0f : (musicVolume > 1.0f) ? 1.0f : musicVolume;
    
    AudioManager::getInstance()->setMusicVolume(musicVolume);
}

void OptionsState::updateSfxVolumeSlider()
{
    Vector2 mousePos = GetMousePosition();
    float relativeX = mousePos.x - sfxVolumeSlider.x;
    sfxVolume = relativeX / sfxVolumeSlider.width;
    sfxVolume = (sfxVolume < 0.0f) ? 0.0f : (sfxVolume > 1.0f) ? 1.0f : sfxVolume;
    
    AudioManager::getInstance()->setSfxVolume(sfxVolume);
}

void OptionsState::render()
{
    BeginDrawing();
    ClearBackground(SKYBLUE);

    int screenWidth = GetScreenWidth();
    float centerX = screenWidth / 2.0f;

    // Título
    const char* title = "OPCIONES";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, centerX - titleWidth / 2, 20, 60, BLACK);

    // Botón de idioma
    DrawRectangleRec(languageButton, mouseOverLanguageButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(languageButton, 2, BLACK);
    const char* langText = TextFormat("IDIOMA: %s", getLanguageName(currentLanguage));
    int langWidth = MeasureText(langText, 20);
    DrawText(langText, languageButton.x + (languageButton.width - langWidth) / 2, languageButton.y + 15, 20, BLACK);

    // Botón de teclas asignadas (nuevo)
    DrawRectangleRec(keyBindingsButton, mouseOverKeyBindingsButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(keyBindingsButton, 2, BLACK);
    const char* keyText = "TECLAS ASIGNADAS";
    int keyWidth = MeasureText(keyText, 20);
    DrawText(keyText, keyBindingsButton.x + (keyBindingsButton.width - keyWidth) / 2, keyBindingsButton.y + 15, 20, BLACK);

    // Etiquetas de volumen
    DrawText("VOLUMEN MAESTRO", masterVolumeSlider.x, masterVolumeSlider.y - 25, 20, BLACK);
    DrawText("VOLUMEN MUSICA", musicVolumeSlider.x, musicVolumeSlider.y - 25, 20, BLACK);
    DrawText("VOLUMEN SFX", sfxVolumeSlider.x, sfxVolumeSlider.y - 25, 20, BLACK);

    // Dibujar sliders
    DrawRectangleRec(masterVolumeSlider, LIGHTGRAY);
    DrawRectangle(masterVolumeSlider.x, masterVolumeSlider.y, 
                  masterVolumeSlider.width * masterVolume, masterVolumeSlider.height, DARKGRAY);
    DrawRectangleLinesEx(masterVolumeSlider, 2, BLACK);

    DrawRectangleRec(musicVolumeSlider, LIGHTGRAY);
    DrawRectangle(musicVolumeSlider.x, musicVolumeSlider.y, 
                  musicVolumeSlider.width * musicVolume, musicVolumeSlider.height, DARKGRAY);
    DrawRectangleLinesEx(musicVolumeSlider, 2, BLACK);

    DrawRectangleRec(sfxVolumeSlider, LIGHTGRAY);
    DrawRectangle(sfxVolumeSlider.x, sfxVolumeSlider.y, 
                  sfxVolumeSlider.width * sfxVolume, sfxVolumeSlider.height, DARKGRAY);
    DrawRectangleLinesEx(sfxVolumeSlider, 2, BLACK);

    // Porcentajes de volumen
    DrawText(TextFormat("%d%%", (int)(masterVolume * 100)), masterVolumeSlider.x + masterVolumeSlider.width + 10, masterVolumeSlider.y, 20, BLACK);
    DrawText(TextFormat("%d%%", (int)(musicVolume * 100)), musicVolumeSlider.x + musicVolumeSlider.width + 10, musicVolumeSlider.y, 20, BLACK);
    DrawText(TextFormat("%d%%", (int)(sfxVolume * 100)), sfxVolumeSlider.x + sfxVolumeSlider.width + 10, sfxVolumeSlider.y, 20, BLACK);

    // Botón volver
    DrawRectangleRec(backButton, mouseOverBackButton ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(backButton, 2, BLACK);
    const char* backText = "VOLVER";
    int backWidth = MeasureText(backText, 20);
    DrawText(backText, backButton.x + (backButton.width - backWidth) / 2, backButton.y + 15, 20, BLACK);

    EndDrawing();
}

OptionsState::Language OptionsState::getLanguage() 
{
    return currentLanguage;
}