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

    // Función para actualizar las teclas
    void setFirePlayerKeys(int left, int right, int jump);
    void setWaterPlayerKeys(int left, int right, int jump);

private:
    char entered_key;
    Player fire_player;
    Player water_player;
    std::vector<Platform> level_platforms;
    bool fire_on_ground = false;
    bool water_on_ground = false;
    std::vector<Enemy> level_enemies;

    MapLoader mapLoader;
    Camera2D camera;
};