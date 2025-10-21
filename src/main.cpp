#include <StateMachine.hpp>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>
#include <memory>
#include <chrono>

extern "C" {
    #include <raylib.h>
}

// Variable global para controlar la música
bool g_musicEnabled = true;
Music g_backgroundMusic;

int main()
{
    // Initialize the game window
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "Fuego y Agua");
    SetExitKey(KEY_NULL); // Deshabilitar que Escape cierre la ventana
    SetTargetFPS(60);

    // Inicializar sistema de audio y cargar música
    InitAudioDevice();
    g_backgroundMusic = LoadMusicStream("assets/music/background_music.wav");
    g_backgroundMusic.looping = true;
    
    // Reproducir música si está habilitada
    if (g_musicEnabled) {
        PlayMusicStream(g_backgroundMusic);
    }

    float delta_time = 0.0f;

    StateMachine state_machine = StateMachine();
    state_machine.add_state(std::make_unique<MainMenuState>(), false);

    // Bucle principal del juego proporcionado por Raylib
    while (!WindowShouldClose() && !state_machine.is_game_ending()) // Este bucle se ejecuta hasta que el usuario cierra la ventana
    {
        delta_time = GetFrameTime(); // Obtiene el tiempo transcurrido desde el último fotograma
        
        // Actualizar la música
        UpdateMusicStream(g_backgroundMusic);
        
        // Controlar reproducción según el estado de musicEnabled
        if (g_musicEnabled && !IsMusicStreamPlaying(g_backgroundMusic)) {
            PlayMusicStream(g_backgroundMusic);
        } else if (!g_musicEnabled && IsMusicStreamPlaying(g_backgroundMusic)) {
            StopMusicStream(g_backgroundMusic);
        }
        
        state_machine.handle_state_changes(delta_time);
        if (state_machine.getCurrentState() != nullptr) {
            state_machine.getCurrentState()->handleInput();
            state_machine.getCurrentState()->update(delta_time);
            state_machine.getCurrentState()->render();
        }
    }

    // Des-inicializa la música y el sistema de audio
    UnloadMusicStream(g_backgroundMusic);
    CloseAudioDevice();
    
    // Des-inicializa la ventana y los recursos
    CloseWindow();

    return 0;
}