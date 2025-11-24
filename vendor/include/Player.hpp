#pragma once

#include <Entity.hpp>
#include <Platform.hpp>

class Player : public Entity {
public:
    Player();
    Player(float x, float y, ElementType t);
    
    void update(float deltaTime) override;
    void render() override;
    void handleInput(bool isOnGround); // Nuevo método

    // Configurar teclas
    void setKeys(int left, int right, int jump);
    
    // Métodos de control
    void walk(int direction); // -1 = izquierda, 0 = parar, 1 = derecha
    void jump();
    int checkPlatformCollision(const Platform & platform) const;
    bool resolvePlatformCollision(const Platform & platform); // Resuelve la física de colisión, retorna true si está en el suelo
    
private:
    static constexpr float WALK_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = 500.0f;

    // Teclas configurables
    int key_left;
    int key_right;
    int key_jump;
};