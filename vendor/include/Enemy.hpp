#pragma once

#include <Entity.hpp>

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

    Enemy();
    Enemy(Rectangle bounds, ElementType type, float range = 50.0f, bool vertical = false);
    // Constructor con textura
    Enemy(Rectangle bounds, ElementType type, float range, Texture2D tex, Rectangle src, bool vertical = false);
    
    void update(float deltaTime) override;
    void render() override;
};