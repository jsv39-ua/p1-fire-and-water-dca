#include <StateMachine.hpp>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>
#include <AudioManager.hpp>
#include <memory>
#include <chrono>

extern "C" {
    #include <raylib.h>
}

int main()
{
    // Initialize the game window
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "Fuego y Agua");
    SetExitKey(KEY_NULL); // Deshabilitar que Escape cierre la ventana
    SetTargetFPS(60);

    // Inicializar el dispositivo de audio, sin esto no se pueden reproducir sonidos
    InitAudioDevice();
    SetMasterVolume(0.5f); // Ajustar el volumen maestro al 50%

    // Inicializar el AudioManager para sonidos globales
    AudioManager::getInstance()->init();

    Music backgroundMusic = LoadMusicStream("assets/music/backgroundMusic.mp3");
    PlayMusicStream(backgroundMusic); // Iniciar la reproducción de la música
    backgroundMusic.looping = true; // Hacer que la música se repita en bucle

    float delta_time = 0.0f;

    StateMachine state_machine = StateMachine();
    state_machine.add_state(std::make_unique<MainMenuState>(), false);

    // Bucle principal del juego proporcionado por Raylib
    while (!WindowShouldClose() && !state_machine.is_game_ending()) // Este bucle se ejecuta hasta que el usuario cierra la ventana
    {
        UpdateMusicStream(backgroundMusic); // Reproduce la música de fondo
        delta_time = GetFrameTime(); // Obtiene el tiempo transcurrido desde el último fotograma
        
        state_machine.handle_state_changes(delta_time);
        if (state_machine.getCurrentState() != nullptr) {
            state_machine.getCurrentState()->handleInput();
            state_machine.getCurrentState()->update(delta_time);
            state_machine.getCurrentState()->render();
        }
    }

    UnloadMusicStream(backgroundMusic); // Liberar la memoria de la música
    AudioManager::cleanup(); // Limpiar el AudioManager
    CloseAudioDevice();

    // Des-inicializa la ventana y los recursos
    CloseWindow();

    return 0;
}