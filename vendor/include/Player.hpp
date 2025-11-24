#pragma once

#include <Entity.hpp>
#include <Platform.hpp>

class Player : public Entity {
public:
    Player();
    Player(float x, float y, float height, float width, ElementType t);
    
    void update(float deltaTime) override;
    void render() override;
    
    // Métodos de control
    void walk(int direction); // -1 = izquierda, 0 = parar, 1 = derecha
    void jump();
    int checkPlatformCollision(const Platform & platform) const;
    bool resolvePlatformCollision(const Platform & platform); // Resuelve la física de colisión, retorna true si está en el suelo
    
private:
    static constexpr float WALK_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = 500.0f;
};