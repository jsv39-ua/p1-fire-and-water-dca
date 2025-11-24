#pragma once

#include <GameState.hpp>
#include <ElementType.hpp>
#include <Player.hpp>
#include <Enemy.hpp>
#include <Platform.hpp>
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

    // Getters estáticos para acceder a los jugadores desde cualquier parte
    static Player& getStaticFirePlayer() { return static_fire_player; }
    static Player& getStaticWaterPlayer() { return static_water_player; }

    Player& getFirePlayer() { return fire_player; }
    Player& getWaterPlayer() { return water_player; }

private:
    char entered_key;
    Player fire_player;
    Player water_player;
    
    // Players estáticos globales para opciones
    static Player static_fire_player;
    static Player static_water_player;
    
    std::vector<Platform> level_platforms;
    bool fire_on_ground = false;    
    bool water_on_ground = false;
    std::vector<Enemy> level_enemies;

    MapLoader mapLoader;
    Camera2D camera;
};