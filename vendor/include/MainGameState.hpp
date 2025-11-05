#pragma once

#include <GameState.hpp>
#include <ElementType.hpp>
#include <Player.hpp>
#include <Platform.hpp>
#include <vector>
#include <string>

extern "C" {
    #include <raylib.h>
}

struct Hazard {
    Rectangle bounds;
    ElementType type;
    
    Hazard(Rectangle b, ElementType t) : bounds(b), type(t) {}
};

struct Obstacle : public Hazard {
    Obstacle(Rectangle bounds, ElementType type) : Hazard(bounds, type) {}
};

struct Enemy : public Hazard {
    float velocity;
    float patrolRange;
    float originalX;
    bool movingRight;
    
    Enemy(Rectangle bounds, ElementType type, float range = 50.0f) 
        : Hazard(bounds, type), patrolRange(range), originalX(bounds.x), movingRight(true) {
        velocity = 30.0f; // Movimiento lento
    }
};

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
    Player fire_player;
    Player water_player;
    std::vector<Platform> level_platforms;
    // Variables para controlar si los jugadores están en el suelo
    bool fire_on_ground = false;
    bool water_on_ground = false;
    // Nueva variable para almacenar los peligros del nivel
    std::vector<Hazard> level_hazards;
    // Enemigos que se mueven
    std::vector<Enemy> level_enemies;
};