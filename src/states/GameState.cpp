#include <GameState.hpp>

#include <GameState.hpp>

extern "C" {
    #include <raylib.h>
}

GameState::GameState()
{
}

void GameState::renderGlobalUI()
{
    // Dibuja elementos globales que deben aparecer en todos los estados
    DrawFPS(10, 10);
}