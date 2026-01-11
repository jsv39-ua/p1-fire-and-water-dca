#pragma once

#include <states/GameState.hpp>
#include <components/ElementType.hpp>
#include <entities/Player.hpp>
#include <entities/Enemy.hpp>
#include <entities/Platform.hpp>
#include <components/Animation.hpp>
#include <components/Camera.hpp>
#include <entities/Projectile.hpp>
#include <vector>
#include <string>
#include <systems/MapLoader.hpp>

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
    
    // Flags para indicar si un jugador está esperando en una transición
    bool waitingForTransition[2] = {false, false};
    
    std::vector<Enemy> level_enemies;
    std::vector<Projectile> projectiles;
    std::vector<MapTransition> level_transitions;

    // Cargar nivel. Si useMapSpawn es true, posiciona los jugadores en el spawn del mapa
    void loadLevel(const std::string& mapPath, bool useMapSpawn = false);
    
    // Métodos auxiliares para update()
    void handleTransitions();
    void updateCamera(float deltaTime);
    void handleMapBoundaries();
    void checkFallDeath();
    
    // Resolver path de mapa destino
    std::string resolveMapPath(const std::string& targetMap);

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
    
    // Texto del cartel activo
    std::string activeSignText = "";
    
    static constexpr float DEATH_DELAY = 1.5f; // 1.5 segundos antes de Game Over
    static constexpr float PLAYER_SPAWN_OFFSET = 40.0f;
    static constexpr float FALL_DEATH_THRESHOLD = 200.0f;
    static constexpr const char* LEVELS_BASE_PATH = "assets/levels/niveles/";
    
    // Path del nivel actual (carpeta contenedora)
    std::string currentLevelPath;
    
    // Path de la zona actual (archivo .tmx completo) - para LevelStateManager
    std::string currentZonePath;
};