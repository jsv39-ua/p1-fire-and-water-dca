#pragma once

#include <Entity.hpp>
#include <string>

class Enemy : public Entity {
public:
    float patrolRange;
    float originalX;
    bool movingRight;
    // Vertical movement support
    bool verticalMovement;
    float originalY;
    bool movingDown;
    
    // Textura y frame del enemigo
    Texture2D texture;
    Rectangle sourceRect;
    bool hasTexture;
    
    // Sistema de vida e invencibilidad
    int life_count;
    float invincibility_timer;

    Enemy();
    Enemy(Rectangle bounds, ElementType type, float range = 50.0f, bool vertical = false);
    // Constructor con textura
    Enemy(Rectangle bounds, ElementType type, float range, Texture2D tex, Rectangle src, bool vertical = false);
    
    void update(float deltaTime) override;
    void render() override;
    
    // Sistema de vida
    int getLifeCount() const { return life_count; }
    void updateLifeCount(int change);
    bool isInvincible() const { return invincibility_timer > 0.0f; }
    void activateInvincibility() { invincibility_timer = INVINCIBILITY_DURATION; }
    bool shouldRenderVisible() const;

    // Trigger system
    std::string targetTag;
    
private:
    static constexpr float INVINCIBILITY_DURATION = 1.0f; // 1 segundo de invencibilidad
    static constexpr float BLINK_INTERVAL = 0.1f; // Parpadeo cada 0.1 segundos
};