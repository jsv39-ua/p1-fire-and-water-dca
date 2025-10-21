#include "OptionsState.hpp"
#include "StateMachine.hpp"
#include <iostream>

extern "C" {
    #include <raylib.h>
}

// Variables estáticas para las opciones
bool OptionsState::dark_theme = false;
float OptionsState::volume = 0.5f;
OptionsState::Language OptionsState::current_language = OptionsState::SPANISH;

OptionsState::OptionsState() : dragging_volume(false)
{
}

const char* getLanguageName(OptionsState::Language lang) {
    switch(lang) {
        case OptionsState::SPANISH: return "ESPAÑOL";
        case OptionsState::ENGLISH: return "ENGLISH (En desarrollo)";
        default: return "ESPAÑOL";
    }
}

void OptionsState::init()
{
    // Configurar las posiciones de los elementos de la UI
    back_button = { 50, 50, 100, 40 };
    language_button = { 200, 150, 300, 50 };
    volume_slider = { 200, 250, 200, 20 };
    volume_handle = { 200 + (volume * 180), 245, 20, 30 };
}

void OptionsState::handleInput()
{
    Vector2 mouse = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, back_button)) {
            this->state_machine->remove_state(false);
        }
        
        if (CheckCollisionPointRec(mouse, language_button)) {
            current_language = (Language)((current_language + 1) % 2);
        }
        
        if (CheckCollisionPointRec(mouse, volume_handle)) {
            dragging_volume = true;
        }
    }
    
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        dragging_volume = false;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->remove_state(false);
    }
}

void OptionsState::update(float deltaTime)
{
    if (dragging_volume) {
        Vector2 mouse = GetMousePosition();
        float x = mouse.x - volume_slider.x;
        if (x < 0) x = 0;
        if (x > volume_slider.width - volume_handle.width) x = volume_slider.width - volume_handle.width;
        volume = x / (volume_slider.width - volume_handle.width);
        updateVolumeSlider();
    }
}

void OptionsState::updateVolumeSlider()
{
    volume_handle.x = volume_slider.x + (volume * (volume_slider.width - volume_handle.width));
}

// Funciones estáticas para acceder a las opciones desde otros estados

void OptionsState::render()
{
    BeginDrawing();

    
    ClearBackground(SKYBLUE);
    
    // Título
    DrawText("OPCIONES", 300, 50, 40, DARKGRAY);
    
    Vector2 mouse = GetMousePosition();
    
    // Botón volver
    bool over_back = CheckCollisionPointRec(mouse, back_button);
    DrawRectangleRec(back_button, over_back ? GRAY : LIGHTGRAY);
    DrawRectangleLinesEx(back_button, 2, DARKGRAY);
    DrawText("VOLVER", 65, 60, 20, DARKGRAY);
    
    // Botón idioma
    bool over_lang = CheckCollisionPointRec(mouse, language_button);
    DrawRectangleRec(language_button, over_lang ? GRAY : LIGHTGRAY);
    DrawRectangleLinesEx(language_button, 2, DARKGRAY);
    const char* lang = getLanguageName(current_language);
    DrawText(lang, 240, 165, 20, DARKGRAY);
    
    // Volumen
    DrawText("VOLUMEN", 200, 220, 20, DARKGRAY);
    
    // Slider
    DrawRectangleRec(volume_slider, LIGHTGRAY);
    DrawRectangleLinesEx(volume_slider, 2, DARKGRAY);
    
    Rectangle fill = { volume_slider.x, volume_slider.y, volume * volume_slider.width, volume_slider.height };
    DrawRectangleRec(fill, DARKBLUE);
    
    Color handle_col = dragging_volume ? RED : GRAY;
    DrawRectangleRec(volume_handle, handle_col);
    DrawRectangleLinesEx(volume_handle, 2, DARKGRAY);
    
    char vol_text[16];
    sprintf(vol_text, "%.0f%%", volume * 100);
    DrawText(vol_text, 420, 320, 20, DARKGRAY);
    
    // Ayuda
    DrawText("WASD/Flechas: Navegar", 250, 400, 16, DARKGRAY);
    DrawText("ENTER: Seleccionar", 270, 425, 16, DARKGRAY);
    DrawText("ESC: Volver", 305, 450, 16, DARKGRAY);
    
    EndDrawing();
}

// Funciones estáticas para acceder a las opciones desde otros estados
bool OptionsState::getDarkTheme() 
{
    return OptionsState::dark_theme;
}

float OptionsState::getVolume() 
{
    return OptionsState::volume;
}

OptionsState::Language OptionsState::getLanguage() 
{
    return OptionsState::current_language;
}