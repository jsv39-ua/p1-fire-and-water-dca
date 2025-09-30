#include <GameOverState.hpp>
#include <MainGameState.hpp>
#include <StateMachine.hpp>
#include <memory>

extern "C" {
    #include <raylib.h>
}

GameOverState::GameOverState() {}

void GameOverState::init() {
    // No se necesita inicialización para esta pantalla
}

void GameOverState::handleInput() {
    // Si se presiona la tecla de espacio, reinicia el juego
    if (IsKeyPressed(KEY_SPACE)) {
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }
}

void GameOverState::update(float deltaTime) {
    // No se necesita actualizar nada
}

void GameOverState::render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    const char* message = "Game Over";
    int textWidth = MeasureText(message, 30);
    int x = (GetScreenWidth() - textWidth) / 2;
    int y = (GetScreenHeight() / 2) - 15;

    DrawText(message, x, y, 30, BLACK);

    EndDrawing();
}