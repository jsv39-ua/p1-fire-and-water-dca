#pragma once

#include <entities/Entity.hpp>
#include <entities/Platform.hpp>
#include <systems/SoundPlayer.hpp>

class Player : public Entity {
public:
    // Sonidos para los personajes (salto, daño, caminar)

    SoundPlayer jumpSound;
    SoundPlayer damageSound;
    SoundPlayer deathSound;
    Music walkMusic;
    bool isWalkMusicPlaying;

    Player();
    Player(float x, float y, float height, float width, ElementType t);
    
    void update(float deltaTime) override;
    void render() override;
    
    // Configurar teclas
    void setKeys(int left, int right, int jump, int attack);
    
    // Getters para las teclas
    int getKeyLeft() const { return key_left; }
    int getKeyRight() const { return key_right; }
    int getKeyJump() const { return key_jump; }
    int getKeyAttack() const { return key_attack; }
    int getLifeCount() const { return life_count; }
    void setLifeCount(int lives) { life_count = lives; }
    
    // Sistema de invencibilidad
    bool isInvincible() const { return invincibility_timer > 0.0f; }
    void activateInvincibility() { invincibility_timer = INVINCIBILITY_DURATION; }
    bool shouldRenderVisible() const; // Para el efecto de parpadeo
    
    // Métodos de control
    void walk(int direction); // -1 = izquierda, 0 = parar, 1 = derecha
    void jump();
    void attack(); // Realizar ataque
    bool canAttack() const { return attack_cooldown <= 0.0f; }
    void updateJump(float deltaTime, bool jumpKeyHeld); // Actualizar salto variable
    void updateLifeCount(int change); // Cambiar vida por 'change' unidades
    int checkPlatformCollision(const Platform & platform) const;
    bool resolvePlatformCollision(const Platform & platform); // Resuelve la física de colisión, retorna true si está en el suelo
    
private:
    static constexpr float WALK_SPEED = 200.0f;
    static constexpr float JUMP_FORCE = 270.0f;
    static constexpr float JUMP_BOOST = 600.0f; 
    static constexpr float MAX_JUMP_HOLD_TIME = 0.25f;
    static constexpr float INVINCIBILITY_DURATION = 1.0f; // 1 segundo de invencibilidad
    static constexpr float BLINK_INTERVAL = 0.1f; // Parpadeo cada 0.1 segundos
    static constexpr float ATTACK_COOLDOWN = 0.5f; // Medio segundo entre ataques
    
    // Teclas configurables
    int key_left;
    int key_right;
    int key_jump;
    int key_attack;
    int life_count;
    
    // Timer para salto variable
    float jump_hold_timer;
    
    // Sistema de invencibilidad
    float invincibility_timer;
    
    // Sistema de ataque
    float attack_cooldown;

    // Esto es si queremos que el jugador emita logs periódicos (para debug)
    float log_timer;
    static constexpr float LOG_INTERVAL = 1.0f;
};