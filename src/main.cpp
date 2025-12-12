#include <StateMachine.hpp>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>
#include <AudioManager.hpp>
#include <ResourceManager.hpp>
#include <memory>
#include <chrono>
#include "LocalizationManager.hpp"

extern "C" {
    #include <raylib.h>
}

int main()
{
    // Initialize the game window
    const int screenWidth = 1280;  // Más grande (era 800)
    const int screenHeight = 720;  // Más grande (era 450)

    LocalizationManager::getInstance()->init();
    LocalizationManager::getInstance()->setLanguage("en");
    
    InitWindow(screenWidth, screenHeight, "Fuego y Agua");
    SetExitKey(KEY_NULL); // Deshabilitar que Escape cierre la ventana
    SetTargetFPS(60);

    // Inicializar el dispositivo de audio, sin esto no se pueden reproducir sonidos
    InitAudioDevice();
    SetMasterVolume(0.5f); // Ajustar el volumen maestro al 50%

    // Inicializar gestores Singleton
    AudioManager::getInstance()->init();
    // resourcemanager se inicializa automaticamente al llamar a getInstance()
    float delta_time = 0.0f;

    StateMachine state_machine = StateMachine();
    state_machine.add_state(std::make_unique<MainMenuState>(), false);

    // Bucle principal del juego proporcionado por Raylib
    while (!WindowShouldClose() && !state_machine.is_game_ending()) // Este bucle se ejecuta hasta que el usuario cierra la ventana
    {
        AudioManager::getInstance()->updateMusicStream(); // Reproduce la música de fondo
        delta_time = GetFrameTime(); // Obtiene el tiempo transcurrido desde el último fotograma
        
        state_machine.handle_state_changes(delta_time);
        if (state_machine.getCurrentState() != nullptr) {
            state_machine.getCurrentState()->handleInput();
            state_machine.getCurrentState()->update(delta_time);
            state_machine.getCurrentState()->render();
        }
        
    }

    // Limpiar gestores Singleton
    AudioManager::cleanup();
    ResourceManager::cleanup();
    
    CloseAudioDevice();

    // Des-inicializa la ventana y los recursos
    CloseWindow();

    return 0;
}