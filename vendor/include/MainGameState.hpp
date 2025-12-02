#pragma once

#include <GameState.hpp>
#include <ElementType.hpp>
#include <Player.hpp>
#include <Enemy.hpp>
#include <Platform.hpp>
#include <Animation.hpp>
#include <vector>
#include <string>
#include <MapLoader.hpp>

extern "C" {
    #include <raylib.h>
}

class MainGameState : public GameState
{
public:
    MainGameState();
    ~MainGameState() = default;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;

    void pause(){};
    void resume(){};

private:
    char entered_key;
    
    // Usar array para los dos jugadores: [0] = FIRE, [1] = WATER
    Player players[2];
    
    std::vector<Platform> level_platforms;
    
    // Flags de suelo para cada jugador
    bool on_ground[2] = {false, false}; // [0] = fire, [1] = water
    
    std::vector<Enemy> level_enemies;
    std::vector<MapTransition> level_transitions;

    void loadLevel(const std::string& mapPath);

    // Texturas de personajes
    Texture2D character_textures[2];
    
    // Animaciones para los personajes [0] = fire, [1] = water
    Animation idle_animations[2];
    Animation walk_animations[2];

    MapLoader mapLoader;
    Camera2D camera;
    
    // Timer para muerte del jugador
    bool playerDying = false;
    float deathTimer = 0.0f;
    static constexpr float DEATH_DELAY = 1.5f; // 1.5 segundos antes de Game Over
};