#pragma once

#include <Entity.hpp>

class Player : public Entity {
public:
    Player();
    Player(float x, float y, ElementType t);
    
    void update(float deltaTime) override;
    void render() override;
    
    // Métodos de control
    void walk(int direction); // -1 = izquierda, 0 = parar, 1 = derecha
    void jump();
    
private:
    static constexpr float WALK_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = 300.0f;
};