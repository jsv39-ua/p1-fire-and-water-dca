#include <StateMachine.hpp>
#include <MainGameState.hpp>
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
    SetTargetFPS(60);

    float delta_time = 0.0f;

    StateMachine state_machine = StateMachine();
    state_machine.add_state(std::make_unique<MainGameState>(), false);

    // Bucle principal del juego proporcionado por Raylib
    while (!WindowShouldClose()) // Este bucle se ejecuta hasta que el usuario cierra la ventana
    {
        delta_time = GetFrameTime(); // Obtiene el tiempo transcurrido desde el último fotograma
        
        state_machine.handle_state_changes(delta_time);
        if (state_machine.getCurrentState() != nullptr) {
            state_machine.getCurrentState()->handleInput();
            state_machine.getCurrentState()->update(delta_time);
            state_machine.getCurrentState()->render();
        }
    }

    // Des-inicializa la ventana y los recursos
    CloseWindow();

    return 0;
}