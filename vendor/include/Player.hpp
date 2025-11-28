#pragma once

#include <Entity.hpp>
#include <Platform.hpp>

class Player : public Entity {
public:
    // Sonidos para los personajes (salto, daño, caminar)

    Sound jumpSound;
    Sound damageSound;
    Sound walkSound;

    Player();
    Player(float x, float y, float height, float width, ElementType t);
    
    void update(float deltaTime) override;
    void render() override;
    
    // Configurar teclas
    void setKeys(int left, int right, int jump);
    
    // Getters para las teclas
    int getKeyLeft() const { return key_left; }
    int getKeyRight() const { return key_right; }
    int getKeyJump() const { return key_jump; }
    
    // Métodos de control
    void walk(int direction); // -1 = izquierda, 0 = parar, 1 = derecha
    void jump();
    void updateJump(float deltaTime, bool jumpKeyHeld); // Actualizar salto variable
    int checkPlatformCollision(const Platform & platform) const;
    bool resolvePlatformCollision(const Platform & platform); // Resuelve la física de colisión, retorna true si está en el suelo
    
private:
    static constexpr float WALK_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = 270.0f;
    static constexpr float JUMP_BOOST = 600.0f; 
    static constexpr float MAX_JUMP_HOLD_TIME = 0.25f; 
    
    // Teclas configurables
    int key_left;
    int key_right;
    int key_jump;
    
    // Timer para salto variable
    float jump_hold_timer;

    
};

// Declaración de variables globales (extern)
extern Player fire_player;
extern Player water_player;