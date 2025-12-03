#pragma once

#include <GameState.hpp>
#include <ElementType.hpp>
#include <Player.hpp>
#include <Enemy.hpp>
#include <Platform.hpp>
#include <Animation.hpp>
#include <Camera.hpp>
#include <Projectile.hpp>
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
    std::vector<Projectile> projectiles;
    std::vector<MapTransition> level_transitions;

    void loadLevel(const std::string& mapPath);
    
    // Métodos auxiliares para update()
    void handleTransitions();
    void updateCamera(float deltaTime);
    void handleMapBoundaries();
    void checkFallDeath();

    // Texturas de personajes
    Texture2D character_textures[2];
    
    // Animaciones para los personajes [0] = fire, [1] = water
    Animation idle_animations[2];
    Animation walk_animations[2];

    MapLoader mapLoader;
    GameCamera camera;
    
    // Timer para muerte del jugador
    bool playerDying = false;
    float deathTimer = 0.0f;
    static constexpr float DEATH_DELAY = 1.5f; // 1.5 segundos antes de Game Over
    static constexpr float PLAYER_SPAWN_OFFSET = 40.0f;
    static constexpr float FALL_DEATH_THRESHOLD = 200.0f;
    static constexpr const char* LEVEL_BASE_PATH = "assets/levels/niveles/nivel-tutorial/";
};