#pragma once

#include <Entity.hpp>
#include <Platform.hpp>

class Player : public Entity {
public:
    Player();
    Player(float x, float y, float height, float width, ElementType t);
    
    void update(float deltaTime) override;
    void render() override;
    void handleInput(bool isOnGround);
    
    // Configurar teclas
    void setKeys(int left, int right, int jump);
    
    // Getters para las teclas
    int getKeyLeft() const { return key_left; }
    int getKeyRight() const { return key_right; }
    int getKeyJump() const { return key_jump; }
    
    // Métodos de control
    void walk(int direction);
    void jump();
    int checkPlatformCollision(const Platform & platform) const;
    bool resolvePlatformCollision(const Platform & platform);
    
private:
    static constexpr float WALK_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = 500.0f;
    
    // Teclas configurables
    int key_left;
    int key_right;
    int key_jump;
};